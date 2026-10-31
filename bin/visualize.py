#!/usr/bin/env python3
"""Run either simulation without X/Qt, producing PNG, VRML, JSON and Plotly HTML."""

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

from geant4_scene import figure, read_vrml


def positive_int(value):
    number = int(value)
    if number <= 0:
        raise argparse.ArgumentTypeError("must be positive")
    return number


def positive_float(value):
    number = float(value)
    if not 0 < number < float("inf"):
        raise argparse.ArgumentTypeError("must be finite and positive")
    return number


def macro_path(path):
    # Geant4 file-name UI parameters do not reliably support shell-style quoting.
    value = str(Path(path).resolve())
    if any(c.isspace() for c in value) or '"' in value or "'" in value:
        raise ValueError("Geant4 macro paths must not contain whitespace or quotes")
    return value


def make_macro(args, output):
    simulation = args.simulation
    prefix = "/" + simulation
    commands = ["/control/verbose 1", "/run/verbose 1",
                f"/random/setSeeds {args.seed} {args.seed + 1}"]
    if simulation == "GammaLabMC":
        commands += [
            prefix + "/detector/DetectorType 1",
            prefix + "/detector/SourceHolderType 0",
            prefix + "/detector/DetectorDistance 50",
            prefix + "/detector/AbsorberOn 0",
            prefix + "/physics/SetEmModel Standard",
            prefix + "/physics/SetEmProcess All",
        ]
    else:
        commands += [prefix + "/detector/TumourOn 1",
                     prefix + "/physics/addPhysics standard_opt3"]
    if args.geometry_macro:
        commands.append("/control/execute " + macro_path(args.geometry_macro))
    commands += [prefix + "/analysis/setOutputFile " +
                 macro_path(output / ("analysis.root" if simulation == "GammaLabMC"
                                      else "analysis.csv")),
                 "/run/initialize"]
    if simulation == "GammaLabMC":
        commands += [
            prefix + "/generator/Mode 0", "/gps/particle gamma",
            f"/gps/energy {args.energy or 0.661657} MeV",
            "/gps/pos/type Point", "/gps/pos/centre 0 0 0 mm",
            # An illustrative forward cone, not an isotropic efficiency sample.
            "/gps/ang/type iso", "/gps/ang/mintheta 150 deg",
            "/gps/ang/maxtheta 180 deg",
        ]
    else:
        commands += [
            "/gps/particle " + args.particle,
            f"/gps/energy {args.energy or 150} MeV",
            "/gps/direction 0 -1 0", "/gps/pos/type Plane",
            "/gps/pos/shape Circle", "/gps/pos/radius 5 mm",
            "/gps/pos/centre 0 1000 0 mm",
            "/gps/pos/rot1 1 0 0", "/gps/pos/rot2 0 0 1",
        ]
    if args.source_macro:
        commands.append("/control/execute " + macro_path(args.source_macro))
    commands += [
        "/vis/verbose warnings",
        f"/vis/open TOOLSSG_OFFSCREEN {args.width}x{args.height}-0+0",
        "/vis/tsg/offscreen/set/format zb_png",
        "/vis/viewer/set/autoRefresh false",
        "/vis/geometry/set/forceSolid all -1 false",
        "/vis/geometry/set/forceWireframe all -1 false",
        "/vis/geometry/set/colour all -1 0.5 0.6 0.8 0.2",
        "/vis/drawVolume",
        "/vis/viewer/set/background white",
        f"/vis/viewer/set/style {args.style}",
        "/vis/viewer/set/auxiliaryEdge true",
        "/vis/scene/add/trajectories smooth",
        "/vis/modeling/trajectories/create/drawByParticleID",
    ]
    if simulation == "GammaLabMC":
        for volume, rgb, alpha in (
                ("detcase_log", "1 0 0", 0.2), ("det_log", "1 1 0", 0.35),
                ("source_log", "0 0 1", 0.25), ("LeadShield_log", "0 1 0", 0.2),
                ("lead_log", "0.4 0.4 0.4", 0.15), ("desk_log", "0.5 0.5 0.5", 0.1)):
            commands.append(f"/vis/geometry/set/colour {volume} 0 {rgb} {alpha}")
    else:
        commands.append("/vis/geometry/set/colour det1_log 0 0.2 0.4 1 0.2")
    for particle, colour in (("gamma", "green"), ("e-", "red"), ("e+", "cyan"),
                            ("proton", "magenta"), ("alpha", "yellow"),
                            ("neutron", "blue"), ("ion", "yellow")):
        commands.append(
            f"/vis/modeling/trajectories/drawByParticleID-0/set {particle} {colour}")
    commands += [f"/vis/scene/endOfEventAction accumulate {args.events}",
                 f"/run/beamOn {args.events}"]
    for name, theta, phi in (("oblique", 70, 20), ("front", 90, 90), ("side", 90, 0)):
        commands += [
            "/vis/tsg/offscreen/set/file " + macro_path(output / (name + ".png")),
            f"/vis/viewer/set/viewpointThetaPhi {theta} {phi}",
            f"/vis/viewer/zoomTo {args.zoom}",
            "/vis/viewer/rebuild",
        ]
    # A second viewer uses the existing scene and all kept events, not a rerun.
    commands += ["/vis/open VRML2FILE", "/vis/viewer/set/style surface",
                 "/vis/viewer/flush"]
    return "\n".join(commands) + "\n"


