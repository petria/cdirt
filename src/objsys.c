#include <stdlib.h>
#include "kernel.h"
#include "objects.h"
#include "mobiles.h"
#include "mobile.h"
#include "locations.h"
#include "quests.h"
#include "sendsys.h"
#include "levels.h"
#include "parse.h"
#include "objsys.h"
#include "rooms.h"
#include "objsys.h"
#include "bprintf.h"
#include "fight.h"
#include "commands.h"
#include "log.h"
#include "clone.h"

typedef struct {
  char *class_name;
  int oflag;
  short class_state;
} CLASS_DATA;

extern HASH_TABLE oalts_n[];
extern HASH_TABLE objects_n[];
extern HASH_TABLE ublock_n[];
extern HASH_TABLE ualts_n[];
extern HASH_TABLE locations_z[];

static CLASS_DATA *findclass (char *n);
static int value_class (CLASS_DATA * cl, int plx, Boolean silent);
static Boolean classmatch (int ob, CLASS_DATA * cl);
static void dropall (void);
static void getall (void);
static void getallfr (int);

static CLASS_DATA class_data[] = {
  {"clothing", OFL_WEARABLE, -2},
  {"weapons", OFL_WEAPON, -1},
  {"containers", OFL_CONTAINER, -1},
  {"food", OFL_FOOD, -1},
  {"keys", OFL_KEY, -1},
  {"all", -1, -1},
  {NULL, 0, -1}
};

void usecom (void) {
  int obj;

  if (brkword() == -1) {
    usercom();
    return;
  }

  if ((obj = cantake(ob1, "Use what?", "I don't know what that is.")) == -1)
    return;

#ifdef LOCMIN_PLAYHOUSE
  if (obj == OBJ_PLAYHOUSE_TOILET) {
    if (state (OBJ_PLAYHOUSE_TOILETLID) == 1) {
      bprintf ("You may want to lift the lid first.\n");
      return;
    }
    if (state (OBJ_PLAYHOUSE_TOILET) == 1) {
      bprintf ("&+GEwww!! &*Someone forgot to flush last time "
               "they used it!!\n");
      bprintf ("You decide &+Wnot &*to use the toilet. If only "
               "they had &+Cflushed&*.\n");
      return;
    } 
    else {
      if (psex (mynum) && !psitting (mynum)) {
        bprintf ("Shouldn't you sit down, ma'am?\n");
        return;
      }
      setobjstate (OBJ_PLAYHOUSE_TOILET, 1);
      bprintf ("Isn't that &+Cr&+Be&+Cf&+Br&+Ce&+Bs&+Ch&+Bi&+Cn&+Bg&+W? "
               "&+WDon't &*forget to &+Wflush!&*\n");
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
                "\001p%s\003 uses the toilet and looks more refreshed.\n",
                pname (mynum));
      broad ("A &+Wloud, &+Cr&+Be&+Cf&+Br&+Ce&+Bs&+Ch&+Bi&+Cn&+Bg "
             "&+Ytinkle &*is heard throughout the land.\n");
      return;
    }
  }
#endif
  bprintf ("You can't use that!\n");
}

int cantake (int ob, char *str1, char *str2) {
  if (ob != -1 && ocarrf(ob) == IN_CONTAINER)
    ob = oloc(ob);

  if (EMPTY (item1))
    bprintf("%s\n", str1);
  else if (ob == -1)
    bprintf("%s\n", str2);
  else if (oloc(ob) != ploc(mynum) && oloc(ob) != mynum)
    bprintf("It isn't here.\n");
  else if (plev(mynum) < LVL_WIZARD && otstbit(ob, OFL_DESTROYED))
    bprintf("%s\n", str2);
  else
    return(ob);
  return(-1);
}

int seeplr (char *str1, char *str2) {
  if (EMPTY (item1))
    bprintf("%s\n", str1);
  else if (pl1 == -1)
    bprintf("%s\n", str2);
  else if (pl1 < max_players && pvis(pl1) > plev(mynum))
    bprintf("%s\n", str2);
  else if (ploc(mynum) != ploc(pl1))
    bprintf("They aren't here.\n");
  else
    return(pl1);
  return(-1);
}

int seeplrhere (char *str1, char *str2) {
  int rslt = seeplr(str1, str2);
  if (rslt != -1 && ploc(mynum) != ploc(pl1)) {
    bprintf("They aren't here.\n");
    return(-1);
  } 
  return(rslt);
}

void lockcom (void) {
  if (cantake(ob1, "Lock what?", "I don't know what that is, sorry.") == -1)
    return;

  switch (ob1) {
  default:
    if (!otstbit (ob1, OFL_LOCKABLE))
      bprintf ("You can't lock that!\n");
    else if (state (ob1) > 1)
      bprintf ("It's already locked.\n");
    else if (!has_key(mynum))
      bprintf ("You have no key.\n");
    else {
      if (strstr(oname(ob1), "door"))
        bprintf("The door locks with a click.\n");
      else
        bprintf ("Locked.\n");
      setobjstate (ob1, 2);
    }
  }
}

