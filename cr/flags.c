#include "kernel.h"
#include "pflags.h"
#include "lflags.h"
#include "aflags.h"
#include "oflags.h"
#include "mflags.h"
#include "sflags.h"
#include "nflags.h"
#include "eflags.h"
#include "quests.h"
#include "flags.h"
#include "verbs.h"
#include "objsys.h"
#include "parse.h"
#include "flagindex.h"
#include "mobile.h"
#include "flags.h"
#include "bprintf.h"
#include "rooms.h"
#include "uaf.h"

extern char *Oflags[];
int get_key_value(char *, char **, char **);
int proc_flags(int, long int *, int[], char *, char *[], char *);

char *TF[3] = {"False", "True", TABLE_END};
char *OO[3] = {"Off", "On", TABLE_END};

/* stores a players's flags to a file */

void store_flags(FILE *outfp, PERSONA *player) {
  int i;
  char buff[256];
  char *p;

  for (i = 0 ; i < MAX_MOB_FLAGS ; i++) {
    p = dump_bits(player->ublock.bits, mindex, mobflagsindex[i], i);
    if (*p) {
      strcpy(buff, Mobflags[i]);
      fprintf(outfp, "%%%-9s { %s}\n", buff, p); // Token len is 11
    }
  }
}

/* next three functions are very similar.  inverse controls which   *
 * to store, reset flags (false) or non reset flags (true). it      *
 * also forces the game to store an empty set if it's set to        *
 * non reset flags, so that when it updates, the mud will reset     *
 * that to zeros                                                    */

void store_obj_flags(Boolean inverse, char *prefix, FILE *outfp, int obj) {
  int i;
  char buff[256];
  char *p;

  for (i = 0 ; i < MAX_OBJ_FLAGS ; i++) {
    if ((strstr(Objflags[i], "Reset") && !inverse) || 
        (!strstr(Objflags[i], "Reset") && inverse)) {
      p = dump_bits(obits(obj), oindex, oflagsindex[i], i);
      if (*p || inverse) {
        strcpy(buff, Objflags[i]);
        if (!inverse)
          *strstr(buff, "Reset") = 0;
        fprintf(outfp, "%s%s { %s}\n", prefix, buff, p);
      }
    }
  }
}

/* stores a location's flags to a file */

void store_loc_flags(Boolean inverse, char *prefix, FILE *outfp, int loc) {
  int i;
  Boolean set = False;
  char buff[256];
  char *p;

  for (i = 0 ; i < MAX_LOC_FLAGS ; i++) {
    if ((strstr(Locflags[i], "Reset") && !inverse) ||
        (!strstr(Locflags[i], "Reset") && inverse)) {
      p = dump_bits(lbits(loc), lindex, lflagsindex[i], i);
      if (*p || inverse) {
        strcpy(buff, Locflags[i]);
        if (!inverse) 
          *strstr(buff, "Reset") = 0;
        fprintf(outfp, "%s%s { %s}\n", prefix, buff, p);
        set = True;
      }
    }
  }
  if (!set && !*prefix)
    fprintf(outfp, "lflags { }\n");
}

/* stores a mobile's flags to a file */

void store_mob_flags(Boolean inverse, char *prefix, FILE *outfp, int mob) {
  int i;
  char buff[256];
  char *p;

  for (i = 0 ; i < MAX_MOB_FLAGS ; i++) {
    if ((strstr(Mobflags[i], "Reset") && !inverse) ||
        (!strstr(Mobflags[i], "Reset") && inverse)) {
      p = dump_bits(mbits(mob), mindex, mobflagsindex[i], i);
      if (*p || inverse) {
        strcpy(buff, Mobflags[i]);
        if (!inverse) 
          *strstr(buff, "Reset") = 0;
        fprintf(outfp, "%s%s { %s}\n", prefix, buff, p);
      }
    }
  }
}

