# Gamma Labs Simulation Software
Simulation software and set up for attentuation and absolute determination experiments for the PHYS5036 labs.

## Continuous integration

The `Compile Geant4 simulations` GitHub Actions workflow builds `DoseProfileMC`
and `GammaLabMC` independently on pushes and pull requests, and can also be run
manually. It checks CMake configuration, compilation and linking using the
default UI/visualisation-enabled build, then runs a small headless simulation,
checks three non-blank PNG views and meshes/tracks in the exported scene, and
executes the example 3D notebook. GammaLabMC also exercises ROOT-input
visualization and the legacy automatic batch run. Download the
`visualization-GammaLabMC` / `visualization-DoseProfileMC` workflow artifacts
to inspect the images, interactive HTML, JSON, VRML and executed notebook.

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

## Headless images and interactive notebook visualization

The existing `macros/vis.mac` files open `OGLSQt` and require a graphical
session. GammaLabMC previously initialized visualization only in interactive
mode; batch visualization is now opt-in with `GammaLabMC macro.mac --vis`.
Normal production invocations remain unchanged and do not store trajectories.
DoseProfileMC already initializes visualization in batch mode.

`bin/visualize.py` uses Geant4 11.2.2's **TOOLSSG_OFFSCREEN** software renderer
for PNGs and **VRML2FILE** for geometry plus accumulated trajectories.
Neither requires X, Qt, a GPU or Xvfb. Rebuild the executable first; the checked-in
GammaLabMC binary predates this change. Source your Geant4/ROOT environment and
use Python 3.9 or later:

```bash
python -m pip install -r requirements-visualization.txt
python bin/visualize.py --executable build/GammaLabMC/GammaLabMC \
  --output visualization --events 20
python bin/visualize.py --simulation DoseProfileMC \
  --executable build/DoseProfileMC/DoseProfileMC \
  --output visualization-dose --events 20 --energy 150
```

Each **fresh** output directory contains `oblique.png`, `front.png`, `side.png`,
`scene.wrl`, `scene.json`, and the self-contained, offline `scene.html` 3D viewer,
plus `visualize.mac`, `simulation.log` and the simulation's usual analysis file.
Images use translucent geometry so internal tracks remain visible; use
`--style wireframe`, `--zoom 2`, or `--width 1600 --height 1200` as needed.
The three views and 3D exports use the **same run**, not separate simulations.
Keep event counts small: retained trajectories and the DoseProfileMC CSV can
consume substantial memory/disk. The Geant4 build must include the two drivers
and PNG support; missing drivers/outputs cause a reported failure, not a fallback.

GammaLabMC defaults to NaI, a 50 mm detector distance, and a 661.657 keV
forward-cone GPS source to illustrate interactions. This is **not** the
isotropic, spectrum-weighted, geometrically filtered efficiency pipeline.
DoseProfileMC defaults to a 150 MeV proton beam into the water phantom with a
tumour. `--seed` controls Geant4's random seeds, `--energy` is in MeV, and
`--particle` changes the DoseProfileMC particle.

To visualize your actual setup, pass `--geometry-macro detector.mac` containing
detector/physics commands **before** initialization. Pass
`--source-macro source.mac` for GPS overrides or existing GammaLabMC ROOT input
**after** initialization, for example:

```text
/GammaLabMC/generator/Mode 1
/GammaLabMC/generator/InputFile /absolute/path/to/Gen_SodiumIodide_Cs137.root
```

These customization macros must not initialize, start a run, change the
analysis output or issue visualization commands. Use absolute paths inside
them (the run's working directory is the output directory). For ROOT input,
`--events` must not exceed the number of retained entries in `h1`. Geant4 macro
paths must not contain whitespace or quotes.

### 3D inside Jupyter

Open `examples/visualization.ipynb` after generating `visualization/scene.json`.
It displays **rotatable/zoomable 3D geometry and tracks inside the notebook**,
with transparent meshes and clickable legend entries to hide volumes or particle
groups. Set `GEANT4_SCENE` to another exported JSON path if desired.
The notebook needs Plotly, but **does not need Geant4 or ROOT**, so the data can
be copied to another machine. In a notebook opened from the repository root:

```python
import sys
sys.path.insert(0, "bin")
from geant4_scene import load_scene, figure
fig = figure(load_scene("visualization/scene.json"), geometry_opacity=0.25)
fig.show(renderer="notebook")
```

Trust the notebook to render its embedded JavaScript. Use
`renderer="plotly_mimetype"` in JupyterLab/VS Code. Opening `scene.html` in a
browser is an alternative that works offline without a notebook server.

The JSON schema `geant4-scene-v1` stores coordinates in **mm**, volume names,
vertices/triangle indices, and track polylines/colours. It is a tessellated
display scene, not a simulation-ready geometry export. VRML does not preserve
event IDs, track IDs, energies, material metadata or PDG codes; notebook particle
labels follow the supplied macro's colour mapping. The reader deliberately
supports Geant4's flat face/line output, not arbitrary VRML, markers or transforms.
