// TSM_CPP_Windows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <filesystem>  // C++17 - only needed for the interface

#include "tsm.h"

using namespace std;
namespace fs = std::filesystem;

int main()
{
	// current path
	fs::path current_path = fs::current_path();
	fs::path exec_dir;
	string basename = current_path.filename().string();
	//cout << "Current path: " << current_path << "." << endl;
	if (basename == "Debug") {
		// simple_sparse_matrix\\CPP_Project\\x64\\Debug
		exec_dir = current_path / ".." / ".." / "..";
	}
	else {
		// simple_sparse_matrix\\CPP_Project\\TSM_CPP_Windows
		exec_dir = current_path / ".." / "..";
	}

	// filenames
	fs::path stamps_filename_in = exec_dir / "example_stamps_file.tsm";
	fs::path period_filename_in = exec_dir / "example_period_file.tsm";
	if (!fs::exists(stamps_filename_in)) {
		cout << "Input file " << stamps_filename_in << " does not exist." << endl;
		return -1;
	}
	if (!fs::exists(period_filename_in)) {
		cout << "Input file " << period_filename_in << " does not exist." << endl;
		return -1;
	}


	std::vector<double> time;
	std::vector<std::vector<std::vector<double>>> m;
	vector<size_t> dims;

	cout << "Loading " << stamps_filename_in << endl;
	TSM::Tsm tsm_stamps(stamps_filename_in.string());
	cout << endl;
	tsm_stamps.print();
	cout << "Extracting matrices for print:" << endl;
	time = tsm_stamps.time;
	dims = tsm_stamps.dims;
	m = tsm_stamps.get_matrices();
	for (size_t i_time = 0; i_time < time.size(); i_time++) {
		cout << time[i_time] << ":" << endl;
		for (size_t i_row = 0; i_row < dims[0]; i_row++) {
			cout << "\t";
			for (size_t i_col = 0; i_col < dims[1]; i_col++) {
				cout << " " << m[i_time][i_row][i_col];
			}
			cout << endl;
		}
	}
	cout << endl;

	cout << "Loading " << period_filename_in << endl;
	TSM::Tsm tsm_period(period_filename_in.string());
	cout << endl;
	tsm_period.print();
	cout << "Extracting matrices for print:" << endl;
	time = tsm_period.time;
	dims = tsm_period.dims;
	m = tsm_period.get_matrices();
	for (size_t i_time = 0; i_time < time.size(); i_time++) {
		cout << time[i_time] << ":" << endl;
		for (size_t i_row = 0; i_row < dims[0]; i_row++) {
			cout << "\t";
			for (size_t i_col = 0; i_col < dims[1]; i_col++) {
				cout << " " << m[i_time][i_row][i_col];
			}
			cout << endl;
		}
	}
	cout << endl;

	return 0;
}