def run(args):
    import plotly  # Fail before an expensive run if the notebook dependency is missing.

    output = args.output.resolve()
    macro_path(output)
    if output.exists() and any(output.iterdir()):
        raise ValueError(f"Output directory is not empty: {output}; choose a fresh directory")
    executable = args.executable or shutil.which(args.simulation)
    if not executable:
        raise ValueError(f"{args.simulation} not found; use --executable with a rebuilt binary")
    executable = str(Path(executable).resolve())
    for path in (args.geometry_macro, args.source_macro):
        if path and not path.is_file():
            raise ValueError(f"Macro not found: {path}")
    output.mkdir(parents=True, exist_ok=True)
    macro = output / "visualize.mac"
    macro.write_text(make_macro(args, output))
    env = os.environ.copy()
    env.pop("DISPLAY", None)
    env.pop("WAYLAND_DISPLAY", None)
    env["G4VRMLFILE_DEST_DIR"] = str(output) + "/"
    env.pop("G4VRMLFILE_VIEWER", None)
    log = output / "simulation.log"
    command = [executable, str(macro)]
    if args.simulation == "GammaLabMC":
        command.append("--vis")
    with log.open("w") as stream:
        result = subprocess.run(command, cwd=output, env=env,
                                stdout=stream, stderr=subprocess.STDOUT)
    text = log.read_text(errors="replace")
    if result.returncode or re.search(
            r"COMMAND NOT FOUND|Illegal parameter|Batch is interrupted|"
            r"G4Exception-START|ERROR:", text, re.I):
        raise RuntimeError(f"Simulation failed (exit {result.returncode}); see {log}")
    for name in ("oblique", "front", "side"):
        image = output / (name + ".png")
        if not image.is_file() or image.read_bytes()[:8] != b"\x89PNG\r\n\x1a\n":
            raise RuntimeError(f"Missing PNG {image}; requires TOOLSSG_OFFSCREEN with PNG support; see {log}")
    vrml = sorted(output.glob("*.wrl"))
    if len(vrml) != 1:
        raise RuntimeError(f"Expected one VRML2FILE export, found {len(vrml)}; see {log}")
    scene = read_vrml(vrml[0])
    if not scene["tracks"]:
        raise RuntimeError(f"No trajectories exported; see {log}")
    vrml[0].rename(output / "scene.wrl")
    scene["run"] = {"simulation": args.simulation, "events": args.events,
                    "seed": args.seed, "executable": executable,
                    "plotly_version": plotly.__version__}
    (output / "scene.json").write_text(json.dumps(scene, separators=(",", ":")))
    figure(scene).write_html(output / "scene.html", include_plotlyjs=True, full_html=True)
    print(f"Created three PNGs, scene.wrl, scene.json and interactive scene.html in {output}")
    print(f"Exported {len(scene['geometry'])} volumes and {len(scene['tracks'])} polylines")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--simulation", choices=("GammaLabMC", "DoseProfileMC"),
                        default="GammaLabMC")
    parser.add_argument("--executable", help="Path to the rebuilt simulation executable")
    parser.add_argument("--output", type=Path, default=Path("visualization"))
    parser.add_argument("--events", type=positive_int, default=20,
                        help="Number of events to transport and accumulate (default: 20)")
    parser.add_argument("--seed", type=positive_int, default=12345)
    parser.add_argument("--energy", type=positive_float, help="Primary energy in MeV")
    parser.add_argument("--particle", choices=("proton", "gamma", "e-"), default="proton",
                        help="DoseProfileMC primary (default: proton)")
    parser.add_argument("--geometry-macro", type=Path,
                        help="Detector/physics overrides before /run/initialize")
    parser.add_argument("--source-macro", type=Path,
                        help="Source overrides after initialization; no beamOn or vis commands")
    parser.add_argument("--width", type=positive_int, default=1200)
    parser.add_argument("--height", type=positive_int, default=900)
    parser.add_argument("--zoom", type=positive_float, default=1)
    parser.add_argument("--style", choices=("surface", "wireframe"), default="surface")
    args = parser.parse_args()
    try:
        run(args)
    except (ValueError, RuntimeError, OSError, ImportError) as error:
        parser.exit(1, f"Error: {error}\n")


if __name__ == "__main__":
    main()
