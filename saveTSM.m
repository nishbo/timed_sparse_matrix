function saveTSM(filename, data, time, varargin)

% Get data information
data_dim = size(data);
dims = data_dim(2:end);

% Check the default_value
default_value = mode(data(:));

% Check the type of time data that was passed in
if length(time) == data_dim(1)
    time_type = 'stamps';
elseif length(time) == 2
    time_type = 'period';
    time_start = time(1);
    time_period = time(2);
else
    warning('Not valid time argument')
end

for vv = 1:2:length(varargin)
    if strcmpi(varargin{vv}, 'default_value')
        default_value = varargin{vv + 1};
    elseif strcmpi(varargin{vv}, 'n')
        N = varargin{vv + 1};
    else
        warning('Invalid argument')
    end
end

%% Write metadata

% TODO: Check if file exists and overwrite it (or ask user if they wish to
% overwrite it?)
fid = fopen(filename, 'w');

fprintf(fid, 'dims=');
for dd = 1:length(dims)
    if dd ~= length(dims)
        fprintf(fid, '%i,', dims(dd));
    else
        fprintf(fid, '%i\n', dims(dd));
    end
end
fprintf(fid, 'default_value=%i\n', default_value);
fprintf(fid, 'time=%s\n', time_type);
if strcmpi(time_type, 'period')
    fprintf(fid, 'time_start=%i\n', time_start);
    fprintf(fid, 'time_period=%f\n', time_period);

    % TODO: Implement N
end
fprintf(fid, '----\n');

%% Write data
numTime = data_dim(1);
for tt = 1:numTime
    % TODO: Implement decimal point precision
    if strcmpi(time_type, 'stamps')
        fprintf(fid, '%f', time(tt));
    else
        fprintf(fid, '%i', time_start + (tt - 1));
    end
    
    % TODO: default value check (breaks for NaNs and what not)
    [rowId, colId] = find(squeeze(data(tt, :, :)) ~= default_value);
    for ii = 1:length(rowId)
        fprintf(fid, ';%d,%d,%f', rowId(ii), colId(ii), data(tt, rowId(ii), colId(ii)));
    end
    fprintf(fid, '\n');
end

fid = fclose(fid);

end
