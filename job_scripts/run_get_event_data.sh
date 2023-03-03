#!/bin/bash
###SBATCH --qos=regular
###SBATCH --time=24:00:00
###SBATCH --nodes=4
###SBATCH --constraint=haswell
###SBATCH -o /global/homes/v/vsharma2/test_output.o
###SBATCH -e /global/homes/v/vsharma2/test_error.e
###SBATCH -J test

cd ../macros/

for i in {3601..3615}
do
    ./GetEventCounts $i
done
