"""Guard the source-derived verb and dispatch inventory."""

from __future__ import annotations

import importlib.util
import json
import re
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "tests" / "verb_catalog.json"
BUILDER = ROOT / "tests" / "build_verb_catalog.py"
SCENARIOS = ROOT / "tests" / "scenarios.json"

spec = importlib.util.spec_from_file_location("build_verb_catalog", BUILDER)
builder = importlib.util.module_from_spec(spec)
assert spec and spec.loader
sys.modules[spec.name] = builder
spec.loader.exec_module(builder)


class VerbCatalogTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.catalog = json.loads(CATALOG.read_text())
        cls.entries = cls.catalog["entries"]

    def test_generated_catalog_matches_source(self) -> None:
        result = subprocess.run(
            [sys.executable, str(BUILDER), "--check"],
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_every_registered_word_has_one_dispatch_route(self) -> None:
        source_entries = builder.read_verbs()
        self.assertEqual(self.catalog["verb_count"], 390)
        self.assertEqual(len(self.entries), len(source_entries))
        self.assertEqual(
            [item["word"] for item in self.entries],
            [item["word"] for item in source_entries],
        )
        for entry in self.entries:
            with self.subTest(word=entry["word"]):
                self.assertTrue(entry["dispatch"])
                self.assertIn(entry["dispatch_kind"], {"c-handler", "doverb-default"})

    def test_aliases_share_their_verb_number(self) -> None:
        entries_by_word = {entry["word"]: entry for entry in self.entries}
        for entry in self.entries:
            if not entry["alias_of"]:
                continue
            with self.subTest(word=entry["word"], alias=entry["alias_of"]):
                target = entries_by_word[entry["alias_of"]]
                self.assertEqual(entry["verb_id"], target["verb_id"])
                self.assertEqual(entry["dispatch"], target["dispatch"])

    def test_dispatch_inventory_includes_non_switch_routes(self) -> None:
        by_word = {entry["word"]: entry for entry in self.entries}
        for word, handler in {
            "north": "dodirn",
            "east": "dodirn",
            "aboot": "cmd_aboot",
            "aflags": "obj_flags",
            "take": "getcom",
        }.items():
            with self.subTest(word=word):
                self.assertIn(handler, by_word[word]["dispatch"])

    def test_output_api_inventory_is_nonempty(self) -> None:
        counts = self.catalog["output_api_call_sites"]
        for api in ("bprintf", "sendf", "sendl", "send_msg", "lsend_msg",
                    "send_magic_msg"):
            with self.subTest(api=api):
                self.assertGreater(counts[api], 0)

    def test_scenario_references_and_recipient_sets_are_valid(self) -> None:
        scenarios = json.loads(SCENARIOS.read_text())
        by_word = {entry["word"]: entry for entry in self.entries}
        for scenario in scenarios:
            with self.subTest(scenario=scenario["id"]):
                self.assertTrue(scenario["verbs"])
                self.assertTrue(scenario["exact_text"])
                command_word = re.match(r"\s*([A-Za-z]+)", scenario["command"])
                self.assertIsNotNone(command_word)
                self.assertIn(command_word.group(1).lower(), scenario["verbs"])
                recipients = scenario["audiences"]["receive"]
                excluded = scenario["audiences"]["exclude"]
                self.assertTrue(set(recipients).isdisjoint(excluded))
                self.assertEqual(set(scenario["expected_lines"]), set(recipients))
                for word in scenario["verbs"]:
                    self.assertIn(word, by_word)
                    self.assertIn(scenario["id"], by_word[word]["output_cases"])


if __name__ == "__main__":
    unittest.main()
