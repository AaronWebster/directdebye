#!/bin/bash
#PBS -N directdebye-parabola_findoffset
#PBS -o parabola_findoffset.log
#PBS -l walltime=00:30:00

# go to proper location
cd $PBS_O_WORKDIR
export LD_LIBRARY_PATH=${WOODYHOME}/lib:/usr/lib64:$LD_LIBRARY_PATH
echo beginning job id $PBS_JOBID '('$PBS_JOBNAME')' on host `hostname`

# get processor info
NPROCS=`wc -l < $PBS_NODEFILE`;
echo running on $NPROCS processors
RAYFILE="rays.txt"

time /usr/bin/mpirun -v -machinefile $PBS_NODEFILE -np $NPROCS \
./directdebye \
--efield-out=1:1:1:512 \
--efield-lb=0:0.0:0.0:-6.671281905e-13 \
--efield-ub=1:0.1:0.2:6.671281905e-13 \
--temporal-offset=0 \
--radius=5.8250765955586 \
--output-file="findoffset.h5" \
$FASTTMP/$RAYFILE
