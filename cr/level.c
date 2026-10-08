#include "kernel.h"
#include "levels.h"
#include "sendsys.h"
#include "eflags.h"
#include "log.h"
#include "flags.h"
#include "wizlist.h"
#include "bprintf.h"
#include "level.h"
#include "mobile.h"
#include "quests.h"
#include "levelnames.h"

char *levnam(int lev, int class, Boolean sex) {
  if (lev == -1) 
    return("Mobile");
 
  if (lev >= LVL_STDWIZ && lev < LVL_ISTARI)
    lev = LVL_WIZARD;

  if (lev > LVL_WIZARD) {
    if (sex)
      return(FWizLevels[wlevel(lev)]);
    else
      return(MWizLevels[wlevel(lev)]);
  }
  else {
    switch(class) {
    case WARRIOR:
      return(WarriorLevels[lev]);
    case THIEF:
      return(ThiefLevels[lev]);
    case PRIEST:
      return(PriestLevels[lev]);
    case MAGE:
      return(MageLevels[lev]);
    }
  }
  return(NULL);
}

char *std_title(int level, Boolean sex, int class) {
  static char buff[TITLE_LEN + 10];

  sprintf(buff, "%%s the %s", levnam(level, class, sex));
  return(buff);
}

int levelof (int score, int lev) {
  int i;

  if (lev > LVL_WIZARD || lev < 1)
    return lev;
  for (i = LVL_WIZARD; i > 0; i--)
    if (score >= levels[i])
      return i;
  return 0;
}

static void calib_player_impl (int pl, Boolean update_skills) {
  int b, oldlev;

  oldlev = plev(pl);

  if (pl >= max_players || !players[pl].iamon || aliased(pl))
    return;

  if ((b = levelof(pscore(pl), plev(pl))) != plev(pl) && plev(pl) > 0) {
    if (qcheck(pl) < b) {
      if (needsquests(pl))
        return;
      else
        needsquests(pl) = True;

  #if QTYPE == 0
      sendf(pl, "You need all the quests to become a wizard.\n");

  #elif QTYPE == 1
      sendf(pl, "You need %d quests to %s, and only have %d.\n", 
                 qlevels[b], (b < LVL_WIZARD) ? "level" : "wiz", qpoints(pl));

  #elif QTYPE == 2
      sendf(pl, "You need %d qpoints to %s, and only have %d.\n",
                 qlevels[b], (b < LVL_WIZARD) ? "level" : "wiz", qpoints(pl));

  #elif QTYPE == 3
      if (b != LVL_WIZARD)
        sendf(pl, "You need %d more qpoints to level.\n",
          qlevels[b] - qpoints(pl));
  #else
      if (b == LVL_WIZARD) {
        if (qpoints(pl) < WIZ_QPOINTS && crit_qtest(pl))
          sendf(pl, "You need %d more qpoints to wiz.\n",
            WIZ_QPOINTS - qpoints(pl));
        else if (qpoints(pl) >= WIZ_QPOINTS && !crit_qtest(pl))
          sendf(pl, "You need to complete all the critical quests to wiz.\n");
        else
          sendf(pl, "You need %d qpoints, and the critical quests to wiz.\n",
            WIZ_QPOINTS - qpoints(pl));
      }
  #endif
      return;
    }

#ifdef AUTO_FROB
  if (b == LVL_WIZARD)
    b = LVL_STDWIZ;
#endif

    setplev(pl, b);
    setptitle (pl, std_title (b, psex(pl), pclass(pl)));

    sendf (pl, "You are now %s.\n", make_title (ptitle (pl), pname (pl)));
    mudlog ("&+WSYSTEM:&N %s to level %d", pname (pl), b);
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (pl), LVL_WIZARD),
              LVL_MAX, NOBODY, NOBODY,
             "&+B[&+W%s &+wis now a %s (level &+C%d&N)&+B]\n", 
              pname (pl), player_level (pl), plev (pl));

    if (update_skills) {
      if (oldlev < b)
        give_skills(mbits(pl), pl, oldlev, b);
      else
        take_skills(mbits(pl), pl, oldlev, b);
    }

    if (b >= LVL_WIZARD) {
      set_xpflags(mbits(pl), plev(pl));
      send_msg(DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY, 
            "Trumpets sound to praise &+W%s&+w, the new %s.\n",
             pname(pl), player_level(pl));
      sendf(pl, "\001f%s\003", GWIZ);
      update_wizlist (pname (pl), wlevel(pl));
      return;
    }
  }
  if (pstr (pl) > (b = maxstrength (pl)))
    setpstr (pl, b);

  if (pmagic (pl) > (b = maxmagic (pl)))
    setpmagic (pl, b);
}

