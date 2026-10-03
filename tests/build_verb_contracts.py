#!/usr/bin/env python3
"""Refresh route inventory without manufacturing reviewed expectations."""
import json
from pathlib import Path

TESTS = Path(__file__).resolve().parent


def build() -> dict:
    catalog = json.loads((TESTS / "verb_catalog.json").read_text())
    path = TESTS / "verb_contracts.json"
    current = json.loads(path.read_text()) if path.exists() else {"scenarios": [], "routes": []}
    existing = {route["id"]: route for route in current["routes"]}
    routes = {}
    for entry in catalog["entries"]:
        # Fallback words retain distinct obligations, despite sharing the handler.
        identity = entry["route_id"] if entry["route_kind"] != "fallback" else "fallback:" + entry["word"]
        if identity not in routes:
            route = {"id": identity, "kind": entry["route_kind"], "words": [],
                     "handlers": entry["handlers"], "source": entry["handler_sources"],
                     "status": "unreviewed", "branches": []}
            route.update({key: value for key, value in existing.get(identity, {}).items()
                          if key in {"status", "branches", "authority", "notes"}})
            routes[identity] = route
        routes[identity]["words"].append(entry["word"])
    return {"version": 1, "routes": list(routes.values()), "scenarios": current["scenarios"]}


if __name__ == "__main__":
    data = build()
    (TESTS / "verb_contracts.json").write_text(json.dumps(data, indent=2) + "\n")
    print(f"Inventoried {len(data['routes'])} distinct routes; review and execution tracked separately")
