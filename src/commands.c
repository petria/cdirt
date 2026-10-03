#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include "kernel.h"
#include "locations.h"
#include "objects.h"
#include "mobiles.h"
#include "group.h"
#include "quests.h"
#include "sendsys.h"
#include "levels.h"
#include "commands.h"
#include "rooms.h"
#include "objsys.h"
#include "mobile.h"
#include "bprintf.h"
#include "parse.h"
#include "uaf.h"
#include "clone.h"
#include "acct.h"
#include "mud.h"
#include "fight.h"
#include "zones.h"
#include "log.h"
#include "wizlist.h"
#include "climate.h"
#include "mail.h"
#include "mud.h"
#include "timing.h"
#include "mobdesc.h"
#include "xrscore.h"
#include "files.h"
#include "move.h"
#include "update.h"

extern char *scorpse[];
extern void wipe_duration(int);
static Boolean find_stuff (int s, int o, char *t);
int help (int, long int[], char *);

#define SECS_IN_A_MIN 60

void flushcom(void) {
  int obj;

  if ((obj = ob1) == -1) {
    bprintf ("That isn't here.\n");
    return;
  }
#ifdef LOCMIN_PLAYHOUSE
  if (obj == OBJ_PLAYHOUSE_TOILET) {
    if (plev (mynum) < LVL_ENCHANTER)
      setpscore (mynum, pscore (mynum) + 10);

    setobjstate (OBJ_PLAYHOUSE_TOILET, 0);
    bprintf ("You flush the toilet. Wow! Look at the pretty &+Bblue &*water!\n");
    broad ("You hear a &+WPorcelain God &*gargling in the distance.\n");
    return;
  }
#endif
  bprintf ("You can't flush that!\n");
}

void awaymsgcom(void) {
  char txt[MAX_COM_LEN];

  getreinput (txt);

  if( EMPTY(txt) ) {
    bprintf( "Away message removed\n" );
    FREE( pawaymsg(mynum));
    return;
  }

  FREE( pawaymsg(mynum) );
  pawaymsg(mynum) = COPY( txt );
  bprintf( "Away message set to: %s\n", txt );
  if (check_away (mynum)) {
        send_msg (DEST_ALL, 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
  		"&+G[&+W%s &*has changed %s away message:&+G]\n&+G[&N%s&+G]\n",
  		pname (mynum), his_or_her (mynum), pawaymsg(mynum));
  }
  return;
}

void bashcom(void) {

}

void backstabcom(void) {

  if (pclass(mynum) != THIEF)
    bprintf("For thiefs only, sorry.\n");
  else if (plev(mynum) < 4)
    bprintf("You don't have enough experience to pull it off.\n");
  else if (pfighting(mynum) != -1)
    bprintf("How can you sneak up? You're already fighting!\n");
  else {
    if (seeplrhere("Backstab who?", "I don't know who that is.") == -1);
    else if (ltstflg(ploc(mynum), LFL_PEACEFUL))
      bprintf("Sorry, it's too peaceful here.\n");
    else if (pl1 == mynum)
      bprintf("Nhilistic!\n");
    else if (pstr(pl1) < 0)
      bprintf("They are already dead.\n");
    else {
      if (randperc() + plev(mynum) * 2 > 50) {
        bprintf("You plummet your hidden knife deep into %s's back.\n",
          pname(pl1));
        setpstr(pl1, pstr(pl1) - 20 + plev(mynum));
      }
      if (pl1 > max_players) {
        setpangry(pl1, mynum);
        setpangry(mynum, pl1);
      }
      hit_player(pl1, mynum, pwpn(mynum));
    }
  }
}

void pickcom(void) {
  if (pclass(mynum) != THIEF)
    bprintf("You can't pick locks... you're not a thief!\n");
  else if (plev(mynum) < 6)
    bprintf("You're not quite up to this yet.\n");
  else if (oloc(ob1) != ploc(mynum))
    bprintf("It's not here.\n");
  else if (ob1 == -1)
    bprintf("You can't pick that!\n");
  else if (state (ob1) == 2)
    bprintf ("It's already unlocked.\n");
  else {
    if (randperc() + plev(mynum) * 4 > 84) {
      bprintf("The door flies open!\n");
      setobjstate (ob1, 0);  
    }
    else
      bprintf("Your attempt fails.\n");
  }
}

void sneakcom(void) {
  if (pclass(mynum) != THIEF)
    bprintf("You're not quiet enough! Thiefs only!\n");
  else if (!has_foot(mynum))
    bprintf("You can't sneak too well without feet.\n");
  else if (plev(mynum) < 10)
    bprintf("You can't sneak yet.\n");
  else if (!*item1)
    bprintf("Sneak where exactly?\n");
  else {
    if (randperc() + plev(mynum) * 4 > 84) {
      bprintf("You sneak!\n");
      dogocom(True);
    }
    else {
      bprintf("You don't feel too sneaky...\n");
      dogocom(False);
    }
  }
}

void fillcom(void) {
  bprintf( "Not implemented yet.\n" );
  return;
}

void rubcom(void) {
  int a;
  
  if(!item1[0]) {
    bprintf("What do you want to rub!?!\n");
    return;
  }

  if ((a = fmbn(item1)) != -1) { 
    if (a == mynum) {
      bprintf("You rub yourself!\n");
      return;
    }

    if (ploc(a) != ploc(mynum) && plev(mynum) < LVL_WIZARD) {
      bprintf("They aren't here.\n");
      return;
    }

    bprintf("You give %s a nice soothing back massage!\n", pname(a));
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, a,
	    "%s gives %s a nice soothing back massage!\n", 
	    pname(mynum), pname(a)); 
    sendf(a, "%s gives you a nice soothing back massage!\n", pname(mynum));
    return;
  }

  if ((a = ob1) == -1) {
    bprintf("What on %s is a %s?!?\n", MUD_NAME, item1);
    return;
  }

  bprintf("You vigorously rub the %s and...\n", oname(a));
  send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
            "%s vigorously rubs the %s.\n", pname (mynum), oname (a));

  /* wiz+ can rub to light burnable objects. :) */
  if( plev(mynum) >= LVL_WIZARD && otstbit(a, OFL_LIGHTABLE) && 
      !otstbit(a, OFL_LIT)) {
      bprintf("It bursts into &+Yflames&N!\n");
      sillycom("And it bursts into &+Yflames&N!\n");
      setobjstate(a, 0);
      osetbit(a, OFL_LIT);
      return;
  }

  switch(a) {
   default:  
     bprintf("Nothing happens!\n");
     break;
  }
  return;
}

void lookcom (void) {
  int a;
  
  if (brkword () == -1)
    lookin (ploc (mynum), SHOW_LONG);
  else {
    if (EQ (wordbuf, "at"))
      examcom();

    else if (!EQ (wordbuf, "in") && !EQ (wordbuf, "into"))
      bprintf ("I don't understand.  Are you trying to LOOK, LOOK AT, "
	       "or LOOK IN something?\n");

    else if (EMPTY (item1))
      bprintf ("In what?\n");

    else if ((a = ob1) == -1)
      bprintf ("What's that?\n");

    else if (0 
#ifdef LOCMIN_BLIZZARD
              || a == OBJ_BLIZZARD_TUBE 
#endif
#ifdef LOCMIN_TREEHOUSE
	      || a == OBJ_TREEHOUSE_BEDDING
#endif
#ifdef LOCMIN_ORCHOLD
	      || a == OBJ_ORCHOLD_TOPCOVER
#endif
 ) {
      strcpy (item1, item2);
      ob1 = ob2;
      examcom ();
    } 

    else if (!otstbit (a, OFL_CONTAINER))
      bprintf ("That isn't a container.\n");

    else if (otstbit (a, OFL_OPENABLE) && state (a) != 0)
      bprintf ("It's closed!\n");

    else {
      bprintf ("The %s contains:\n", oname (a));
      aobjsat (a, IN_CONTAINER, 8);
    }
  }
}

static Boolean
find_stuff (int s, int o, char *t)
{
  if (odamage (s) == 0) {
    osetdamage (s, 1);
    bprintf (t);
    create (o);
    setoloc (o, ploc (mynum), IN_ROOM);
    return True;
  }
  return False;
}

void examcom (void) {
  int a, tabnum;
#if defined(LOCMIN_TOWER) || defined(LOCMIN_BLIZZARD)
  int foo, z;
#endif
  FILE *x;
  char *t;
  char text[80];

  if (!item1[0]) {
    bprintf ("Examine what?\n");
    return;
  }
  if ((a = pl1) != -1 && ploc (a) == ploc (mynum)) {
    if (pstr (a) < 1) {
      time_t cur_time;
      int t_elapsed, p;
      char buff[100];

      time(&cur_time);
      t_elapsed = cur_time - ublock[a - max_players].death_time;

      if (t_elapsed < 2 * SECS_IN_A_MIN) p = 0;
      else if (t_elapsed < 5 * SECS_IN_A_MIN) p = 1;
      else if (t_elapsed < 8 * SECS_IN_A_MIN) p = 2;
      else if (t_elapsed < 11 * SECS_IN_A_MIN) p = 3;
      else if (t_elapsed < 15 * SECS_IN_A_MIN) p = 4;
      else if (t_elapsed < 20 * SECS_IN_A_MIN) p = 5;
      else return;
      sprintf(buff, "&+W%s\n", scorpse[p]);
      bprintf(buff, pname(a)); 
      return;
    }
    if (a >= max_players && (t = pexam(a)) != NULL) 
      bprintf ("%s\n", t);
    else {
      if (a != mynum)
	sendf (a, "%s examines you closely.\n", pname (mynum));

      sprintf (text, "%s/%s", DESC_DIR, pname (a));
      if ((x = FOPEN(text, "r")) != NULL) {
	FCLOSE(x);
	bprintf ("\001f%s\003", text);
      }
    }
    if (a >= max_players) {
      for(tabnum = 0; tabnum < 5; tabnum++) {
        if(pstr_reset(a) + 25 * pdam(a) < MobRanges[tabnum])
          break;
      }
      pmess(a, False);
      bprintf(MobDesc[tabnum], he_or_she(a));
      if (pwpn(a) != -1)
        bprintf("&+Wo&+y %s is wielding a weapon\n", he_or_she(a));
      if (pac(a) > 0)
        bprintf("&+Wo&+y %s is wearing some armor\n", he_or_she(a));
    }
    return;
  }
  if ((a = ob1) == -1) {
    bprintf("You see nothing special.\n");
    return;
  }
  else if (oloc(a) != ploc(mynum) && oloc(a) != mynum) {
    bprintf("The %s is not here.\n", oname(a));
    return;
  }
  send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	    "%s examines the %s closely.\n", pname (mynum), oname (a));


  /* Checks the cloned object */

