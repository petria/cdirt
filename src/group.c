#include "kernel.h"
#include "mudmacros.h"
#include "exits.h"
#include "quests.h"
#include "levels.h"
#include "locations.h"
#include "timing.h"
#include "sendsys.h"
#include "objects.h"
#include "mobiles.h"

#include "mobile.h"
#include "fight.h"
#include "group.h"
#include "bprintf.h"
#include "log.h"
#include "mud.h"
#include "move.h"

/*
**  report tells you and any player in same group as you your score and 
**  strength. Optional, it may also tell all those in same room as you too.
**  See group.h
*/
void reportcom()
{
  int i;
  char buf[80];
  
  if (aliased(real_mynum)) {
    bprintf("Not while aliased.\n");
    return;
  }
  else if (pgrouped(mynum) == -1) {
    bprintf("You're not part of any group.\n");
    return;
  }

  sprintf (buf, "&+Ystrength: %3d/%-3d, mana: %2d/%-2d, score: %d&N",
	   pstr(mynum), maxstrength(mynum), pmagic(mynum),
           maxmagic(mynum), pscore(mynum));
  bprintf ("You report '%s'\n",buf);

#if (REPORT_WHO >= 1)
  send_msg (sendloc(mynum), LOC), MODE_NODEAF, LVL_MIN, LVL_MAX,
            mynum, NOBODY,
	    "\001p%s\003 reports '%s'\n", pname(mynum), buf);
#endif

#if (REPORT_WHO <= 1)
  for (i = 0; i < max_players; i++) {
    if (is_in_game(i) && (pgrouped(i) == pgrouped(mynum)) && (i != mynum)) {

  #if (REPORT_WHO == 1)
      if (ploc(i) != ploc(mynum))
  #endif

  #if (REPORT_WHO == 0)
      if (ploc(i) == (ploc(mynum))
  #endif
        sendf(i, "%s reports '%s'\n", pname(mynum), buf);
    }
  }
#endif
  return;
}

/*
**  Do not know if this is really useful to awiz+, but is for debugging.
**  May choose to let wiz's use this, but the undefine SRGD. That info 
**  will only confuse anyone who don't know what it is.
*/
void glist()
{
  int i,j,k;
  int groups[max_players*2]; /* worst situation is max_player groups of 1     */
                             /* player each requiring extra 32 nums to sep    */

  if(plev(mynum) < LVL_ARCHWIZARD)/* Min level can use this. Suggest LVL_AWIZ */
  {
    bprintf("Pardon?\n");
    return;
  }

#ifdef SHOW_RAW_GROUP_DATA
  bprintf("The data format is\n [player_slot_number(i),rplrs[i].grouped,"
          "rplrs[i].i_follow].\n");
#endif

  k = 0;
  for(i = 0; i < max_players; i++) {

#ifdef SHOW_RAW_GROUP_DATA
    if ((i % 4) == 0) /* May be raw debugging info, but no need to be sloppy */
      bprintf("\n");

    bprintf("[%3d, %3d, %3d]   ", i, pgrouped(i), pfollow(i));
#endif

    if (pgrouped(i) == i) {
      groups[k++] = -1;
      groups[k++] = i;

      for (j = 0; j < max_players; j++)
        if (is_in_game(j) && (i != j) && (pgrouped(j) == i))
          groups[k++] = j;
    }
  }
  if (k < 2) /* smallest possible if any group existed */
  {
    bprintf("\n\tThe are No players in the game grouped.\n");
    return;
  }
  
  j = 1;
  for (i = 0; i < k; i++) {
    if(groups[i] == -1) {
      bprintf("\n&+gGROUP %d: Leader = %s\n", j, pname(groups[i+1]));
      j++;
    }
    else
      bprintf("\t%s ",pname(groups[i]));
  }
  bprintf("\n\n\tThere are %d groups in the game.\n",(j-1));
  return;
}
          

/*
**  This takes care of grouping players and ungrouping them when requested.
**  (It will toggle players grouped status, but calls ungroup() to actually
**  do the ungrouping. If group is called from the mud with no argument then
**  it will display the details of the group to which ** mynum ** currently
**  belongs. Note also that only a group leader can group others and they must
**  follow them first.
*/
void group()
{
  int i;
    
  if (pfighting(mynum) != -1) 
  {
    bprintf("Not in a fight!\n");
    return;
  }
  if (EMPTY(item1)) /* We want to see who is in our group */
  {
    if (pgrouped(mynum) == -1) {
      bprintf("You are not grouped.\n");
      return;
    }
    bprintf("%-*s [cur_str/max_str]  [Vis]    [Level]  [Score]\n",PNAME_LEN + 3,
      "NAME");

    for (i = 0; i < max_players; i++)
      if (is_in_game(i) && pgrouped(i) == pgrouped(mynum))
        bprintf("%-*s [%7d/%7d]  [%5d]  [%5d]  [%d] %s\n",PNAME_LEN+3, pname(i),
          pstr(i), maxstrength(i), pvis(i), plev(i), pscore(i), 
          ((pgrouped(i) == i) ? "(Leader)" : " "));
    return;
  }

  /* Ok, we want to add item1 to our group */
  if ((pl1 == -1) || (plev(mynum) < pvis(pl1)))
    bprintf("Group with who?\n"); 

  else if (pl1 >= max_players)
    bprintf("You can't group a monster.\n");

  else if (pgrouped(pl1) == mynum)
    ungroup(pl1);

  else if (pgrouped(mynum) == -1) {
    if (mynum == pl1) {
      pgrouped(pl1) = mynum;
      bprintf("You group yourself.\n");
      return;
    }
    bprintf("You have to group yourself first.\n");
  }

  else if (pgrouped(mynum) != mynum)
    bprintf("You are grouped with someone already. [use lose to leave]\n");

  else if (pfollow(pl1) != mynum)
    bprintf("%s is not following you.\n",pname(pl1));

  else /* Ok, lets group pl1 */
  {
    pgrouped(pl1) = mynum;
    bprintf("Ok\n");
    sendf(pl1, "You are now a group member. [Leader %s]\n",pname(mynum));
    if (mynum != pl1)
      mudlog("GROUP: %s has joined %s.",pname(pl1), pname(mynum));
  }
  return;
}

