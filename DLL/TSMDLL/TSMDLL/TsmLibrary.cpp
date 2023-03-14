/*
* Timed Sparse Matrices
* Copyright (C) 2023  Anton Sobinov
* https://github.com/nishbo/timed_sparse_matrix
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*
* Defines the exported functions for the DLL.
*
*/
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

#include <iostream>
#include <stdlib.h>     /* malloc, free */
#include <tsm.h>
#include "TsmLibrary.h"


using namespace std;


template<typename T>
T* vec2arr(std::vector<T>& vec) {
    T* arr = (T*)malloc(vec.size() * sizeof(T));
    if (arr)
        for (size_t i = 0; i < vec.size(); i++)
            arr[i] = vec[i];
    return arr;
}

// TODO docs
void tsm_load(
    const char* filename,
    double** times,
    size_t* N,
    double** tensor_data,
    size_t** dims,
    size_t* numdim)
{
    TSM::Tsm tsm(filename);

    *times = vec2arr(tsm.time);
    *N = tsm.time.size();

    *tensor_data = vec2arr(tsm.data);

    *dims = vec2arr(tsm.dims);
    *numdim = tsm.dims.size();
}


// TODO docs
void tsm_free(double** times, double** tensor_data, size_t** dims)
{
    free(times);
    free(tensor_data);
    free(dims);
}


// TODO docs
void tsm_save(
    const char* filename,
    const char* type,
    const double* times,
    const size_t N,
    const double* tensor_data,
    const size_t* dims,
    const size_t numdim,
    const double default_value)
{
    std::vector<double> time(N);
    for (size_t i = 0; i < N; i++)
        time[i] = times[i];

    size_t M = 1;
    std::vector<size_t> _dims(numdim);
    for (size_t i = 0; i < numdim; i++) {
        M *= dims[i];
        _dims[i] = dims[i];
    }

    std::vector<double> data(M * N);
    for (size_t i = 0; i < M * N; i++)
        data[i] = tensor_data[i];

    TSM::Tsm tsm(time, data, _dims);
    int answ = tsm.save(filename, type, default_value);
    if (answ < 0)
        cout << "Problems saving file. Code: " << to_string(answ) << "." << endl;

}
