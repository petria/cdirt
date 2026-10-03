#!/usr/bin/env python3
"""Assert verb dispatch, game state, and complete rendered player output."""
from __future__ import annotations

import argparse
import gzip
import hashlib
import json
import os
import re
import shutil
import socket
import subprocess
import tempfile
import time
from pathlib import Path

from docker_smoke import IMAGE, Player, available_port, docker_logs, register_player, wait_server

ROOT = Path(__file__).resolve().parents[1]
ROLES = {"actor": "VActor", "target": "VTarget", "witness": "VWitness", "remote": "VRemote"}


def fingerprint() -> str:
    digest = hashlib.sha256()
    paths = []
    for directory in ("src", "cr", "include", "flags", "cr_inc", "specials", "tests"):
        paths.extend(path for path in (ROOT / directory).rglob("*")
                     if path.suffix in (".c", ".h", ".py", ".json")
                     and path.is_file() and not path.name.startswith(".")
                     and "__pycache__" not in path.parts)
    paths.extend((ROOT / "data" / "ZONES").glob("*.zone"))
    paths.extend(ROOT / name for name in ("Dockerfile", ".makefile.in", "data/verbs.src"))
    for path in sorted(paths):
        # Generated machine headers are tied to the image, not the host tree.
        if path.name in {"config.h", "MACHINE.H", "objects.h", "locations.h", "mobiles.h", "verbs.h"}:
            continue
        digest.update(str(path.relative_to(ROOT)).encode())
        digest.update(path.read_bytes())
    return digest.hexdigest()


class Control:
    def __init__(self, path: Path):
        self.path = path

    def request(self, *args: object) -> dict:
        values = [str(value) for value in args]
        if any(not value or any(c in value for c in "\t\r\n\0") for value in values):
            raise ValueError("invalid control argument")
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as client:
            client.settimeout(5)
            client.connect(str(self.path))
            client.sendall(("\t".join(values) + "\n").encode())
            response = bytearray()
            while True:
                part = client.recv(65536)
                if not part:
                    break
                response.extend(part)
        result = json.loads(response)
        if "error" in result:
            raise AssertionError(f"control {values!r}: {result['error']}")
        return result


def wire(text: str) -> str:
    """C-Dirt renders LF as LF followed by CR; preserve all other characters."""
    return text.replace("\n", "\n\r")


def assert_output(receipt: dict, expected: dict, names: dict) -> None:
    if receipt.get("overflow"):
        raise AssertionError("audit capture overflow")
    actual = receipt["output"]
    for role, name in names.items():
        wanted = wire(expected.get(role, "").format_map(names))
        got = actual.get(name, "")
        if got != wanted:
            raise AssertionError(f"{role} output differs: expected {wanted!r}, actual {got!r}")
    extra = set(actual) - set(names.values())
    if extra:
        raise AssertionError(f"unexpected recipients: {sorted(extra)}")


def assert_state(actual: dict, expected: dict, before: dict | None = None) -> None:
    for field, wanted in expected.items():
        got = actual.get(field)
        if isinstance(wanted, dict):
            if "contains" in wanted and not set(wanted["contains"]).issubset(got):
                raise AssertionError(f"{field}: expected members {wanted['contains']!r}, got {got!r}")
            if "excludes" in wanted and set(wanted["excludes"]) & set(got):
                raise AssertionError(f"{field}: forbidden members {wanted['excludes']!r}, got {got!r}")
            if wanted.get("unchanged") and (before is None or got != before[field]):
                raise AssertionError(f"{field}: changed on a rejection path")
        elif got != wanted:
            raise AssertionError(f"{field}: expected {wanted!r}, got {got!r}")


def format_value(value: object, names: dict) -> object:
    return value.format_map(names) if isinstance(value, str) else value


def resolve_state(value: object, control: Control, names: dict) -> object:
    if isinstance(value, dict):
        if "ref" in value:
            kind, identity, field = value["ref"]
            return control.request(kind, format_value(identity, names))[field]
        return {key: resolve_state(item, control, names) for key, item in value.items()}
    if isinstance(value, list):
        return [resolve_state(item, control, names) for item in value]
    return format_value(value, names)


