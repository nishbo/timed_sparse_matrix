#!python3
# -*- coding: utf-8 -*-
"""Main module file. Defines formatted load and save functions.

Timed Sparse Matrices
Copyright (C) 2023 Anton Sobinov
https://github.com/nishbo/timed_sparse_matrix

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""
import os
import ctypes
import functools

import numpy as np


tsm_dll = ctypes.cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), 'TSMDLL.dll'))


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

    # for some reason, crashes the process
    # tsm_dll.tsm_free(
    #     ctypes.pointer(ptimes),
    #     ctypes.pointer(ptensor_data),
    #     ctypes.pointer(pdims))

    return times, N, tensor_data, dims, M


def load(filename):
    """Loads a .tsm file into formatted numpy arrays.

    Arguments:
        filename {str} -- string with the address of the file.
    Returns:
        times {numpy array} -- time stamps.
        formatted_tensor_data {numpy matrix} -- data. Indices:
            [time index, first data index, second data index, etc.]
    """
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
    """Saves formatted numpy arrays into a .tsm file.

    Arguments:
        filename {str} -- string with the address of the file.
        type {'period' or 'stamps'} -- type of file to export. If 'period' is specified, but the
            times are not periodic, the export will be aborted.
        times {numpy array} -- time stamps.
        formatted_tensor_data {numpy matrix} -- data. Indices:
            [time index, first data index, second data index, etc.]. Has to match the length of
            times.
    Keyword Arguments:
        default_value {float} -- elements of the tensor equal to this value will be skipped during
            export, saving space. (default: 0)
    """
    N = len(times)
    dims = np.shape(formatted_tensor_data)[1:]
    M = int(np.prod(dims))
    tensor_data = np.reshape(formatted_tensor_data, (N*M, 1))
    _save_low(filename, type, times, N, tensor_data, dims, M, default_value=default_value)


if __name__ == '__main__':
    # proto example
    times, formatted_tensor_data = load(os.path.join('..', 'example_period_file.tsm'))
    print(times)
    print(formatted_tensor_data)
    print(formatted_tensor_data[0][0][1])

    save(os.path.join('..', 'example_period_file_po.tsm'), 'period', times, formatted_tensor_data)

    times, formatted_tensor_data = load(os.path.join('..', 'example_stamps_file.tsm'))
    save(os.path.join('..', 'example_stamps_file_po.tsm'), 'stamps', times, formatted_tensor_data)
