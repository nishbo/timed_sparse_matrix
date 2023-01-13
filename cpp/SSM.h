#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

enum class SSM_FILETYPE
{
	NONETYPE,  // error type
	STAMPS,
	PERIOD
};


class SSM_HEADER_VARIABLES
{
public:
	~SSM_HEADER_VARIABLES();

	int* dims = nullptr;
	int numdim = 0;

	double default_value = 0;  // 0 by default

	SSM_FILETYPE filetype = SSM_FILETYPE::NONETYPE;

	// scans using atoi, so still restricted to integer
	size_t N = -1;

	// for SSM_FILETYPE::PERIOD
	double time_start = 0;  // TODO note in docs that default is 0
	double time_period = 1;  // TODO note in docs that default is 1

	// checks if the header information is enough for matrix construction
	bool is_valid();
	// sets variable
	int set_var(std::string var_name, std::string str);

private:
	int set_dims(std::string);
	int set_default_value(std::string);
	int set_filetype(std::string);
	int set_N(std::string);
	int set_time_start(std::string);
	int set_time_period(std::string);
	std::map<std::string, int(SSM_HEADER_VARIABLES::*)(std::string)> m_setter_map = {
		{"dims",			&SSM_HEADER_VARIABLES::set_dims},
		{"default_value",	&SSM_HEADER_VARIABLES::set_default_value},
		{"time",			&SSM_HEADER_VARIABLES::set_filetype},
		{"N",				&SSM_HEADER_VARIABLES::set_N},
		{"time_start",		&SSM_HEADER_VARIABLES::set_time_start},
		{"time_period",		&SSM_HEADER_VARIABLES::set_time_period},
	};
};


class SSM
{
public:
	SSM() = delete;

	static int load(const std::string filename, double* time, double* data, size_t& N, int* dims, int& numdim);
	static int save(const std::string filename, const double* time, const double* data, const int* dims, const int numdim);

	// TODO make 2D and 1D simplified APIs

private:
	static int process_header(std::ifstream& infile, SSM_HEADER_VARIABLES& shv);
	static size_t pos2ind(const int* pos, const size_t* dim_mult, const int numdim);
};

