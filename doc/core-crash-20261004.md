# October 4 background crash

## Evidence

The main server closed both idle player connections at 03:38:54 EEST on
2026-10-04. Docker recorded `buffer overflow detected` and restarted once.
The core from host PID 2020190 was examined with its matching executable
and container libraries. Resolving the libraries exposed the original stack:

```
on_timer -> aberchat_autorecon -> aberchat_boot -> authenticate
         -> aprintf -> __strcat_chk -> __chk_fail -> __fortify_fail -> abort
```

The 4096-byte `abuffer` had no terminating zero and contained 80 repeated
authentication headers. Shutdown retained queued output, and reconnect appended
another authentication packet using an unchecked `strcat`. Formatting into the
temporary buffer also used unchecked `vsprintf`.

The final SIGSEGV occurred while attempting recovery:

```
sig_handler -> run_reboot -> storecom -> test_bit
```

Timer processing had selected the internal context (`mynum == -1`). Wizard-zone
persistence incorrectly used the player command's permission check and read
flags through that invalid index. This explains the abrupt disconnection with
no reliably delivered shutdown message.

## Repairs

- Format outgoing AberChat messages with `vsnprintf`; reject oversized messages
  and queue exhaustion before copying. Log and close the external connection
  rather than copying a truncated packet or overflowing the queue.
- Reset authentication and queued output on shutdown and before a new connection.
- Remove the persistent write pointer. Retain unsent bytes after a partial write;
  retry interrupted or temporarily blocked writes without modifying the queue.
  Close the external connection on permanent write errors.
- Separate permission-checked player storage from internal reboot storage.
  Internal saves retain the zone serialization but never require a player slot.
  Player-initiated saves still require the original clone/store permissions.
- Bound wizard-zone path formatting.

AberChat remains disabled in Docker builds. These changes repair the identified
output queue and recovery index defects, but do not establish that all legacy
signal-handler recovery or external protocol processing is safe. The normal
Docker build passed; gameplay and network regressions have not been run.
