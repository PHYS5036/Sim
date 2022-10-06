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
if(len(sys.argv)>=2):
    if any(source == sys.argv[1] for source in sourceTypes):
        sourceName = sys.argv[1]

        if(len(sys.argv)>=3):
            nEvents = sys.argv[2]
        
        #---------------------
        # Make file names
        #---------------------
        sourcePropName = "/SodiumIodideMC/macros/"+sourceName+".txt"
        genName = "Gen_"+sourceName+".root"
        simName = "G4_" +sourceName+".root"
        pdfName = "G4_" +sourceName+".pdf"
        SpeName = "G4_" +sourceName+".Spe"
        bckName = "Bg_Sim.Spe"


        #---------------------
        # Run Gen code
        #---------------------
        subprocess.run(["root","-b","-q",simdir+"/SodiumIodideMC/GenerateSpectrum.C+(\""+simdir+sourcePropName+"\",\""+genName+"\","+nEvents+")"])
        
        
        #---------------------
        # Write Macro file
        #---------------------
        f = open(macroName, "w")
        f.write("/SodiumIodideMC/detector/AbsorberOn 0 \n")
        f.write("/SodiumIodideMC/detector/AbsorberMaterial G4_AIR \n")
        f.write("/SodiumIodideMC/detector/AbsorberThickness 10.0 \n")
        f.write("/SodiumIodideMC/physics/SetEmModel Standard \n")
        f.write("/SodiumIodideMC/physics/SetEmProcess All \n")
        f.write("/run/initialize \n")

        f.write("/SodiumIodideMC/generator/Mode 1 \n")
        f.write("/SodiumIodideMC/generator/InputFile " + genName +" \n")
        
        f.write("/SodiumIodideMC/analysis/setOutputFile " + simName+" \n")
        f.close()
        
        #---------------------
        # Run Sim code
        #---------------------
        subprocess.run(["SodiumIodideMC",macroName])

        os.remove(macroName)

        #---------------------
        # Run Smear code
        #---------------------
        subprocess.run(["root","-b","-q",simdir+"/SodiumIodideMC/SmearSpectrum.C+(\""+simName+"\")"])
        
        os.remove(genName)
        os.remove(simName)
        

        
    else:
        print('WARNING, wrong input: please re-run as eg ./runSodiumIodide.py [source] [N events]')
        print('where [source] is either Cs137,Na22,Co60,Am241')

    
else:
    print('WARNING, wrong input: please re-run as eg ./runSodiumIodide.py [source] [N events]')
    print('where [source] is either Cs137,Na22,Co60,Am241')
    

