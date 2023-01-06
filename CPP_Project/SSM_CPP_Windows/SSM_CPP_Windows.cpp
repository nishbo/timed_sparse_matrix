// SSM_CPP_Windows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <filesystem>  // C++17 - only needed for the interface

#include "SSM.h"

using namespace std;
namespace fs = std::filesystem;

int main()
{

	fs::path current_path = fs::current_path();
	fs::path exec_dir;
	string basename = current_path.filename().string();
	//cout << "Current path: " << current_path << "." << endl;
	if (basename == "Debug") {
		// simple_sparse_matrix\\CPP_Project\\x64\\Debug
		exec_dir = current_path / ".." / ".." / "..";
	}
	else {
		// simple_sparse_matrix\\CPP_Project\\SSM_CPP_Windows
		exec_dir = current_path / ".." / "..";
	}
	fs::path period_filename_in = exec_dir / "example_period_file.ssm";
	fs::path stamps_filename_in = exec_dir / "example_stamps_file.ssm";
	if (!fs::exists(period_filename_in)) {
		cout << "Input file " << period_filename_in << " does not exist." << endl;
		return -1;
	}
	if (!fs::exists(stamps_filename_in)) {
		cout << "Input file " << stamps_filename_in << " does not exist." << endl;
		return -1;
	}

	double* time = nullptr;
	double* data = nullptr;
	int* dims = nullptr;
	int numdim = 0, answ;
	answ = SSM::load(period_filename_in.string(), time, data, dims, numdim);
	answ = SSM::load(stamps_filename_in.string(), time, data, dims, numdim);

	return answ;
}
