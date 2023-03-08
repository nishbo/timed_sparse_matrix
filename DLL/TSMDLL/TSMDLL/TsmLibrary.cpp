// TsmLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

#include <iostream>
#include <stdlib.h>     /* malloc, free */
#include <tsm.h>
#include "TsmLibrary.h"


using namespace std;
/*
from ctypes import *
import numpy as np
lib = cdll.LoadLibrary('TSMDLL.dll')
N = c_size_t()
ptimes = pointer((c_double*0)())

lib.test(pointer(ptimes), byref(N))

N = N.value
times = np.array((c_double * N).from_address(addressof(ptimes[0])))
 
lib.test_free(pointer(ptimes))
 
*/

void test(double** times, size_t* N)
{
    *times = (double*) malloc (2 * sizeof(double));
    if (*times) {
        (*times)[0] = 10.;
        (*times)[1] = 20.;
        std::cout << "Normal\n";
    }
    else {
        std::cout << "Error\n";
    }

    *N = 2;
}

// Don't call twice - will kill the process
void test_free(double** times)
{
    free(*times);
}


template<typename T>
T* vec2arr(std::vector<T>& vec) {
    T* arr = (T*)malloc(vec.size() * sizeof(T));
    if (arr)
        for (size_t i = 0; i < vec.size(); i++)
            arr[i] = vec[i];
    return arr;
}

// TODO descr
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

void tsm_free(double** times, double** tensor_data, size_t** dims)
{
    free(times);
    free(tensor_data);
    free(dims);
}


// TODO descr
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
