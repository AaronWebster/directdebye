# COMPILING DIRECTDEBYE

In the `directdebye` directory, compiling should be as simple as typing

```
make clean
make
```

`directdebye` works with mpicc, both GNU and Intel compilers have been tested
and work fine.  Additional compiler flags (e.g. `-fast` for Intel) may be
specified in the `CFLAGS` variable in the `Makefile`.

In addition to the standard set of libraries, the following are required
for compilation and must be in your `INCLUDE_PATH` and `LIBRARY_PATH`:

libgsl
zlib
libhdf5
libmatheval


# OBTAINING RAYS

`directdebye` operates on rays, which must be properly sampled and obtained
from an external program.  This document assumes you are working with
RAYTRACE, but any program which provides the same output is acceptable.

There are three types of programs in this directory:

1.  `directdebye` itself, written in C and compiled with mpicc.
2.  Supporting scripts in the ./scripts directory
3.  Scripts in the base directory for compiling and launching `directdebye`.

First, create an optical system in RAYTRACE.  If you want a certain
entrance pupil radius or certain numerical aperture, the script
entrancepupil.m can calculate either for some common geometries.  It's
sometimes nice to have the  the Gaussian focus is at x=y=z=0.  This is done
by first tracing some rays at the center wavelength and then looking at the
output of Evaluation->Foci->perpendicular (2D).  Then the whole system is
moved by selecting every element, right click -> move, and entering the
negative of the value in the "z (in mm)" column of the "2D-foci of the
object points" window.  If you do not do this, `directdebye` can do it for
you with the --move-focus option.

Under the light source properties, make sure the entrance pupil type is
"elliptic".  This is currently the only entrance pupil type which seems to
produce reliable results.  Elliptic/gauss is okay too.

The script raytrace-spacialsampling.m helps determine the value to enter in
the box "rays/point (square number)".  A typical usage and output for a
200x200 micron window at 800 nanometers and NA=0.5 might be

./raytrace-spacialsampling.m 200e-6 800e-9 0.5
numerical aperture:     0.5
window size:    0.0002
wavelength:     8e-07
samples required:       250 x 250 = 62500

Thus, the number of rays to enter in the box would be 62500.

Next, edit the file raytrace-temporalsampling.m to reflect the desired
pulse parameters.  When you run the script, it will produce an output which
tells RAYTRACE what frequency components to trace.  Copy the output of the
script and paste it into the window in RAYTRACE under Extras -> Script
editor.  Right click on this window and select "RUN script".  The script
will run and many different frequency components will be traced.  The
output will be saved as a tab separated values file "rays.txt".  The output
of this file is 

0  1  2   ray position px py pz
3  4  5   normalized ray direction ex ey ez
6  7      polarization in x (real, complex)
8  9      polarization in y (real, complex)
10 11     polarization in z (real, complex)
12        optical path length
13        wavelength
14        apodization factor
15        ray intensity for the particular wavelength

all units are in millimeters.  Note that if a particular value such as the
apodization factor or the ray intensity is not specified, its value should
be unity.  These factors can be specified upon executing `directdebye`.

The script getrays.sh provides a convenient way of getting the ray file
from a directory, appending some ones on to the end if need be, and copying
them to the local directory.


RUNNING DIRECTDEBYE
-------------------
The following assumes you are running the program on an MPI capable cluster
running torque (Here in Erlangen woody, lima, etc.)

As a prerequisite, in the 'extras' directory there is a program called
linspace.  This functions much the same way as the matlab/octave function
linspace, but is written in C.  Compile this program:

gcc linspace.c -o linspace