void opencom(void) {
#ifdef LOCMIN_VOLCANO
  int plr;
#endif

  if (brkword() == -1) {
    bprintf("Open what?\n");
    return;
  }
  else if (cantake(ob1, "Open what?", 
            "I don't know how to open one of those.")== -1)
    return;

  switch (ob1) {

#ifdef LOCMIN_FOREST
  case OBJ_FOREST_TREEEATING:
  case OBJ_FOREST_INSIDETREE:
    bprintf("You can't shift the tree!\n");
    return;
#endif

#ifdef LOCMIN_VILLAGE
  case OBJ_VILLAGE_BOT_BOARDS:
  case OBJ_VILLAGE_TOP_BOARDS:
    if (state(ob1) > 0) {
      bprintf("You shift the floorboards, with much heaving and tugging, to "
              "reveal an exit\nbeyond.\n");
      setobjstate(OBJ_VILLAGE_TOP_BOARDS, 0);
    }
    else
      bprintf("The floorboards are already moved away.\n");
    return;
#endif

#ifdef LOCMIN_TOWER
  case OBJ_TOWER_DOOR_SHAZARETH:
    if (state(ob1) > 0)
      bprintf ("It seems to be magically closed.\n");
    return;
  case OBJ_TOWER_DOOR_TREASURE:
    bprintf ("You can't shift the door at all from this side.\n");
    return;
#endif

#ifdef LOCMIN_START
  case OBJ_START_UMBRELLA:
    if (state(ob1) > 0) {
      bprintf("You open the umbrella.\n");
      setobjstate(OBJ_START_UMBRELLA, 0);
    }
    else
      bprintf("The umbrella seems to already be opened.\n");
    break;
#endif

#ifdef LOCMIN_VOLCANO 
  case OBJ_VOLCANO_JAR:
    if (ploc(mynum) == LOC_VOLCANO_BANDILBAR39) {
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "%s opens the %s causing a large explosion which brings down part\n"
           "of the wall, revealing an exit to the north!\n",
          pname(mynum), oname(ob1));
      bprintf("As the phosphorus comes into contact with the air, it "
              "ignites with a violent\nexplosion which brings down part "
              "of the wall, revealing an exit to the north.\n" );
      broad("There is a loud rumbling in the distance as Mount Bandilbar "
            "begins to erupt!\n");
      setobjstate(OBJ_VOLCANO_BANDILBAR39NWALL, 0);
      lexit(LOC_VOLCANO_BANDILBAR23, 2) = LOC_VOLCANO_BANDILBAR25;
      lexit(LOC_VOLCANO_BANDILBAR28, 1) = LOC_VOLCANO_BANDILBAR27;
      for (plr = 0; plr < max_players; plr++) {
        if (ploc(plr) == LOC_VOLCANO_BANDILBAR24)
          setploc(plr, LOC_VOLCANO_BANDILBAR25);
        else if (ploc(plr) == LOC_VOLCANO_BANDILBAR26)
          setploc(plr, LOC_VOLCANO_BANDILBAR27);
      }
    }
    else {
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "%s opens the %s causing a large explosion!\n", pname(mynum),
           oname(ob1));
      bprintf("As the phosphorus comes into contact with the air, it "
              "ignites with a violent explosion!\n");
    }
    destroy(OBJ_VOLCANO_JAR);
    destroy(OBJ_VOLCANO_PHOSPHORUS);
    return;
#endif

  default:
    if (!otstbit (ob1, OFL_OPENABLE))
      bprintf("You can't open that!\n");
    else if (state (ob1) == 0)
      bprintf("It's already open.\n");
    else {
      if (state(ob1) == 2 && !has_key(mynum))
        bprintf ("It seems to be locked.\n");
      else {
        if (strstr(oname(ob1), "door"))
          bprintf("The door creaks open.\n");
        else
          bprintf ("Done.\n");
        setobjstate (ob1, 0);
      }
    }
    break;
  }
}

Boolean has_key(int plr) {
  int i, a;

  for (i = 0; i < lnumobs (ploc (plr)); i++) {
    a = lobj_nr (i, ploc (plr));
    if (p_ishere (plr, a) && otstbit(a, OFL_KEY))
      return True;
  }

  for (i = 0; i < pnumobs (plr); i++) {
    a = pobj_nr (i, plr);
    if (iscarrby (a, plr) && otstbit(a, OFL_KEY))
      return True;
  }
  return False;
}

void unlockcom (void) {
  if (cantake(ob1, "What do you want to unlock?",
                   "You can't unlock that.  I don't know what one is.") == -1)
    return;

  switch (ob1) {
  default:
    if (!otstbit (ob1, OFL_LOCKABLE))
      bprintf ("You can't unlock that!\n");
    else if (state(ob1) < 2)
      bprintf ("It's already unlocked.\n");
    else if (!hasobjtype(mynum, OFL_KEY))
      bprintf ("You have no key.\n");
    else {
      if (strstr(oname(ob1), "door"))
        bprintf("You lock the door with a firm twist of the key.\n");
      else
        bprintf("Ok.\n");
      setobjstate (ob1, 1);
    }
  }
}

void closecom (void) {
  if (brkword() == -1) {
    bprintf("Close what exactly?\n");
    return;
  }
  else if (cantake(ob1, "What do you want to close?",
           "You can't close that.  I don't know what that is") == -1)
    return;

  switch (ob1) {
  case OBJ_START_UMBRELLA:
    bprintf ("You close the umbrella.\n");
    setobjstate (OBJ_START_UMBRELLA, 0);
    break;
  default:
    if (!otstbit (ob1, OFL_OPENABLE))
      bprintf("You can't close that!\n");
    else if (state(ob1) > 0)
      bprintf("It's already closed.\n");
    else { 
      if (strstr(oname(ob1), "door"))
        bprintf("You close the door.\n");
      else
        bprintf("The %s is now closed.\n", oname(ob1));
      setobjstate(ob1, 1);
    }
  }
}

void givecom (void) {
  int plr, ob;

  if (brkword() == -1) {
    bprintf ("Give what to who?\n");
    return;
  }

  if (pl1 != -1)
    plr = pl1;
  else if (pl2 != -1)
    plr = pl2;
  else
    plr = -1;

  if (ob1 != -1)
    ob = ob1;
  else if (ob2 != -1)
    ob = ob2;
  else
    ob = -1;

  if (plr == -1 || pvis(plr) > plev(mynum)) {
    bprintf("Who's that?\n");
    return;
  }
  else {
    if (mynum == plr)
      bprintf ("Cheap skate!\n");

    else if (EMPTY(item2))
      bprintf ("Give them what?\n");

    else if (ob == -1)
      bprintf ("You don't have it.\n");
    else if (ob >= num_const_obs && ozone(ob) != -1) {
      if (ozone(ob) != pzone(plr))
        bprintf("You can't give that to them; it's a cloned object.\n");
      else {
        oloc_reset(ob) = plr;
        dogive(ob, plr);
      }
    }
    else
      dogive(ob, plr);
  }
}

void dogive (int ob, int pl) {
  int i, o, p;

  o = i = 0;
  p = pl - max_players;

  if (plev (mynum) < LVL_WIZARD && ploc (pl) != ploc (mynum)) {
    bprintf ("They aren't here.\n");
    return;
  }
  if (!iscarrby (ob, mynum)) {
    bprintf ("You don't have any %s.\n", oname (ob));
    return;
  }

#ifdef LOCMIN_NEWBIE
  if (otstbit(ob, OFL_NEWBIE) && 
     ((plev(pl) > 3 && plev(pl) < LVL_WIZARD) || (pl >= max_players))) {
    bprintf("Hey, they can't have that!!!  For new players only.\n");
    return;
  }
#endif

  if (!cancarry (pl)) {
    bprintf ("They can't carry it.\n");
    return;
  }
  if (pl >= max_players && mtstflg (pl, MFL_QFOOD) && otstbit (ob, OFL_FOOD)) {
    bprintf ("%s thanks you.\n", pname (pl));
    sendl(ploc (mynum), "%s has left the game.\n", pname (pl));
    setpscore (pl, pscore (pl) + 50);
    dumpstuff (pl, ploc (pl));
    strcpy (pname (pl), "");
    eat (ob);
    return;
  }

  if (iswornby(ob, mynum)) {
    removeobj(False, ob, mynum);
    bprintf("You remove the %s, and hand it to %s.\n", oname(ob), pname(pl));
  }
  else
    bprintf("You hand the %s to %s.\n", oname(ob), pname(pl));

  p = pl - max_players;

  #include "dogive.h"

  setoloc (ob, pl, CARRIED_BY);
  sendf(pl, "%s gives you the %s.\n", see_name (pl, mynum), oname (ob));
  send_msg(sendloc(pl), 0, LVL_MIN, LVL_MAX, pl, mynum,
	    "\001p%s\003 gives \001p%s\003 the %s.\n",
	    pname (mynum), pname (pl), oname (ob));
  return;
}

