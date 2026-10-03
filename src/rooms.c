#include <stdlib.h>
#include "kernel.h"
#include "locations.h"
#include "objects.h"
#include "sendsys.h"
#include "rooms.h"
#include "climate.h"
#include "parse.h"
#include "mobile.h"
#include "objsys.h"
#include "zones.h"
#include "bprintf.h"
#include "mobile.h"
#include "fight.h"
#include "uaf.h"
#include "log.h"
#include "mobiles.h"

extern int room_search(int, int);
extern HASH_TABLE locations_z[];

/* A room's short description (title)
 */
char *
sdesc (int room)
{
  return exists (room) ? lshort (room) : "Where no man has gone before";
}

/* A room's long description
 */
char *
ldesc (int room)
{
  return exists (room) ? llong (room) : "";
}

int getexit (int room, int ex) {
  return lexit (room, ex);
}


/* Set the <dir>-most exit in <room> to <dest>.
 * Return False if the new exit was a room which didn't exist. */

void setexit (int room, int dir, int dest) {
  int i, n;

  if (exists(lexit(room, dir))) {

    for (i = n = 0; i < NEXITS && n <= 1; i++)
      if (lexit (room, i) == lexit (room, dir))
	++n;

    /* If we are removing the only exit to that room from this: */
    if (n == 1) 
      remove_int (room, lexits_to_me(lexit(room, dir)));
  }
  if (exists (dest))
    add_int (room, lexits_to_me (dest));

  lexit (room, dir) = dest;
}


/* Get a random exit direction from a location, or -1 if none exists.
 */
int get_rand_exit_dir (int room) {
  int i, ex[NEXITS], newch, n_exits = 0;

  for (i = 0; i < NEXITS; i++) {
    if (!exists (lexit (room, i)) && lexit (room, i) < DOOR)
      continue;

    newch = lexit(room, i);
    if (!exists (newch)) {
      if (state (newch -= DOOR) > 0) {
	continue;
      } 
      else
	newch = obj_loc (olinked (newch));
    }
    ex[n_exits++] = i;
  }

  return n_exits == 0 ? -1 : ex[my_random () % n_exits];
}


/* Get a random exit (the room itself) from a location, or 0 if none exists.
 */
int
get_rand_exit (int room)
{
  int newch;
  int i = get_rand_exit_dir (room);

  if (i == -1)
    return 0;

  newch = lexit (room, i);

  return !exists (newch) ? obj_loc (olinked (newch - DOOR)) : newch;
}


int count_players (int loc, int flags) {
  int i, ct, mob;

  i = 0;

  for (ct = 0 ; ct < lnumchars(loc) ; ct++) {
    mob = lmob_nr(ct, loc);
    if (flags & COUNT_PLAYERS) {
      if (mob < max_players || ststflg(mob, SFL_OCCUPIED))
        i++;
    }
    if (flags & COUNT_MOBILES) {
      if (mob >= max_players && !ststflg(mob, SFL_OCCUPIED))
        i++;
    }
  }
  return(i);
}

void gotocom (Boolean tiptoe) {
  char xx[SETIN_MAX + 200];
  int a, pc;

  if (!ptstflg (mynum, PFL_GOTO)) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf("Goto where?\n");
    return;
  } 
  else if (ob1 != -1) {             /* expands out where something is */
    a = ob1;
    while(ocarrf(a) != IN_ROOM && ocarrf(a) < CARRIED_BY)
      a = oloc(a);
    if (ocarrf(a) == IN_ROOM)
      a = oloc(a);
    else                            /* CARRIED_BY etc... */
      a = ploc(oloc(a));
  }
  else if (pl1 != -1)
    a = ploc(pl1);
  else if ((a = find_loc_by_name(wordbuf)) == -1) {
    bprintf ("Unknown player, object or room.\n");
    return;
  }

  if (a == ploc(mynum)) {
    bprintf("Why? You're already there.\n");
    return;
  }
  if (chkroom (a, mynum) || 
      (ltstflg(a, LFL_NOGOTO) && plev (mynum) < LVL_GOD)) {
    bprintf ("A magical force prevents you from entering that room.\n");
    return;
  }
  if (plev (mynum) < LVL_GOD) {
    if (ltstflg (a, LFL_PRIVATE)) {
      pc = count_players (a, COUNT_PLAYERS);
      if (pc > 1) {
	bprintf ("I'm sorry.  There's a private conference in that location.\n");
	return;
      }
    }
  }

  if (!tiptoe)
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	      "%s\n", build_setin (SETIN_SETMOUT, xx, cur_player->setmout, 
              pname (mynum), NULL, NULL));
  else
    send_msg(sendloc(mynum), 0, max (pvis (mynum), LVL_GOD), LVL_MAX, mynum, 
              NOBODY, "%s\n", build_setin (SETIN_SETMOUT, xx,
              cur_player->setmout, pname (mynum), NULL, NULL));

  setploc (mynum, a);
  trapch (a);

  if (!tiptoe) {
    check_follow ();
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
      "%s\n", build_setin (SETIN_SETMIN, xx, cur_player->setmin, pname(mynum), 
      NULL, NULL));
  }
  else
    send_msg(sendloc(mynum), 0, max (pvis (mynum), LVL_GOD), LVL_MAX, mynum, 
      NOBODY, "%s\n", build_setin (SETIN_SETMIN, xx, cur_player->setmin,
      pname (mynum), NULL, NULL));
}

