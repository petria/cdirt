#include <stdlib.h>
#include "kernel.h"
#include "levels.h"
#include "uaf.h"
#include "mobile.h"
#include "sendsys.h"
#include "change.h"
#include "parse.h"
#include "zones.h"
#include "bprintf.h"
#include "log.h"
#include "wizlist.h"
#include "rooms.h"
#include "objsys.h"
#include "mud.h"
#include "fight.h"

static void change_visibility (void);
static void change_speed (void);
static void change_sex (void);
static void change_level (void);
static void change_score (void);
static void change_strength (void);
static void change_damage (void);
static void change_armor (void);
static void change_aggression (void);
static void change_passwd (void);
static void change_name (void);
static void change_wimpy (void);
static void change_magic (void);
static void change_coins (void);
static void change_start (void);

static char *ChTable[] =
{
  "sex", "speed", "score", "strength", "level", "visibility", "damage",
  "aggression", "title", "password", "name", "description", "armor",
  "wimpy", "magic", "coins", "start", TABLE_END
};

void (*ChFunTable[]) (void) =
{
  change_sex, change_speed, change_score, change_strength, change_level,
  change_visibility, change_damage, change_aggression, change_title, 
  change_passwd, change_name, change_desc, change_armor, change_wimpy,
  change_magic, change_coins, change_start
};

#define SEX         0
#define SPEED       1
#define SCORE       2
#define STRENGTH    3
#define LEVEL       4
#define VIS         5
#define DAMAGE      6
#define AGG        11
#define TITLE      12
#define PASSWD     13
#define NAME       14
#define DESC       15
#define ARMOR      16
#define WIMPY      17
#define MAGIC	   18
#define COINS      19
#define START      20

void changecom (void) {
  int x;

  if (brkword () == -1) {
    bprintf ("Change what?\n");
    return;
  }
  if (aliased(real_mynum)) {
    bprintf ("Not while aliased.\n");
    return;
  }
  if ((x = tlookup (wordbuf, ChTable)) >= 0)
    ChFunTable[x] ();
  else
    bprintf ("I don't know how to change that.\n");
}

static void change_sex (void) {
  int a, w;
  PERSONA p;
  Boolean f, is_me;

  if (!ptstflg (mynum, PFL_UAF) || plev (mynum) < LVL_WIZARD) {
    if ((a = vicf2 (SPELL_VIOLENT, 7)) < 0)
      return;
    player2pers (&p, NULL, a);
    f = False;
  } 
  else if (brkword () == -1 || (a = find_player(wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }

  is_me = !f && a == mynum;

  if ((w = wlevel (plev (mynum)) - wlevel (p.ublock.plev)) < 0
      || (w == 0 && plev (mynum) >= LVL_WIZARD && plev (mynum) < LVL_GOD && !is_me)) {
    bprintf ("You can't do that to %s.\n", p.ublock.pname);
    return;
  }

  if (f) {
    pset_flg(p.ublock.bits, SFL_FEMALE, SFLAGS); 

    if (p.ublock.plev <= LVL_WIZARD && p.ublock.plev >= 1)
      strcpy(p.player.ptitle, std_title (p.ublock.plev, 
        ptst_flg(p.ublock.bits, SFL_FEMALE, SFLAGS), p.ublock.class));
    putuaf (&p);
  } 
  else {
    setpsex (a, !psex (a));
    if (plev (a) <= LVL_WIZARD && plev (a) >= 1) {
      setptitle (a, std_title (plev (a), psex (a), pclass(a)));
    }
    sendf (a, "Your sex has been magically changed!\nYou are now %s.\n",
	   psex (a) ? "female" : "male");

    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
	       LVL_WIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange &*(Sex: "
	      "%s): &+W%s &*changed &+W%s&+B]\n",
	      psex (a) ? "Female" : "Male", pname (mynum), pname (a));
  }
}

/* Change the speed of mobiles.
 */
static void
change_speed (void)
{
  int a, new_speed, old_speed;

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("I don't know how to change that.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("For whom ?\n");
    return;
  }
  if ((a = fmbn (wordbuf)) < 0 || a < max_players) {
    bprintf ("There is no mobile by that name.\n");
    return;
  }
  if (brkword () == -1 || !isdigit (*wordbuf)) {
    bprintf ("Set speed to what ?");
    return;
  }
  if ((new_speed = atoi (wordbuf)) > 100) {
    bprintf ("The highest value is 100.\n");
    new_speed = 100;
  }
  old_speed = pspeed (a);
  bprintf ("Setting speed for %s to %d.\n", pname (a), new_speed);
  setpspeed (a, new_speed);

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
	     LVL_WIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange &*(Speed: "
	    "%d->%d): &+W%s &*changed &+W%s&+B]\n",
	    old_speed, new_speed, pname (mynum), pname (a));
}

static void
change_score (void)
{
  Boolean f, is_me, is_mobile;
  int a;
  int new_score, old_score;
  PERSONA p;

  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if (plev (mynum) < LVL_WIZARD || !ptstflg (mynum, PFL_CH_SCORE)) {
    erreval ();
    return;
  }
  if (!is_mobile && !is_me && !ptstflg (mynum, PFL_FROB)) {
    bprintf ("You can only change your own score.\n");
    return;
  }
  if (wlevel (plev (mynum)) <= wlevel (p.ublock.plev) && !is_me) {
    bprintf ("That is beyond your powers.\n");
    return;
  }
  new_score = (brkword () < 0) ? p.ublock.pscore : atoi (wordbuf);

  if (new_score < 0 || new_score > levels[LVL_WIZARD]) {
    bprintf ("Score out of range!\n");
    return;
  }
  if (!is_me && !is_mobile) {
    mudlog ("CHANGE: %s changed score on %s from %d to %d",
	    pname (mynum), p.ublock.pname, p.ublock.pscore, new_score);
  }
  bprintf ("Setting score for %s to %d\n", p.ublock.pname, new_score);

  if (f) {
    old_score = p.ublock.pscore;
    p.ublock.pscore = new_score;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Score: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_score, new_score, pname (mynum), p.ublock.pname);
  } else {
    old_score = pscore (a);
    setpscore (a, new_score);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Score: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_score, new_score, pname (mynum), pname (a));
  }
}

