/* Local test control and observations. Absent from normal game behavior. */
#include "audit.h"
#ifdef CDIRT_AUDIT
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include "kernel.h"
#include "mobile.h"
#include "objsys.h"
#include "rooms.h"
#include "zones.h"
#include "mud.h"
#include "timing.h"
#include "spell.h"
#include "verbs.h"

#define LIMIT 131072
#define TRACE_LIMIT 1024
static int control = -1, depth, serial, overflow, manual_timer;
static char **audit_messages;
static char (*output_names)[MNAME_LEN+1];
static size_t *lengths;
static struct { int actor, verb; char action[MAX_COM_LEN]; } trace[TRACE_LIMIT];
static int traces;

static void quote(FILE *f, const char *s) {
  const unsigned char *p = (const unsigned char *)(s ? s : "");
  fputc('"', f);
  for (; *p; p++) {
    if (*p == '"' || *p == '\\') { fputc('\\', f); fputc(*p, f); }
    else if (*p < 32 || *p >= 127) fprintf(f, "\\u%04x", *p);
    else fputc(*p, f);
  }
  fputc('"', f);
}

static int character(const char *s) {
  int p;
  for (p = 0; p < numchars; p++)
    if (!strcasecmp(s, pname(p))) return p;
  return -1;
}

static int object(char *s) {
  char copy[256], *zone;
  int id = idtxt2int(s, OBJ), i;
  if (id >= 0) return id;
  if (strlen(s) >= sizeof(copy)) return -1;
  strcpy(copy, s); zone = strchr(copy, '@');
  if (!zone) return -1;
  *zone++ = 0;
  for (i = 0; i < numobs; i++)
    if (!strcasecmp(copy, ozname(i)) && ozone(i) >= 0 &&
        !strcasecmp(zone, zname(ozone(i)))) return i;
  return -1;
}
static int room(char *s) {
  char copy[256]; int id;
  if (strlen(s) >= sizeof(copy)) return -1;
  strcpy(copy, s);
  return find_loc_by_name_ex(copy, &id) ? id : -1;
}

static void audit_bits(FILE *f, long *b, int *index, char **names, int group, int count) {
  int i, sep = 0;
  fputc('[', f);
  for (i = 0; i < count; i++) if (test_bit(b, index, i, group)) {
    if (sep++) fputc(',', f);
    quote(f, names ? names[i] : "");
  }
  fputc(']', f);
}

static void ids(FILE *f, int_set *s) {
  int i;
  fputc('[', f);
  for (i = 0; i < set_size(s); i++) {
    if (i) fputc(',', f);
    fprintf(f, "%d", int_number(i, s));
  }
  fputc(']', f);
}

static void snapshot_player(FILE *f, int p) {
  int i, first = 1;
  fprintf(f, "{\"id\":%d,\"name\":", p); quote(f, pname(p));
  fprintf(f, ",\"room\":%d,\"level\":%d,\"strength\":%d,\"score\":%u,"
    "\"class\":%d,\"weapon\":%d,\"fighting\":%d,\"sitting\":%d,"
    "\"visibility\":%d,\"connected\":%s,\"objects\":",
    ploc(p), plev(p), pstr(p), pscore(p), pclass(p), pwpn(p), pfighting(p),
    psitting(p), pvis(p), p < max_players && is_conn(p) ? "true" : "false");
  ids(f, pinv(p));
  { int count = 0; SPELL_DURATION *d;
    if (p < max_players) for (d = players[p].duration; d; d = d->next) count++;
    fprintf(f, ",\"durations\":%d", count);
  }
  fputs(",\"sflags\":", f); audit_bits(f, mbits(p), mindex, Sflags, SFLAGS, SFL_MAX);
  fputs(",\"pflags\":", f); audit_bits(f, mbits(p), mindex, Pflags, PFLAGS, PFL_MAX);
  fputs(",\"mflags\":", f); audit_bits(f, mbits(p), mindex, Mflags, MFLAGS, MFL_MAX);
  fputs(",\"eflags\":", f); audit_bits(f, mbits(p), mindex, Eflags, EFLAGS, EFL_MAX);
  fputs(",\"quests\":[", f);
  for (i = 0; i < Q_MAX; i++) if (test_bit(mbits(p), mindex, i, QFLAGS)) {
    fprintf(f, "%s%d", first ? "" : ",", i); first = 0;
  }
  fputs("]}", f);
}

