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
size_t Tsm::pos2ind(const int* pos, const size_t* dim_mult, const int numdim)
{
	size_t answ = 0;
	for (int i_dim = 0; i_dim < numdim; i_dim++)
		answ += dim_mult[i_dim] * pos[i_dim];
	return answ;
}

size_t Tsm::calc_matrix_M(const int* dims, const int numdim)
{
	size_t M = 1;
	for (int i_dim = 0; i_dim < numdim; i_dim++)
		M *= dims[i_dim];
	return M;
}

void Tsm::print(const double* time, const double* data, const size_t& N, const int* dims, const int& numdim)
{
	size_t M = calc_matrix_M(dims, numdim);
	for (size_t i = 0; i < N; i++) {
		cout << to_string(time[i]) << ":";

		for (size_t i_dim = 0; i_dim < numdim; i_dim++) {
			for (size_t j = 0; j < dims[i_dim]; j++)
				cout << " " << to_string(data[i * M + j]);
			cout << endl;
		}
		cout << endl;
	}
}

int Tsm::load(const string filename, double* time, double* data, size_t& N, int* dims, int& numdim)
{
	// set to null
	time = nullptr;
	data = nullptr;
	dims = nullptr;

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
	numdim = shv.numdim;
	cout << "Processed header." << endl;

	// total number of elements per tensor
	size_t* dim_mult = new size_t[numdim];
	size_t M = 1;
	for (int i_dim = 0; i_dim < numdim; i_dim++) {
		dim_mult[numdim - 1 - i_dim] = M;
		M *= dims[i_dim];
	}
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
		// not implemented
		// TODO meaningful throw
		throw(exception());
		break;
	}
	N = shv.N;

	if (N == 0)
		return 1;  // empty

	// allocate
	time = new double[N];
	data = new double[N * M];

	// fill
	// sorry if you are fixing overflow here
	for (size_t i = 0; i < M * N; i++)
		data[i] = shv.default_value;
	cout << "Allocated memory." << endl;

	// transfer from buffer
	int* pos = new int[numdim];
	switch (shv.filetype)
	{
	case TSM_FILETYPE::STAMPS:
		for (size_t i = 0; i < N; i++) {
			time[i] = file_line_buffer[i].time;
			for (size_t j = 0; j < file_line_buffer[i].size(); j++)
			{
				TsmCommaSeparatedView csv = file_line_buffer[i][j];
				for (size_t i_dim = 0; i_dim < numdim; i_dim++)
					pos[i_dim] = atoi(string(csv[i_dim]).c_str());

				data[i * M + pos2ind(pos, dim_mult, numdim)] = atof(string(csv[numdim]).c_str());
			}
		}
		break;
	case TSM_FILETYPE::PERIOD:
		for (size_t i = 0; i < N; i++)
			time[i] = shv.time_start + shv.time_period * i;

		for (TsmLine& flb : file_line_buffer) {
			for (size_t j = 0; j < flb.size(); j++)
			{
				TsmCommaSeparatedView csv = flb[j];
				for (size_t k = 0; k < numdim; k++)
					pos[k] = atoi(string(csv[k]).c_str());

				data[flb.num * M + pos2ind(pos, dim_mult, numdim)] = atof(string(csv[numdim]).c_str());
			}
		}

		break;
	}
	cout << "Transferred from buffer." << endl;

	delete [] dim_mult;

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
	if (!dims || !numdim) {
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
	numdim = (int) csv.size();
	if (numdim == 0) {
		return -1;
	}

	dims = new int[numdim];
	for (size_t i_dim = 0; i_dim < numdim; i_dim++)
		dims[i_dim] = atoi(string(csv[i_dim]).c_str());

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
