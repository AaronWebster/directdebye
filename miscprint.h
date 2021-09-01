#ifndef MISCPRINT_H
#define MISCPRINT_H

#include <stdio.h>
#include <stdlib.h>

#include "parseargs.h"

// Print usage information.
void print_usage(void);

// Prints command line arguments.
void print_args(args_t *a, unsigned int nrays);

#endif  // MISCPRINT_H
