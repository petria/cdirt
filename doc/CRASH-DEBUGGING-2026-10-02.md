# Fountain to Ice Palace crash: diagnosis and fix

## Report and reproduction

The crash was reported after creating a character, following the newbie-zone
tour, taking and wielding the `Newbiesword`, then moving east from `The
Fountain` toward `Ice Palace`. The supplied session transcript,
`play_to_crash.txt`, ends with the connection closing immediately after that
move. In the transcript the sword is still wielded when the character reaches
the Fountain.

The C-Dirt Docker image was rebuilt with debug symbols (`-g3 -ggdb3`) and frame
pointers (`-fno-omit-frame-pointer`). Replaying the session under GDB produced
a SIGSEGV in `set_weapon()` at the access to `pwpn(plr)`. The failing values
were `plr == -1` and object index `1297`.

## Root cause of the movement crash

When a move changes `pzone`, `src/mobile.c` calls `destruct_clones()` to remove
zone-specific objects the player is carrying. `destruct_object()` removes the
object and compacts the global object array by moving its last object into the
freed slot. During this compaction it temporarily calls:

```c
setoloc(from, -1, ocarrf(from));
```

For a wielded object, `ocarrf(from)` is `WIELDED_BY`. `setoloc()` used to call
`set_weapon(loc, obj)` for every `WIELDED_BY` object, including this temporary
`loc == -1` state. `set_weapon()` immediately evaluated `pwpn(plr)`, which
expands to `ublock[plr].pweapon`; with `plr == -1`, this reads before the
`ublock` array and segfaults. This is an invalid-index bug during object-array
compaction, not a failure to load the Ice Palace zone.

`setoloc()` now runs weapon and armor side effects only when the destination is
a valid character index (`0 <= loc < numchars`). `set_weapon()` also rejects
invalid character indices before reading or writing player state. The object
still passes through its temporary `-1` location, then the compaction code
assigns it to its final valid location and updates the wielder normally.

## Crash-recovery overflow found in the same trace

After the SIGSEGV, the crash handler attempted to restart the server. GDB then
reported a separate stack-buffer overflow in `run_reboot()`: the code copied
`VERSION` (`"CDirt 3.0beta3"`) into `REBOOT_REC.version[10]` with `strcpy()`. The
version string is longer than that field, so fortified libc aborted crash
recovery with SIGABRT before the restart could complete.

The record field now uses `sizeof(VERSION)`, so it fits the configured version
string including its terminating NUL. `run_reboot()` uses `snprintf()` with the
actual field size. This fixes the secondary restart failure; it did not cause
the original movement SIGSEGV.

## Scope

The first fault was exposed by the temporary `-1` location used while
compacting objects after clone destruction. The second fault was independent
and only appeared when the server tried to recover from the first. The GDB
reproduction and the supplied player transcript provided the evidence for
both fixes. No player files or core dumps are part of this repository change.
