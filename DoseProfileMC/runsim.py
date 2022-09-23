#!/usr/bin/python

import os
import subprocess
import sys

particleTypes = ["gamma","proton","ion"]

if(len(sys.argv)==3):
    if any(particle == sys.argv[1] for particle in particleTypes):
        float(sys.argv[2])
        
    else:
        print('WARNING, wrong input: please re-run as eg ./runsim.sh [beam type] [energy]')
        print('where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV')
else:
    print('WARNING, wrong input: please re-run as eg ./runsim.sh [beam type] [energy]')
    print('where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV')


