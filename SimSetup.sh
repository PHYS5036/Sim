#!/bin/bash

cp -n ~/.bashrc ~/bashrc.orig
cp /local/cern/software/Sim/bashrc ~/.bashrc

mkdir -p ~/data
cd ~/data
cp -n /local/cern/software/Notebooks/DoseProfileAnalysis.ipynb ./

bash
