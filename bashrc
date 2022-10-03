# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
        . /etc/bashrc
fi

# User specific aliases and functions
source /local/cern/software/root/root-6.22.08-install/bin/thisroot.sh
source /local/cern/software/geant4/geant4.10.07.p02-install/bin/geant4.sh

export PATH=/local/cern/software/Sim/bin/:$PATH:./:$ROOTSYS/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./:$ROOTSYS/lib

alias l='ls -lhtr'
