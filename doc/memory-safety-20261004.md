# C-Dirt allocation and timer memory repair

This change follows the static timer/allocation audit against `236792e`.
The live `cdirt` container on port 6715 is not replaced by this work.

## Repairs

| Area | Defect | Repair |
| --- | --- | --- |
| Allocation macros | Unchecked failures, repeated argument evaluation, unchecked sizes | Shared `memory.h` helpers check multiplication/addition, evaluate arguments once, return NULL for empty allocations and exit cleanly on failure. |
| Collection growth | Failed allocation followed by memcpy; absent integer removal reads past end | Checked allocate/copy/free, update capacity after allocation, bound search before indexing; clear freed collection metadata. |
| World growth | Integer capacity arithmetic; cached ublock pointer can point at old allocation | Guard sizes before arithmetic and rebase cached pointer. |
| Description loader | Wrong old copy size, dangling append pointer after resize, close-and-continue, empty-input underflow | Read successful chunks into an owned builder; commit replacement only after validation/read success; close once; remove only a present final LF. |
| Player records | `long *` casts overwrite adjacent `int` fields on 64-bit systems; unbounded fixed strings | Explicit int/unsigned/time/string descriptors, strict range parsing, fixed destination lengths, cleanup on corrupt records. Keep token names and decimal file schema. |
| Formatted sends/logs | Fixed 256/2048/4096-byte `vsprintf` destinations | Determine length with `vsnprintf`, allocate full text, deliver synchronously and free. |
| Output formatter | A single reservation cannot cover color expansion, LF/CR, recursion and large text | Reserve before every write group and final terminator; preserve/rebase read, write and active destination offsets. Local allocated special arguments replace a shared 256-byte array; malformed codes stop at NUL. |
| File substitutions/filter | Expand into unknown-capacity caller arrays; fixed profanity buffers | Owned builder results and synchronous consumption; preserve filter token spacing and random choices. |
| Prompt | Accepted 60-byte templates overflow 90/100-byte render/pager buffers | Derived `EXPANDED_PROMPT_LEN` covers maximal substitutions and wrappers in all stored prompt/pager fields; empty expansion avoids preceding-byte read. Raw input limit remains 60. |
| Travel text | Valid repeated substitutions overflow static 106-byte result; NULL name and empty text bugs | Dynamic borrowed scratch replaced on next call, conditional substitutions; clear freed template on reset. |
| Combat/magic/social | Unbounded substitution arrays and expanded messages reused as printf formats | Build full strings, preserve substitution choices/audiences, pass rendered messages through literal `%s`. |
| HTML/WHO | Title expansion exceeds 256/300-byte arrays; padding writes one byte past stored mud-name buffer | Owned title, WHO and padding strings; preserve historical extra padding space; clamp backwards line-break search at its start. |
| Splotch | 255-byte replacement buffer, leak, unknown destination size, fixed response/word expansion, shift/lookahead overrun | Owned expanded input/response/match words, resize on replacement/shift, bounded lookahead/table size, safe random-file path and empty-file handling. |
| Editor/mail | Newline append beyond line field, sendmail formatting into 200 bytes, reply-prefix growth, stale/unused nodes | Owned exact editor lines/mail subjects; free with nodes; exact command/title formatting; complete owned subject/body reads, bounded fixed header fields and explicit borrowed work-message handling. |
| File tracking | Allocated node contains fixed 20-byte source and 256-byte path fields; 8-byte mode display receives `read/write` | Own full source/path/mode strings, release on close/open failure, correct mode-display capacity. |
| Clones | Name suffix and description formatting into fixed arrays | Exact owned unique-name and description formatting; cloned collection heads retain separate ownership. |
| Voting/wizard/frob | Extra voter outside pointer array, unbounded parsed names, leaked discarded records, too-small mobile-name field | Extra current-voter slot, bounded successful record reads, cleanup on failure/demotion, mobile-sized frob field. |
| Spell lifecycle | Allocate then discard head; wipe leaves freed head | Remove discarded allocation; detach duration head before callbacks/free; do not change normal checks or expiry links. |
| Timer selection | 256 entries while configuration permits 1000 slots; fixed spell-choice array | Size selection arrays from player slots and spell table. |
| Timer diagnostics/indexes | Missing holy-symbol `%s` argument; invalid combat opponent and close slot dereferences | Supply NPC name and guard indexes before accessing arrays. |
| Socket writes | Negative result used as pointer offset; terminator-based queue progress | Handle EINTR/EAGAIN without mutation, close on permanent error, advance only positive bytes, compare read/write positions. Keep a safe select width including listener/control descriptors. |
| IDENT/DNS | Overlapping username concatenation, unbounded scans, negative/partial offset updates, stale reconnect state, bad DNS cache chain | Separate identity temporary, bounded accumulated IDENT reply/fields, fixed DNS frames with explicit offsets/reset, slot checks, correct cache chain. |

## Ownership contracts

`Text` owns its buffer. `text_take()` transfers it. Formatted send/log,
file-code, social, combat, editor-line and mail-subject buffers have explicit
consumers/releases. `make_title`, `build_setin`, `make_magic_msg` and Splotch
`lower` return borrowed scratch, valid until the next call to that function;
callers consume/copy it synchronously. `build_prompt` returns borrowed static
storage with a derived capacity for accepted templates. World data and lookup
nodes retain their existing lifetime; cloned collections are independent.

`tests/allocation_inventory.json` lists every active textual allocation/helper
site under src/cr/include/cr_inc, including declarations and definitions.
The companion script regenerates it and marks newly discovered files as
unreviewed. Its classifications record static capacity/ownership review; the
inventory is not a proof of every possible execution or lifetime.

