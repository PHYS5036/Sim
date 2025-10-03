#!/usr/bin/python3

import os
import subprocess
import sys
import json

macroName = "runmacro.mac"

# Allowed types of particle and settings
allowedSourceTypes = ["Cs137", "Na22", "Co57", "Co60", "Am241"]
allowedDetectorTypes = ["0", "1"] # 0 for GAGG, 1 for NaI
allowedSourceHolderTypes = ["0", "1"] # 0 for 3D printed, 1 for Lead with hole

# Determine config file path
simdir = os.environ.get('SIM_DIR')
if not simdir:
    print("Error: SIM_DIR environment variable is not set.")
    sys.exit(1)
    
default_config_path = os.path.join(simdir, 'bin', 'defaultconfig.json')
config_file_path = sys.argv[1] if len(sys.argv) >= 2 and sys.argv[1].endswith('.json') else None

# Load configuration from file
if config_file_path:
    # User provided a config file path
    if os.path.isabs(config_file_path) and os.path.exists(config_file_path):
        try:
            with open(config_file_path) as config_file:
                config = json.load(config_file)
            print(f"Using config file: {config_file_path}")
        except json.JSONDecodeError:
            print(f"Error: {config_file_path} is not a valid JSON file.")
            sys.exit(1)
    else:
        # Config file doesn't exist, fall back to default
        print(f"Error: Config file '{config_file_path}' not found.")
        print(f"Using default config file: {default_config_path}")
        try:
            with open(default_config_path) as config_file:
                config = json.load(config_file)
        except FileNotFoundError:
            print(f"Error: Default config file '{default_config_path}' not found.")
            sys.exit(1)
        except json.JSONDecodeError:
            print(f"Error: {default_config_path} is not a valid JSON file.")
            sys.exit(1)
    # Adjust argv index for source name
    source_arg_index = 2
else:
    # No config file provided, use default
    try:
        with open(default_config_path) as config_file:
            config = json.load(config_file)
        print(f"Using default config file: {default_config_path}")
    except FileNotFoundError:
        print(f"Error: Default config file '{default_config_path}' not found.")
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"Error: {default_config_path} is not a valid JSON file.")
        sys.exit(1)
    source_arg_index = 1

sourceName = config.get("sourceName", "Cs137")
nEvents = config.get("nEvents", "250000")
absorberOn = config.get("absorberOn", "0")
absorberMat = config.get("absorberMat", "G4_AIR")
absorberThickness = config.get("absorberThickness", "10.0")
detectorType = config.get("detectorType", "0")
sourceHolderType = config.get("sourceHolderType", "0")
detectorHolderDistance = config.get("detectorHolderDistance", "55")
resconst = config.get("resconst", "3.3")
respower = config.get("respower", "-0.5")

detectorTypeName = 'SoidiumIodide' if detectorType == '1' else 'GAGG'

print("\n ----- Settings from config file -----")
print(f" sourceName: {sourceName}")
print(f" nEvents: {nEvents}")
print(f" absorberOn: {'Yes' if absorberOn == '1' else 'No'}")
print(f" absorberMaterial: {absorberMat}")
print(f" absorberThickness: {absorberThickness} mm")
print(f" detectorType: {detectorTypeName}")
print(f" sourceHolderType: {'3D Printed' if sourceHolderType == '0' else 'Lead with hole'}")
print(f" detectorHolderDistance: {detectorHolderDistance} mm")
print(f" --------------------------------------\n")



# Check python arguments to override defaults
if len(sys.argv) >= source_arg_index + 1:
    sourceName = sys.argv[source_arg_index]  # Override with command line argument