static void change_coins (void) {
  Boolean f, is_me, is_mobile;
  int a;
  int new_coins, old_coins;
  PERSONA p;

  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if (plev (mynum) < LVL_WIZARD || !ptstflg (mynum, PFL_CH_COINS)) {
    erreval ();
    return;
  }
  if (!is_mobile && !is_me && !ptstflg (mynum, PFL_FROB)) {
    bprintf ("You can only change your own coins.\n");
    return;
  }
  if (wlevel (plev (mynum)) <= wlevel (p.ublock.plev) && !is_me) {
    bprintf ("That is beyond your powers.\n");
    return;
  }
  new_coins = (brkword() == -1) ? p.player.coins : atoi (wordbuf);

  if (new_coins < 0) {
    bprintf ("Must be a positive number!\n");
    return;
  }
  if (!is_me && !is_mobile) {
    mudlog ("CHANGE: %s changed coins on %s from %d to %d",
	    pname (mynum), p.ublock.pname, p.player.coins, new_coins);
  }
  bprintf ("Setting coins for %s to %d\n", p.ublock.pname, new_coins);

  if (f) {
    old_coins = p.player.coins;
    p.player.coins = new_coins;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Coins: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_coins, new_coins, pname(mynum), p.ublock.pname);
  } else {
    old_coins = pcoins(a);
    pcoins(a) = new_coins;
    send_msg(DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Coins: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_coins, new_coins, pname(mynum), pname(a));
  }
}

static void
change_strength (void)
{
  Boolean f, is_mobile, is_me;
  int a, new_str, old_str, max_str;
  PERSONA p;

  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;
  max_str = is_mobile ? pstr_reset (a) : maxstrength (a);

  if ((is_mobile && !ptstflg (mynum, PFL_CH_MDATA))
      || (!is_mobile && !ptstflg (mynum, PFL_HEAL))) {
    erreval ();
    return;
  }
  if (wlevel (plev (mynum)) <= wlevel (p.ublock.plev) && !is_me) {
    bprintf ("That is beyond your powers.\n");
    return;
  }
  new_str = (brkword () < 0) ? p.ublock.pstr : max (0, atoi (wordbuf));

  if (new_str < 1 || new_str > max_str) {
    bprintf ("Out of Range!\n");
    return;
  }
  if (p.ublock.pstr > new_str && !is_mobile && !is_me) {
    if (!ptstflg (mynum, PFL_FROB)) {
      bprintf ("How rude of you! Trying to make other players weaker!\n");
      return;
    } else {
      mudlog ("CHANGE: %s lowered %s's strength from %d to %d",
	      pname (mynum), p.ublock.pname, p.ublock.pstr, new_str);
    }
  }
  bprintf ("Setting strength of %s to %d\n", p.ublock.pname, new_str);

  if (f) {
    old_str = p.ublock.pstr;
    p.ublock.pstr = new_str;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Strength: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_str, new_str, pname (mynum), p.ublock.pname);
  } else {
    old_str = pstr (a);
    setpstr (a, new_str);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Strength: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_str, new_str, pname (mynum), pname (a));
  }
}