#ifdef LOCMIN_BLIZZARD
  if (strncasecmp(ozname(a), "scroll", 6) && EQ(zname(ozone(a)), "blizzard")) {
#ifdef LOCMIN_CATACOMB
    if (iscarrby (OBJ_CATACOMB_CUPSERAPH, mynum)) {
      bprintf ("Funny, I thought this was a teleport scroll, but "
	       "nothing happened.\n");
      return;
    }
#endif
    bprintf ("As you read the scroll you are teleported!\n");
    destroy (a);
    teletrap (LOC_BLIZZARD_PENTACLE);
    return;
  }
#endif
  switch (a) {

#include "examcom.h"

#ifdef LOCMIN_ANCIENT
  case OBJ_ANCIENT_SKYMAP:
    if (ploc (mynum) == LOC_ANCIENT_ANC53) {
      bprintf ("You feel like you are flying....\n"
	       "     ....and land on a desolate planet,"
	       "obviously far from the sun.\n\n\n");
      trapch (LOC_ANCIENT_ANC56);
    } 
    else
      bprintf ("You ain't lost, Mac... yet!\n");
    break;

  case OBJ_ANCIENT_LBOOK:
    if (oarmor (a) > 0)
      bprintf ("The Book contains old writings. You can make out\n"
	       "the names of old mages like 'Elmo' and 'Infidel'.\n");
    else {
      bprintf ("Ancient wisdom fills your head, you feel much more capable...\n");
      osetarmor (a, 1);
    }
    break;
#endif

#ifdef LOCMIN_EFOREST
  case OBJ_EFOREST_TREE:
    if (odamage (a) == 0) {
      osetdamage (a, 1);
      bprintf ("You find a door in the tree.\n");
      setobjstate (a, 0);
      return;
    }
    break;
  case OBJ_EFOREST_DESK:
    if (find_stuff (OBJ_EFOREST_DESK, OBJ_EFOREST_BUTTON,
		    "You find a button hidden in a recess of the desk.\n"))
      return;
    break;
  case OBJ_EFOREST_THRONE_WOOD:
    if (find_stuff (OBJ_EFOREST_THRONE_WOOD, OBJ_EFOREST_CROWN,
		    "You find a crown hidden under the throne.\n"))
      return;
    break;
  case OBJ_EFOREST_COAT:
    if (find_stuff (OBJ_EFOREST_COAT, OBJ_EFOREST_COIN,
		    "You find a coin in the pocket.\n"))
      return;
    break;
  case OBJ_EFOREST_TABLE:
    if (find_stuff (OBJ_EFOREST_TABLE, OBJ_EFOREST_GAUNTLETS,
		    "You find some gauntlets admidst the rubble "
		    "of this table.\n"))	/* ; */
      return;
    break;
  case OBJ_EFOREST_PAINTING:
    if (find_stuff (OBJ_EFOREST_PAINTING, OBJ_EFOREST_NOTE,
		    "There was a note behind the painting.\n"))
      return;
    break;
  case OBJ_EFOREST_CHAIR:
    if (find_stuff (OBJ_EFOREST_CHAIR, OBJ_EFOREST_RING,
		    "A ring was between the cushions of that chair!\n"))
      return;
    break;
  case OBJ_EFOREST_RACK:
    if (find_stuff (OBJ_EFOREST_RACK, OBJ_EFOREST_SCARAB,
		    "You found a scarab in the rack.\n"))
      return;
    break;
  case OBJ_EFOREST_PAPERS:
    if (find_stuff (OBJ_EFOREST_PAPERS, OBJ_EFOREST_TREATY,
		    "Among the papers, you find a treaty.\n"))
      return;
    break;
  case OBJ_EFOREST_DESK_LICH:
    if (find_stuff (OBJ_EFOREST_DESK_LICH, OBJ_EFOREST_EMERALD,
		    "Inside the desk is a beautiful emerald.\n"))
      return;
    break;
  case OBJ_EFOREST_ALTAR:
    if (find_stuff (OBJ_EFOREST_ALTAR, OBJ_EFOREST_STATUE,
		    "Inside the altar is a statue of a dark elven deity.\n"))
      return;
    break;
  case OBJ_EFOREST_MATTRESS:
    if (find_stuff (OBJ_EFOREST_MATTRESS, OBJ_EFOREST_PURSE,
		    "Hidden under the mattress is a purse.\n"))
      return;
    break;
  case OBJ_EFOREST_TRASH:
    if (find_stuff (OBJ_EFOREST_TRASH, OBJ_EFOREST_TRASH_COIN,
		    "In the trash is a silver coin.\n"))
      return;
    break;
#endif

#if defined(LOCMIN_ISLAND) && defined(LOCMIN_LABYRINTH)
  case OBJ_ISLAND_BONE:
    bprintf ("There is a flash and you are teleported...\n");
    setploc (mynum, LOC_LABYRINTH_K);
    lookin (ploc (mynum), 0);
    return;
    break;
#endif

#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_PLATFORM_ALTAR:
    if (state (OBJ_BLIZZARD_ALTAR) == 1) {
      bprintf ("You see nothing special.\n");
      return;
    } else
      break;

  case OBJ_BLIZZARD_BRICK:
    if (state (OBJ_BLIZZARD_ALTAR) == 1) {
      bprintf ("You see nothing special.\n");
      return;
    } else {
      break;
    }
#endif

#ifdef LOCMIN_RUINS
  case OBJ_RUINS_SCROLL:
#endif

#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_LITTABLET: 
#endif

#ifdef LOCMIN_RUINS
  case OBJ_RUINS_RUNES:
  case OBJ_RUINS_DAMAGERUNES:
#endif

#if defined(LOCMIN_RUINS) || defined(LOCMIN_BLIZZARD) || defined(LOCMIN_RUINS)
    bprintf("Hmm... nothing too special about this object.\n");
    return;
#endif

#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_TUBE:
    if (oarmor (a) == 0) {
      int obj = clone_object (OBJ_BLIZZARD_SCROLL, -1, NULL, mynum, CARRIED_BY);

      if (obj >= 0) {
	osetarmor (a, 1);
	bprintf ("You take a scroll from the tube.\n");
	return;
      }
    }
    break;

  case OBJ_BLIZZARD_BLACKROBE:
    if (oarmor (a) == 0) {
      int obj = clone_object (OBJ_BLIZZARD_KEY, -1, NULL, mynum, CARRIED_BY);

      if (obj >= 0) {
	bprintf ("You take a key from one pocket.\n");
	osetarmor (a, 1);
	return;
      }
    }
    break;

  case OBJ_BLIZZARD_BALL:
    setobjstate (a, randperc () % 3 + 1);
    switch (state (a)) {
    case 1:
      bprintf ("It glows red.");
      break;
    case 2:
      bprintf ("It glows blue.");
      break;
    case 3:
      bprintf ("It glows green.");
      break;
    }
    bprintf ("\n");
    return;
#endif

#ifdef LOCMIN_TOWER
  case OBJ_TOWER_WAND:
    if (oarmor (a) != 0) {
      bprintf ("It seems to be charged.\n");
      return;
    }
    break;

  case OBJ_TOWER_SCROLL:
    foo = -1;
#ifdef LOCMIN_BLIZZARD
    foo = carries_obj_type (mynum, OBJ_BLIZZARD_BALL);
#endif
    if (foo < 0)
      foo = OBJ_BLIZZARD_BALL;

    if (((z = carries_obj_type (mynum, OBJ_TOWER_RED_CANDLE)) > -1 &&
	 state (foo) == 1 &&
	 otstbit (z, OFL_LIT)) ||

	((z = carries_obj_type (mynum, OBJ_TOWER_BLUE_CANDLE)) > -1 &&
	 state (foo) == 2 &&
	 otstbit (z, OFL_LIT)) ||

	((z = carries_obj_type (mynum, OBJ_TOWER_GREEN_CANDLE)) > -1 &&
	 state (foo) == 3 &&
	 otstbit (z, OFL_LIT))) {

      bprintf ("Everything shimmers and then solidifies into a different "
	       "view!\n");
      destroy (a);
      teletrap (LOC_TOWER_POTION);
      return;
    }
    break;
#endif

#ifdef LOCMIN_VALLEY
  case OBJ_VALLEY_BED:
    if (!odamage (a)) {
      int c = clone_object (OBJ_VALLEY_LOAF, -1, NULL, mynum, CARRIED_BY);
      int b = clone_object (OBJ_VALLEY_PIE, -1, NULL, mynum, CARRIED_BY);

      if (c >= 0 && b >= 0) {
	bprintf ("Aha!  Under the bed you find a loaf and a "
		 "rabbit pie.\n");
	osetdamage (a, 1);
	return;
      }
    }
    break;
#endif
#ifdef LOCMIN_ORCHOLD
  case OBJ_ORCHOLD_GARBAGE:
    if (state (a) == 0) {

      int x = clone_mobile (max_players + MOB_ORCHOLD_MAGGOT, -1, NULL);

      if (x >= 0) {

	bprintf ("In the garbage you find a gold plate... "
		 "with a maggot on it!\n");

	sillycom ("\001s%s\003A maggot leaps out of the garbage "
		  "onto %s!\n\003");

	setploc (x, ploc (mynum));

	/* Make sure maggot attacks the right person. */
	hit_player (x, mynum, -1);

	setobjstate (a, 1);
      } else {
	bprintf ("In the garbage you find a gold plate with some "
		 "slime on it.\n");
      }

      x = clone_object(OBJ_ORCHOLD_ORCGOLD, -1, NULL, mynum, CARRIED_BY);
      if (x > -1) 
	setobjstate (a, 1);
      return;
    }
    break;
#endif

#ifdef LOCMIN_TREEHOUSE
  case OBJ_TREEHOUSE_BEDDING:
    if (a != OBJ_TREEHOUSE_BEDDING)
      break;

    if (!odamage (OBJ_TREEHOUSE_AMULET)) {
      create (OBJ_TREEHOUSE_AMULET);
      bprintf ("You pull an amulet from the bedding.\n");
      osetdamage (OBJ_TREEHOUSE_AMULET, 1);
      return;
    }
    break;
#endif

#ifdef LOCMIN_OAKTREE
  case OBJ_OAKTREE_BOARSKIN:
    if (a != OBJ_OAKTREE_BOARSKIN)
      break;

    if (!odamage (OBJ_OAKTREE_WHISTLE)) {
      create (OBJ_OAKTREE_WHISTLE);
      bprintf ("Under the boarskin you find a silver whistle.\n");
      osetdamage (OBJ_OAKTREE_WHISTLE, 1);
      return;
    }
    break;
#endif
  }

  bprintf("%s\n", oexam_text(a));
}

