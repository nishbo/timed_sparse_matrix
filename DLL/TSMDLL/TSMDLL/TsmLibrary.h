// TsmLibrary.h - Contains declarations of exported tsm functions
#pragma once

#ifdef TSMDLL_EXPORTS
#define TSMLIBRARY_API __declspec(dllexport)
#else
#define TSMLIBRARY_API __declspec(dllimport)
#endif

// TODO HELP

extern "C" TSMLIBRARY_API void test(double** times, size_t * N);
extern "C" TSMLIBRARY_API void test_free(double** times);

// TODO
extern "C" TSMLIBRARY_API void tsm_load(
    const char* filename, 
    double** times, 
    size_t* N,
    double** tensor_data,
    size_t** dims,
    size_t* numdim);


// TODO 
extern "C" TSMLIBRARY_API void tsm_free(
    double** times,
    double** tensor_data,
    size_t** dims);


// TODO
extern "C" TSMLIBRARY_API void tsm_save(
    const char* filename,
    const char* type,
    const double* times,
    const size_t N,
    const double* tensor_data,
    const size_t * dims,
    const size_t numdim,
    const double default_value=0.);
