function tsm_save(filename, type, times, formatted_tensors, default_value)
%TSM_SAVE Saves a tensor into a file from a MATLAB array-tensor
% Wrapper around the .mex function that formats the data for C++ and deals with different order
% of indices in MATLAB and C.
    dims = size(formatted_tensors);
    dims = dims(2:end);
    Ndims = length(dims);
    tensor_data = reshape(permute(formatted_tensors, [Ndims+1:-1:1]), [1 numel(formatted_tensors)]);

    tsm('save', filename, type, times, tensor_data, dims, default_value);
end
