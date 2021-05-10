#!/bin/bash
qsub -l nodes=16:ppn=4 circ_xy_main.sh
qsub -l nodes=16:ppn=4 circ_xy_bw.sh
qsub -l nodes=16:ppn=4 rect_xy_main.sh
qsub -l nodes=16:ppn=4 rect_xy_bw.sh
