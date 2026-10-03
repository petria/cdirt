"""Exercise the actual legacy C helpers with boundary inputs."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def function(path, signature):
    source = (ROOT / path).read_text()
    start = source.index(signature)
    return source[start:source.index('\n}\n', start) + 3]


class CBoundsTests(unittest.TestCase):
    def test_zone_prepass_and_ban_match_boundaries(self):
        compiler = shutil.which('gcc')
        if not compiler:
            self.skipTest('gcc required for direct C helper regression')
        source = r'''
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int Boolean;
#define True 1
#define False 0
#define PAGELEN 16384
static int xzon;
#define MAX_FDS 8
static int max_players = 2, sock_fds[MAX_FDS], playerfd[2];
#define fildes(I) (playerfd[I])
static const char *zname(int unused) { return "test"; }
'''
        source += function('src/bootstrap.c', 'char *prepass(')
        source += function('src/utils.c', 'Boolean match (')
        source += function('src/mud.c', 'int find_pl_index (')
        source += r'''
int main(void) {
  Boolean c = 0, q = 0, h = 0;
  char longtext[512];
  int i;
  for (i = 0; i < MAX_FDS; i++) sock_fds[i] = -1;
  assert(find_pl_index(-1) == -1);
  assert(find_pl_index(MAX_FDS) == -1);
  assert(find_pl_index(0) == -1);
  sock_fds[3] = max_players;
  assert(find_pl_index(3) == -1);
  sock_fds[3] = 0; playerfd[0] = 3;
  assert(find_pl_index(3) == 0);
  playerfd[0] = 4;
  assert(find_pl_index(3) == -1);
  memset(longtext, 'x', sizeof(longtext)-1);
  longtext[sizeof(longtext)-1] = 0;
  assert(!match("", "Player"));
  assert(!match("\n", "Player"));
  assert(match("", ""));
  assert(match("PLAYER\n", "player"));
  assert(match("Play*\n", "Player"));
  assert(match("*ayer\n", "Player"));
  assert(!match("Other*", "Player"));
  assert(match(longtext, longtext));
  assert(!strcmp(prepass(&c, &q, &h, ""), ""));
  assert(!strcmp(prepass(&c, &q, &h, "\n"), ""));
  assert(!strcmp(prepass(&c, &q, &h, "/*comment*/\n"), ""));
  assert(!strcmp(prepass(&c, &q, &h, "  \t\n"), ""));
  prepass(&c, &q, &h, "Examine = ^\n");
  assert(h);
  prepass(&c, &q, &h, "text\n");
  assert(h);
  prepass(&c, &q, &h, "^\n");
  assert(!h);
  assert(!strcmp(prepass(&c, &q, &h, "North:^door\n"), "North:^door"));
  assert(!h);
  return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='cdirt-c-bounds-') as folder:
            path = Path(folder)
            (path / 'bounds.c').write_text(source)
            subprocess.run([compiler, '-g', '-fsanitize=address,undefined',
                            '-fno-sanitize-recover=all', str(path / 'bounds.c'),
                            '-o', str(path / 'bounds')], check=True, capture_output=True)
            env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
            result = subprocess.run([str(path / 'bounds')], env=env, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