static void change_level (void) {
  Boolean f, is_mobile, is_me;
  int a, new_lvl, old_lvl;
  PERSONA p;

  if (plev (mynum) < LVL_WIZARD || !ptstflg (mynum, PFL_CH_LEVEL)) {
    bprintf ("I don't know how to change that.\n");
    return;
  }
  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who is that?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if (is_mobile) {
    bprintf ("Changing level on mobiles doesn't make sense now!\n");
    return;
  }
  if (!is_me && (!ptstflg (mynum, PFL_FROB)
		 || (wlevel (p.ublock.plev) >= wlevel (plev (mynum)) && plev (mynum) < LVL_GOD))) {
    bprintf ("You can't change level on %s.\n", p.ublock.pname);
    return;
  }
  if (brkword () < 0) {
    bprintf ("What level?\n");
    return;
  }
  if ((new_lvl = atoi (wordbuf)) >= LVL_MAX || new_lvl < LVL_MIN) {
    bprintf ("Level must be between %d and %d.\n", LVL_MIN, LVL_MAX - 1);
    return;
  }
  if (plev (mynum) >= LVL_GOD || wlevel (new_lvl) < wlevel (plev (mynum))
      || (is_me && wlevel (new_lvl) == wlevel (plev (mynum)))) {
    if (wlevel (p.ublock.plev) != wlevel (new_lvl)) {
      update_wizlist (p.ublock.pname, wlevel (new_lvl));
      set_xpflags(f ? p.ublock.bits : mbits(a), new_lvl);
    }
    if (f) {
      if (p.ublock.plev <= LVL_WIZARD || new_lvl <= LVL_WIZARD) {
	strcpy (p.player.ptitle, std_title(new_lvl, 
          ptst_flg(p.ublock.bits, SFL_FEMALE, SFLAGS), p.ublock.class));
      }
      old_lvl = p.ublock.plev;
      p.ublock.plev = new_lvl;
      putuaf (&p);
      mudlog("&+BLEVEL:&N %s changed stored player %s's level [%d->%d]", 
        pname(mynum), p.ublock.pname, old_lvl, new_lvl);
      send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
		"&*(Level: %d->%d): &+W%s &*changed &+W%s&+B]\n",
		old_lvl, new_lvl, pname (mynum), p.ublock.pname);
    } 
    else {
      if (p.ublock.plev <= LVL_WIZARD || new_lvl <= LVL_WIZARD) {
	setptitle (a, std_title (new_lvl, 
          ptst_flg(p.ublock.bits, SFL_FEMALE, SFLAGS), p.ublock.class));
      }
      old_lvl = plev (a);
      setplev (a, new_lvl);
      send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
		"&*(Level: %d->%d): &+W%s &*changed &+W%s&+B]\n",
		old_lvl, new_lvl, pname (mynum), pname (a));
      mudlog("&+BLEVEL:&N %s changed %s from %d to %d", 
        pname(mynum), pname(a), old_lvl, new_lvl);
    }
  } 
  else
    bprintf ("You can't change %s to level %d!\n", p.ublock.pname, new_lvl);
}

static void change_visibility (void)
{
  int a, new_vis, max_vis, old_vis;
  Boolean f, is_me;
  PERSONA p;

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("I don't know how to change that.\n");
    return;
  }
  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who is that?\n");
    return;
  }
  is_me = !f && a == mynum;

  if (!is_me && wlevel (p.ublock.plev) >= wlevel (plev (mynum))
      && plev (mynum) < LVL_GOD) {

    bprintf ("You can't change visibility on %s!\n", p.ublock.pname);
    return;
  }
  new_vis = (brkword () == -1) ? 0 : atoi (wordbuf);

  if (plev (mynum) < LVL_ISTARI)
    max_vis = LVL_ISTARI;
  else if (plev (mynum) < LVL_ARCHWIZARD)
    max_vis = LVL_ARCHWIZARD;
  else if (plev (mynum) < LVL_DEMI)
    max_vis = LVL_DEMI;
  else if (plev (mynum) < LVL_SHALAFI)
    max_vis = LVL_SHALAFI;
  else if (plev (mynum) < LVL_GOD)
    max_vis = LVL_GOD;
  else if (plev (mynum) < LVL_CREATOR)
    max_vis = LVL_CREATOR;
  else
    max_vis = LVL_MAX - 1;

  if (new_vis > max_vis) {
    bprintf ("The maximum is %d.\n", max_vis);
    new_vis = max_vis;
  }
  bprintf ("Setting visibility for %s to %d.\n", p.ublock.pname, new_vis);

  if (f) {
    old_vis = p.ublock.pvis;
    p.ublock.pvis = new_vis;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Vis: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_vis, new_vis, pname (mynum), p.ublock.pname);
  } else {
    old_vis = pvis (a);
    setpvis (a, new_vis);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Vis: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_vis, new_vis, pname (mynum), pname (a));
  }
}

