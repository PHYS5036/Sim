#!/bin/bash

mkdir -p ~/data

cp -n ~/.bashrc ~/bashrc.orig
cp /local/cern/software/Sim/bashrc ~/.bashrc

mkdir ~/data
cd ~/data
cp -n /local/cern/software/Notebooks/DoseProfileAnalysis.ipynb ./

bash
