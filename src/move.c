
/* Movement routines (north, south, go, etc.) */

#include "kernel.h"
#include "parse.h"
#include "objects.h"
#include "mobiles.h"
#include "locations.h"
#include "levels.h"
#include "sendsys.h"
#include "move.h"
#include "wizard.h"
#include "group.h"
#include "bprintf.h"
#include "objsys.h"
#include "mobile.h"
#include "rooms.h"
#include "fight.h"
#include "uaf.h"
#include "verbs.h"

char *mvitals[] = {
  "bounds",	"runs",		"wanders",	"hobbles",	"crawls"
};

int exitnum[] = {
  1, 2, 3, 4, 5, 6,
  1, 2, 3, 4, 5, 6
};

int verbdirs[] = {
  VERB_NORTH,
  VERB_EAST,
  VERB_SOUTH,
  VERB_WEST,
  VERB_UP,
  VERB_DOWN
};

int dodirn (Boolean silent, int vb) {
  int i, pc, n, lloc;
  int newch, drnum, droff;

  if (pfighting (mynum) >= 0) {
    bprintf ("You can't just stroll out of a fight!\n");
    bprintf ("If you wish to leave, you must FLEE in a direction.\n");
    return -1;
  }
#ifdef LOCMIN_CATACOMB
  if (iscarrby (OBJ_CATACOMB_CUPSERAPH, mynum)
      && (i = alive ((max_players + MOB_CATACOMB_SERAPH))) != -1
      && ploc (i) == ploc (mynum)) {
    bprintf ("The Seraph says 'I cannot allow you to leave this place "
	     "with the Holy Relic.'\n");
    return -1;
  }
#endif
#ifdef LOCMIN_CASTLE
  if (iscarrby (OBJ_CASTLE_RUNESWORD, mynum)
      && ploc ((max_players + MOB_CASTLE_GOLEM)) == ploc (mynum)
      && alive((max_players + MOB_CASTLE_GOLEM)) != -1
      && !EMPTY (pname ((max_players + MOB_CASTLE_GOLEM)))) {
    bprintf ("\001cThe Golem\003 bars the doorway!\n");
    return -1;
  }
#endif

  for (i = 0, n = -1 ; i < NEXITS ; i++)
    if (verbdirs[i] == vb)
      n = i;

  if (n == -1) {
    mudlog("ERROR: in dodirn(), given invalid location.\n");
    return -1;
  }

  if (chksitting ())
    return -1;

  /* these are invisible exits (non-obvious) */

#ifdef LOCMIN_BLIZZARD
  if (ploc(mynum) == LOC_BLIZZARD_DAMP && vb == VERB_DOWN) {
    if (state (OBJ_BLIZZARD_SLIME_PIT) != 0) {
      bprintf ("That doesn't look like a very good idea!\n");
      return -1;
    }
    else
      newch = LOC_BLIZZARD_SLIME;
  }
#endif

#ifdef LOCMIN_CATACOMB
  else if (ploc(mynum) == LOC_CATACOMB_SLEDGE2 && vb == VERB_SOUTH)
    newch = LOC_CATACOMB_CHAMBER;
  else if (ploc(mynum) == LOC_CATACOMB_NLEDGE2 && vb == VERB_NORTH)
    newch = LOC_CATACOMB_HALL;
#endif

  else switch (newch = getexit (ploc (mynum), n)) {
 
#if defined(LOCMIN_HEAVEN) && defined(LOCMIN_VILLAGE)
  case LOC_HEAVEN_HEAVEN36:
  case LOC_HEAVEN_HEAVEN27:
  case LOC_HEAVEN_HEAVEN16:
  case LOC_HEAVEN_HEAVEN17:
  case LOC_HEAVEN_HEAVEN18:
  case LOC_HEAVEN_HEAVEN14:
  case LOC_HEAVEN_HEAVEN15:
      newch = ploc(mynum);
      bprintf("You faintly hear voices, then feel an irresistable tugging.\n");
      setploc(mynum,LOC_VILLAGE_HOSPITAL);
      lookin(ploc(mynum),0);
      bprintf("A nurse tells you that they almost didn't pull you through.\n");
      send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, NOBODY, NOBODY,
               "%s has left.\n",pname(mynum));
      return -1;
      break;
#endif

  default:

    if (newch >= DOOR) {
      drnum = newch - DOOR;
      droff = olinked (drnum);	            /* other door side */
      if (state (drnum)) {
	if (!EQ (oname (drnum), "door") || isdark ()
	    || !(olongt (drnum, state (drnum))))
	  bprintf ("You can't go that way.\n");
	else
	  bprintf ("The door is closed.\n");
	return -1;
      }
      newch = obj_loc (droff);
    }
    break;
  }

  if (!exists (newch)) {
    bprintf ("You can't go that way.\n");
    return -1;
  }
  else if (chkroom (newch, mynum) && !ltstflg (newch, LFL_MOVE)) {
    bprintf ("A magical force prevents you from entering that room.\n");
    return -1;
  }
  else if (ltstflg (newch, LFL_PRIVATE) || ltstflg (newch, LFL_ONE_PERSON)) {
    pc = 0;
    for (i = 0; i < max_players; i++)
      if (ploc (i) == newch && is_in_game(i))
	pc++;
    if (pc > (ltstflg (newch, LFL_PRIVATE) ? 1 : 0)) {
      bprintf ("I'm sorry, that room is currently full.\n");
      return -1;
    }
  }

  if (ltstflg(newch, LFL_PKZONE) && !ltstflg(newch, LFL_PKZONE))
    bprintf("Warning: You have just entered a Player Killing Area\n");

  if (ltstflg (newch, LFL_NO_SNOOP) && snooped(mynum))
    for (i = 0 ; i < max_players ; i++)
      if (snooptarget(i) == mynum)
	snoop_off(i);

