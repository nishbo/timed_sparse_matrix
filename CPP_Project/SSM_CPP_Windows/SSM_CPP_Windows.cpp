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
		// simple_sparse_matrix\\CPP_Project\\SSM_CPP_Windows
		exec_dir = current_path / ".." / "..";
	}

	// filenames
	fs::path stamps_filename_in = exec_dir / "example_stamps_file.ssm";
	fs::path period_filename_in = exec_dir / "example_period_file.ssm";
	if (!fs::exists(stamps_filename_in)) {
		cout << "Input file " << stamps_filename_in << " does not exist." << endl;
		return -1;
	}
	if (!fs::exists(period_filename_in)) {
		cout << "Input file " << period_filename_in << " does not exist." << endl;
		return -1;
	}

	double* time = nullptr;
	double* data = nullptr;
	size_t N = 0;
	int* dims = nullptr;
	int numdim = 0, answ;

	cout << "Loading " << stamps_filename_in << endl;
	answ = SSM::load(stamps_filename_in.string(), time, data, N, dims, numdim);
	cout << endl;

	cout << "Loading " << period_filename_in << endl;
	answ = SSM::load(period_filename_in.string(), time, data, N, dims, numdim);
	cout << endl;

	return answ;
}
