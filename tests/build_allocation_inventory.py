#!/usr/bin/env python3
"""Inventory active allocator sites; classifications document static review scope."""
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
PATTERN = re.compile(r'\b(NEW|COPY|BCOPY|malloc|calloc|realloc|strdup|xmalloc|resize_array|memory_alloc|memory_string|memory_copy|text_take|text_format|text_vformat)\s*\(')
REVIEWS = {
 'src/comm.c': ('Bob input', 'Owned exact question string released after synchronous robot call.'),
 'src/frob.c': ('frob session', 'Owned context and prompt backup; name storage covers mobile names; freed on completion.'),
 'src/wizlist.c': ('wizard list', 'Checked node allocation and bounded parsed names; demoted nodes released.'),
 'cr/fight.c': ('combat output', 'Each substitution and complete message owns exact storage; freed after synchronous output; templates never used as printf format.'),
 'include/utils.h': ('declarations', 'Allocator and resize helper declarations; implementations are reviewed in src/utils.c.'),
 'src/bootstrap.c': ('world/generator', 'Owned world strings and arrays; checked allocation/growth; current ublock pointer rebased.'),
 'src/clone.c': ('world clones', 'Unique names and descriptions owned by clones; exact copies; collection heads independently initialized.'),
 'src/zones.c': ('zones', 'Owned zone name and collections; world-lifetime storage.'),
 'src/utils.c': ('collections', 'Checked growth and integer bounds; absent removal bounded; hash/tree strings owned by their nodes; ban copies checked.'),
 'src/utils/dns-serv.c': ('DNS cache', 'Node/string storage owned by process-lifetime cache; collision chaining repaired.'),
 'src/mud.c': ('connections', 'Input handlers owned by player; travel/prompt/mail/pronoun strings released and cleared on disconnect; socket text released.'),
 'src/main.c': ('socket buffers', 'Per-player output storage; negative write results ignored or close connection; queued byte counts govern progress.'),
 'src/parse.c': ('command history', 'Owned exact copy, previous command released before replacement.'),
 'src/objsys.c': ('object pronouns', 'Owned exact object-name copy; previous it pointer released.'),
 'src/mobile.c': ('player rendering', 'Stored travel strings owned by player; rendered title borrowed scratch; WHO and HTML temporaries freed; prompt storage has derived bound.'),
 'src/commands.c': ('player preferences', 'Owned away/prompt strings; previous value released; raw prompt length unchanged.'),
 'src/wizard.c': ('player home', 'Owned exact home string; per-player temporary record uses existing persistence ownership.'),
 'src/puff.c': ('timer selection', 'Slot-count-sized array; freed after random selection.'),
 'src/spell.c': ('spell durations', 'Owned nodes; head detached before wipe; expiry unlinks; magic scratch borrowed until next call.'),
 'src/sendsys.c': ('message formatting', 'Exact formatted heap strings freed after synchronous delivery.'),
 'src/log.c': ('logging', 'Exact formatted string and non-expanding color-strip copy; both freed after write.'),
 'src/change.c': ('description editor', 'Builder owns pending text; validation/read failure leaves old target intact; success transfers ownership.'),
 'src/audit.c': ('test instrumentation', 'Audit arrays checked on allocation; bounded capture uses subtraction check and temporary realloc; no production path.'),
 'cr/bprintf.c': ('output rendering', 'Exact varargs and file-code results freed; local special arguments freed; output reserves before each write/rebase.'),
 'cr/uaf.c': ('player files/travel', 'Typed fields; fixed strings checked; owned pointer replacement and corrupt-record cleanup; travel result borrowed until next call.'),
 'cr/actions.c': ('social actions', 'Owned loader nodes/strings, successful fgets and bounded tokens; exact expanded strings freed after send.'),
 'cr/edit.c': ('editor', 'Exact owned line text released with each node; owned sendmail command/title freed after popen.'),
 'cr/mail.c': ('mail', 'Owned subjects and nodes; work pointer may borrow list node; distinct cleanup avoids double frees; bounded file reads.'),
 'cr/mudfd.c': ('tracked files', 'Owned source/path/mode strings; full copies; released on close or failed open.'),
 'cr/vote.c': ('vote records', 'Bounded record tokens; extra slot for current voter; success/failure releases allocated nodes.'),
 'cr/client.c': ('AberChat', 'Exact reply-target copies; existing 236792e queue/reconnect fix; disabled in Docker configuration.'),
 'cr/idlookup.c': ('IDENT', 'Temporary identity string freed; bounded stream/field storage; no overlapping formatting.'),
 'cr/noswear.c': ('text filter', 'Owned copy, tokens and output builder freed after synchronous output; original RNG/spacing rules preserved.'),
 'cr/splotch.c': ('Splotch', 'Owned expanded question/response/words; replacements resize; scratch is borrowed; input lookahead and table bounds checked.'),
 'include/memory.h': ('shared primitives', 'Overflow-checked sizes, single evaluation, explicit zero allocation, controlled failure and owned Text capacity.'),
 'include/kernel.h': ('daemon macros', 'Checked helpers replace raw allocation macros.'),
 'include/bootstrap.h': ('generator macros', 'Same checked helpers as daemon.'),
 'src/utils/dns-serv.h': ('DNS macros', 'Same single-evaluation checked string helper.'),
}


def inventory():
    entries=[]
    for directory in ('src','cr','include','cr_inc'):
        for path in sorted((ROOT/directory).rglob('*')):
            if not path.is_file() or path.name.startswith('.') or path.suffix not in ('.c','.h'): continue
            relative=str(path.relative_to(ROOT))
            if path.name in ('MACHINE.H','config.h','objects.h','mobiles.h','locations.h','verbs.h'): continue
            for lineno,line in enumerate(path.read_text(errors='replace').splitlines(),1):
                if line.lstrip().startswith(('/*','*','//')): continue
                for match in PATTERN.finditer(line):
                    category,review=REVIEWS.get(relative,('UNREVIEWED','Review required'))
                    entries.append({'file':relative,'line':lineno,'allocator':match[1],
                                    'source':line.strip(),'category':category,'review':review})
    return {'method':'Static site inventory and ownership/capacity review; not exhaustive runtime proof.',
            'entries':entries,'unreviewed':sum(e['category']=='UNREVIEWED' for e in entries)}


if __name__=='__main__':
    data=inventory()
    (ROOT/'tests/allocation_inventory.json').write_text(json.dumps(data,indent=2)+'\n')
    print(f"Allocation inventory: {len(data['entries'])} sites, {data['unreviewed']} unreviewed")
    raise SystemExit(bool(data['unreviewed']))
