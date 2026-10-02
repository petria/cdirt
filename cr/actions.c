#include <stdlib.h>
#include <unistd.h>
#include "kernel.h"
#include "locations.h"
#include "sendsys.h"
#include "bprintf.h"
#include "objects.h"
#include "mobiles.h"
#include "actions.h"
#include "mobile.h"
#include "sflags.h"
#include "mflags.h"
#include "objsys.h"
#include "zones.h"
#include "parse.h"
#include "rooms.h"
#include "fight.h"
#include "cflags.h"
#include "log.h"
#include "lflags.h"

#define BLEN      256
#define FLEN      100
#define ALEN      30
#define F_ALL     0x1000
#define F_SINGLE  0x100
#define F_ALOOF   0x10
#define F_HOSTILE 0x1

static char *s_ext[] =
{"Flowers", "Pet", "Pray", "Rose",
 "Tickle", "Wave", "Wipe", TABLE_END};

long int set_flags(char *line) {
  char *p;
  long int flags = 0;

  for (p = line ; *p ; p++)
    switch (*p) {
      case 'a':
        flags = flags | F_ALL;
        break;
      case 's':
        flags = flags | F_SINGLE;
        break;
      case 'i':
        flags = flags | F_ALOOF;
        break;
      case 'f':
        break; /* for now */
      default:
        mudlog("ACTIONS: Unknown flag in action - %c", *p);
    }
  return(flags);
}

int boot_actions(void) {
  char *p;
  FILE *f;
  int num_booted, str_size;
  char buff[BLEN], flagstr[FLEN], action[ALEN];
  Actionptr cur_act = (Actionptr) NEW(Action, 1);

  num_booted = 0;
  str_size = 0;
  actions = 0;

  if (!(f = FOPEN(DATA_DIR "/actions", "r")))
    return(-1);

  while(!feof(f)) {
    fgets(buff, BLEN - 1, f);

    if (*buff == ':') {
      if (sscanf(buff, ":%s %s", action, flagstr) != 2)
        break;
      else {
        if (num_booted++ > 0)            /* add prev. action */
          add_action(&actions, cur_act);
        cur_act = (Actionptr) NEW(Action, 1);
        cur_act->name = COPY(action);
        str_size += strlen(action) + 1;
        cur_act->flags = set_flags(flagstr);
      }
    }
    else if ((p = strchr(buff, ':'))) {
      *p = 0;
      if (!strcmp(buff, "me"))
        cur_act->tome = COPY(p + 1);
      else if (!strcmp(buff, "all"))
        cur_act->toall = COPY(p + 1);
      else if (!strcmp(buff, "target"))
        cur_act->totarget = COPY(p + 1);
      else if (!strcmp(buff, "sender"))
        cur_act->tosender = COPY(p + 1);
      else if (!strcmp(buff, "others"))
        cur_act->toothers = COPY(p + 1);
      else
        mudlog("ACTIONS: Unknown mesage code %s in %s", buff, cur_act->name);
      str_size += strlen(p + 1) + 1;
    }
  }
  add_action(&actions, cur_act);
  FCLOSE(f);
  return(0);
}

void add_action(Actionptr *base, Actionptr newone) {
  Actionptr aptr;

  newone->nxt = NULL;
  if (!*base)
    *base = newone;
  else {
    for (aptr = actions ; aptr->nxt ; aptr = aptr->nxt);
    aptr->nxt = newone;
  }
}

void actionscom (void) {
  FILE *f;
  char line[80], fn[255];
  int i = -1;
  int j = 0;
  char **t;
  Actionptr v;

  if (brkword () != -1)
    dump_act_vb (wordbuf);
  else {
    sprintf (fn, "%s/ACTIONS.%s", TEMP_DIR, pname (mynum));

    if ((f = FOPEN(fn, "w")) == NULL) {
      bprintf ("Error reading syslog file.\n");
      mudlog ("ERROR: Cannot load syslog temp file");
      return;
    }
    fprintf (f, "The following actions are defined:\n");
    for (v = actions; v ; v = v->nxt) {
      ++j;
      ++i;
      if (i % 9 == 0)
        fprintf (f, "\n");
      strcpy (line, v->name);
      *line = toupper (*line);
      fprintf (f, "%s\t", line);
    }
    for (t = s_ext; *t != TABLE_END; ++t) {
      ++j;
      ++i;
      if (i % 9 == 0)
        fprintf (f, "\n");
      fprintf (f, "%s%c", *t, t[1] == TABLE_END ? '\n' : '\t');
    }
    fprintf (f, "\n&+wThere are &+W%d &+wactions defined.\n", j);
    FCLOSE(f);
    bprintf("\001f%s\003", fn);
  }
  return;
}


