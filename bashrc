# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
        . /etc/bashrc
fi

# User specific aliases and functions
source /local/cern/software/root/bin/thisroot.sh # /cern/root/v6.22.03/bin/thisroot.sh
source /local/cern/software/geant4/geant4-v11.2.2-install/bin/geant4.sh # /cern/geant4/v11.2.2-install/bin/geant4.sh

# Project dir is the name of the GitHub repo containing the lab scripts and configs.
PROJECT_DIR=~/PHYS_5036-labs
export PROJECT_DIR=$PROJECT_DIR
export OUTPUT_DIR=$PROJECT_DIR/outputs
export CONFIG_PATH=$PROJECT_DIR/config.json

export PATH=/local/cern/Sim/bin/:$PATH:./:$ROOTSYS/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./:$ROOTSYS/lib
export SIM_DIR=/local/cern/Sim

alias l='ls -lhtr'
