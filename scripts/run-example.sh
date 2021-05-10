#!/bin/bash
#
# run-example.sh
# 
# Runs directdebye for different timesteps and submits them as jobs to
# torque.

# ray file
RAYFILE="rrays.txt"

# generate different files for submission
J=0
for I in $(linspace -6.671281905e-13 6.671281905e-13 7)
do
	touch `printf %.4d $J`-parabola-qsub.sh 
	cat << EOF > `printf %.4d $J`-parabola-qsub.sh
#!/bin/bash
#PBS -N directdebye-parabola_$J
#PBS -o parabola_$J.log
#PBS -l walltime=00:30:00

# go to proper location
cd \$PBS_O_WORKDIR
export LD_LIBRARY_PATH=\${WOODYHOME}/lib:/usr/lib64:\$LD_LIBRARY_PATH
echo beginning job id \$PBS_JOBID '('\$PBS_JOBNAME')' on host \`hostname\`

# get processor info
NPROCS=\`wc -l < \$PBS_NODEFILE\`;
echo running on \$NPROCS processors

time /usr/bin/mpirun -v -machinefile \$PBS_NODEFILE -np \$NPROCS \\
./directdebye \\
--efield-out=1:50:100:1 \\
--efield-lb=0:-0.1:-0.2:$I \\
--efield-ub=1:0.1:0.2:10 \\
--radius=50.0 \\
--temporal-offset=3.9928428404523743911e-14 \\
--output-file="`printf %.4d $J`-parabola-$I.h5" \\
--pulse-function='"sqrt(40e-15*sqrt(pi)/sqrt(ln2))*exp(-(x-2354564459136066)*(x-2354564459136066)*40e-15*40e-15*0.125/ln2)"' \\
--apodization-function='"0.5*(1+cos(x))"' \\
$FASTTMP/$RAYFILE

EOF
	let J=$J+1
done

# want to make sure $RAYFILE is in fasttmp
if [ -e $FASTTMP/$RAYFILE ]
then
	echo "$FASTTMP/$RAYFILE already exists"
else
	cp $RAYFILE $FASTTMP/$RAYFILE
	echo "moved $RAYFILE to $FASTTMP"
fi


# submit
for FILE in *parabola-qsub.sh
do
	qsub -l nodes=16:ppn=4 $FILE
done
