#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <stdio.h>  /* gets */
#include <stdlib.h> /* atoi, malloc */
#include <string.h> /* strcpy */

/* holds the data we read from the file */
typedef struct {
  double **data; /* two dimensional array of data */
  unsigned int rows;
  unsigned int columns;
} tsv_t;

void tsv_free(tsv_t *tsv);
tsv_t *tsv_fread(char *filename);