void exitcom (Boolean use_args) {
  int a, v, newch;
  int drnum, droff;
  int b = 0;
  char st[64];
  int room = 0;
  int exit = 0;
  Boolean gotroom = False; 
  Boolean gotexit = False;
  Boolean gotarg = False;

  if (use_args) {
    room = !(gotarg = brkword () != -1) ? ploc (mynum) : find_loc_by_name (wordbuf);

    if (gotarg && !ptstflg (mynum, PFL_GOTO)) {
      bprintf ("You aren't powerful enough to give arguments to EXITS.\n");
      return;
    }
  }
  else
    room = ploc(mynum);
  gotroom = exists (room);

  if (!gotroom && !(gotexit = ((exit = tlookup (wordbuf, Exits)) != -1))) {
    bprintf ("Non-existant room or exit.\n");
    return;
  }
  if ((!gotexit && brkword () == -1) || !use_args ) {
    if (r_isdark (room, mynum)) {
      bprintf ("It is dark.....\n");
      return;
    }
    if(ststflg(mynum, SFL_BLIND) && plev(mynum) < LVL_WIZARD) {
      bprintf("You are blind.\n");
      return;
    }
    bprintf ("&+CObvious exits are:\n");
 
    for (a = 0; a < 6; a++) {
      newch = getexit (room, a);
      if (newch >= DOOR) {
	drnum = newch - DOOR;
	droff = olinked (drnum);
	if (state (drnum))
	  continue;
	newch = obj_loc (droff);
      }

      if (newch == -1)
	continue;

      if (plev (mynum) < LVL_WIZARD)
	bprintf ("  &+y%-5s &+Y: &+G%s\n", Exits[a], sdesc (newch));
      else {
	v = findzone (newch, st);
	if (ltstflg (newch, LFL_MAZE) && (plev (mynum) < LVL_ARCHWIZARD))
	  bprintf ("  &+y%-5s &+Y: &+G%-45s &+Y: [Maze]\n",
		   Exits[a], sdesc (newch));
	else
	  bprintf ("  &+y%-5s &+Y: &+G%-45s &+Y: &+c%s%d %s\n",
		   Exits[a], sdesc (newch), st, v,
		   (ltstflg( newch, LFL_DEATH ) ) ? "&+r<&+LD&+r>&N" : "" );

      }
      b = 1;
    }
    if (b == 0)
      bprintf ("&+RNone....\n");
    return;
  }

  if (!gotroom)
    room = ploc (mynum);
  if (lpermanent (room) && !ptstflg (mynum, PFL_ROOM)) {
    bprintf ("Your are only powerful enough to change exits on\n"
	     "non-permanent (wizard-created) rooms.\n");
    return;
  }
  else if (!gotexit && (exit = tlookup (wordbuf, Exits)) == -1) {
    bprintf ("Illegal exit-name, use north, east, etc...\n");
    return;
  }
  else if (brkword () == -1 || !use_args)
    newch = 0;
  else { 
    if ((newch = find_loc_by_name(wordbuf)) == -1) {
      bprintf ("I don't know that destination.\n");
      return;
    }
  }
  setexit (room, exit, newch);

  bprintf ("Exit %s from %s is now %s.\n", Exits[exit],
	   sdesc (room), (newch == 0) ? "removed" : sdesc (newch));
}


