#include <stdio.h>   /* gets */
#include <stdlib.h>  /* atoi, malloc */
#include <string.h>  /* strcpy */
#include <malloc.h>
#include <error.h>
#include <errno.h>

/* holds the data we read from the file */
typedef struct {
	double **data; /* two dimensional array of data */
	unsigned int rows;
	unsigned int columns;
} tsv_t;

void tsv_free(tsv_t *tsv);
tsv_t *tsv_fread(char *filename);
