# Verb behavior audit

## Scope and evidence

The audit inventory has 390 registered inputs and 356 distinct numeric verbs.
`verb_contracts.json` separates authored branch obligations from review status.
A route remains partial until its handler, helpers, special hooks, permission
checks, and state gates have all been reviewed and represented by scenarios.
Dynamic social actions are recorded separately from registered verbs.

`docker_verb_audit.py` sends real commands through four player sockets in a
fresh server per scenario. A test-only control socket sets initial conditions
and observes dispatch, room/object/player state, and rendered output. Fixtures
never execute the command under test. Output assertions preserve punctuation,
spacing, ordering, and the game's LF-then-CR line endings. Unmentioned recipients
must receive no command output. Raw socket captures retain prompts and transport
bytes separately from rendered command output.

Timers become manual after login; each server has a fixed random seed. Startup
coverage is reset before the first command. Compiler preprocessing preserves the
active dispatch source and removes inactive conditional branches. Gcov records
actual C function and branch execution. Compiler coverage is supporting evidence;
a branch hit alone does not establish correct game behavior.

The reports identify the image digest and source/scenario fingerprint. Modified
sources invalidate old runtime credit. Strict coverage remains failing while
registered words or fully reviewed route obligations are missing.

## Defects reproduced and repaired

| Defect | Reproduction | Cause and repair |
| --- | --- | --- |
| Closed container opens without retrieving item | `get horn from casket` when closed | An `else if` chain ended after opening. Continue through capacity checking and transfer after opening; locked containers still require a key. |
| Muted player can address somebody | Dumb flag then `sayto <player> hello` | `saytocom` lacked the mute gate present in `saycom`. Use the existing mute message and return before delivery. |
| Wield broadcast missing | Carry Stick, `wield stick`, observe another player in the room | `send_msg` takes an encoded identifier; callers supplied raw `ploc` indexes. Use `sendloc` in combat/equipment, social, bong, dig, throw, and untie callers. |
| `get ball` interpreted as bulk pickup | Put Ball in the room, `get ball` | Substring search matched `all` inside `ball`. Match the first argument exactly and match `from` as a complete word. |
| Abbreviated bulk drop fails | Carry Ball, `dro all` | The handler searched for the literal string `drop all`. Match the parsed first argument `all`. `dr` resolves to `drink` under the original command table. |
| Zone loader reads before a buffer | Build all zones with AddressSanitizer; a description delimiter begins a line | Guard the preceding-character read at the start of the input, and trim empty output without reading before it. |
| Login ban matcher reads before a buffer | Register with a blank line in the ban patterns | Check the pattern length before reading its last character; allocate copies to fit the input instead of using fixed 100-byte buffers. Existing wildcard rules are preserved. |
| Inventory reads past its last entry | Take Excalibur after the quest, or inspect a Zodiac mobile with a full inventory array under ASan | Fetch each entry only after testing its index against the set size. Apply the same loop repair to cloned-location inbound-exit iteration. |
| Socket lookup indexes a disconnected player slot | Full sanitized room walk; descriptor no longer maps to a player | Reject out-of-range descriptors, negative player slots, and the exclusive maximum before calling `fildes`. |
| Missing/unknown light target silently ignored | `light`, `light nosuchitem`; equivalent extinguish commands | Validate target and use the existing C/iDiRT missing and unavailable target messages before reading object fields. |
| Extinguishing zero-state object requests invalid state | Light and extinguish Sherwood Stick | Clear Lit; only set state 1 when that state exists in the object's zone definition. |

The missing-target cases also prompted a bounds guard in `p_ishere`, so invalid
object indexes cannot read outside the object array.

## Current review limits

The initial scenarios cover generic light/extinguish, opening/closing and
container retrieval, say/sayto, wield/inventory/pickup, argument boundaries,
sitting/standing, social audiences, and registered fallback verbs. Their passing
branches are regression evidence; they do not establish complete review of the
remaining commands, zone specials, combat, magic, administration, or lifecycle.
`handler_branch_report.py` lists compiler branches not yet exercised, and
`coverage_report.py --verbose` lists registered input and handler gaps.

`test_c_bounds.py` compiles the actual prepass and match functions with ASan/UBSan and exercises blank, newline-only, whitespace, comment, multiline delimiter, linked-exit, case-insensitive wildcard, and long-input cases. Generator lifetime allocations are reported separately; the sanitizer build uses `CDIRT_ASAN_OPTIONS=detect_leaks=0` for the generator only.

## Verification run

- 22 structural and direct C regression tests passed.
- All 62 exact verb steps passed in the final ASan/UBSan build, with 50 steps asserting game state.
- Runtime evidence covers 27 of 390 registered input words and 13 primary C handlers. Complete branch review remains pending for every route; the strict coverage gate correctly returns failure.
- The sanitized room walk passed all 2,916 compiled rooms and 6,990 declared exits.
- Excalibur and the other 23 quest trigger routes passed with sanitizer instrumentation. The regular build also passed all 24 quest regressions, the fallback/say/sayto smoke checks, and the 14 light transitions. Quest shortcuts continue to leave natural travel and combat coverage incomplete.
- The regular `cdirt:latest` image was rebuilt after diagnostic testing. The live container on port 6715 was kept running throughout.

Artifacts are in `/tmp/cdirt-verb-audit`: `report.json`, `branches.json`, `zone-walk.json`, quest/smoke logs, and preserved sanitizer reproductions. These temporary artifacts are execution evidence, not committed repository data.
