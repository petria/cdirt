
#include <stdlib.h>
#include "kernel.h"
#include "sendsys.h"
#include "magic.h"
#include "objects.h"
#include "bprintf.h"
#include "parse.h"
#include "mobile.h"
#include "mud.h"
#include "uaf.h"
#include "objsys.h"
#include "rooms.h"


void healcom (void) {
  int a;
  Boolean f;
  PERSONA p;

  if (EMPTY (item1)) {
    bprintf ("Heal who?\n");
    return;
  }
  if (!ptstflg (mynum, PFL_HEAL)) {
    bprintf ("The spell fails.\n");
    return;
  }
  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Heal who?\n");
    return;
  }
  if (a == mynum) {
    bprintf ("You feel much better.\n");
    setpstr (mynum, maxstrength (mynum));
    setpmagic (a, maxmagic (a));
    return;
  } 
  else if (a >= max_players) {
    setpstr (a, pstr_reset (a));
  } 
  else if (a >= 0) {
    if (plev (mynum) < LVL_SHALAFI) {
      if (ststflg (a, SFL_NOHEAL)) {
	bprintf ("A magical force prevents you from healing that person.\n");
	return;
      }
      if ((pfighting (a) >= 0) && !ststflg (a, SFL_HEALFIGHT)) {
	bprintf ("You cannot heal a person in a fight.\n");
	return;
      }
    }
    setpstr (a, maxstrength (a));
    setpmagic (a, maxmagic (a));
  } 
  else if (f) {
    bprintf ("%s will feel much better now!\n", p.ublock.pname);
    p.ublock.pstr = pmaxstrength (p.ublock.plev);
    p.player.pmagic = pmaxmagic (p.ublock.plev);
    putuaf (&p);
    return;
  }
  bprintf ("Power radiates from your hands to heal %s.\n", pname (a));
  sendf (a, "%s heals all your wounds.\n", see_name (a, mynum));

  send_msg(DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	    "&+B[&+W\001p%s\003 &*has &+Chealed &+W\001p%s\003&+B]\n",
	    pname (mynum), pname (a));

}

void healallcom (void) {
  int i;
  int j = 0;

  if (!ptstflg (mynum, PFL_HEAL)) {
    bprintf ("The spell fails.\n");
    return;
  }
  if (plev (mynum) <= LVL_ARCHWIZARD && HEALALL_TIME != 0) {
    if (last_healall < global_clock && global_clock < last_healall + HEALALL_TIME) {
      bprintf ("The spell fails.\n");
      return;
    }
  }
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && pfighting (i) < 0 && plev (i) < LVL_WIZARD &&
	((pstr (i) < maxstrength (i) || pmagic (i) < maxmagic (i)) ||
	 (pstr (i) < maxstrength (i) && pmagic (i) < maxmagic (i)))) {

      if (j == 0) {
	bprintf ("You have healed the following players:\n");
	j = 1;
      }
      setpstr (i, maxstrength (i));
      setpmagic (i, maxmagic (i));
      bprintf ("  %s\n", pname (i));
      sendf (i, "%s unleashes a healing spell over the world.\n",
	     see_name (i, mynum));
    }
  }

  if (j == 0) {
    bprintf ("No players needing healing.\n");
  } else {
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	      "&+B[&+CHealall &*by &+W\001p%s\003&+B]\n", pname (mynum));
    last_healall = global_clock;
  }
}

void forcecom (void)
{
  int rme = real_mynum;
  int me = mynum;
  int a;
  char z[MAX_COM_LEN];

  if( plev( mynum ) < LVL_WIZARD ) {
    bprintf( "Mortals can't force other people to do things!!\n" );
    return;
  }

  if ((a = vicf2 (SPELL_REFLECTS, 4)) < 0)
    return;
  if (a >= max_players) {
    bprintf ("You can only force players to do things.\n");
    return;
  }
  if (plev (mynum) < LVL_WIZARD && ploc (mynum) != ploc (a)) {
    bprintf ("There is no %s here.\n", pname (a));
    return;
  }
  getreinput (z);

  if (do_okay (me, a, PFL_NOFORCE)) {

    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
	      "[\001p%s\003 forced \001p%s\003]\n[Force:%s]\n",
	      pname (rme), pname (a), z);

    setup_globals (a);
    bprintf ("%s has forced you to %s\n", see_name (a, me), z);
    cur_player->isforce = True;
    gamecom (z, True);
    cur_player->isforce = False;
    setup_globals (rme);

  } else
    sendf (a, "%s tried to force you to %s\n", see_name (a, me), z);
}

