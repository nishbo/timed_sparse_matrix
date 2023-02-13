mex COMPFLAGS='$COMPFLAGS -std:c++17 /Otx' tsm_matlab.cpp ..\cpp\tsm.cpp -I..\cpp -output tsm.mex

[times, data, dims] = tsm('load', '../example_period_file.tsm');
N = length(times);
Ndims = length(dims);
d = permute(permute(reshape(data, [fliplr(dims) N]), [Ndims:-1:1 Ndims+1]), [Ndims+1 1:Ndims]);