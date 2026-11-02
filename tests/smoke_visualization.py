"""End-to-end checks against a real Geant4 executable (also used in CI)."""

import argparse
import csv
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import zlib

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "bin"))
from geant4_scene import figure, load_scene


def check_outputs(output, events):
    scene = load_scene(output / "scene.json")
    assert scene["geometry"] and scene["tracks"]
    assert scene["run"]["events"] == events
    assert sum(len(m["triangles"]) for m in scene["geometry"]) > 10
    fig = figure(scene)
    assert {"mesh3d", "scatter3d"} <= {trace.type for trace in fig.data}
    assert 'Plotly.newPlot' in (output / "scene.html").read_text()
    assert read_png(output / "oblique.png") != read_png(output / "side.png")
    read_png(output / "front.png")
    log = (output / "simulation.log").read_text()
    assert re.findall(r"Number of events processed\s*:\s*(\d+)", log) == [str(events)], log[-2000:]
    print(f"Verified {output}: {len(scene['geometry'])} meshes, {len(scene['tracks'])} tracks")


def read_png(path):
    data = path.read_bytes()
    assert data[:8] == b"\x89PNG\r\n\x1a\n"
    width, height = struct.unpack(">II", data[16:24])
    assert (width, height) == (640, 480)
    compressed = b""
    offset = 8
    while offset < len(data):
        size = struct.unpack(">I", data[offset:offset + 4])[0]
        kind = data[offset + 4:offset + 8]
        chunk = data[offset + 8:offset + 8 + size]
        if kind == b"IDAT":
            compressed += chunk
        offset += size + 12
    pixels = zlib.decompress(compressed)
    assert len(pixels) > width * height
    assert len(set(pixels)) > 16, f"Blank or nearly blank image: {path}"
    return pixels


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--simulation", required=True, choices=("GammaLabMC", "DoseProfileMC"))
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    base = args.output.resolve()
    base.mkdir(parents=True, exist_ok=True)
    command = [sys.executable, str(REPO / "bin/visualize.py"), "--simulation", args.simulation,
               "--executable", str(args.executable.resolve()), "--events", "5",
               "--width", "640", "--height", "480"]
    output = base / "gps"
    subprocess.run(command + ["--output", str(output)], check=True)
    check_outputs(output, 5)
    if args.simulation == "DoseProfileMC":
        with (output / "analysis.csv").open() as stream:
            reader = csv.reader(stream)
            next(reader)
            row = next(reader)
        assert int(row[0]) == 5
    else:
        # Exercise ROOT input and ensure an explicit beamOn is NOT run again.
        generator = base / "generator.root"
        call = (f'{REPO}/GammaLabMC/GenerateSpectrum.C('
                f'"{REPO}/GammaLabMC/macros/Cs137.txt","{generator}",5000,0,1,0,50)')
        subprocess.run(["root", "-l", "-b", "-q", call], check=True)
        source = base / "source.mac"
        source.write_text("/GammaLabMC/generator/Mode 1\n"
                          f"/GammaLabMC/generator/InputFile {generator}\n")
        root_output = base / "root-input"
        subprocess.run(command + ["--source-macro", str(source),
                                  "--output", str(root_output)], check=True)
        check_outputs(root_output, 5)
        # Also preserve the legacy production macro's automatic full-list run.
        batch = base / "batch.mac"
        batch.write_text(
            "/run/verbose 1\n/GammaLabMC/physics/SetEmModel Standard\n"
            "/GammaLabMC/physics/SetEmProcess All\n/run/initialize\n"
            + source.read_text()
            + f"/GammaLabMC/analysis/setOutputFile {base}/batch.root\n")
        result = subprocess.run([str(args.executable.resolve()), str(batch)],
                                cwd=base, text=True, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, check=True)
        (base / "batch.log").write_text(result.stdout)
        counts = re.findall(r"Number of events processed\s*:\s*(\d+)", result.stdout)
        assert len(counts) == 1 and int(counts[0]) > 5
    # Execute the actual example notebook, including its embedded 3D MIME output.
    env = dict(os.environ, GEANT4_SCENE=str(output / "scene.json"))
    subprocess.run(
        [sys.executable, "-m", "nbconvert", "--to", "notebook", "--execute",
         str(REPO / "examples/visualization.ipynb"), "--output", "visualization-executed.ipynb",
         "--output-dir", str(base), "--ExecutePreprocessor.timeout=120"],
        env=env, check=True)
    notebook = json.loads((base / "visualization-executed.ipynb").read_text())
    assert any("text/html" in item.get("data", {}) and "Plotly.newPlot" in
               "".join(item["data"]["text/html"])
               for cell in notebook["cells"] for item in cell.get("outputs", []))


if __name__ == "__main__":
    main()