void calib_player (int pl) {
  calib_player_impl(pl, True);
}

/* Frob applies skills once, from the original level to the calibrated level. */
void calib_player_without_skills (int pl) {
  calib_player_impl(pl, False);
}

void give_skills(long int bits[], int pl, int oldlev, int newlev) {
  int i;

  for (i = oldlev + 1 ; i <= newlev ; i++)
    switch (pclass(pl)) {
    case WARRIOR:
      warrior_level(False, bits, pl, i);
      break;
    case MAGE:
      mage_level(False, bits, pl, i);
      break;
    case THIEF:
      thief_level(False, bits, pl, i);
      break;
    case PRIEST:
      priest_level(False, bits, pl, i);
      break;
    }
}

void take_skills(long int bits[], int pl, int oldlev, int newlev) {
  int i;

  for (i = oldlev ; i > newlev ; i--)
    switch (pclass(pl)) {
    case WARRIOR:
      warrior_level(True, bits, pl, i);
      break;
    case MAGE:
      mage_level(True, bits, pl, i);
      break;
    case THIEF:
      thief_level(True, bits, pl, i);
      break;
    case PRIEST:
      priest_level(True, bits, pl, i);
      break;
    }
}

void armor_inc(Boolean take, int plr, int num) {
  char *msg1 = "You feel the tone of your body increase from experience.\n";
  char *msg2 = "You feel much less powerful.\n";

  if (take) {
    sendf(plr, msg2);
    parmor(plr) -= num;
  }
  else {
    sendf(plr, msg1);
    parmor(plr) += num;
  }
}

void str_inc(Boolean take, int plr, int num) {
  char *msg1 = "You feel much more powerful.\n";
  char *msg2 = "You feel the tone of your body weaken from lost experience.\n";

  if (take) {
    sendf(plr, msg2);
    pstr(plr) -= num;
  }
  else {
    sendf(plr, msg1);
    pstr(plr) += num;
  }
}

void warrior_level(Boolean take, long int bits[], int plr, int lev) {

  switch (lev) {
  case 21:
    str_inc(take, plr, 4);
    break;
  case 19:
    armor_inc(take, plr, 4);
    break;
  case 16:
    str_inc(take, plr, 3);
    break;
  case 13:
    armor_inc(take, plr, 3);
    break;
  case 10:
    str_inc(take, plr, 2);
    break;
  case 6:
    sendf(plr, "Congrats warrior, you may now use the &+WBASH&N command.\n");
    break;
  case 3:
    armor_inc(take, plr, 2);
    break;
  case 2:
    if (take) {
      sendf(plr, "You have forgotten the WHERE spell.\n");
      pclr_flg(bits, EFL_WHERE, EFLAGS);
    }
    else {
      sendf(plr, "You have learned the WHERE spell.\n");
      pset_flg(bits, EFL_WHERE, EFLAGS);
    }
    break;
  }
}

