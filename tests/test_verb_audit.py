"""Guard the audit against accepting the same false positives as old smoke tests."""
import unittest
import json
import re
from pathlib import Path
from docker_verb_audit import assert_output, assert_state, wire
from build_verb_catalog import function_sources, read_routes


class AuditAssertions(unittest.TestCase):
    def test_room_message_destinations_are_encoded(self):
        root = Path(__file__).resolve().parents[1]
        for directory in ("src", "cr", "specials"):
            for path in (root / directory).glob("*"):
                if path.suffix in {".c", ".h"}:
                    self.assertIsNone(re.search(r"\b(?:l?send_msg|send_g_msg)\s*\(\s*(?:ploc\s*\(|LOC_)",
                                                path.read_text()), str(path))

    def test_contract_branch_references_exist(self):
        root = Path(__file__).resolve().parent
        data = json.loads((root / "verb_contracts.json").read_text())
        pairs = {(scenario["id"], step["case"]) for scenario in data["scenarios"]
                 for step in scenario["steps"]}
        self.assertEqual(len(pairs), sum(len(s["steps"]) for s in data["scenarios"]))
        for route in data["routes"]:
            for branch in route["branches"]:
                self.assertIn((branch["scenario"], branch["case"]), pairs)
            if route["status"] == "reviewed":
                self.assertTrue(route["branches"], route["id"])

    def test_complete_output_and_silence(self):
        names = {"actor": "Actor", "witness": "Witness"}
        good = {"output": {"Actor": wire("Ok\n")}, "overflow": False}
        assert_output(good, {"actor": "Ok\n"}, names)
        for wrong in (" Ok\n", "Ok \n", "Ok\nextra\n", "Ok", "Ok\n\n"):
            with self.subTest(wrong=wrong), self.assertRaises(AssertionError):
                assert_output({"output": {"Actor": wire(wrong)}}, {"actor": "Ok\n"}, names)
        with self.assertRaises(AssertionError):
            assert_output({"output": {"Actor": wire("Ok\n"), "Witness": " "}},
                          {"actor": "Ok\n"}, names)

    def test_state_rejects_successful_text_without_effect(self):
        with self.assertRaises(AssertionError):
            assert_state({"oflags": ["Lightable"]}, {"oflags": {"contains": ["Lit"]}})

    def test_rejection_must_not_change_state(self):
        with self.assertRaises(AssertionError):
            assert_state({"location": 7}, {"location": {"unchanged": True}}, {"location": 5})

    def test_capture_overflow_is_not_silence(self):
        with self.assertRaises(AssertionError):
            assert_output({"output": {}, "overflow": True}, {}, {"actor": "Actor"})

    def test_macros_and_formatting_are_not_handlers(self):
        definitions = function_sources()
        self.assertNotIn("ssetflg", definitions)
        self.assertIn("lightcom", definitions)
        self.assertEqual(read_routes()["COLOR"], ["bprintf", "sclrflg", "ssetflg"])


if __name__ == "__main__":
    unittest.main()