/* Is a room dark or not? It's dark if the Lflag DARK is set but
 * not if there is someone in the room carrying somthing lit.
 */
Boolean
roomdark (int room)
{
  int i, j;

  if (!ltstflg (room, LFL_DARK))
    return False;

  for (i = 0; i < lnumobs (room); ++i) {
    if (otstbit (lobj_nr (i, room), OFL_LIT) &&
	!otstbit (lobj_nr (i, room), OFL_DESTROYED))
      return False;
  }

  for (i = 0; i < lnumchars (room); ++i) {
    for (j = 0; j < pnumobs (lmob_nr (i, room)); ++j) {
      if (otstbit (pobj_nr (j, lmob_nr (i, room)), OFL_LIT) &&
	  !otstbit (pobj_nr (j, lmob_nr (i, room)), OFL_DESTROYED))
	return False;
    }
  }

  for (i = 0; i < max_players; ++i) {
    if (ststflg (i, SFL_LIT))
      return False;
  }

  return True;
}

/* Determine if a player can see in a room or not. Same as above with the
 * difference that if they are wiz, they will allways be able to see.
 */
Boolean
r_isdark (int room, int plr)
{
  return roomdark (room) && plev (plr) < LVL_WIZARD;
}

Boolean
isdark ()
{
  return r_isdark (ploc (mynum), mynum);
}



void teletrap (int newch)
{
  send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	    "%s has left.\n", pname (mynum));

  send_msg(int2idx(newch, LOC), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	    "%s has arrived.\n", pname (mynum));

  trapch (newch);
}


Boolean trapch (int loc) {
  int ct, mob;

  setploc (mynum, loc);
  lookin (loc, 0);
  if (ltstflg (loc, LFL_DEATH)) {
    if (plev (mynum) < LVL_WIZARD) {
      quit_msg("You seem to have died...", "Deathroom");
      quit_player(False);
      return False;
    }
  }

  if (pfighting (mynum) != -1 && ploc (mynum) != ploc (pfighting (mynum)))
    setpfighting (mynum, -1);

  for (ct = 0; ct < lnumchars (loc); ct++) {
    mob = lmob_nr(ct,loc);
    if (pangry(mob) == mynum && pstr(mob) >= 0 && mob >= max_players) {
      bprintf("%s says, &+W\"&+YSo we meet again..&+W\"\n", pname(mob));
      hit_player(mob, mynum, pwpn(mob));
    }
  }
  return True;
}


void lookin (int loc, int showfl) {
  if (ststflg (mynum, SFL_BLIND))
    bprintf ("You're blind, you can't see a thing!\n");
  else {
    if (ptstflg (mynum, PFL_GOTO)) {
      if (plev(mynum) >= LVL_DEMI)
        bprintf("&+M%s &+B[&+C%s@%s&+B] &+Y[&+WAltitude: &+B%d&+Y]&N\n",
              showname(loc), lname(loc), 
              zname(loc2zone(ploc(mynum))), laltitude(loc));
      else
        bprintf("&+M%s &+Y[&+WAltitude: &+B%d&+Y]&N\n", 
                showname(loc), laltitude(loc));
      if (roomdark (loc) && plev (mynum) >= LVL_WIZARD)
	bprintf ("[DARK]\n");
    }
    if (isdark()) {
      bprintf ("It's dark.  Be careful or you may be eaten by a Grue!\n");
      return;
    }
    else
      bprintf ("&+g%s\n", sdesc (loc));
  }

if (plev(mynum) > LVL_WIZARD)
  bprintf("&+c[ %s]\n", dump_bits(lbits(loc), lindex, Lflags, LFLAGS));

  if (!isdark () && !ststflg (mynum, SFL_BLIND)) {
    if ((showfl & SHOW_LONG) != 0 || ststflg (mynum, SFL_BRIEF) == 0)
      bprintf ("%s\n", ldesc (loc));

    show_weather();
    list_objects();
    list_people();

    if (ststflg (mynum, SFL_AUTOEXIT)) {
      bprintf ("\n");
      exitcom (False);
    }
  }
  bprintf ("\n");

  if (mynum < max_players && pfollow(mynum) != -1)
    bprintf(cur_player->cprompt);
}

char *showname (int loc) {
  int k;
  char n[64];
  static char b[66];

  k = findzone (loc, n);
  sprintf (b, "%s%d", n, k);
  return b;
}