#include "dodirn.h"

  if (n == 0) {
#ifdef LOCMIN_ORCHOLD
    if (ploc (mynum) == LOC_ORCHOLD_DAMP) {
      bprintf ("You fall through a trap door, plummeting to the ground below!\n");
      bprintf ("Dazed, you pick yourself up from the floor to find yourself...\n");
      setploc (mynum, LOC_ORCHOLD_TRAP);
      lookin (ploc (mynum), 0);
      return -1;
    }
#endif
  }

#ifdef LOCMIN_RUINS
  if (newch == LOC_RUINS_SINKHOLE) {
    trapch (LOC_RUINS_SINKHOLE);
    bprintf ("You sink into the goo and find yourself in a lower level!\n");
    setploc (mynum, LOC_RUINS_GOOEY);
    trapch (ploc (mynum));
    return -1;
  }
  if (newch == LOC_RUINS_TELEPORT) {
    trapch (LOC_RUINS_TELEPORT);
    bprintf ("You open your eyes and find yourself in a different room..\n");
    setploc (mynum, LOC_RUINS_TELEPORTEND);
    trapch (ploc (mynum));
    return -1;
  }
#endif

#ifdef LOCMIN_FANTASY
  if (newch == LOC_FANTASY_GAIA1) {
    if ((!iswornby (OBJ_FANTASY_SILVERARMOR, mynum)
	 && !iscarrby (OBJ_FANTASY_SILVERSWORD, mynum))) {
      bprintf ("The Gaia Cave will not let you enter..\n");
      return -1;
    }
    bprintf ("The Gaia Cave seems to like the silver armor "
	     "and sword, you\nare allowed to pass.\n\n");
  }
  if (newch == LOC_FANTASY_AIRSHIP1) {
    bprintf ("The airship begins to take off as you fly over the world..\n");
    setploc (mynum, LOC_FANTASY_CASTLE1);
    trapch (ploc (mynum));
    return -1;
  }
  if (newch == LOC_FANTASY_AIRSHIP2) {
    bprintf ("The airship begins to take off as you fly over the world..\n");
    setploc (mynum, LOC_FANTASY_DFOREST18);
    trapch (ploc (mynum));
    return -1;
  }
#endif

