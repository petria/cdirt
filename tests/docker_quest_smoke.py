#!/usr/bin/env python3
"""Exercise the C-Dirt Excalibur quest in a disposable server instance."""

from __future__ import annotations

import secrets
import os
import string
import subprocess
import sys
import time

from docker_smoke import (IMAGE, Player, assert_absent, assert_line, available_port,
                          docker_logs, register_player, wait_server)

UNVEIL_PASS = os.environ.get("CDIRT_TEST_UNVEIL_PASS", "AberMUD")


def require(output: str, text: str, phase: str) -> None:
    if text.casefold() not in output.casefold():
        raise AssertionError(f"{phase}: expected {text!r}; received:\n{output}")


def command(player: Player, line: str, expected: str | None = None) -> str:
    output = player.command(line)
    if expected:
        require(output, expected, f"command {line!r}")
    return output


def run() -> None:
    suffix = "".join(secrets.choice(string.ascii_uppercase) for _ in range(6))
    container = f"cdirt-quest-smoke-{suffix.lower()}"
    port = available_port()
    subprocess.run(
        ["docker", "run", "--detach", "--rm", "--name", container,
         "--publish", f"127.0.0.1:{port}:6715", IMAGE],
        check=True, stdout=subprocess.DEVNULL,
    )
    time.sleep(2)
    players: list[Player] = []
    try:
        first = wait_server("127.0.0.1", port, container)
        actor = register_player("127.0.0.1", port, "Master", first,
                                password=UNVEIL_PASS)
        players.append(actor)
        witness = register_player("127.0.0.1", port, f"QWit{suffix}")
        players.append(witness)
        remote = register_player("127.0.0.1", port, f"QRem{suffix}")
        players.append(remote)

        # Teleport only provides deterministic access to the two remote zones;
        # object retrieval, blowing the horn, reward placement, and quest award
        # all go through their regular C command handlers.
        command(actor, "goto lair@labyrinth")
        require(command(actor, "look"), "Lair", "arriving in labyrinth")
        command(actor, "get horn from casket", "You take the horn from the casket.")
        require(command(actor, "inventory"), "horn", "taking horn from casket")

        command(actor, "goto 1@sea")
        require(command(actor, "look"), "All At Sea", "arriving at sea")
        witness.drain()
        remote.drain()
        blow_output = command(actor, "blow horn")
        assert_line(actor, blow_output, "A mighty horn blast echoes around you.")
        assert_line(actor, blow_output,
                    "A hand breaks through the water holding up the sword Excalibur!")
        assert_line(actor, blow_output,
                    "Congratulations! You have completed the quest Excalibur.")
        for observer in (witness, remote):
            observer_output = observer._read(2.0, idle=0.55)
            assert_line(observer, observer_output,
                        "A mighty horn blast echoes around you.")
            assert_absent(observer, observer_output,
                          "Congratulations! You have completed the quest Excalibur.")

        require(command(actor, "look"), "Excalibur", "sword reward placement")
        command(actor, "get Excalibur", "Ok")
        require(command(actor, "inventory"), "Excalibur", "taking sword reward")
        require(command(actor, "quests"), "Excalibur", "quest flag visibility")
        print("Excalibur quest smoke passed: horn retrieval, sea trigger, sword reward, quest flag")
    except Exception:
        print("Throwaway C-Dirt server log:\n" + docker_logs(container), file=sys.stderr)
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
        print(f"Excalibur quest smoke failed: {exc}", file=sys.stderr)
        raise