/*
**  ungroups player pl, and the whole group if pl was the leader.
*/
void ungroup(int pl)
{
  int i;

  if (pgrouped(pl) == -1)
    return;

  if (pl == mynum) /* ungroup the leader and everyone grouped to them */
  {
    for(i = 0; i < max_players; i++)
      if (is_in_game(i) && (pgrouped(i) == mynum) && (i != pl))
        ungroup(i);
  }
  pgrouped(pl) = -1;
  sendf(pl,"You are no longer grouped.\n");
  return;
}

/*
**  TELL <msg> to all the members of my group. Note that visibility, s,p,m or 
**  lflags make no difference here. Invisible players will have their names 
**  seen by the group.
*/
void gtell() {
  int b, leader;
  char buf[80];

  if ((leader = is_grouped(mynum)) == -1) {
    bprintf("Your not grouped with anyone.\n");
    return;
  }

  if (EMPTY(txt1)) {
    bprintf("What do you want to tell them?\n");
    return;
  }

  for (b = 0; b < max_players; b++) {
    if (is_in_game(b) && leader == is_grouped(b)) {
      if (b == mynum)
        sprintf(buf,"You tell the group, ");
      else
        sprintf(buf,"%s tells the group, ", pname(mynum));

      sendf(b, "%s&+M\"&+m%s&+M\"\n", buf, txt1);
    }
  }
  return;
}

/*
**  This will decide fi members of a group should follow their leader and
**  cause them to if they should. Or try to in somecases.
*/
void check_group_leader(int lloc, int dir)
{
  int i;
  int me, rme;
 
  if (is_grouped(mynum) != mynum)
    return;

  me = mynum;
  rme = real_mynum;
  for (i = 0; i < max_players; i++) {
    if (i != me && is_grouped(i) == me && ploc(i) == lloc) {
      setup_globals(i);
      sendf(i,"You follow %s.\n",pname(me));
      dodirn(False, dir+2);
    }
  }
  setup_globals(rme);
  return;
}

/*
**  Promote another member of your group to leader
*/
void promote() {
  int i;

  if (is_grouped(mynum) != mynum)
  {
    bprintf("You do not lead a group.\n");
    return;
  }
  if ((pl1 == -1) || (mynum != is_grouped(pl1)))
  {
    bprintf("Promote who to leader? You must specify another group member.\n");
    return;
  }
  for (i = 0; i < max_players; i++) {
    if (is_in_game(i) && mynum == is_grouped(i)) {
      pgrouped(i) = pl1;
      sendf(i,"The new group leader is %s.\n",pname(pl1));
    } 
  }
  return;
}

/*
**  kill_bonus() just shares the points awarded for the death blow, amongst
**  the group. See group.h for some configuration.
**
**  BONUS_FACTOR_DAM 	: Default is 10, as was in origional dirt3.0
**  BONUS_FACTOR_ARMOR	: Default 5. Extra bonus for armored mobs.
**  BONUS_PER_MEMBER	: Xtra % bonus added to score gain / group member.
**  MAX_BONUS		: Maximum bonus to be added as %.
**
*/
void kill_bonus(int a, int v)
{
  int i, leader;
  int xp,x;

  if (v < max_players) /* v is a player */
    xp = ((plev(v) > 4) ? (plev(v)*plev(v)*plev(v)*10) : 0);
  else
    xp = (BONUS_FACTOR_DAM * pdam(v)) + (BONUS_FACTOR_ARMOR * parmor(v));
  
  x = 0;
  if ((leader = is_grouped(a)) >= 0) {
    for(i = 0; i < max_players; i++)
      x += (((is_grouped(i) == leader) && (ploc(a) == ploc(i))) ? 1 : 0);

    /* add % to xp */
    xp += ((xp * (min(MAX_BONUS, (x * BONUS_PER_MEMBER)))) / 100);
    
    for (i = 0; i < max_players; i++)
      if ((leader == is_grouped(i)) && (ploc(i) == ploc(a)))
        setpscore(i, pscore(i) + xp/x);
  }
  else
    setpscore(a, pscore(a) + xp);

  return;
}
    
