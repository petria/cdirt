#!/usr/bin/env python3
"""Replay verified quest-trigger routes against a disposable cdirt:latest."""

from __future__ import annotations

import json
import secrets
import string
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

from docker_smoke import (IMAGE, Player, available_port, docker_logs,
                          register_player, wait_server)

PASSWORD = "AberMUD"
TESTS = Path(__file__).resolve().parent


def run_case(case: dict) -> str:
    suffix = "".join(secrets.choice(string.ascii_lowercase) for _ in range(6))
    container = f"cdirt-quest-{suffix}"
    port = available_port()
    subprocess.run(
        ["docker", "run", "--detach", "--rm", "--name", container,
         "--publish", f"127.0.0.1:{port}:6715", IMAGE],
        check=True, stdout=subprocess.DEVNULL,
    )
    time.sleep(2)
    actor: Player | None = None
    try:
        sock = wait_server("127.0.0.1", port, container)
        actor = register_player("127.0.0.1", port, "Master", sock,
                                password=PASSWORD)

        result = []
        for command_index, line in enumerate(case["commands"]):
            output = actor.command(line)
            result.append(output)
            if "[Signal Error:" in output:
                raise AssertionError(f"{case['name']}: server signal during {line!r}:\n{output}")
            check = case.get("checks", {}).get(str(command_index), {})
            for fragment in check.get("contains", []):
                if fragment.casefold() not in output.casefold():
                    raise AssertionError(
                        f"{case['name']}: command {line!r} did not return "
                        f"{fragment!r}. Output:\n{output}"
                    )
            for fragment in check.get("absent", []):
                if fragment.casefold() in output.casefold():
                    raise AssertionError(
                        f"{case['name']}: command {line!r} unexpectedly returned "
                        f"{fragment!r}. Output:\n{output}"
                    )

        result.append(actor.command("quests"))
        transcript = "\n".join(result).casefold()
        expected = f"congratulations! you have completed the quest {case['name']}."
        if expected.casefold() not in transcript:
            raise AssertionError(
                f"{case['name']}: expected completion line {expected!r}.\n"
                f"Scenario transcript:\n{transcript[-5000:]}"
            )
        return f"PASS {case['name']} ({case['id']})"
    except Exception:
        logs = docker_logs(container)
        raise RuntimeError(f"{case['name']}: {sys.exc_info()[1]}\n"
                           f"Throwaway C-Dirt server log:\n{logs}") from None
    finally:
        if actor is not None:
            actor.close()
        subprocess.run(["docker", "stop", container], check=False,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def run() -> None:
    cases = [case for case in json.loads((TESTS / "quest_scenarios.json").read_text())
             if case.get("runner") == Path(__file__).name
             and case.get("status") == "runtime"]
    failures = []
    with ThreadPoolExecutor(max_workers=2) as pool:
        futures = {pool.submit(run_case, case): case for case in cases}
        for future in as_completed(futures):
            try:
                print(future.result(), flush=True)
            except Exception as exc:
                failures.append(str(exc))
                print(f"FAIL {futures[future]['name']}", flush=True)

    if failures:
        raise RuntimeError("\n\n".join(failures))
    print(f"Quest trigger audit passed: {len(cases)} scenarios, each in a fresh "
          "disposable server. Goto/zap shortcuts test quest handlers, "
          "not natural travel or combat.", flush=True)


if __name__ == "__main__":
    try:
        run()
    except Exception as exc:
        print(f"Quest trigger audit failed: {exc}", file=sys.stderr)
        raise
