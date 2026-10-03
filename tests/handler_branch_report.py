#!/usr/bin/env python3
"""Summarize command-only gcov evidence without treating branch hits as assertions."""
from __future__ import annotations
import argparse
import json
from pathlib import Path
from docker_verb_audit import ROOT, fingerprint


def summarize(report: dict) -> dict:
    functions, branches = {}, {}
    for case in report.get('cases', []):
        for source in case.get('coverage', {}).get('files', []):
            filename = source['file']
            for function in source.get('functions', []):
                key = (filename, function['name'])
                item = functions.setdefault(key, dict(file=filename, name=function['name'],
                    line=function.get('start_line'), calls=0, branches=0, taken=0))
                item['calls'] += function.get('execution_count', 0)
            for line in source.get('lines', []):
                name = line.get('function_name')
                for index, branch in enumerate(line.get('branches', [])):
                    key = (filename, line['line_number'], index)
                    item = branches.setdefault(key, dict(file=filename, line=line['line_number'],
                        function=name, index=index, count=0))
                    item['count'] += branch.get('count', 0)
    for branch in branches.values():
        function = functions.get((branch['file'], branch['function']))
        if function:
            function['branches'] += 1
            function['taken'] += branch['count'] > 0
    catalog = json.loads((ROOT / 'tests/verb_catalog.json').read_text())
    handlers = {name for entry in catalog['entries'] for name in entry['handlers']} | {'do_action'}
    routed = sorted((item for item in functions.values() if item['name'] in handlers),
                    key=lambda item: (item['file'], item['line'] or 0, item['name']))
    return dict(handlers=routed, compiler_functions=len(functions),
                compiler_branches=len(branches),
                compiler_branches_taken=sum(item['count'] > 0 for item in branches.values()),
                uncovered_branches=[item for item in branches.values() if not item['count']])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, default=Path('/tmp/cdirt-verb-audit/report.json'))
    parser.add_argument('--output', type=Path, default=Path('/tmp/cdirt-verb-audit/branches.json'))
    args = parser.parse_args()
    report = json.loads(args.report.read_text())
    if report.get('fingerprint') != fingerprint():
        raise SystemExit('Stale evidence: rerun the command audit before reporting coverage')
    result = summarize(report)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    for item in result['handlers']:
        if item['calls']:
            print(f"{item['name']}: {item['calls']} calls, {item['taken']}/{item['branches']} compiler branches")
    print(f"Compiler branch hits: {result['compiler_branches_taken']}/{result['compiler_branches']}")
    print('Branch execution does not establish correct gameplay or complete meaningful branch review.')


if __name__ == '__main__':
    main()