int boot_pflags(void) {
  char buff[256], *p, *q;
  int lev, oldlev, bitnum, i, type, len, j;
  long int *from, *to;
  FILE *fptr;

  oldlev = type = lev = -1;

  if (!(fptr = fopen(DATA_DIR "/pflags", "r"))) {
    printf("Unable to open pflags file.\n");
    return(-1);
  }
  else 
    while (!feof(fptr)) {
      fgets(buff, 255, fptr);

      if ((p = strchr(buff, '\n')))
	*p = 0;

      if (*buff == '#' || allspaces(buff) || feof(fptr))
	continue;
      else {
      	if (!strncasecmp(buff, "Pflag", 5))
	  type = PFLAGS;
	else if (!strncasecmp(buff, "Mask", 4))
	  type = MASKS;
	else {
	  if (type == -1) {
	    printf("Boot Pflags: first line not PflagsLevel: or MaskLevel:\n");
	    return(-1);
	  }
	  else {               /* a line with names of flags on it */
	    q = buff;
	    
	    while (*q) {
	      for (p = q ; *p ; p++)
		if (!isspace(*p))
		  break;
	      
	      for (q = p ; *q ; q++)
		if (isspace(*q)) {
		  *q++ = 0;
		  break;
		}
	      
	      bitnum = tlookup(p, Pflags);
	      
	      if (bitnum == -1) {
		printf("Boot Pflags: Invalid flag (%s)\n", p);
		return(-1);
	      }
	      else
		set_bit(pbits(lev), pflindex, bitnum, True, type);

	      p = q;
	    }
	    continue;
	  }
	}
	
	if ((q = strchr(buff, ':')))   /* fall through here */
	  *q = 0;
	else {
	  printf("Boot Pflags: Invalid line (%s) - missing \":\"\n", buff);
	  return(-1);
	}
	
	if ((q = strchr(buff, ' ')))
	  *q = 0;
	if ((q = strchr(buff, '\n')))
	  *q = 0;

	if (type == PFLAGS)
	  p = buff + 5;
	else
	  p = buff + 4;
	
        if (EQ(p, "Novice"))
	  lev = 0;
	else if ((lev = tlookup(p, MWizLevels)) == -1) {
	  printf("Boot Pflags: Invalid level %s\n", p); 
	  return(-1);
	}
	else if (lev < oldlev) {
	  printf("Boot Pflags: Levels must come in order\n");
	  return(-1);
	}
	else {
	  if (oldlev != -1) {                     /* copy over older bits */
	    for (i = oldlev ; i < lev ; i++) {
	      from = &the_world->pbits[i][pflindex[MASKS]];
	      to = &the_world->pbits[i+1][pflindex[MASKS]];
	      len = pflindex[MASKS+1] - pflindex[MASKS];
	      
	      for (j = 0 ; j < len ; j++)
		to[j] = from[j];

	      from = &the_world->pbits[i][pflindex[PFLAGS]];
	      to = &the_world->pbits[i+1][pflindex[PFLAGS]];
	      len = pflindex[PFLAGS+1] - pflindex[PFLAGS];
	      
	      for (j = 0 ; j < len ; j++)
		to[j] = from[j];
	    }
	  }
	  oldlev = lev;
	}
      }
    }
  fclose(fptr);
  return(0);
}

void set_xpflags(long int bits[], int l) {
  long int *from, *to, len, i;

  from = &the_world->pbits[wlevel(l) - 1][pflindex[PFLAGS]];
  to = &bits[mindex[PFLAGS]];
  len = mindex[PFLAGS+1] - mindex[PFLAGS];

  if (l >= LVL_CREATOR)
    for (i = 0 ; i < len ; i++)
      to[i] = 0xFFFFFFFF;
  else
    for (i = 0 ; i < len ; i++)
      to[i] = from[i];

  from = &the_world->pbits[wlevel(l) - 1][pflindex[MASKS]];
  to = &bits[mindex[PMASK]];
  len = mindex[PMASK+1] - mindex[PMASK];

  if (l >= LVL_CREATOR)
    for (i = 0 ; i < len ; i++)
      to[i] = 0xFFFFFFFF;
  else
    for (i = 0 ; i < len ; i++)
      to[i] = from[i];  
}

/* loads a line of flags from the player file */

