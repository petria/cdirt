"""Preserve the compiler's active source and dispatch map for an exact image."""
import json
import shlex
import subprocess
from pathlib import Path
from build_verb_catalog import read_routes


def compiled_routes(container: str, artifact: Path) -> dict:
    config = subprocess.check_output(['docker', 'exec', '--user', '0', container,
                                      'cat', '/mud/src/Makefile'], text=True)
    options = next(line.partition('=')[2].strip() for line in config.splitlines()
                   if line.startswith('COPT ='))
    sources = {}
    for filename in ('src/parse.c', 'cr/client.c', 'cr/flags.c'):
        command = ['docker', 'exec', '--user', '0', container, 'gcc', '-E', '-fdirectives-only',
                   *shlex.split(options), '-I/mud/flags', '-I/mud/include',
                   '-I/mud/cr_inc', '-I/mud/specials', '/mud/' + filename]
        result = subprocess.run(command, text=True, capture_output=True, check=True)
        sources[filename] = result.stdout
        (artifact / (Path(filename).stem + '.active.c')).write_text(result.stdout)
        (artifact / (Path(filename).stem + '.preprocessor.log')).write_text(result.stderr)
    result = dict(compiler_options=options, routes=read_routes(sources))
    (artifact / 'compiled_dispatch.json').write_text(json.dumps(result, indent=2) + '\n')
    return result
