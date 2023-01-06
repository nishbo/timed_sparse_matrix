#include "SSM.h"

using namespace std;

//------------------------------------------- COMMA_SEPARATED_VIEW
// used to go through comma-sepearated strings
// a variation on CSVReader
class COMMA_SEPARATED_VIEW
{
public:
	COMMA_SEPARATED_VIEW() = delete;
	COMMA_SEPARATED_VIEW(std::string str, char separator=',');

	std::string_view operator[](std::size_t index) const;
	std::size_t size() const;
private:
	string m_str;
	vector<size_t> m_data;
};

COMMA_SEPARATED_VIEW::COMMA_SEPARATED_VIEW(std::string str, char separator) {
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

std::string_view COMMA_SEPARATED_VIEW::operator[](std::size_t index) const
{
	return std::string_view(&m_str[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));

}

std::size_t COMMA_SEPARATED_VIEW::size() const
{
	return m_data.size() - 1;
}

//------------------------------------------- SSM_LINE
class SSM_LINE
{
public:
	SSM_LINE() = delete;
	SSM_LINE(size_t _num, SSM_HEADER_VARIABLES& shv);
	SSM_LINE(std::string str, SSM_HEADER_VARIABLES& shv);

	size_t num = -1;
	double time = -1;
	COMMA_SEPARATED_VIEW line;

	COMMA_SEPARATED_VIEW operator[](std::size_t index) const;
	std::size_t size() const;
private:

};

SSM_LINE::SSM_LINE(size_t _num, SSM_HEADER_VARIABLES& shv) : line("", ';')
{
}

SSM_LINE::SSM_LINE(std::string str, SSM_HEADER_VARIABLES& shv) : line(str, ';')
{
	switch (shv.filetype)
	{
	case SSM_FILETYPE::STAMPS:
		time = atof(string(line[0]).c_str());
		// num is unset
		break;
	case SSM_FILETYPE::PERIOD:
		num = atoi(string(line[0]).c_str());
		// no need for time - is set outside
		// time = shv.time_start + shv.time_period * num;
		break;
	default:
		// not implemented
		// TODO meaning
		throw(exception());
		break;
	}
}

COMMA_SEPARATED_VIEW SSM_LINE::operator[](std::size_t index) const
{
	return COMMA_SEPARATED_VIEW(string(line[index+1]));
}

std::size_t SSM_LINE::size() const
{
	return line.size() - 1;
}

//------------------------------------------- SSM class
int SSM::load(const string filename, double* time, double* data, size_t& N, int* dims, int& numdim)
{
	// set to null
	time = nullptr;
	data = nullptr;
	dims = nullptr;

	// open file
	std::ifstream infile(filename);
	if (!infile.is_open())
		return -1;

	// process header
	SSM_HEADER_VARIABLES shv;
	if (SSM::process_header(infile, shv) < 0)
		return -2;
	dims = shv.dims;
	numdim = shv.numdim;

	// total number of elements per tensor
	size_t M = 1;
	for (size_t i_dim = 0; i_dim < numdim; i_dim++)
		M *= dims[i_dim];

	// load the rest of the file into a buffer to estimate the size for allocation
	string line;
	vector<SSM_LINE> file_line_buffer;
	while (getline(infile, line)) {
		if (~line.empty())
			file_line_buffer.push_back(SSM_LINE(line, shv));
	}

	// figure out the number of time points
	// depending on type and N
	switch (shv.filetype)
	{
	case SSM_FILETYPE::STAMPS:
		shv.N = file_line_buffer.size();
		break;
	case SSM_FILETYPE::PERIOD:
		// assumes the lines are ordered
		// specify in docs
		shv.N = max(shv.N, file_line_buffer.back().num+1);
		break;
	default:
		// not implemented
		// TODO meaning
		throw(exception());
		break;
	}
	N = shv.N;

	if (N == 0)
		return 1;  // empty

	// allocate
	time = new double(N);
	data = new double(N * M);

	// fill
	switch (shv.filetype)
	{
	case SSM_FILETYPE::STAMPS:
		for (size_t i = 0; i < N; i++)
			time[i] = file_line_buffer[i].time;
		break;
	case SSM_FILETYPE::PERIOD:
		for (size_t i = 0; i < N; i++)
			time[i] = shv.time_start + shv.time_period * i;
		break;
	}

	
	// TODO transfer from buffer

	return 0;
}

int SSM::process_header(ifstream& infile, SSM_HEADER_VARIABLES& shv)
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

//-------------------------- SSM_HEADER_VARIABLES
SSM_HEADER_VARIABLES::~SSM_HEADER_VARIABLES()
{
}

bool SSM_HEADER_VARIABLES::is_valid()
{
	if (!dims || !numdim) {
		cout << "Header dimensions variables are unset." << endl;
		return false;
	}

	if (filetype == SSM_FILETYPE::NONETYPE) {
		cout << "Header filetype (time) variable is unset." << endl;
		return false;
	}

	// these produce weird, but technically working results
	//if (filetype == SSM_FILETYPE::PERIOD) {
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

int SSM_HEADER_VARIABLES::set_var(std::string var_name, std::string str)
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

int SSM_HEADER_VARIABLES::set_dims(std::string str)
{
	COMMA_SEPARATED_VIEW csv(str);
	numdim = (int) csv.size();
	if (numdim == 0) {
		return -1;
	}

	dims = new int[numdim];
	for (size_t i_dim = 0; i_dim < numdim; i_dim++)
		dims[i_dim] = atoi(string(csv[i_dim]).c_str());

	return 0;
}

int SSM_HEADER_VARIABLES::set_default_value(std::string str)
{
	// TODO shield atof throw
	default_value = atof(str.c_str());
	return 0;
}

int SSM_HEADER_VARIABLES::set_filetype(std::string str)
{
	if (str == "stamps")
		filetype = SSM_FILETYPE::STAMPS;
	else if (str == "period") 
		filetype = SSM_FILETYPE::PERIOD;
	else 
		return -1;
	return 0;
}

int SSM_HEADER_VARIABLES::set_N(std::string str)
{
	// TODO shield atoi
	N = atoi(str.c_str());
	return 0;
}

int SSM_HEADER_VARIABLES::set_time_start(std::string str)
{
	// TODO shield atof
	time_start = atof(str.c_str());
	return 0;
}

int SSM_HEADER_VARIABLES::set_time_period(std::string str)
{
	// TODO shield atof
	time_period = atof(str.c_str());
	return 0;
}
