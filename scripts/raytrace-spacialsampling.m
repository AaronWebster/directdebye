#!/usr/bin/octave -qf

# raytrace-spatialsampling.m
#
# Given the numerical aperture, wavelength, and size of the output field,
# determine the minimum number of samples to avoid aliasing in that field.

arg_list = argv();
if(length(arg_list)<3)
	printf("Usage:  ./raytrace-spatialsampling.m <window size> <wavelength> <numerical aperture>\n");
	exit();
endif

fullwindowsize = str2num(arg_list{1});
wavelength = str2num(arg_list{2});
numericalaperture = str2num(arg_list{3});

printf("numerical aperture:\t%g\n", numericalaperture);
printf("window size:\t%g\n", fullwindowsize);
printf("wavelength:\t%g\n", wavelength);
samples = fullwindowsize*2*numericalaperture/wavelength;
printf("samples required:\t%g x %g = %g\n",samples,samples,samples^2);
