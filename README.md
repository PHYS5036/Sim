# Gamma Labs Simulation Software
Simulation software and set up for attentuation and absolute determination experiments for the PHYS5036 labs.

## Continuous integration

The `Compile Geant4 simulations` GitHub Actions workflow builds `DoseProfileMC`
and `GammaLabMC` independently on pushes and pull requests, and can also be run
manually. It checks CMake configuration, compilation and linking using the
default UI/visualisation-enabled build; it does not run the simulations.

CI uses the public Docker image `jeffersonlab/geant4:g4v11.2.2-ubuntu24`,
pinned by digest in the workflow. It includes Geant4 11.2.2, ROOT 6.32.02,
CMake and GCC. The `GammaLabMC` job also installs `nlohmann-json3-dev` and
`libvdt-dev`, required by the image's ROOT CMake configuration.
No Geant4 or ROOT build, CVMFS mount or Apptainer installation is needed.

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
