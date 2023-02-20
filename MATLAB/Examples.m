[times, formatted_tensors] = tsm_load('../example_period_file.tsm');
tsm_save('../example_period_file_mo.tsm', 'period', times, formatted_tensors, 0.)

[times, formatted_tensors] = tsm_load('../example_stamps_file.tsm');
tsm_save('../example_stamps_file_mo.tsm', 'stamps', times, formatted_tensors, 0.)