static void
change_damage (void)
{
  Boolean f, is_me, is_mobile;
  int a, new_damage, old_damage;
  PERSONA p;

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("I don't know how to change that.\n");
    return;
  }
  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if ((is_mobile && !ptstflg (mynum, PFL_CH_MDATA)) ||
      (!is_mobile && !is_me && (plev (mynum) < LVL_ARCHWIZARD)) ||
      (wlevel (plev (mynum)) < wlevel (p.ublock.plev))) {
    bprintf ("That is beyond your powers.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("Change damage on %s to what ?\n", p.ublock.pname);
    return;
  }
  new_damage = max (0, atoi (wordbuf));

  if (new_damage < 0 || new_damage > 100) {
    bprintf ("Out of Range!\n");
    return;
  }
  if (!is_me) {
    mudlog ("CHANGE: %s changed damage on %s from %d to %d",
	    pname (mynum), p.ublock.pname, p.ublock.pdam, new_damage);
  }
  bprintf ("Setting damage on %s %s to %d\n",
	   p.ublock.pname, is_mobile ? "" : "permanently", new_damage);

  if (f) {
    old_damage = p.ublock.pdam;
    p.ublock.pdam = new_damage;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Damage: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_damage, new_damage, pname (mynum), p.ublock.pname);
  } else {
    old_damage = pdam (a);
    setpdam (a, new_damage);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Damage: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_damage, new_damage, pname (mynum), pname (a));
  }
}

static void
change_armor (void)
{
  Boolean f, is_me, is_mobile;
  int a, new_armor, old_armor;
  PERSONA p;

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("I don't know how to change that.\n");
    return;
  }
  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if ((is_mobile && !ptstflg (mynum, PFL_CH_MDATA)) ||
      (!is_mobile && !is_me && (plev (mynum) < LVL_ARCHWIZARD)) ||
      (wlevel (plev (mynum)) < wlevel (p.ublock.plev))) {
    bprintf ("That is beyond your powers.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("Change armor on %s to what ?\n", p.ublock.pname);
    return;
  }
  new_armor = max (0, atoi (wordbuf));

  if (!is_me) {
    mudlog ("CHANGE: %s changed armor on %s from %d to %d",
	    pname (mynum), p.ublock.pname, p.ublock.parmor, new_armor);
  }
  bprintf ("Setting armor on %s%sto %d\n",
	   p.ublock.pname, is_mobile ? " " : " permanently ", new_armor);

  if (f) {
    old_armor = p.ublock.parmor;
    p.ublock.parmor = new_armor;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Armor: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_armor, new_armor, pname (mynum), p.ublock.pname);
  } else {
    old_armor = parmor (a);
    setparmor (a, new_armor);
    calc_ac(a);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Armor: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_armor, new_armor, pname (mynum), pname (a));
  }
}

