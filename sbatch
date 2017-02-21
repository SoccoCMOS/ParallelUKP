#!/bin/bash
#SBATCH -n3
/lib64/openmpi/bin/mpirun -np 3 -host star2,star3,star4 /home/mpiuser/executables/Algo_Gen/AG
