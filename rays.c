#include "rays.h"

/* compare doubles for qsort */
int compare_doubles(const void *a, const void *b) {
  const double *da = (const double *)a;
  const double *db = (const double *)b;
  return (*da > *db) - (*da < *db);
}

/* reads rays from a data file */
ray_t *readrays(char *filename, unsigned int *nrays) {
  tsv_t *tsv = tsv_fread(filename);
  *nrays = tsv->rows;

  /* should be 16 columns identical to the ray struct above */
  if (tsv->columns != 16) {
    error_at_line(-1, errno, __FILE__, __LINE__,
                  "%s: too few columns (got %d, expected 16)", filename,
                  tsv->columns);
  }

  ray_t *ray = (ray_t *)malloc(tsv->rows * sizeof(ray_t));

  /* because we have carefully crafted the input file to match the data
   * structure of ray_t, we can just memcpy to our hearts content */
  unsigned int i;
  for (i = 0; i < tsv->rows; i++) {
    memcpy(ray, *tsv->data, sizeof(ray_t));
    tsv->data++;
    ray++;
  }
  tsv->data -= i;
  ray -= i;

  tsv_free(tsv);

  return ray;
}

/* if the focus isn't at exactly x=y=z=0, move it based on the average ray
 * intersections with the optical axis */
void movefocus(ray_t *ray, unsigned int nrays) {
  double *point[3];
  double *px = (double *)malloc(nrays * sizeof(double));
  double *py = (double *)malloc(nrays * sizeof(double));
  double *pz = (double *)malloc(nrays * sizeof(double));

  point[0] = px; /* points in x,y,z */
  point[1] = py;
  point[2] = pz;
  unsigned int p_i[3] = {0}; /* how many points in x,y,z */

  /* set up random number generator */
  const gsl_rng_type *T;
  gsl_rng *r;
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc(T);

  /* seed both rngs */
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  gsl_rng_set(r, ts.tv_nsec);

  /* make a random order to pick rays from */
  unsigned int i, j;
  gsl_ran_shuffle(r, ray, nrays, sizeof(ray_t));

  /* This is hard to understand but works well. Sorry. */
  double s, t, div;
  unsigned int x, y;
  x = 0;
  y = 1;          /* start with x-y plane */
  while (x < 3) { /* iterate through x-y, y-z, and x-z planes */
    for (i = 0, j = nrays / 2; i < nrays / 2; i++, j++) {
      div = ray[i].e0[y] * ray[j].e0[x] - ray[i].e0[x] * ray[j].e0[y];
      if (div == 0) {
        continue;
      } /* lines are parallel */
      t = (ray[j].e0[x] * (ray[j].p0[y] - ray[i].p0[y]) +
           ray[j].e0[y] * (ray[i].p0[x] - ray[j].p0[x])) /
          div;
      s = (ray[i].e0[x] * (ray[j].p0[y] - ray[i].p0[y]) +
           ray[i].e0[y] * (ray[i].p0[x] - ray[j].p0[x])) /
          div;
      if (t * s == 0) {
        continue;
      } /* lines intersect before their starting points */
      // printf("%g %g\n",ray[i].p0[x]+t*ray[i].e0[x],
      // ray[i].p0[y]+t*ray[i].e0[y] ); printf("%g
      // %g\n",ray[j].p0[x]+s*ray[j].e0[x], ray[j].p0[y]+s*ray[j].e0[y] );
      point[x][p_i[x]++] = ray[i].p0[x] + t * ray[i].e0[x];
      point[y][p_i[y]++] = ray[i].p0[y] + t * ray[i].e0[y];
    }
    x++;
    y++;
    y %= 3;
  }

  /* sort the list and print the median of the values.  I've found that the
   * median works better than the mean in most cases.  Sometimes if the
   * focus was, say <0,0,0>, you'd get a bunch of 10^-15 values and a
   * couple of 10^10 for some reason.  That bigger value would throw
   * everything out of whack */
  qsort(point[0], p_i[0], sizeof(double), compare_doubles);
  qsort(point[1], p_i[1], sizeof(double), compare_doubles);
  qsort(point[2], p_i[2], sizeof(double), compare_doubles);

  double pp_x = point[0][p_i[0] / 2];
  double pp_y = point[1][p_i[1] / 2];
  double pp_z = point[2][p_i[2] / 2];

  /* go through the ray file and adjust the focus based on this calculation */
  for (i = 0; i < nrays; i++) {
    ray->p0[0] -= pp_x;
    ray->p0[1] -= pp_y;
    ray->p0[2] -= pp_z;
  }
}