static void change_aggression (void) {
  int a, new_agg, old_agg;

  if (plev (mynum) < LVL_WIZARD || !ptstflg (mynum, PFL_CH_MDATA)) {
    bprintf ("I don't know how to change that.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("For whom ?\n");
    return;
  }
  if ((a = fmbn (wordbuf)) < 0 || a < max_players) {
    bprintf ("There is no mobile by that name.\n");
    return;
  }
  if (brkword () == -1 || !isdigit (*wordbuf)) {
    bprintf ("Set aggression to what ?");
    return;
  }
  if ((new_agg = atoi (wordbuf)) > 100) {
    bprintf ("The highest value is 100.\n");
    new_agg = 100;
  }
 
  bprintf("Setting aggression on %s to %d\n", pname(a), new_agg);
  old_agg = pagg (a);
  setpagg (a, new_agg);

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		       LVL_WIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	    "&*(Aggression: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	    old_agg, new_agg, pname (mynum), pname (a));

  mudlog ("CHANGE: %s set %s to %d", pname (mynum), pname (a), new_agg);
}

static Boolean check_title (char *title) {
  int num_title = 0;
  char *p;

  for (p = title ; *p ; p++) {
    if (*p == '%') {
      if (*(p + 1) == 's')
	num_title++;
      else
	return False;
    }
  }

  if (num_title == 1 || (num_title == 0 && plev(mynum) > LVL_WIZARD))
    return True;
  else
    return False;
}

void change_title (void)
{
  PERSONA p;
  int a;
  char buff[300];
  Boolean f, is_me, is_mobile, disp_title = False;

  if (!ptstflg (mynum, PFL_TITLES)) {
    erreval ();
    return;
  }
  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  getreinput (buff);

  if (EMPTY (buff)) {
    disp_title = True;
  } else if (is_mobile) {
    int lev;
    char *owner = powner (a);

    if (ppermanent (a)) {
      bprintf ("That's a permanent mobile.\n");
      return;
    }
    lev = plev(a);

    if (!EQ (pname (mynum), owner) && !do_okay_l (plev (mynum), lev, False)) {
      bprintf ("You can't change title on %s's mobiles.\n", owner);
      return;
    }
    if (strchr (buff, '^') != NULL) {
      bprintf ("Illegal character(s): '^' in title.\n");
      return;
    }
  } else if (!is_me && !do_okay_l (plev (mynum), p.ublock.plev,
			wlevel (p.ublock.plev) < wlevel (plev (mynum)))) {
    bprintf ("That is beyond your powers.\n");
    return;
  } else if (strlen (buff) + strlen (pname(mynum)) > TITLE_LEN) {
    bprintf ("Maximum title-length is %d characters.\n", TITLE_LEN);
    return;
  } else if (!check_title (buff)) {
    bprintf ("Invalid format. See HELP TITLE\n");
    return;
  }
  if (!f) {
    if (disp_title)
      bprintf ("Title: %s\n", is_mobile ? pftxt (a) : 
	                                  make_title (ptitle (a), "<name>"));
    else if (is_mobile) {
      if (pftxt (a) != NULL)
	FREE (pftxt (a));
      pftxt (a) = COPY (buff);
    } else {
      setptitle (a, buff);
    }
  } else {
    if (disp_title)
      bprintf ("Title: %s\n", make_title (p.player.ptitle, "<name>"));
    else {
      strcpy (p.player.ptitle, buff);
      putuaf (&p);
      send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		       LVL_WIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
		"&*(Title): &+W%s &*changed &+W%s&+B]\n",
		pname (mynum), p.ublock.pname);
    }
  }

  if (!disp_title)
    bprintf ("Title changed on %s.\n", is_mobile ? pname (a) : p.ublock.pname);

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
	    LVL_WIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange &*(Title): "
	    "&+W%s &*changed &+W%s&+B]\n",
	    pname (mynum), is_mobile ? pname (a) : p.ublock.pname);
}

static void
change_name (void)
{
  PERSONA p, q;
  char old_name[MNAME_LEN + 1];
  char buff[256];
  char buff2[256];
  char *r;
  int num;
  int max_len;
  Boolean in_uaf;
  Boolean is_mobile;
  Boolean is_me;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf ("Change name on who ?\n");
    return;
  }
  if ((num = find_player (wordbuf, &p, &in_uaf)) == -1) {
    bprintf ("No such player or mobile: %s.\n", wordbuf);
    return;
  }
  strcpy (old_name, p.ublock.pname);

  is_mobile = num >= max_players;
  is_me = num == mynum;
  max_len = is_mobile ? MNAME_LEN : PNAME_LEN;

  if (EMPTY (getreinput (wordbuf))) {
    bprintf ("Change name on %s to what ?\n", old_name);
    return;
  }
  if (strlen (wordbuf) > max_len) {
    bprintf ("Name too long, Maximum: %d.\n", max_len);
    return;
  }
  for (r = wordbuf; isalpha (*r) || (is_mobile && *r == ' '); r++) ;

  if (*r != '\0') {
    bprintf ("Name contains illegal character: %c\n", *r);
    return;
  }
  if (isalpha (*wordbuf))
    *wordbuf = toupper (*wordbuf);

  if (is_mobile) {
    Boolean denied;
    char *owner = powner (num);

    if (ppermanent (num)) {
      bprintf ("That's a permanent mobile.\n");
      return;
    }
    if ((denied = !EQ (pname (mynum), owner))) {
      int x, lev;

      lev = (x = fpbn (owner)) >= 0 ? plev (x) :
	getuaf (owner, &p) ? p.ublock.plev : 0;

      if (do_okay_l (plev (mynum), lev, False)) {
	denied = False;
      }
    }
    if (denied) {
      bprintf ("You're not powerful enough to change %s's mobiles.\n", owner);
      return;
    }
    setpname (num, wordbuf);

    FREE (pname_reset (num));
    pname_reset (num) = COPY (wordbuf);

  } else {
    char *new_name = wordbuf;

    if (plev (mynum) < LVL_SHALAFI) {
      bprintf ("You can only change name on mobiles.\n");
      return;
    }
    if (getuaf (new_name, &q)) {
      bprintf ("%s already exists.\n", new_name);
      return;
    }
    if (deluaf (old_name))
      return;

    strcpy (p.ublock.pname, new_name);
    newuaf (&p);

    if (!in_uaf)
      setpname (num, new_name);

    if ((num = get_zone_by_name (old_name)) >= num_const_zon) {
      FREE (zname (num));
      zname (num) = COPY (new_name);
    }
    sprintf(buff, "%s/%s", WIZ_ZONE_DIR, old_name);
    sprintf(buff2, "%s/%s", WIZ_ZONE_DIR, new_name);
    rename(buff, buff2);

    update_wizlist (old_name, LEV_MORTAL);
    update_wizlist (new_name, wlevel (p.ublock.plev));

    bprintf ("Ok.\n");
    mudlog ("CHANGE: %s changed name on %s to %s",
	    pname (mynum), old_name, new_name);

    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Name: %s->%s): &+W\001p%s\003 &*changed it&+B]\n",
	      old_name, new_name, pname (mynum));
  }
}

