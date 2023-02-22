# Timed Sparse Matrices

Load timed sparse matrices and tensors into a lists, matrices, vectors, etc. in Python, MATLAB, and C++.


## I/O functions

Load function accepts filename (with extension), and returns 2 arrays: one array with timepoints, and one array with normal matrices created according to the description in the file.

Save function accepts a filename (with extension), data matrices, and time defined either as start time and period, or as timestamps with the same length as data matrices first index. You can also provide the default value, or let it be calculated as the highest mode in the data. NaN and Inf currently are not supported.

### Python

TODO description

### MATLAB

TODO description

### C++

TODO description

## File structure

Everything above 4 dashes (`----`) contains meta-information, below that line is only data. Meta-information has description of the data structure.

`dims` describes dimensionality of matrices at each point of time, separated by commas. E.g., `dims=5` means that the data are vectors of five elements; `dims=3,4` -- 3 by 4 matrices; `dims=3,4,5` -- tensors of 3x4x5 shape, and so on. The loaded data is then of format [time index][first data axis index][second data axis index], etc. For more detail, see specific language implementation descriptions.

`default_value` describes the default value used for all omitted elements, e.g. `default_value=0` or `default_value=NaN`.

`time` describes the type of representation of time values. 

* `stamps` means that each line starts with a value for time. If the matrix is only consists of the default_values, then the line is empty after the time stamp. [Example](./example_stamps_file.tsm).
* `period` means that the data was sampled regularly from a specific time with a specified period. Needs two additional definitions: `time_start` and `time_period`. The first column is then an index of the timestamp, and time is reconstructed from those definitions. Indexing start at 0. [Example](./example_period_file.tsm).

`N` is an OPTIONAL parameter to define the maximum number of time points expected. For example, if in `time=period` the last time-index is 7, but `N=10`, the functions will return 2 additional timepoints filled with default_value.


## Authors

- [**Anton Sobinov**](https://github.com/nishbo)
- Developed in [Bensmaia Lab](http://bensmaialab.org/).
