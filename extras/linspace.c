#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* command line arguments, get memory */
#include <getopt.h> /* command line arguments */

int main(int argc, char **argv) {
	if(argc<3){
		fprintf(stderr,"Usage: linspace <min> <max> <samples>\n");
		return 0;
	}

	double lb,ub;
	unsigned int n;

	int er;

	er = sscanf(argv[1],"%lg",&lb);
	if(er<1){
		fprintf(stderr,"Usage: linspace <min> <max> <samples>\n");
		return 0;
	}
	er = sscanf(argv[2],"%lg",&ub);
	if(er<1){
		fprintf(stderr,"Usage: linspace <min> <max> <samples>\n");
		return 0;
	}
	er = sscanf(argv[3],"%d",&n);
	if(er<1){
		fprintf(stderr,"Usage: linspace <min> <max> <samples>\n");
		return 0;
	}

	double dn;
	if(n==1){
		dn = (ub-lb)/(double)(n);
	}
	else{
		dn = (ub-lb)/(double)(n-1);
	}

	unsigned int i;
	for(i=0;i<n;i++){
		printf("%.20g\n",lb+dn*(double)i);
	}
	return 0;
}
