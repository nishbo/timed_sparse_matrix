#include "tsm.h"

using namespace std;
using namespace TSM;

//------------------------------------------- COMMA_SEPARATED_VIEW
TsmCommaSeparatedView::TsmCommaSeparatedView(std::string str, char separator) {
	// TODO check for str len 0
	m_str = str;
	m_data.clear();

	size_t pos = 0;
	m_data.emplace_back(pos - 1);
	while ((pos = m_str.find(separator, pos)) != std::string::npos) {
		m_data.emplace_back(pos);
		++pos;
	}
	// This checks for a trailing comma with no data after it.
	pos = m_str.size();
	m_data.emplace_back(pos);
}

std::string_view TsmCommaSeparatedView::operator[](std::size_t index) const
{
	return std::string_view(&m_str[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));

}

std::size_t TsmCommaSeparatedView::size() const
{
	return m_data.size() - 1;
}

//------------------------------------------- TsmLine
TsmLine::TsmLine(size_t _num, TsmHeaderVariables& shv) : line("", ';')
{
}

TsmLine::TsmLine(std::string str, TsmHeaderVariables& shv) : line(str, ';')
{
	switch (shv.filetype)
	{
	case TSM_FILETYPE::STAMPS:
		time = atof(string(line[0]).c_str());
		// num is unset
		break;
	case TSM_FILETYPE::PERIOD:
		num = atoi(string(line[0]).c_str());
		// no need for time - is set outside
		// time = shv.time_start + shv.time_period * num;
		break;
	default:
		// not implemented
		// TODO meaningful throw
		throw(exception());
		break;
	}
}

TsmCommaSeparatedView TsmLine::operator[](std::size_t index) const
{
	return TsmCommaSeparatedView(string(line[index+1]));
}

std::size_t TsmLine::size() const
{
	return line.size() - 1;
}

//------------------------------------------- Tsm class
size_t Tsm::pos2ind(const std::vector<size_t>& pos)
{
	// assumes same length of vectors
	size_t answ = 0;
	for (size_t i_dim = 0; i_dim < pos.size(); i_dim++)
		answ += dim_mult[i_dim] * pos[i_dim];
	return answ;
}

void Tsm::calc_params()
{
	dim_mult.clear();
	dim_mult.resize(dims.size());
	M = 1;
	for (int i_dim = dims.size() - 1; i_dim >= 0; i_dim--) {
		dim_mult[i_dim] = M;
		M *= dims[i_dim];
	}
}

double TSM::Tsm::get(const std::size_t i_time, const std::vector<std::size_t> pos)
{
	return data[i_time * M + pos2ind(pos)];
}

double TSM::Tsm::set(const std::size_t i_time, const std::vector<std::size_t> pos, const double value)
{
	return data[i_time * M + pos2ind(pos)] = value;
}

Tsm::Tsm(const std::string filename)
{
	int answ = load(filename);
	// TODO also set state
	if (answ < 0)
		throw exception();
}

void Tsm::print()
{
	for (size_t i_time = 0; i_time < time.size(); i_time++) {
		cout << to_string(time[i_time]) << ":";
		for (size_t i_el = 0; i_el < M; i_el++)
			cout << " " << to_string(data[i_time * M + i_el]);
		cout << endl;
	}
}

int TSM::Tsm::save_stamps(const std::string filename, const double default_value)
{
	// TODO create header
	// TODO output lines
	return 0;
}

std::vector<std::vector<double>> TSM::Tsm::get_vectors()
{
	// check
	if (dims.size() != 1)
		throw exception(); // TODO meaning

	// fill
	std::vector<std::vector<double>> answ;
	for (size_t i_time = 0; i_time < time.size(); i_time++)
		answ.push_back(std::vector<double>(
			data.begin() + i_time * M, 
			data.begin() + (i_time + 1) * M));
	
	return answ;
}

std::vector<std::vector<std::vector<double>>> TSM::Tsm::get_matrices()
{
	// check
	if (dims.size() != 2)
		throw exception(); // TODO meaning

	// fill
	std::vector<std::vector<std::vector<double>>> answ(N);
	size_t begin, end;
	for (size_t i_time = 0; i_time < time.size(); i_time++) {
		for (size_t i = 0; i < dims[0]; i++) {
			begin = i_time * M + i * dims[1];
			end = i_time * M + (i + 1) * dims[1];
			answ[i_time].push_back(std::vector<double>(data.begin() + begin, data.begin() + end));
		}
	}
	return answ;
}