## Verification

The focused C helper tests compile the actual repaired source functions with
AddressSanitizer and UndefinedBehaviorSanitizer. They cover:

- empty allocation, argument single evaluation, overflow failure, grow/shrink,
  absent integer removal and repeated collection release;
- full repeated name/health/travel expansion, empty templates, literal title
  formatting, an 80 KB description, invalid replacement and absent final LF;
- 460 KB colored output, recursion, long special arguments, malformed codes,
  offset rebasing and LF-then-CR rendering;
- 32-bit int/unsigned player fields next to each other, unsigned maximum,
  64-bit timestamp round trip, out-of-range/overlong corrupt records;
- 10 KB editor input, bounded legacy reads and full 10 KB mail subject/body loads, EINTR/EAGAIN/partial/permanent socket
  write failures, surviving spell duration/head expiry/repeated wipe;
- 40 KB Splotch substitution and growing grammatical replacements;
- full-length tracked source strings and failed-open/close cleanup.

The Docker memory runner uses an isolated audit-enabled image and private Unix
socket. It exercises repeated prompts and travel text, timer-driven combat,
duration expiry/wipe and 180 ticks including HTML/weather/regeneration. The
HTML fixture uses a valid 255-byte literal title plus `%s`, yielding 260 visible
bytes for Rydis. Production builds contain neither fixtures nor audit sockets.

Existing exact-output verb contracts, all 24 quest scenarios and the full
2,916-room / 6,990-exit walk are also replayed in disposable containers.
Quest audit shortcuts use goto/zap and do not establish ordinary travel or
combat coverage. Verb contracts cover 62 steps, not every branch of 390 verbs.

Build sanitizer/audit image using only the supported tag:

```sh
docker build -t cdirt:latest \
  --build-arg 'CDIRT_CFLAGS=-O1 -g3 -fno-omit-frame-pointer -fcommon -DCDIRT_DOCKER -DCDIRT_AUDIT -DCDIRT_COVERAGE --coverage -fsanitize=address,undefined -fno-sanitize-recover=all -fno-pie' \
  --build-arg 'CDIRT_LDFLAGS=-lm -lcrypt --coverage -fsanitize=address,undefined -no-pie' \
  --build-arg CDIRT_ASAN_OPTIONS=detect_leaks=0 .
python3 -m unittest discover -s tests -v
python3 tests/docker_memory_audit.py
python3 tests/docker_verb_audit.py
python3 tests/docker_quest_audit.py
python3 tests/docker_quest_smoke.py
python3 tests/docker_zone_walk.py --strict
# Restore the normal image after isolated verification; do not restart cdirt.
docker build -t cdirt:latest .
```

Generator LeakSanitizer reporting is disabled because it retains generated
world data until process exit. The tests distinguish invalid accesses/UB from
lifetime/leak auditing; no claim of a leak-free engine or exhaustive safety is
made. Weather logic, timer scheduling/starvation, arbitrary shell-filter
semantics and async-signal-safe crash recovery are outside these repairs.

## Results from this implementation

- 33 unit checks passed, including 11 new allocation/ownership boundary checks
  (the C helper programs run with ASan/UBSan).
- All 62 existing verb-contract steps passed with exact output comparisons.
- All 24 quest scenarios passed (23-route audit plus Excalibur).
- Full world walk passed: 2,916 rooms and 6,990 declared exits.
- The timer/memory runner passed all six scenarios, including actor-only spell
  expiry, repeated template resets and the 260-byte HTML title.
- The allocator inventory contains 212 textual sites and no unclassified files.

Detailed disposable-run artifacts are under `/tmp/cdirt-memory-audit/`, with
build/unit/runner logs in `/tmp/cdirt-memory-*.log`. These results cover the
listed cases; they do not prove every verb branch or timer event correct.

## Existing LP64 player records: deployment follow-up

The first deployment exposed a migration omission: the old saver also read
eight bytes through each `long *` descriptor pointing to a four-byte field.
On this little-endian LP64 host, the decimal file value therefore contained
the intended field in its low 32 bits and an adjacent field in its high bits.
The new strict loader rejected these oversized values. For example Rydis's
`Damage 94489280520` represented damage 8 combined with adjacent armor 22.
`Level 51539607556` represented level 4 combined with weapon index 12.

Use `utils/migrate_lp64_players.py` for records known to originate from that
old little-endian 64-bit saver. Stop the MUD and retain a full data backup
first. The default is a dry run; applying requires explicit format selection
and a fresh backup directory:

```sh
python3 utils/migrate_lp64_players.py /path/to/players
python3 utils/migrate_lp64_players.py /path/to/players \
  --apply --legacy-lp64 --backup-dir /path/to/fresh-record-backups
```

The migration extracts the signed or unsigned low 32 bits for integer fields;
timestamps, password hashes, text, flags and other lines remain byte-for-byte
unchanged. It validates candidates before writing, backs up all affected
records, and replaces each file atomically. Run under the file owner or root
to retain ownership; when copying repaired files into Docker restore their
`mud` ownership. Do not use it on an unknown format or an unrelated corrupt
record. It cannot reconstruct game state already lost to earlier corruption.
The daemon's strict numeric validation remains in place.

Three live records required migration: Exquest, Master and Rydis. The complete
pre-migration data is retained at
`/tmp/cdirt-lp64-migration-20261004T083237Z/original-data`. Port 6715 was
restarted with the existing normal `cdirt:latest` image after the data repair.
A socket login with Rydis now reaches the password prompt; no password was
submitted during that check.