void dump_act_vb (char *s) {
  Actionptr v;
  int i;

  for (v = actions; v != NULL && !EQ (v->name, wordbuf); v = v->nxt);
  if (v == NULL) {
    if ((i = tlookup (wordbuf, s_ext)) < 0)
      bprintf ("I don't know about any %s action.\n", wordbuf);
    else
      bprintf ("%s is a special action.\n", s_ext[i]);
  }
  else 
    dump_action (v);
}

void dump_action(Actionptr v) {
  char buff[BLEN];

  if (v == NULL)
    return;
  bprintf ("Action %s:\n", v->name);
  bprintf ("Flags: ");
  if (v->flags & F_ALL)
    bprintf ("All ");
  if (v->flags & F_SINGLE)
    bprintf ("Single ");
  if (v->flags & F_HOSTILE)
    bprintf ("Hostile ");
  if (v->flags & F_ALOOF)
    bprintf ("Aloof ");
  bprintf ("\n");
  if (v->flags & F_ALL) {
    astrcpy (-1, buff, v->toall);
    bprintf ("Msg to all: %s", buff);
    astrcpy (-1, buff, v->tome);
    bprintf ("Msg to me: %s", buff);
  }
  if (v->flags & F_SINGLE) {
    astrcpy (-1, buff, v->totarget);
    bprintf ("Msg to target: %s", buff);
    astrcpy (-1, buff, v->tosender);
    bprintf ("Msg to sender: %s", buff);
    if (v->toothers) {
      astrcpy (-1, buff, v->toothers);
      bprintf ("Msg to others: %s", buff);
    }
  }
  bprintf ("\n");
}

Actionptr find_act(char *word, Actionptr base) {
  Actionptr v;

  for (v = base ; v ; v = v->nxt)
    if (EQ(v->name, word))
      break;

  if (!v)
    return(NULL);
  else
    return(v);
}

int do_action(Actionptr v) {
  int p = -1;
  Boolean bad_player = False;
  char buff[BLEN], *aptr;

  if ((aptr = strchr(strbuf, ' '))) {     /* has a recepient */
    aptr++;

    if (strchr(aptr, ' '))
      *strchr(aptr, ' ') = 0;

    p = fmbn(aptr);
    if (p == -1)
      bad_player = True;
  }

  if (bad_player)
    bprintf("No such player.\n");
  else if (pfighting (mynum) != -1)
    bprintf("Not in a fight!\n");
  else if (!(v->flags & F_ALL) && p == -1)
    bprintf("That action requires a target player.\n");
  else if (!(v->flags & F_SINGLE) && p >= 0)
    bprintf("You can't do that to them.\n");
  else if ((v->flags & F_HOSTILE) && p >= 0 && testpeace(mynum))
    bprintf("Nah, that's violent.\n");
  else if (p == mynum)
    bprintf ("Good trick, that.\n");
  else if ((v->flags & F_ALOOF) != 0 && ststflg(p, SFL_ALOOF) &&
    plev(mynum) < LVL_WIZARD)
      bprintf("%s doesnt want to be bothered with your actions.\n", pname(p));
  else if (p < 0) {
    astrcpy(p, buff, v->toall);
    sillycom(buff);
    astrcpy(p, buff, v->tome);
    bprintf(buff);
  }
  else {
    astrcpy(p, buff, v->totarget);
    sendf(p, buff);
    if (v->toothers) {
      astrcpy(p, buff, v->toothers);
      send_msg(ploc(p), 0, pvis(mynum), LVL_MAX, mynum, p, buff);
    }
    astrcpy(p, buff, v->tosender);
    bprintf(buff);
    if ((v->flags & F_HOSTILE) && p >= max_players)
      hit_player(p, mynum, pwpn(p));
  }
  return(0);
}

