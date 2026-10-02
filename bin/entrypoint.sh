#!/bin/sh
set -eu

# The PID file lives on the persistent data volume, but its PID is local to a
# container and may refer to an unrelated process after replacement.
rm -f /mud/data/pid.6715

exec /mud/bin/aberd -p 6715 -d /mud/data -f -v
