# Gamma Labs Simulation Software
Simulation software and set up for attentuation and absolute determination experiments for the PHYS5036 labs.

Bugs Found:
- Uses old runSodiumIodide.py command in labscript (should be runGammaMC.py).
- Defaults to use GAGG crystal (detectorType: "0", should default to NaI detector for absolute determination lab ("1").