static void snapshot_object(FILE *f, int o) {
  fprintf(f, "{\"id\":%d,\"name\":", o); quote(f, oname(o));
  fputs(",\"zone_name\":", f); quote(f, ozname(o));
  fprintf(f, ",\"location\":%d,\"carry\":%d,\"state\":%d,\"max_state\":%d,"
    "\"linked\":%ld,\"value\":%d,\"damage\":%d,\"armor\":%d,"
    "\"visibility\":%d,\"objects\":", oloc(o), ocarrf(o), state(o),
    omaxstate(o), (long)olinked(o), obaseval(o), odamage(o), oarmor(o), ovis(o));
  ids(f, oinv(o));
  fputs(",\"oflags\":", f); audit_bits(f, obits(o), oindex, Oflags, OFLAGS, OFL_MAX);
  fputs(",\"aflags\":", f); audit_bits(f, obits(o), oindex, Aflags, AFLAGS, AFL_MAX);
  fputc('}', f);
}

static void snapshot_room(FILE *f, int l) {
  int i;
  fprintf(f, "{\"id\":%d,\"name\":", l); quote(f, lname(l));
  fputs(",\"exits\":[", f);
  for (i = 0; i < NEXITS; i++) fprintf(f, "%s%d", i ? "," : "", lexit(l, i));
  fputs("],\"objects\":", f); ids(f, linv(l));
  fputs(",\"characters\":", f); ids(f, lmobs(l));
  fputs(",\"lflags\":", f); audit_bits(f, lbits(l), lindex, Lflags, LFLAGS, LFL_MAX);
  fputc('}', f);
}

void audit_boot(void) {
  struct sockaddr_un addr;
  const char *path = getenv("CDIRT_AUDIT_SOCKET");
  if (!path || strlen(path) >= sizeof(addr.sun_path)) return;
  memset(&addr, 0, sizeof(addr)); addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, path);
  control = socket(AF_UNIX, SOCK_STREAM, 0);
  if (control < 0 || bind(control, (struct sockaddr *)&addr, sizeof(addr)) < 0 ||
      listen(control, 8) < 0) { perror("audit socket"); exit(1); }
  chmod(path, 0666); /* The host runner encloses the mount in a private directory. */
  if (control >= MAX_FDS) { fputs("audit fd exceeds MAX_FDS\n", stderr); exit(1); }
  if (control >= width) width = control + 1;
  audit_messages = calloc(max_players, sizeof(*audit_messages));
  output_names = calloc(max_players, sizeof(*output_names));
  lengths = calloc(max_players, sizeof(*lengths));
  if (!audit_messages || !lengths || !output_names) exit(1);
}

int audit_descriptor(void) { return control; }
int audit_manual_timer(void) { return control >= 0 && manual_timer; }
void audit_begin(const char *s) { if (control >= 0) depth++; }
void audit_end(void) { if (control >= 0 && depth > 0 && !--depth) serial++; }
void audit_dispatch(int vb) {
  if (control < 0) return;
  if (traces >= TRACE_LIMIT) { overflow = 1; return; }
  trace[traces].actor = mynum; trace[traces].action[0] = 0;
  trace[traces++].verb = vb;
}
void audit_action(const char *name) {
  audit_dispatch(-1);
  if (traces && traces <= TRACE_LIMIT)
    snprintf(trace[traces-1].action, sizeof(trace[traces-1].action), "%s", name);
}
void audit_output(int p, const unsigned char *s, unsigned long n) {
  char *next;
  if (control < 0 || !depth || p < 0 || p >= max_players) return;
  if (n >= LIMIT || lengths[p] >= LIMIT - n) { overflow = 1; return; }
  if (!audit_messages[p]) snprintf(output_names[p], sizeof(*output_names), "%s", pname(p));
  next = realloc(audit_messages[p], lengths[p] + n + 1);
  if (!next) { overflow = 1; return; }
  audit_messages[p] = next; memcpy(next + lengths[p], s, n);
  lengths[p] += n; next[lengths[p]] = 0;
}

static int flag_index(char **names, const char *s, int count) {
  int i;
  for (i = 0; i < count; i++) if (!strcasecmp(names[i], s)) return i;
  return -1;
}

