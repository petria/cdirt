#!/usr/bin/env python3
"""Build a source-derived C-Dirt zone and quest verification inventory."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ZONES = ROOT / "data" / "ZONES"
QUESTS_H = ROOT / "include" / "quests.h"
QUESTNAMES_H = ROOT / "include" / "questnames.h"
SPECIALS = ROOT / "specials"
SCENARIOS = ROOT / "tests" / "quest_scenarios.json"
OUT = ROOT / "tests" / "world_catalog.json"


def quest_entries() -> list[dict[str, object]]:
    constants = {
        name: int(value)
        for name, value in re.findall(
            r"^\s*#define\s+(Q_[A-Z0-9_]+)\s+(\d+)\s*$",
            QUESTS_H.read_text(), re.MULTILINE,
        )
        if name != "Q_MAX"
    }
    table = re.search(r"QUEST\s+Quests\[\]\s*=\s*\{(.*?)\};", QUESTNAMES_H.read_text(), re.S)
    if not table:
        raise ValueError("could not find QUEST Quests[] table")
    names = re.findall(r'\{\s*\d+\s*,\s*(?:True|False)\s*,\s*"([^"]+)"', table.group(1))
    by_index = {index: symbol for symbol, index in constants.items()}

    trigger_sites: dict[str, list[dict[str, object]]] = {}
    source_files = (list(SPECIALS.glob("*.h")) + list((ROOT / "src").glob("*.c")) +
                    list((ROOT / "cr").glob("*.c")))
    # Calls occur both in shared zone specials and in ordinary C handlers.
    for source in sorted(source_files):
        contents = source.read_text(errors="replace")
        for match in re.finditer(r"\bset_quest\s*\((.*?)\)", contents, re.S):
            symbol_match = re.search(r"\b(Q_[A-Z0-9_]+)\b", match.group(1))
            if not symbol_match:
                continue
            symbol = symbol_match.group(1)
            line_no = contents.count("\n", 0, match.start()) + 1
            site = {"file": source.relative_to(ROOT).as_posix(), "line": line_no}
            items = trigger_sites.setdefault(symbol, [])
            if site not in items:
                items.append(site)

    scenarios = json.loads(SCENARIOS.read_text()) if SCENARIOS.exists() else []
    scenario_ids: dict[str, list[str]] = {}
    for scenario in scenarios:
        scenario_ids.setdefault(scenario["quest"], []).append(scenario["id"])

    entries: list[dict[str, object]] = []
    for index, name in enumerate(names):
        symbol = by_index.get(index)
        if not symbol:
            raise ValueError(f"quest table index {index} ({name}) has no Q_* constant")
        entries.append({
            "index": index,
            "symbol": symbol,
            "name": name,
            "trigger_sites": trigger_sites.get(symbol, []),
            "runtime_scenarios": scenario_ids.get(symbol, []),
        })

    # Older optional-zone hooks remain in the shared special files after the
    # matching quest was removed from the active quest table. Preserve them in
    # the inventory so build/preprocessor checks can establish whether any are
    # active in this particular world build.
    for symbol in sorted(set(trigger_sites) - set(constants)):
        entries.append({
            "index": None,
            "symbol": symbol,
            "name": None,
            "trigger_sites": trigger_sites[symbol],
            "runtime_scenarios": [],
            "status": "legacy-hook-not-in-active-quest-table",
        })
    return entries


def zone_record_counts(text: str) -> dict[str, int]:
    """Count record keys using the same quote/description boundaries as C."""
    counts = {"locations": 0, "objects": 0, "mobiles": 0}
    section: str | None = None
    in_quote = in_hat = in_comment = False
    for line in text.splitlines():
        if not (in_quote or in_hat or in_comment):
            directive = re.match(r"^\s*%\s*([A-Za-z]+)", line)
            if directive:
                name = directive.group(1).lower()
                section = name if name in counts else None
                continue
            if section and not line.lstrip().startswith("#"):
                key_pattern = r"^\s*lflags\s*\{" if section == "locations" else r"^\s*Name\s*="
                if re.search(key_pattern, line, re.IGNORECASE):
                    counts[section] += 1

        previous = "\0"
        index = 0
        while index < len(line):
            char = line[index]
            following = line[index + 1] if index + 1 < len(line) else "\0"
            if not in_comment and char == "/" and following == "*":
                in_comment = True
                index += 2
                continue
            if in_comment and char == "*" and following == "/":
                in_comment = False
                index += 2
                continue
            if in_comment:
                index += 1
                continue
            if char == '"' and not in_hat:
                in_quote = not in_quote
            elif char == "^" and previous != ":":
                in_hat = not in_hat
            previous = char
            index += 1
    return counts


def room_inventory(text: str, default_zone: str) -> list[dict[str, object]]:
    """Extract C room header names and six declared exits from zone source.

    The scanner preserves C-Dirt's quoted/caret multiline boundaries, so text
    and ASCII art inside descriptions cannot be mistaken for room headers.
    Room identifiers are the first token, matching proc_location().
    """
    logical: list[str] = []
    buffer = ""
    quoted = hatted = comment = False
    for physical in text.splitlines():
        i = 0
        while i < len(physical):
            ch = physical[i]
            nxt = physical[i + 1] if i + 1 < len(physical) else ""
            if comment:
                if ch == "*" and nxt == "/":
                    comment = False
                    buffer += "  "
                    i += 2
                    continue
                i += 1
                continue
            if not quoted and not hatted and ch == "/" and nxt == "*":
                comment = True
                buffer += "  "
                i += 2
                continue
            if ch == '"' and not hatted and (i == 0 or physical[i - 1] != "\\"):
                quoted = not quoted
            elif ch == "^" and (i == 0 or physical[i - 1] != ":"):
                hatted = not hatted
            if not comment:
                buffer += ch
            i += 1
        if quoted or hatted:
            buffer += "\n"
        else:
            logical.append(buffer.strip())
            buffer = ""
    if buffer:
        logical.append(buffer.strip())

    result: list[dict[str, object]] = []
    section = ""
    zone = default_zone
    active: dict[str, object] | None = None
    directions = {"n": "north", "s": "south", "e": "east", "w": "west",
                  "u": "up", "d": "down"}
    for line in logical:
        if not line or line.startswith("#"):
            continue
        directive = re.match(r"^%\s*([A-Za-z]+)\s*:?[ \t]*(.*)$", line)
        if directive:
            kind, value = directive.groups()
            kind = kind.lower()
            if kind == "zone":
                zone = value.strip().split()[0] if value.strip() else ""
                section = ""
            elif kind in {"locations", "objects", "mobiles"}:
                section = kind
            else:
                section = ""
            active = None
            continue
        if section != "locations":
            continue
        if re.match(r"^\s*(?:lflags\s*\{|altitude\s*=)", line, re.I):
            continue
        # A caret after the colon in `n:^door` belongs to the linked-exit
        # token, so it must not be treated as a room-description delimiter.
        header = line.strip()
        if "=" in header:
            continue
        parts = header.split()
        if not parts:
            continue
        exits = {}
        for token in parts[1:]:
            match = re.fullmatch(r"([nsewud]):(.+?);?", token, re.I)
            if match:
                direction, target = match.groups()
                exits[directions[direction.lower()]] = target.rstrip(";")
        if line.rstrip().endswith(";") or exits:
            active = {"zone": zone.lower(), "name": parts[0].rstrip(";"),
                      "exits": exits}
            result.append(active)
    return result


def zone_entries() -> list[dict[str, object]]:
    scenarios = json.loads(SCENARIOS.read_text()) if SCENARIOS.exists() else []
    scenario_zones: dict[str, list[str]] = {}
    for scenario in scenarios:
        for zone in scenario.get("zones", []):
            scenario_zones.setdefault(zone.lower(), []).append(scenario["id"])
    entries = []
    for path in sorted(ZONES.glob("*.zone")):
        text = path.read_text(errors="replace")
        record_counts = zone_record_counts(text)
        rooms = room_inventory(text, path.stem)
        record_counts["locations"] = len(rooms)
        entries.append({
            "file": path.relative_to(ROOT).as_posix(),
            "zone": path.stem.lower(),
            "records": record_counts,
            "rooms": rooms,
            "runtime_scenarios": scenario_zones.get(path.stem.lower(), []),
        })
    return entries


def render() -> str:
    quest_inventory = quest_entries()
    quests = [entry for entry in quest_inventory if entry.get("index") is not None]
    legacy_hooks = [entry for entry in quest_inventory if entry.get("index") is None]
    zones = zone_entries()
    return json.dumps({
        "source": [
            "data/ZONES/*.zone", "include/quests.h", "include/questnames.h",
            "specials/*.h", "src/*.c", "cr/*.c",
        ],
        "zone_count": len(zones),
        "room_count": sum(zone["records"]["locations"] for zone in zones),
        "declared_exit_count": sum(len(room["exits"]) for zone in zones
                                    for room in zone["rooms"]),
        "linked_exit_count": sum(target.startswith("^") for zone in zones
                                  for room in zone["rooms"]
                                  for target in room["exits"].values()),
        "quest_count": len(quests),
        "legacy_hook_count": len(legacy_hooks),
        "zones": zones,
        "quests": quests,
        "legacy_quest_hooks": legacy_hooks,
    }, indent=2) + "\n"


def main() -> None:
    rendered = render()
    if "--check" in sys.argv:
        if not OUT.exists() or OUT.read_text() != rendered:
            raise SystemExit("world_catalog.json is stale; regenerate it")
        data = json.loads(rendered)
        print(f"Verified {data['zone_count']} zone files, {data['room_count']} rooms, "
              f"{data['declared_exit_count']} exits and {data['quest_count']} quests")
        return
    OUT.write_text(rendered)
    data = json.loads(rendered)
    print(f"Wrote {OUT.relative_to(ROOT)}: {data['zone_count']} zone files, "
          f"{data['room_count']} rooms, {data['declared_exit_count']} exits, "
          f"{data['quest_count']} quests, "
          f"{data['legacy_hook_count']} inactive/optional quest hooks")


if __name__ == "__main__":
    main()
