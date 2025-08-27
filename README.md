# GAGG Sim
Simulation software and set up for PHYS5036 labs

## To Do

### Sodium Iodide (NaI) Detector Simulation

0. Relabel all files from NaI to GAGG.

1. Change Detector Setup.
    - Find out schematic of GAGG Detector, as well as GAGG G4 Material (Rick/Ross). Status: Email sent.
    - Find out schematic of detector setup housing (Claire, on friday 19/08, otherwise email). Status: N/A.
    - Implement changes to DetectorConstruction.cc. Status: N/A.

2. Change Spectrum smearing to match GAGG resolution -- find power law constants.
    - Determined from power law fit C*E^P from experimental data. (i.e. straight line fit on ln-ln scale).
    - Chase WillH to get the data and/or fits. Status: N/A.
