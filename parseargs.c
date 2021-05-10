/* parseargs.c
 * parse command line arguments
 */
#include "parseargs.h"

#include "miscprint.h"

/* parse command line arguments.  Return a properly set up args_t */
void parse_args(int argc, char **argv, args_t *a) {
  /* Since the pulse and apod function may or may not be set, default to
   * return just 1.0.  That way it can work if the ray file is set or not. */
  bzero(a->pfcn_g_buffer, BUFFER_SIZE * sizeof(char));
  bzero(a->pfcn_intensity_buffer, BUFFER_SIZE * sizeof(char));
  sprintf(a->pfcn_g_buffer, "1.0");
  sprintf(a->pfcn_intensity_buffer, "1.0");

  errno = 0;
  int c, er;
  /* clear out args just to make sure */
  bzero(a, sizeof(args_t));
  for (;;) {
    struct option long_options[] = {
        {"output-ex", no_argument, &a->output_ex, 1},
        {"output-ey", no_argument, &a->output_ey, 1},
        {"output-ez", no_argument, &a->output_ez, 1},
        {"output-ex2", no_argument, &a->output_ex2, 1},
        {"output-ey2", no_argument, &a->output_ey2, 1},
        {"output-ez2", no_argument, &a->output_ez2, 1},
        {"output-e2", no_argument, &a->output_e2, 1},
        {"efield-out", optional_argument, 0, 'o'},
        {"efield-ub", optional_argument, 0, 'u'},
        {"efield-lb", optional_argument, 0, 'l'},
        {"output-file", optional_argument, 0, 'f'},
        {"pulse-function", optional_argument, 0, 'p'},
        {"apodization-function", optional_argument, 0, 'g'},
        {"wavelength", optional_argument, 0, 'w'},
        {"temporal-offset", optional_argument, 0, 't'},
        {"radius", optional_argument, 0, 'r'},
        {0, 0, 0, 0}};
    int option_index = 0;
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) {
      break;
    }            /* end of options */
    switch (c) { /* actually do the parsing of options */
      case 'o':  /* efield size in pixels */
        er = sscanf(optarg, "%d:%d:%d:%d", &a->dim_efield_out[0],
                    &a->dim_efield_out[1], &a->dim_efield_out[2],
                    &a->dim_efield_out[3]);
        /* only specify x,y,z then assume one time step */
        if (er < 4) {
          a->dim_efield_out[3] = 1;
        }
        /* not enough arguments */
        if (er < 3) {
          if (!errno) {
            errno = EINVAL;
          }
          print_usage();
          error_at_line(-1, errno, __FILE__, __LINE__,
                        "Incorrect specification of --efield-out.  Use three "
                        "or four values seperated by colons x:y:z:t\n");
        }
        break;
      case 'u': /* efield upper bounds, in millimeters */
        er = sscanf(optarg, "%lg:%lg:%lg:%lg", &a->dim_efield_ub[0],
                    &a->dim_efield_ub[1], &a->dim_efield_ub[2],
                    &a->dim_efield_ub[3]);
        if (er < 4) {
          if (!errno) {
            errno = EINVAL;
          }
          error_at_line(
              -1, errno, __FILE__, __LINE__,
              "Incorrect specification of --efield-lb.  Use four values, in "
              "millimeters, seperated by colons lbx:lby:lbz:lbt\n");
        }
        break;
      case 'l': /* efield lower bounds, in millimeters */
        er = sscanf(optarg, "%lg:%lg:%lg:%lg", &a->dim_efield_lb[0],
                    &a->dim_efield_lb[1], &a->dim_efield_lb[2],
                    &a->dim_efield_lb[3]);
        if (er < 4) {
          if (!errno) {
            errno = EINVAL;
          }
          error_at_line(
              -1, errno, __FILE__, __LINE__,
              " Incorrect specification of --efield-ub.  Use four values, in "
              "millimeters, seperated by colons lbx:lby:lbz:lbt\n");
        }
        break;
      case 'f': /* output filename */
        bzero(a->filename_out, FILENAME_MAX * sizeof(char));
        sprintf(a->filename_out, "%s", optarg);
        if (errno) {
          error_at_line(-1, errno, __FILE__, __LINE__,
                        "Error setting output filename.\n");
        }
        break;
      case 'p': /* pulse funciton */
        bzero(a->pfcn_intensity_buffer, BUFFER_SIZE * sizeof(char));
        sprintf(a->pfcn_intensity_buffer, "%s", optarg);
        if (errno) {
          error_at_line(-1, errno, __FILE__, __LINE__,
                        "Error setting pulse function.\n");
        }
        break;
      case 'g': /* apodization funciton */
        bzero(a->pfcn_g_buffer, BUFFER_SIZE * sizeof(char));
        sprintf(a->pfcn_g_buffer, "%s", optarg);
        if (errno) {
          error_at_line(-1, errno, __FILE__, __LINE__,
                        "Error setting apodization function.\n");
        }
        break;

      case 't': /* temporal offset */
        er = sscanf(optarg, "%lg", &a->temporal_offset);
        if (er < 1) {
          if (!errno) {
            errno = EINVAL;
          }
          error_at_line(-1, errno, __FILE__, __LINE__,
                        "Wrong format for temporal offset.\n");
        }
        break;
      case 'r': /* focal sphere radius */
        er = sscanf(optarg, "%lg", &a->fs_radius);
        if (er < 1) {
          if (!errno) {
            errno = EINVAL;
          }
          error_at_line(-1, errno, __FILE__, __LINE__,
                        "Wrong format for focal sphere radius\n");
        }
        break;
      default:
        break;
    }
  }
  /* last argument is the ray filename */
  if (optind - 1 < argc) {
    if (argv[optind] == NULL) {
      print_usage();
      error_at_line(-1, errno, __FILE__, __LINE__, "Missing filename.\n");
    } else { /* set the input ray filename */
      memset(&a->filename_in, 0, FILENAME_MAX * sizeof(char));
      sprintf(a->filename_in, "%s", argv[optind]);
      if (errno) {
        error_at_line(-1, errno, __FILE__, __LINE__,
                      "Could not set input filename.\n");
      }
    }
  }
  /* If no electric field components are selected for output, default to
   * output everything. */
  if (!a->output_ex && !a->output_ey && !a->output_ez && !a->output_ex2 &&
      !a->output_ey2 && !a->output_ez2 && !a->output_e2) {
    a->output_ex = 1;
    a->output_ey = 1;
    a->output_ez = 1;
    a->output_ex2 = 1;
    a->output_ey2 = 1;
    a->output_ez2 = 1;
    a->output_e2 = 1;
  }
  /* if the focal sphere radius wasn't set, make it unity and print a flag
   * asserting the output is only qualatative */
  if (a->fs_radius == 0) {
    // qualatative=1;
    a->fs_radius = 1;
  }

  /* the output file name */
  if (a->filename_out == NULL) {
    bzero(a->filename_out, FILENAME_MAX * sizeof(char));
    sprintf(a->filename_out, "%s", a->filename_in);
  }
}
