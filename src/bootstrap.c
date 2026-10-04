/****************************************************************************
 * The purpose of this file is to boot the mud, or generate the headers for *
 * objects or mobiles in the game.  The first stage is the prepass step.    *
 * That eliminates the need for using cpp (which slows things down).  It    *
 * builds hash tables, z_* for zone names.  n_* is for in-game Pnames.      *
 * Then it links all the locations (by freeing the text descriptions, and   *
 * casting an int to the pointers), and performs all the necessary          *
 * assignments to get the game up and running.                              *
 ****************************************************************************/

#include <stdio.h>
#include <sys/file.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>
#include <stdlib.h>
#include "bootstrap.h"
#include "global.h"
#include "wizlist.h"
#include "actions.h"
#include "zones.h"
#include "bprintf.h"
#include "mobile.h"
#include "fight.h"

#undef DEBUG

void add_tree(T_elemptr *, char *, int);

#ifdef GEN

#include "flagindex.h"

int make_verbs(void) {  
  FILE *In, *Out, *H;
  char *p, *q;
  int vcode, num_v;
  XTABLE *first, *last, *v, *w;
  char buff[128], verb[30];

  if (!(In = fopen(DATA_DIR "/verbs.src", "r"))) {
    printf("Unable to open %s for input\n", DATA_DIR "/verbs.src");
    exit(1);
  }
  if (!(Out = fopen(DATA_DIR "/verbs", "w"))) {
    printf("Unable to open %s for output.\n", DATA_DIR "/verbs");
    exit(1);
  }
  if (!(H = fopen(H_DIR "/verbs.h", "w"))) {
    printf("Unable to open %s for output\n", DATA_DIR "/verbs.h");
    exit(1);
  }

  first = last = NULL;
  vcode = num_v = 0;
  while (fgets (buff, sizeof buff, In)) {
    for (p = buff, q = verb; isalpha (*p);)
      *q++ = *p++;
    if (q == verb)
      continue;
    *q = 0;
    lowercase (verb);

    v = (XTABLE *) NEW(XTABLE, 1);
    strcpy (v->name, verb);
    ++num_v;

    if (*p != '=')
      v->code = ++vcode;
    else {
      for (q = verb, ++p; isalpha (*p);)
        *q++ = *p++;
      *q = 0;
      lowercase (verb);

      for (w = first; w != NULL && strcmp (w->name, verb) != 0; w = w->next);
      if (w == NULL)
        v->code = ++vcode;
      else
        v->code = w->code;
    }

    v->next = NULL;
    if (first == NULL)
      first = last = v;
    else
      last = (last->next = v);
  }
  fclose (In);

  fprintf (H, "\
/*\n\
**\tVerb file header generated from verbs.src\n\
**\tDON'T MAKE CHANGES HERE -- THEY WILL GO AWAY!\n\
*/\n\n\
#ifndef _VERBS_H\n\
#define _VERBS_H\n\n");
  fprintf (Out, "%d\n", num_v);

  for (w = first; w != NULL; w = w->next) {
    strcpy (verb, w->name);
    (void) fprintf (Out, "%s %d\n", verb, w->code);

    uppercase (verb);
    (void) fprintf (H, "#define\tVERB_%s\t%d\n", verb, w->code);
  }
  fclose (Out);
  fprintf (H, "\n#endif\n");
  fclose (H);
  return(num_v);
}

int main(void) {
  int verbs;

  bootstrap();
  verbs = make_verbs();
  fprintf(stderr, "\nThe world: [%d Objects | %d Mobiles | %d Locations "
         "| %d Verbs]\n", numobs, numchars, numloc, verbs);
  return(0);
}
#endif

void set_flagpoints(void) {
  int i, a, b, sum;

  b = oindex[0];
  oindex[0] = 0;
  for (i = 1, sum = 1 ; i < MAX_OBJ_FLAGS + 1 ; i++) {
    a = oindex[i];
    oindex[i] = (b / 32) + sum;
    b = a;
    sum = oindex[i] + 1;
  }

  b = mindex[0];
  mindex[0] = 0;
  for (i = 1, sum = 1 ; i < MAX_MOB_FLAGS + 1 ; i++) {
    a = mindex[i];
    mindex[i] = (b / 32) + sum;
    b = a;
    sum = mindex[i] + 1;
  }

  b = lindex[0];
  lindex[0] = 0;
  for (i = 1, sum = 1 ; i < MAX_LOC_FLAGS + 1 ; i++) {
    a = lindex[i];
    lindex[i] = (b / 32) + sum;
    b = a;
    sum = lindex[i] + 1;
  }

  b = windex[0];
  windex[0] = 0;
  for (i = 1, sum = 1 ; i < MAX_WORLD_FLAGS + 1 ; i++) {
    a = windex[i];
    windex[i] = (b / 32) + sum;
    b = a;
    sum = windex[i] + 1;
  }

  b = pflindex[0];
  pflindex[0] = 0;
  for (i = 1, sum = 1 ; i < MAX_PFL_FLAGS + 1 ; i++) {
    a = pflindex[i];
    pflindex[i] = (b / 32) + sum;
    b = a;
    sum = pflindex[i] + 1;
  }
}

#ifndef GEN
static int load_level_scores(void) {
  FILE *fp;
  int count, i;
  int score[LVL_WIZARD + 1];

  if (!(fp = fopen("levels", "r"))) {
    perror("Unable to open levels");
    return -1;
  }

  if (fscanf(fp, "%d", &count) != 1 || count != LVL_WIZARD) {
    fprintf(stderr, "Invalid levels file: expected %d level thresholds.\n",
            LVL_WIZARD);
    fclose(fp);
    return -1;
  }

  for (i = 1; i <= count; i++) {
    if (fscanf(fp, "%d", &score[i]) != 1 || score[i] < 0 ||
        (i > 1 && score[i] < score[i - 1])) {
      fprintf(stderr, "Invalid score threshold for level %d in levels file.\n",
              i);
      fclose(fp);
      return -1;
    }
  }

  fclose(fp);
  for (i = 1; i <= count; i++)
    levels[i] = score[i];

  return 0;
}
#endif

