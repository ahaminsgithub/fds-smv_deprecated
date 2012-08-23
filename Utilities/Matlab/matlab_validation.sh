#!/bin/bash

matlab -r "try, disp('Running Matlab Validation script'), FDS_validation_script, catch, disp('Matlab error'), err = lasterror, err.message, err.stack, end, exit" 
