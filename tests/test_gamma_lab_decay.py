"""Integration checks: set GAMMALABMC_EXECUTABLE and source the Geant4/ROOT environment."""

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest


EXECUTABLE = os.environ.get("GAMMALABMC_EXECUTABLE")
REPOSITORY = Path(__file__).resolve().parents[1]


@unittest.skipUnless(EXECUTABLE and shutil.which("root"), "requires GammaLabMC and ROOT")
class RadioactiveDecayTests(unittest.TestCase):
    def test_beta_particles_and_decay_counts(self):
        for source, z, a, beta, endpoint in (
            ("Cs137", 55, 137, "e-", 1.176),
            ("Co60", 27, 60, "e-", 1.492),
            ("Na22", 11, 22, "e+", 0.546),
        ):
            with self.subTest(source=source), tempfile.TemporaryDirectory() as directory:
                work = Path(directory)
                macro = work / "decay.mac"
                macro.write_text(f"""
/GammaLabMC/detector/DetectorType 1
/GammaLabMC/detector/SourceHolderType 0
/GammaLabMC/detector/DetectorDistance 50
/GammaLabMC/physics/SetEmModel Standard
/GammaLabMC/physics/SetEmProcess All
/GammaLabMC/physics/RadioactiveDecay true
/run/initialize
/process/had/rdm/thresholdForVeryLongDecayTime 1e60 year
/GammaLabMC/analysis/setOutputFile {work / "decay.root"}
/GammaLabMC/generator/Mode 0
/GammaLabMC/generator/Seed 12345
/gps/particle ion
/gps/ion {z} {a} 0 0
/gps/ene/mono 0 keV
/gps/pos/type Point
/gps/pos/centre 0 0 0 mm
/tracking/verbose 1
/run/beamOn 100
""")
                result = subprocess.run(
                    [EXECUTABLE, str(macro)], cwd=work, text=True,
                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=120,
                )
                self.assertEqual(result.returncode, 0, result.stdout[-8000:])
                self.assertFalse("COMMAND NOT FOUND" in result.stdout, result.stdout[-8000:])
                self.assertFalse("G4Exception" in result.stdout, result.stdout[-8000:])
                beta_energies = []
                for track in re.split(r"\* G4Track Information:", result.stdout)[1:]:
                    if re.match(
                        rf"\s+Particle = {re.escape(beta)},\s+Track ID = \d+,\s+Parent ID = 1\n",
                        track,
                    ):
                        energy = re.search(
                            r"^\s*0\s+\S+\s+\S+\s+\S+\s+([0-9.e+-]+)", track, re.MULTILINE,
                        )
                        self.assertIsNotNone(energy)
                        beta_energies.append(float(energy[1]))
                self.assertGreater(len(beta_energies), 5, result.stdout[-8000:])
                self.assertGreater(len(set(beta_energies)), 3)
                self.assertTrue(all(0 < energy < endpoint for energy in beta_energies))
                self.assertTrue("Particle = gamma," in result.stdout, result.stdout[-8000:])
                if source == "Na22":
                    self.assertTrue("annihil" in result.stdout, result.stdout[-8000:])
                check = work / "CheckCounts.C"
                check.write_text("""
#include "TFile.h"
#include "TTree.h"
#include "TParameter.h"
#include "TSystem.h"
void CheckCounts() {
  TFile file("decay.root");
  auto tree = file.Get<TTree>("T");
  auto generated = file.Get<TParameter<Long64_t>>("GeneratedEventCount");
  auto transported = file.Get<TParameter<Long64_t>>("TransportedEventCount");
  if (!tree || tree->GetBranch("Detector_EdBeta") ||
      tree->GetBranch("Detector_EdGamma"))
    gSystem->Exit(1);
  if (!tree || !generated || !transported ||
      generated->GetVal() != 100 || transported->GetVal() != 100)
    gSystem->Exit(1);
  Int_t nhits;
  Float_t hitEdep[50000];
  tree->SetBranchAddress("Detector_Nhits", &nhits);
  tree->SetBranchAddress("Detector_Ed", hitEdep);
  bool hasMultipleParticleHits = false;
  for (Long64_t entry = 0; entry < tree->GetEntries(); entry++) {
    tree->GetEntry(entry);
    Float_t hitTotal = 0;
    for (Int_t hit = 0; hit < nhits; hit++) hitTotal += hitEdep[hit];
    if (nhits > 1) {
      hasMultipleParticleHits = true;
    }
    if (hitTotal <= 0)
      gSystem->Exit(1);
  }
  if (!hasMultipleParticleHits) gSystem->Exit(1);
}
""")
                result = subprocess.run(
                    ["root", "-l", "-b", "-q", str(check)], cwd=work, text=True,
                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=60,
                )
                self.assertEqual(result.returncode, 0, result.stdout)

    def test_runner_produces_spectra(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            # ACLiC needs a writable copy of the ROOT macros.
            simulation = work / "GammaLabMC"
            simulation.mkdir()
            for name in ("GenerateSpectrum.C", "SmearSpectrum.C"):
                shutil.copy2(REPOSITORY / "GammaLabMC" / name, simulation / name)
            shutil.copytree(REPOSITORY / "GammaLabMC" / "macros", simulation / "macros")
            environment = dict(
                os.environ, SIM_DIR=str(work), PROJECT_DIR=str(work), OUTPUT_DIR=str(work),
                PATH=str(Path(EXECUTABLE).parent) + os.pathsep + os.environ["PATH"],
            )
            for source, enabled in (("Cs137", False), ("Cs137", True),
                                    ("Co60", True), ("Na22", True)):
                with self.subTest(source=source, includeBetas=enabled):
                    config = work / "config.json"
                    config.write_text(json.dumps({
                        "sourceName": source, "includeBetas": enabled,
                        "nEvents": 2000, "detectorType": "1",
                    }))
                    result = subprocess.run(
                        [sys.executable, str(REPOSITORY / "bin" / "runGammaLabMC.py"),
                         str(config)],
                        cwd=work, env=environment, text=True,
                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=120,
                    )
                    self.assertEqual(result.returncode, 0, result.stdout[-8000:])
                    self.assertFalse("G4Exception" in result.stdout, result.stdout[-8000:])
                    suffix = "_withBetas" if enabled else ""
                    stem = f"G4_SodiumIodide_{source}{suffix}"
                    spectrum = (work / f"{stem}.Spe").read_text().splitlines()
                    self.assertEqual(spectrum[9:11], ["$DATA:", "0 511"])
                    channels = [float(value) for value in spectrum[11:523]]
                    self.assertEqual(len(channels), 512)
                    self.assertGreater(sum(channels), 0)
                    self.assertGreater((work / f"{stem}.pdf").stat().st_size, 0)
                    counts = (work / f"{stem}_event_counts.txt").read_text()
                    if enabled:
                        self.assertEqual(
                            counts, "GeneratedEventCount 2000\nTransportedEventCount 2000\n",
                        )
                    self.assertFalse((work / f"{stem}.root").exists())


if __name__ == "__main__":
    unittest.main()
