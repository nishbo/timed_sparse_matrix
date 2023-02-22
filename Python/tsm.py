#!python3.7
import os
import ctypes
import functools

import numpy as np


tsm_dll = ctypes.cdll.LoadLibrary(os.path.join(
    '..', 'DLL', 'TSMDLL', 'x64', 'Release', 'TSMDLL.dll'))


def ctypes2np(parr, N):
    return np.array((parr[0]._type_ * N).from_address(ctypes.addressof(parr[0])))


def _load_low(filename):
    ptimes = ctypes.pointer((ctypes.c_double*0)())
    N = ctypes.c_size_t()
    ptensor_data = ctypes.pointer((ctypes.c_double*0)())
    pdims = ctypes.pointer((ctypes.c_size_t*0)())
    numdim = ctypes.c_size_t()

    tsm_dll.tsm_load(
        filename.encode('ascii'),
        ctypes.pointer(ptimes),
        ctypes.byref(N),
        ctypes.pointer(ptensor_data),
        ctypes.pointer(pdims),
        ctypes.byref(numdim))

    N = N.value
    numdim = numdim.value
    times = ctypes2np(ptimes, N)
    dims = ctypes2np(pdims, numdim)
    M = int(np.prod(dims))
    tensor_data = ctypes2np(ptensor_data, M * N)

    # tsm_dll.tsm_free(
    #     ctypes.pointer(ptimes),
    #     ctypes.pointer(ptensor_data),
    #     ctypes.pointer(pdims))

    return times, N, tensor_data, dims, M


def load(filename):
    times, N, tensor_data, dims, M = _load_low(filename)

    Mdims = np.insert(dims, 0, N)
    formatted_tensor_data = np.reshape(tensor_data, Mdims)

    return times, formatted_tensor_data


def _save_low(filename, type, times, N, tensor_data, dims, M, default_value=0.):
    ctimes = (ctypes.c_double * N)()
    for i_time, time in enumerate(times):
        ctimes[i_time] = time

    ctensor_data = (ctypes.c_double * (M * N))()
    for i_tensor_data, td in enumerate(tensor_data):
        ctensor_data[i_tensor_data] = td

    cdims = (ctypes.c_size_t * len(dims))()
    for i_dims, dim in enumerate(dims):
        cdims[i_dims] = dim

    tsm_dll.tsm_save(
        filename.encode('ascii'),
        type.encode('ascii'),
        ctimes,
        ctypes.c_size_t(N),
        ctensor_data,
        cdims,
        ctypes.c_size_t(len(dims)),
        ctypes.c_double(default_value))


def save(filename, type, times, formatted_tensor_data, default_value=0.):
    N = len(times)
    dims = np.shape(formatted_tensor_data)[1:]
    M = int(np.prod(dims))
    tensor_data = np.reshape(formatted_tensor_data, (N*M, 1))
    _save_low(filename, type, times, N, tensor_data, dims, M, default_value=default_value)




if __name__ == '__main__':
    times, formatted_tensor_data = load(os.path.join('..', 'example_period_file.tsm'))
    print(times)
    print(formatted_tensor_data)
    print(formatted_tensor_data[0][0][1])

    save(os.path.join('..', 'example_period_file_po.tsm'), 'period', times, formatted_tensor_data)

    times, formatted_tensor_data = load(os.path.join('..', 'example_stamps_file.tsm'))
    save(os.path.join('..', 'example_stamps_file_po.tsm'), 'stamps', times, formatted_tensor_data)