void load_flags(char *flags, PERSONA *pers) {
  char *key, *val;
  int rslt;

  if (get_key_value(flags + 1, &key, &val) == -1) { /* +1 is for '%' */
    mudlog("Invalid flags line: %s\n", flags);
    return; 
  }
  if ((rslt = tlookup(key, Mobflags)) == -1) {
    mudlog("Unknown key in player file: %s\n", key);
    return;
  }
  else
    proc_flags(rslt, pers->ublock.bits, mindex, key, mobflagsindex[rslt], val);
}

/* This is used for copying reset bits to use bits */

void copy_bits(long int bits[], int fpoints[], int flagsto, 
  int flagsfrom) {
  int stop = fpoints[flagsfrom + 1];
  int i, j;

  for (i = fpoints[flagsfrom], j = fpoints[flagsto] ; i < stop ; i++, j++)
    bits[j] = bits[i];
}

/* Primitive for setting a bit in a flagset, use the macro instead */

void set_bit(long int bits[], int fpoints[], int bitnum, Boolean val, 
             int flagset) {
  int len = fpoints[flagset + 1] - fpoints[flagset];
  int intpos, bitpos;

  if (len * 32 <= bitnum || bitnum < 0)
    mudlog("Attempt to set bit %d of flagset %d", bitnum, flagset);
  else {
    intpos = fpoints[flagset] + (bitnum / 32);
    bitpos = bitnum % 32;

    if (val)
      xsetbit(bits[intpos], bitpos);
    else
      xclrbit(bits[intpos], bitpos);
  }
}

/* Primitive for testing a bit in a flagset, use the macro instead */

Boolean test_bit(long int bits[], int fpoints[], int bitnum, int flagset) {
  int len = fpoints[flagset + 1] - fpoints[flagset];
  int intpos, bitpos;

  if (len * 32 <= bitnum || bitnum < 0) {
    mudlog("Attempt to test bit %d of flagset %d", bitnum, flagset);
    return(False);
  }
  else {
    intpos = fpoints[flagset] + (bitnum / 32);
    bitpos = bitnum % 32;
   
    return(xtstbit(bits[intpos], bitpos) ? True : False);
  }
}

void obj_flags(int flagset, int viewflag, int edtflg, char *f_names[]) {
  int o, b, c;

  if (!ptstflg(mynum, viewflag) && plev(mynum) < LVL_GOD)
    bprintf("Pardon?\n");
  else if ((o = ob1) == -1)
    bprintf("What's that?\n");
  else {
    if (!*item2) {
      bprintf ("Object: %s\nFlags:\t", oname(o));
      show_bits(obits(o), oindex, f_names, flagset, False);
    }
    else if (!*item3) {
      if ((b = tlookup(item2, f_names)) == -1)
	bprintf("%s: No such flag.\n", item2);
      else {
	c = test_bit(obits(o), oindex, b, flagset);
	bprintf("Value of %s is %s\n", f_names[b], TF[c]);
      }
    }
    else {
      if ((b = tlookup(item2, f_names)) == -1)
	bprintf("%s: No such flag.\n", item2);
      else if ((c = tlookup(item3, TF)) == -1)
	bprintf("Value must be True or False.\n");
      else {
        bprintf("%s is now set to %s.\n", f_names[b], TF[c]);
	if (!c)
	  set_bit(obits(o), oindex, b, False, flagset);
	else
          set_bit(obits(o), oindex, b, True, flagset);
      }
    }
  }
}

void loc_flags(int flagset, int viewflag, int edtflg, char *f_names[]) {
  int l, b, c;

  if (!ptstflg(mynum, viewflag) && plev(mynum) < LVL_GOD)
    bprintf("Pardon?\n");
  else if ((l = find_loc_by_name(item1)) == -1)
    bprintf("Where's that?\n");
  else {
    if (!*item2) {
      bprintf ("Location: %s\nFlags:\t", lname(l));
      show_bits(lbits(l), lindex, f_names, flagset, False);
    }
    else if (!*item3) {
      if ((b = tlookup(item2, f_names)) == -1)
	bprintf("%s: No such flag.\n", item2);
      else {
	c = test_bit(lbits(l), lindex, b, flagset);
	bprintf("Value of %s is %s\n", f_names[b], TF[c]);
      }
    }
    else {
      if ((b = tlookup(item2, f_names)) == -1)
	bprintf("%s: No such flag.\n", item2);
      else if ((c = tlookup(item3, TF)) == -1)
	bprintf("Value must be True or False.\n");
      else {
        bprintf("%s is now set to %s.\n", f_names[b], TF[c]);
	if (!c)
	  set_bit(lbits(l), lindex, b, False, flagset);
	else
          set_bit(lbits(l), lindex, b, True, flagset);
      }
    }
  }
}

