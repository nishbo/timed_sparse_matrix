function install()
%INSTALL adds the bin directory to the MATLAB path
    p = fullfile(pwd(), 'bin');
    addpath(p);
    savepath();
end