static void change_room_desc (int loc, char *room_name) {
  char buff[256];
  char *owner = lowner (loc);
  Boolean denied;

  if (lpermanent (loc)) {
    bprintf ("That's a permanent location.\n");
    return;
  }
  if ((denied = !EQ (pname (mynum), owner))) {
    int x, lev;
    PERSONA p;

    lev = (x = fpbn (owner)) >= 0 ? plev (x) :
      getuaf (owner, &p) ? p.ublock.plev : 0;

    if (do_okay_l (plev (mynum), lev, False)) {
      denied = False;
    }
  }
  if (denied) {
    bprintf ("You're not powerful enough to change %s's rooms.", owner);
    return;
  }
  cur_player->work2[0] = (long int) &llong(loc);
  strcpy((char *) &(cur_player->work2[1]), "Room description changed.\n");
  sprintf(buff, "%s/DESC.%s", TEMP_DIR, pname(mynum));
  start_editor("Please enter the room description:\n",
               "&+W[&+BC-Desc %.3d&+W]&N ", NULL, buff, False, TYPE_CHDESC);
}

#define LOAD_INCR 256

void load_from_file(char *filename) {
  char *p, *q;
  FILE *f;
  int len = LOAD_INCR * 2;

  f = FOPEN(filename, "r");
  p = q = NEW(char, len);

  while(!feof(f)) {
    fgets(q, LOAD_INCR - 1, f);
    if (strchr(q, '^')) {
      bprintf("Text block cannot contain the ^ character.\n");
      FCLOSE(f);
    }
    else if (strchr(q, '"')) {
      bprintf("Text block cannot contain the \" character.\n");
      FCLOSE(f);
    }
    q = q + strlen(q);
    if (len - (q - p) < LOAD_INCR) {
      len += LOAD_INCR;
      p = resize_array(p, sizeof(char), len, len + LOAD_INCR);
    }
  }

  FCLOSE(f);

  *(q + strlen(q) - 1) = 0;                        /* remove newline */

  if (*(char **) (cur_player->work2[0]))        /* set whatever desc */
    FREE(*(char **) (cur_player->work2[0]));
  *(char **) (cur_player->work2[0]) = p;
  bprintf((char *) &cur_player->work2[1]);
}

static void change_mob_desc (int mob, char *mob_name) {
  char *owner = powner (mob);
  char buff[256];
  Boolean denied;

  if (ppermanent (mob)) {
    bprintf ("That's a permanent mobile.\n");
    return;
  }
  if ((denied = !EQ (pname (mynum), owner))) {
    int x, lev;
    PERSONA p;

    lev = (x = fpbn (owner)) >= 0 ? plev (x) :
      getuaf (owner, &p) ? p.ublock.plev : 0;

    if (do_okay_l (plev (mynum), lev, False)) {
      denied = False;
    }
  }
  if (denied) {
    bprintf ("You're not powerful enough to change %s's mobiles.", owner);
    return;
  }
  cur_player->work2[0] = (long int) &pexam(mob);
  strcpy((char *) &cur_player->work2[1], "Mobile description changed.\n");
  sprintf(buff, "%s/DESC.%s", TEMP_DIR, pname(mynum));
  start_editor("Please enter the mobile description:\n",
               "&+W[&+BC-Desc %.3d&+W]&N ", NULL, buff, False, TYPE_CHDESC);
}

