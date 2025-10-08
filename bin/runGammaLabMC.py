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
    print("Error: SIM_DIR environment variable is not set. Make sure to source bash script.")
    sys.exit(1)

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
    
default_config_path = os.path.join(simdir, 'bin', 'defaultconfig.json')

# Priority: 1. Command line arg (from PROJECT_DIR), 2. CONFIG_PATH env var, 3. Default
if len(sys.argv) >= 2:
    # Config file specified as command line argument - look in PROJECT_DIR
    config_filename = sys.argv[1]
    config_file_path = os.path.join(project_dir, config_filename)
    if os.path.exists(config_file_path):
        print(f"Using config file from command line argument: {config_file_path}")
    else:
        print(f"Error: Config file '{config_file_path}' specified in command line not found in PROJECT_DIR.")
        sys.exit(1)
else:
    # Check CONFIG_PATH environment variable
    config_path_env = os.environ.get('CONFIG_PATH')
    if config_path_env and os.path.exists(config_path_env):
        config_file_path = config_path_env
        print(f"Using config file from CONFIG_PATH: {config_file_path}")
    else:
        config_file_path = default_config_path
        if config_path_env:
            print(f"Warning: CONFIG_PATH is set but file '{config_path_env}' not found.")
        print(f"Using default config file: {default_config_path}")

# Load configuration from file
try:
    with open(config_file_path) as config_file:
        config = json.load(config_file)
except FileNotFoundError:
    print(f"Error: Config file '{config_file_path}' not found.")
    sys.exit(1)
except json.JSONDecodeError:
    print(f"Error: {config_file_path} is not a valid JSON file.")
    sys.exit(1)

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
print("Generated spectrum!")

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
print("Macro file written.")

# ---------------------
# Run Sim code
# ---------------------
# subprocess.run(["./GammaLabMC", macroName])  # Just for local run
# subprocess.run(["GammaLabMC", macroName]) # For run on cluster
print(f"Running Geant4 simulation at GammaLabMC")
subprocess.run(["GammaLabMC", macroName])

os.remove(macroName)

# ---------------------
# Run Smear code
# ---------------------
print(f"Smearing spectrum with resconst={resconst}, respower={respower}")
subprocess.run(["root", "-b", "-q", f'{simdir}/GammaLabMC/SmearSpectrum.C+("{simName}",{resconst},{respower})'])
print("Smeared spectrum!")

os.remove(genName)
os.remove(simName)