int bootstrap(void) {
  char buff[256];
  struct dirent **namelist;
  int n, x;

#ifndef GEN
  if (load_level_scores() == -1)
    return -1;
#endif

  xzon = 0;

  x = scandir(ZONE_DIR, &namelist, 0, NULL);
  if (x < 0) {
    perror("scandir");
    return(-1);
  }

  set_flagpoints();

#ifndef GEN
  numchars = max_players;

  if (boot_verbs() == -1) {
    fprintf(stderr, "\nVerb boot failed.\n");
    return(1);
  }
  else if (boot_pflags() == -1) {
    fprintf(stderr, "\nPflags boot failed.\n");
    return(1);
  }
  else if (boot_wizlist() == -1) {
    fprintf(stderr, "\nBoot wizlist failed.\n");
    return(1);
  }
  else if (boot_actions() == -1) {
    fprintf(stderr, "\nBoot actions failed.\n");
    return(1);
  }
#endif
  boot_arrays(x);

  for (n = x - 1 ; n > 0 ; n--) {
    if (namelist[n]->d_name[0] == '.' || !valid_zone(namelist[n]->d_name))
      continue; 
    else {
      sprintf(buff, "%s/%s", ZONE_DIR, namelist[n]->d_name);
      if (!old_proc_num && !(n % (x > 9 ? (x/10) : 1)))
        fprintf(stderr, ".");
      if (process_zone(buff, False, False) == -1) {
        fprintf(stderr, "\nFailure at zone: %s\n", namelist[n]->d_name);
        return(-1);
      }
      numzon = ++xzon;

      resize_zones();
    }
  }
  
#ifdef GEN
  if (link_zones() == -1) {
    fprintf(stderr, "\nFailure linking zones.\n");
    return(-2);
  }
  else if (store_headers() == -1) {
    fprintf(stderr, "\nFailure storing headers.\n");
    return(-3);
  }
#else
  if (link_zones() == -1) {
    fprintf(stderr, "\nFailure linking zones.\n");
    return(-2);
  }
  num_const_locs = numloc;
  num_const_chars = numchars;
  num_const_obs = numobs;
  num_const_zon = numzon;
  if (!old_proc_num)
    fprintf(stderr, "Linked.\n");
#endif
  return(0);
}

int boot_arrays(int bzones) {

#ifndef GEN
  players = NEW (PLAYER_REC, max_players);
  rplrs = NEW (RPLR_REC, max_players);
  iplrs = NEW (IO_REC, max_players);
#endif

  if (bzones < 0 || bzones > INT_MAX / BASEMULT || bzones > INT_MAX / 2) memory_failure();
  zones = NEW (ZONE, (size_t)bzones * 2);
  ublock = NEW (UBLOCK_REC, bzones * BASEMULT);
  objs = NEW (OBJECT, bzones * BASEMULT);
  room_data = NEW (Location, bzones * BASEMULT);
  ublock_len = bzones * BASEMULT;
  objects_len = bzones * BASEMULT;
  locations_len = bzones * BASEMULT;
  zones_len = bzones * 2;

  return(0);
}

#ifndef GEN

int boot_verbs (void) {
  int numverbs, i, verbnum;
  FILE *fpr;
  char verbstr[64];

  if (!(fpr = fopen(DATA_DIR "/verbs", "r"))) {
    fprintf(stderr, "\nUnable to load verbs file.\n");
    return(-1);
  }

  verb_t = NULL;
  fscanf (fpr, "%d", &numverbs);

  for (i = 0; i < numverbs; i++) {
    if (fscanf(fpr, "%63s %d", verbstr, &verbnum) != 2) { fclose(fpr); return -1; }
    add_tree(&verb_t, verbstr, verbnum);
  }
  return(0);
}
#endif

Boolean valid_zone(char *file) {
  char *p;

  if ((p = strstr(file, ".zone"))) {
    if (strlen(p) == strlen(".zone"))
      return(True);
  }
  return(False);
}

#ifndef GEN

void load_zone(char *player, Boolean silent) {
  int numloc_p = numloc;
  int numobs_p = numobs;
  int numchars_p = numchars;
  Boolean empty;
  char buff[256];

  sprintf(buff, "%s/%s", WIZ_ZONE_DIR, player);

  xzon = get_wizzone_by_name(player);
  empty = (!znumloc(xzon) && !znumchars(xzon) && !znumobs(xzon));
  process_zone(buff, empty, silent);
  link_zone(xzon);
  reset_zone(xzon);

  if (!(numloc - numloc_p) && !(numobs - numobs_p) && !(numchars-numchars_p)) {
   if (!silent) 
     bprintf("Zone %s: No new objects, mobiles, or locations loaded.\n", 
        pname(mynum));
   return;
  }
  else {
    if (!silent)
      bprintf("Zone %s loaded: [%d object%s | %d mobile%s, %d location%s]\n",
        pname(mynum), zmaxobjs(xzon), zmaxobjs(xzon) != 1 ? "s" : "",
           zmaxmobs(xzon), zmaxmobs(xzon) != 1 ? "s" : "", 
           zmaxlocs(xzon), zmaxlocs(xzon) != 1 ? "s" : "");
    return;
  }
}
#endif

