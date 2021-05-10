/* tsvread.c
 * reads data from a tab seperated values file and returns a struct
 * describing that data
 */

#include "tsvread.h"

/* read a tsv file given a filename */
tsv_t *tsv_fread(char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    error_at_line(-1, errno, __FILE__, __LINE__, "%s", filename);
  }
  char c;
  unsigned int rows = 0;
  unsigned int tabs = 0;
  unsigned int columns = 0;

  /* determine how many rows and columns are in the file */
  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') {
      rows++;
    }
    if (c == '\t') {
      tabs++;
    }
  }
  rewind(fp);
  columns =
      (tabs + rows) / rows; /* have to do this b/c there are columns-1 tabs */

  /* allocate memory for structure */
  tsv_t *tsv = malloc(sizeof(tsv_t));
  tsv->data = malloc(rows * sizeof(double *));
  unsigned int i, j, n;
  for (i = 0; i < rows; i++) {
    tsv->data[i] = malloc(columns * sizeof(double));
  }
  tsv->rows = rows;
  tsv->columns = columns;

  /* read data */
  char buf[128]; /* holds a string to be given to atof */
  bzero(buf, 128 * sizeof(char));
  i = 0;
  j = 0;
  n = 0;

  while ((c = fgetc(fp)) != EOF) {
    buf[n++] = c;
    if (c == '\t') { /* next column */
      buf[n] = '\0';
      tsv->data[i][j++] = atof(buf);
      n = 0;
      bzero(buf, 128 * sizeof(char));
    }
    if (c == '\n') { /* next row, go back to first column */
      buf[n] = '\0';
      tsv->data[i++][j] = atof(buf);
      j = 0;
      n = 0;
      bzero(buf, 128 * sizeof(char));
    }
  }
  fclose(fp);

  return tsv;
}

/* free the structure */
void tsv_free(tsv_t *tsv) {
  unsigned int i;
  for (i = 0; i < tsv->rows; i++) {
    free(tsv->data[i]);
  }
  free(tsv->data);
  free(tsv);
}
