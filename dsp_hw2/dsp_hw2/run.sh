#!/bin/bash

states=$1
gaussians=$2

acc=result/accuracy


# State number: 5 ~ $state
for state in `seq 5 $states`
do

    # Initial the HMM
    python3 proto_make.py $state

    # Gaussian number: 2 ~ $gaussians
    for gaussian in `seq 2 $gaussians`
    do
        # Gaussian number modify
        python3 gaussian_make.py $gaussian

        echo "state:" $state >> $acc
        echo "gaussian:" $gaussian >> $acc
        
        bash 00_clean_all.sh
        bash 01_run_HCopy.sh
        bash 02_run_HCompV.sh
        bash 03_training.sh
        bash 04_testing.sh
        
    done 

done
