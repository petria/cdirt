#!/bin/sh
set -eu

# The PID file lives on the persistent data volume, but its PID is local to a
# container and may refer to an unrelated process after replacement.
rm -f /mud/data/pid.6715

# The daemon defers login until the bundled DNS service resolves the client's
# address. Start the companion service inside this container before the MUD.
/mud/bin/dns-serv

exec /mud/bin/aberd -p 6715 -d /mud/data -f -v
