#!/usr/bin/octave -qf

# raytrace-samplepulse.m
#
# Given parameters of a pulse, this program determines the correct sampling
# for that pulse in the frequency domain.
#
# The output suitable for use in RAYTRACE as a "script".


fwhm=4e-15; # full width at half max in the time domain (|E|^2)
wavelength=800e-9; # center wavelength
fullwindowz=50e-6; # size of full window in z direction
energyint=0.99999; # want this much energy to be included in the time domain

st=fwhm; # my alias for FWHM
# center (angular) frequency
centerw=2*pi*299792458/wavelength;
# window in the time domain
bounds_w=erfinv(energyint)*2*sqrt(log(2))/st;
# corresponding window in ang. frequency domain for the same energy
bounds_t=erfinv(energyint)*st/(2*sqrt(log(2)));

# how many samples we must take in the time domain
samples=ceil(2*bounds_w*((fullwindowz)/299792458)/(2*pi));

# make a space with such and such amount of samples
wspace = linspace(centerw-bounds_w,centerw+bounds_w,samples);
for w = wspace
	printf('SETVAL(0,LAMBDA)=%.15g;\n',2*pi*299792458/w * 1000);
	printf('TRACE\n');
	printf('EXPORTRAYS(0,0,0,0,0,0,40,1,"c:\\rays.txt",1)\n');
endfor
