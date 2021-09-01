#ifndef TSVREAD_H
#define TSVREAD_H

#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* holds the data we read from the file */
typedef struct {
  double **data; /* two dimensional array of data */
  unsigned int rows;
  unsigned int columns;
} tsv_t;

void tsv_free(tsv_t *tsv);
tsv_t *tsv_fread(char *filename);

#endif  // TSVREAD_H