if sourceName and any(source == sourceName for source in allowedSourceTypes):

    if len(sys.argv) >= source_arg_index + 2:
        nEvents = sys.argv[source_arg_index + 1]
    if len(sys.argv) >= source_arg_index + 3:
        absorberOn = sys.argv[source_arg_index + 2]
    if len(sys.argv) >= source_arg_index + 4:
        absorberMat = sys.argv[source_arg_index + 3]
    if len(sys.argv) >= source_arg_index + 5:
        absorberThickness = sys.argv[source_arg_index + 4]
    if len(sys.argv) >= source_arg_index + 6:
        detectorType = sys.argv[source_arg_index + 5]
    if len(sys.argv) >= source_arg_index + 7:
        sourceHolderType = sys.argv[source_arg_index + 6]
    if len(sys.argv) >= source_arg_index + 8:
        detectorHolderDistance = sys.argv[source_arg_index + 7]
    if len(sys.argv) >= source_arg_index + 9:
        resconst = sys.argv[source_arg_index + 8]
    if len(sys.argv) >= source_arg_index + 10:
        respower = sys.argv[source_arg_index + 9]


    # --- Input Validation ---
    if sourceName not in allowedSourceTypes:
        print(f"Error: Invalid sourceName '{sourceName}'. Allowed values are {allowedSourceTypes}.")
        sys.exit(1)
    if detectorType not in allowedDetectorTypes:
        print(f"Error: Invalid detectorType '{detectorType}'. Allowed values are {allowedDetectorTypes}.")
        sys.exit(1)
    if sourceHolderType not in allowedSourceHolderTypes:
        print(f"Error: Invalid sourceHolderType '{sourceHolderType}'. Allowed values are {allowedSourceHolderTypes}.")
        sys.exit(1)
    try:
        float(detectorHolderDistance)
    except ValueError:
        print(f"Error: detectorHolderDistance '{detectorHolderDistance}' must be a number.")
        sys.exit(1)

    # ---------------------
    # Make file names
    # ---------------------
    sourcePropName = f"/GammaLabMC/macros/{sourceName}.txt"

    if absorberOn == "1":
        sourceName = f"{sourceName}_{absorberMat}_{absorberThickness}"

    genName = f"Gen_{detectorTypeName}_{sourceName}.root"
    simName = f"G4_{detectorTypeName}_{sourceName}.root"
    pdfName = f"G4_{detectorTypeName}_{sourceName}.pdf"
    SpeName = f"G4_{detectorTypeName}_{sourceName}.Spe"
    bckName = "Bg_Sim.Spe"

    # ---------------------
    # Run Gen code
    # ---------------------
    print("simdir:", simdir)
    print("sourcePropName:", sourcePropName)
    print("genName:", genName)
    print("nEvents:", nEvents)
    subprocess.run(["root", "-b", "-q", f'{simdir}/GammaLabMC/GenerateSpectrum.C+("{simdir}{sourcePropName}","{genName}",{nEvents})'])

    # ---------------------
    # Write Macro file
    # ---------------------
    with open(macroName, "w") as f:
        f.write(f"/GammaLabMC/detector/DetectorType {detectorType} \n")
        f.write(f"/GammaLabMC/detector/SourceHolderType {sourceHolderType} \n")
        f.write(f"/GammaLabMC/detector/DetectorDistance {detectorHolderDistance} \n")
        f.write(f"/GammaLabMC/detector/AbsorberOn {absorberOn} \n")
        f.write(f"/GammaLabMC/detector/AbsorberMaterial {absorberMat} \n")
        f.write(f"/GammaLabMC/detector/AbsorberThickness {absorberThickness} \n")
        f.write("/GammaLabMC/physics/SetEmModel Standard \n")
        f.write("/GammaLabMC/physics/SetEmProcess All \n")
        f.write("/run/initialize \n")
        f.write("/GammaLabMC/generator/Mode 1 \n")
        f.write(f"/GammaLabMC/generator/InputFile {genName} \n")
        f.write(f"/GammaLabMC/analysis/setOutputFile {simName} \n")

    # ---------------------
    # Run Sim code
    # ---------------------
    # subprocess.run(["./GammaLabMC", macroName])  # Just for local run
    # subprocess.run(["GammaLabMC", macroName]) # For run on cluster
    result = subprocess.run(["./GammaLabMC", macroName], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: Simulation failed with return code {result.returncode}")
        print(f"stdout: {result.stdout}")
        print(f"stderr: {result.stderr}")
        sys.exit(1)

    os.remove(macroName)

    # ---------------------
    # Run Smear code
    # ---------------------
    subprocess.run(["root", "-b", "-q", f'{simdir}/GammaLabMC/SmearSpectrum.C+("{simName}",{resconst},{respower})'])

    os.remove(genName)
    os.remove(simName)

else:
    print('WARNING, wrong input. Usage: ./runSodiumIodide.py [source] [N events] [abs on] [abs mat] [abs thick] [det type] [holder type] [holder dist]')
    print('Where [source] is one of:', ", ".join(allowedSourceTypes))
    print('Arguments in [] are optional and override config.json.')
    print('See https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html for materials.')