#!/usr/bin/env python3
"""Run multi-player message-delivery smoke cases in an isolated cdirt:latest."""

from __future__ import annotations

import re
import secrets
import socket
import string
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path

import json

IMAGE = "cdirt:latest"
PASSWORD = "CdirtSmoke991"
ANSI = re.compile(r"\x1b\[[0-?]*[ -/]*[@-~]")
PROMPT = "by what name should i call you?"


def clean(data: bytes) -> str:
    text = data.decode("utf-8", errors="replace")
    text = ANSI.sub("", text).replace("\r", "\n")
    return "".join(ch for ch in text if ch in "\n\t" or " " <= ch <= "~")


@dataclass
class Player:
    name: str
    sock: socket.socket
    received: bytearray

    @classmethod
    def connect(cls, host: str, port: int, name: str) -> "Player":
        sock = socket.create_connection((host, port), timeout=8)
        sock.settimeout(0.15)
        return cls(name, sock, bytearray())

    def close(self) -> None:
        self.sock.close()

    def _read(self, timeout: float, idle: float = 0.15) -> str:
        deadline = time.monotonic() + timeout
        last_data = time.monotonic()
        while time.monotonic() < deadline:
            try:
                part = self.sock.recv(65536)
            except socket.timeout:
                if time.monotonic() - last_data >= idle:
                    break
                continue
            if not part:
                break
            self.received.extend(part)
            last_data = time.monotonic()
        return clean(bytes(self.received))

    def wait_for(self, text: str, timeout: float = 12) -> str:
        deadline = time.monotonic() + timeout
        needle = text.lower()
        while time.monotonic() < deadline:
            current = self._read(min(1.0, deadline - time.monotonic()))
            if needle in current.lower():
                return current
        raise AssertionError(f"{self.name}: timed out waiting for {text!r}; got:\n"
                             f"{clean(bytes(self.received))[-1200:]}")

    def send(self, line: str) -> None:
        self.sock.sendall(line.encode() + b"\r\n")

    def drain(self) -> str:
        result = self._read(1.2, idle=0.4)
        self.received.clear()
        return result

    def command(self, line: str) -> str:
        self.drain()
        self.send(line)
        result = self._read(2.0, idle=0.55)
        self.received.clear()
        return result


def register_player(host: str, port: int, name: str,
                    sock: socket.socket | None = None,
                    password: str = PASSWORD) -> Player:
    if sock:
        sock.settimeout(0.15)
        player = Player(name, sock, bytearray())
    else:
        player = Player.connect(host, port, name)
    try:
        player.wait_for(PROMPT)
        player.send(name)
        player.wait_for("did i hear correctly")
        player.send("y")
        player.wait_for("password:")
        player.send(password)
        player.wait_for("confirm password:")
        player.send(password)
        player.wait_for("do you have color?")
        player.send("y")
        player.wait_for("which class would you like?")
        player.send("w")
        player.wait_for("sex (m/f)")
        player.send("m")
        player._read(1.5, idle=0.5)
        player.send("")  # Continue past the MOTD.
        player._read(3.0, idle=0.7)
        return player
    except Exception:
        player.close()
        raise


def assert_contains(player: Player, output: str, fragment: str) -> None:
    if fragment.lower() not in output.lower():
        raise AssertionError(f"{player.name} did not receive {fragment!r}; got:\n{output}")


def assert_line(player: Player, output: str, expected: str) -> None:
    lines = [line.strip() for line in output.splitlines()]
    if expected not in lines:
        raise AssertionError(f"{player.name} did not receive exact line {expected!r}; "
                             f"got:\n{output}")


def assert_absent(player: Player, output: str, fragment: str) -> None:
    if fragment.lower() in output.lower():
        raise AssertionError(f"{player.name} unexpectedly received {fragment!r}; got:\n{output}")


def run_scenarios(players: dict[str, Player], suffix: str) -> None:
    scenarios = [scenario for scenario in json.loads(
        (Path(__file__).with_name("scenarios.json")).read_text())
        if scenario.get("smoke", True)]
    for scenario in scenarios:
        phrase = scenario["id"].replace("-", "") + suffix
        values = {"phrase": phrase, "actor": players["actor"].name,
                  "target": players["target"].name}
        values.update({key: str(value).format(**values)
                       for key, value in scenario.get("values", {}).items()})
        for setup in scenario.get("setup", []):
            role = setup["role"]
            output = players[role].command(setup["command"].format(**values))
            for line in setup.get("expected_lines", []):
                assert_line(players[role], output, line.format(**values))
        for player in players.values():
            player.drain()
        input_role = scenario.get("input_role", "actor")
        players[input_role].send(scenario["command"].format(**values))
        outputs = {role: player._read(2.0, idle=0.55)
                   for role, player in players.items()}
        for role, template in scenario["expected_lines"].items():
            assert_line(players[role], outputs[role], template.format(**values))
        for role in scenario["audiences"]["exclude"]:
            for template in scenario["excluded_text"]:
                assert_absent(players[role], outputs[role], template.format(**values))
        for followup in scenario.get("followups", []):
            role = followup["role"]
            output = players[role].command(followup["command"].format(**values))
            for line in followup.get("expected_lines", []):
                assert_line(players[role], output, line.format(**values))
            for fragment in followup.get("expected_contains", []):
                assert_contains(players[role], output, fragment.format(**values))