#ifdef LOCMIN_BLIZZARD
  if (newch == LOC_BLIZZARD_LAVA_PATH2) {
    if (!iswornby (OBJ_BLIZZARD_SHIELD1, mynum)
	&& !iswornby (OBJ_BLIZZARD_SHIELD2, mynum)

#ifdef LOCMIN_TREEHOUSE
	&& !iswornby (OBJ_TREEHOUSE_SHIELD, mynum)
#endif

#ifdef LOCMIN_CATACOMB
	&& !iswornby (OBJ_CATACOMB_SHIELD, mynum)
#endif

#ifdef LOCMIN_EFOREST
	&& !iswornby (OBJ_EFOREST_SHIELD, mynum)
#endif
 ) {
      bprintf ("The intense heat drives you back.\n");
      return -1;
    }
    bprintf ("The shield protects you from the worst of the lava's heat.\n");
  }
#endif /* LOCMIN_BLIZZARD */
 
  if (n == EX_NORTH) {
    for (i = max_players; (!mtstflg (i, MFL_BAR_N) || ploc (i) != ploc (mynum)
			   || alive (i) == -1) && i < numchars; i++) ;
    if (mtstflg (i, MFL_BAR_N) && alive (i) != -1 && ploc (i) == ploc (mynum)
	&& plev (mynum) < LVL_WIZARD) {
      bprintf ("\001p%s\003 says 'None shall pass!'\n", pname (i));
      return -1;
    }
#ifdef LOCMIN_EFOREST
    if (iscarrby (OBJ_EFOREST_HOPE, mynum) && ploc (mynum) == LOC_EFOREST_STONE) {
      bprintf ("A mysterious force prevents you from going that way.\n");
      return -1;
    }
#endif
  }
  if (n == EX_WEST) {

    for (i = max_players; (!mtstflg (i, MFL_BAR_W) || ploc (i) != ploc (mynum)
			   || alive (i) == -1) && i < numchars; i++) ;
    if (mtstflg (i, MFL_BAR_W) && alive (i) != -1 && ploc (i) == ploc (mynum)
	&& plev (mynum) < LVL_WIZARD) {
      bprintf ("\001p%s\003 gives a warning growl.\n", pname (i));
      bprintf ("\001p%s\003 won't let you go West!\n", pname (i));
      return -1;
    }
  }
  if ((n == 0) || (n == 1) || (n == 2) || (n == 5)) {
#ifdef LOCMIN_EFOREST
    if ((i = alive ((max_players + MOB_EFOREST_ASMADEUS))) != -1
	&& plev (mynum) < LVL_WIZARD
	&& ploc (i) == ploc (mynum)) {
      bprintf ("\001pAsmadeus\003 refuses to let you enter his museum.\n");
      return -1;
    }
#endif
  }
  if (n == EX_DOWN) {		/* can't go down unless empty-handed */
    if ((0
#ifdef LOCMIN_CATACOMB
          || ploc (mynum) == LOC_CATACOMB_BEGGAR
#endif
#ifdef LOCMIN_VALLEY
          || ploc (mynum) == LOC_VALLEY_FALLS
#endif
        ) && gotanything (mynum)) {

      if (False);
#ifdef LOCMIN_CATACOMB
      else if (ploc (mynum) == LOC_CATACOMB_BEGGAR) {
	bprintf ("A mysterious force blocks your passage.\n");
	if (ploc ((max_players + MOB_CATACOMB_BEGGAR)) == ploc (mynum))
	  sendl(ploc (mynum),
		 "%s says 'To continue on, you must forego all worldly "
	      "possessions.'\n", pname (max_players + MOB_CATACOMB_BEGGAR));
	return -1;
      }
#endif
      else {
	bprintf ("The steep and slippery sides of the pool make it "
		 "impossible to climb down\nwithout dropping everything "
		 "first.\n");
	return -1;
      }
    }
    for (i = max_players; (!mtstflg (i, MFL_BAR_D) || ploc (i) != ploc (mynum)
			   || alive (i) == -1) && i < numchars; i++) ;
    if (mtstflg (i, MFL_BAR_D) && alive (i) != -1 && ploc (i) == ploc (mynum)
	&& plev (mynum) < LVL_WIZARD) {
      bprintf ("\001p%s\003 refuses to let you go Down!\n", pname (i));
      return -1;
    }
  }
  if (n == EX_UP) {
    for (i = max_players; (!mtstflg (i, MFL_BAR_U) || ploc (i) != ploc (mynum)
			   || alive (i) == -1) && i < numchars; i++) ;
    if (mtstflg (i, MFL_BAR_U) && alive (i) != -1 && ploc (i) == ploc (mynum)
	&& plev (mynum) < LVL_WIZARD) {
      bprintf ("\001p%s\003 blocks your way up!\n", pname (i));
      return -1;
    }
#ifdef LOCMIN_ANCIENT
    if ((i = ploc (mynum)) == ploc ((max_players + MOB_ANCIENT_RATTLESNAKE))
	&& alive ((max_players + MOB_ANCIENT_RATTLESNAKE)) != -1
	&& (!ishere (OBJ_ANCIENT_CHAIN) || !ishere (OBJ_ANCIENT_RBLOCK) ||
	    !ishere (OBJ_ANCIENT_RCOINS) || !ishere (OBJ_ANCIENT_RPLATE))) {
      hit_player (max_players + MOB_ANCIENT_RATTLESNAKE, mynum, -1);
      return -1;
    }
#endif
  }
  if (n == EX_SOUTH) {

#ifdef LOCMIN_BLIZZARD
    if ((i = alive ((max_players + MOB_BLIZZARD_FIGURE))) != mynum && i != -1
	&& ploc (i) == ploc (mynum) && !iswornby (OBJ_BLIZZARD_BLACKROBE, mynum)) {
      bprintf ("\001pThe Figure\003 holds you back!\n");
      bprintf ("\001pThe Figure\003 says 'Only true sorcerors may pass.'\n");
      return -1;
    }
#endif
    for (i = max_players; (!mtstflg (i, MFL_BAR_S) || ploc (i) != ploc (mynum)
			   || alive (i) == -1) && i < numchars; i++) ;
    if (mtstflg (i, MFL_BAR_S) && alive (i) != -1 && ploc (i) == ploc (mynum)
	&& plev (mynum) < LVL_WIZARD) {
      bprintf ("\001p%s\003 holds you back!\n", pname (i));
      return -1;
    }
  }
  if (n == EX_EAST) {
#ifdef LOCMIN_OAKTREE
    if ((i = alive ((max_players + MOB_OAKTREE_VIOLA))) != mynum && i != -1
	&& ploc (i) == ploc (mynum) &&
	carries_obj_type (i, OBJ_OAKTREE_FAN) == -1) {
      bprintf ("\001pViola\003 says 'How dare you come to our land!  "
	       "Leave at once!'\n");
      return -1;
    }
#endif
    for (i = max_players; (!mtstflg (i, MFL_BAR_E) || ploc (i) != ploc (mynum)
			   || alive (i) == -1) && i < numchars; i++) ;
    if (mtstflg (i, MFL_BAR_E) && alive (i) != -1 && ploc (i) == ploc (mynum)
	&& plev (mynum) < LVL_WIZARD) {
      bprintf ("\001p%s\003 won't let you go East!\n", pname (i));
      return -1;
    }
  }