void mage_level(Boolean take, long int bits[], int plr, int lev) {
  switch (lev) {
  case 18: 
    if (take) {
      sendf(plr, "You have forgotten the &+WICESTORM&N spell.\n");
      pclr_flg(bits, EFL_ICESTORM, EFLAGS);
    }
    else {
      sendf(plr, "You have learned the &+WICESTORM&N spell.\n");
      pset_flg(bits, EFL_ICESTORM, EFLAGS);
    }
    break;
  case 14: 
    if (take) {
      sendf(plr, "You have forgotten the &+WVTOUCH&N spell.\n");
      pclr_flg(bits, EFL_VTOUCH, EFLAGS);
    }
    else {
      sendf(plr, "You now have knowledge of the &+WVTOUCH&N spell.\n"); 
      pset_flg(bits, EFL_VTOUCH, EFLAGS);
    }
    break;
  case 12: 
    if (take) {
      sendf(plr, "You have forgotten the &+WFIREBALL&N spell.\n");
      pclr_flg(bits, EFL_FIREBALL, EFLAGS);
    }
    else {
      sendf(plr, "Congrats, you have learned the &+WFIREBALL&N spell.\n"); 
      pset_flg(bits, EFL_FIREBALL, EFLAGS);
    }
    break;
  case 10:
    if (take)
      sendf(plr, "You have forgotten the &+WPOSE&N spell.\n");
    else 
      sendf(plr, "Congrats, you have gained knowledge of the &+WPOSE&N spell.\n");
    break;
  case 8:
    if (take) {
      sendf(plr, "You have forgotten the &+WFROST&N spell.\n");
      pclr_flg(bits, EFL_FROST, EFLAGS);
    }
    else {
      sendf(plr, "You have gained the &+WFROST&N spell.\n");
      pset_flg(bits, EFL_FROST, EFLAGS);
    }
    break;
  case 6:
    if (take) {
      sendf(plr, "You have forgotten the &+WBLIND&N spell.\n");
      pclr_flg(bits, EFL_BLIND, EFLAGS);
    }
    else {
      sendf(plr, "Congrats, you have learned the &+CBLIND&N spell.\n");
      pset_flg(bits, EFL_BLIND, EFLAGS);
    }
    break;
  case 5: 
    if (take) {
      sendf(plr, "You have forgotten the &+WMISSILE&N spell.\n");
      pclr_flg(bits, EFL_MISSILE, EFLAGS);
    }
    else {
      sendf(plr, "From old manuscripts you have decyphered the "
           "&+WMISSILE&N spell.\n"); 
      pset_flg(bits, EFL_MISSILE, EFLAGS);
      break;
    }
  case 4:
    if (take) {
      sendf(plr, "You have forgotten the &+WCRIPPLE&N spell.\n");
      pclr_flg(bits, EFL_CRIPPLE, EFLAGS);
    }
    else {
      sendf(plr, "You can now use the &+CCRIPPLE&N spell to cripple your "
        "opponents.\n");
      pset_flg(bits, EFL_CRIPPLE, EFLAGS);
    }
    break;
  case 3:
    if (take) {
      sendf(plr, "You have forgotten the &+WDEAFEN&N and &+WMUTE&N spells.\n");
      pclr_flg(bits, EFL_DEAFEN, EFLAGS);
      pclr_flg(bits, EFL_MUTE, EFLAGS);
    }
    else {
      sendf(plr, "You have learned the &+CDEAFEN&N and &+CMUTE&N spells.\n");
      pset_flg(bits, EFL_DEAFEN, EFLAGS);
      pset_flg(bits, EFL_MUTE, EFLAGS);
    }
    break;
  case 2:
    if (take) {
      sendf(plr, "You have forgotten the &+WWHERE&N spell.\n");
      pclr_flg(bits, EFL_WHERE, EFLAGS);
    }
    else {
      sendf(plr, "You have learned the WHERE spell.\n");
      pset_flg(bits, EFL_WHERE, EFLAGS);
    }
    break;
  }
}

void thief_level(Boolean take, long int bits[], int plr, int lev) {
  switch (lev) {
    case 2:
      if (take) {
        sendf(plr, "You have forgotten the &+WWHERE&N spell.\n");
        pclr_flg(bits, EFL_FIREBALL, EFLAGS);
      }
      else {
        sendf(plr, "You have learned the WHERE spell.\n");
        pset_flg(bits, EFL_WHERE, EFLAGS);
      }
      break;
    case 4:
      if (take)
        sendf(plr, "You have lost the ability to &+WBACKSTAB\n");
      else 
        sendf(plr, "Excellent work thief, you may now use the "
          "&+WBACKSTAB&N command.\n");
      break;
    case 6:
      if (take)
        sendf(plr, "You have lost the ability to &+Wlock pick\n");
      else 
        sendf(plr, "Good job, you have gained the skill of lock picking.\n");
      break;
    case 8:
      if (take)
        sendf(plr, "You have lost the ability to &+WSTEAL\n");
      else 
        sendf(plr, "Good work, you have gained proficiency in stealing.\n");
      break;
    case 10:
      if (take)
        sendf(plr, "You have lost the ability to &+WSNEAK\n");
      else 
        sendf(plr,"Good work, thief.  You may now use the &+WSNEAK&N "
          "command.\n");
      break;
  }
}