void astrcpy(int plr, char *outs, char *line) {
  char *p, *op;

  *outs = 0;

  bzero(outs, BLEN);

  if (plr != -1 && ploc(plr) != ploc(mynum)) {
    strcpy(outs, "From far away, ");
    op = outs + strlen(outs);
  }
  else
    op = outs;

  for (p = line ; *p ; p++) {
    if (*p != '%')
      *op++ = *p;
    else if (*(++p)) {
      switch (*p) {
        case 'a':
          strcat(outs, "\001p");
          strcat(outs, pname(mynum));
          strcat(outs, "\003");
          break;
        case '^':
          if (psex(mynum))
            strcat(outs, "her");
          else
            strcat(outs, "his");
          break;
        case '~':
          if (psex(mynum))
            strcat(outs, "her");
          else
            strcat(outs, "him");
          break;
        case '@':
          if (psex(mynum))
            strcat(outs, "she");
          else
            strcat(outs, "he");
          break;
        case '$':
          if (plr == -1)
            strcat(outs, "[his/her]");
          else if (psex(plr))
            strcat(outs, "her");
          else
            strcat(outs, "his");
          break;
        case '#':
          if (plr == -1)
            strcat(outs, "[him/her]");
          else if (psex(plr))
            strcat(outs, "her");
          else
            strcat(outs, "him");
          break;
        case '!':
          if (plr == -1)
            strcat(outs, "[he/she]");
          else if (psex(plr))
            strcat(outs, "she");
          else
            strcat(outs, "he");
          break;
       case '+':
          if (plr == -1)
            strcat(outs, "[his/her]");
          else if (psex(plr))
            strcat(outs, "his");
          else
            strcat(outs, "hers");
          break;
        case 't':
          if (plr == -1)
            strcat(outs, "[target]");
          else {
            strcat(outs, "\001p");
            strcat(outs, pname(plr));
            strcat(outs, "\003");
          }
          break;
      }
      op += strlen(op);
    }
  }
  *op = 0;
}

void flowercom (void) {
  int a;

  if ((a = vichere ()) < 0)
    return;
  sendf (a, "\001p%s\003 sends you flowers.\n", pname (mynum));
  sendf (a, "&+Y         (*)\n");
  sendf (a, "&+M     *    &+g|    &+C*\n");
  sendf (a, "&+g    \\|/  \\|/  \\|/\n");
  sendf (a, "&+y   ---------------\n");
  bprintf ("You send %s flowers.\n", him_or_her (a));
}

void ticklecom (void) {
  int a;

#ifdef LOCMIN_FOREST
  if (ob1 == OBJ_FOREST_INSIDETREE) {
    bprintf ("You tickle the tree into paroxyms of giggles and manage to "
             "escape.\n");
    trapch (LOC_FOREST_F4);
    setpscore (mynum, pscore (mynum) + 100);
    return;
  }
#endif
  if ((a = vichere ()) < 0)
    return;
  if (a == mynum) {
    bprintf ("You tickle yourself.\n");
    return;
  }
  sendf(a, "\001p%s\003 tickles you.\n", pname(mynum));
  bprintf ("You tickle %s.\n", him_or_her (a));
}

void petcom (void) {
  int a;

  if ((a = vichere ()) < 0)
    return;
  if (a == mynum) {
    bprintf ("No petting yourself in public, please.\n");
    return;
  }
#ifdef LOCMIN_OAKTREE
  if (a == MOB_OAKTREE_OTTIMO) {
    bprintf ("Ottimo rubs up against your leg and licks your hand.\n");
    return;
  }
#endif
  sendf(a, "\001p%s\003 pats you on the head.\n", pname(mynum));
  bprintf ("You pat %s on the head.\n", him_or_her (a));
}

void wavecom (void) {
  int a;

  if (pfighting (mynum) >= 0) {
    bprintf ("What are you trying to do?  Make 'em die laughing?\n");
    return;
  }
  if (pl1 != -1) {
    bprintf("You wave at %s.\n", pname(pl1));
    sendf(pl1, "%s waves at you!\n", pname(mynum));
    send_msg(ploc(mynum), 0, LVL_MIN, LVL_MAX, mynum, pl1, 
      "\001p%s\003 waves at \001p%s\003.\n", pname(mynum), pname(pl1));
    return;
  }
  if (EMPTY(item1)) {
    send_msg(ploc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
      "\001p%s\003 waves happily.\n", pname(mynum));
    bprintf("You wave happily.\n");
    return;
  }

  if ((a = ob1) == -1)
    return;

  #include "wavecom.h"

  bprintf ("Nothing happens.\n");
}