#ifdef LOCMIN_RAINFOREST
  if (ltstflg(newch, LFL_ON_WATER))  {
    if( plev(mynum) < LVL_WIZARD && 
      ( ploc(mynum) >= LOCMAX_RAINFOREST && ploc(mynum) < LOCMIN_RAINFOREST ) )
      {
      if(!carries_obj_type(mynum, OBJ_RAINFOREST_RAFT)) {
        if( carries_boat( mynum ) )
          bprintf( "Your craft is unsuitable for a river journey such as this.\n" );
        else
          bprintf( "You will need a sturdy raft to travel on the river.\n");
        return -1;
      }
    }
  }
#endif

  if (ltstflg (newch, LFL_ON_WATER)) {
    if (plev (mynum) < LVL_WIZARD && !carries_boat (mynum)) {
      bprintf ("You need a boat to go to sea!\n");
      return -1;
    }
  }
  if (ltstflg (newch, LFL_IN_WATER) && 
      (plev (mynum) < LVL_WIZARD)
#ifdef LOCMIN_EVOLUTION
      && !iscarrby(OBJ_EVOLUTION_AQUALUNG, mynum) 
#endif 
#ifdef LOCMIN_TREEHOUSE
      && !iscarrby(OBJ_TREEHOUSE_AMULET, mynum) 
#endif
#ifdef LOCMIN_BLIZZARD
      && !iscarrby(OBJ_BLIZZARD_TALISMAN, mynum) 
#endif
#ifdef LOCMIN_LOLIGO
      && !iscarrby( OBJ_LOLIGO_SUIT, mynum)
#endif
      ) {
      bprintf ("You'd surely drown!\n");
      return -1;
    }