int process_zone(char *file, Boolean empty, Boolean silent) {
  FILE *fptr;
  char *line;
  char buff[BUFFLEN];
  Boolean in_comment = False;
  Boolean in_quote = False;
  Boolean in_hat = False;

  if (!(fptr = fopen(file, "r"))) {
#ifdef GEN
    if (!silent)
      fprintf(stderr, "\nError in fopen(): unable to open zone file.\n");
#else
    if (!silent)
      bprintf("Unable to load your wiz-zone, sorry.\n");
#endif
    return(-1);
  }
  else {
    setup_zone(empty, strrchr(file, '/') + 1);

    while (!feof(fptr)) {
      fgets(buff, BUFFLEN - 1, fptr);

      if (!(line = prepass(&in_comment, &in_quote, &in_hat, buff)))
	return(-1);
      else if (in_quote || in_hat)
	continue;

      if (!EMPTY(line) && *line != '#' && *line != '\n') {
	if (*line == '%') {
	  if (set_zone_global(line+1) == -1)
	    return(-1);
	}
	else {
	  switch (loading) {
	  case PROC_MOBILES:
	    if (proc_mobile(line) == -1)
	      return(-1);
	    break;
	  case PROC_LOCATIONS:
	    if (proc_location(line) == -1)
	      return(-1);
	    break;
	  case PROC_OBJECTS:
	    if (proc_object(line) == -1)
	      return(-1);
	    break;
	  default:
	    fprintf(stderr, "\nAll mobiles must be defined after a %%mobiles, etc."
                   "Line failed: %s\n", line);
	    return(-1);
	    break;
	  }
	}
      }
    }
    fclose(fptr);
  }
  return(0);
}

int get_key_value(char *buff, char **key, char **value) {
  char *p;

  for (p = buff ; *p ; p++)    /* skip over leading whitespaces */
    if (!isspace(*p) && *p != '\t')
      break;

  *key = p;

  while (*p++)                /* skip over keyword or whatever */
    if (isspace(*p) || *p == '\t')
      break;

  *p = 0;

  for (++p ; *p ; p++)        /* skip over whitespace after keyword */
    if (!isspace(*p) && *p != '=' && *p != '\t')
      break;

  *value = p;
  
  if (!**key)
    return(-1);
  return(0);
}

int string2num(int *num, char *str) {
  char *p;

  for (p = str ; *p ; p++)
    if (!isdigit(*p) && *p != '-')
      break;

  if (*p != 0)
    return(-1);

  *num = atoi(str);
  return(0);
}

void resize_zones(void) {
  if (zones_len > INT_MAX - BASE_INCR) memory_failure();
  if (numzon < zones_len)
    return;
  
   zones = (ZONE *) resize_array(zones, sizeof(ZONE), zones_len,
           zones_len + BASE_INCR/10);
   zones_len += BASE_INCR/10;
}

void resize_ublock(void) {
  if (ublock_len > INT_MAX - BASE_INCR) memory_failure();
  if (numchars < ublock_len)
    return;

#ifndef GEN
  ptrdiff_t current = cur_ublock ? cur_ublock - ublock : -1;
#endif
  ublock = (UBLOCK_REC *) resize_array(ublock, sizeof(UBLOCK_REC), 
           ublock_len, ublock_len + BASE_INCR);
  ublock_len += BASE_INCR;
#ifndef GEN
  if (current >= 0) cur_ublock = ublock + current;
#endif
}

void resize_objects(void) {
  if (objects_len > INT_MAX - BASE_INCR) memory_failure();
  if (numobs < objects_len)
    return;

  objs = (OBJECT *) resize_array(objs, sizeof(OBJECT),
            objects_len, objects_len + BASE_INCR);
  objects_len += BASE_INCR;
}

void resize_locs(void) {
  if (locations_len > INT_MAX - BASE_INCR) memory_failure();
  if (numloc < locations_len)
    return;
  room_data = (Location *) resize_array(room_data, sizeof(Location),
              locations_len, locations_len + BASE_INCR);
  locations_len += BASE_INCR;
}

/* resets all bits to zero, then loads alphanumeric flags */

int proc_flags(int flagset, long int *bits, int index[],
               char *flsetname, char *flagnames[], char *buff) {
  char *p, *q;
  int rslt, i;

  for (p = buff ; *p ; p++)
    if (*p == '{') {
      p++;
      break;
    }

  if (!*p) {
    fprintf(stderr, "\n%s: missing {\n", flsetname);
    return(-1);
  }
  else {
    for (q = p ; *q ; q++)
      if (*q == '}')
        break;
    if (!*q) {
      fprintf(stderr, "\n%s: missing }\n", flsetname);
      return(-1);
    }
    else {
      for (i = index[flagset] ; i < index[flagset+1] ; i++)
        bits[i] = 0;

      *q = 0;
      while (*p) {
        while(isspace(*p))
          p++;

        if ((q = strchr(p, ' ')))
          *q++ = 0;
        else
          q = p + strlen(p);

        if ((rslt = tlookup(p, flagnames)) != -1) {
#ifndef GEN
          if (*p) {
            if (index == lindex)
              set_bit(bits, lindex, rslt, True, flagset);
            else if (index == mindex)
              set_bit(bits, mindex, rslt, True, flagset);
            else if (index == oindex)
              set_bit(bits, oindex, rslt, True, flagset);
          }
#endif
        }
        else {
          fprintf(stderr, "\nUnknown %s: %s\n", flsetname, p);
          return(-1);
        }
        p = q;
      }
      return(0);
    }
  }
}

