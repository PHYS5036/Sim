#!/usr/bin/python3

import os
import subprocess
import sys

macroName = "runmacro.mac"

#Allowed types of particle
sourceTypes = ["Cs137","Na22","Co60","Am241"]

#nEvents
nEvents = "250000"

simdir = os.environ.get('SIM_DIR')

#Check python arguments
if(len(sys.argv)>=3):
    if any(source == sys.argv[1] for source in sourceTypes):
        sourceName = sys.argv[1]

        if(len(sys.argv)>=3):
            nEvents = sys.argv[2]
        
        #---------------------
        # Make file names
        #---------------------
        sourcePropName = "SodiumIodideMC/macros/"+sourceName+".txt"
        genName = "Gen_"+sourceName+".root"
        simName = "G4_" +sourceName+".root"
        pdfName = "G4_" +sourceName+".pdf"
        SpeName = "G4_" +sourceName+".Spe"
        bckName = "Bg_Sim.Spe"


        #---------------------
        # Run Gen code
        #---------------------
        subprocess.run([simdir+"/SodiumIodide/GenerateSpectrum.C+("+simdir+sourcePropName+","+genName+","+nEvents)])

        '''
        #---------------------
        # Write Macro file
        #---------------------
        
        
        if(len(sys.argv)>=5):
            tumourFlag = sys.argv[4]
        if(int(tumourFlag)==1):
            outName = outName + "Tumour_"
            
        if(len(sys.argv)>=6):
            physicsFlag = int(sys.argv[5])
        if(physicsFlag==1):    
            outName = outName + "Hadronic_"

        #Construct output file name
        outName = outName+"Out.csv\n"
        
        #Write macro file
        f = open(macroName, "w")

        f.write("/DoseProfileMC/analysis/setOutputFile " + outName)
        
        f.write("/DoseProfileMC/detector/TumourOn "+tumourFlag+"\n")
        f.write("/DoseProfileMC/detector/TumourRadius 0.5\n")
        f.write("/DoseProfileMC/detector/TumourHeight 0.0\n")
        
        f.write("/DoseProfileMC/physics/addPhysics "+physicsList[physicsFlag]+"\n")
        f.write("/run/initialize\n")

        f.write("/gps/particle "+particleDict[partName]+"\n")
        if(partName=="ion"):
            f.write("/gps/ion 6 12 6\n")
            
        f.write("/gps/energy "+sys.argv[2]+" MeV\n")
        f.write("/gps/direction 0 -1 0\n")
        f.write("/gps/pos/type Plane\n")
        f.write("/gps/pos/shape Circle\n")
        f.write("/gps/pos/radius 1 mm\n")
        f.write("/gps/pos/centre 0 1000 0 mm\n")
        f.write("/gps/pos/rot1 1 0 0\n")
        f.write("/gps/pos/rot2 0 0 1\n")
        f.write("/run/beamOn "+nEvents+"\n")
        f.close()


        #--------------------------
        # Run simulation
        #--------------------------
        subprocess.run(["./DoseProfileMC",macroName])


        os.remove(macroName)
'''


        
    else:
        print('WARNING, wrong input: please re-run as eg ./runsim.py [beam type] [energy] [N events] [Tumour 0/1] [Hadronic 0/1]')
        print('where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV')

    
else:
    print('WARNING, wrong input: please re-run as eg ./runsim.py [beam type] [energy]')
    print('where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV')


