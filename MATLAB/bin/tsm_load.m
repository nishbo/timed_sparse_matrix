function [times, formatted_tensors] = tsm_load(filename)
%TSM_LOAD Loads a tensor from a file into a MATLAB array
% Wrapper around the .mex function that formats the data for MATLAB and deals with different order
% of indices in MATLAB and C.
    [times, tensor_data, dims] = tsm('load', filename);
    N = length(times);
    Ndims = length(dims);
    formatted_tensors = permute(reshape(tensor_data, [fliplr(dims) N]), [Ndims+1:-1:1]);
end