void
incom (Boolean inv)
{
  int x, y;
  char st[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_GOTO)) {
    /* Mortals who doesn't have goto will assume that in = inventory... */
    /* Thus hiding the fact that there IS a command called in... */
    /* Must fix it so that at this should only be the case when IN is used */
    /* and not when AT is used */

    /* old: erreval(); */

    if (inv)
      inventory ();
    else
      bprintf ("Pardon?\n");
    return;
  }
  if ((x = getroomnum ()) == 0 || !exists (x)) {
    bprintf ("Unknown player or room.\n");
    return;
  }
  getreinput (st);
  if (EQ (st, "!")) {
    bprintf ("What exactly do you want to do?\n");
    return;
  }
  if (ltstflg (x, LFL_NOAT) && !ptstflg (mynum, PFL_IGNORENOAT)) {
    bprintf ("Something won't let you do that in that location.\n");
    return;
  }
  y = ploc (mynum);
  setploc (mynum, x);
  gamecom (st, False);
  setploc (mynum, y);
}

/* reset stone */

void sys_reset (void)
{
  int i;
  int time = RESET_IDLE;

  for (i = 0; i < max_players; i++) {
    if (is_in_game (i) && plev (i) < LVL_WIZARD && ploc (i) != ploc (mynum) &&
	global_clock > plast_cmd (i) - time) {
      bprintf ("There are other players on who are not at this location,"
	       " so it won't work.\n");
      return;
    }
  }
  if (last_reset < global_clock && global_clock < last_reset + 3600) {
    bprintf ("Sorry, at least an hour must pass between resets.\n");
    return;
  }
  resetcom (0);
}

void resetcom (int flags) {
  int zone, nlocs, nobjs, nmobs;

  if ((flags & RES_TEST) && !ptstflg (mynum, PFL_RESET)) {
    erreval ();
    return;
  }
  if (brkword () != -1 && plev (mynum) >= LVL_WIZARD) {
    if ((zone = get_zone_by_name (wordbuf)) == -1) {
      bprintf ("&*No such zone: &+W%s\n", wordbuf);
      return;
    }
    send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	      "&+C[&+W\001p%s\003 &+whas reset zone &+W%s&+C]\n",
              pname(mynum), zname (zone));

    mudlog ("&+WSYSTEM:&N Reset by %s (Zone: %s)", pname(mynum), zname(zone));

    nlocs = znumloc (zone);
    nobjs = znumobs (zone);
    nmobs = znumchars (zone);

    reset_zone (zone);
    bprintf ("&*Zone &+W%s&*: Reset Completed.\n", zname (zone));
  } 
  else {                               /* normal full game reset */
    if (plev(mynum) > LVL_WIZARD) {
      send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
		"&+C[&+wReset Called by &+W\001p%s\003&+C]\n", pname (mynum));
      
      send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD - 1, NOBODY, NOBODY,
		"&+C[&+wReset Called&+C]\n");
      confirm_reset(NULL);
      return;
    }
    else {
      send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
       "&+C[&+wReset at the Stone by &+W\001p%s\003&+C]\n",pname(mynum));
      send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD - 1, NOBODY, NOBODY,
                "&+C[&+wReset Called&+C]\n");
      reset();
    }
  }
}

void reset(void) {
  int i;

  last_reset = global_clock;
  qdresetflgs();

  for (i = 0 ; i < max_players ; i++)  /* set armors which are Worn */
    if (is_in_game(i))
      reset_armor(False, i);
    
  if (breset) {
    broad ("&+C[&NSystem Rebooting&+C]\n");
    run_reboot (False, False);
  }
  mudlog ("&+WSYSTEM:&N Reset by %s", pname (mynum));    

  for (i = 0; i < numzon; i++)
    reset_zone (i);

  qdresetflgs();  
  move_pouncie();
  scatter_potions();
  ++numresets;
  broad ("&+C[&+wReset Completed&+C]\n");
}

void confirm_reset(char *input) {
  Boolean clean_up = False;

  if (input == NULL) {
    strcpy(cur_player->cprompt, "Confirm reset (y/n)? ");
    push_input_handler(confirm_reset);
    return;
  }
  else if (tolower(*input) == 'y') {
    broad ("&+C[&+wReset Confirmed&+C]\n");
    reset();
    clean_up = True;
  }
  else if (tolower(*input) == 'n') {
    broad ("&+C[&+wReset Cancelled&+C]\n");
    clean_up = True;
  }
  else
    bprintf("Please answer yes or no.\n");

  if (clean_up) {
    strcpy(cur_player->cprompt, build_prompt(mynum));
    pop_input_handler();
    return;
  }
  replace_input_handler(confirm_reset);
}

void unveilcom (char *unv_pass) {
  char buff[PASSWD_LEN];
  int lev;

  if (!isawiz(mynum)) {
    erreval ();
    return;
  }
  if (unv_pass == NULL) {
    if (brkword () == -1)
      cur_player->work = LVL_CREATOR;
    else
      cur_player->work = atoi (wordbuf);

    bprintf("\n\001R\003");
    strcpy (cur_player->cprompt, "Magic Word: ");
    push_input_handler (unveilcom);
  } 
  else {
    bprintf ("\001E\003");
    pop_input_handler ();
    my_crypt(buff, unv_pass);

    if (strcmp(buff, UNVEIL_PASS)) {
      bprintf ("Eek!  Go away!&#\n");
      mudlog ("UNVEIL (BAD): %s", pname (mynum));
    } 
    else {
      lev = cur_player->work;
      if (lev >= LVL_MAX) {
	mudlog ("UNVEIL (WRONG): %s to %d", pname (mynum), plev (mynum));
	bprintf ("The maximum level is %d.\n", LVL_MAX - 1);
      } 
      else {
        set_xpflags(mbits(mynum), lev);
	setplev (mynum, lev);
	update_wizlist (pname (mynum), wlevel (lev));
	mudlog ("UNVEIL: %s to %d", pname (mynum), plev (mynum));
	bprintf ("Certainly, master!\n");
      }
    }
    get_command (NULL);
  }
}

void reviewcom (void) {
  char xx[SETIN_MAX + 200];
  static PERSONA p;
  int x;
  Boolean me = False;
  PLAYER_REC *v = NULL;
  char *name = NULL;

  if (brkword () == -1 || EQ (wordbuf, pname (mynum))) {
    me = True;
    v = cur_player;
    x = mynum;
    name = pname(mynum);
  } 
  else if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  } 
  else if ((x = fpbn(wordbuf)) >= 0 && plev (x) >= LVL_WIZARD) {
    v = players + x;
    name = pname(x);
  } 
  else if (getuaf (wordbuf, &p) && p.ublock.plev >= LVL_WIZARD) {
    v = &p.player;
    name = p.ublock.pname;
  } 
  else {
    bprintf ("I know no wizard with the name '%s'.\n", wordbuf);
    return;
  }

  if (!me)
    bprintf("                     &-RTravel Messages for %s", name);
  else
    bprintf ("                         &-RCurrent Travel Messages");
  bprintf ("\n&+YCommand     &+BSetting\n------------------------"
	   "-------------------------------------------------------\n");
  if (v->setin)
    bprintf ("&+WSETIN&N     : %s\n", build_setin (SETIN_SETIN, xx,
      v->setin, name, NULL, NULL));
  else
    bprintf ("&+WSETIN&N     : Special system default.\n");
  
  if (v->setout)
    bprintf ("&+WSETOUT&N    : %s\n", build_setin (SETIN_SETOUT, xx,
      v->setout, name, "<dir>", NULL));
  else
    bprintf ("&+WSETIN&N     : Special system default.\n");

  bprintf ("&+WSETMIN&N    : %s\n", build_setin (SETIN_SETMIN, xx, v->setmin, name, NULL, NULL));
  bprintf ("&+WSETMOUT&N   : %s\n", build_setin (SETIN_SETMOUT, xx, v->setmout, name, NULL, NULL));
  bprintf ("&+WSETVIN&N    : %s\n", build_setin (SETIN_SETVIN, xx, v->setvin, name, NULL, NULL));
  bprintf ("&+WSETVOUT&N   : %s\n", build_setin (SETIN_SETVOUT, xx, v->setvout, name, NULL, NULL));
  bprintf ("&+WSETQIN&N    : %s\n", build_setin (SETIN_SETQIN, xx, v->setqin, name, NULL, NULL));
  bprintf ("&+WSETQOUT&N   : %s\n", build_setin (SETIN_SETQOUT, xx, v->setqout, name, NULL, NULL));
  bprintf ("&+WSETSIT&N    : %s\n", build_setin (SETIN_SETSIT, xx, v->setsit, name, NULL, NULL));
  bprintf ("&+WSETSTND&N   : %s\n", build_setin (SETIN_SETSTAND, xx, v->setstand, name, NULL, NULL));
  bprintf ("&+WSETSUM&N    : %s\n", build_setin (SETIN_SETSUM, xx, v->setsum, name, NULL, NULL));
  bprintf ("&+WSETSIN&N    : %s\n", build_setin (SETIN_SETSUMIN, xx, v->setsumin, name, NULL, "<victim>"));
  bprintf ("&+WSETSOUT&N   : %s\n", build_setin (SETIN_SETSUMOUT, xx, v->setsumout, name, NULL, "<victim>"));
  bprintf ("&+WPROMPT&N    : %s\n", build_prompt(x));
}

