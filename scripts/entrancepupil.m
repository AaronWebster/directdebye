#!/usr/bin/octave -qf

# entrancepupil.m
#
# Prints the radius of the entrance pupil for a given numerical aperture
# and vice versa for several different geometries.
#
# Either the numerical aperture or radius of entrance pupil may be
# specified.  It is up to the user to properly interpret the outptut.

arg_list = argv();
if(length(arg_list)!=2)
	printf("Usage:  ./entrancepupil.m [ <numerical aperture> | <radius of entrance pupil> ] <focal length>\n");
	exit();
endif

# numerical aperture or radius
nar = str2num(arg_list{1});
# focal length
f = str2num(arg_list{2});

# check if arguments got parsed correctly
if(length(nar)==0 || length(f)==0)
	printf("Usage:  ./entrancepupil.m [ <numerical aperture> | <radius of entrance pupil> ] <focal length>\n");
	exit();
endif

printf("parabola:\n");
printf("if given NA, r = %s", disp(f*2*(1-sqrt(1-nar*nar))/nar));
printf("if given r, NA = %s\n", disp(sin(2*atan(nar/(2*f)))));

printf("aplanatic:\n");
printf("if given NA, r = %s", disp(f*nar));
printf("if given r, NA = %s\n", disp(nar/f));

printf("flat diffractive:\n");
printf("if given NA, r = %s", disp(f*tan(asin(nar))));
printf("if given r, NA = %s\n", disp(sin(atan(nar/f))));