and move into your $HOME/bin or /usr/local/bin (or wherever you please as
long as it's in your path).

Make a subdirectory, compile `directdebye`, and move the executable into it.
Also, copy the file "run.sh" and your ray file into the subdirectory. 

Since ray files tend to be quite large, it is important that each node have
fast access to the file.  If you're running on one of the RRZE clusters,
copy the file to $FASTTMP.  If you're running it on only one local machine,
you need not move it anywhere special.  If you do not have a high bandwidth
device such as what's mounted on $FASTTMP, make a local copy of the ray
file on each node.  The example script scatterfile.pl can help speed up
this transfer.

The main script for executing `directdebye` is called "run.sh".  proceed to
edit this file.  This exemplifies only one of many ways `directdebye` can be
run.  The default operation of "run.sh" is to create many different
scripts which run `directdebye`, each operating on one specific time slice.

First, change the variable $RAYFILE to reflect the name of the ray file.  This
script assumes this file is copied to $FASTTMP (and will copy it for you if
it's not already there).  Change the variable $FASTTMP to reflect the
directory of the ray file if running on a different kind of cluster.

Next, note the line 

for I in $(linspace -6.671281905e-13 6.671281905e-13 7)

This means that the program will run for 7 different time slices ranging
from -6.671281905e-13 to 6.671281905e-13 seconds, inclusive.  Change these
values to reflect the time slices you wish to calculate.

The next lines worth editing are the actual arguments to `directdebye`.
Let's go through them one by one and explain what they do:

--efield-out=1:50:100:1
Create an output file with dimensions x:y:z:t pixels.  Here it is set to
output the y-z slice.

--efield-lb=0:-0.1:-0.2:$I
Lower bounds for the electric field in the region of the focus.  Here the
lower bound is set for y=-0.1 and z=-0.2 millimeters.  The variable $I is
obtained from the main for loop and represents the time slice at hand.
Since 1 pixel in the t dimension was previously specified, it will only
output this lower bound for t

--efield-ub=1:0.1:0.2:10
Upper bounds for the electric field in the region of the focus.  Here the
upper bound is set for y=1.0 and z=0.2 millimeters.  The value '10' for t
is meaningless, since there is only one slice in t.  Note that the spacings
are determined by the boundaries and the --efield-out.  For example, the
spacings in y are the same as would be obtained by running the command

linspace -0.1 0.1 50

--radius=50.0 \\
Radius of the focal sphere.  This is required if --apodization-function is
set.  Otherwise it defaults to one.  Note that if this value is not set,
the output will only be qualitatively correct.

--temporal-offset
Most of the time the rays will be traced such that the temporal t=0 is not
exactly coincident with the spatial x=y=z=0.  Thus, a temporal offset can
be entered here.  To find the offset, follow these steps:

1.  Compile the program 'findmax' in the ./extras directory.  Copy this
somewhere in your $PATH, or put it in a local directory and modify
findoffset.sh appropriately.

2.  Create the data file findoffset.h5 by running the script
findoffset-qsub.sh in the scripts directory with the appropriate
parameters.  This effectively monitors the point x=y=z=0 as the pulse is
swept through the system.  Copy that file to this directory.  Then, run
findoffset.sh.  This script will take findoffset.h5, convert it into text
files, and then use the program findmax to find the maximum.  The negative
of the domain value at the maximum is the value to put into
--temporal-offset

--output-file="`printf %.4d $J`-parabola-$I.h5"
The name of the HDF file to write.  Here it's formatted such that you will
get files like:

0000-parabola--6.6712819049999997504e-13.h5

Where the first four numbers are just an incrementing index, and the
timestep the following number (stored in the filename).

--pulse-function='sqrt(40e-15*sqrt(pi)/sqrt(ln2))*
exp(-(x-2354564459136066)*(x-2354564459136066)*40e-15*40e-15*0.125/ln2)'
If the intensity of each wavelength isn't stored explicitly in the ray data
(ray->intensity), a pulse function may be specified.  Any string which is
supported by libmatheval is supported (see documentation for libmatheval
for specifics on this).  The function must be of only one variable x --
corresponding to the angular frequency (w=2*pi*c/lambda).  This function is
multiplied by the ray intensity.  This argument is optional, and if not
specified defaults to return 1.0.  Note the function is in SINGLE QUOTES!

--apodization-function='0.5*(1+cos(x))'
The apodization is represented by a function of a single variable, x,
representing the angle the ray makes with the optical axis.  x is
calculated internally as acos(z/r), where z is the intersection the ray
makes with the focal sphere and r is the radius of the focal sphere.

$FASTTMP/$RAYFILE
The last argument is the ray file.


Invoking the script ./run.sh creates a separate script for each of the
timesteps.  By default, these are automatically submitted to torque.  The
number nodes can be adjusted according to the size of the problem.  It is
important that the line

#PBS -l walltime=XX:XX:XX

Be adjusted to give the program enough time to run.  Scripts can be
submitted individually with the command

qsub -l nodes=16:ppn=4 <filename>

When finished, an HDF file is written containing the electric field data.
Two other files are also written by torque: a .log file and a .e* file.
These contain information about how the program was invoked and any errors
which may have occurred.


DIRECTDEBYE OUTPUT
------------------
Let's examine the output of a typical run of `directdebye`

```
h5ls ./example.h5

e2                       Dataset {1, 500, 1000, 1}
e2_phase                 Dataset {1, 500, 1000, 1}
efield_lower_bounds      Dataset {4}
efield_upper_bounds      Dataset {4}
ex.i                     Dataset {1, 500, 1000, 1}
ex.r                     Dataset {1, 500, 1000, 1}
ex2                      Dataset {1, 500, 1000, 1}
ex_phase                 Dataset {1, 500, 1000, 1}
ey.i                     Dataset {1, 500, 1000, 1}
ey.r                     Dataset {1, 500, 1000, 1}
ey2                      Dataset {1, 500, 1000, 1}
ey_phase                 Dataset {1, 500, 1000, 1}
ez.i                     Dataset {1, 500, 1000, 1}
ez.r                     Dataset {1, 500, 1000, 1}
ez2                      Dataset {1, 500, 1000, 1}
ez_phase                 Dataset {1, 500, 1000, 1}
temporal_offset          Dataset {1}
```

Each HDF file contains several datasets, each with a different part of the
electric field.  Note also that the boundary information (from `--efield-ub`
and `--efield-lb`) is stored as a linear array in `efield_lower_bounds` and
`efield_upper_bounds`. 

To convert, say the e2 field to a PNG file, use the h5topng command (part
of the h5tools package)

```
h5topng -c jet -t 0:0 -d e2 example.h5
```

For more options, see the man page for h5topng.  There are many other tools
(h5math, h5topng, h5totxt) which are useful for manipulating HDF files.
These may be useful for converting the output to a particular numerical
units.  For example, in the above no mention of the entrance pupil radius
was mentioned.  If not taken explicitly into account in the ray intensity,
the output will only be qualitative.  Using the h5math command however,
this can be done explicitly.  For example:

```
h5math -d e2 -e "d1/(0.25*0.25)" example_adjusted.h5 example.h5
```

This will multiply every member of the e2 dataset of the file example.h5 by
$1/(0.25*0.25)$, and produce a new file, example_adjusted.h5.  See the h5math
manual for more information.

