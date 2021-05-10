#include <stdio.h>
#include <stdlib.h>

#include "parseargs.h"

#ifndef INC_MISCPRINT_H
#define INC_MISCPRINT_H
/* print usage information */
void print_usage(void);

/* print command line arguments */
void print_args(args_t *a, unsigned int nrays);
#endif