void stealcom (void) {
  int a, e, f;

  if (pclass(mynum) != THIEF) {
    bprintf ("You're not a thief.\n");
    return;
  }
  else if (plev(mynum) < 8) {
    bprintf ("You aren't up to it yet.\n");
    return;
  }
  else if (EMPTY(item1)) {
    bprintf ("Steal what?\n");
    return;
  }
  else if (EMPTY(item2)) {
    bprintf ("From who?\n");
    return;
  }
  else if (pl2 == -1 || pvis(pl2) > plev(mynum)) {
    bprintf ("Who is that?\n");
    return;
  }
  else if (mynum == pl2) {
    bprintf ("A true kleptomaniac.\n");
    return;
  }
  else if ((a = objinv(pl2, item1)) == -1) {
    bprintf ("They don't have it.\n");
    return;
  }
  else if (plev(mynum) < LVL_WIZARD && ploc(pl2) != ploc(mynum)) {
    bprintf ("They're not here!\n");
    return;
  }
  else if (ocarrf(a) == WORN_BY) {
    bprintf ("They're wearing it.\n");
    return;
  }
  else if (pwpn(pl2) == a) {
    bprintf ("They have it firmly to hand ... for KILLING people with!\n");
    return;
  }

  if (pl2 >= max_players) {
    if (mtstflg(pl2, MFL_NOSTEAL)) {
      sendl(ploc(pl2), "%s says 'How dare you try to steal from me, %s!'\n",
	    pname(pl2), pname (mynum));
      hit_player(pl2, mynum, -1);
      return;
    }
    else if (ptstflg(pl2, PFL_NOSTEAL)) {
      switch (randperc () % 3) {
      case 0:
        bprintf ("%s is too watchful.\n", he_or_she(pl2));
        return;
      case 1:
        bprintf ("%s is too alert.\n", he_or_she(pl2));
        return;
      case 2:
        bprintf ("%s is too crafty.\n", he_or_she(pl2));
        return;
      }
    }
  }  
  else {                                 /* victim is a player */
    if (ptstflg(pl2, PFL_NOSTEAL)) {
      bprintf("You couldn't possibly.\n");
      return;
    }
  }
  if (!cancarry (mynum)) {
    bprintf ("You can't carry any more.\n");
    return;
  }
  f = randperc ();
  e = (10 + plev (mynum) - plev(pl2)) * 5;
  if (f < e || plev (mynum) >= LVL_WIZARD) {
    bprintf ("Got it!\n");
    sendf(pl2, "%s steals the %s from you!\n", see_name(pl2, mynum), oname(a));
    setoloc (a, mynum, CARRIED_BY);
    if ((f & 1) && (pl2 >= max_players))
      hit_player(pl2, mynum, -1);
    return;
  }
  bprintf ("Your attempt fails.\n");
}


/*
 * Does this item stop magic spells from working on holder?
 */

Boolean is_antimagic( int obj ) {
  int antimagic[ ] = {
#ifdef LOCMIN_CATACOMB
    OBJ_CATACOMB_SHIELD,
#endif
#ifdef LOCMIN_TALON
    OBJ_TALON_TALONSHIELD,
#endif
#ifdef LOCMIN_CAVE
    OBJ_CAVE_DRAKESHIELD,
#endif
    -1
  };

  return int_is_on_table(antimagic, obj);
}

Boolean wears_antimagic (int pl) {
  int i;

  for (i = 0; i < pnumobs (pl); i++)
    if (iswornby (pobj_nr (i, pl), pl) && is_antimagic (pobj_nr (i, pl)))
      return True;

  return False;  
}

Boolean carries_boat (int pl) {
  int i;

  for (i = 0; i < pnumobs (pl); i++)
    if (iscarrby (pobj_nr (i, pl), pl) && otstbit(pobj_nr (i, pl), OFL_BOAT))
      return(True);

  return(False);
}

int carries_obj_type (int pl, int type) {
  int i;

  for (i = 0; i < pnumobs (pl); i++) {

    if (iscarrby (pobj_nr (i, pl), pl) && pobj_nr(i, pl) == type)
      return pobj_nr (i, pl);
  }
  return -1;
}

int wears_obj_type (int pl, int type) {
  int i;

  for (i = 0; i < pnumobs (pl); i++)
    if (iswornby (pobj_nr (i, pl), pl) && pobj_nr (i, pl) == type)
      return pobj_nr (i, pl);

  return -1;
}

static int value_class (CLASS_DATA * cl, int plx, Boolean silent) {
  int obj, sum = 0;

  for (obj = 0; obj < numobs; obj++) {

    if (in_inventory (obj, plx) && classmatch (obj, cl)) {
      sum += ovalue (obj);
      if (!silent) {
	if (plev (plx) >= LVL_WIZARD)
	  bprintf ("[%3d]", obj);
	bprintf ("%12.12s:%5d points\n", oname (obj), ovalue (obj));
      }
    }
  }
  return sum;
}

void valuecom (void) {
  CLASS_DATA *c;
  int a;

  if (brkword () == -1)
    bprintf ("Total value of all your possessions: %d points.\n",
	     value_class (findclass ("all"), mynum, True)); 
  else {
    do {
      if ((c = findclass (wordbuf)) != NULL) {
	bprintf ("\nTotal value:%*d points.\n",
		 plev (mynum) >= LVL_WIZARD ? 11 : 6,
		 value_class (c, mynum, False));
      } 
      else if ((a = fobn (wordbuf)) == -1)
	bprintf ("%s: no such object\n", wordbuf);
      else {
	if (plev (mynum) >= LVL_WIZARD)
	  bprintf ("[%3d]", a);
	bprintf ("%12.12s:%5d points\n", oname (a), ovalue (a));
      }
    }
    while (brkword () != -1);
  }
}