void
sitcom (void)
{
  if (psitting (mynum)) {
    bprintf ("You're already sitting.\n");
    return;
  } else if (pfighting (mynum) >= 0) {
    bprintf ("You want to sit down while fighting?  Do you have a death "
	     "wish or something?\n");
    return;
  } else {
    bprintf ("You assume the lotus position.\n");
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	      "%s sits down.\n", pname (mynum));
    setpsitting (mynum, 1);
  }
#ifdef LOCMIN_ICECAVE
  if (ploc (mynum) == LOC_ICECAVE_THRONE) {
    if (oloc (OBJ_ICECAVE_WESTICETHRONE)) {
      bprintf ("The throne collapses under your weight, revealing a "
               "passage east.\n");
      setobjstate (OBJ_ICECAVE_WESTICETHRONE, 0);
      return;
    }
  }
#endif
}

void ringcom (void) {
  switch (ob1) {
#ifdef LOCMIN_VILLAGE
  case OBJ_VILLAGE_BELL:
    broad ("There is an almighty &+WBONG&*!\n");
    break;
#endif
  default:
    bprintf ("You can't ring that.\n");
    break;
  }
}

void
standcom (int player)
{
  if (!psitting (player)) {
    sendf (player, "You're already standing.\n");
    return;
  } 
  else {
    sendf (player, "You clamber to your feet.\n");
    send_msg(sendloc(player), 0, pvis (player), LVL_MAX, player, NOBODY,
	  "%s clambers to %s feet.\n", pname (player), his_or_her (player));
    setpsitting (player, 0);
  }
}

void pncom (void) {
  int plx = real_mynum;
  char *p;

  if (pl1 == -1)
    bprintf("Current pronouns are:\n");
  else if (pl1 > max_players)
    bprintf("That's a mobile.\n");
  else {
    plx = pl1;
    bprintf("Current pronouns for %s are:\n", pname (plx));
  }
  bprintf("Me    : %s\n", pname (plx));
  bprintf("It    : %s\n", (p = wd_it(plx)) == NULL ? "<none>" : p);
  bprintf("Him   : %s\n", (p = wd_him(plx)) == NULL ? "<none>" : p);
  bprintf("Her   : %s\n", (p = wd_her(plx)) == NULL ? "<none>" : p);
  bprintf("Them  : %s\n", (p = wd_them(plx)) == NULL ? "<none>" : p);
}

void bugcom (void)
{
  char x[MAX_COM_LEN];

  getreinput (x);
  if (EMPTY (txt1)) {
    bprintf ("What do you want to bug me about?\n");
  } else {
    mudlog ("BUG: %s - %s", pname (mynum), x);
    bprintf ("Thank you for reporting a bug.\n");
  }
}

void suggestcom (void)
{
    char x[MAX_COM_LEN];

  getreinput (x);
  if (EMPTY (txt1)) {
    bprintf ("What would you like to suggest?\n");
  } else {
    mudlog ("SUGGEST: %s - %s", pname (mynum), x);
    bprintf ("Thank you for your suggestion.  We appreciate feedback.\n");
  }
}



void
typocom (void)
{
  int l = ploc (mynum);
  char x[MAX_COM_LEN];

  getreinput (x);
  if (EMPTY (x)) {
    bprintf ("What typo do you wish to inform me of?\n");
  } else {
    mudlog ("TYPO: %s (%s): %s", pname (mynum), showname (l), x);
    bprintf ("Ok.\n");
  }
}

void helpcom (void) {
  int a, b;

  if (*item1 != 0) {
    if (help(plev(mynum), mbits(mynum), item1) < 0)
      return;
    if ((a = vichere ()) < 0)
      return;
    if ((b = phelping (mynum)) >= 0) {
      sendf(b, "\001p%s\003 stopped helping you.\n", pname(mynum));
      bprintf ("Stopped helping \001p%s\003.\n", pname (b));
    }
    if (a == mynum) {
      bprintf ("You are beyond help.\n");
      return;
    }
    setphelping (mynum, a);
    sendf(a, "\001p%s\003 has offered to help you.\n", pname(mynum));
    bprintf ("Started helping \001p%s\003.\n", pname (phelping (mynum)));
    return;
  }
  bprintf ("\001f%s/%s\003", HELP_DIR, HELP1);
  if (plev (mynum) < LVL_WIZARD) {
    return;
  }
  strcpy (cur_player->cprompt, qwait);
  replace_input_handler (help2);
}

void help2 (char *cont) {
  if (tolower(*cont) == 'q')
    get_command (NULL);
  else {
    bprintf ("\001f%s/%s\003\n", HELP_DIR, HELP2);
    if (plev (mynum) < LVL_ISTARI)
      get_command (NULL);
    else
      replace_input_handler (help3);
  }
}

void help3 (char *cont) {
  if (tolower(*cont) == 'q')
    get_command (NULL);
  else {
    bprintf ("\001f%s/%s\003\n", HELP_DIR, HELP3);
    if (plev (mynum) < LVL_ARCHWIZARD)
      get_command (NULL);
    else
      replace_input_handler (help4);
  }
}

void help4 (char *cont) {
  if (tolower(*cont) == 'q')
    get_command (NULL);
  else {
    bprintf ("\001f%s/%s\003\n", HELP_DIR, HELP4);
    if (plev (mynum) < LVL_DEMI)
      get_command (NULL);
    else
      replace_input_handler (help5);
  }
}

void help5 (char *cont) {
  if (tolower(*cont) == 'q')
    get_command (NULL);
  else {
    bprintf ("\001f%s/%s\003\n", HELP_DIR, HELP5);
    if (plev (mynum) < LVL_SHALAFI)
      get_command (NULL);
    else
      replace_input_handler (help6);
  }
}

void help6 (char *cont) {
  if (tolower(*cont) == 'q')
    get_command (NULL);
  else {
    bprintf ("\001f%s/%s\003\n", HELP_DIR, HELP6);
    if (plev (mynum) < LVL_GOD)
      get_command (NULL);
    else
      replace_input_handler (help7);
  }
}

void help7 (char *cont)
{
  if (tolower(*cont) == 'q')
    get_command (NULL);
  else {
    bprintf ("\001f%s/%s\003\n", HELP_DIR, HELP7);
    get_command (NULL);
  }
}

void qinfocom (void) {
  char file[100];

  if (brkword () == -1) {
    strcpy (file, QINFO_DIR "/" QINFO);
  } else {
    if (strlen (wordbuf) >= (sizeof (file) - 10)) {
      bprintf ("No info available on that topic.\n");
      return;
    }
    sprintf (file, "%s/%s.qinfo", QINFO_DIR, lowercase (wordbuf));
    if (access (file, R_OK) < 0) {
      bprintf ("No info available on that topic.\n");
      return;
    }
  }
 
  bprintf("\001f%s\003", file);
}

/* The INFO command.
 */
void
infocom (void)
{
  char file[100];

  if (brkword () == -1) {
    strcpy (file, INFO_DIR "/" INFO);
  } else {
    if (strlen (wordbuf) >= (sizeof (file) - 10)) {
      bprintf ("No info available on that topic.\n");
      return;
    }
    sprintf (file, "%s/%s.i", INFO_DIR, lowercase (wordbuf));
    if (access (file, R_OK) < 0) {
      bprintf ("No info available on that topic.\n");
      return;
    }
  }

  bprintf("\001f%s\003", file);
}

/* The POLICY command.
 */
void
policycom (void)
{
  char file[100];

  if (brkword () == -1) {
    strcpy (file, POLICY_DIR "/" POLICY);
  } else {
    if (strlen (wordbuf) >= (sizeof (file) - 10)) {
      bprintf ("No info available on that topic.\n");
      return;
    }
    sprintf (file, "%s/%s.p", POLICY_DIR, lowercase (wordbuf));
    if (access (file, R_OK) < 0) {
      bprintf ("No such policy exists.\n");
      return;
    }
  }
  bprintf("\001%s\003", file);
}

void globalcom (void) {
  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  } else if (plev (mynum) >= LVL_GOD && dump_pflags ())
    return;

  bprintf ("&+CGlobal Settings:\n");
  bprintf ("&+B---------------------------------------------------------------------------\n");
#ifndef XR_WEATHER
  bprintf ("&+GWeather (Season)  : &+W%s (%s)\n",
	   weather_desc (the_climate->weather),
	   season_name (the_climate->season));
#endif
  if (the_world->w_lock)
    bprintf ("&+GLock-status       : &+WLevel %d\n", the_world->w_lock);
  else
    bprintf ("&+GLock-status       : &+WUn-locked\n");
         
  bprintf ("&+GMobiles           : &+W%s\n",
	   (the_world->w_mob_stop) ? "Stopped" : "Started");
  bprintf ("&+GWar/Peace         : &+W%s\n",
	   (the_world->w_peace) ? "Peace" : "War");
  bprintf ("&+GTournament-Mode   : &+W%s\n",
	   (the_world->w_tournament) ? "On" : "Off");
  bprintf ("&+GReboot on Reset   : &+W%s\n", breset ? "Yes" : "No");
  bprintf ("&+GMax. Users        : &+W%d\n", max_players);
  bprintf ("&+GNumber of Rooms   : "
     "&+W%4d &*permanent +&+W%4d &*wiz-made = &+W%4d\n",
	   num_const_locs, numloc - num_const_locs, numloc);
  bprintf ("&+GNumber of Mobiles : "
     "&+W%4d &*permanent +&+W%4d &*wiz-made = &+W%4d\n",
	   num_const_chars - max_players, numchars - num_const_chars,
	   numchars - max_players);
  bprintf ("&+GNumber of Objects : "
     "&+W%4d &*permanent +&+W%4d &*wiz-made = &+W%4d\n",
	   num_const_obs, numobs - num_const_obs, numobs);
  bprintf ("&+B---------------------------------------------------------------------------\n");

}

