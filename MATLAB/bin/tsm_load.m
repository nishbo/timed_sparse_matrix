function [times, formatted_tensors] = tsm_load(filename)
%TSM_LOAD Loads a tensor from a file into a MATLAB array
% TODO documentation
    [times, tensor_data, dims] = tsm('load', filename);
    N = length(times);
    Ndims = length(dims);
    formatted_tensors = permute(reshape(tensor_data, [fliplr(dims) N]), [Ndims+1:-1:1]);
end