/****************************************************************************** 
 * Find Location By Name
 *
 * Name is either <zonename/abbrev><offsett>    (for instance "cat44")
 *             or <absolute room number>        (for instance "-233")
 *             or <room>@<zone>                 (for instance "home@start")
 *
 * Return location, or 0 on error, or 1 if correct zonename (only) was given.
 ******************************************************************************/

int find_loc_by_name (char *name) {
  char buff[MAX_COM_LEN], *b = buff;
  int y, n;
  char *p;

  if ((p = strchr(name, '@'))) {
    *p = 0;
    if ((y = get_zone_by_name(p+1)) == -1) {
      bprintf("%s: no such zone.\n", p+1);
      return(0);
    }
    else if ((n = ht_lookup(locations_z, name, 0, y, room_search)) == -1) {   
      bprintf("%s: no such location.\n", name);
      return(0);
    }
    else
      return(n);
  } 

  if ((n = idtxt2int(name, LOC)) != -1)
    return(n);

  for (p = name, b = buff ; isalpha (*p) ; p++, b++)
    *b = *p;
  *b = 0;

  n = atoi (p);

  if (!*buff)
    return exists (n) ? n : 0;

  if ((y = get_zone_by_name (buff)) != -1)
    return n == 0 ? 1 : getlocid (y, n);
  else
    return 0;
}


/*
 * Basic function to find a room number from: 1) zonename<offset>,
 *                                        or  2) mobile/player
 *                                        or  3) object
 *                                        or  4) zonaname (no offset, assumes1)
 * Return room number, or 0 on error.
 */

int findroomnum (char *w) {
  int loc, a;

  while (*w != 0 && *w != '-' && !isalnum (*w))
    ++w;

  if ((loc = find_loc_by_name(w)) != -1) 
    return(loc);
  else if ((a = fmbn (w)) != -1) 
    return(ploc(a));
  else if ((a = fobn (w)) != -1)
    return(obj_loc(a));
  else if (loc == 1)
    return(getlocid(get_zone_by_name(w), 1));
  else
    return 0;
}

/* Same as findroomnum(), but use brkword() and wordbuf.
 */
int
getroomnum ()
{
  return (brkword () == -1) ? 0 : findroomnum (wordbuf);
}

Boolean
cango (int d)
{
  if (getexit (ploc (mynum), d) == 0)
    return False;
  if (getexit (ploc (mynum), d) < 0)
    return True;
  if (getexit (ploc (mynum), d) < DOOR)
    return False;
  if (state (getexit (ploc (mynum), d) - DOOR) == 0)
    return True;
  return False;
}


void reset_location (int loc) {
  int i, x;

  xlresetflgs(loc);
  laltitude(loc) = laltitude_reset(loc);

  for (i = 0; i < NEXITS; i++)
    if ((x = lexit_reset(loc, i)) && x < DOOR)
      setexit (loc, i, x);
    else
      lexit(loc, i) = x;
}

Boolean chkroom (int a, int plr) {

  if (ltstflg (a, LFL_WIZARD) && plev (plr) < LVL_WIZARD)
    return True;

  if (ltstflg (a, LFL_AWIZ) && plev (plr) < LVL_ARCHWIZARD)
    return True;

  if (ltstflg (a, LFL_SHALAFI) && plev (plr) < LVL_SHALAFI)
    return True;

  if (ltstflg (a, LFL_GOD) && plev (plr) < LVL_GOD)
    return True;

  if (ltstflg (a, LFL_NOINVIS) && pvis (plr) != 0)
    return True;

  if (ltstflg (a, LFL_OWNER) && !EQ (pname (plr), lowner (a)))
    return True;

  return False;
}

int check_light (int loc) {
  if (ltstflg (loc, LFL_LIGHT))
    return LFL_LIGHT;

  if (ltstflg (loc, LFL_DARK))
    return LFL_DARK;

  if (ltstflg (loc, LFL_L_REAL))
    return LFL_L_REAL;

  return LFL_LIGHT;
}

int check_temp (int loc) {
  if (ltstflg (loc, LFL_T_ORDINARY))
    return LFL_T_ORDINARY;

  if (ltstflg (loc, LFL_HOT))
    return LFL_HOT;

  if (ltstflg (loc, LFL_COLD))
    return LFL_COLD;

  if (ltstflg (loc, LFL_T_REAL))
    return LFL_T_REAL;

  return LFL_T_ORDINARY;
}
