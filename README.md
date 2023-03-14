# Timed Sparse Matrices

Save timed sparse matrices and tensors to readable files from Python, MATLAB, and C++.


## File Format

The file starts with meta information, which describes the format of the data.


### Meta-information fields

* `dims` -- dimensionality (shape) of matrices. E.g., `dims=5` means that the data are vectors of five elements; `dims=3,4` -- 3 by 4 matrices; `dims=3,4,5` -- tensors of 3x4x5 shape, and so on. The loaded data is then of format [time index][first data axis index][second data axis index], etc. For more detail, see specific language implementation descriptions.

* `default_value` -- the default value used for all omitted elements, e.g., `default_value=0` or `default_value=NaN`.

* `time` -- how time point of each matrix is calculated.

    - `stamps` means that each line starts with a value for time. If there is a time point with all values equal to `default_values`, then the line is empty after the time stamp. [Example](./example_stamps_file.tsm).

    - `period` means that the data was sampled regularly from a specific time with a specified period. Needs two additional definitions: `time_start` and `time_period`. The first column is then an index of the timestamp, and time is reconstructed from those definitions. Indexing start at 0, and if an index is omitted, that matrix is filled with `default_value`. [Example](./example_period_file.tsm).

* `N` is an optional parameter to define the maximum number of time points expected. For example, if in `time=period` the last time-index is 7, but `N=10`, the functions will return 2 additional timepoints filled with default_value.

### Data

Everything below the 4 dashes (`----`) is stored data. On each line, after the time-index, the data points are separated with semicolons (e.g., `0.4;0,2,3.5;2,3,2.2`, where `0.4` is the time stamp, `0,2,3.5` and `2,3,2.2` are the two non-zero elements). First values separated by commas describe the index of the element, and the last one -- its value (from previous example, matrix at that time point will have 3.5 in the element with index [0][2] and 2.2 in element [2][3]).

### Example files

#### [Stamps](./example_stamps_file.tsm)

```
dims=3,4
default_value=0
time=stamps
----
0
0.1
0.2;1,1,1
0.4;0,2,3.5;2,3,2.2
0.5
0.75;2,3,4.5
```

This file describes explicitly timestamped matrices. The time vector will be [0, 0.1, 0.2, 0.4, 0.5, 0.75]. The values of the matrices will be 0, except for: matrix corresponding to the third timestamp (0.2), where the element in the second row second column will be 1, etc.

#### [Period](./example_period_file.tsm)

```
dims=3,4
default_value=0
time=period
time_start=0
time_period=0.1
N=10
----
0;0,1,12
2;1,1,1
4;0,2,3.5;2,3,2.2
7;2,3,4.5
```

This file describes ten 3x4 matrices timestamped periodically from 0. Time vector will be [0, 0.1, 0.2, ..., 0.9]. The values of the matrices will be 0, except for the: matrix corresponding to the first time point will have 12 in the first row second column, third time point -- 1 in the second row second column, etc.


## Functions

Load functions usually accept a filename, and returns 2 arrays: one array with timepoints, and one array with matrices created according to the description in the file.

Save function accepts a filename, keyword for `time`, data matrices, array with timepoints, and default value. NaN and Inf currently are not supported.

### C++

The [header file](./src/tsm.h) defines `Tsm` class, an instance of which can be created from data or from a filename:

```
Tsm::Tsm(const std::string filename);
Tsm::Tsm(const std::vector<double> time, const std::vector<double> data, const std::vector<size_t> dims);
```

There are two additional convenience functions that accept vectors of vectors and vectors of matrices:

```
Tsm::Tsm(const std::vector<double> time, const std::vector<std::vector<double>> vecs);
Tsm::Tsm(const std::vector<double> time, const std::vector<std::vector<std::vector<double>>> matrices);
```

The matrices within the object can be exported to a file using functions:

```
int Tsm::save_stamps(const std::string filename, const double default_value = 0.);
int Tsm::save_period(const std::string filename, const double default_value = 0.);
int Tsm::save(const std::string filename, const std::string type, const double default_value = 0.);
```

For example of use, see [CPP Project](./CPP_Project/TSM_CPP_Windows.sln) and its [main file](./CPP_Project/TSM_CPP_Windows/TSM_CPP_Windows.cpp).

### DLL

Another option is to use [DLL](./DLL/TSMDLL.dll). It's [header](./DLL/TSMDLL/TSMDLL/TsmLibrary.h) describes generic load and save functions. It has been compiled for Windows x64 use, but retargeting a solution should be trivial. It is also used to provide a Python interface.

### Python

Python works through a DLL and associated simplified API installed as a Python module. Examples of use are in the end of [tsm.py](./Python/tsm/tsm.py):

```
times, formatted_tensor_data = load(os.path.join('..', 'example_period_file.tsm'))
print(times)
print(formatted_tensor_data)
print(formatted_tensor_data[0][0][1])

save(os.path.join('..', 'example_period_file_po.tsm'), 'period', times, formatted_tensor_data)

times, formatted_tensor_data = load(os.path.join('..', 'example_stamps_file.tsm'))
save(os.path.join('..', 'example_stamps_file_po.tsm'), 'stamps', times, formatted_tensor_data)
```

TODO: a separate example of use.

### MATLAB

The API provides a save and a load function:

```
tsm_save(filename, type, times, formatted_tensors, default_value)
[times, formatted_tensors] = tsm_load(filename)
```

[Example](./MATLAB/Examples.m) of use.


## Installation

### C++

It is recommended to directly include [source code](./src) in your project. Another option is to link to [DLL](./DLL/TSMDLL.dll).

### Python

Navigate to the `./Python` directory and run:

```
timed_sparse_matrix\Python>python setup.py install
```

Replace `python` with any python that you are going to use TSM with. Then you can `include tsm` in your code.

Beware, we had experienced that sometimes the DLL is not updated after update to the source code, so it is recommended to uninstall the module and install it again when updating.

### MATLAB

Open `MATLAB` folder of the project in your MATLAB and run `install.m`, which will include the `bin` folder in your MATLAB PATH.


## Authors

- [**Anton Sobinov**](https://github.com/nishbo)
