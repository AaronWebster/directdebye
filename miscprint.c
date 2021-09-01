#include "miscprint.h"

// Print program usage.
void print_usage(void) {
  fprintf(stderr, "Usage: ./directdebye [OPTIONS] RAYFILE\n");
  fprintf(stderr,
          "Consult the manual (`man directdebye`) for complete usage "
          "information.\n\n");
}

/* Print some of the program parameters to stderr.  This information is
 * (usually) automatically logged by torque. */
void print_args(args_t *a, unsigned int nrays) {
  fprintf(stderr, "read %d rays from %s\n", nrays, a->filename_in);
  fprintf(stderr, "efield dimensions x:y:z:t   %d:%d:%d:%d\n",
          a->dim_efield_out[0], a->dim_efield_out[1], a->dim_efield_out[2],
          a->dim_efield_out[3]);
  fprintf(stderr, "efield lower bounds x,y,z,t %g:%g:%g:%g\n",
          a->dim_efield_lb[0], a->dim_efield_lb[1], a->dim_efield_lb[2],
          a->dim_efield_lb[3]);
  fprintf(stderr, "efield upper bounds x:y:z:t %g:%g:%g:%g\n",
          a->dim_efield_ub[0], a->dim_efield_ub[1], a->dim_efield_ub[2],
          a->dim_efield_ub[3]);
  fprintf(stderr, "temporal offset:\t%g\n", a->temporal_offset);
  fprintf(stderr, "output will consist of\n");
  if (a->output_ex) {
    fprintf(stderr, "\tex\n");
  }
  if (a->output_ey) {
    fprintf(stderr, "\tey\n");
  }
  if (a->output_ez) {
    fprintf(stderr, "\tez\n");
  }
  if (a->output_ex2) {
    fprintf(stderr, "\t|ex|^2\n");
  }
  if (a->output_ey2) {
    fprintf(stderr, "\t|ey|^2\n");
  }
  if (a->output_ez2) {
    fprintf(stderr, "\t|ez|^2\n");
  }
  if (a->output_e2) {
    fprintf(stderr, "\t|e|^2\n");
  }
  fprintf(stderr, "output file:\t%s\n", a->filename_out);
  fprintf(stderr, "pulse envelope:\n%s\n", a->pfcn_intensity_buffer);
  fprintf(stderr, "apodization function :\n%s\n", a->pfcn_g_buffer);
}
