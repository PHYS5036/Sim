import contextlib
import io
import json
import os
from pathlib import Path
import runpy
import tempfile
import unittest
from unittest.mock import patch


REPOSITORY = Path(__file__).resolve().parents[1]
RUNNER = REPOSITORY / "bin" / "runGammaLabMC.py"


class GammaLabRunnerTests(unittest.TestCase):
    def run_config(self, config):
        calls = []
        macros = []
        removed = []

        def run(command, check):
            self.assertTrue(check)
            calls.append(command)
            if command[0] == "GammaLabMC":
                macros.append(Path(command[1]).read_text())

        with tempfile.TemporaryDirectory() as directory:
            config_path = Path(directory) / "config.json"
            config_path.write_text(json.dumps(config))
            previous_directory = os.getcwd()
            try:
                os.chdir(directory)
                with patch.dict(os.environ, {
                    "SIM_DIR": str(REPOSITORY),
                    "PROJECT_DIR": directory,
                    "OUTPUT_DIR": directory,
                }), patch("sys.argv", [str(RUNNER), "config.json"]), \
                        patch("subprocess.run", side_effect=run), \
                        patch("os.remove", side_effect=removed.append), \
                        contextlib.redirect_stdout(io.StringIO()):
                    runpy.run_path(str(RUNNER), run_name="__main__")
            finally:
                os.chdir(previous_directory)
        return calls, macros[0], removed

    def test_default_preserves_gamma_pipeline(self):
        calls, macro, removed = self.run_config({"nEvents": "12"})
        self.assertEqual(len(calls), 3)
        self.assertIn("GenerateSpectrum.C+", calls[0][-1])
        self.assertIn("/GammaLabMC/generator/Mode 1", macro)
        self.assertNotIn("/gps/ion", macro)
        self.assertNotIn("RadioactiveDecay", macro)
        self.assertNotIn("/run/beamOn", macro)
        self.assertIn("Gen_GAGG_Cs137.root", removed)

    def test_initial_beta_sources_use_full_decay(self):
        for source, ion in (
            ("Cs137", "55 137"),
            ("Co60", "27 60"),
            ("Na22", "11 22"),
        ):
            for enabled in (True, "1"):
                with self.subTest(source=source, enabled=enabled):
                    calls, macro, removed = self.run_config({
                        "sourceName": source, "includeBetas": enabled,
                        "nEvents": 12, "absorberOn": "1",
                        "absorberMat": "G4_Al", "absorberThickness": "2",
                    })
                    self.assertEqual(len(calls), 2)
                    self.assertEqual(calls[0][0], "GammaLabMC")
                    self.assertIn("SmearSpectrum.C+", calls[1][-1])
                    self.assertLess(macro.index("RadioactiveDecay true"),
                                    macro.index("/run/initialize"))
                    self.assertIn(f"/gps/ion {ion} 0 0", macro)
                    self.assertIn("/gps/ene/mono 0 keV", macro)
                    self.assertIn("/gps/pos/centre 0 0 0 mm", macro)
                    self.assertIn("thresholdForVeryLongDecayTime 1e60 year", macro)
                    self.assertIn("/run/beamOn 12", macro)
                    self.assertIn(f"{source}_withBetas_G4_Al_2.root", macro)
                    self.assertNotIn("InputFile", macro)
                    self.assertFalse(any(name.startswith("Gen_") for name in removed))

    def test_disabled_values_preserve_gamma_pipeline(self):
        for disabled in (False, "0"):
            with self.subTest(disabled=disabled):
                calls, macro, _ = self.run_config({"includeBetas": disabled})
                self.assertIn("GenerateSpectrum.C+", calls[0][-1])
                self.assertNotIn("RadioactiveDecay", macro)

    def test_non_beta_sources_remain_gamma_only(self):
        for source in ("Co57", "Am241"):
            with self.subTest(source=source):
                calls, macro, _ = self.run_config({
                    "sourceName": source, "includeBetas": True,
                })
                self.assertIn(f"/{source}.txt", calls[0][-1])
                self.assertNotIn("RadioactiveDecay", macro)
                self.assertNotIn("_withBetas", macro)

    def test_invalid_beta_flags_are_rejected(self):
        for value in ("yes", "false", 1, None, []):
            with self.subTest(value=value), self.assertRaises(SystemExit) as error:
                self.run_config({"includeBetas": value})
            self.assertEqual(error.exception.code, 1)

    def test_invalid_event_counts_are_rejected(self):
        for value in (0, -1, 1.5, "invalid", True, 2147483648):
            with self.subTest(value=value), self.assertRaises(SystemExit) as error:
                self.run_config({"nEvents": value, "includeBetas": True})
            self.assertEqual(error.exception.code, 1)


if __name__ == "__main__":
    unittest.main()
