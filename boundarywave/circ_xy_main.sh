#!/bin/bash
#PBS -N circ_xy_main
#PBS -o circ_xy_main.log
#PBS -l walltime=04:00:00

# go to proper location
cd $PBS_O_WORKDIR
export LD_LIBRARY_PATH=${WOODYHOME}/lib:/usr/lib64:$LD_LIBRARY_PATH
echo beginning job id $PBS_JOBID '('$PBS_JOBNAME')' on host $(hostname)

# get processor info
NPROCS=$(wc -l < $PBS_NODEFILE)
echo running on $NPROCS processors
RAYFILE="rays.txt"

#time `which mpirun` -np $NPROCS \
../directdebye \
  --efield-out=512:512:1:1 \
  --efield-lb=-0.010:-0.010:0.0159597:-53.333e-15 \
  --efield-ub=0.010:0.010:1:10 \
  --apodization-function='0.5*(1+cos(x))' \
  --pulse-function='sqrt(4e-15*sqrt(pi)/sqrt(ln2))*exp(-(x-2354564459136066.560)*(x-2354564459136066.560)*4e-15*4e-15*0.125/ln2)' \
  --temporal-offset=-8.5705411623394667231e-14 \
  --output-file="circ_xy_main.h5" \
  --radius=50 \
  $FASTTMP/$RAYFILE