void putcom (void) {
  int a;
  char ar[128];
  int c, l;

  l = ar[0] = 0;  /* make compiler happier */
  a = ob1;
  c = ob2;

  #include "putcom.h"      

  if ((cantake(ob1, "Put what?", "What's that?")) == -1);

  else if (!has_arm(mynum) || !has_hand(mynum))
    bprintf ("With what exactly?\n");

  else if (cantake(ob2, "Put it where?", "What's that?") == -1);

  else if (ob1 == ob2)
    bprintf ("What do you think this is, the goon show?\n");

  else if (otstbit (ob2, OFL_CONTAINER) == 0)
    bprintf ("You can't put things in the %s.\n", oname(ob2));

  else if (state (ob2) != 0)
    bprintf ("It's not open.\n");

  else if (otstbit (ob1, OFL_NOGET))
    bprintf ("You can't take the %s!\n", oname(ob1));

  else if (otstbit (ob1, OFL_LIT))
    bprintf ("I'd try putting it out first!\n");

  else if (!willhold (ob2, ob1))
    bprintf ("It won't fit.\n");

  else {
    setoloc (a, c, IN_CONTAINER);
    bprintf ("Ok\n");

    send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
  	      "\001D%s\003\001c puts the %s in the %s.\n\003",
	      pname (mynum), oname (a), oname (c));

    if (otstbit (a, OFL_GETFLIPS))
      setobjstate (a, 0);

  #include "putcom2.h"
  }
}

void eatcom (void) {
  int b;
  char s[100];

  b = ob1 == -1 ? ob2 : ob1;

  if (brkword () == -1) {
    bprintf ("Eat what?\n");
    return;
  }
  if (EQ (wordbuf, "water"))
    strcpy (wordbuf, "spring");

  if (b == -1) {
    bprintf("What's that?\n");
    return;
  }
  else if (oloc(b) != mynum && oloc(b) != ploc(mynum)) {
    bprintf ("It isn't here.\n");
    return;
  }
  else if (otstbit(b, OFL_POTION)) {
    switch(randperc() % 3) {
      case 0:
        set_vital(mynum);
        bprintf("You feel your vitality returning.\n");
        break;
      case 1:
        setpstr(mynum, maxstrength(mynum));
	setpmagic(mynum, maxmagic(mynum));
        bprintf("You feel your health returning.\n");
        break;
      case 2:
        setpscore(mynum, pscore(mynum) + 3000);
        bprintf("You feel more experienced.\n");
        break;
      }
    calibme();
    destroy(b);
    return;
  }

  switch (b) {

  #include "eatcom.h"

  default:
    if (otstbit (b, OFL_FOOD)) {
      eat (b);
      bprintf ("Delicious!\n");

      setpstr (mynum, pstr (mynum) + 12);
      calibme ();
      sprintf (s, "\001P%s\003 greedily devours the %s.\n",
	       pname (mynum), oname (b));
      sillycom (s);
    } 
    else {
      bprintf ("I think I've lost my appetite.\n");
      return;
    }
    break;
  }
}

Boolean has_inventory(int plr) {
  int ct, i;

    for (ct = 0 ; ct < pnumobs(plr) ; ct++) {
      i = pobj_nr(ct, plr);
      if (!(iswornby(i, plr) || iswieldby(i, plr)))
        return(True);
    }
  return(False);
}

void inventory (void) {
  if (plev (mynum) < LVL_WIZARD) {
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	      "%s rummages through %s backpack.\n",
	      pname (mynum), his_or_her (mynum));
  }
  if (has_inventory(mynum)) {
    bprintf ("Your backpack contains:\n");
    aobjsat (mynum, CARRIED_BY, 0);
  }
  else
    bprintf("You are carrying nothing.\n");
}

void printpad(int nchars) {
  char pad[1024];
  int i;

  for (i = 0; i < nchars; i++)
    pad[i] = ' ';
  pad[i] = '\0';

  bprintf(pad);
}


/* All OBJectS AT - list all objects at the destination given. */

void aobjsat (int loc, int mode, int marg) {
  int obj, stp, ostack[64], ostackp = 0, ct;

  char b[100], line[200];

  int_set *inv = mode == IN_ROOM ? linv (loc) :
  mode == IN_CONTAINER ? oinv (loc) : pinv (loc);

  *line = 0;       /* line to assemble */

  for (obj = first_int(inv), ct = 0; ct < set_size(inv); 
    obj = int_number(++ct, inv)) {

    if (ovis (obj) > plev (mynum) || iswornby(obj, loc) ||
      iswieldby(obj, loc) || otstbit(obj, OFL_DESTROYED))
        continue;

    *b = 0;   /* buffer for current object */

    if ((mode == CARRIED_BY && iscarrby (obj, loc))
	|| (mode == IN_CONTAINER && iscontin (obj, loc))) {

      if (otstbit(obj, OFL_DESTROYED))
	strcat(b, "&+r(&N");
      if (otstbit(obj, OFL_FOOD))
        strcat(b, "&+G");

      strcat(b, oname(obj));

      if (otstbit(obj, OFL_DESTROYED)) 
	strcat(b, "&+r)&N");
      if (otstbit(obj, OFL_FOOD))
        strcat(b, "&N");

      strcat(b, " ");
      if (otstbit(obj, OFL_WEAPON))
        strcat(b, "<&+Mweapon&N> ");
      if (otstbit(obj, OFL_LIT))
        strcat(b, "<&+Ylit&N> ");
      if (ocarrf(obj) == WIELDED_BY)
        strcat(b, "<&+Rwielded&N> ");
      if (otstbit(obj, OFL_ARMOR))
        strcat(b, "<&+Carmor&N> ");
      if (otstbit(obj, OFL_CONTAINER)) {
        strcat(b, "<&+ycont&N> ");
	if (!otstbit(obj, OFL_OPENABLE) || state(obj) == 0)
	  ostack[ostackp++] = obj;
      }
    }

    if (strlen(line)+strlen(b)+marg-count_colors(line)-count_colors(b) > 79) {
      printpad(marg);
      bprintf("%s\n", line);
      *line = 0;
       ct--;
    } 
    else
      strcat(line, b);
  }

  if (*line) {
    printpad(marg);
    bprintf("%s\n", line);
  }

  for (stp = 0; stp < ostackp; stp++) {
    obj = ostack[stp];
    if (onumobs(obj) > 0) {
      printpad(marg + 4);
      bprintf("The %s contains:\n", oname (obj));
      aobjsat(obj, IN_CONTAINER, marg + 8);
    }
  }
}

