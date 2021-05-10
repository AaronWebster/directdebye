#include <assert.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this struct holds all arguments which can be passed from the command
 * line */
#ifndef INC_ARGS_T
#define INC_ARGS_T
#define BUFFER_SIZE 512
typedef struct {
  /* the output_* flags control which components are written to the HDF
   * file */
  /* output real + imaginary components of the electric field? */
  int output_ex;
  int output_ey;
  int output_ez;

  /* output mag squared components of electric field? */
  int output_ex2;
  int output_ey2;
  int output_ez2;

  /* output |ex|^2 + |ey|^2 + |ez|^2 ? */
  int output_e2;

  /* pixel dimensions of the output field, x, y, z, and t */
  unsigned int dim_efield_out[4];

  /* upper bounds for the output field */
  double dim_efield_ub[4];

  /* lower bounds for the output field */
  double dim_efield_lb[4];

  unsigned int efield_ss_out;

  /* temporal offset */
  double temporal_offset;

  /* radius of focal sphere, not required */
  double fs_radius;

  /* input file with rays */
  char filename_in[FILENAME_MAX];

  /* output file for the HDF files */
  char filename_out[FILENAME_MAX];

  /* contains equation for pulse envelope */
  char pfcn_intensity_buffer[BUFFER_SIZE];
  char pfcn_g_buffer[BUFFER_SIZE];

  /* entrance pupil in meters */
  double epupil;

} args_t;
#endif

/* parse command line arguments.  Return a properly set up args_t */
void parse_args(int argc, char **argv, args_t *a);