int aliased_mob (int plx) {
  int i;

  for (i = 0 ; i < max_players ; i++)
    if (aliasto(i) == plx)
      return(i);
  return(-1);
}

static void do_alias (int plx) {
  setpfighting(mynum, -1);

  if (plx == -1) {
    sclrflg (mynum, SFL_OCCUPIED);
    mynum = real_mynum;
    aliased(mynum) = False;
    aliasto(mynum) = -1;
  }
  else {
    aliased(mynum) = True;
    aliasto(mynum) = plx;
    mynum = plx;
    ssetflg (plx, SFL_OCCUPIED);
  }
}

void aliascom (void) {
  if (!ptstflg(mynum, PFL_ALIAS) && !ptstflg(mynum, PFL_ALIASP)) {
    bprintf("You couldn't alias your way out of a cardboard box.\n");
    return;
  }
  else if (pl1 == -1) {
    bprintf ("Who?\n");
    return;
  }
  if (polymorphed(real_mynum) >= 0) {
    bprintf ("A mysterious force stops you.\n");
    return;
  }
  if (ststflg (pl1, SFL_OCCUPIED)) {
    bprintf ("Already occupied!\n");
    return;
  }
  if (pstr(pl1) < 0) {
    bprintf ("They're dead, you sicko.\n");
    return;
  }

  if (pl1 < max_players) {
    if (!ptstflg (mynum, PFL_ALIASP)) {
      bprintf ("You can't become another player!\n");
      return;
    }
    if (!do_okay (mynum, pl1, PFL_NOALIAS)) {
      if (plev (mynum) <= LVL_WIZARD)
	bprintf ("Already occupied!\n");
      else
	bprintf ("They don't want to be aliased!\n");
      return;
    }
  } 
  else if (!ptstflg (mynum, PFL_ALIAS)) {
    bprintf ("You can't become a mobile!\n");
    return;
  }
  send_msg (DEST_ALL, 0, LVL_ARCHWIZARD, LVL_MAX, pl1, NOBODY,
            "&+B[&+W%s &*has &+Caliased &+W%s&+B]\n",
            pname (mynum), pname (pl1));

  do_alias(pl1);
  bprintf ("Aliased to %s.\n", pname (pl1));
}

void polymorph (int plx, int turns) {
  do_alias(plx);
  polymorphed(real_mynum) = turns;
  bprintf("You pass out.....\n..... and wake up.\n");
}

void unalias (int me)  {

#ifdef LOCMIN_GREEN
  if (aliasto(me) == max_players+MOB_GREEN_THEDOCTOR)
    {
    setptitle(me, std_title(plev(me), psex(me), pclass(me)));
    pvis(me) = 0;
    setpscore(max_players+MOB_GREEN_THEDOCTOR, 0);
  }
#endif

  setup_globals(me);

  if (aliased(me)) {
    do_alias (-1);
    setpfighting(me, -1);

    if (polymorphed(me) == -1)
      bprintf ("Back to good old %s....\n", pname (mynum));
    else
      bprintf ("Suddenly you awake... were you dreaming or what?\n");
    polymorphed(me) = -1;
  }
}

void becom (char *passwd) {
  char x[128];
  static PERSONA p;

  if (passwd == NULL) {
    if (aliased(real_mynum))
      bprintf ("Not when aliased.");
    else if (cur_player->writer)
      bprintf ("Finish whatever you're writing first!\n");
    else if (pfighting (mynum) != -1)
      bprintf ("Not while fighting!\n");
    else if (brkword () == -1)
      bprintf ("Become what?  Inebriated?\n");
    else {
      strcpy ((char *) cur_player->work2, wordbuf);
      bprintf ("\n\001R\003");
      strcpy (cur_player->cprompt, "Password: ");
      push_input_handler (becom);
    }
  } 
  else {
    pop_input_handler ();

    if (!getuaf ((char *) cur_player->work2, &p))
      bprintf ("No such player.\n");
    else if (!EQ (p.player.passwd, my_crypt (x, passwd)))
      bprintf ("Incorrect password!\n");
    else if (fpbn((char *) cur_player->work2) >= 0)
      bprintf ("That player is already on the game.\n");
    else {
      dumpitems();
      saveme(False);
      wipe_duration(mynum);
      p.ublock.ploc = ploc(mynum);

      mudlog ("&+WSYSTEM:&N %s has BECOME %s", pname (mynum), p.ublock.pname);
      send_msg (DEST_ALL, MODE_QUIET, max (LVL_WIZARD, pvis (mynum)), LVL_MAX,
		mynum, NOBODY, "&+B[&+W%s &*has &+CBECOME &+W%s&+B]\n", pname (mynum), p.ublock.pname);
      send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
		"%s disintegrates, and reappears as %s.\n",
		pname (mynum), p.ublock.pname);

      pers2player (&p, mynum);

      if (cur_player->prompt)
        strcpy(cur_player->cprompt, cur_player->prompt);
      else
        strcpy(cur_player->cprompt, DEFAULT_PROMPT);

      cur_player->iamon = True;

      bprintf ("You are now %s.\n", pname (mynum));
      check_files ();

      if (privleged_user(pname(mynum)))
	isawiz(mynum) = True;

      if (is_plr_monitored(pname(mynum)))
	open_plr_log ();
    }
    bprintf ("\001E\003");
    get_command (NULL);
  }
}



void rollcom (void) {
  if (!ishere(ob1))
    return;

  switch (ob1) {
#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_PILLAR_WEST:
  case OBJ_BLIZZARD_PILLAR_EAST:
    gamecom ("push pillar", True);
    break;
  case OBJ_BLIZZARD_BOULDER:
    gamecom ("push boulder", True);
    break;
#endif
  default:
    bprintf ("You can't roll that.\n");
  }
}

void emptycom (void) {
  int a, b, ct, nobs;
  Boolean carried = False;

  if ((a = ob1) == -1) {
    bprintf("What's that?\n");
    return;
  }

  nobs = onumobs(a);

  if (oloc(a) == mynum)
    carried = True;
  else if (oloc(a) != ploc(mynum)) {
    bprintf("It's not here.\n");
    return;
  }

  if (otstbit(a, OFL_LOCKABLE) && state (a) == 2 && 
    !hasobjtype(mynum, OFL_KEY)) {
    bprintf("The %s is locked, and you have no key.\n", oname(a));
    return;
  } 
  else if ((otstbit (a,OFL_OPENABLE) || otstbit(a, OFL_LOCKABLE))
    && state(a) > 0) {
    bprintf("You open the %s.\n", oname(a));
    setobjstate(a, 0);
  }

  if (nobs == 0) {
    bprintf("The %s is empty.\n", oname(a));
    return;
  }

  bprintf("You take the %s from the %s, and put %s %s%s\n", 
           nobs < 2 ? oname(ofirst_obj(a)) : "following items", oname(a),
           nobs < 2 ? "it" : "them",
           carried ? "in your backpack" : "on the floor",
           nobs < 2 ? "." : ":");

  for (ct = 0 ; ct < onumobs(a) ; ct++) {
    b = oobj_nr(ct, a);
    if (iscontin(b, a)) {
      ct--;
      if (nobs > 1)
        bprintf("%s\n", oname(b));
      if (carried)
        setoloc(b, mynum, CARRIED_BY);
      else
        setoloc(b, ploc(mynum), IN_ROOM);
    }
  }
  if (nobs > 1) {
    bprintf("\n");
    send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
      "%s empties things from the %s.\n", pname(mynum), oname(a));
  }
  else
    send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
      "%s empties something from the %s.\n", pname(mynum), oname(a));
}

void
treasurecom (void)
{
  int a, c, tc = 0;
  char b[40], d[40];

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  for (a = 0; a < numobs; a++) {
    if (!oflannel (a) && !otstbit (a, OFL_DESTROYED) && ocarrf (a) == IN_ROOM) {
      tc++;
      c = findzone (oloc (a), b);
      sprintf (d, "%s%d", b, c);
      bprintf ("%-13s%-13s", oname (a), d);
      if (tc % 3 == 0)
	bprintf ("\n");
    }
  }
  bprintf ("\nApproximately %d treasures remain.\n", tc);
}

void
promptcom (void)
{
  char buff[MAX_COM_LEN];

  getreinput (buff);

  if (strlen (buff) > PROMPT_LEN)
    bprintf ("Max. prompt length is %d.\n", PROMPT_LEN);
  else if (!*buff) {
    FREE (cur_player->prompt);
    cur_player->prompt = NULL;
    bprintf ("Prompt reset.\n");
  }
  else {
    FREE( cur_player->prompt );
    cur_player->prompt = COPY( buff );
    bprintf ("Ok\n");
  }
}

void playcom (void) {
#ifdef LOCMIN_FOREST
  if (ob1 == OBJ_FOREST_PIPES) {
    bprintf ("Much as you try, the pipes only make nasty squeaking noises.\n");
    return;
  }
#endif
#ifdef LOCMIN_BLIZZARD
  if (ob1 == OBJ_BLIZZARD_HARP) {
    bprintf ("A beautiful harp melody floats to you on a gentle breeze.\n");
    return;
  }
#endif
}