/* Is o1 contained in o2 ? */

Boolean iscontin (int o1, int o2) {
  if (ocarrf (o1) != IN_CONTAINER || oloc (o1) != o2)
    return(False);

  if (plev (mynum) < LVL_WIZARD && otstbit (o1, OFL_DESTROYED))
    return(False);

  return(True);
}

/* The room where an object, or its container or its carrier, are at. */

int obj_loc (int obj) {
  while (ocarrf(obj) == IN_CONTAINER)
    obj = oloc(obj);

  return ocarrf (obj) >= CARRIED_BY ? ploc (oloc (obj)) : oloc (obj);
}

int game_objsearch(int a, int b) {
  if (ovis(a) <= plev(b) && 
      ((plev(b) > LVL_WIZARD) || !otstbit(a, OFL_DESTROYED)))
    return(True);
  else
    return(False);
}

Boolean eqobj (char *objname, int objnum, int *count, int plr, int realplr) {
  if (EQ(objname, oname(objnum)) || 
       (oaltname(objnum) && EQ(objname, oaltname(objnum)))) {
    if (ovis(objnum) <= plev(plr) && 
         ((plev(plr) > LVL_WIZARD) || !otstbit(objnum, OFL_DESTROYED))) {
      if (!*count) {
        if (wd_it(realplr))
          FREE(wd_it(realplr));
        wd_it(realplr) = COPY(oname(objnum));
        return(True);
      }
      else {
        (*count)--;
        return(False);
      }
    }
  }
  return(False);
}

/* recursively check containers for an object */

int chkcont(char *str, int ob, int *count, int plr, int realplr) {
  int ct, i, j;

  if (eqobj(str, ob, count, plr, realplr))
    return(ob);
  else if (onumobs(ob) > 0) {
    for (ct = 0 ; ct < onumobs(ob) ; ct++) {
      i = oobj_nr(ct, ob);
      if ((j = chkcont(str, i, count, plr, realplr)) != -1)
        return(j);
    }
  }
  return(-1);
}

/* find an object: ALL checks inv, then loc, then all objects */

int findob (int plr, char *txt, int type, int cont) {
  char word[MAX_COM_LEN];
  int i, ct, loc, count, ob, maxcount, realplr;

  if (plr != -1)
    loc = ploc(plr);
  else
    loc = 0;

  if ((realplr = aliased_mob(plr)) == -1)
    realplr = plr;

   if ((ob = idtxt2int(txt, OBJ)) != -1 && ovis(ob) <= plev(plr)) {
      if (type == INV && iscarrby(ob, plr))
        return(ob);
      else if (type == LOC && ishere(ob))
        return(ob);
      else if (type == CONT && iscontin(ob, cont))
        return(ob);
      else if (type == ALL)
        return(ob);
      else
        return(-1);
   }

  if (EQ(txt, "it") && wd_it(plr))
    strcpy(word, wd_it(plr));
  else
    strcpy(word, txt);

  maxcount = 0;
  for (i = 0 ; word[i] ; i++)
    if (isdigit(word[i])) {
      maxcount = atoi(&word[i]) - 1;
      word[i] = 0;
    }

  /* objs in inv */

  count = maxcount;
  if (type == INV || type == ALL) {
    for (ct = 0 ; ct < pnumobs(plr) ; ct++) {
      i = pobj_nr(ct, plr);
      if (eqobj(word, i, &count, plr, realplr) && iscarrby(i, plr))
        return(i);
      else if (onumobs(i)>0 && (ob=chkcont(word, i, &count,plr,realplr)) != -1)
        return(ob);
    }
  }

  /* objs in loc */

  count = maxcount;
  if (loc && (type == LOC || type == ALL)) 
    for (ct = 0 ; ct < lnumobs(loc) ; ct++) {
      i = lobj_nr(ct, loc);
      if (eqobj(word, i, &count, plr, realplr) && ishere(i))
        return(i);
    }

  /* objs in obj */

  count = maxcount;
  if (type == CONT)
    for (ct = 0 ; ct < onumobs(cont) ; ct++) {
      i = oobj_nr(ct, cont);
      if (eqobj(word, i, &count, plr, realplr) && iscontin(i, cont))
        return(i);
    }
  /* objs in game */

  if (type == ALL) {
    if ((ob = ht_lookup(objects_n, word, count, plr, game_objsearch)) == -1)
      if ((ob = ht_lookup(oalts_n, word, count, plr, game_objsearch)) == -1)
        return(-1);
    return(ob);
  }
  else
    return(-1);
}

/* get an object from a contiainer, or a room if container == -1 */
/* return -2 to terminate a getall, -1 to skip item, 0 on success */

int get1objfrom (int ob, int container) {
  int i, ct, l;
#ifdef LOCMIN_TOWER
  char *s;
#endif
  char bf[81];

  l = 0;    /* make compiler happy */

  for (ct = 0 ; ct < lnumchars(ploc(mynum)) ; ct++) {
    i = lmob_nr(ct, ploc(mynum));
    if (mtstflg(i, MFL_NOGRAB)) {
      bprintf("\001p%s\003 doesn't let you take it.\n", pname(i));
      return(-2);
    }
    if (mtstflg(i, MFL_GRABH)) {
      bprintf("\001p%s\003 says, you'll have to kill me first!\n", pname(i));
      hit_player(mynum, i, pwpn(i));
      return(-2);
    }
  }
  if (container != -1) {
    if (!iscontin(ob, container)) {
      bprintf("There's no such object in that container.\n");
      return(-1);
    }
    if (oloc(container) != ploc(mynum) && oloc(container) != mynum) {
      bprintf("You can't take things from that!  It's not here!\n");
      return(-2);
    }
    else if (otstbit(container, OFL_LOCKABLE) && 
              state(container) == 2 && !hasobjtype(mynum, OFL_KEY)) {
        bprintf("The %s is locked, and you have no key.\n", oname(container));
        return(-2);
    }
    else if (otstbit(container, OFL_OPENABLE) && state(container) == 1) {
      bprintf("You open the %s.\n", oname(container));
      setobjstate(container, 0);
    }
    else if (!cancarry (mynum)) {
      bprintf ("You can't carry any more.\n");
      return(-2);
    }
    else {
      setoloc(ob, mynum, CARRIED_BY);
      bprintf("You take the %s from the %s.\n", oname(ob), oname(container));
      return(0);
    }
  }
  else {
    if (!cancarry (mynum)) {
      bprintf("You can't carry any more.\n");
      return(-2);
    }
    else if (!has_arm(mynum) || !has_hand(mynum)) {
      bprintf("With what exactly?\n");
      return(-2);
    }  

    #include "getcom.h"

    setoloc(ob, mynum, CARRIED_BY);

    if (container == -1)
      *bf = 0;
    else
      sprintf (bf, " from the %s", oname (container));

    send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
	      "\001p%s\003 takes the %s%s.\n", pname (mynum), oname (ob), bf);

    if (otstbit (ob, OFL_GETFLIPS))
      setobjstate (ob, 0);

  #ifdef LOCMIN_TOWER
    if ((ploc(mynum) == LOC_TOWER_TREASURE) &&
        (state (OBJ_TOWER_DOOR_TREASURE) == 0)) {
      setobjstate (OBJ_TOWER_DOOR_TREASURE, 1);
      sendl (LOC_TOWER_TREASURE, s = "The door clicks shut...\n");
      sendl (obj_loc (olinked (OBJ_TOWER_DOOR_TREASURE)), s);
    }
  #endif

    bprintf("Ok\n");
  }
  return(0);
}