int proc_mobile(char *buff) {
  int rslt, i;
  static char name[MNAME_LEN] = "";
  char *key, *value, flagsbuff[32];

  if (get_key_value(buff, &key, &value) == -1) {
    fprintf(stderr, "\nError: KEY = VALUE expected.  Got %s\n", buff);
    return(-1);
  }    
  else {
    i = tlookup(key, mobtab);
    switch(i) {
    case 0:                                     /* Name */
      if (strlen(value) > MNAME_LEN) {
	fprintf(stderr, "\nMobile %s length exceeds %d.\n", value, MNAME_LEN);
	return(-1);
      }
      else if (*name) {
	fprintf(stderr, "\nMobile %s has no End statement.\n", name);
	return(-1);
      }
      else {
	strcpy(name, value);
        if ((rslt = ht_lookup(ublock_z, name, 0, xzon, mob_search)) != -1) {
	  ht_remove(ublock_z, name, rslt);
          ht_remove(ublock_n, pname(rslt), rslt);
	  xchar = rslt;
        }
	else {                    /* Mobile doesn't already exist */
          zmaxmobs(xzon)++;
	  xchar = numchars++;
          zadd_mob(xchar, xzon);
          resize_ublock();
        }
#ifndef GEN
        memset(&ublock[xchar], 0, sizeof(UBLOCK_REC));  /* for clone */
#endif
	setpname(xchar, value);
        pname_reset(xchar) = value;
      }
      break;
    case 1:                                     /* Pname */
      if (strlen(value) > MNAME_LEN) {
	fprintf(stderr, "\n%s exceeds %d chars.\n", value, MNAME_LEN);
	return(-1);
      }
      else {
	setpname(xchar, value);
        pname_reset(xchar) = value;
      }
      break;
    case 2:                                     /* Location */
      ublock[xchar].ploc_reset = (intptr_t) COPY(value);
      break;
    case 3:                                     /* Strength */
      if (string2num(&rslt, value) == -1) {
	fprintf(stderr, "\nError in mob %s: Strength expects num.\n", name);
	return(-1);
      }
      else
        ublock[xchar].pstr_reset = rslt;
      break;
    case 4:                                     /* Damage */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in mob %s: Damage expects num.\n", name);
	return(-1);
      }
      else
        ublock[xchar].pdam_reset = rslt;
      break;
    case 5:                                     /* Aggression */
      if (string2num(&rslt, value) == -1) {
	fprintf(stderr, "\nError in mob %s: Aggression expects num.\n", name);
	return(-1);
      }
      else
        ublock[xchar].pagg_reset = rslt;
      break;
    case 6:                                     /* Armor */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in mob %s: Armor expects num.\n", name);
	return(-1);
      }
      else
        parmor_reset(xchar) = rslt;
      break;
    case 7:                                     /* Speed */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in mob %s: Speed expects num.\n", name);
	return(-1);
      }
      else
        pspeed_reset(xchar) = rslt;
      break;
    case 8:                                     /* Description */
      pftxt(xchar) = COPY(value);
      break;
    case 9:                                     /* End */
      if (!*name) {
	fprintf(stderr, "\nMobile %s has no Begin statement.\n", value);
	return(-1);
      }
      else if (!EQ(value, name)) {
	fprintf(stderr, "\nMobile %s has no End statement.\n", name);
	return(-1);
      }
      else if (!ublock[xchar].ploc_reset) {
	fprintf(stderr, "\nMobile %s has no Location.\n", name);
	return(-1);
      }
      else if (!pftxt(xchar)) {
	fprintf(stderr, "\nMobile %s has no Description.\n", name);
	return(-1);
      }
      else { 
  	ht_add(ublock_z, name, xchar);
        pname(xchar)[0] = toupper(pname(xchar)[0]);
	pzname(xchar) = COPY(name);
	pname_reset(xchar) = COPY(pname(xchar));
	plev_reset(xchar) = -1;
        ublock[xchar].unlinked = True;
#ifndef GEN
	ht_add(ublock_n, xname(pname(xchar)), xchar);
	set_vital(xchar);
	set_player_parts(xchar);
	init_intset (pinv (xchar), 4);
#endif
	pzone(xchar) = xzon;
	*name = 0;
      }
      break;
    case 10:
      pexam(xchar) = COPY(value);
      break;
    default:
      sprintf(flagsbuff, "%sReset", key);
      if ((rslt = tlookup(flagsbuff, Mobflags)) == -1) {
        if (*name)
          fprintf(stderr, "\nUnknown key in mobile %s: ", name);
        else
          fprintf(stderr, "\nUnknown key: ");
        fprintf(stderr, "\n%s\n", key);
        return(-1);
      }
      else
	return(proc_flags(rslt, ublock[xchar].bits, mindex, flagsbuff,
               mobflagsindex[rslt], value));
      break;
    }
  }
  return(0);
}