def collect_coverage(container: str, folder: Path) -> dict:
    control_dir = folder / "control"
    for directory in ("src", "cr"):
        dest = control_dir / "gcov" / directory
        dest.mkdir(parents=True, exist_ok=True)
        subprocess.run(["docker", "exec", "--user", "0", container, "sh", "-c",
                        f"cp /mud/{directory}/*.gcno /audit/gcov/{directory}/"],
                       check=True, capture_output=True)
        files = subprocess.check_output(["docker", "exec", "--user", "0", container, "sh", "-c",
                                        f"ls /mud/{directory}/*.c"], text=True).splitlines()
        result = subprocess.run(["docker", "exec", "--user", "0", "--workdir", "/audit", container,
                                 "gcov", "--json-format", "--branch-probabilities",
                                 "--branch-counts", "--object-directory",
                                 f"/audit/gcov/{directory}", *files],
                                text=True, capture_output=True)
        # Some sources are generator-only and consequently have no gcno.
        (folder / f"gcov-{directory}.log").write_text(result.stdout + result.stderr)
    files = []
    for path in sorted(control_dir.glob("*.gcov.json.gz")):
        with gzip.open(path, "rt") as handle:
            data = json.load(handle)
        files.extend(data["files"])
    return {"files": files}


def run_case(case: dict, artifact: Path, catalog: dict) -> dict:
    artifact.mkdir(parents=True, exist_ok=True)
    folder = Path(tempfile.mkdtemp(prefix="cdirt-va-"))
    mount = folder / "control"
    mount.mkdir(mode=0o777)
    mount.chmod(0o777)
    for directory in (mount / "gcov", mount / "gcov/src", mount / "gcov/cr"):
        directory.mkdir(mode=0o777, exist_ok=True)
        directory.chmod(0o777)
    container = f"cdirt-verbs-{os.getpid()}-{case['id']}"
    port = available_port()
    names = dict(ROLES)
    names.update(case.get("names", {}))
    players: dict[str, Player] = {}
    report = {"id": case["id"], "passed": False, "steps": [], "coverage": {}}
    subprocess.run(["docker", "run", "--detach", "--name", container,
                    "--publish", f"127.0.0.1:{port}:6715", "--volume", f"{mount}:/audit",
                    "--env", "CDIRT_AUDIT_SOCKET=/audit/control.sock",
                    "--env", "GCOV_PREFIX=/audit/gcov",
                    "--env", "GCOV_PREFIX_STRIP=1", IMAGE], check=True, capture_output=True)
    try:
        time.sleep(2)  # Same DNS/ident startup allowance as the existing runners.
        from compiled_dispatch import compiled_routes
        report["compiled_dispatch"] = compiled_routes(container, artifact)
        first = wait_server("127.0.0.1", port, container)
        for index, (role, name) in enumerate(names.items()):
            players[role] = register_player("127.0.0.1", port, name,
                                           first if index == 0 else None,
                                           password="AberMUD" if name == "Master" else "VerbAudit991")
        control = Control(mount / "control.sock")
        control.request("ping")
        control.request("timers", "manual")
        names = {role: control.request("player", name)["name"] for role, name in names.items()}
        control.request("seed", case.get("seed", 1))
        for role, name in names.items():
            room = "sherwood3@sherwood" if role == "remote" else "sherwood4@sherwood"
            for field, value in (("room", room), ("sflag:Color", 0), ("sflag:NewStyle", 1),
                                 ("sflag:Brief", 0), ("visibility", 0), ("level", 10)):
                control.request("fixture", "player", name, field, value)
        for fixture in case.get("prepare", []):
            control.request("fixture", *resolve_state(fixture, control, names))
        for player in players.values():
            player.drain()
        control.request("coverage_reset")
        for step in case["steps"]:
            step_result = {"case": step["case"], "command": step["command"], "passed": False}
            report["steps"].append(step_result)
            for fixture in step.get("prepare", []):
                control.request("fixture", *resolve_state(fixture, control, names))
            before = {}
            for assertion in step.get("state", []):
                query = [format_value(value, names) for value in assertion["entity"]]
                before[tuple(query)] = control.request(*query)
            for player in players.values():
                # Retain the raw socket bytes for diagnosis; no text cleaning here.
                player.sock.settimeout(0.01)
                while True:
                    try:
                        if not player.sock.recv(65536): break
                    except socket.timeout: break
            mark = control.request("mark")["serial"]
            role = step.get("actor", "actor")
            command = step["command"].format_map(names)
            players[role].send(command)
            deadline = time.monotonic() + 8
            while True:
                receipt = control.request("receipt")
                if receipt["serial"] > mark: break
                if time.monotonic() > deadline:
                    raise AssertionError(f"command did not complete: {command!r}")
                time.sleep(0.02)
            step_result["receipt"] = receipt
            step_result["wire_files"] = {}
            for recipient, player in players.items():
                raw = bytearray()
                player.sock.settimeout(0.1)
                try:
                    while True:
                        part = player.sock.recv(65536)
                        if not part: break
                        raw.extend(part)
                except OSError:
                    pass
                filename = f"{len(report['steps']):03}-{recipient}-wire.bin"
                (artifact / filename).write_bytes(raw)
                step_result["wire_files"][recipient] = filename
            word = re.match(r"\s*([A-Za-z]+)", command)
            entry = catalog.get(step.get("verb", word[1].lower() if word else ""))
            if not entry and not step.get("action"):
                raise AssertionError(f"scenario must declare resolved verb for {command!r}")
            if step.get("action"):
                actor_id = control.request("player", names[role])["id"]
                if not any(t["actor"] == actor_id and t.get("action") == step["action"]
                           for t in receipt["trace"]):
                    raise AssertionError(f"wrong action route: {receipt['trace']!r}")
                step_result.update(route="action:" + step["action"], handler="do_action")
            if entry:
                actor_id = control.request("player", names[role])["id"]
                if not any(t["actor"] == actor_id and t["verb"] == entry["verb_id"]
                           for t in receipt["trace"]):
                    raise AssertionError(f"wrong route for {command!r}: {receipt['trace']!r}")
                literal = word[1].lower() if word else ""
                step_result.update(word=literal if literal in catalog else None,
                                   resolved_word=entry["word"], route=entry["route_id"],
                                   handler=entry["primary_handler"])
            assert_output(receipt, step["output"], names)
            snapshots = {}
            for assertion in step.get("state", []):
                query = [format_value(value, names) for value in assertion["entity"]]
                actual = control.request(*query)
                expected = resolve_state(assertion["expect"], control, names)
                if assertion.get("same"):
                    if actual != before[tuple(query)]:
                        raise AssertionError(f"state unexpectedly changed: {query!r}")
                assert_state(actual, expected, before[tuple(query)])
                snapshots["/".join(query)] = actual
            step_result.update(passed=True, receipt=receipt, state=snapshots,
                               output_asserted=True, state_asserted=bool(step.get("state")))
        report["passed"] = True
    except Exception as exc:
        report["failure"] = str(exc)
    finally:
        if "control" in locals():
            try:
                control.request("coverage")
                report["coverage"] = collect_coverage(container, folder)
                if not report["coverage"].get("files"):
                    raise AssertionError("compiler coverage produced no files")
            except Exception as exc:
                report["coverage_error"] = str(exc)
                report["passed"] = False
        for role, player in players.items():
            raw = bytearray()
            player.sock.settimeout(0.1)
            try:
                while True:
                    part = player.sock.recv(65536)
                    if not part: break
                    raw.extend(part)
            except OSError:
                pass
            (artifact / f"{role}-final-wire.bin").write_bytes(raw)
        report["server_log"] = docker_logs(container)
        (artifact / "case.json").write_text(json.dumps(report, indent=2) + "\n")
        for player in players.values(): player.close()
        subprocess.run(["docker", "rm", "--force", container], capture_output=True)
        # Copy evidence before removing the private socket/coverage mount.
        for path in folder.glob("gcov-*.log"):
            shutil.copy(path, artifact / path.name)
        shutil.rmtree(folder, ignore_errors=True)
    return report


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--case", action="append", help="scenario ID; repeatable")
    parser.add_argument("--report", type=Path, default=Path("/tmp/cdirt-verb-audit/report.json"))
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args()
    contracts = json.loads((ROOT / "tests/verb_contracts.json").read_text())
    catalog = {entry["word"]: entry for entry in json.loads((ROOT / "tests/verb_catalog.json").read_text())["entries"]}
    cases = [case for case in contracts["scenarios"] if not args.case or case["id"] in args.case]
    if not cases: parser.error("no matching scenarios")
    args.report.parent.mkdir(parents=True, exist_ok=True)
    result = {"fingerprint": fingerprint(), "image": subprocess.check_output(
        ["docker", "image", "inspect", "--format", "{{.Id}}", IMAGE], text=True).strip(), "cases": []}
    for case in cases:
        report = run_case(case, args.report.parent / case["id"], catalog)
        result["cases"].append(report)
        args.report.write_text(json.dumps(result, indent=2) + "\n")
        print(f"{'PASS' if report['passed'] else 'FAIL'} {case['id']}: "
              f"{report.get('failure', str(len(report['steps'])) + ' steps')}", flush=True)
    failed = any(not case["passed"] for case in result["cases"])
    if args.strict:
        executed = {step.get("word") for case in result["cases"] for step in case["steps"] if step["passed"]}
        pending = [entry for entry in contracts["routes"] if entry["status"] != "reviewed"]
        failed |= bool(set(catalog) - executed or pending)
    raise SystemExit(1 if failed else 0)


if __name__ == "__main__":
    main()
