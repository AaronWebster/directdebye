#ifndef PARSEARGS_H
#define PARSEARGS_H

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This struct holds all arguments which can be passed from the command line.
typedef struct {
  // The output_* flags control which components are written to the HDF file.
  // Output real + imaginary components of the electric field?
  int output_ex;
  int output_ey;
  int output_ez;

  // Output mag squared components of electric field?
  int output_ex2;
  int output_ey2;
  int output_ez2;

  // Output |ex|^2 + |ey|^2 + |ez|^2 ?
  int output_e2;

  // Pixel dimensions of the output field, x, y, z, and t.
  unsigned int dim_efield_out[4];

  // Upper bounds for the output field.
  double dim_efield_ub[4];

  // Lower bounds for the output field.
  double dim_efield_lb[4];

  unsigned int efield_ss_out;

  // Temporal offset.
  double temporal_offset;

  // Radius of focal sphere, not required.
  double fs_radius;

  // Input file with rays.
  char filename_in[FILENAME_MAX];

  // Output file for the HDF files.
  char filename_out[FILENAME_MAX];

  // Contains equation for pulse envelope.
  char pfcn_intensity_buffer[512];
  char pfcn_g_buffer[512];

  // Entrance pupil in meters.
  double epupil;

} args_t;

// Parse command line arguments.  Return a properly set up args_t.
void parse_args(int argc, char **argv, args_t *a);

#endif  // PARSEARGS_H