int proc_object(char *buff) {
  int rslt, i;
  static char name[ONAME_LEN] = "";
  char *key, *value, flagsbuff[32];

  if (get_key_value(buff, &key, &value) == -1) {
    fprintf(stderr, "\nError: KEY = VALUE expected.  Got %s\n", buff);
    return(-1);
  }
  else {
    i = tlookup(key, objtab);
    switch(i) {
    case 0:                                     /* Name */
      if (strlen(value) > ONAME_LEN) {
        fprintf(stderr, "\nObjects %s length exceeds %d.\n", value, ONAME_LEN);
        return(-1);
      }
      else if (*name) {
        fprintf(stderr, "\nObject %s missing End statement.\n", name);
        return(-1);
      }
      else {
	strcpy(name, value);
	if ((rslt = ht_lookup(objects_z, name, 0, xzon, obj_search)) != -1) {
	  ht_remove(objects_z, name, rslt);
          ht_remove(objects_n, oname(rslt), rslt);
          if (oaltname(rslt))
            ht_remove(oalts_n, oaltname(rslt), rslt);
	  xobj = rslt;
	}
	else {                 /* object doesn't already exist */
          resize_objects();
	  xobj = numobs++;
          zadd_obj(xobj, xzon);
   	  zmaxobjs(xzon)++;
        }
#ifndef GEN
        memset(&objs[xobj], 0, sizeof(OBJECT));
#endif
        oname(xobj) = COPY(value);
      }
      break;
    case 1:                                     /* Pname */
      if (strlen(value) > ONAME_LEN) {
        fprintf(stderr, "\n%s exceeds %d chars.\n", name, ONAME_LEN);
        return(-1);
      }
      else
	oname(xobj) = COPY(value);
      break;
    case 2:                                     /* AltName */
      if (strlen(value) > ONAME_LEN) {
        fprintf(stderr, "\n%s exceeds %d chars.\n", name, ONAME_LEN);
        return(-1);
      }
      else {
        oaltname(xobj) = COPY(value);
        ht_add(oalts_n, value, xobj);
      }
      break;
    case 3:                                     /* Location */
      objs[xobj].oloc_reset = (intptr_t) COPY(value);
      break;
    case 4:                                     /* Linked */
      objs[xobj].linked = (intptr_t) COPY(value);
      break;
    case 5:                                     /* State */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in obj %s: State expects num.\n", name);
	return(-1);
      }
      else
        state_reset(xobj) = rslt;
      break;
    case 6:                                     /* MaxState */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in obj %s: MaxState expects num.\n", name);
	return(-1);
      }
      else
        omaxstate(xobj) = rslt;
      break;
    case 7:                                     /* Desc[0] */
      olongt(xobj, 0) = COPY(value);
      break;
    case 8:                                     /* Desc[1] */
      olongt(xobj, 1) = COPY(value);
      break;
    case 9:                                     /* Desc[2] */
      olongt(xobj, 2) = COPY(value);
      break;
    case 10:                                    /* Desc[3] */
      olongt(xobj, 3) = COPY(value);
      break;
    case 11:                                    /* Armor */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in obj %s: Armor expects num.\n", name);
	return(-1);
      }
      else
        oarmor_reset(xobj) = rslt;
      break;
    case 12:                                    /* BValue */
      if (string2num(&rslt, value) == -1) {
	fprintf(stderr, "\nError in obj %s: BValue expects num.\n", name);
	return(-1);
      }
      else
        ovalue_reset(xobj) =  rslt;
      break;
    case 13:                                    /* Size */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in obj %s: Size expects num.\n", name);
	return(-1);
      }
      else
        osize_reset(xobj) = rslt;
      break;
    case 14:                                    /* Weight */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in obj %s: Weight expects num.\n", name);
	return(-1);
      }
      else
        oweight_reset(xobj) = rslt;
      break;
    case 15:                                    /* Examine */
      objs[xobj].oexam_text = COPY(value);
      break;
    case 16:                                    /* Damage */
      if (string2num(&rslt, value) == -1) {
        fprintf(stderr, "\nError in obj %s: Damage expects num.\n", name);
	return(-1);
      }
      else
        odamage_reset(xobj) = rslt;
      break;
    case 17:                                    /* End */
      if (!*name) {
        fprintf(stderr, "\nObject %s has no Begin statement.\n", value);
        return(-1);
      }
      if (!EQ(value, name)) {
        fprintf(stderr, "\nObject %s has no End statement.\n", name);
        return(-1);
      }
      else {                                       /* finish up */
        objs[xobj].unlinked = True;
	oname(xobj)[0] = toupper(oname(xobj)[0]);
	ht_add(objects_z, name, xobj);
	ht_add(objects_n, oname(xobj), xobj);
	ozname(xobj) = COPY(name);
#ifndef GEN
        if (otstbit(xobj, OFL_CONTAINER))
          init_intset(oinv(xobj), 5);
#endif
	ozone(xobj) = xzon;
	*name = 0;
      }
      break;
    default:
      sprintf(flagsbuff, "%sReset", key);
      if ((rslt = tlookup(flagsbuff, Objflags)) == -1) {
        if (*name)
          fprintf(stderr, "\nUnknown key in object %s: ", name);
        else
          fprintf(stderr, "\nUnknown key: ");
        fprintf(stderr, "\n%s\n", key);
        return(-1);
      }
      else
	return(proc_flags(rslt, objs[xobj].bits, oindex, flagsbuff,
               oflagsindex[rslt], value));
      break;
    }
  }
  return(0);
}

char *posp(char *p) {
  char *q;

  for (q = p ; *q ; q++) {
    if (*q == ';') {
      *q = 0;
      return(q);
    }
    else if (isspace(*q) && !isspace(*(q+1))) {
      *q = 0;
      return(q+1);
    }
  }
  return(q);
}

int proc_location(char *buff) {
  static int i = 0;
  char *key, *value;
  int rslt;
  char *p, *q;

  if (!strncasecmp(buff, "Altitude", strlen("Altitude"))) {
    if (get_key_value(buff, &key, &value) == -1) {
      fprintf(stderr, "\nError: KEY = VALUE expected.  Got %s\n", buff);
      return(-1);
    }
    else if (string2num(&rslt, value) == -1) {
      fprintf(stderr, "\nAltitude expects a number.\n");
      return(-1);
    }
    else
       room_data[xloc].r_altitude_reset = rslt;

    return(0);
  }

  if (i == 0) {                          /* first line in location */
    i++;

    for (p = buff ; *p ; p++) {
      if (*p == ';') {
	*p = 0;
	if ((rslt = ht_lookup(locations_z, buff, 0, xzon, room_search)) != -1){
	  xloc = rslt;
	  ht_remove(locations_z, buff, rslt); 
        }
        else {                        /* loc doesn't already exist */
          xloc = numloc++;
          zadd_loc (xloc, xzon);
          zmaxlocs(xzon)++;
        }

        resize_locs();

#ifndef GEN
        memset(&room_data[xloc], 0, sizeof(Location));
#endif
	room_data[xloc].r_name = COPY(buff);
	ht_add(locations_z, buff, xloc);
	return(0);
      }
      else if (isspace(*p)) {
	*p = 0;
	if ((rslt = ht_lookup(locations_z, buff, 0, xzon, room_search)) != -1){
	  xloc = rslt;
	  ht_remove(locations_z, buff, rslt);
        }
        else {
          xloc = numloc++;
          zadd_loc (xloc, xzon);
          zmaxlocs(xzon)++;
        }
        resize_locs();

        room_data[xloc].r_name = COPY(buff);
        ht_add(locations_z, buff, xloc);
        break;	
      } 
    }

    for (++p ; *p ; p++)
      if (!isspace(*p))
	break;
    
    while (*p) {
      if (*p == ';')
	break;
      else if (*(p+1) != ':' && *(p+2)) {
	fprintf(stderr, "\nWas expecting <dir>:<room> Got: %s\n", p);
	return(-1);
      }
      else if ((rslt = llookup(p, Exits)) == -1) {
	fprintf(stderr, "\nUnknown exit in loc: %s\n", p);
	return(-1);
      }
      else {
	q = posp(p);
	room_data[xloc].r_exit_reset[rslt] = (intptr_t) COPY(p+2);
	if ((p = strchr((char *) room_data[xloc].r_exit_reset[rslt], ' ')))
	  *p = 0;
	p = q;
      }
    }
  }
  else if (i == 1) {                           /* lflags line */
    i++;
    return(proc_flags(LFLAGS_RESET, room_data[xloc].bits, lindex,
        "LflagsReset", Lflags, buff));
  }
  else {                                       /* description */ 
    i = 0;
    
    for (p = buff ; *p ; p++) {
      if (*p == '\n')
	break;
    }

    *p++ = 0;                                  /* finish up */
    room_data[xloc].r_short = COPY(buff);
    room_data[xloc].r_long = COPY(p);
    room_data[xloc].zone = xzon;
    room_data[xloc].unlinked = True;

    init_intset(&room_data[xloc].objects, 5);
    init_intset(&room_data[xloc].mobiles, 3);
    init_intset(&room_data[xloc].exits_to_me, 3);
  }

  return(0);
}

