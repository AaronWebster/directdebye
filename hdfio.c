// Functions for (reading) and writing HDF files.
#include "hdfio.h"

#include "parseargs.h"

// Writes electric field data to an hdf file.
void write_efield(args_t *a, complex double *efield_x, complex double *efield_y,
                  complex double *efield_z) {
  unsigned int i;
  double *efield_out = NULL;
  efield_out = (double *)malloc(a->efield_ss_out * sizeof(complex double));

  // Stuff for hdf output.
  hid_t file, dataset, dataspace;
  herr_t status;
  hsize_t dims[4];  // Dimensionality of the set.

  // Create file.
  file = H5Fcreate(a->filename_out, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  // Dimensionality of the set.
  dims[0] = a->dim_efield_out[0];
  dims[1] = a->dim_efield_out[1];
  dims[2] = a->dim_efield_out[2];
  dims[3] = a->dim_efield_out[3];

  // Create dataset.
  dataspace = H5Screate_simple(4, dims, NULL);

  // Output various parts of the electric field, depending on user options.

  // |E|^2
  if (a->output_e2) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cabs(efield_x[i]) * cabs(efield_x[i]) +
                      cabs(efield_y[i]) * cabs(efield_y[i]) +
                      cabs(efield_z[i]) * cabs(efield_z[i]);
    }
    dataset =
        H5Dcreate1(file, "/e2", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    /* phase */
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = atan2(creal(efield_x[i] + efield_y[i] + efield_z[i]),
                            cimag(efield_x[i] + efield_y[i] + efield_z[i]));
    }
    dataset = H5Dcreate1(file, "/e2_phase", H5T_NATIVE_DOUBLE, dataspace,
                         H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
  }

  // |Ex|^2
  if (a->output_ex2) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cabs(efield_x[i]) * cabs(efield_x[i]);
    }
    dataset =
        H5Dcreate1(file, "/ex2", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = atan2(creal(efield_x[i]), cimag(efield_x[i]));
    }
    dataset = H5Dcreate1(file, "/ex_phase", H5T_NATIVE_DOUBLE, dataspace,
                         H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
  }

  // |Ey|^2
  if (a->output_ey2) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cabs(efield_y[i]) * cabs(efield_y[i]);
    }
    dataset =
        H5Dcreate1(file, "/ey2", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = atan2(creal(efield_y[i]), cimag(efield_y[i]));
    }
    dataset = H5Dcreate1(file, "/ey_phase", H5T_NATIVE_DOUBLE, dataspace,
                         H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
  }

  // |Ez|^2
  if (a->output_ez2) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cabs(efield_z[i]) * cabs(efield_z[i]);
    }
    dataset =
        H5Dcreate1(file, "/ez2", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = atan2(creal(efield_z[i]), cimag(efield_z[i]));
    }
    dataset = H5Dcreate1(file, "/ez_phase", H5T_NATIVE_DOUBLE, dataspace,
                         H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
  }
  // Ex + iEx
  if (a->output_ex) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = creal(efield_x[i]);
    }
    dataset =
        H5Dcreate1(file, "/ex.r", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cimag(efield_x[i]);
    }
    dataset =
        H5Dcreate1(file, "/ex.i", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    if (!a->output_ex2) {
      for (i = 0; i < a->efield_ss_out; ++i) {
        efield_out[i] = atan2(creal(efield_x[i]), cimag(efield_x[i]));
      }
      dataset = H5Dcreate1(file, "/ex_phase", H5T_NATIVE_DOUBLE, dataspace,
                           H5P_DEFAULT);
      status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                        H5P_DEFAULT, efield_out);
      status = H5Dclose(dataset);
    }
  }

  // Ey + iEy
  if (a->output_ey) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = creal(efield_y[i]);
    }
    dataset =
        H5Dcreate1(file, "/ey.r", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cimag(efield_y[i]);
    }
    dataset =
        H5Dcreate1(file, "/ey.i", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    if (!a->output_ey2) {
      for (i = 0; i < a->efield_ss_out; ++i) {
        efield_out[i] = atan2(creal(efield_y[i]), cimag(efield_y[i]));
      }
      dataset = H5Dcreate1(file, "/ey_phase", H5T_NATIVE_DOUBLE, dataspace,
                           H5P_DEFAULT);
      status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                        H5P_DEFAULT, efield_out);
      status = H5Dclose(dataset);
    }
  }

  // Ez + iEz
  if (a->output_ez) {
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = creal(efield_z[i]);
    }
    dataset =
        H5Dcreate1(file, "/ez.r", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);
    for (i = 0; i < a->efield_ss_out; ++i) {
      efield_out[i] = cimag(efield_z[i]);
    }
    dataset =
        H5Dcreate1(file, "/ez.i", H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      efield_out);
    status = H5Dclose(dataset);

    if (!a->output_ez2) {
      for (i = 0; i < a->efield_ss_out; ++i) {
        efield_out[i] = atan2(creal(efield_z[i]), cimag(efield_z[i]));
      }
      dataset = H5Dcreate1(file, "/ez_phase", H5T_NATIVE_DOUBLE, dataspace,
                           H5P_DEFAULT);
      status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                        H5P_DEFAULT, efield_out);
      status = H5Dclose(dataset);
    }
  }

  // Now that we've written all the actual data, write some parameters in case
  // we forget what this file contains first, the upper and lower bounds of the
  // dataset.
  dims[0] = 4;
  dims[1] = 1;
  dims[2] = 1;
  dims[3] = 1;
  status = H5Sclose(dataspace);
  dataspace = H5Screate_simple(1, dims, NULL);
  dataset = H5Dcreate1(file, "/efield_upper_bounds", H5T_NATIVE_DOUBLE,
                       dataspace, H5P_DEFAULT);
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    a->dim_efield_ub);
  status = H5Dclose(dataset);
  dataset = H5Dcreate1(file, "/efield_lower_bounds", H5T_NATIVE_DOUBLE,
                       dataspace, H5P_DEFAULT);
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    a->dim_efield_lb);
  status = H5Dclose(dataset);
  status = H5Sclose(dataspace);

  // Temporal offset.
  dims[0] = 1;
  dims[1] = 1;
  dims[2] = 1;
  dims[3] = 1;
  dataspace = H5Screate_simple(1, dims, NULL);
  dataset = H5Dcreate1(file, "/temporal_offset", H5T_NATIVE_DOUBLE, dataspace,
                       H5P_DEFAULT);
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    &a->temporal_offset);
  status = H5Dclose(dataset);

  // Clean up.
  fprintf(stderr, "wrote %s\n", a->filename_out);
  status = H5Fclose(file);

  free(efield_out);
}