void getcom (void) {
  char buff1[80];
  char buff2[80];

  sprintf(buff1, "What's a %s?", item1);
  sprintf(buff2, "What's a %s?", item2);

  if (EMPTY(item1))
    bprintf("Get what?\n");
  else if (take_bodypart()); 
  else if (isdark())
    bprintf("It's dark!\n");
  else if (!has_hand(mynum))
    bprintf("How exactly? You don't have a hand to pick it up with.\n");
  else if (strcasestr(strbuf, "all from")) {
    if (cantake(ob2, "Get all from what?", buff2) != -1)
      getallfr(ob2);
  }
  else if (strcasestr(strbuf, "all")) {
    if (ob1 == -1 && ob2 == -1)
      getall();
    else if (cantake(ob2, "Get all what?", buff2) != -1)
      getallfr(ob2);
  }
  else if (strcasestr(strbuf, "from")) {
    if ((cantake(ob1, "Get from what?", buff1) != -1) &&
        (cantake(ob2, "Get from what?", buff2) != -1))
      get1objfrom(ob1, ob2);
  }
  else {         			                 /* get <obj> */
    if (cantake(ob1, "Take what?", buff1) == -1);
    else if (otstbit(ob1, OFL_NOGET) && plev(mynum) < LVL_WIZARD)
      bprintf("You cant take that, silly.");
    else
      get1objfrom(ob1, -1);
  }
}

static void getall (void) {
  int x, ct, loc, rslt;
  Boolean nothing = True;

  loc = ploc(mynum);

  for (ct = 0 ; ct < lnumobs(loc) ; ct++) {
    x = lobj_nr(ct, loc);
    if (ishere(x) && !oflannel(x)) {
      nothing = False;
      bprintf("%s: ", oname(x));
      if ((rslt = get1objfrom(x, -1)) == 0)
        ct--;
      else if (rslt == -2)
        return;
    }
  }
  
  if (nothing)
    bprintf("Nothing's here.\n");
}

/* see get1objfrom for result codes */

static void getallfr (int cont) {
  int ct, ob, rslt;
  Boolean empty = True;

  if (!ishere(cont) && !iscarrby(cont, mynum)) {
    bprintf("You cant get stuff from that; it's not here.\n");
    return;
  }    

  for (ct = 0 ; ct < onumobs(cont) ; ct++) {
    ob = oobj_nr(ct, cont);
    if (iscontin(ob, cont) && !oflannel(ob)) {
      bprintf ("%s: ", oname(ob));
      empty = False;
      
      if ((rslt = get1objfrom(ob, cont)) == 0)
        ct--;
      else if (rslt == -2)
        return;
    }
  }

  if (empty)
    bprintf("It's empty.\n");
}

static void dropall(void) {
  int ob, ct;
  Boolean nothing = True;

  for (ct = 0 ; ct < pnumobs(mynum) ; ct++) {
    ob = pobj_nr(ct, mynum);
    if (iscarrby(ob, mynum)) {
      nothing = False;
      bprintf ("%s: ", oname (ob));
      dropobjcom(ob);
      ct--;
    }
  }
  
  if (nothing)
    bprintf("You have nothing to drop.\n");
}

Boolean p_ishere (int plr, int item) {
  if (plev (plr) < LVL_WIZARD && 
    (plev(plr) < ovis(item) || (otstbit (item, OFL_DESTROYED))))
    return(False);

  if (ocarrf(item) != IN_ROOM || oloc(item) != ploc(plr) || !ploc(plr))
    return(False);

  return(True);
}

Boolean iscarrby (int item, int user) {
  if ((item == -1) || 
      (plev(user) < LVL_WIZARD && otstbit(item, OFL_DESTROYED)) ||
      (ocarrf(item) < CARRIED_BY) ||
      (oloc(item) != user))
    return(False);
  else
    return(True);
}


/* 
 * Is the object in a players inventory ?
 * (also handles objects in a container in a container etc...)
 */

Boolean in_inventory (int obj, int player) {
  while (ocarrf (obj) == IN_CONTAINER)
    obj = oloc (obj);

  return(iscarrby (obj, player));
}


void dropobjcom(int a) {
  int l;

  if (a == -1) {                         /* user typed drop <args> */
    if (strcasestr(strbuf, "drop all")) {
      dropall();
      return;
    }
    else if ((a = cantake(ob1, "Drop what?", "What's that?")) == -1)
      return;
    else if (!has_hand(mynum) || !(has_arm(mynum))) {
      bprintf("How? You are missing hands!\n");
      return;
    }
    else if (a >= num_const_obs && ozone(a) != -1 &&
      EQ(zname(ozone(a)), pname(mynum)) && lzone(ploc(mynum)) != ozone(a)) {
        bprintf("You can't drop that here; it's a cloned object.\n");
        return;
    }
  }

  if (iswornby(a, mynum)) {
    bprintf("You take off the %s, and drop it here.\n", oname(a));
    removeobj(False, a, mynum);
  }

  l = ploc(mynum);

#include "dropcom.h"

#ifdef LOCMIN_SEA
  if (ltstflg (l, LFL_ON_WATER) && !otstbit(a, OFL_BOAT)) {
    bprintf ("The %s sinks into the sea.\n", oname (a));
    l = LOC_SEA_7;
  }
#endif

#ifdef LOCMIN_START
  if (ploc(mynum) == oloc(OBJ_START_PIT) || 
      ploc(mynum) == oloc(OBJ_START_CHURCH_PIT) 
#ifdef LOCMIN_SEA
    || (l == oloc(OBJ_SEA_HOLE))
#endif 
    ) {
    bprintf("The %s disappears into the bottomless pit.....\n", oname(a));
    send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
	     "\001p%s\003 drops the %s into the pit.\n",
             pname (mynum), oname (a));
    dropinpit(a);
    return;
  } 