void mob_flags(int flagset, int viewflag, int edtflag, char *f_names[]) {
  long int *bits;
  char *name;
  int lev, b, c;
  PERSONA d;
  Boolean file = False;
  int p = -1;

  if (!ptstflg(mynum, viewflag) && plev(mynum) < LVL_GOD)
    bprintf("Pardon?\n");
  else if ((p = fmbn(item1)) == -1 && !ptstflg(mynum, PFL_UAF) &&
    (plev(mynum) < LVL_GOD))
      bprintf("Who'se that?\n");
  else if (p == -1 && !getuaf(item1, &d))
    bprintf("No such persona in system.\n");
  else {
    if (pl1 == -1 || p != -1) {
      p = (pl1 == -1) ? mynum : p;
      name = pname(p);
      lev = plev(p);
      bits = mbits(p);
    }
    else {
      file = True;
      name = d.ublock.pname;
      lev = d.ublock.plev;
      bits = d.ublock.bits;
    }

    if (!EQ(name, pname(mynum)) && !ptstflg(mynum, edtflag))
      bprintf("You can only change your own, sorry.\n");
    else if (plev(mynum) < LVL_GOD &&
	     (!(EQ(name, pname(mynum))) && wlevel(plev(mynum)) <= wlevel(lev)))
      bprintf ("That is beyond your powers.\n");
    else if (!*item2) {
      bprintf ("Player/Mob: %s\nFlags:\n", name);
      show_bits(bits, mindex, f_names, flagset, False);
    }
    else if (!*item3) {
      if ((b = tlookup(item2, f_names)) == -1)
	bprintf("%s: No such flag.\n", item2);
      else {
	c = test_bit(bits, mindex, b, flagset);
	bprintf("Value of %s is %s\n", f_names[b], TF[c]);
      }
    }
    else {
      if ((b = tlookup(item2, f_names)) == -1)
	bprintf("%s: No such flag.\n", item2);
      else if ((c = tlookup(item3, TF)) == -1)
	bprintf("Value must be True or False.\n");
      else if (plev(mynum) < LVL_GOD && 
	       (flagset == PMASK || flagset == PFLAGS) && !ptstmsk(mynum, b))
	bprintf("You don't have that mask bit.\n");
      else {
	if (p != mynum)
	  mudlog("FLAG: %s by %s, %s = %s", name, pname(mynum), 
		 f_names[b], TF[c]);
        bprintf("%s is now set to %s.\n", f_names[b], TF[c]);
	if (!c)
	  set_bit(bits, mindex, b, False, flagset);
	else
          set_bit(bits, mindex, b, True, flagset);

	if (file)
	  putuaf (&d);
      }
    }
  }
}

Boolean flags_parse(int verb) {
  switch (verb) {
  case VERB_AFLAGS:
    if (ob1 != -1 && !otstbit(ob1, OFL_WEARABLE)) {
      bprintf("That isn't wearable.\n");
      break;
    }  
    else 
      obj_flags(AFLAGS, PFL_AFLAGS, PFL_AFLAGEDIT, Aflags);
    break;
  case VERB_OFLAGS:
    obj_flags(OFLAGS, PFL_OFLAGS, PFL_OFLAGEDIT, Oflags);
    break;
  case VERB_MASK:
    mob_flags(PMASK, PFL_MASK, PFL_MASKEDIT, Pflags);
    break;
  case VERB_EFLAGS:
    mob_flags(EFLAGS, PFL_EFLAGS, PFL_EFLAGEDIT, Eflags);
    break;
  case VERB_LFLAGS:
    loc_flags(LFLAGS, PFL_LFLAGS, PFL_LFLAGEDIT, Lflags);
    break;
  case VERB_MFLAGS:
    mob_flags(MFLAGS, PFL_MFLAGS, PFL_MFLAGEDIT, Mflags);
    break;
  case VERB_NFLAGS:
    mob_flags(NFLAGS, PFL_NFLAGS, PFL_NFLAGEDIT, Nflags);
    break;
  case VERB_PFLAGS:
    mob_flags(PFLAGS, PFL_PFLAGS, PFL_PFLAGEDIT, Pflags);
    break;
  case VERB_SFLAGS:
    mob_flags(SFLAGS, PFL_SFLAGS, PFL_SFLAGEDIT, Sflags);
    break;
  default:
    return(False);
  }
  return(True);
}

