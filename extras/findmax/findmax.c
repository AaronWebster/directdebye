#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <complex.h>
#include <stdlib.h>
#include <gsl/gsl_const_mksa.h>
#include <assert.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <nlopt.h>
#include "tsvread.h"

typedef struct {
	gsl_spline *spline;
	gsl_interp_accel *acc;
	int flags;
	double d;
} f_param_t;

double opt_func(unsigned n, const double *x, double *grad, void *f_data){
	f_param_t *fparam = (f_param_t*)f_data;
	gsl_spline *spline_nu = fparam->spline;
	gsl_interp_accel *acc_nu = fparam->acc;
	int flags = fparam->flags;
	double d = fparam->d;
	return gsl_spline_eval(spline_nu,x[0],acc_nu);
}

int main(int argc, char **argv) {
	/* open file for reading data */
	tsv_t *tsv = tsv_fread(argv[1]);

	if(tsv->columns!=2){
		fprintf(stderr,"input file must be 2 colums tab seperated\n");
		return -1;
	}

	double *x_nu = (double*)malloc(tsv->rows*sizeof(double));
	double *y_nu = (double*)malloc(tsv->rows*sizeof(double));
	assert(x_nu!=NULL);
	assert(y_nu!=NULL);

	unsigned int i;
	for(i=0;i<tsv->rows;i++){
		x_nu[i]=tsv->data[i][0];
		y_nu[i]=tsv->data[i][1];
	}

	gsl_interp_accel *acc_nu = gsl_interp_accel_alloc();
	gsl_spline *spline_nu = gsl_spline_alloc (gsl_interp_cspline, tsv->rows);
	gsl_spline_init (spline_nu, x_nu, y_nu, tsv->rows);

	double x[1]; /* initial guess */
	double lb[1]; /* lower bound */
	double ub[1]; /* upper bound */
	double minf = 0;
	lb[0]=x_nu[0];
	ub[0]=x_nu[tsv->rows-1];
	x[0]=x_nu[tsv->rows/2];

	f_param_t *fparam = (f_param_t*)malloc(1*sizeof(f_param_t));
	fparam->spline = spline_nu;
	fparam->acc = acc_nu;
	fparam->flags=0;
	void *fvparam;
	fvparam=fparam;

	nlopt_opt opt;
	opt = nlopt_create(NLOPT_LN_NELDERMEAD, 1); 
	nlopt_set_lower_bounds(opt, lb);
	nlopt_set_upper_bounds(opt, ub);
	nlopt_set_max_objective(opt, opt_func, fvparam);
	nlopt_set_xtol_rel(opt, 1e-16);
	//nlopt_set_ftol_abs(opt,1e-6);
	//nlopt_set_maxtime(opt,4);

	int err;
	double max_nu;
	err=nlopt_optimize(opt, x, &minf);
	if(err < 0) {
		fprintf(stderr,"nlopt failed! %d\n",err);
		fprintf(stderr,"lower: %g\n",lb[0]);
		fprintf(stderr,"upper: %g\n",ub[0]);
		fprintf(stderr,"guess: %g\n",x[0]);
		if(lb[0]>ub[0]){ fprintf(stderr,"lower bigger than upper\n"); }
	}
	else{
		max_nu = gsl_spline_eval(spline_nu,x[0],acc_nu);
		printf("Maximum: %.20g %.20g \n", x[0], max_nu);
	}
	
	nlopt_destroy(opt);
	tsv_free(tsv);
	free(x_nu);
	free(y_nu);

	return 0;
}