void priest_level(Boolean take, long int bits[], int plr, int lev) {
  switch (lev) {
    case 2:
      if (take) {
        sendf(plr, "You have forgotten the WHERE spell.\n");
        pclr_flg(bits, EFL_WHERE, EFLAGS);
      }
      else {
        sendf(plr, "You have learned the WHERE spell.\n");
        pset_flg(bits, EFL_WHERE, EFLAGS);
      }
      break;
    case 3:
      if (take) {
        sendf(plr, "You have lost immunity to cripple, blind, mute & deaf.\n");
        pclr_flg(bits, EFL_I_CRIPPLE, EFLAGS);
        pclr_flg(bits, EFL_I_BLIND, EFLAGS);
        pclr_flg(bits, EFL_I_MUTE, EFLAGS);
        pclr_flg(bits, EFL_I_DEAFEN, EFLAGS);
      }
      else {
        sendf(plr, "You have gained immunity to many spells (cripple, "
                  "blind, mute, deaf)\n");
        pset_flg(bits, EFL_I_CRIPPLE, EFLAGS);
        pset_flg(bits, EFL_I_BLIND, EFLAGS);
        pset_flg(bits, EFL_I_MUTE, EFLAGS);
        pset_flg(bits, EFL_I_DEAFEN, EFLAGS);
      }
      break;
    case 4:
      if (take)
        sendf(plr, "You lost the ability to meditate.\n");
      else 
        sendf(plr, "Good work, Priest.  You have gained the skill of "
             "meditation.\nYou will now meditate whenever you sit to rest.\n");
      break;
    case 5:
      if (take) {
        sendf(plr, "You have forgotten the &+WLIT&N spell.\n");
        pclr_flg(bits, EFL_LIGHT, EFLAGS);
      }
      else {
        sendf(plr, "You have learned the &+WLIT&N spell.\n");
        pset_flg(bits, EFL_LIGHT, EFLAGS);
      }
      break;
    case 6:
      if (take) {
        sendf(plr, "You lose your ability to use the &+WAID&N spell.\n");
        pclr_flg(bits, EFL_AID, EFLAGS);
      }
      else {
        sendf(plr, "You may now use &+WAID&N to replenish health.\n");
        pset_flg(bits, EFL_AID, EFLAGS);
      }
    break;
  case 8:
    if (take) {
      sendf(plr, "You may forgotten the &+WBHANDS&N spell.\n");
      pclr_flg(bits, EFL_BHANDS, EFLAGS);
    }
    else {
      sendf(plr, "You now have knowledge of the &+WBHANDS&N spell.\n");
      pset_flg(bits, EFL_BHANDS, EFLAGS);
    }
    break;
  case 10:
    if (take)
      sendf(plr, "You may no longer use &+RBANG&N.\n");
    else
      sendf(plr, "Excellent work, priest.  You may now use &+RBANG&N.\n");   
    break;
  case 15:
    if (take) {
      sendf(plr, "You may no longer use the &+WPROTECT&N spell.\n");
      pclr_flg(bits, EFL_PROTECT, EFLAGS);
    }
    else {
      sendf(plr, "Congrats Priest, you have learned the &+WPROTECT&N spell.\n");
      pset_flg(bits, EFL_PROTECT, EFLAGS);
    }
    break;
  case 17:
    if (take) {
      sendf(plr, "You have forgotten the &+WDAMAGE&N spell.\n");
      pclr_flg(bits, EFL_DAMAGE, EFLAGS);
    }
    else {
      sendf(plr, "You have gained knowledge of the &+WDAMAGE&N spell.\n");
      pset_flg(bits, EFL_DAMAGE, EFLAGS);
    }
    break; 
  case 19:
    if (take) {
      sendf(plr, "You may no longer use the &+WSCHOCK&N spell.\n");
      pclr_flg(bits, EFL_SHOCK, EFLAGS);
    }
    else {
      sendf(plr, "You have gained knowledge of the &+WSHOCK&N spell.\n");
      pset_flg(bits, EFL_SHOCK, EFLAGS);
    }
    break;
  }
}

void calibme () {
  calib_player (mynum);
}

int wlevel (int lev) {
  if (lev < 0)
    return LEV_NEG;
  else if (lev < LVL_WIZARD)
    return LEV_MORTAL;
  else if (lev == LVL_WIZARD)
    return LEV_APPRENTICE;
  else if (lev < LVL_STDWIZ)
    return LEV_EMERITI;
  else if (lev < LVL_ISTARI)
    return LEV_WIZARD;
  else if (lev < LVL_ARCHWIZARD)
    return LEV_ISTARI;
  else if (lev < LVL_DEMI)
    return LEV_ARCHWIZARD;
  else if (lev < LVL_SHALAFI)
    return LEV_DEMI;
  else if (lev < LVL_GOD)
    return LEV_SHALAFI;
  else if (lev < LVL_CREATOR)
    return LEV_GOD; 
  else
    return LEV_CREATOR;
}

void levelscom (void)
{
  int i;

  bprintf("Level   Points        Warrior     Mage        Thief       Priest\n"
          "----------------------------------------------------------------\n");

  for (i = 1; i <= LVL_WIZARD; i++)
    bprintf ("%-5d   %6d        %-11s %-11s %-11s %-11s\n", i, levels[i], 
      WarriorLevels[i], MageLevels[i], ThiefLevels[i], PriestLevels[i]);
  bprintf ("\n");
}

