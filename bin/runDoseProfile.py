#!/usr/bin/python3

import os
import subprocess
import sys

# Determine paths are set.
project_dir = os.environ.get('PROJECT_DIR')
if not project_dir:
    print("Error: PROJECT_DIR environment variable is not set. Make sure to source bash script.")
    sys.exit(1)

output_dir = os.environ.get('OUTPUT_DIR')
if not output_dir:
    print("Error: OUTPUT_DIR environment variable is not set. Make sure to source bash script.")
    sys.exit(1)
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

macroName = "runmacro.mac"

# Allowed types of particle
particleTypes = ["gamma","electron","proton","ion"]
particleDict  = {"gamma":"gamma", "electron":"e-", "proton":"proton", "ion":"ion"}

# Physics flags
physicsList  = ['standard_opt3','QGSP_BIC_EMY']
physicsFlag  = 0

# Detector flags
tumourFlag   = "0"

# Number of Events
nEvents = "10000"

# Check python arguments
if(len(sys.argv)>=3):
    if any(particle == sys.argv[1] for particle in particleTypes):
        partName = sys.argv[1]
        energy   = sys.argv[2]
        seed     = "1"

        #---------------------
        # Write Macro file
        #---------------------
        outName = "DoseMC_"+partName+"_"+energy+"MeV_"

        if(len(sys.argv)>=4):
            nEvents = sys.argv[3]

        outName = outName+nEvents+"n_"
        
        if(len(sys.argv)>=5):
            tumourFlag = sys.argv[4]
        if(int(tumourFlag)==1):
            outName = outName + "Tumour_"
            
        if(len(sys.argv)>=6):
            physicsFlag = int(sys.argv[5])
        if(physicsFlag==1):    
            outName = outName + "Hadronic_"
            
        if(len(sys.argv)>7):
            seed = sys.argv[6]
            outName = outName + "Seed-" + seed + "_"

        # Construct output file name
        outName = outName+"Out.csv\n"
        
        # Write macro file
        f = open(macroName, "w")

        f.write("/DoseProfileMC/analysis/setOutputFile " + outName)
        
        f.write("/DoseProfileMC/detector/TumourOn "+tumourFlag+"\n")
        f.write("/DoseProfileMC/detector/TumourRadius 0.5\n")
        f.write("/DoseProfileMC/detector/TumourHeight 0.0\n")
        
        f.write("/DoseProfileMC/physics/addPhysics "+physicsList[physicsFlag]+"\n")

        f.write("/DoseProfileMC/generator/Seed "+seed+"\n")
        
        f.write("/run/initialize\n")

        f.write("/gps/particle "+particleDict[partName]+"\n")
        if(partName=="ion"):
            f.write("/gps/ion 6 12 6\n")
            
        
        f.write("/gps/energy "+sys.argv[2]+" MeV\n")
        f.write("/gps/direction 0 -1 0\n")
        f.write("/gps/pos/type Plane\n")
        f.write("/gps/pos/shape Circle\n")
        f.write("/gps/pos/radius 5 mm\n")
        f.write("/gps/pos/centre 0 1000 0 mm\n")
        f.write("/gps/pos/rot1 1 0 0\n")
        f.write("/gps/pos/rot2 0 0 1\n")
        f.write("/run/beamOn "+nEvents+"\n")
        f.close()


        #--------------------------
        # Run simulation
        #--------------------------
        subprocess.run(["DoseProfileMC",macroName])


        os.remove(macroName)



        
    else:
        print('WARNING, wrong input: please re-run as eg ./runsim.py [beam type] [energy] [N events] [Tumour 0/1] [Hadronic 0/1] [Seed int]')
        print('where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV')

    
else:
    print('WARNING, wrong input: please re-run as eg ./runsim.py [beam type] [energy]')
    print('where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV')