void forceallcom (void)
{
  int i, me = real_mynum;
  char com[MAX_COM_LEN];

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  getreinput (com);

  for (i = 0; i < max_players; ++i) {
    if (see_player (i, mynum) && do_okay (me, i, PFL_NOFORCE) && i != mynum &&
        is_in_game(i) ) {
      setup_globals (i);
      bprintf ("\001p%s\003 has forced you to %s.\n", pname (me), com);
      cur_player->isforce = True;
      gamecom (com, True);
      cur_player->isforce = False;
    }
  }

  setup_globals (me);

  send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
    "&+B[&+CForceall &*by &+W\001p%s\003 &*(%s)&+B]\n", pname (mynum), com);

}

void sumcom (void) {
  int a, me = mynum, rme = real_mynum;
  int c, d, x, obj, i;
  char xx[SETIN_MAX + 200];

  if (brkword () == -1) {
    bprintf ("Summon who?\n");
    return;
  }
  do {
    if ((a = fmbn (wordbuf)) != -1) {
      if (plev (mynum) < LVL_WIZARD) {
	if (a == mynum) {
	  bprintf ("You're already here.\n");
	  return;
	}
        else if (pclass (mynum) == THIEF || pclass (mynum) == WARRIOR) {
          bprintf ("Save that for the magic users.\n");
          return;
        }
	else if (pmagic (mynum) < 1) {
	  bprintf ("You're too weak.\n");
	  return;
	}
	setpmagic (mynum, pmagic (mynum) - 1);

	c = plev (mynum) * 2;


	if (0
#ifdef LOCMIN_BLIZZARD
            || carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE) > -1 
	    || carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE1) > -1 
	    || carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE2) > -1 
#endif
#ifdef LOCMIN_FANTASY
	    || carries_obj_type (mynum, OBJ_FANTASY_MANA) > -1
#endif
          )
	  c += plev (mynum);

	d = randperc ();

	if (ltstflg (ploc (mynum), LFL_ONE_PERSON)) {
	  bprintf ("It's too restricted in here.\n");
	  return;
	}
	if (ltstflg (ploc (mynum), LFL_NO_SUMMON)) {
	  bprintf ("Something prevents your summoning "
		   "from succeeding.\n");
	  return;
	}
	if (ltstflg (ploc (mynum), LFL_ON_WATER)) {
	  bprintf ("The boat is rolling too much.\n");
	  return;
	}
	if (a >= max_players) {
	  bprintf ("You can't summon mobiles.\n");
	  return;
	} 
        else if (c < d) {
	  bprintf ("The spell fails.\n");
	  return;
	}
        if( wears_antimagic( a ) 
#ifdef LOCMIN_TREEHOUSE
	    || wears_obj_type (a, OBJ_TREEHOUSE_AMULET) > -1
#endif
#ifdef LOCMIN_CASTLE
	    || carries_obj_type (a, OBJ_CASTLE_RUNESWORD) > -1
#endif
#ifdef LOCMIN_BLIZZARD
	    || wears_obj_type (a, OBJ_BLIZZARD_TALISMAN) > -1
#endif
#ifdef LOCMIN_ICECAVE
	    || wears_obj_type (a, OBJ_ICECAVE_PENDANT) > -1
#endif
      ) {
	  bprintf ("Something prevents your summoning "
		   "from succeeding.\n");
	  return;
	}
      }	                        			/* end; if not wiz*/
      if (!do_okay (mynum, a, PFL_NOSUMMON)) {
	bprintf ("%s doesn't want to be summoned.\n", pname (a));
	sendf (a, "%s tried to summon you!\n", see_name (a, mynum));
	continue;
      }
      if (a >= num_const_chars && pzone(a) != -1) {
        if (pzone(a) != pzone(mynum)) {
          bprintf("You can't summon a cloned mobile here.\n");
          return;
        }
        else
          ploc_reset(a) = ploc(mynum);
      }

      bprintf ("You cast the summoning...\n");

      if (plev (mynum) < LVL_WIZARD && !the_world->w_tournament) {
        for (i = 0, obj = 0; i < pnumobs (a); i++) {
          obj = pobj_nr (i, a);
          if (!iswornby (obj, a) && !iswielded(obj))
            setcarrf(obj, IN_ROOM);
        }
        if (obj)
          sendf (a, "You drop everything as you are summoned by %s.\n",
            see_name (a, mynum));
      } 
      else if (pvis(a) > plev(mynum))                     /* invis wiz */
        bprintf("You are summoned by someone.\n");
      else
	sendf (a, "%s\n", build_setin (SETIN_SETSUM, xx, players[mynum].setsum, 
          ipname (mynum), NULL, NULL));

      send_msg(sendloc(a), 0, pvis (a), LVL_MAX, a, NOBODY, "%s\n", 
        build_setin (SETIN_SETSUMOUT, xx, players[mynum].setsumout, 
          ipname(mynum), NULL, pname (a)));

      if (a < max_players) {
	send_msg(sendloc(mynum), 0, pvis (a), LVL_MAX, a, NOBODY,
          "%s\n", build_setin (SETIN_SETSUMIN, xx, players[mynum].setsumin, 
             ipname (mynum), NULL, pname (a)));

        send_msg (DEST_ALL, MODE_QUIET, LVL_ARCHWIZARD, LVL_MAX, mynum, a,
	    "&+b[&+B%s &+Whas summoned &+YPLAYER &+B%s &+Wto %s&+b]\n",
	    pname (mynum), pname (a), showname(ploc(mynum)));

	setup_globals (a);
	trapch (ploc (me));
        bprintf("%s", cur_player->cprompt);
	setup_globals (rme);
      } 
      else {
	send_msg (sendloc(mynum), 0, pvis (a), LVL_MAX, a, NOBODY, "%s\n", 
          build_setin (SETIN_SETSUMIN, xx, players[mynum].setsumin, 
            ipname(mynum), NULL, pname (a)));

        send_msg (DEST_ALL, MODE_QUIET, LVL_ARCHWIZARD, LVL_MAX, mynum, a,
	  "&+b[&+B%s &+Whas summoned &+YMOBILE &+B%s&+W to %s&+b]\n",
	    pname(mynum), pname(a), showname(ploc(mynum)));

	setploc (a, ploc (mynum));
      }
      continue;
    }
    if ((a = fobn (wordbuf)) != -1) {
      x = a;
      if (!ptstflg (mynum, PFL_SUMOBJ)) {
	bprintf ("You can only summon people.\n");
	continue;
      }
      else if (ovis (x) > plev (mynum)) {
	bprintf ("Who or what is %s?\n", wordbuf);
        continue;
      }
      else if (a == OBJ_START_PIT || a == OBJ_START_CHURCH_PIT) {
          bprintf ("You can't summon the pit!\n");
          return;
      }
      else if (x >= num_const_obs && ozone(a) != -1) {
        if (ozone(x) != pzone(mynum)) {
          bprintf("You can't summon a cloned object here.\n");
          continue;
        }
        else
          oloc_reset(x) = ploc(mynum);
      }

      while (ocarrf (x) == IN_CONTAINER)
	x = oloc (x);

      if (ocarrf (x) >= CARRIED_BY)
	x = ploc (oloc (x));
      else
	x = oloc (x);

      send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	   "%s fetches something from another dimension.\n", pname (mynum));

      sendl (x, "The %s vanishes!\n", oname (a));

      bprintf ("The %s flies into your hand.\nIt was:", oname (a));
      desrm (oloc (a), ocarrf (a));
      send_msg (DEST_ALL, MODE_QUIET, LVL_ARCHWIZARD, LVL_MAX, mynum, a,
	"&+b[&+B%s &+Whas summoned &+YOBJECT &+B%s&+W to %s&+b]\n",
	  pname (mynum), oname (a), showname(ploc(mynum)));
      setoloc (a, mynum, CARRIED_BY);
      continue;
    } 
    else
      bprintf ("Who or what is %s?\n", wordbuf);
  }
  while (plev (mynum) >= LVL_WIZARD && brkword () != -1);
}


