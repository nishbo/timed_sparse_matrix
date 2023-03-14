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
* Declarations of exported TSM functions.
*
*/
#pragma once

#ifdef TSMDLL_EXPORTS
#define TSMLIBRARY_API __declspec(dllexport)
#else
#define TSMLIBRARY_API __declspec(dllimport)
#endif


// TODO docs
extern "C" TSMLIBRARY_API void tsm_load(
    const char* filename, 
    double** times, 
    size_t* N,
    double** tensor_data,
    size_t** dims,
    size_t* numdim);


// TODO docs
// WARNING - can cause process crash
extern "C" TSMLIBRARY_API void tsm_free(
    double** times,
    double** tensor_data,
    size_t** dims);


// TODO docs
extern "C" TSMLIBRARY_API void tsm_save(
    const char* filename,
    const char* type,
    const double* times,
    const size_t N,
    const double* tensor_data,
    const size_t * dims,
    const size_t numdim,
    const double default_value=0.);
