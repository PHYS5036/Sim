#!/bin/bash

cp -n ~/.bashrc ~/bashrc.orig
cp /local/cern/Sim/bashrc ~/.bashrc

mkdir -p ~/data
cd ~/data
cp -n /local/cern/Notebooks/DoseProfileAnalysis.ipynb ./
cp -rn /local/cern/PHYS5036_Analysis ./

bash