void blowcom (void) {
  int a;
#ifdef LOCMIN_OAKTREE
  char s[100];
#endif

  if (!ishere(ob1) && !iscarrby(ob1, mynum))
    return;
  else
    a = ob1;

  switch(a) {
#ifdef LOCMIN_CAVE
  case OBJ_CAVE_BAGPIPES:
    broad ("\001dA hideous wailing sounds echos all around.\n\003");
    return;
#endif
#ifdef LOCMIN_OAKTREE
  case OBJ_OAKTREE_WHISTLE:
    broad ("\001dA strange ringing fills your head.\n\003");
    if (alive (max_players + MOB_OAKTREE_OTTIMO) != -1) {
      bprintf ("A small dachshund bounds into the room "
	       "and leaps on you playfully.\n");
      sprintf (s, "A small dachshund bounds into the room "
	       "and leaps on %s playfully.\n", pname (mynum));
      sillycom (s);
      setploc (max_players + MOB_OAKTREE_OTTIMO, ploc (mynum));
    }
    return;
#endif
#if defined(LOCMIN_LABYRINTH) && defined(LOCMIN_SEA)
  case OBJ_LABYRINTH_HORN:
    broad ("\001dA mighty horn blast echoes around you.\n\003");
    if (ploc (mynum) >= LOC_SEA_1 && ploc (mynum) <= LOC_SEA_TREASURE &&
	oarmor (OBJ_SEA_EXCALIBUR) == 0) {
      setoloc (OBJ_SEA_EXCALIBUR, ploc (mynum), IN_ROOM);
      setobjstate (OBJ_SEA_EXCALIBUR, 1);
      bprintf ("A hand breaks through the water holding up "
	       "the sword Excalibur!\n");
      osetarmor (OBJ_SEA_EXCALIBUR, 1);
      set_quest (mynum, Q_EXCALIBUR);
    }
    return;
#endif
#ifdef LOCMIN_CAVE
  case OBJ_CAVE_WHISTLE:
    broad( "A deep resonating tone can be heard echoing throughout the realms.\n" );
    if( ploc( mynum ) == LOC_CAVE_336 && state( OBJ_CAVE_FORGE ) != 0 ) {
      bprintf( "The forge tilts on it's axis, revealing a passage leading down.\n" );
      setobjstate( OBJ_CAVE_FORGE, 0 );
    } 
    return;
#endif
  }
  bprintf ("You can't blow that.\n");
}


void lightcom () {
  int a;
#ifdef LOCMIN_EFOREST
  char s[100];
#endif

  if (!ishere(ob1) && !iscarrby(ob1, mynum))
    return;
  else
    a = ob1;

  if (!hasobjtype(mynum, OFL_LIT) && !ststflg (mynum, SFL_LIT)) {
    bprintf ("You have nothing to light things from.\n");
    return;
  }
#ifdef LOCMIN_EFOREST

  if (a == OBJ_EFOREST_THORNS || a == OBJ_EFOREST_THORNSEAST) {
    if (state (OBJ_EFOREST_THORNS) == 0) {
      bprintf ("The thorns have already been burned away.\n");
      return;
    }
    setobjstate (OBJ_EFOREST_THORNS, 0);
    bprintf ("You burn the wall of thorns away!\n");
    bprintf ("Behind them you can see the entrance to a cave.\n");
    sprintf (s, "%s burns the wall of thorns away!\n", pname (mynum));

    send_msg(int2idx(LOC_EFOREST_THORNY, LOC), 0, LVL_MIN, LVL_MAX, mynum, 
      NOBODY, s);
    send_msg(int2idx(LOC_EFOREST_CAVE, LOC), 0, LVL_MIN, LVL_MAX, mynum,
      NOBODY, s);

    return;
  }
#endif

#ifdef LOCMIN_FOREST
  if (a == OBJ_FOREST_INSIDETREE) {
    bprintf ("The tree screams and thrashes around.  In its dying throes "
             "you dash free!\n");
    trapch (LOC_FOREST_F4);
    destroy (OBJ_FOREST_TREEEATING);
    setpscore (mynum, pscore (mynum) + 100);
    return;
  }
#endif
#include "lightcom.h"

  if (!otstbit (a, OFL_LIGHTABLE))
    bprintf ("You can\'t light that!\n");
  else if (otstbit (a, OFL_LIT))
    bprintf ("It\'s already lit.\n");
  else {
    setobjstate (a, 0);
    osetbit (a, OFL_LIT);
    bprintf ("Ok\n");
  }
}

void extinguishcom () {
  int a;

  if (!ishere(ob1) && !iscarrby(ob1, mynum))
    return;
  else
    a = ob1;

  if (!otstbit (a, OFL_LIT))
    bprintf ("It\'s not lit!\n");
  else if (!otstbit (a, OFL_EXTINGUISH))
    bprintf ("You can\'t extinguish that!\n");
  else {
    setobjstate (a, 1);
    oclrbit (a, OFL_LIT);
    bprintf ("Ok\n");
  }
}