#ifdef LOCMIN_OAKTREE
  if (n == EX_DOWN && ploc (mynum) == LOC_OAKTREE_LANDING) {
    bprintf ("You slide down the banister.  Wheee!\n");

    send_msg (sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	      "%s slides down the banister shouting 'Yippeee...'\n",
	      pname (mynum));

    send_msg (int2idx(newch, LOC), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	      "%s slides down the banister and lands at your feet.\n",
	      pname (mynum));

    if (oloc (OBJ_OAKTREE_MARBLEBUST) == newch) {
      bprintf ("On your way down, you smash a valuable bust.\n");

      send_msg (int2idx(newch, LOC), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
		"%s smashed a valuable bust on %s way down.\n",
		pname (mynum), his_or_her (mynum));

      destroy (OBJ_OAKTREE_MARBLEBUST);
      create (OBJ_OAKTREE_BUSTBROKEN);
    }
  } 
#endif /* LOCMIN_OAKTREE */

  if (mynum < max_players && !silent)
    move_msg(newch, n);

  lloc = ploc(mynum);
  setpfighting(mynum, -1);
  trapch(newch);
  if (is_grouped(mynum) != -1)
    check_group_leader(lloc, n);
  else
    do_follow();
  return 0;
}

void move_msg(int newch, int n) {
  char block[SETIN_MAX + 200];
  char buf[100];
  int m;

  if (pstr(mynum) > pmaxstrength(mynum) * 4/5) 
    m = 0;
  else if (pstr(mynum) > pmaxstrength(mynum) * 3/5) 
    m = 1;
  else if (pstr(mynum) > pmaxstrength(mynum) * 2/5) 
    m = 2;
  else if (pstr(mynum) > pmaxstrength(mynum) * 1/5) 
    m = 3;
  else 
    m = 4;

  if (pwpn(mynum) != -1 && oloc(pwpn(mynum)) == mynum)
    sprintf(buf, ", wielding %s &+Y%s&N", psex(mynum) ? "her" : "his",
      oname(pwpn(mynum)));
  else
    *buf = 0;

  if (!cur_player->setout) 
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
      "&+M\001p%s\003&N %s to %s%s.\n", pname (mynum), 
        mvitals[m], exittxt[n], buf);
  else
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY, "%s\n",
      build_setin (SETIN_SETOUT, block, cur_player->setout, 
		   pname(mynum), exittxt[n], NULL));

  if (!cur_player->setin)
    send_msg(int2idx(newch, LOC), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
      "&+M\001p%s\003&N %s in from %s%s.\n", pname (mynum),
        mvitals[m],entertxt[n], buf);
  else     
    send_msg(int2idx(newch, LOC), 0, pvis (mynum), LVL_MAX, mynum,NOBODY,"%s\n",
      build_setin(SETIN_SETIN, block, cur_player->setin, 
		  pname(mynum), NULL, NULL));
} 

int dogocom (Boolean silent) {
  int a;

  a = (brkword () == -1) ? get_rand_exit_dir (ploc (mynum)) : 
        chklist (wordbuf, exits, exitnum) - 1;

  if (!has_foot(mynum) && has_hand(mynum)) {
    bprintf("You're missing legs & feet.  You must CRAWL in adirection\n");
    return -1;
  }
  if ((!has_foot(mynum) && !has_hand(mynum)) || 
     ststflg(mynum, SFL_CRIPPLED)) {
    bprintf("You're totally crippled.  You can't go anywhere.\n");
    return -1;
  }

  if (a < 0 || a >= NEXITS) {
    bprintf ("Go where?\n");
    return -1;
  }
  return dodirn (silent, a + 2);
}
