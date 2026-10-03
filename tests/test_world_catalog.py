"""Check that the zone/quest inventory stays tied to the compiled C sources."""

from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "tests" / "world_catalog.json"
BUILDER = ROOT / "tests" / "build_world_catalog.py"
SCENARIOS = ROOT / "tests" / "quest_scenarios.json"

spec = importlib.util.spec_from_file_location("build_world_catalog", BUILDER)
builder = importlib.util.module_from_spec(spec)
assert spec and spec.loader
sys.modules[spec.name] = builder
spec.loader.exec_module(builder)


class WorldCatalogTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.catalog = json.loads(CATALOG.read_text())
        cls.scenarios = json.loads(SCENARIOS.read_text())

    def test_generated_world_inventory_matches_source(self) -> None:
        result = subprocess.run(
            [sys.executable, str(BUILDER), "--check"],
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_all_active_zone_files_are_in_inventory(self) -> None:
        files = sorted(path.relative_to(ROOT).as_posix() for path in (ROOT / "data/ZONES").glob("*.zone"))
        listed = [entry["file"] for entry in self.catalog["zones"]]
        self.assertEqual(self.catalog["zone_count"], len(files))
        self.assertEqual(listed, files)
        for zone in self.catalog["zones"]:
            with self.subTest(zone=zone["zone"]):
                self.assertGreater(zone["records"]["locations"], 0)
                self.assertGreaterEqual(zone["records"]["objects"], 0)
                self.assertGreaterEqual(zone["records"]["mobiles"], 0)
                self.assertEqual(zone["records"]["locations"], len(zone["rooms"]))

    def test_every_room_and_declared_exit_is_in_inventory(self) -> None:
        rooms = [room for zone in self.catalog["zones"] for room in zone["rooms"]]
        refs = [f"{room['name']}@{room['zone']}" for room in rooms]
        self.assertEqual(len(rooms), sum(z["records"]["locations"] for z in self.catalog["zones"]))
        self.assertEqual(len(refs), len(set(refs)))
        linked = [target for room in rooms for target in room["exits"].values()
                  if target.startswith("^")]
        self.assertEqual(len(linked), self.catalog["linked_exit_count"])
        self.assertGreater(len(linked), 0)
        for room in rooms:
            with self.subTest(room=room):
                self.assertTrue(room["name"])
                self.assertTrue(room["zone"])
                self.assertTrue(set(room["exits"]).issubset(
                    {"north", "south", "east", "west", "up", "down"}))
                self.assertTrue(all(target for target in room["exits"].values()))

    def test_quest_table_indexes_and_symbols_are_unique(self) -> None:
        quests = self.catalog["quests"]
        indexes = [entry["index"] for entry in quests]
        symbols = [entry["symbol"] for entry in quests]
        self.assertEqual(indexes, list(range(self.catalog["quest_count"])))
        self.assertEqual(len(symbols), len(set(symbols)))
        self.assertTrue(all(entry["name"] for entry in quests))

    def test_each_active_quest_has_a_c_trigger_site(self) -> None:
        missing = [entry["symbol"] for entry in self.catalog["quests"]
                   if not entry["trigger_sites"]]
        self.assertEqual(missing, [])

    def test_quest_scenarios_reference_known_quests_and_runners(self) -> None:
        quests = {entry["symbol"] for entry in self.catalog["quests"]}
        zones = {entry["zone"] for entry in self.catalog["zones"]}
        ids: set[str] = set()
        for scenario in self.scenarios:
            with self.subTest(scenario=scenario["id"]):
                self.assertNotIn(scenario["id"], ids)
                ids.add(scenario["id"])
                self.assertIn(scenario["quest"], quests)
                self.assertTrue(scenario["zones"])
                self.assertTrue(set(scenario["zones"]).issubset(zones))
                self.assertTrue((ROOT / "tests" / scenario["runner"]).is_file())
                self.assertIn(scenario["status"], {"runtime", "blocked", "defect"})

    def test_legacy_optional_hooks_are_separated_from_active_quests(self) -> None:
        legacy = self.catalog["legacy_quest_hooks"]
        self.assertEqual(len(legacy), self.catalog["legacy_hook_count"])
        self.assertTrue(all(entry["index"] is None for entry in legacy))


if __name__ == "__main__":
    unittest.main()