static void change_obj_desc (int obj, char *obj_name) {
  char *owner = oowner (obj);
  char buff[256];
  Boolean denied;

  if (opermanent (obj)) {
    bprintf ("That's a permanent object.\n");
    return;
  }
  if ((denied = !EQ (pname (mynum), owner))) {
    int x, lev;
    PERSONA p;

    lev = (x = fpbn (owner)) >= 0 ? plev (x) :
      getuaf (owner, &p) ? p.ublock.plev : 0;

    if (do_okay_l (plev (mynum), lev, False)) {
      denied = False;
    }
  }
  if (denied) {
    bprintf ("You're not powerful enough to change %s's objects.", owner);
    return;
  }

  cur_player->work2[0] = (long int) &oexam_text(obj);
  strcpy((char *) &cur_player->work2[1], "Object description changed.\n");
  sprintf(buff, "%s/DESC.%s", TEMP_DIR, pname(mynum));
  start_editor("Please enter the object description:\n",
               "&+W[&+BC-Desc %.3d&+W]&N ", NULL, buff, False, TYPE_CHDESC);
}

static void change_player_desc (PERSONA * p) {
  char buff[128];

  if (!EQ (pname (mynum), p->ublock.pname) &&
      !do_okay_l (plev (mynum), p->ublock.plev, False)) {

    bprintf ("You're not powerful enough to change %s's description.\n", 
	     p->ublock.pname);
    return;
  }
  sprintf (buff, DESC_DIR "/%s", p->ublock.pname);

  start_editor ("Please enter the new description:\n",
		"&+W[&+BC-Desc %.3d&+W]&N ",
		buff, buff, False, TYPE_DESC);
}

void change_start (void) {
#define START_LOC 0
#define START_MOB 1
#define START_OBJ 2
}

void change_desc (void) {
#define DESC_LOC 0
#define DESC_MOB 1
#define DESC_OBJ 2
#define DESC_PLR 3

  char *desc_type[] = {"Location", "Mobile", "Object", "Player", TABLE_END};

  PERSONA p;
  int loc;

  if (brkword () == -1) {
    if (aliased(real_mynum)) {
      bprintf ("Not while aliased.\n");
      return;
    }
    player2pers (&p, NULL, mynum);
    change_player_desc (&p);
  } 
  else {
    int type = tlookup (wordbuf, desc_type);

    if (brkword () == -1 || type == -1) {
      bprintf ("Change description on what?\n");
      return;
    }
    if (type == DESC_LOC && (loc = find_loc_by_name (wordbuf)) != -1)
      change_room_desc (loc, wordbuf);
    else if (type == DESC_MOB && (loc = fmbn (wordbuf)) >= max_players)
      change_mob_desc (loc, pname (loc));
    else if (type == DESC_OBJ && (loc = fobn (wordbuf)) >= 0)
      change_obj_desc (loc, oname (loc));
    else if (type == DESC_PLR && getuaf (wordbuf, &p))
      change_player_desc (&p);
    else
      bprintf ("Can't find %s: %s\n", desc_type[type], wordbuf);
  }
}

void change_passwd (void) {
  PERSONA *p;

  if (EQ(pname(mynum), MASTERUSER))
    bprintf("Remember, you also need to change the UNVEIL_PASS in the code.\n");

  if (brkword () != -1 && !EQ (wordbuf, pname (mynum))) {
    if (plev (mynum) < LVL_SHALAFI) {
      bprintf ("You can only change your own password.\n");
      return;
    }
    p = NEW (PERSONA, 1);

    if (!getuaf (wordbuf, p)) {
      bprintf ("I cannot find anyone called \"%s\". \n", wordbuf);
      FREE (p);
      return;
    }
    cur_player->work = (intptr_t) p;
    bprintf("\n\001R\003");
    sprintf(cur_player->cprompt, "New Password for %s: ", wordbuf);
    replace_input_handler (ask_new_passwd);
  } 
  else {
    bprintf("\n\001R\003");
    strcpy (cur_player->cprompt, "Old Password: ");
    replace_input_handler (ask_old_passwd);
  }
}

void ask_old_passwd (char *pass) {
  PERSONA *p;
  char b[PASSWD_LEN];

  my_crypt (b, pass);
  if (strcmp (b, cur_player->passwd) != 0) {
    bprintf ("\nWrong password!\n\001E\003");
    mudlog ("CHANGE: Wrong old password for %s", pname (mynum));
    get_command (NULL);
  } else {
    bprintf ("\n");
    strcpy (cur_player->cprompt, "New Password: ");
    cur_player->work = (intptr_t) (p = NEW (PERSONA, 1));
    strcpy (p->ublock.pname, pname (mynum));
    replace_input_handler (ask_new_passwd);
  }
}

