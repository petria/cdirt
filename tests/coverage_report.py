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

    runtime_verbs = {word for scenario in scenarios for word in scenario["verbs"]}
    # docker_smoke.py also exercises every source-routed default fallback.
    runtime_verbs.update(entry["word"] for entry in verbs["entries"]
                         if entry["dispatch_kind"] == "doverb-default")
    missing_verbs = [entry for entry in verbs["entries"]
                     if entry["word"] not in runtime_verbs]
    handlers = {entry["primary_handler"] for entry in verbs["entries"]
                if entry["dispatch_kind"] == "c-handler"}
    covered_handlers = {
        entry["primary_handler"]
        for scenario in scenarios
        for word in scenario["verbs"]
        for entry in verbs["entries"]
        if entry["word"] == word and entry["dispatch_kind"] == "c-handler"
    }
    missing_handlers = sorted(handlers - covered_handlers)
    branch_cases = {
        (handler, branch)
        for scenario in scenarios
        for handler, branches in scenario.get("branch_cases", {}).items()
        for branch in branches
    }
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

    print(f"Verb inputs with runtime cases: {len(runtime_verbs & {e['word'] for e in verbs['entries']})}/"
          f"{verbs['verb_count']}")
    print(f"C handlers with runtime cases: {len(covered_handlers)}/{len(handlers)}")
    print(f"Named handler branch cases: {len(branch_cases)}")
    print(f"Active quests with runtime cases: {len(covered_quests & active_quests)}/{len(active_quests)}")
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
    strict_failure = (("--strict" in sys.argv and (missing_verbs or missing_quests))
                      or ("--strict-handlers" in sys.argv and missing_handlers))
    if strict_failure:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
