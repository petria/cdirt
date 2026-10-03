#!/usr/bin/env python3
"""Build a source-derived inventory of C-Dirt verbs and doverb routes."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERBS = ROOT / "data" / "verbs.src"
PARSE = ROOT / "src" / "parse.c"
CLIENT = ROOT / "cr" / "client.c"
FLAGS = ROOT / "cr" / "flags.c"
OUT = ROOT / "tests" / "verb_catalog.json"
SCENARIOS = ROOT / "tests" / "scenarios.json"

CONTROL = {"if", "else", "for", "while", "switch", "return", "sizeof"}


def read_verbs() -> list[dict[str, object]]:
    entries: list[dict[str, object]] = []
    ids: dict[str, int] = {}
    next_id = 0
    for line_no, line in enumerate(VERBS.read_text().splitlines(), 1):
        match = re.match(r"\s*([A-Za-z]+)(?:\s*=\s*([A-Za-z]+))?", line)
        if not match:
            continue
        name, alias_of = match.groups()
        name = name.lower()
        canonical = (alias_of or name).lower()
        if alias_of and canonical in ids:
            verb_id = ids[canonical]
        else:
            next_id += 1
            verb_id = next_id
            if alias_of:
                ids[canonical] = verb_id
        ids[name] = verb_id
        entries.append({
            "line": line_no,
            "word": name,
            "alias_of": canonical if alias_of else None,
            "verb_id": verb_id,
        })
    return entries


def switch_body(source: str) -> str:
    # doverb has a small pre-dispatch switch and the main routing switch.
    start = source.index("void doverb (int vb)")
    pos = start
    switches: list[int] = []
    while pos >= 0:
        candidates = [
            candidate for candidate in
            (source.find("switch(vb)", pos), source.find("switch (vb)", pos))
            if candidate >= 0
        ]
        if not candidates:
            break
        pos = min(candidates)
        switches.append(pos)
        pos += 1
    if not switches:
        raise ValueError("could not locate doverb switch")
    switch_at = switches[-1]
    opening = source.index("{", switch_at)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[opening + 1:index]
    raise ValueError("unterminated doverb switch")


def direct_calls(block: str) -> list[str]:
    names: list[str] = []
    for line in block.splitlines():
        match = re.match(r"\s*([A-Za-z_]\w*)\s*\(", line)
        if match and match.group(1) not in CONTROL:
            name = match.group(1)
            if name not in names:
                names.append(name)
    return names


def read_routes() -> dict[str, list[str]]:
    body = switch_body(PARSE.read_text())
    labels = list(re.finditer(r"\bcase\s+VERB_([A-Z0-9_]+)\s*:", body))
    routes: dict[str, list[str]] = {}
    pending: list[str] = []
    for index, label in enumerate(labels):
        pending.append(label.group(1))
        end = labels[index + 1].start() if index + 1 < len(labels) else len(body)
        calls = direct_calls(body[label.end():end])
        if calls:
            for verb in pending:
                routes[verb] = calls
            pending = []
    for verb in pending:
        routes[verb] = []

    # These commands are consumed before the main doverb switch.
    routes.update(read_switch_routes(CLIENT, "Boolean aberchat_parse"))
    routes.update(read_switch_routes(FLAGS, "Boolean flags_parse"))
    for direction in ("NORTH", "EAST", "SOUTH", "WEST", "UP", "DOWN"):
        routes[direction] = ["dodirn"]
    return routes


def read_switch_routes(path: Path, function_signature: str) -> dict[str, list[str]]:
    source = path.read_text()
    start = source.index(function_signature)
    opening = source.index("{", start)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                body = source[opening + 1:index]
                break
    else:
        raise ValueError(f"unterminated function {function_signature}")

    labels = list(re.finditer(r"\bcase\s+VERB_([A-Z0-9_]+)\s*:", body))
    routes: dict[str, list[str]] = {}
    pending: list[str] = []
    for index, label in enumerate(labels):
        pending.append(label.group(1))
        end = labels[index + 1].start() if index + 1 < len(labels) else len(body)
        calls = direct_calls(body[label.end():end])
        if calls:
            for verb in pending:
                routes[verb] = calls
            pending = []
    for verb in pending:
        routes[verb] = []
    return routes


def main() -> None:
    entries = read_verbs()
    routes = read_routes()
    scenarios = json.loads(SCENARIOS.read_text())
    scenarios_by_word: dict[str, list[str]] = {}
    for scenario in scenarios:
        for word in scenario["verbs"]:
            scenarios_by_word.setdefault(word, []).append(scenario["id"])
    id_routes: dict[int, tuple[str, list[str]]] = {}
    for entry in entries:
        macro = str(entry["word"]).upper()
        if macro in routes:
            id_routes.setdefault(int(entry["verb_id"]), (macro, routes[macro]))
    for entry in entries:
        macro = str(entry["word"]).upper()
        source_word, calls = id_routes.get(int(entry["verb_id"]), (macro, []))
        entry["dispatch"] = calls or ["doverb_default"]
        entry["primary_handler"] = calls[-1] if calls else "doverb_default"
        entry["dispatch_source_word"] = source_word if calls else "DEFAULT"
        entry["dispatch_kind"] = "c-handler" if calls else "doverb-default"
        entry["test_cases"] = scenarios_by_word.get(str(entry["word"]), [])
        entry["output_cases"] = list(entry["test_cases"])
    output_apis = ("bprintf", "sendf", "sendl", "send_msg", "lsend_msg",
                   "send_magic_msg", "sillycom")
    source_files = list((ROOT / "src").glob("*.c")) + list((ROOT / "cr").glob("*.c"))
    api_counts = {
        api: sum(len(re.findall(rf"\b{api}\s*\(", path.read_text()))
                 for path in source_files)
        for api in output_apis
    }
    data = {
        "source": ["data/verbs.src", "src/parse.c"],
        "verb_count": len(entries),
        "unique_verb_ids": len({entry["verb_id"] for entry in entries}),
        "output_api_call_sites": api_counts,
        "entries": entries,
    }
    rendered = json.dumps(data, indent=2) + "\n"
    if "--check" in sys.argv:
        if not OUT.exists() or OUT.read_text() != rendered:
            raise SystemExit("verb_catalog.json is stale; regenerate it")
        print(f"Verified {len(entries)} words, {data['unique_verb_ids']} verb IDs, "
              f"{len(routes)} C routing labels")
    else:
        OUT.write_text(rendered)
        print(f"Wrote {OUT.relative_to(ROOT)}: {len(entries)} words, "
              f"{data['unique_verb_ids']} verb IDs, {len(routes)} C routing labels")


if __name__ == "__main__":
    main()
