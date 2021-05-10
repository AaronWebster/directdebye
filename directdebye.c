// directdebye.c
// Computes the Debye sum from raytracing data.
// See man page for usage information.
#include <assert.h>
#include <complex.h>
#include <errno.h>
#include <error.h>
#include <gsl/gsl_const_mksa.h>
#include <malloc.h>
#include <math.h>
#include <matheval.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hdfio.h"
#include "miscprint.h"
#include "parseargs.h"
#include "rays.h"

int main(int argc, char **argv) {
  /* MPI initialization */
  int rank, nprocs;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /* parse command line arguments */
  args_t *a = malloc(sizeof(args_t));
  parse_args(argc, argv, a);

  /* read the rays */
  unsigned int nrays; /* number of rays */
  ray_t *ray = NULL;
  ray = readrays(a->filename_in, &nrays);
  if (ray == NULL) {
    error_at_line(-1, errno, __FILE__, __LINE__, "Problem reading ray file.\n");
  }

  /* pulse function */
  void *pfcn_intensity = NULL;
  pfcn_intensity = evaluator_create(a->pfcn_intensity_buffer);
  if (pfcn_intensity == NULL) {
    error_at_line(-1, EINVAL, __FILE__, __LINE__,
                  "Problem creating pulse evaluator function.");
  }

  /* apodization function */
  void *pfcn_g = NULL;
  pfcn_g = evaluator_create(a->pfcn_g_buffer);
  if (pfcn_g == NULL) {
    error_at_line(-1, EINVAL, __FILE__, __LINE__,
                  "Problem creating apodization evaluator function.");
  }

  /* root node prints some useful information */
  if (rank == 0) {
    print_args(a, nrays);
  }

  /* Before allocating what could potentially be a lot of memory, check to
   * see if it's even possible.  */
  unsigned int i;
  a->efield_ss_out = 1;
  for (i = 0; i < 4; ++i) {
    a->efield_ss_out *= a->dim_efield_out[i];
  }

  long page_size = sysconf(_SC_PAGE_SIZE);
  // long pages_avail = sysconf(_SC_AVPHYS_PAGES);
  long pages_tot = sysconf(_SC_PHYS_PAGES);
  if (pages_tot * page_size < 8 * a->efield_ss_out * sizeof(complex double)) {
    error_at_line(-1, ENOMEM, __FILE__, __LINE__,
                  "Size of output arrays exceed avaliable memory.\n");
  }

  /* Allocate memory for the fields.  The _rcv is memory for nodes. */
  complex double *efield_x = NULL;
  complex double *efield_x_rcv = NULL;
  complex double *efield_y = NULL;
  complex double *efield_y_rcv = NULL;
  complex double *efield_z = NULL;
  complex double *efield_z_rcv = NULL;

  double *efield_r = NULL;
  double *efield_r_rcv = NULL;

  /* The root node will be receiving all of the data, so it gets the whole
   * thing.  Calloc here to pre-zero out everything. */
  if (rank == 0) {
    efield_x =
        (complex double *)calloc(a->efield_ss_out, sizeof(complex double));
    efield_y =
        (complex double *)calloc(a->efield_ss_out, sizeof(complex double));
    efield_z =
        (complex double *)calloc(a->efield_ss_out, sizeof(complex double));
    efield_r = (double *)calloc(4 * (a->efield_ss_out), sizeof(double));
  }

  /* The root node and all the other nodes also get this little chunk to
   * work on.  The root node really has nothing better to do while waiting
   * for information so there's no real waste. */
  efield_x_rcv = (complex double *)calloc(a->efield_ss_out / nprocs,
                                          sizeof(complex double));
  efield_y_rcv = (complex double *)calloc(a->efield_ss_out / nprocs,
                                          sizeof(complex double));
  efield_z_rcv = (complex double *)calloc(a->efield_ss_out / nprocs,
                                          sizeof(complex double));
  efield_r_rcv =
      (double *)calloc(4 * (a->efield_ss_out / nprocs), sizeof(double));

  /* Catch some possible malloc errors*/
  if (errno) {
    fprintf(stderr, "Node %d\n", rank);
    error_at_line(-1, errno, __FILE__, __LINE__, "Error allocating memory.");
  }

  unsigned int n;
  double w;
  /* precompute some ray information */
  for (n = 0; n < nrays; ++n, ++ray) {
    w = 2 * M_PI * GSL_CONST_MKSA_SPEED_OF_LIGHT * 1000 / ray->lambda;
    /* ray intensity for gaussian pulse */
    ray->intensity *= evaluator_evaluate_x(pfcn_intensity, w);
    if (isnan(ray->intensity)) {
      error_at_line(-1, errno, __FILE__, __LINE__,
                    "Error with intensity function: got nan.");
    }

    /* analytic apodization factor for parabola */
    ray->g *= evaluator_evaluate_x(pfcn_g, acos(ray->p0[2] / a->fs_radius));
    if (isnan(ray->g)) {
      error_at_line(-1, errno, __FILE__, __LINE__,
                    "Error with apodization function: got nan.");
    }

    /* all of the intensity information is stored in ray->g in order to
     * save a few multiplications in the inner loop */
    ray->g =
        (1.0 / (a->fs_radius * ray->lambda * 0.001)) * ray->g * ray->intensity;

    ray->e0[0] /= ray->lambda;
    ray->e0[1] /= ray->lambda;
    ray->e0[2] /= ray->lambda;
  }
  ray -= nrays;

  /* precompute the position vectors in the efield we'll be dealing with */
  if (rank == 0) {
    double dr[4];

    /* get the spacings of the electric field */
    for (i = 0; i < 4; ++i) {
      if (a->dim_efield_out[i] == 1) {
        dr[i] = (a->dim_efield_ub[i] - a->dim_efield_lb[i]) /
                ((double)a->dim_efield_out[i]);
      } else {
        dr[i] = (a->dim_efield_ub[i] - a->dim_efield_lb[i]) /
                ((double)a->dim_efield_out[i] - 1);
      }
    }

    /* set them */
    unsigned int r_i[4] = {};
    for (r_i[0] = 0; r_i[0] < a->dim_efield_out[0]; r_i[0]++) {
      for (r_i[1] = 0; r_i[1] < a->dim_efield_out[1]; r_i[1]++) {
        for (r_i[2] = 0; r_i[2] < a->dim_efield_out[2]; r_i[2]++) {
          for (r_i[3] = 0; r_i[3] < a->dim_efield_out[3]; r_i[3]++) {
            *efield_r++ = a->dim_efield_lb[0] + dr[0] * (double)r_i[0];
            *efield_r++ = a->dim_efield_lb[1] + dr[1] * (double)r_i[1];
            *efield_r++ = a->dim_efield_lb[2] + dr[2] * (double)r_i[2];
            *efield_r++ = a->dim_efield_lb[3] + dr[3] * (double)r_i[3];
          }
        }
      }
    }
    efield_r -= 4 * a->efield_ss_out;
  }

  /* send this spacing information out to everyone */
  MPI_Scatter(efield_r, 4 * (a->efield_ss_out / nprocs), MPI_DOUBLE,
              efield_r_rcv, 4 * (a->efield_ss_out / nprocs), MPI_DOUBLE, 0,
              MPI_COMM_WORLD);

  /* Main loop over ray data.  This should be fast as possible. */
  /* pre-computing the three efield_r_rcv lines doesn't appear to have a *
   * significant effect on performance */
  complex double tmp;
  for (i = 0; i < a->efield_ss_out / nprocs; ++i) {
    for (n = 0; n < nrays; ++n, ++ray) {
      tmp = -1.0i * ray->g *
            cexp(2.0i * M_PI *
                 ((*(efield_r_rcv + 0) - ray->p0[0]) * ray->e0[0] +
                  (*(efield_r_rcv + 1) - ray->p0[1]) * ray->e0[1] +
                  (*(efield_r_rcv + 2) - ray->p0[2]) * ray->e0[2] - ray->OPL -
                  *(efield_r_rcv + 3) * GSL_CONST_MKSA_SPEED_OF_LIGHT * 1000.0 /
                      ray->lambda +
                  a->temporal_offset * GSL_CONST_MKSA_SPEED_OF_LIGHT * 1000.0 /
                      ray->lambda));
      /* three polarization components */
      efield_x_rcv[i] += ray->pol[0] * tmp;
      efield_y_rcv[i] += ray->pol[1] * tmp;
      efield_z_rcv[i] += ray->pol[2] * tmp;
    }
    ray -= nrays;
    efield_r_rcv += 4;
  }
  efield_r_rcv -= 4 * i;

  /* gather all data from nodes */
  MPI_Gather(efield_x_rcv, a->efield_ss_out / nprocs, MPI_DOUBLE_COMPLEX,
             efield_x, a->efield_ss_out / nprocs, MPI_DOUBLE_COMPLEX, 0,
             MPI_COMM_WORLD);
  MPI_Gather(efield_y_rcv, a->efield_ss_out / nprocs, MPI_DOUBLE_COMPLEX,
             efield_y, a->efield_ss_out / nprocs, MPI_DOUBLE_COMPLEX, 0,
             MPI_COMM_WORLD);
  MPI_Gather(efield_z_rcv, a->efield_ss_out / nprocs, MPI_DOUBLE_COMPLEX,
             efield_z, a->efield_ss_out / nprocs, MPI_DOUBLE_COMPLEX, 0,
             MPI_COMM_WORLD);

  /* write electric field information to HDF file */
  if (rank == 0) {
    write_efield(a, efield_x, efield_y, efield_z);
  }

  /* clean up bit */
  free(efield_x);
  free(efield_y);
  free(efield_z);
  free(a);

  free(efield_x_rcv);
  free(efield_y_rcv);
  free(efield_z_rcv);
  free(efield_r_rcv);
  if (rank == 0) free(efield_r);
  free(ray);
  evaluator_destroy(pfcn_intensity);
  evaluator_destroy(pfcn_g);
  MPI_Finalize();
  return 0;
}
