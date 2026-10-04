#!/bin/sh
set -eu

cd /mud

CDIRT_CFLAGS=${CDIRT_CFLAGS:--O2 -g3 -ggdb3 -fno-omit-frame-pointer -fcommon -DCDIRT_DOCKER}

# C-Dirt's original configure program is interactive and then starts the
# daemon. Feed it a deterministic local configuration during image build;
# the startup script launches only the game server.
gcc -o /mud/.config /mud/config.c -lcrypt
printf '%s\n' \
  'N' \
  'B' \
  '/mud' \
  'localhost' \
  'Master' \
  "${CDIRT_UNVEIL_PASS:-AberMUD}" \
  'C-Dirt' \
  '6715' \
  'mud@localhost' \
  'gcc' \
  "$CDIRT_CFLAGS" \
  "${CDIRT_LDFLAGS:--lm -lcrypt}" \
  'N' \
  | /mud/.config

mv /mud/.config.pri.tmp /mud/.config.pri
mv /mud/.config.mkfile.tmp /mud/.config.mkfile
# Basic configure defaults enable the historical external AberChat network.
# Docker deployments keep it disabled across rebuilds and container restarts.
sed -i 's/^#define ABERCHAT$/#undef ABERCHAT/' /mud/.config.pri
cat /mud/.config.pri /mud/.config.sec > /mud/include/config.h
cat /mud/.config.mkfile /mud/.makefile.in > /mud/src/Makefile