#endif

  setoloc (a, l, IN_ROOM);
  send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
    "\001p%s\003 drops the %s.\n", pname (mynum), oname (a));

  bprintf ("Ok\n");
}

void dropinpit (int o) {
  int i, ct;

  if (o == OBJ_START_PIT || o == OBJ_START_CHURCH_PIT) {
    bprintf("Drop the pit into the pit? Weirdo.\n");
    return;
  }
  if (o < num_const_obs) {
    setpscore (mynum, pscore (mynum) + ovalue (o));
    calib_player (mynum);
  }

  osetbit (o, OFL_DESTROYED);
  setoloc (o, LOC_PIT_PIT, IN_ROOM);

#include "dropinpit.h"

  if (otstbit (o, OFL_CONTAINER)) {
    for (ct = 0 ; ct < onumobs(o) ; ct++) {
      i = oobj_nr(ct, o);
      if (iscontin (i, o))
	dropinpit (i);
    }
  }
}


/* List the objects at the current players location. */
/* Note: also sets wd_it to the last object in the room */

void list_objects (void) {
  int i, a;

  for (i = 0; i < lnumobs (ploc (mynum)); i++) {

    a = lobj_nr (i, ploc (mynum));

    if (ishere (a)) {
      if (state (a) > 3)
	continue;
      if (olongt(a, state (a))) {
	if (otstbit (a, OFL_DESTROYED))
	  bprintf ("--");
	oplong (a);
        if (a != OBJ_START_PIT && a != OBJ_START_CHURCH_PIT) {
          if (wd_it(real_mynum))
            FREE(wd_it(real_mynum));
          wd_it(real_mynum) = COPY(oname(a));
        }
      } 
      else if (plev (mynum) >= LVL_ARCHWIZARD)
	bprintf ("<marker>%s\n", oname (a));
    }
  }
}

void dumpstuff (int n, int loc) {
  int b, ct;

  for (ct = 0 ; ct < pnumobs(n) ; ct++) {
    b = pobj_nr(ct, n);
    if (iscarrby (b, n)) {
      if (loc == LOC_PIT_PIT)
	dropinpit (b);
      else
	setoloc (b, loc, IN_ROOM);
      ct--;
    }
  }
}

void oplong (int x) {
  char *t = olongt (x, state (x));

  if (t)
    bprintf ("%s\n", t);
}


Boolean gotanything (int x) {
  int ct, i;
  
  for (ct = 0 ; ct < pnumobs(x) ; ct++) {
    i = pobj_nr(ct, x);
    if (iscarrby (i, x) && !iswieldby(i, x) && !iswornby(i, x)) 
      return(True);
  }
  return(False);
}

static CLASS_DATA *findclass (char *n) {
  CLASS_DATA *cl;

  for (cl = class_data; cl->class_name != NULL; cl++)
    if (EQ (cl->class_name, n))
      return(cl);

  return(NULL);
}

static Boolean classmatch (int ob, CLASS_DATA * cl) {
  register short st;

  return (cl == NULL || (((st = cl->class_state) < 0 || st == state (ob)) && 
                         (cl->oflag == -1 || otstbit(ob, cl->oflag))));
}

/* Can carry any more objects now ? */

Boolean cancarry (int plyr) {
  int i, a;
  int num = 0;

  if (plev (plyr) >= LVL_WIZARD || plyr >= max_players)
    return True;

  for (i = 0; i < pnumobs (plyr); i++) {
    a = pobj_nr (i, plyr);
    if (iscarrby (a, plyr) && !iswornby (a, plyr))
      num++;
  }

  return num < plev (plyr) + 5;
}

/* Try to reset an object, return True on success. */

Boolean reset_object (int o) {
  int loc = 0;

  osetbaseval (o, ovalue_reset (o));
  osetsize (o, osize_reset (o));
  osetvis (o, ovis_reset (o));
  osetdamage (o, odamage_reset (o));
  osetarmor (o, oarmor_reset (o));
  state(o) = state_reset(o);
  oresetbits(o);
  aresetbits(o);
  oweight(o) = oweight_reset(o);

  loc = oloc_reset (o);
  setoloc (o, loc, ocarrf_reset (o));
  return(True);
}

void setobjstate (int obj, int state) {
  if (state >= 0 && state <= omaxstate (obj)
      && (olinked (obj) == -1 || state <= omaxstate (olinked (obj)))) {

    state (obj) = state;
    if (olinked (obj) != -1) 
      state (olinked (obj)) = state;
  } 
  else
    mudlog ("ERROR: Attempt to set object %s[%d] to state %d",
	    oname (obj), int2idx(obj, OBJ), state);
}

void destroy (int ob) {
  osetbit (ob, OFL_DESTROYED);
  setoloc (ob, LOC_DEAD_DESTROYED, IN_ROOM);
}

void eat (int ob) {
  osetbit (ob, OFL_DESTROYED);
  setoloc (ob, LOC_DEAD_EATEN, IN_ROOM);
}

/* SET Object LOCation */

void setoloc (int obj, int loc, int c) {

  /* The object is already in the right place/status */
  if (oloc(obj) == loc && ocarrf(obj) == c && ocarrf(obj) < WIELDED_BY)
    return;

  /* First remove the object from wherever it is: */
  switch (ocarrf (obj)) {
  case IN_ROOM:
    if (exists (oloc (obj)))
      remove_int (obj, linv (oloc (obj)));
    break;
  case IN_CONTAINER:
    if (oloc (obj) >= 0 && oloc (obj) < numobs)
      remove_int (obj, oinv (oloc (obj)));
    break;
  case CARRIED_BY:
  case WORN_BY:
  case WIELDED_BY:
  case BOTH_BY:
    if (oloc (obj) >= 0 && oloc (obj) < numchars)
      remove_int (obj, pinv (oloc (obj)));
    break;
  }

  /* Then add it to the right place: */
  switch (c) {
  case IN_ROOM:
    if (exists (loc))
      add_int (obj, linv (loc));
    break;
  case IN_CONTAINER:
    if (loc >= 0 && loc < numobs)
      add_int (obj, oinv (loc));
    break;
  case CARRIED_BY:
  case WORN_BY:
  case WIELDED_BY:
  case BOTH_BY:
    if (loc >= 0 && loc < numchars)
      add_int (obj, pinv (loc));
    break;
  }

  if (c >= WIELDED_BY)
    set_weapon(loc, obj);
  if (c == WORN_BY || c == BOTH_BY)
    wear_item(obj, loc);

  oloc (obj) = loc;   /* needs to come after, because set_weapon & */
  ocarrf (obj) = c;   /* wear_item will mess up BOTH_BY            */
}