void setup_zone(Boolean new, char *str) {
  char *p;

  loading = -1;

  if ((p = strstr(str, ".zone")))   /* setup_zone called with filename */
    *p = 0;

#ifdef DEBUG
  fprintf(stderr, "\nProcessing zone %d:%s\n", xzon, str);
#endif

#ifndef GEN
  if (new) {
    init_intset (zlocs (xzon), 15);
    init_intset (zmobs (xzon), 5);
    init_intset (zobjs (xzon), 10);
  }
#endif
  zones[xzon].z_name = COPY(str);
}

int set_zone_global(char *str) {
  char *p;
  
  if (EQ(str, "objects"))
    loading = PROC_OBJECTS;
  else if (EQ(str, "locations"))
    loading = PROC_LOCATIONS;
  else if (EQ(str, "mobiles"))
    loading = PROC_MOBILES;
  else if (!strncasecmp(str, "rainfall", strlen("rainfall"))) {
    if (!(p = strchr(str, ':'))) {
      fprintf(stderr, "\nInvalid usage of %%rainfall.  Must use "
                      "%%rainfall:<integer>\n");
      return(-1);
    }    
    else {
      for (++p ; *p && isspace(*p) ; p++);
      zones[xzon].rainfall = atoi(p);
    }
  }
  else if (!strncasecmp(str, "latitude", strlen("latitude"))) {
    if (!(p = strchr(str, ':'))) {
      fprintf(stderr, "\nInvalid usage of %%latitude.  Must use "
               "%%latitutde:<integer>\n");
      return(-1);
    }
    else {
      for (++p ; *p && isspace(*p) ; p++);
      zones[xzon].latitude = atoi(p);
    }
  }
  else if (!strncasecmp(str, "zone", strlen("zone"))) {
    if (!(p = strchr(str, ':'))) {
      fprintf(stderr, "\nInvalid usage of %%zone.  Must use "
        "%%zone:<zonename>\n");
      return(-1);
    }
    else {
      for (++p ; *p && isspace(*p) ; p++);
      if (!EQ(zones[xzon].z_name, p)) {      /* new zone, not same as fname */
        numzon = ++xzon;

        resize_zones();
        setup_zone(True, p);
      }
    }
  }
  else {
    fprintf(stderr, "\nUnknown %% sequence: %s\n", str);
    return(-1);
  }
  return(0);
}

char *prepass(Boolean *in_comment, Boolean *in_quote, 
	      Boolean *in_hat, char *inp) {
  static char newbuff[PAGELEN];
  char *p, *q;

  if (*in_quote || *in_hat) {
    strcat(newbuff, "\n");
    if (*inp == '\n')
      return(newbuff);
    q = newbuff + strlen(newbuff);
    if (q - newbuff > PAGELEN) {
      fprintf(stderr, "\nQuote page length exceeded in zone %s", zname(xzon));
      exit(0);
    }
  }
  else {
    q = newbuff;
    *newbuff = 0;
  }

  for (p = inp ; *p ; p++) {
    switch (*p) {
    case '/':
      if (*(p+1) == '*') {
       	if (*in_comment) {
	  fprintf(stderr, "\nComment (/*) not allowed within comment (/*).\n");
	  return(NULL);
	  }
	*in_comment = True;
	p += 2;
      }
      break;
    case '*':
      if (*(p+1) == '/') {
	if (!*in_comment) {
	 fprintf(stderr, "\nEnd comment (*/) must follow comment (/*).\n");
	 return(NULL);
	} 
	*in_comment = False;
	p += 2;
      }
      else
	*p = '*';
      break;
    case '"':
      if (!*in_hat)
	*in_quote = !*in_quote;
      else
	*q++ = *p;
      break;
    case '^':
      if (p == inp || *(p-1) != ':')
	*in_hat = !*in_hat;
      else
	*q++ = '^';
      break;
    default:
      if (!*in_comment)
	*q++ = *p;
      break;
    }
  }
  *q = 0;

  if (q > newbuff && *(q - 1) == '\n')                     /* remove newline */
    *(--q) = 0;

  if (!*in_quote)
    while (q > newbuff && isspace((unsigned char)q[-1]))
      *--q = 0;

  return(newbuff);
}

void ucase(char *dest, char *src) {
  char *p, *q;

  if ((p = strstr(src, "The ")))
    p = src + 4;
  else
    p = src;

  for (q = dest ; *p && !isspace(*p) ; p++, q++)
    *q = toupper(*p);
  
  *q = 0;
}

