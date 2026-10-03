#!/usr/bin/env python3
"""Audit every compiled C-Dirt room and its live exit list.

Runs only against a throwaway cdirt:latest container. The normal privileged
`exits <room>@<zone>` command exercises compiled room lookup and exit resolution
without triggering room traps or combat.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import secrets
import string
import subprocess
import sys
import time
from pathlib import Path

from docker_smoke import (IMAGE, Player, available_port, clean, docker_logs,
                          register_player, wait_server)

ROOT = Path(__file__).resolve().parents[1]
UNVEIL_PASS = os.environ.get("CDIRT_TEST_UNVEIL_PASS", "AberMUD")
ROOM_ERRORS = ("non-existant room or exit", "no such zone", "no such location")


def command(player: Player, line: str) -> str:
    """Issue one command and wait until C-Dirt emits the next input prompt."""
    player.received.clear()
    player.send(line)
    deadline = time.monotonic() + 8.0
    while time.monotonic() < deadline:
        player._read(min(0.5, deadline - time.monotonic()), idle=0.12)
        if clean(bytes(player.received)).endswith("C: "):
            break
    output = clean(bytes(player.received))
    player.received.clear()
    return output


def elevate_auditor(player: Player) -> None:
    """Use the configured unveil password so traps and private rooms are audited safely."""
    player.received.clear()
    player.send("unveil 90000")  # LVL_GOD bypasses mortal death and private-room gates.
    deadline = time.monotonic() + 10.0
    while time.monotonic() < deadline:
        player._read(min(0.5, deadline - time.monotonic()), idle=0.12)
        if "magic word:" in clean(bytes(player.received)).casefold():
            break
    if "magic word:" not in clean(bytes(player.received)).casefold():
        raise AssertionError("unveil did not request the configured magic word: "
                             + clean(bytes(player.received)))
    player.received.clear()
    player.send(UNVEIL_PASS)
    deadline = time.monotonic() + 10.0
    while time.monotonic() < deadline:
        player._read(min(0.5, deadline - time.monotonic()), idle=0.12)
        if clean(bytes(player.received)).endswith("C: "):
            break
    output = clean(bytes(player.received))
    player.received.clear()
    if "maximum level" in output.casefold() or "go away" in output.casefold():
        raise AssertionError("could not elevate zone auditor: " + output)


def report_data(results: list[dict[str, object]], all_room_count: int,
                compiled_room_count: int, source_only_rooms: list[dict[str, object]]) -> dict[str, object]:
    failed = [item for item in results if item["room_lookup_errors"] or
              item["missing_ordinary_exits"] or item["exit_command_error"]]
    return {"image": IMAGE, "room_count": len(results),
            "c_generated_room_count": compiled_room_count,
            "source_room_count": all_room_count,
            "source_only_room_count": len(source_only_rooms),
            "source_only_rooms": [f"{room['name']}@{room['zone']}"
                                  for room in source_only_rooms],
            "declared_exit_count": sum(len(row["declared_exits"]) for row in results),
            "linked_exit_count": sum(len(row["linked_exit_directions"]) for row in results),
            "failed_room_count": len(failed), "rooms": results}


def run(limit: int | None, start: int, report_path: Path | None, strict: bool) -> None:
    catalog = json.loads((ROOT / "tests/world_catalog.json").read_text())
    all_rooms = [room for entry in catalog["zones"] for room in entry["rooms"]]
    rooms: list[dict[str, object]] = []
    suffix = "".join(secrets.choice(string.ascii_uppercase) for _ in range(6))
    container = f"cdirt-zone-walk-{suffix.lower()}"
    port = available_port()
    subprocess.run(["docker", "run", "--detach", "--rm", "--name", container,
                    "--publish", f"127.0.0.1:{port}:6715", IMAGE], check=True,
                   stdout=subprocess.DEVNULL)
    time.sleep(2)  # Let the bundled DNS helper and game loop settle.
    player: Player | None = None
    results: list[dict[str, object]] = []
    try:
        initial = wait_server("127.0.0.1", port, container)
        c_header = subprocess.run(["docker", "exec", "--user", "0", container,
                                   "cat", "/mud/include/locations.h"],
                                  text=True, capture_output=True, check=True).stdout
        room_macros = set(re.findall(r"^#define LOC_([^\s]+)\s+\d+",
                                     c_header, re.MULTILINE))
        compiled_room_count = len(room_macros)
        for room in all_rooms:
            macro = f"{room['zone']}_{room['name']}".upper()
            if macro in room_macros:
                rooms.append(room)
        if len(rooms) != compiled_room_count:
            missing = [f"{room['name']}@{room['zone']}" for room in all_rooms
                       if f"{room['zone']}_{room['name']}".upper() not in room_macros]
            raise AssertionError(f"matched {len(rooms)} source rooms to {compiled_room_count} "
                                 f"C-generated rooms; unmatched source rooms: {missing[:30]}")
        source_only_rooms = [room for room in all_rooms if room not in rooms]
        rooms = rooms[start:start + limit] if limit is not None else rooms[start:]
        player = register_player("127.0.0.1", port, "Master", initial,
                                 password=UNVEIL_PASS)
        elevate_auditor(player)
        for index, room in enumerate(rooms, 1):
            reference = f"{room['name']}@{room['zone']}"
            # EXITS accepts a room argument and leaves the actor in place.
            exit_output = command(player, f"exits {reference}")
            room_errors = [message for message in ROOM_ERRORS
                           if message in exit_output.casefold()]
            displayed = {
                direction for direction in ("north", "south", "east", "west", "up", "down")
                if any(re.sub(r"&(?:\+[A-Za-z]|[A-Za-z])", "", line)
                       .strip().casefold().startswith(direction) and ":" in line
                       for line in exit_output.splitlines())
            }
            ordinary = {direction for direction, target in room["exits"].items()
                        if not target.startswith("^")}
            linked = set(room["exits"]) - ordinary
            missing = sorted(ordinary - displayed)
            results.append({"room": reference, "room_lookup_errors": room_errors,
                            "declared_exits": room["exits"],
                            "displayed_directions": sorted(displayed),
                            "missing_ordinary_exits": missing,
                            "linked_exit_directions": sorted(linked),
                            "exit_output_excerpt": (exit_output[:300]
                                                    if ordinary - displayed else ""),
                            "exit_command_error": any(token in exit_output.casefold()
                                                       for token in ("not powerful enough",
                                                                     "aren't powerful enough"))})
            if index % 100 == 0 or index == len(rooms):
                print(f"Visited {index}/{len(rooms)} rooms", flush=True)
                if report_path:
                    report_path.write_text(json.dumps(
                        report_data(results, len(all_rooms), compiled_room_count,
                                    source_only_rooms), indent=2) + "\n")

        output = report_data(results, len(all_rooms), compiled_room_count,
                             source_only_rooms)
        failed = [item for item in results if item["room_lookup_errors"] or
                  item["missing_ordinary_exits"] or item["exit_command_error"]]
        rendered = json.dumps(output, indent=2) + "\n"
        if report_path:
            report_path.write_text(rendered)
        if failed:
            print(f"Zone walk found {len(failed)} room issues", file=sys.stderr)
            for row in failed[:30]:
                print(f"  {row['room']}: lookup={row['room_lookup_errors']}; "
                      f"missing exits={row['missing_ordinary_exits']}; "
                      f"exit error={row['exit_command_error']}; "
                      f"output={row['exit_output_excerpt']!r}", file=sys.stderr)
            if strict:
                raise AssertionError(f"{len(failed)} room checks failed")
        else:
            print(f"Zone walk passed: {len(results)} rooms; "
                  f"{output['declared_exit_count']} declared exits "
                  f"({output['linked_exit_count']} linked/stateful)")
    except Exception:
        if report_path and rooms:
            report = report_data(results, len(all_rooms), compiled_room_count,
                                 source_only_rooms)
            report["incomplete"] = True
            report["last_completed_room"] = results[-1]["room"] if results else None
            report["interrupted_at_room"] = (f"{rooms[len(results)]['name']}@"
                                              f"{rooms[len(results)]['zone']}"
                                              if len(results) < len(rooms) else None)
            report_path.write_text(json.dumps(report, indent=2) + "\n")
        print("Throwaway C-Dirt server log:\n" + docker_logs(container), file=sys.stderr)
        raise
    finally:
        if player:
            player.close()
        subprocess.run(["docker", "stop", container], check=False,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--limit", type=int, help="visit only the first N rooms")
    parser.add_argument("--start", type=int, default=0,
                        help="start at this zero-based compiled-room inventory offset")
    parser.add_argument("--report", type=Path, help="write the complete JSON audit report")
    parser.add_argument("--strict", action="store_true", help="fail if a room/ordinary exit check fails")
    args = parser.parse_args()
    run(args.limit, args.start, args.report, args.strict)


if __name__ == "__main__":
    main()