/* do we have an object which has the flag passed or is it on the ground */

Boolean hasobjtype(int plr, int flag) {
  int i, a;

  for (i = 0; i < lnumobs (ploc (plr)); i++) {
    a = lobj_nr (i, ploc (plr));
    if (p_ishere (plr, a) && (otstbit(a, flag)))
      return(True);
  }

  for (i = 0; i < pnumobs (plr); i++) {
    a = pobj_nr (i, plr);
    if (iscarrby (a, plr) && otstbit(a, flag))
      return(True);
  }
  return(False);
}

int ovalue (int ob) {
  #ifdef USE_TSCALE
    return (tscale () * obaseval (ob) / 9);
  #else
    return obaseval (ob);
  #endif
}

char *xdesloc (char *b, int loc, int cf) {
  char v[256];
  char buff[256];

  *buff = '\0';

  while (cf == IN_CONTAINER) {
    sprintf (v, "&+In the %s ", oname (loc));
    strcat (buff, v);
    cf = ocarrf (loc);
    loc = oloc (loc);
  }

  if (cf >= CARRIED_BY) {
    if (cf == CARRIED_BY)
      strcat (buff, "Carried");
    if (cf == WORN_BY)
      strcat (buff, "Worn");
    if (cf == WIELDED_BY)
      strcat (buff, "Wielded");
    if (cf == BOTH_BY)
      strcat (buff, "Worn & Wielded");
    sprintf (v, " by %s ", see_name (mynum, loc));
    strcat (buff, v);
    loc = ploc (loc);
  }
  if (!exists (loc)) {
    if (plev (mynum) < LVL_GOD)
      return strcpy (b, "Out in the void");
    else {
      sprintf (b, "NOT IN UNIVERSE[%d]", int2idx(loc, LOC));
      return b;
    }
  }
  if (*buff != '\0')
    strcat (buff, "in ");

  if (plev (mynum) >= LVL_WIZARD)
    sprintf (v, "| %s", showname (loc));
  else
    *v = '\0';

  strcat (buff, sdesc (loc));
  sprintf (b, "%-34.34s%s", buff, v);

  return b;
}

void desloc (int loc, int cf) {
  char b[512];

  bprintf ("%s\n", xdesloc (b, loc, cf));
}

/* Will the container x hold object y ? */

Boolean willhold (int x, int y) {
  int i, a, sum = 0;

  for (i = 0; i < onumobs (x); i++) {
    a = oobj_nr (i, x);
    if (iscontin (a, x))
      sum += osize (a);
  }

  sum += osize (y);
  return sum <= osize (x);
}

void drop_some_objects (int plx) {
  int obj, ct;

  for (ct = 0 ; ct < pnumobs(plx) ; ct++) {
    obj = pobj_nr(ct, plx);

    if (ocarrf (obj) == CARRIED_BY && oloc (obj) == plx)
      setoloc (obj, ploc (plx), IN_ROOM);
  }
}

char *xdesrm (char *b, int loc, int cf) {
  char v[30];

  if (plev (mynum) < LVL_WIZARD && cf == IN_ROOM && loc == LOC_LIMBO_LIMBO) {
    return strcpy (b, "Somewhere.....");
  }
  if (cf == IN_CONTAINER) {
    sprintf (b, "In the %s", oname (loc));
    return b;
  }
  if (cf >= CARRIED_BY) {
    if (!seeplayer (loc))
      return strcpy (b, "Somewhere.....");
    else {
      sprintf (b, "Carried by %s", pname (loc));
      return b;
    }
  }
  if (!exists (loc)) {
    if (plev (mynum) < LVL_ARCHWIZARD)
      return strcpy (b, "Out in the void");
    else {
      sprintf (b, "NOT IN UNIVERSE[%d]", int2idx(loc, LOC));
      return b;
    }
  }
  if (plev (mynum) >= LVL_WIZARD)
    sprintf (v, "| %s", showname (loc));
  else
    *v = 0;

  sprintf (b, "%-30s%s", sdesc (loc), v);
  return b;
}


void desrm (int loc, int cf) {
  char b[80];

  bprintf ("%s\n", xdesrm (b, loc, cf));
}

char *odescrm (int obj) {
  static char b[300];

  if (ocarrf (obj) == IN_CONTAINER)
    odescrm (oloc (obj));
  else {
    if (ocarrf (obj) >= CARRIED_BY) {
      sprintf (b, "%s", showname (ploc (oloc (obj))));
      return b;
    }
    if (!exists (oloc (obj))) {
      sprintf (b, "Unknown (%d)", oloc (obj));
      return b;
    }
    sprintf (b, "%s", showname (oloc (obj)));
    return b;
  }
  return NULL;
}

void move_pouncie (void) {
  Boolean ok = 0;
  int room;

  while (!ok) {
    room = my_random() % (num_const_locs - 1);

    if (
#ifdef LOCMIN_CASTLE
          room != LOC_CASTLE_TORTURE &&
          room != LOC_CASTLE_MAIDEN &&
#endif
          !ltstflg (room, LFL_DEATH) &&
	  !ltstflg (room, LFL_ON_WATER) &&
	  !ltstflg (room, LFL_ONE_PERSON) &&
	  !ltstflg (room, LFL_PRIVATE))
      ok = True;
  }

  setoloc (OBJ_LIMBO_POUNCIE, room, IN_ROOM);
}

void wearall (void) {
  int obj;
  Boolean worn = False;
  Boolean twiddle = False;

  for (obj = 0; obj < numobs; obj++) {
    if (iscarrby (obj, mynum) && otstbit (obj, OFL_WEARABLE)) {
      if (ocarrf(obj) != CARRIED_BY)
        twiddle = True;
      else {
        wearcom(mynum, obj);
        worn = True;
      }
    }
  }

  if (twiddle)
    bprintf ("You're already wearing everything you can.\n");
  else if (!worn)
    bprintf ("You have nothing to wear.\n");
}