void pushcom (void) {
  int x;
#if defined(LOCMIN_EFOREST) || defined(LOCMIN_FROBOZZ)
  char s[128];
#endif

  if (brkword () == -1) {
    bprintf ("Push what?\n");
    return;
  }
  if ((x = ob1) == -1) {
    bprintf ("There is no such object to push.\n");
    return;
  }
  if (oloc(x) != ploc(mynum) && oloc(x) != mynum) {
    bprintf("It's not here.\n");
    return;
  }

  switch (x) {
#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_TRIPWIRE:
    bprintf ("The tripwire moves and a huge stone crashes down from above!\n");
    broad ("\001dYou hear a thud and a squelch in the distance.\n\003");
    quit_msg("S   P    L    A    T!", "&+WCrushed by a stone");
    quit_player(False);
    break;
  case OBJ_BLIZZARD_BOOKCASE:
    bprintf ("A trapdoor opens at your feet and you plumment downwards!\n");
    sillycom ("\001p\003 disappears through a trapdoor!\n");
    teletrap (LOC_BLIZZARD_FISSURE);
    return;
#endif
  }


  /* If it's the original object (only), not a copy: */
  switch (x) {

#include "pushcom.h"

#ifdef LOCMIN_EFOREST

  case OBJ_EFOREST_BUTTON:
    setobjstate (OBJ_EFOREST_BUTTON, 0);
    setobjstate (OBJ_EFOREST_THRONE_CATHEDRAL,
		 1 - state (OBJ_EFOREST_THRONE_CATHEDRAL));

    sendl(ploc(mynum),
	"You hear a grinding sound from near the entrance of the caves.\n");

    sprintf (s, "You hear a grinding sound as a mysterious force moves "
	     "the throne!\n");

    sendl(LOC_EFOREST_EASTEND, s);
    sendl(LOC_EFOREST_BOTTOM, s);
    break;
#endif

#ifdef LOCMIN_FROBOZZ
  case OBJ_FROBOZZ_BUTTON_OUTSIDE:
    if (state (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE) == 1) {
      setobjstate (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE, 0);

      sendl(LOC_FROBOZZ_OUTSIDE, "Without a sound the whole southern "
	     "wall moves some feet westwards!\n");

      sendl(LOC_FROBOZZ_VAULT, "Without a sound the whole northern wall "
	     "moves some feet westwards!\n");
      break;
    }
    if (state (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE) == 0) {
      setobjstate (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE, 2);
      sendl(LOC_FROBOZZ_OUTSIDE, "The southern wall glides some feet "
	     "to the east closing the vault!\n");

      sendl(LOC_FROBOZZ_VAULT, "The northern wall glides some feet to "
	     "the east closing the vault!\n");
      break;
    }
    if (state (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE) == 2) {
      bprintf ("Nothing happens.\n");
      break;
    }
  case OBJ_FROBOZZ_RUG_LIVING:
    if (state (OBJ_FROBOZZ_RUG_LIVING) == 1) {
      setobjstate (OBJ_FROBOZZ_RUG_LIVING, 0);
      setobjstate (OBJ_FROBOZZ_TRAPDOOR_LIVING, 2);
      bprintf ("You move the rug and uncover a trapdoor!\n");
      sprintf (s, "%s moves the heavy rug and uncovers a trapdoor!\n",
	       pname (mynum));
      sillycom (s);
    } else {
      bprintf ("You move the rug, but find nothing.\n");
      sprintf (s, "%s moves the rug.\n", pname (mynum));
      sillycom (s);
    }
    break;
#endif
#ifdef LOCMIN_ANCIENT
  case OBJ_ANCIENT_MOONCROSS:
    if (state (OBJ_ANCIENT_MOONCROSS) > 0) {
      setobjstate (OBJ_ANCIENT_MOONCROSS, 0);
      if (state (OBJ_ANCIENT_BIGCROSS) > 0) {
	bprintf ("You move the cross to one side, but nothing happens.\n");
      } else {
	bprintf ("You hear a loud hollow sound from a room nearby!\n");
	setobjstate (OBJ_ANCIENT_HOLE_ANC31, 0);
      }
    } else {
      setobjstate (OBJ_ANCIENT_MOONCROSS, 1);
      setobjstate (OBJ_ANCIENT_HOLE_ANC31, 1);
      bprintf ("You move the cross back into place.\n");
    }
    break;
  case OBJ_ANCIENT_BIGCROSS:
    if (state (OBJ_ANCIENT_BIGCROSS) > 0) {
      setobjstate (OBJ_ANCIENT_BIGCROSS, 0);
      if (state (OBJ_ANCIENT_MOONCROSS) > 0) {
	bprintf ("You move the cross to one side, but nothing happens.\n");
      } else {
	bprintf ("You hear a loud hollow sound from a room nearby!\n");
	setobjstate (OBJ_ANCIENT_HOLE_ANC31, 0);
      }
    } else {
      setobjstate (OBJ_ANCIENT_BIGCROSS, 1);
      setobjstate (OBJ_ANCIENT_HOLE_ANC31, 1);
      bprintf ("You move the cross back into place.\n");
    }
    break;
#endif

#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_ALTAR:
    if (state (OBJ_BLIZZARD_PLATFORM_ALTAR) == 2) {
      setobjstate (OBJ_BLIZZARD_PLATFORM_ALTAR, 1);
      setobjstate (OBJ_BLIZZARD_ALTAR, 0);
      bprintf ("You move the altar to the side, uncovering the platform.\n");
      break;
    }
  case OBJ_BLIZZARD_BRICK:
    if (state (OBJ_BLIZZARD_ALTAR) == 1) {
      bprintf ("That is not here.\n");
      break;
    } 
    else {
      setobjstate (OBJ_BLIZZARD_PLATFORM_ALTAR, 0);
      setobjstate (OBJ_BLIZZARD_BRICK, 0);
      bprintf ("You push the brick, and the platform opens!\n");
      break;
    }
  case OBJ_BLIZZARD_IRONBAR:
    if (state (OBJ_BLIZZARD_PASS_STONE) == 1) {
      setobjstate (OBJ_BLIZZARD_PASS_FISSURE, 0);
      bprintf ("A secret panel opens in the east wall!\n");
      break;
    }
    bprintf ("Nothing happens.\n");
    break;
  case OBJ_BLIZZARD_BOULDER:
    bprintf ("With a mighty heave you manage to move the boulder a few feet\n");
    if (state (OBJ_BLIZZARD_HOLE_CRACK) == 1) {
      bprintf ("uncovering a hole behind it.\n");
      setobjstate (OBJ_BLIZZARD_HOLE_CRACK, 0);
    } else {
      bprintf ("covering a hole behind it.\n");
      setobjstate (OBJ_BLIZZARD_HOLE_CRACK, 1);
    }
    break;
  case OBJ_BLIZZARD_LEVER:
    if (ptothlp (mynum) == -1) {
      bprintf ("It's too stiff.  Maybe you need help.\n");
      return;
    }
    bprintf ("Ok\n");
    if (state (OBJ_BLIZZARD_SLIME_PIT) != 0) {
          send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX,
              mynum, NOBODY,
              "%s pulls the lever.\n", pname(mynum));
      sendl(oloc (OBJ_BLIZZARD_LEVER),
	     "\001dYou hear a gurgling noise and then silence.\n\003");
      setobjstate (OBJ_BLIZZARD_SLIME_PIT, 0);
      sendl(oloc (OBJ_BLIZZARD_SLIME_PIT),
	  "\001cThere is a muffled click and the slime drains away.\n\003");
    }
    break;
  case OBJ_BLIZZARD_THRONE_CURTAINS:
  case OBJ_BLIZZARD_ROUGH_CURTAINS:
    setobjstate (OBJ_BLIZZARD_THRONE_CURTAINS,
		 1 - state (OBJ_BLIZZARD_THRONE_CURTAINS));
    bprintf ("Ok\n");
    break;
  case OBJ_BLIZZARD_BRIDGE_LEVER:
    setobjstate (OBJ_BLIZZARD_BRIDGE_HALL, 1 - state (OBJ_BLIZZARD_BRIDGE_HALL));
    if (state (OBJ_BLIZZARD_BRIDGE_HALL)) {
      sendl(oloc (OBJ_BLIZZARD_BRIDGE_HALL), "\001cThe drawbridge rises.\n\003");
      sendl(oloc (OBJ_BLIZZARD_BRIDGE_FIRE), "\001cThe drawbridge rises.\n\003");
    } 
    else {
      sendl(oloc (OBJ_BLIZZARD_BRIDGE_HALL),
	     "\001cThe drawbridge is lowered.\n\003");
      sendl(oloc (OBJ_BLIZZARD_BRIDGE_FIRE),
	     "\001cThe drawbridge is lowered.\n\003");
    }
    break;
#endif

#ifdef LOCMIN_RUINS
  case OBJ_RUINS_OUTSIDETORCH:
    if (state (OBJ_RUINS_OUTSIDETORCH) == 1) {
      setobjstate (OBJ_RUINS_OUTSIDETORCH, 0);
      bprintf ("You pull the torch down and the west wall collapses!\n");
      break;
    } else {
      bprintf ("The torch has already been moved.\n");
      break;
    }
#endif

#ifdef LOCMIN_CASTLE
  case OBJ_CASTLE_LEVER:
    setobjstate (OBJ_CASTLE_PORT_INSIDE, 1 - state (OBJ_CASTLE_PORT_INSIDE));
    if (state (OBJ_CASTLE_PORT_INSIDE)) {
      sendl(oloc (OBJ_CASTLE_PORT_INSIDE), "\001cThe portcullis falls.\n\003");
      sendl(oloc (OBJ_CASTLE_PORT_OUTSIDE), "\001cThe portcullis falls.\n\003");
    } 
    else {
      sendl(oloc (OBJ_CASTLE_PORT_INSIDE), "\001cThe portcullis rises.\n\003");
      sendl(oloc (OBJ_CASTLE_PORT_OUTSIDE), "\001cThe portcullis rises.\n\003");
    }
    break;
  case OBJ_CASTLE_TORCH:
    if (state (OBJ_CASTLE_DOOR_GOLEM) == 1) {
      setobjstate (OBJ_CASTLE_DOOR_GOLEM, 0);
      sendl(oloc (OBJ_CASTLE_DOOR_GOLEM),
	     "A secret door slides quietly open in the south wall!\n");
    } else
      bprintf ("It moves but nothing seems to happen.\n");
    return;
#endif

#ifdef LOCMIN_CHURCH
  case OBJ_CHURCH_ROPE:
    if (oarmor (OBJ_CHURCH_ROPE) >= 12)
      bprintf ("\001dChurch bells ring out around you.\n\003");
    else {
      broad ("\001dChurch bells ring out around you.\n\003");
      if (++oarmor (OBJ_CHURCH_ROPE) == 12) {
	bprintf ("A strange ghostly guitarist shimmers briefly before you.\n");
	setpscore (mynum, pscore (mynum) + 300);
	broad ("\001dA faint ghostly guitar solo "
	       "floats through the air.\n\003");
      }
    }
    break;
#endif

#ifdef LOCMIN_CATACOMB
  case OBJ_CATACOMB_DUST:
    bprintf ("Great clouds of dust billow up, causing you to sneeze "
	     "horribly.\nWhen you're finished sneezing, you notice "
	     "a message carved into one wall.\n");

    broad ("\001dA loud sneeze echoes through the land.\n\003");
    destroy (OBJ_CATACOMB_DUST);
    create (OBJ_CATACOMB_KOAN);
    break;
#endif

#ifdef ORCHOLD
  case OBJ_ORCHOLD_BOTCOVER:
    bprintf ("You can't seem to get enough leverage to move it.\n");
    return;
  case OBJ_ORCHOLD_TOPCOVER:
    if (ptothlp(mynum) == -1) {
      bprintf ("You try to shift it, but it's too heavy.\n");
      break;
    }
    sendf(ptothlp(mynum), "\001p%s\003 pushes the cover aside with your "
                         "help.\n", pname(mynum));
    setobjstate (x, 1 - state (x));
    oplong (x);
    return;
  case OBJ_ORCHOLD_SWITCH:
    if (state (x)) {
      bprintf ("A hole slides open in the north wall!\n");
      setobjstate (x, 0);
    } else
      bprintf ("You hear a little 'click' sound.\n");
    return;
#endif

#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_STATUE_DOWN:
    if (ptothlp (mynum) == -1) {
      bprintf ("You can't shift it alone, maybe you need help.\n");
      break;
    }
    sendf(ptothlp(mynum), "\001P%s\003 pushes the statue with your "
                         "help.\n", pname(pfollow(mynum)));
    /* FALLTHROUGH */
#endif
  default:
    if (otstbit (x, OFL_PUSHABLE)) {
      setobjstate (x, 0);
      oplong (x);
      return;
    }
    if (otstbit (x, OFL_PUSHTOGGLE)) {
      setobjstate (x, 1 - state (x));
      oplong (x);
      return;
    }
    bprintf ("Nothing happens.\n");
  }
}

void followcom (void) {
  if (mynum >= max_players) {
    bprintf("But you're a mobile! You don't need to follow anyone.\n");
    return;
  } 
  if (pfollow(mynum) >= 0) {
    bprintf ("Stopped following \001P%s\003.\n", pname (pfollow(mynum)));
    sendf(pfollow(mynum), "\001p%s\003 has stopped following you.\n", pname (mynum));
    pfollow(mynum) = -1;
    return;
  }
  if (pfighting (mynum) != -1) {
    bprintf ("Not in a fight!\n");
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Who?\n");
    return;
  }
  if (pl1 == -1) {
    bprintf ("Who?\n");
    return;
  }
  if (ploc (pl1) != ploc (mynum)) {
    bprintf ("They aren't here.\n");
    return;
  }
  if (mynum == pl1)
  {
    if (is_grouped(mynum) != mynum)
      ungroup(mynum);
    bprintf("You run round in circles!\n");
    return;
  }
  if (pl1 != is_grouped(mynum))
    ungroup(mynum);

  sendf (pl1, "%s is following you.\n", see_name(pl1, mynum));
  pfollow(mynum) = pl1;
  bprintf ("Ok\n");
}

void losecom(void) {
  if (pgrouped(mynum) != -1)
    ungroup(mynum);

  if (pfollow(mynum) == -1)   {
    bprintf("You aren't following anyone.\n");
    return;
  }

  sendf(pfollow(mynum), "\001p%s\003 has stopped following you.\n", pname(mynum));
  bprintf("You sneak away from %s!\n", pname(pfollow(mynum)));
  pfollow(mynum) = -1;
}


void
togglecom (int flg, char on[80], char off[80])
{
  Boolean away = (flg == SFL_AWAY);

  if (!ststflg (mynum, flg)) {
    ssetflg (mynum, flg);
    bprintf ("%s\n", on);

    if (away) {
      if( pawaymsg(mynum) )
        send_msg (DEST_ALL, 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
  		"&+G[&+W%s &*has walked away from %s keyboard:&+G]\n&+G[&N%s&+G]\n",
  		pname (mynum), his_or_her (mynum), pawaymsg(mynum));
      else
        send_msg (DEST_ALL, 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
  		"&+G[&+W%s &*has walked away from %s keyboard&+G]\n",
  		pname (mynum), his_or_her (mynum));
    }
  } else {
    sclrflg (mynum, flg);
    bprintf ("%s\n", off);

    if (away)
      send_msg (DEST_ALL, 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
		"&+G[&+W%s &*has returned to %s keyboard&+G]\n",
		pname (mynum), his_or_her (mynum));
  }
}