void ask_new_passwd (char *pass) {
  PERSONA *p = (PERSONA *) (cur_player->work);

  strcpy (p->player.passwd, pass);
  strcpy (cur_player->cprompt, "Confirm Password: ");
  replace_input_handler (ask_confirm_passwd);
}

void ask_confirm_passwd (char *pass) {
  PERSONA *p = (PERSONA *) (cur_player->work);
  PERSONA q;
  int plx;

  bprintf ("\001E\003");
  if (strcmp (p->player.passwd, pass) != 0)
    bprintf ("\nNot same new password, password not changed.\n");
  else if (!getuaf (p->ublock.pname, &q))
    bprintf ("\nCouldn't find entry in UAF file.\n");
  else {
    if ((plx = fpbn (p->ublock.pname)) >= 0)
      player2pers (&q, &q.player.last_on, plx);

    my_crypt (q.player.passwd, p->player.passwd);
    putuaf (&q);

    if (plx >= 0 && plx < max_players)
      strcpy (players[plx].passwd, q.player.passwd);
    bprintf (" Password changed.\n");
    mudlog ("CHANGE: %s changed password on %s.", pname (mynum), q.ublock.pname);
  }
  get_command (NULL);
}

static void change_wimpy (void) {
  char *player;
  int oldwimp, newwimp, plx;

  if (brkword() == -1) {
    bprintf("Set your wimpy to what value?\n");
    return;
  }
  else if (plev(mynum) < LVL_WIZARD && !isdigit(*wordbuf)) {
    bprintf("You can only set your own wimpy level");
    return;
  }
  else if (isdigit(*wordbuf)) {
    player = pname(mynum);
    oldwimp = pwimpy(mynum);
    newwimp = atoi(wordbuf);
    setpwimpy(mynum, newwimp);
    if (newwimp)
      bprintf("You will now flee at %d hit points.\n", newwimp);
    else
      bprintf("Auto-flee Disabled!\n");
  }
  else if ((plx = fmbn(wordbuf)) != -1) {
    if (brkword() == -1) {
      bprintf("Set %s's auto-flee to what value?\n", pname(plx));
      return;
    }
    player = pname(plx);
    oldwimp = pwimpy(plx);
    newwimp = atoi(wordbuf);
    setpwimpy(mynum, newwimp);
    bprintf("You have set %s's wimpy to %d hit points.\n", pname(plx), newwimp);
  }
  else {
    bprintf("%s is not online.\n", wordbuf);
    return;
  }
  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
            LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
           "&*(Wimpy: %d->%d): &+W%s &*changed &+W%s&+B]\n",
            oldwimp, newwimp, pname(mynum), player);
}

static void change_magic (void) {
  Boolean f, is_mobile, is_me;
  int a, new_mag, old_mag;
  PERSONA p;

  if (brkword () == -1 || (a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if (!ptstflg (mynum, PFL_HEAL)) {
    erreval ();
    return;
  }
  if (is_mobile) {
    bprintf ("Mobiles do not use mana!\n");
    return;
  }
  if (wlevel (plev (mynum)) <= wlevel (p.ublock.plev) && !is_me) {
    bprintf ("That is beyond your powers.\n");
    return;
  }
  new_mag = (brkword () < 0) ? p.player.pmagic : max (0, atoi (wordbuf));

  if (new_mag < 0 || new_mag > maxmagic (a)) {
    bprintf ("Out of Range!\n");
    return;
  }
  if (p.player.pmagic > new_mag && !is_mobile && !is_me) {
    if (!ptstflg (mynum, PFL_FROB)) {
      bprintf ("How rude of you! Trying to take away people's mana!\n");
      return;
    }
  }
  bprintf ("Setting mana of %s to %d\n", p.ublock.pname, new_mag);

  if (f) {
    old_mag = p.player.pmagic;
    p.player.pmagic = new_mag;
    putuaf (&p);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Magic: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_mag, new_mag, pname (mynum), p.ublock.pname);

  } else {
    old_mag = pmagic (a);
    setpmagic (a, new_mag);
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), max (pvis (mynum),
		   LVL_ARCHWIZARD), LVL_MAX, mynum, NOBODY, "&+B[&+WChange "
	      "&*(Magic: %d->%d): &+W%s &*changed &+W%s&+B]\n",
	      old_mag, new_mag, pname (mynum), pname (a));
  }
}
