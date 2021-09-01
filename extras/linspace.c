#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int Usage() {
  fprintf(stderr, "Usage: linspace <min> <max> <samples>\n");
  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  if (argc < 3) return Usage();

  double lb;
  double ub;
  int n;
  if (sscanf(argv[1], "%lg", &lb) < 1) return Usage();
  if (sscanf(argv[2], "%lg", &ub) < 1) return Usage();
  if (sscanf(argv[3], "%d", &n) < 1) return Usage();

  double dn = (n == 1) ? (ub - lb) / (double)(n) : (ub - lb) / (double)(n - 1);

  int i;
  for (i = 0; i < n; i++) {
    printf("%.20g\n", lb + dn * (double)i);
  }

  return EXIT_SUCCESS;
}
