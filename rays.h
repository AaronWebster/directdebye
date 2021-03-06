#ifndef RAYS_H
#define RAYS_H

#include <complex.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "tsvread.h"

// A ray.
typedef struct {
  double p0[3];           // position
  double e0[3];           // direction
  complex double pol[3];  // polarization
  double OPL;             // optical path length
  double lambda;          // wavelength
  double intensity;       // intensity of this paricular ray
  double g;               // apodisation factor
} ray_t;

ray_t *readrays(char *filename, unsigned int *nrays);
void movefocus(ray_t *ray, unsigned int nrays);

#endif  // RAYS_H