int link_zones(void) {
  int i, rslt;

  rslt = -1;

  for (i = 0 ; i < numzon ; i++)
    if ((rslt = link_zone(i)) == -1)
      return(-1);

  return(rslt);
}

/* could use some optimization.  -1 = fail, 0 = succeed, 1 = warn */

int link_zone(int zn) {
  char *p, *z;
  int i, j, rslt, zone, ct;
  char buff[BUFFLEN];

  for (ct = 0 ; ct < znumobs(zn) ; ct++) {
    i = zobj_nr(ct, zn);

    if (!objs[i].unlinked)
      continue;
    else
      objs[i].unlinked = False;

    strcpy(buff, (char *) objs[i].oloc_reset);
    free((char *) objs[i].oloc_reset);

    if (!(p = strchr(buff, ':'))) {
      fprintf(stderr, "\nError in zone %s: Location keyword expects "
             "FLAG:location\nGot: %s\n", zname(ozone(i)), buff);
      return(-1);
    }
    *p = 0;
    p++;

    if ((z = strchr(p, '@'))) {
      *z++ = 0;
      for (zone = 0 ; zone < numzon ; zone++)
	if (EQ(zones[zone].z_name, z))
	  break;
      
      if (zone == numzon) {
	fprintf(stderr, "\nUnknown link to zone: %s\n", z);
	return(-1);
      }
    }
    else
      zone = ozone(i);

    switch (tlookup(buff, loctypes)) {
    case 0:                                               /* IN_CONTAINER */
      rslt = ht_lookup(objects_z, p, 0, zone, obj_search);
      if (rslt == -1) {
	fprintf(stderr, "\nUnknown container in zone %s: %s\n",
          zname(ozone(i)), p);
	return(-1);
      }
      oloc_reset(i) = rslt;
      ocarrf_reset(i) = IN_CONTAINER;
      ozone(i) = ozone(rslt);
      break;
    case 1:                                               /* IN_ROOM */
      rslt = ht_lookup(locations_z, p, 0, zone, room_search);
      if (rslt == -1) {
        fprintf(stderr, "\nUnknown location in zone %s: %s\n", 
          zname(ozone(i)), p);
        return(-1);
      }
      oloc_reset(i) = rslt;
      ocarrf_reset(i) = IN_ROOM;
      break;
    case 2:                                               /* CARRIED_BY */
      rslt = ht_lookup(ublock_z, p, 0, zone, mob_search);
      if (rslt == -1) {
	fprintf(stderr, "\nUnknown mobile in zone %s: %s\n",
          zname(ozone(i)), p);
	return(-1);
      }
      oloc_reset(i) = rslt;
      ocarrf_reset(i) = CARRIED_BY;
      ozone(i) = pzone(rslt);
      break;
    case 3:                                               /* WIELDED_BY */
      rslt = ht_lookup(ublock_z, p, 0, zone, mob_search);
      if (rslt == -1) {
	fprintf(stderr, "\nUnknown mobile in zone %s: %s\n",
          zname(ozone(i)), p);
	return(-1);
      }
      oloc_reset(i) = rslt;
      ocarrf_reset(i) = WIELDED_BY;
      ozone(i) = pzone(rslt);
      break;
    case 4:                                               /* WORN_BY */
      rslt = ht_lookup(ublock_z, p, 0, zone, mob_search);
      if (rslt == -1) {
        fprintf(stderr, "\nUnknown mobile in zone %s: %s\n", zname(ozone(i)), p);
        return(-1);
      }
      oloc_reset(i) = rslt;
      ocarrf_reset(i) = WORN_BY;
      ozone(i) = pzone(rslt);
      break;
    case 5:                                               /* BOTH_BY */
      rslt = ht_lookup(ublock_z, p, 0, zone, mob_search);
      if (rslt == -1) {
        fprintf(stderr, "\nUnknown mobile in zone %s: %s\n", zname(ozone(i)), p);
        return(-1);
      }
      oloc_reset(i) = rslt;
      ocarrf_reset(i) = BOTH_BY;
      ozone(i) = pzone(rslt);
      break;
    default:
      fprintf(stderr, "\nUnknown location tag in zone %s: %s\n", zname(ozone(i)), buff);
      return(-1);
      break;
    }

    if (olinked(i)) {
      if ((z = strchr((char *) objs[i].linked, '@'))) {   /* sets the zone */
	*z++ = 0;
	for (zone = 0 ; zone < numzon ; zone++)
	  if (EQ(zones[zone].z_name, z))
	    break;
	
	if (zone == numzon) {
	  fprintf(stderr, "\nUnknown link to zone: %s\n", z);
	  return(-1);
	}
      }
      else
	zone = ozone(i);
      
      if ((rslt = ht_lookup(objects_z, (char *) objs[i].linked, 0, zone,
          obj_search)) == -1) {
	fprintf(stderr, "\nObject %s linked to nonexistant %s.\n", oname(i),
	       (char *) objs[i].linked);
	fprintf(stderr, "\nFailed at zone %s\n", zname(zone));
	return(-1);
      }
      else {
	free((char *) objs[i].linked);
	olinked(i) = rslt;
      }
    }
    else
      olinked(i) = -1;
  }

  for (ct = 0 ; ct < znumchars(zn) ; ct++) {
    i = zmob_nr(ct, zn);

    if (!ublock[i].unlinked)
      continue;
    else
      ublock[i].unlinked = False;

    strcpy(buff, (char *) ublock[i].ploc_reset);
    free((char *) ublock[i].ploc_reset);

    if ((z = strchr(buff, '@'))) {
      *z++ = 0;
      for (zone = 0 ; zone < numzon ; zone++)
        if (EQ(zones[zone].z_name, z))
          break;

      if (zone == numzon) {
        fprintf(stderr, "\nUnknown link to zone: %s\n", z);
        return(-1);
      }
    }
    else
      zone = ublock[i].zone;

    if ((rslt = ht_lookup(locations_z, buff, 0, zone, room_search)) != -1)
      ublock[i].ploc_reset = rslt;
    else {
      fprintf(stderr, "\nMobile \"%s\" in zone %s linked to %s (invalid loc)\n",
	     pname(i), zones[pzone(i)].z_name, buff);
      return(-1);
    }
  }


  for (ct = 0 ; ct < znumloc(zn) ; ct++) {
    i = zloc_nr(ct, zn);

    if (!room_data[i].unlinked)
      continue;
    else
      room_data[i].unlinked = False;

    for (j = 0 ; j < NEXITS ; j++) {
     if (!lexit_reset(i, j))
       lexit_reset(i, j) = -1; 
     else {
	strcpy(buff, (char *) lexit_reset(i, j));
	free((char *) lexit_reset(i, j));
	
	if (*buff == '^')
	  p = buff + 1;
	else
	  p = buff;
	
	if ((z = strchr(p, '@'))) {
	  *z++ = 0;
	  for (zone = 0 ; zone < numzon ; zone++)
	    if (EQ(zname(zone), z))
	      break;
	  
	  if (zone == numzon) {
            lexit_reset(i, j) = -1;
#ifdef GEN
	    fprintf(stderr, "\nWARNING: Unknown link to zone %s in zone %s.", 
                    z, zname(lzone(i)));
#endif
	    continue;
	  }
	}
	else
	  zone = lzone(i);
	
	if (*buff == '^') {
	  if ((rslt = ht_lookup(objects_z, p, 0, zone, obj_search)) != -1)
	    lexit_reset(i, j) = DOOR + rslt;
	  else {
	    fprintf(stderr, "\nLocation \"%s\" in zone %s linked to %s "
                   "(invalid obj)\n", lname(i), zname(lzone(i)), p);
	    return(-1);
	  }
	}
	else {
	  if ((rslt = ht_lookup(locations_z, p, 0, zone, room_search)) != -1)
	    lexit_reset(i, j) = rslt;
	  else {
	    fprintf(stderr, "\nLocation \"%s\" in zone %s linked to %s "
                  "(invalid loc)\n", lname(i), zname(lzone(i)), p);
	    return(-1);
	  }
	}
      }
    }
  }
  return(0);
}