void beepcom (void)
{
  int a;

  if (brkword () == -1) {
    bprintf ("Who do you want to beep?\n");
    return;
  }
  if ((a = pl1) == -1) {
    bprintf ("I can't seem to find that player.\n");
    return;
  }
  if (a >= max_players) {
    bprintf ("Beep a mobile? Why?\n");
    return;
  }
  if (a == mynum) {
    bprintf ("You want to beep yourself? Ok then!&#&#\n");
    return;
  }
  if (check_forget (a, mynum)) {
    bprintf ("&+W%s &*has forgotten you and will not receive your beeps.\n", pname (a));
    return;
  }
  if (ltstflg (ploc (a), LFL_SOUNDPROOF) && plev(a) < LVL_WIZARD) {
    bprintf ("%s seems to be in a soundproof room.\n", pname (a));
    return;
  }
  bprintf ("Beeping %s.\n", pname (a));
  sendf (a, "\001p%s\003 is trying to get your attention!&#&#\n", pname (mynum));
}

void togglefinger (void)
{
  if (!ststflg (mynum, SFL_NOFINGER)) {
    ssetflg (mynum, SFL_NOFINGER);
    bprintf ("You can no longer be checked on by other users.\n");
  } else {
    sclrflg (mynum, SFL_NOFINGER);
    bprintf ("You can now be checked on by other users.\n");
  }
}

void lastoncom (void) {
  int c, b;
  PERSONA p;

  if (brkword () == -1) {
    togglefinger ();
    return;
  }
  if ((b = fpbn(wordbuf)) != -1) {
    bprintf ("They are currently online.\n");
    return;
  }
  if (!getuaf (wordbuf, &p)) {
    bprintf ("Person not found in system.\n");
    return;
  }
  c = test_bit(p.ublock.bits, mindex, SFL_NOFINGER, SFLAGS) ? 1 : 0;

  if (c && plev (mynum) < LVL_SHALAFI) {
    bprintf ("That player cannot be finger'd.\n");
    return;
  }
  bprintf ("&+wPlayer: &+C%s\n", p.ublock.pname);
  bprintf ("&+wLast Login: &+C%s\n", time2ascii (p.player.last_on));

  if (ptstflg (mynum, PFL_SEEUSER))
    bprintf ("&+WHost/User: &+C%s\n", p.rplr.usrname);
  else if (ptstflg (mynum, PFL_SEEHOST))
    bprintf ("&+wFrom Host: &+C%s\n", p.rplr.hostname);
}

void
setpager (void)
{
  int new;

  if (brkword () == -1) {
    bprintf ("&+wPager is currently set to &+C%d &+wlines.\n", ppager (mynum));
    return;
  }
  new = atoi (wordbuf);

  if (new > 200) {
    bprintf ("Do you really need the pager set that high?\n");
    return;
  }
  if (new < 0) {
    bprintf ("Do you really need the pager set that low?\n");
    return;
  }
  setppager (mynum, new);

  if (new) {
    bprintf ("&+wSetting pager to &+C%d &+wlines.\n", new);
  } else {
    bprintf ("&+wTurning pager &+COff&+w.\n");
  }
}

void
optionscom (void)
{
  char pager[20], chan[20], wimpy[20];

  sprintf (pager, "&+C%d Lines", ppager (mynum));
  sprintf (chan, "&+C%d", pchannel (mynum));
  sprintf (wimpy, "&+C%d", pwimpy (mynum));

  bprintf ("&+WYour current game options:\n");
  bprintf ("&+B---------------------------------------------------------\n");
  bprintf ("&+wColor          %-11s       &+wNewStyle       %-11s\n",
	   ststflg (mynum, SFL_COLOR) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_NEWSTYLE) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wBrief          %-11s       &+wAway           %-11s\n",
	   ststflg (mynum, SFL_BRIEF) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_AWAY) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wBusy           %-11s       &+wNoFight        %-11s\n",
	   ststflg (mynum, SFL_BUSY) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_NOFIGHT) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wAutoExit       %-11s       &+wNoFinger       %-11s\n",
	   ststflg (mynum, SFL_AUTOEXIT) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_NOFINGER) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wNoInventory    %-11s       &+wNoBeep         %-11s\n",
	   ststflg (mynum, SFL_NOINV) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_NOBEEP) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wHearBack       %-11s       &+wNoBlink        %-11s\n",
	   ststflg (mynum, SFL_HEARBACK) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_NOBLINK) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wNoFlag         %-11s       &+wNoOracle       %-11s\n",
	   ststflg (mynum, SFL_NOFLAG) ? "&+CEnabled" : "&+cDisabled",
	   ststflg (mynum, SFL_NOORACLE) ? "&+CEnabled" : "&+cDisabled");
  bprintf ("&+wNoWeather      %-11s       &+wWimpy          %-11s\n",
	   ststflg (mynum, SFL_NOWET) ? "&+CEnabled" : "&+cDisabled",
	   pwimpy (mynum) == 0 ? "&+cDisabled" : wimpy);
  bprintf ("&+wChat Channel   %-11s       &+wPager          %-11s\n",
	   chan, ppager (mynum) == 0 ? "&+cDisabled" : pager);

  if (plev (mynum) >= LVL_WIZARD) {
    bprintf ("&+wQuiet          %-11s       &+wAloof          %-11s\n",
	     ststflg (mynum, SFL_QUIET) ? "&+CEnabled" : "&+cDisabled",
	     ststflg (mynum, SFL_ALOOF) ? "&+CEnabled" : "&+cDisabled");
    bprintf ("&+wNoWish         %-11s       &+wSeeExtended    %-11s\n",
	     ststflg (mynum, SFL_NOWISH) ? "&+CEnabled" : "&+cDisabled",
	     ststflg (mynum, SFL_SEEEXT) ? "&+CEnabled" : "&+cDisabled");
    bprintf ("&+wNoSlain        %-11s       &+wLit            %-11s\n",
	     ststflg (mynum, SFL_NOSLAIN) ? "&+CEnabled" : "&+cDisabled",
	     ststflg (mynum, SFL_LIT) ? "&+CEnabled" : "&+cDisabled");
  }
  if (plev (mynum) >= LVL_GOD) {
    bprintf ("&+wSilent Entry   %-11s       &+wCoding         %-11s\n",
	     ststflg (mynum, SFL_SILENT) ? "&+CEnabled" : "&+cDisabled",
	     ststflg (mynum, SFL_CODING) ? "&+CEnabled" : "&+cDisabled");
    bprintf ("&+wIdle           %-11s       &+wSeeIdle        %-11s\n",
	     ptstflg (mynum, PFL_IDLE) ? "&+CEnabled" : "&+cDisabled",
	     ptstflg (mynum, PFL_SEEIDLE) ? "&+CEnabled" : "&+cDisabled");
    bprintf ("&+wSeeSocket      %-11s\n",
	     ptstflg (mynum, PFL_SEESOCKET) ? "&+CEnabled" : "&+cDisabled");
  }
  bprintf ("&+B---------------------------------------------------------\n");
}

void
klockcom (char *passwd)
{
  char pass[sizeof (cur_player->passwd)];

  if (passwd == NULL) {
    if (phandler (mynum) != get_command) {
      bprintf ("You can only lock the keyboard from the command prompt.\n");
      return;
    }
    bprintf ("Locking Keyboard With Login Password.\001R\003\n");
    strcpy (cur_player->cprompt, "Enter Keyboard Password: ");
    replace_input_handler (klockcom);
  } 
  else {
    my_crypt (pass, passwd);
    if (strcmp (cur_player->passwd, pass) == 0) {
      strcpy(cur_player->cprompt, build_prompt(mynum));
      bprintf ("\nPassword Correct, Keyboard Unlocked.\n\001E\003");
      replace_input_handler (get_command);
      return;
    } 
  }
}

/* Compares two objects that are weapon or armor and tells which object 
 * has the better DP/AC.
 */

void comparecom (void) {
  int a, b;			/* Objects */
  int adata, bdata, data;	/* DP/AC and compared data for object */

  if (plev (mynum) < LVL_WIZARD) {
    if ((a = cantake(ob1, "Compare what?", "What is the first object?")) == -1)
      return; 
    else if ((b = cantake(ob2, "", "What is the second object?")) == -1)
      return;
  } 
  else {
    if ((a = ob1) == -1 && (a = ob2) == -1) {
      bprintf ("Neither object exists!\n");
      return;
    }
    if ((a = ob1) == -1) {
      bprintf ("Object does not exist! (First one)\n");
      return;
    }
    if ((b = ob2) == -1) {
      bprintf ("Object does not exist! (Second one)\n");
      return;
    }
  }

  /* Check to see if objects are weapons, if so compare them */

  if (otstbit (a, OFL_WEAPON) && otstbit (b, OFL_WEAPON)) {
    adata = odamage (a);
    bdata = odamage (b);
    if (adata > bdata) {
      data = adata - bdata;
      bprintf ("The %s (Item %d) is stronger than the %s (Item %d) by %d DP.\n",
	       oname (a), int2idx(a, OBJ), oname (b), int2idx(b, OBJ), data);
    }
    if (adata < bdata) {
      data = bdata - adata;
      bprintf ("The %s (Item %d) is stronger than the %s (Item %d) by %d DP.\n",
	       oname (b), int2idx(b, OBJ), oname (a), int2idx(a, OBJ), data);
    }
    if (adata == bdata) {
      bprintf ("The %s (Item %d) has the same DP has the %s (Item %d).\n",
	       oname (a), int2idx(a, OBJ), oname (b), int2idx(b, OBJ));
    }
  } 
  else if (otstbit (a, OFL_ARMOR) && otstbit (b, OFL_ARMOR)) {
    adata = oarmor (a);
    bdata = oarmor (b);
    if (adata > bdata) {
      data = adata - bdata;
      bprintf ("The %s (Item %d) is stronger than the %s (Item %d) by %d AC.\n",
	       oname (a), int2idx(a, OBJ), oname (b), int2idx(b, OBJ), data);
    }
    if (adata < bdata) {
      data = bdata - adata;
      bprintf ("The %s (Item %d) is stronger than the %s (Item %d) by %d AC.\n",
	       oname (b), int2idx(b, OBJ), oname (a), int2idx(a, OBJ), data);
    }
    if (adata == bdata) {
      bprintf ("The %s (Item %d) has the same AC has the %s (Item %d).\n",
	       oname (a), int2idx(a, OBJ), oname (b), int2idx(b, OBJ));
    }
  } 
  else
    bprintf ("Both objects must be either weapons or armor.\n");
}
