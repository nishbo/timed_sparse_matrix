#include "tsm.h"

using namespace std;
using namespace TSM;

// @https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp)
{
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
		// unless the result is subnormal
		|| std::fabs(x - y) < std::numeric_limits<T>::min();
}

// @https://stackoverflow.com/questions/38874605/generic-method-for-flattening-2d-vectors
template<typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& orig)
{
	std::vector<T> ret;
	for (const auto& v : orig)
		ret.insert(ret.end(), v.begin(), v.end());
	return ret;
}

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
	// This checks for a trailing separator with no data after it and removes it.
	if (m_data.back() == m_str.size() - 1)
		m_data.pop_back();
	pos = m_str.size();
	m_data.emplace_back(pos);
}

std::string_view TsmCommaSeparatedView::operator[](size_t index) const
{
	return std::string_view(&m_str[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
}

size_t TsmCommaSeparatedView::size() const
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

TsmCommaSeparatedView TsmLine::operator[](size_t index) const
{
	return TsmCommaSeparatedView(string(line[index+1]));
}

size_t TsmLine::size() const
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

std::vector<size_t> TSM::Tsm::ind2pos(const size_t ind)
{
	vector<size_t> answ;
	size_t ind_ = ind;
	size_t dim_pos;
	for (size_t i_dim = 0; i_dim < dim_mult.size(); i_dim++) {
		dim_pos = ind_ / dim_mult[i_dim];
		answ.push_back(dim_pos);
		ind_ -= dim_pos * dim_mult[i_dim];
	}
	return answ;
}

void Tsm::calc_params()
{
	N = time.size();
	dim_mult.clear();
	dim_mult.resize(dims.size());
	M = 1;
	for (int i_dim = dims.size() - 1; i_dim >= 0; i_dim--) {
		dim_mult[i_dim] = M;
		M *= dims[i_dim];
	}
}

std::string TSM::Tsm::pos2str(const std::vector<size_t>& pos)
{
	string answ;
	if (pos.empty())
		return answ;
	answ += to_string(pos[0]);
	for (size_t i_dim = 1; i_dim < pos.size(); i_dim++)
		answ += "," + to_string(pos[i_dim]);
	return answ;
}

std::string TSM::Tsm::d_to_string(double val)
{
	std::ostringstream _oss_buf;
	_oss_buf.clear();
	_oss_buf << val;
	return _oss_buf.str();
}

double TSM::Tsm::get(const size_t i_time, const std::vector<size_t> pos)
{
	return data[i_time * M + pos2ind(pos)];
}

double TSM::Tsm::set(const size_t i_time, const std::vector<size_t> pos, const double value)
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

TSM::Tsm::Tsm(const std::vector<double> time, const std::vector<double> data, const std::vector<size_t> dims)
{
	// TODO validity/consistency checks?
	this->time = time;
	this->data = data;
	this->dims = dims;
	calc_params();
}

TSM::Tsm::Tsm(const std::vector<double> time, const std::vector<std::vector<double>> vecs)
{
	this->time = time;
	this->data = flatten(vecs);
	this->dims = vector<size_t>{ vecs[0].size() };
	calc_params();
}


TSM::Tsm::Tsm(const std::vector<double> time, const std::vector<std::vector<std::vector<double>>> matrices)
{
	this->time = time;
	this->data = flatten(flatten(matrices));
	this->dims = vector<size_t>{ matrices[0].size(), matrices[0][0].size() };
	calc_params();
}

void Tsm::print()
{
	for (size_t i_time = 0; i_time < time.size(); i_time++) {
		cout << d_to_string(time[i_time]) << ":";
		for (size_t i_el = 0; i_el < M; i_el++)
			cout << " " << d_to_string(data[i_time * M + i_el]);
		cout << endl;
	}
}

int TSM::Tsm::save_stamps(const std::string filename, const double default_value)
{
	// create header
	TsmHeaderVariables thv;
	thv.dims = dims;
	thv.filetype = TSM_FILETYPE::STAMPS;
	thv.default_value = default_value;

	string header = thv.get_string();
	header += "----\n";

	// open file
	std::ofstream oufile(filename);
	if (!oufile.is_open())
		return -1;
	// cout << "Opened file." << endl;
	oufile << header;

	// generate data lines
	string data_s;
	// with stamps, we save every time point, since that preserves information
	for (size_t i_time = 0; i_time < time.size(); i_time++)
	{
		data_s += d_to_string(time[i_time]);
		// add the data
		for (size_t i_dat = 0; i_dat < M; i_dat++)
			if (data[i_dat + i_time*M] != default_value) 
				data_s += ";" + pos2str(ind2pos(i_dat)) + 
					"," + d_to_string(data[i_dat + i_time * M]);
		data_s += "\n";
	}
	oufile << data_s;
	oufile.close();

	return 0;
}

int TSM::Tsm::save_period(const std::string filename, const double default_value)
{
	// create header
	TsmHeaderVariables thv;
	thv.dims = dims;
	thv.filetype = TSM_FILETYPE::PERIOD;
	thv.default_value = default_value;
	// TODO add checks for N == 0 and N == 1
	thv.N = N;
	thv.time_start = time[0];
	thv.time_period = (time.back() - time[0]) / (N - 1);

	// check if the time stamps are periodic
	double dt;
	for (size_t i = 1; i < time.size(); i++)
	{
		dt = time[i] - time[i - 1];
		if ((dt < 0) || !almost_equal(dt, thv.time_period, 2)) {
			return -2;  // on one or more time points the dt is not equal to expected
		}
	}

	string header = thv.get_string();
	header += "----\n";

	// open file
	std::ofstream oufile(filename);
	if (!oufile.is_open())
		return -1;
	// TODO add debug option
    //cout << "Opened file." << endl;
	oufile << header;

	// generate data lines
	string data_s;
	string data_s_buf;
	bool line_has_data;
	// with stamps, we save every time point, since that preserves information
	for (size_t i_time = 0; i_time < time.size(); i_time++)
	{
		line_has_data = false;
		data_s_buf = to_string(i_time);
		// add the data
		for (size_t i_dat = 0; i_dat < M; i_dat++)
			if (data[i_dat + i_time * M] != default_value) {
				data_s_buf += ";" + pos2str(ind2pos(i_dat)) +
					"," + d_to_string(data[i_dat + i_time * M]);
				line_has_data = true;
			}
		if (line_has_data)
			data_s += data_s_buf + "\n";
	}
	oufile << data_s;
	oufile.close();

	return 0;
}

int TSM::Tsm::save(const std::string filename, const std::string type, const double default_value)
{
	if (type == "period")
		return save_period(filename, default_value);
	if (type == "stamps")
		return save_stamps(filename, default_value);
	// TODO meaning
	return -10;
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

int TSM::Tsm::get_matrices_ip(std::vector<std::vector<std::vector<double>>>& m)
{
	m.clear();

	// check
	if (dims.size() != 2)
		throw exception(); // TODO meaning

	// fill
	m.resize(N);
	size_t begin, end;
	for (size_t i_time = 0; i_time < time.size(); i_time++) {
		for (size_t i = 0; i < dims[0]; i++) {
			begin = i_time * M + i * dims[1];
			end = i_time * M + (i + 1) * dims[1];
			m[i_time].push_back(std::vector<double>(data.begin() + begin, data.begin() + end));
		}
	}
	return 0;
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
	// cout << "Opened file." << endl;

	// process header
	TsmHeaderVariables shv;
	if (Tsm::process_header(infile, shv) < 0)
		return -2;
	dims = shv.dims;
	int numdim = (int) dims.size();
	// cout << "Processed header." << endl;

	// tensor-associated parameters
	calc_params();
	// cout << "Calculated dimensions." << endl;

	// load the rest of the file into a buffer to estimate the size for allocation
	string line;
	vector<TsmLine> file_line_buffer;
	while (getline(infile, line))
		if (!line.empty())
			file_line_buffer.push_back(TsmLine(line, shv));
	// cout << "Buffered file info." << endl;

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
	// cout << "Allocated memory." << endl;

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
	// cout << "Transferred from buffer." << endl;

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

std::string TSM::TsmHeaderVariables::get_string()
{
	string answ;
	// shared parameters
	answ += "dims=" + to_string(dims[0]);
	for (size_t i_dim = 1; i_dim < dims.size(); i_dim++)
		answ += "," + to_string(dims[i_dim]);
	answ += "\n";
	answ += "default_value=" + Tsm::d_to_string(default_value) + "\n";

	// type-dependent
	switch (filetype)
	{
	case TSM::TSM_FILETYPE::STAMPS:
		answ += "time=stamps\n";
		break;
	case TSM::TSM_FILETYPE::PERIOD:
		answ += "time=period\n";
		answ += "N=" + to_string(N) + "\n";
		answ += "time_start=" + Tsm::d_to_string(time_start) + "\n";
		answ += "time_period=" + Tsm::d_to_string(time_period) + "\n";
		break;
	default:
		// TODO meaning
		throw exception();
		break;
	}

	return answ;
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