/* A deliberately small private protocol; fixture values never execute verbs. */
static const char *fixture(char **v, int n) {
  int id, val = 0, f, loc, ancestor, hops;
  long number;
  char *end;
  if (n != 5) return "fixture requires kind, identity, field, value";
  if (strcmp(v[3], "room") && strcmp(v[3], "carrier") && strcmp(v[3], "container") && strcmp(v[3], "title")) {
    errno = 0; number = strtol(v[4], &end, 10);
    if (errno || !*v[4] || *end || number < INT_MIN || number > INT_MAX)
      return "invalid numeric fixture value";
    val = (int)number;
  }
  if (!strcmp(v[1], "player")) {
    if ((id = character(v[2])) < 0) return "unknown character";
    if (!strcmp(v[3], "room")) {
      if ((loc = room(v[4])) < 0) return "unknown room";
      setploc(id, loc);
    }
    else if (!strcmp(v[3], "level")) { if (val < 1 || val > LVL_MAX) return "invalid level"; setplev(id, val); }
    else if (!strcmp(v[3], "title")) {
      if (id >= max_players || strlen(v[4]) >= sizeof(players[id].ptitle)) return "invalid title";
      strcpy(players[id].ptitle, v[4]);
    }
    else if (!strcmp(v[3], "duration:lit")) {
      if (id >= max_players || val < 0 || val > 3600) return "invalid duration";
      push_duration(id, VERB_LIT, val, 0);
    }
    else if (!strcmp(v[3], "duration:wipe")) {
      if (id >= max_players) return "invalid player";
      wipe_duration(id);
    }
    else if (!strcmp(v[3], "strength")) setpstr(id, val);
    else if (!strcmp(v[3], "score")) setpscore(id, val);
    else if (!strcmp(v[3], "class")) { if (val < WARRIOR || val > MAGE) return "invalid class"; setpclass(id, val); }
    else if (!strcmp(v[3], "visibility")) setpvis(id, val);
    else if (!strcmp(v[3], "sitting")) psitting(id) = val;
    else if (!strcmp(v[3], "fighting")) { if (val < -1 || val >= numchars) return "invalid target"; pfighting(id) = val; }
    else if (!strncmp(v[3], "sflag:", 6)) {
      if ((f = flag_index(Sflags, v[3]+6, SFL_MAX)) < 0) return "unknown sflag";
      set_bit(mbits(id), mindex, f, val != 0, SFLAGS);
    }
    else if (!strncmp(v[3], "pflag:", 6)) {
      if ((f = flag_index(Pflags, v[3]+6, PFL_MAX)) < 0) return "unknown pflag";
      set_bit(mbits(id), mindex, f, val != 0, PFLAGS);
    }
    else return "unsupported player field";
  }
  else if (!strcmp(v[1], "object")) {
    if ((id = object(v[2])) < 0) return "unknown object";
    if (!strcmp(v[3], "room")) {
      if ((loc = room(v[4])) < 0) return "unknown room";
      setoloc(id, loc, IN_ROOM);
    }
    else if (!strcmp(v[3], "carrier")) {
      if ((loc = character(v[4])) < 0) return "unknown carrier";
      setoloc(id, loc, CARRIED_BY);
    }
    else if (!strcmp(v[3], "container") && strcmp(v[3], "title")) {
      if ((loc = object(v[4])) < 0 || loc == id || !otstbit(loc, OFL_CONTAINER)) return "invalid container";
      ancestor = loc; hops = 0;
      while (ocarrf(ancestor) == IN_CONTAINER) {
        ancestor = oloc(ancestor);
        if (ancestor == id || ancestor < 0 || ancestor >= numobs || ++hops > numobs)
          return "container cycle";
      }
      setoloc(id, loc, IN_CONTAINER);
    }
    else if (!strcmp(v[3], "state")) { if (val < 0 || val > omaxstate(id)) return "invalid state"; setobjstate(id, val); }
    else if (!strcmp(v[3], "visibility")) osetvis(id, val);
    else if (!strncmp(v[3], "oflag:", 6)) {
      if ((f = flag_index(Oflags, v[3]+6, OFL_MAX)) < 0) return "unknown oflag";
      set_bit(obits(id), oindex, f, val != 0, OFLAGS);
    }
    else return "unsupported object field";
  }
  else if (!strcmp(v[1], "room")) {
    if ((id = room(v[2])) < 0) return "unknown room";
    if (!strncmp(v[3], "lflag:", 6)) {
      if ((f = flag_index(Lflags, v[3]+6, LFL_MAX)) < 0) return "unknown lflag";
      set_bit(lbits(id), lindex, f, val != 0, LFLAGS);
    }
    else return "unsupported room field";
  }
  else return "unknown fixture kind";
  return NULL;
}

