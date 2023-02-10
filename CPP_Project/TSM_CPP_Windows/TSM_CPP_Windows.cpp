// TSM_CPP_Windows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <filesystem>  // C++17 - only needed for the interface

#include "tsm.h"

using namespace std;
namespace fs = std::filesystem;

// TODO make 1d and 3d examples


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

	int answ;

	// filenames
	fs::path stamps_filename_in = exec_dir / "example_stamps_file.tsm";
	fs::path period_filename_in = exec_dir / "example_period_file.tsm";
	fs::path stamps_filename_ou = exec_dir / "example_stamps_file_o.tsm";
	fs::path period_filename_ou = exec_dir / "example_period_file_o.tsm";
	fs::path stamps_filename_ou2 = exec_dir / "example_stamps_file_o2.tsm";
	fs::path period_filename_ou2 = exec_dir / "example_period_file_o2.tsm";
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

	// export directly from loaded structure
	answ = tsm_stamps.save_stamps(stamps_filename_ou.string());
	if (answ < 0)
		cout << "Could not export to " << stamps_filename_ou.string() << ". Error: " << to_string(answ) << endl;

	// export from programmatically generated data
	TSM::Tsm tsm_stamps_2(time, m);
	answ = tsm_stamps_2.save_stamps(stamps_filename_ou2.string());
	if (answ < 0)
		cout << "Could not export to " << stamps_filename_ou2.string() << ". Error: " << to_string(answ) << endl;

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

	// export directly from loaded structure
	answ = tsm_period.save_period(period_filename_ou.string());
	if (answ < 0)
		cout << "Could not export to " << period_filename_ou.string() << ". Error: " << to_string(answ) << endl;

	// export from programmatically generated data
	TSM::Tsm tsm_period_2(time, m);
	answ = tsm_period_2.save_period(period_filename_ou2.string());
	if (answ < 0)
		cout << "Could not export to " << period_filename_ou2.string() << ". Error: " << to_string(answ) << endl;

	return 0;
}
