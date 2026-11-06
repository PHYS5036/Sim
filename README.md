# Gamma Labs Simulation Software
Simulation software and set up for attentuation and absolute determination experiments for the PHYS5036 labs.

## Local environment setup

From the repository root, source `setupLocal.sh` to configure the project paths
without copying a machine-specific bashrc:

```bash
source ./setupLocal.sh
```

By default, the repository is used for `SIM_DIR` and `PROJECT_DIR`; output files
go to its `outputs` directory, and `config.json` is used as the configuration.
Set `PROJECT_DIR`, `OUTPUT_DIR` or `CONFIG_PATH` before sourcing the script to
override those defaults. To load local ROOT and Geant4 installations, set
`ROOT_SETUP` and `GEANT4_SETUP` to their respective `thisroot.sh` and
`geant4.sh` scripts before sourcing. If their environments are already loaded,
these variables are not needed.

## Including initial beta particles

Set `"includeBetas": true` in the JSON configuration passed to
`runGammaLabMC.py` (the strings `"1"` and `"0"` are also accepted).
The default is `false`, preserving the existing gamma-only simulation.

For Cs-137 and Co-60 this includes beta-minus electrons; for Na-22 it
includes beta-plus positrons. The enabled mode transports stationary source
ions at the same source position using Geant4 radioactive-decay data, rather
than the photon-line generator. Beta spectra, decay branching, daughter
de-excitation and positron annihilation are handled by Geant4, so the Na-22
511 keV photons are not also injected from the gamma table. Co-57 (electron
capture) and Am-241 (alpha decay) retain the gamma-only simulation, with a
message explaining that they have no initial beta branch.

In this mode `nEvents` counts parent decays, and both generated and transported
event counts equal `nEvents`. The photon-only geometric cut is disabled:
charged-particle scattering and secondary photons can reach the detector even
when the initial direction misses it. Runs will therefore be slower. Output
filenames include `_withBetas` to distinguish them from gamma-only results.
`Prim_pdg` identifies the parent ion, not an emitted beta.

The existing detector readout sums all deposits within a simulation event;
this mode therefore includes decay-cascade summing, including delayed daughter
emissions, without modelling an electronics coincidence window. It is not
simply an independent beta continuum added to the old photon spectrum.
The ROOT event tree stores one hit per final decay product that deposits energy
in the detector, including its PDG code and energy deposit. Deposits from
secondary tracks descended from that product are accumulated in the same hit;
excited or unstable nuclear intermediates are not recorded as hit groups.
Distinct final products in an event appear as separate entries in the hit arrays.
The `Decay_*` branches record the state of final decay products after nuclear
de-excitation: emitted non-nuclear particles and stable nuclei, excluding
excited or unstable intermediate nuclei. They include PDG code, kinetic energy,
momentum, position and time for each saved detector event (MeV, MeV/c, mm and
ns, respectively).
`SmearSpectrum.C` sums all hit deposits in each event before smearing.
Geant4's radioactive-decay and photon-evaporation datasets must be installed
(they are included in the CI image).

For direct Geant4 macros, enable `/GammaLabMC/physics/RadioactiveDecay true`
**before** `/run/initialize`, then configure a GPS ion source, for example:

```text
/process/had/rdm/thresholdForVeryLongDecayTime 1e60 year
/GammaLabMC/generator/Mode 0
/gps/particle ion
/gps/ion 55 137 0 0
/gps/ene/mono 0 keV
/gps/pos/type Point
/gps/pos/centre 0 0 0 mm
/run/beamOn 10000
```

Set the analysis output filename before `/run/beamOn`. For interactive use,
set `GLMC_VIS_CUT=0` and configure these commands in `macros/vis.mac` to prevent
the default gamma event list from replacing the ion source.

## Continuous integration

The `Compile Geant4 simulations` GitHub Actions workflow builds `DoseProfileMC`
and `GammaLabMC` independently on pushes and pull requests, and can also be run
manually. It checks CMake configuration, compilation and linking using the
default UI/visualisation-enabled build. The `GammaLabMC` job also checks
configuration handling, beta-minus/beta-plus production, decay event counts,
and end-to-end spectrum output.

CI uses the public Docker image `jeffersonlab/geant4:g4v11.2.2-ubuntu24`,
pinned by digest in the workflow. It includes Geant4 11.2.2, ROOT 6.32.02,
CMake and GCC. The `GammaLabMC` job also installs `nlohmann-json3-dev` and
`libvdt-dev`, required by the image's ROOT CMake configuration.
No Geant4 or ROOT build, CVMFS mount or Apptainer installation is needed.

Run configuration tests with
`python3 -m unittest discover -s tests -p 'test_gamma_lab_*.py' -v`.
To include the integration checks, source the Geant4/ROOT environment and set
`GAMMALABMC_EXECUTABLE` to the absolute path of the compiled `GammaLabMC`.

To reproduce a build with Docker, run from the repository root (replace
`GammaLabMC` with `DoseProfileMC` for the other simulation; the package
installation is only needed for `GammaLabMC`):

```bash
docker run --rm --entrypoint /bin/bash \
  --volume "$PWD:/workspace:ro" --workdir /workspace \
  jeffersonlab/geant4:g4v11.2.2-ubuntu24@sha256:642e1e4c41e768f44b85993eb197800cb92c623aa397940b615399193bd48e61 \
  -e -o pipefail -c '
    apt-get update -qq
    apt-get install -y --no-install-recommends nlohmann-json3-dev libvdt-dev
    source /etc/profile.d/localSetup.sh
    cmake -S GammaLabMC -B /tmp/build/GammaLabMC -DCMAKE_BUILD_TYPE=Release
    cmake --build /tmp/build/GammaLabMC --parallel 2
  '
```