void rosecom (void) {
  int x;

  static char *ColorTable[] = {
    "black", "blue", "red", "pink", "yellow", "white", TABLE_END 
  };
  static char *Colors[] = {
    "&+L", "&+b", "&+r", "&+R", "&+Y", "&+W"
  };

  if (pl1 == -1)
    pl1 = pl2;

  if (seeplr("Send a rose to who?", "Who's that?") == -1)
    return;

  if (!*item1 || (x = tlookup (item1, ColorTable)) < 0)
    if (!*item2 || (x = tlookup (item2, ColorTable)) < 0) {
      bprintf("You need to pick a color.  Example: ROSE PROMETHEUS RED\n");
      bprintf("Colors: &+LBlack, &+bBlue, &+rRed, &+RPink, &+YYellow, "
             "&+WWhite\n");
      return;
    }

  bprintf ("You send a %s%s &*rose to %s. %s@&+g>--`--,--&*\n",
           Colors[x], ColorTable[x], pname (pl1), Colors[x]);
  sendf (pl1, "\001p%s\003 sends you a %s%s &*rose. %s@&+g>--`--,--\n",
         pname (mynum), Colors[x], ColorTable[x], Colors[x]);
}

void
praycom (void)
{
#ifdef LOCMIN_MITHDAN
  if (ploc (mynum) == LOC_MITHDAN_MITHDAN35) {
    bprintf ("You bow down before the Guardian Dragons...\n");
    bprintf ("Pleased with your reaction, the dragons allow you to pass.\n");
    teletrap (LOC_MITHDAN_MITHDAN37);
    return;
  }
#endif
#if defined(LOCMIN_MOOR) && defined(LOCMIN_WASTE)
  if (ploc (mynum) == oloc (OBJ_MOOR_ALTAR))
    if (state (OBJ_MOOR_ALTAR)) {
      bprintf ("Some mighty force hurls you across the universe.\n"
               "The stars seem to spin and whirl around you, whipping into "
               "a fiery storm of\nmultihued flaming light, spinning, "
             "twisting, half blinding you in its wild\ncrazy flickerings.\n"
               "Suddenly, everything seems to jolt back to a kind of almost "
               "sanity.\n");
      trapch (LOC_WASTE_BEFORE);
      setobjstate (OBJ_MOOR_ALTAR, 0);
      return;
    }
#endif
#if defined(LOCMIN_WASTE) && defined(LOCMIN_CHURCH)
  if (loc2zone (ploc (mynum)) == loc2zone (LOCMAX_WASTE)) {
    if (pfighting (mynum) != -1) {
      bprintf ("Not while fighting!\n");
      return;
    }
    if ((my_random () % 100) < 50) {
      bprintf ("You feel as if some force is offended.  There is a terrific "
               "lurch as if the\nvery ground is alive, and then everything "
               "clears.\n");
      trapch (LOC_CHURCH_BEHIND);
    } 
    else
      bprintf ("No one seems to hear you, perhaps later?\n");
    return;
  }
#endif
  sillycom ("\001s%s\003%s falls down and grovels in the dirt.\n\003");
  bprintf ("You fall down and grovel in the dirt.\n");
}

void
wipecom (void)
{
  int plr;

  if ((plr = pl1) == -1) {
    bprintf ("Wipe who?\n");
    return;
  }
  if (ploc (plr) != ploc (mynum)) {
    bprintf ("They aren't here.\n");
    return;
  }
  if (plr == mynum) {
    bprintf ("You wipe yourself. (Hope nobody is watching..)\n");
    send_msg (ploc (mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
              "%s wipes %sself.\n", pname (mynum), him_or_her (mynum));
    return;
  }
  bprintf ("You wipe %s.\n", pname (plr));
  sendf (plr, "%s wipes you.\n");
  send_msg (ploc (mynum), 0, pvis (mynum), LVL_MAX, mynum, plr,
            "%s wipes \001p%s\003.\n", pname (mynum), pname (plr));
}

void
meditatecom (void) {
#ifdef LOCMIN_ANCIENT
  if (ploc (mynum) == LOC_ANCIENT_ANC59) {
    if (oarmor (OBJ_ANCIENT_LBOOK) == 0) {
      bprintf ("You meditate, but nothing seems to happen.\n");
    } else {
      bprintf ("You are teleported!\n\n\n");
      trapch (LOC_ANCIENT_ANC35);
    }
    return;
  }
#endif

  bprintf ("You meditate quietly in the corner.\n");
  return;
}