int Tsm::load(const std::string filename)
{
	// set to null
	time.clear();
	data.clear();
	dims.clear();

	// open file
	std::ifstream infile(filename);
	if (!infile.is_open())
		return -1;
	cout << "Opened file." << endl;

	// process header
	TsmHeaderVariables shv;
	if (Tsm::process_header(infile, shv) < 0)
		return -2;
	dims = shv.dims;
	int numdim = (int) dims.size();
	cout << "Processed header." << endl;

	// tensor-associated parameters
	calc_params();
	cout << "Calculated dimensions." << endl;

	// load the rest of the file into a buffer to estimate the size for allocation
	string line;
	vector<TsmLine> file_line_buffer;
	while (getline(infile, line))
		if (!line.empty())
			file_line_buffer.push_back(TsmLine(line, shv));
	cout << "Buffered file info." << endl;

	// figure out the number of time points
	// depending on type and N
	switch (shv.filetype)
	{
	case TSM_FILETYPE::STAMPS:
		shv.N = file_line_buffer.size();
		break;
	case TSM_FILETYPE::PERIOD:
		// assumes the lines are ordered
		// specify in docs
		shv.N = max(shv.N, file_line_buffer.back().num+1);
		break;
	default:
		return -3;
		break;
	}

	// handle empty file
	N = shv.N;
	if (N == 0)
		return 1;  // empty

	// allocate
	time.resize(N);
	data.resize(N * M, shv.default_value);
	cout << "Allocated memory." << endl;

	// transfer from buffer
	vector<size_t> pos(numdim);
	switch (shv.filetype)
	{
	case TSM_FILETYPE::STAMPS:
		for (size_t i_time = 0; i_time < N; i_time++) {
			time[i_time] = file_line_buffer[i_time].time;
			for (size_t j = 0; j < file_line_buffer[i_time].size(); j++)
			{
				TsmCommaSeparatedView csv = file_line_buffer[i_time][j];
				for (size_t i_dim = 0; i_dim < numdim; i_dim++)
					pos[i_dim] = atoi(string(csv[i_dim]).c_str());
				set(i_time, pos, atof(string(csv[numdim]).c_str()));
			}
		}
		break;

	case TSM_FILETYPE::PERIOD:
		// fill out all times
		for (size_t i = 0; i < N; i++)
			time[i] = shv.time_start + shv.time_period * i;

		// fill the corresponding values
		for (TsmLine& flb : file_line_buffer) {
			for (size_t j = 0; j < flb.size(); j++)
			{
				TsmCommaSeparatedView csv = flb[j];
				for (size_t k = 0; k < numdim; k++)
					pos[k] = atoi(string(csv[k]).c_str());
				set(flb.num, pos, atof(string(csv[numdim]).c_str()));
			}
		}
		break;
	}
	cout << "Transferred from buffer." << endl;

	return 0;
}

int Tsm::process_header(ifstream& infile, TsmHeaderVariables& shv)
{
	string line;
	string var_name;
	string buf;
	string::size_type eq_pos;
	while (getline(infile, line)) {
		// end of header
		if (line == "----")
			break;

		// find the name of the varibale
		eq_pos = line.find("=");
		if (eq_pos == 0 || eq_pos == string::npos) {
			cout << "Could not process header line " << line << endl;
			continue;
		}

		if (shv.set_var(line.substr(0, eq_pos), line.substr(eq_pos + 1)) < 0) {
			cout << "Error during header processing." << endl;
			return -1;
		}
	}

	// TODO check if all necessary fields have been found
	if (!shv.is_valid()) {
		cout << "Loaded header is invalid." << endl;
		return -2;
	}

	return 0;
}

//-------------------------- TsmHeaderVariables
TsmHeaderVariables::~TsmHeaderVariables()
{
}

bool TsmHeaderVariables::is_valid()
{
	if (dims.empty()) {
		cout << "Header dimensions variables are unset." << endl;
		return false;
	}

	if (filetype == TSM_FILETYPE::NONETYPE) {
		cout << "Header filetype (time) variable is unset." << endl;
		return false;
	}

	// these produce weird, but technically working results
	//if (filetype == TSM_FILETYPE::PERIOD) {
	//	//if (N < 0) {
	//	//	cout << "Header N variable is negative." << endl;
	//	//	return false;
	//	//}
	//	//if (time_period == 0) {
	//	//	cout << "Header time_period variable is zero." << endl;
	//	//	return false;
	//	//}
	//}

	return true;
}

int TsmHeaderVariables::set_var(std::string var_name, std::string str)
{
	if (auto search = m_setter_map.find(var_name); search != m_setter_map.end()) {
		// found
		int answ = (this->*(search->second))(str);
		if (answ < 0)
			cout << "Errors encountered processing header variable " << search->first << endl;
		return answ;
	}
	else {
		cout << "Unknown header variable " << var_name << endl;
		return 1;
	}
}

int TsmHeaderVariables::set_dims(std::string str)
{
	TsmCommaSeparatedView csv(str);
	dims.clear();
	if (csv.size() == 0) {
		return -1;
	}

	for (size_t i_dim = 0; i_dim < csv.size(); i_dim++)
		dims.push_back(atoi(string(csv[i_dim]).c_str()));

	return 0;
}

int TsmHeaderVariables::set_default_value(std::string str)
{
	// TODO shield atof throw
	default_value = atof(str.c_str());
	return 0;
}

int TsmHeaderVariables::set_filetype(std::string str)
{
	if (str == "stamps")
		filetype = TSM_FILETYPE::STAMPS;
	else if (str == "period") 
		filetype = TSM_FILETYPE::PERIOD;
	else 
		return -1;
	return 0;
}

int TsmHeaderVariables::set_N(std::string str)
{
	// TODO shield atoi
	N = atoi(str.c_str());
	return 0;
}

int TsmHeaderVariables::set_time_start(std::string str)
{
	// TODO shield atof
	time_start = atof(str.c_str());
	return 0;
}

int TsmHeaderVariables::set_time_period(std::string str)
{
	// TODO shield atof
	time_period = atof(str.c_str());
	return 0;
}
