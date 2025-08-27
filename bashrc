# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
        . /etc/bashrc
fi

# User specific aliases and functions
source /local/cern/software/root/bin/thisroot.sh
source /local/cern/software/geant4/geant4-v11.2.2-install/bin/geant4.sh

export PATH=/local/cern/Sim/bin/:$PATH:./:$ROOTSYS/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./:$ROOTSYS/lib
export SIM_DIR=/local/cern/Sim

alias l='ls -lhtr'
