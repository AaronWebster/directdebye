#ifndef HDFIO_H
#define HDFIO_H

#include <assert.h>
#include <complex.h>
#include <errno.h>
#include <error.h>
#include <hdf5.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseargs.h"

void write_efield(args_t *a, complex double *efield_x, complex double *efield_y,
                  complex double *efield_z);

#endif  // HDFIO_H
