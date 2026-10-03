#!/usr/bin/env python3
"""Report runtime behavior coverage; --strict is the eventual release gate."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TESTS = ROOT / "tests"


def main() -> None:
    verbs = json.loads((TESTS / "verb_catalog.json").read_text())
    world = json.loads((TESTS / "world_catalog.json").read_text())
    scenarios = json.loads((TESTS / "scenarios.json").read_text())
    quest_scenarios = json.loads((TESTS / "quest_scenarios.json").read_text())

    # Declarations are plans, not execution evidence. Only a matching run report
    # can establish that a word, branch, state assertion, or audience check passed.
    from docker_verb_audit import fingerprint
    report_path = Path("/tmp/cdirt-verb-audit/report.json")
    if "--audit-report" in sys.argv:
        report_path = Path(sys.argv[sys.argv.index("--audit-report") + 1])
    report = json.loads(report_path.read_text()) if report_path.exists() else {}
    valid_report = report.get("fingerprint") == fingerprint()
    if report and not valid_report:
        print(f"Audit report is stale: {report_path}; no runtime credit")
    steps = [step for case in report.get("cases", []) for step in case["steps"]
             if valid_report and step.get("passed") and step.get("output_asserted")]
    runtime_verbs = {step["word"] for step in steps if step.get("word")}
    missing_verbs = [entry for entry in verbs["entries"]
                     if entry["word"] not in runtime_verbs]
    handlers = {entry["primary_handler"] for entry in verbs["entries"]
                if entry["route_kind"] == "handler"}
    covered_handlers = {step["handler"] for step in steps if step.get("handler") in handlers}
    missing_handlers = sorted(handlers - covered_handlers)
    branch_cases = {(step.get("route"), step["case"]) for step in steps}
    contracts = json.loads((TESTS / "verb_contracts.json").read_text())
    pending_routes = [route for route in contracts["routes"] if route["status"] != "reviewed"]
    active_quests = {entry["symbol"] for entry in world["quests"]}
    runtime_quest_scenarios = [scenario for scenario in quest_scenarios
                               if scenario.get("status") == "runtime"]
    covered_quests = {scenario["quest"] for scenario in runtime_quest_scenarios}
    missing_quests = sorted(active_quests - covered_quests)
    covered_zones = {zone for scenario in runtime_quest_scenarios
                     for zone in scenario.get("zones", [])}
    rooms = [room for zone in world["zones"] for room in zone.get("rooms", [])]
    room_count = len(rooms)
    exit_count = sum(len(room["exits"]) for room in rooms)
    linked_count = sum(target.startswith("^") for room in rooms
                       for target in room["exits"].values())

    print(f"Verb inputs with passing runtime evidence: {len(runtime_verbs & {e['word'] for e in verbs['entries']})}/"
          f"{verbs['verb_count']}")
    print(f"C handlers with runtime cases: {len(covered_handlers)}/{len(handlers)}")
    print(f"Named handler branch cases: {len(branch_cases)}")
    print(f"Passing steps with state assertions: {sum(bool(step.get('state_asserted')) for step in steps)}")
    print(f"Routes awaiting complete branch review: {len(pending_routes)}/{len(contracts['routes'])}")
    print(f"Active quests with declared scenarios: {len(covered_quests & active_quests)}/{len(active_quests)}")
    print(f"Quest route zones referenced by runtime quest cases: {len(covered_zones)}")
    print(f"Zone walk inventory: {room_count} rooms and {exit_count} declared exits "
          f"({linked_count} linked/stateful exits)")
    print("Zone runtime walk: docker_zone_walk.py checks each compiled room; "
          "the per-player wiz.zone template is listed separately")
    print("Handler branch/output audience coverage: incomplete until every "
          "handler branch has a scenario")
    if missing_verbs:
        print(f"Uncovered verb inputs: {len(missing_verbs)}")
    if missing_quests:
        print("Quests without runtime scenarios: " + ", ".join(missing_quests))
    if "--verbose" in sys.argv:
        if missing_verbs:
            print("Uncovered verb words: " + ", ".join(entry["word"] for entry in missing_verbs))
        if missing_handlers:
            print("Uncovered C handlers: " + ", ".join(missing_handlers))
        for quest in world["quests"]:
            if quest["symbol"] in missing_quests:
                sites = ", ".join(f"{site['file']}:{site['line']}"
                                   for site in quest["trigger_sites"])
                print(f"  {quest['symbol']} ({quest['name']}): {sites}")
    strict_failure = (("--strict" in sys.argv and (missing_verbs or missing_quests or pending_routes))
                      or ("--strict-handlers" in sys.argv and missing_handlers))
    if strict_failure:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