/* formatted list of the names of bits */

void show_bits(long int bits[], int fpoints[], char *flnames[],
  int flagset, Boolean inverse) {
  int bitnum, i, j, rslt;
  char *flag;

  for (j = 1, i = 0 ; ; i++)
    for (bitnum = 0 ; bitnum < 32 ; bitnum++) {
      if (flagset == QFLAGS)
        flag = ((QUEST *) flnames)[i*32 + bitnum].name;
      else
        flag = flnames[i*32 + bitnum];
      
      if (flag == TABLE_END) {
        bprintf("\n");
        return;
      }
      else if (((rslt = xtstbit(bits[fpoints[flagset] + i], bitnum)) &&
		!inverse) || (!rslt && inverse))
  	bprintf("%-16s%s", flag, !(j++ % 5) ? "\n" : "");
    }
}

/* used to produce an unformatted string containing the names of bits */

char *dump_bits(long int bits[], int fpoints[], char *flnames[], int flagset) {
  static char buff[4096];
  int len = fpoints[flagset+1] - fpoints[flagset];
  int bitnum, i, j;

  j = 1;
  *buff = 0;

  for (i = 0 ; i < len ; i++) {
    for (bitnum = 0 ; bitnum < 32 ; bitnum++)
      if (xtstbit(bits[fpoints[flagset] + i], bitnum)) {
        if (flnames[i*32 + bitnum] == TABLE_END)
          return(buff);
        else {
	  strcat(buff, flnames[i*32 + bitnum]);
          strcat(buff, " ");
        } 
     }
  }
  return(buff);
}

Boolean dump_pflags (void) {
  static char *t[] = { "Pflags", "Mask", TABLE_END };
  int k, k2;

  if (plev (mynum) < LVL_ARCHWIZARD || EMPTY(item1))
    return(False);
  else if (EMPTY(item2)) {
    bprintf("You need to give a level. Example: GLOBAL pflags Creator\n");
    return(True);
  }
  else if ((k = tlookup(item1, t)) < 0) {
    bprintf("You need to type: GLOBAL pflags <lev> or GLOBAL mask <lev>\n"); 
    return(True);
  }
  else if ((k2 = tlookup(item2, MWizLevels)) >= LEV_APPRENTICE)
    bprintf ("%s for %s:\n", t[k], MWizLevels[k2]);
  else if ((k2 = tlookup(item2, FWizLevels)) >= LEV_APPRENTICE)
    bprintf ("%s for %s:\n", t[k], FWizLevels[k2]);
  else if (EQ(item2, "Novice")) {
    bprintf ("%s for %s:\n", t[k], "Novice");
    k2 = 0;
  }
  else {
    bprintf("That isn't a level. Example: GLOBAL pflags Wizard\n");
    return(True);
  }

  if (k == 0)
    show_bits(pbits(k2), pflindex, Pflags, PFLAGS, False);
  else
    show_bits(pbits(k2), pflindex, Pflags, MASKS, False);

  return(True);
}

Boolean bitschanged(long int bits[], int idx[], char *flsetnames[], int len) {
  int a, b, i;

  for (i = 0 ; i < len ; i++) {
    if (strstr(flsetnames[i], "Reset")) {
      for (a = idx[i], b = idx[i-1] ; b != idx[i] ; a++ , b++)
        if (bits[a] != bits[b])
          return(True);
    }
  }  
  return(False);
}