static void vis_proc (int new)
{
  char xx[SETIN_MAX + 200];

  int oldvis = pvis (mynum);

  setpvis (mynum, new);

  bprintf ("Ok\n");

  if (new < oldvis) {
    send_msg(sendloc(mynum), 0, new, oldvis, mynum, NOBODY,
    "%s\n", build_setin (SETIN_SETVIN, xx, cur_player->setvin, pname (mynum), NULL, NULL));
  } else {
    send_msg(sendloc(mynum), 0, oldvis, new, mynum, NOBODY,
	      "%s\n", build_setin (SETIN_SETVOUT, xx, cur_player->setvout, pname (mynum), NULL, NULL));
  }
}

void viscom (void)
{
  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("You can't do that sort of thing at will, you know.\n");
    return;
  }
  if (pvis (mynum) == 0) {
    bprintf ("You're already visible.\n");
    return;
  }
  vis_proc (0);
}

void inviscom ()
{
  int x, y;

  switch (wlevel (plev (mynum))) {
  case LEV_CREATOR:
    x = LVL_MAX;
    break;
  case LEV_GOD:
    x = LVL_CREATOR;
    break;
  case LEV_SHALAFI:
    x = LVL_GOD;
    break;
  case LEV_DEMI:
    x = LVL_SHALAFI;
    break;
  case LEV_ARCHWIZARD:
    x = LVL_DEMI;
    break;
  case LEV_ISTARI:
    x = LVL_ARCHWIZARD;
    break;
  case LEV_WIZARD:
  case LEV_EMERITI:
  case LEV_APPRENTICE:
    x = LVL_ISTARI;
    break;

  default:
    bprintf ("You can't turn invisible at will, you know.\n");
    return;
  }

  if (brkword () == -1 || (y = atoi (wordbuf)) < 0 || y > x)
    y = x;

  if (pvis (mynum) == y) {
    if (pvis (mynum) == 0)
      bprintf ("You're already visible.\n");
    else
      bprintf ("You're already invisible (Level %d).\n", pvis (mynum));
    return;
  }
  vis_proc (y);
}

