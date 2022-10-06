#!/bin/bash

export SIM_DIR=/local/cern/software/Sim

cp -n ~/.bashrc ~/bashrc.orig
cp $SIM_DIR/bashrc ~/.bashrc

mkdir -p ~/data
cd ~/data
cp -n /local/cern/software/Notebooks/DoseProfileAnalysis.ipynb ./

bash
