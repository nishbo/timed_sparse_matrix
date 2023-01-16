#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

namespace TSM {

	enum class TSM_FILETYPE
	{
		NONETYPE,  // error type
		STAMPS,
		PERIOD
	};


	class TsmHeaderVariables
	{
	public:
		~TsmHeaderVariables();

		int* dims = nullptr;
		int numdim = 0;

		double default_value = 0;  // 0 by default

		TSM_FILETYPE filetype = TSM_FILETYPE::NONETYPE;

		// scans using atoi, so still restricted to integer
		size_t N = -1;

		// for TSM_FILETYPE::PERIOD
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
		std::map<std::string, int(TsmHeaderVariables::*)(std::string)> m_setter_map = {
			{"dims",			&TsmHeaderVariables::set_dims},
			{"default_value",	&TsmHeaderVariables::set_default_value},
			{"time",			&TsmHeaderVariables::set_filetype},
			{"N",				&TsmHeaderVariables::set_N},
			{"time_start",		&TsmHeaderVariables::set_time_start},
			{"time_period",		&TsmHeaderVariables::set_time_period},
		};
	};

	// used to go through comma-sepearated strings
	// a variation on CSVReader
	class TsmCommaSeparatedView
	{
	public:
		TsmCommaSeparatedView() = delete;
		TsmCommaSeparatedView(std::string str, char separator = ',');

		std::string_view operator[](std::size_t index) const;
		std::size_t size() const;
	private:
		std::string m_str;
		std::vector<std::size_t> m_data;
	};

	class TsmLine
	{
	public:
		TsmLine() = delete;
		TsmLine(std::size_t _num, TsmHeaderVariables& shv);
		TsmLine(std::string str, TsmHeaderVariables& shv);

		std::size_t num = -1;
		double time = -1;
		TsmCommaSeparatedView line;

		TsmCommaSeparatedView operator[](std::size_t index) const;
		std::size_t size() const;
	private:

	};


	class Tsm
	{
	public:
		Tsm() = delete;

		static void print(const double* time, const double* data, const size_t& N, const int* dims, const int& numdim);

		static int load(const std::string filename, double* time, double* data, size_t& N, int* dims, int& numdim);
		static int save(const std::string filename, const double* time, const double* data, const int* dims, const int numdim);

		// TODO make 2D and 1D simplified APIs

	private:
		static int process_header(std::ifstream& infile, TsmHeaderVariables& shv);
		static size_t pos2ind(const int* pos, const size_t* dim_mult, const int numdim);
		static size_t calc_matrix_M(const int* dims, const int numdim);
	};

}