void resurcom (void) {
  int a;

  if (!ptstflg (mynum, PFL_RES)) {
    erreval ();
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Resurrect what?\n");
    return;
  }
  if ((a = fobn (item1)) == -1) {
    if ((a = fmbn(item1)) == -1 || a < max_players) {
      bprintf ("You can only resurrect objects and mobiles.\n");
      return;
    }
    if (!EMPTY (pname (a))) {
      if (pfighting (a) != -1) {
	bprintf ("%s is fighting at the moment.\n", pname (a));
	return;
      } 
      else if (pstr (a) >= 0) {
	bprintf ("%s is resurrected.\n", pname (a));
	setpstr (a, pstr_reset (a));
	return;
      }
    }
    if (ltstflg (ploc (mynum), LFL_ONE_PERSON)) {
      bprintf ("This room is too small.\n");
      return;
    }
    reset_mobile (a);

    setploc (a, ploc (mynum));

    sendl(ploc(mynum), "%s suddenly appears.\n", pname (a));
    return;
  }
  if (ospare (a) != -1) {
    bprintf ("It already exists.\n");
    return;
  }
  oclrbit (a, OFL_DESTROYED);
  setoloc (a, ploc (mynum), IN_ROOM);

  sendl(ploc (mynum), "The %s suddenly appears.\n", oname (a));
}

void curecom (void)
{
  int a;

  if (EMPTY (item1)) {
    bprintf ("Cure who?\n");
    return;
  }
  else if (pclass(mynum) != PRIEST && plev(mynum) < LVL_WIZARD) {
    bprintf ("But you aren't wise enough! Find a priest.\n");
    return;
  }
  if ((a = vicf2 (SPELL_PEACEFUL, 8)) >= 0) {
    sclrflg (a, SFL_DUMB);
    sclrflg (a, SFL_CRIPPLED);
    sclrflg (a, SFL_BLIND);
    sclrflg (a, SFL_DEAF);
    sclrflg (a, SFL_BABEL);
    sclrflg (a, SFL_ISPIG);
    sendf (a, "All your ailments have been cured.\n");
    if (a != mynum)
      bprintf ("With a laying on of hands, you miraculously cure %s.\n",
	       pname (a));
  }
}

