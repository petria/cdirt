#!/usr/bin/env python3
"""Exercise carried and room-target light commands in disposable C-Dirt."""

from __future__ import annotations

import json
import os
import secrets
import string
import subprocess
import sys
import time
from pathlib import Path

from docker_smoke import (IMAGE, Player, assert_contains, assert_line,
                          available_port, docker_logs, register_player,
                          wait_server)

UNVEIL_PASS = os.environ.get("CDIRT_TEST_UNVEIL_PASS", "AberMUD")
TESTS = Path(__file__).resolve().parent


def run() -> None:
    scenario = next(case for case in json.loads(
        (TESTS / "scenarios.json").read_text())
        if case.get("runner") == Path(__file__).name)
    suffix = "".join(secrets.choice(string.ascii_uppercase) for _ in range(6))
    container = f"cdirt-light-audit-{suffix.lower()}"
    port = available_port()
    subprocess.run(
        ["docker", "run", "--detach", "--rm", "--name", container,
         "--publish", f"127.0.0.1:{port}:6715", IMAGE],
        check=True, stdout=subprocess.DEVNULL,
    )
    time.sleep(2)
    players: list[Player] = []
    try:
        initial_socket = wait_server("127.0.0.1", port, container)
        actor = register_player("127.0.0.1", port, "Master", initial_socket,
                                password=UNVEIL_PASS)
        players.append(actor)
        same_room = register_player("127.0.0.1", port, f"LS{suffix}")
        remote = register_player("127.0.0.1", port, f"LR{suffix}")
        players.extend((same_room, remote))

        if "temple of paradise" in remote.command("look").casefold():
            for direction in ("south", "north", "east", "west", "up", "down"):
                remote.command(direction)
                if "temple of paradise" not in remote.command("look").casefold():
                    break
            else:
                raise AssertionError("could not move remote witness out of Start1")

        for setup in scenario["prepare"]:
            output = actor.command(setup["command"])
            for expected in setup.get("expected", []):
                assert_line(actor, output, expected)
            for expected in setup.get("contains", []):
                assert_contains(actor, output, expected)

        summon = actor.command(f"summon {same_room.name}")
        assert_contains(actor, summon, "You cast the summoning")
        assert_contains(same_room, same_room.command("look"), "Temple Of Paradise")

        for index, step in enumerate(scenario["steps"], 1):
            same_room.drain()
            remote.drain()
            output = actor.command(step["command"])
            for expected in step.get("expected", []):
                try:
                    assert_line(actor, output, expected)
                except AssertionError as exc:
                    raise AssertionError(f"step {index} ({step['command']}): {exc}") from exc
            for expected in step.get("contains", []):
                assert_contains(actor, output, expected)
            if step.get("summon_same_room"):
                summon = actor.command(f"summon {same_room.name}")
                assert_contains(actor, summon, "You cast the summoning")
                room_output = same_room.command("look")
                assert_contains(same_room, room_output, "Trophy Room")
            if step.get("actor_only"):
                for observer in (same_room, remote):
                    seen = observer._read(0.45, idle=0.15)
                    if seen.strip():
                        raise AssertionError(
                            f"{step['command']!r} unexpectedly reached "
                            f"{observer.name}:\n{seen}"
                        )

        print(f"Light handler audit passed: {len(scenario['steps'])} transitions; "
              "carried/floor targets, source gating, and actor-only output")
    except Exception:
        print("Throwaway C-Dirt server log:\n" + docker_logs(container),
              file=sys.stderr)
        raise
    finally:
        for player in players:
            player.close()
        subprocess.run(["docker", "stop", container], check=False,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


if __name__ == "__main__":
    try:
        run()
    except Exception as exc:
        print(f"Light handler audit failed: {exc}", file=sys.stderr)
        raise