void audit_request(void) {
  int fd, n = 0, id, i, old = real_mynum;
  FILE *in, *out;
  char request[4096], *v[8], *save, *buf = NULL;
  size_t size = 0;
  const char *err = NULL;
  struct timeval timeout = { 1, 0 };
  if ((fd = accept(control, NULL, NULL)) < 0) return;
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if (!(in = fdopen(fd, "r+"))) { close(fd); return; }
  if (!fgets(request, sizeof(request), in)) { fclose(in); return; }
  request[strcspn(request, "\r\n")] = 0;
  for (v[n] = strtok_r(request, "\t", &save); v[n] && n < 7; v[++n] = strtok_r(NULL, "\t", &save));
  if (!(out = open_memstream(&buf, &size))) { fclose(in); return; }
  setup_globals(-1);
  if (!n) err = "empty request";
  else if (!strcmp(v[0], "ping")) fprintf(out, "{\"audit\":true,\"serial\":%d,\"objects\":%d,\"characters\":%d,\"rooms\":%d}", serial, numobs, numchars, numloc);
  else if (!strcmp(v[0], "mark")) {
    for (i = 0; i < max_players; i++) { free(audit_messages[i]); audit_messages[i] = NULL; lengths[i] = 0; }
    traces = overflow = 0; fprintf(out, "{\"serial\":%d}", serial);
  }
  else if (!strcmp(v[0], "receipt")) {
    fprintf(out, "{\"serial\":%d,\"overflow\":%s,\"trace\":[", serial, overflow ? "true" : "false");
    for (i = 0; i < traces; i++) {
      fprintf(out, "%s{\"actor\":%d,\"verb\":%d,\"action\":", i ? "," : "", trace[i].actor, trace[i].verb);
      quote(out, trace[i].action); fputc('}', out);
    }
    fputs("],\"output\":{", out);
    for (i = 0, id = 0; i < max_players; i++) if (audit_messages[i]) {
      if (id++) fputc(',', out);
      quote(out, output_names[i]); fputc(':', out); quote(out, audit_messages[i]);
    }
    fputs("}}", out);
  }
  else if (!strcmp(v[0], "seed") && n == 2) { srand(strtoul(v[1], NULL, 10)); srand48(strtol(v[1], NULL, 10)); fputs("{}", out); }
  else if (!strcmp(v[0], "timers") && n == 2) {
    if (strcmp(v[1], "manual") && strcmp(v[1], "automatic")) err = "invalid timer mode";
    else { manual_timer = !strcmp(v[1], "manual"); fputs("{}", out); }
  }
  else if (!strcmp(v[0], "tick") && n == 2) {
    id = atoi(v[1]);
    if (id < 0 || id > 3600) err = "invalid tick count";
    else { audit_begin("tick"); for (i = 0; i < id; i++) on_timer(); audit_end(); fputs("{}", out); }
  }
  else if (!strcmp(v[0], "fixture")) { err = fixture(v, n); if (!err) fputs("{}", out); }
  else if (!strcmp(v[0], "player") && n == 2) { if ((id = character(v[1])) < 0) err = "unknown character"; else snapshot_player(out, id); }
  else if (!strcmp(v[0], "object") && n == 2) { if ((id = object(v[1])) < 0) err = "unknown object"; else snapshot_object(out, id); }
  else if (!strcmp(v[0], "room") && n == 2) { if ((id = room(v[1])) < 0) err = "unknown room"; else snapshot_room(out, id); }
  else if (!strcmp(v[0], "coverage")) {
#ifdef CDIRT_COVERAGE
    extern void __gcov_dump(void);
    __gcov_dump(); fputs("{}", out);
#else
    err = "coverage not compiled";
#endif
  }
  else if (!strcmp(v[0], "coverage_reset")) {
#ifdef CDIRT_COVERAGE
    extern void __gcov_reset(void);
    __gcov_reset(); fputs("{}", out);
#else
    err = "coverage not compiled";
#endif
  }
  else err = "unsupported request";
  if (err) { fputs("{\"error\":", out); quote(out, err); fputc('}', out); }
  fclose(out);
  setup_globals(old);
  fprintf(in, "%s\n", buf); fflush(in); free(buf); fclose(in);
}
#endif