void wherecom (void) {
  int cha, rnd, num_obj_found = 0, num_chars_found = 0;
  char buf[80];

  if (plev (mynum) < LVL_WIZARD && pmagic (mynum) < 2) {
    bprintf ("&+YYou're too weak to cast this spell.\n");
    return;
  }
  if (ltstflg (ploc (mynum), LFL_NO_MAGIC) && plev (mynum) < LVL_WIZARD) {
    bprintf ("&+RSomething about this location has drained your mana.\n");
    return;
  }
  if (plev (mynum) < LVL_WIZARD)
    setpmagic (mynum, pmagic (mynum) - 2);

  rnd = randperc ();
  cha = 6 * plev (mynum);

  if (0 
    #ifdef LOCMIN_BLIZZARD
      || carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE) > -1
      || carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE1) > -1
      || carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE2) > -1
    #endif
    #ifdef LOCMIN_FANTASY
      || carries_obj_type (mynum, OBJ_FANTASY_MANA) > -1
    #endif
      )
    cha = 100;

  if (rnd > cha) {
    bprintf ("Your spell fails.\n");
    return;
  }
  if (!item1[0]) {
    bprintf ("What's that?\n");
    return;
  }
  for (cha = 0; cha < numobs; cha++) {

    if (cha == num_const_obs && plev (mynum) < LVL_WIZARD)
      break;

    if (EQ (oname (cha), item1)
        || (plev (mynum) >= LVL_WIZARD && oaltname(cha) && 
           EQ(oaltname(cha), item1))) {
      if (ovis (cha) <= plev (mynum)) {
        num_obj_found++;
        if (plev (mynum) >= LVL_WIZARD)
          bprintf ("&+B[&+c%4d&+B] ", int2idx(cha, OBJ));
        bprintf ("&+y%20.20s &N- ", oname (cha));
        if (plev (mynum) < LVL_WIZARD && ospare (cha) == -1)
          bprintf ("&+gNowhere.\n");
        else
          desloc (oloc (cha), ocarrf (cha));
      }
    }
  }

  for (cha = 0; cha < numchars; cha++) {

    if (cha == num_const_chars && plev (mynum) < LVL_WIZARD)
      break;

    if (EQ (xname (pname (cha)), item1) && (pvis (cha) <= plev (mynum))) {

      num_chars_found++;

      if (plev (mynum) >= LVL_WIZARD)
        bprintf ("&+B[&+c%4d&+B] ", int2idx(cha, MOB));

      if( alive( cha ) != -1 )
        strcpy(buf, pname(cha));
      else
        sprintf( buf, "%s's corpse", pname(cha));

      bprintf ("&+M%20.20s &N- ", buf);
      desloc (ploc (cha), 0);
    }
  }

  if (num_obj_found == 0 && num_chars_found == 0)
    bprintf ("I don't know what that is.\n");
}

void posecom (void)
{
  char x[128];
  int n;
  int m = 0;

  char *POSE[] =
    {"gestures", "fireball", "hamster", "sizzle", "crackle", TABLE_END};

  if (pclass(mynum) != MAGE) {
    bprintf("You have no knowledge when it comes to magic.\n");
    return;
  }
  else if (plev (mynum) < 10) {
    bprintf ("You're not up to this yet.\n");
    return;
  }
  if (brkword () != -1) {
    if (plev (mynum) < LVL_WIZARD) {
      bprintf ("Usage: POSE\n");
      return;
    }
    if (((n = atoi (wordbuf)) > 5 || n < 1) &&
        (m = tlookup (wordbuf, POSE)) == -1) {
      bprintf ("Usage: POSE <gestures/fireball/hamster/sizzle/crackle>\n");
      return;
    }
    if (m != -1)
      n = m;
    else
      --n;
  } else
    n = randperc () % 5;

  switch (n) {
  case 0:
    sprintf (x, "\001s%%s\003%%s raises %s arms in mighty magical "
             "invocations.\n\003", his_or_her (mynum));
    sillycom (x);
    bprintf ("You make mighty magical gestures.\n");
    break;
  case 1:
    sillycom ("\001s%s\003%s throws out one arm and sends a huge bolt of "
              "fire high into the sky.\n\003");
    bprintf ("You toss a fireball high into the sky.\n");
    broad ("\001cA massive ball of fire explodes high up in the sky.\n\003");
    break;
  case 2:
    sillycom ("\001s%s\003%s turns casually into a hamster before resuming "
              "normal shape.\n\003");
    bprintf ("You casually turn into a hamster before resuming normal "
             "shape.\n");
    break;
  case 3:
    sillycom ("\001s%s\003%s starts sizzling with magical energy.\n\003");
    bprintf ("You sizzle with magical energy.\n");
    break;
  case 4:
    sillycom ("\001s%s\003%s begins to crackle with magical fire.\n\003");
    bprintf ("You crackle with magical fire.\n");
    break;
  }
}