int store_headers(void) {
  int i;
  FILE *fptr;
  char buff1[256], buff2[256];

  if (!(fptr = fopen(LOCATIONS_H, "w"))) {
    fprintf(stderr, "\nUnable to open %s for writing.\n", LOCATIONS_H);
    return(-1);
  }
  else {
    fprintf(fptr, "/*\n"
             "**\tLocations file header generated from %s\n"
             "**\tDON'T MAKE CHANGES HERE -- THEY WILL GO AWAY!\n"
             "*/\n\n"
             "#ifndef _LOCATIONS_H\n"
             "#define _LOCATIONS_H\n\n", ZONE_DIR);

    for (i = 0 ; i < numloc ; i++) {
      if (i == 0 || room_data[i].zone != room_data[i-1].zone) {
        ucase(buff1, zname(room_data[i].zone));
        fprintf(fptr, "#define LOCMIN_%s\t%d\n", buff1, i);
        fprintf(fptr, "#define LOCMAX_%s\t%d\n", buff1,
                i + zmaxlocs(room_data[i].zone));
      }

      ucase(buff1, zones[room_data[i].zone].z_name);
      ucase(buff2, room_data[i].r_name);
      fprintf(fptr, "#define LOC_%s_%s\t%d\n", buff1, buff2, i);
    }

    fprintf(fptr, "#endif\n");
    fclose(fptr);
  }

  if (!(fptr = fopen(OBJECTS_H, "w"))) {
    fprintf(stderr, "\nUnable to open %s for writing.\n", OBJECTS_H);
    return(-1);
  }
  else {
    fprintf(fptr, "/*\n"
             "**\tObjects file header generated from %s\n"
             "**\tDON'T MAKE CHANGES HERE -- THEY WILL GO AWAY!\n"
	    "*/\n\n"
             "#ifndef _OBJECTS_H\n"
             "#define _OBJECTS_H\n\n", ZONE_DIR);
   
    for (i = 0 ; i < numobs ; i++) {
      ucase(buff1, zname(ozone(i)));
      ucase(buff2, ozname(i));
      fprintf(fptr, "#define OBJ_%s_%s\t%d\n", buff1, buff2, i);
    }
    fprintf(fptr, "#endif\n");
    fclose(fptr);
  }

  if (!(fptr = fopen(MOBILES_H, "w"))) {
    fprintf(stderr, "\nUnable to open %s for writing.\n", MOBILES_H);
    return(-1);
  }
  else {
    fprintf(fptr, "/*\n"
             "**\tMobiles file header generated from %s\n"
             "**\tDON'T MAKE CHANGES HERE -- THEY WILL GO AWAY!\n"
	    "*/\n\n"
             "#ifndef _MOBILES_H\n"
             "#define _MOBILES_H\n\n", ZONE_DIR);

    for (i = 0 ; i < numchars ; i++) {
      ucase(buff1, zname(pzone(i)));
      ucase(buff2, pzname(i));
      fprintf(fptr, "#define MOB_%s_%s\t%d\n", buff1, buff2, i);
    }
    fprintf(fptr, "#endif\n");
    fclose(fptr);
  }
  return(0);
}

int obj_search(int a, int b) {
  if (objs[a].zone == b)
    return(1);
  return(0);
}

int room_search(int a, int b) {
  if (room_data[a].zone == b)
    return(1);
  return(0);
}

int mob_search(int a, int b) {
  if (ublock[a].zone == b)
    return(1);
  return(0);
}

int llookup (char *elem, char **table) {
  return(glookup(elem, 1, table, strncasecmp));
}