def run_default_route_cases(players: dict[str, Player]) -> int:
    catalog = json.loads((Path(__file__).with_name("verb_catalog.json")).read_text())
    words = [entry["word"] for entry in catalog["entries"]
             if entry["dispatch_kind"] == "doverb-default"]
    for word in words:
        for player in players.values():
            player.drain()
        actor_output = players["actor"].command(word)
        assert_line(players["actor"], actor_output, "You can't do that now.")
        for role in ("target", "same_room_witness", "remote_room"):
            other_output = players[role]._read(0.35, idle=0.15)
            assert_absent(players[role], other_output, "You can't do that now.")
    return len(words)


def available_port() -> int:
    with socket.socket() as probe:
        probe.bind(("127.0.0.1", 0))
        return int(probe.getsockname()[1])


def wait_server(host: str, port: int, container: str) -> socket.socket:
    deadline = time.monotonic() + 90
    while time.monotonic() < deadline:
        try:
            return socket.create_connection((host, port), timeout=1)
        except OSError:
            state = subprocess.run(["docker", "inspect", "-f", "{{.State.Running}}",
                                    container], text=True, capture_output=True)
            if state.returncode == 0 and state.stdout.strip() != "true":
                raise RuntimeError("test server stopped during startup:\n" +
                                   docker_logs(container))
            time.sleep(0.5)
    raise TimeoutError("test server did not begin listening:\n" + docker_logs(container))


def docker_logs(container: str) -> str:
    result = subprocess.run(["docker", "logs", container], text=True,
                            capture_output=True, check=False)
    return (result.stdout + result.stderr)[-4000:]


def run_smoke() -> None:
    suffix = "".join(secrets.choice(string.ascii_uppercase) for _ in range(6))
    container = f"cdirt-verb-smoke-{suffix.lower()}"
    port = available_port()
    subprocess.run(["docker", "run", "--detach", "--name", container,
                    "--publish", f"127.0.0.1:{port}:6715", IMAGE],
                   check=True, stdout=subprocess.DEVNULL)
    time.sleep(2)  # Let the DNS helper and MUD event loop settle.
    players: list[Player] = []
    try:
        initial_socket = wait_server("127.0.0.1", port, container)
        actor = register_player("127.0.0.1", port, f"VAct{suffix}", initial_socket)
        players.append(actor)
        target = register_player("127.0.0.1", port, f"VTgt{suffix}")
        players.append(target)
        witness = register_player("127.0.0.1", port, f"VWit{suffix}")
        players.append(witness)
        remote = register_player("127.0.0.1", port, f"VRem{suffix}")
        players.append(remote)

        # Move the control player through an actual room exit. Start location
        # can vary between configured C-Dirt worlds, so probe the six exits.
        exits_output = remote.command("exits")
        moved = False
        for direction in ("north", "east", "south", "west", "up", "down"):
            remote_output = remote.command(direction)
            lowered = remote_output.lower()
            if "you can't go that way" not in lowered and "you can't just stroll" not in lowered:
                moved = True
                break
        if not moved:
            raise AssertionError("control player could not leave its start room; exits were:\n" +
                                 exits_output)
        sessions = {"actor": actor, "target": target,
                    "same_room_witness": witness, "remote_room": remote}
        default_cases = run_default_route_cases(sessions)
        run_scenarios(sessions, suffix)
        print(f"Docker smoke passed: {default_cases} intentional fallback verbs and "
              "say/sayto recipient delivery")
    except Exception:
        print("Throwaway C-Dirt server log:\n" + docker_logs(container), file=sys.stderr)
        raise
    finally:
        for player in players:
            player.close()
        subprocess.run(["docker", "rm", "--force", container], check=False,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


if __name__ == "__main__":
    try:
        run_smoke()
    except Exception as exc:
        print(f"Docker smoke failed: {exc}", file=sys.stderr)
        raise
