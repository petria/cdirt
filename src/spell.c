
#include <stdlib.h>
#include "kernel.h"
#include "sendsys.h"
#include "verbs.h"
#include "mobiles.h"
#include "objects.h"
#include "locations.h"
#include "objsys.h"
#include "spell.h"
#include "spells.h"
#include "bprintf.h"
#include "parse.h"
#include "fight.h"
#include "mobile.h"

Boolean cast_spell(int, int, struct SPELL *);

void spellscom (void) {
  struct SPELL *spell;
  char dam[4], type[9];

  if ((pclass(mynum) != PRIEST && pclass(mynum) != MAGE) 
    && plev(mynum) < LVL_ISTARI && !etstflg(mynum, EFL_WHERE)) {
    bprintf("You have no spells.\n");
    return;
  }

  bprintf ("You know the following spells:\n");
  bprintf ("&+B---------------------------------------------------------------\n");
  bprintf ("&+WCommand    Spell                Mana  Damage  Chance  Type\n");
  bprintf ("&+B---------------------------------------------------------------\n");

  for (spell = spell_table; spell->verb >= 0; spell++) {
    if (etstflg(mynum, spell->flag) || plev(mynum) >= LVL_ISTARI) {
      sprintf (dam, "%d", spell->damage);
      sprintf (type, "%s", spell->type == ATTACK ? "Attack" :
	       spell->type == HEAL ? "Heal" :
	       spell->duration ? "Duration" : "Special");
      bprintf ("&*%-10.10s %-20.20s %4d  %6.6s  %6d  %-9.9s\n",
	       spell->verbname, spell->name, spell->mana,
	       spell->damage ? dam : "---", spell->chance, type);
    }
  }
  bprintf ("&+B---------------------------------------------------------------\n");
}

void spellcom (int verb) {
  struct SPELL *spell;
  int victim = brkword () < 0 ? pfighting (mynum) : fmbn (wordbuf);

  for (spell = spell_table; spell->verb != verb; spell++)
    if (spell->verb < 0)
      return;

  if (ststflg(mynum, SFL_DUMB) && plev(mynum) < LVL_WIZARD) {
    bprintf("Sorry, you been struck dumb.\n");
    return;
  }
  else if (!is_in_game(victim)) {
    bprintf ("Who?\n");
    return;
  }
  else if ((verb==VERB_BABEL || verb==VERB_MAKEPIG) && victim >= max_players) {
    bprintf("Those spells are for human players only.\n");
    return;
  }
  else if (plev(mynum) < LVL_ISTARI) {
    if (cur_player->cast_spell && pfighting(mynum))
      bprintf("You can't cast more than one spell per round.\n");
    else if (!etstflg(mynum, spell->flag))
      bprintf("You have not learned this spell yet.\n");
    else if (ploc(mynum) != ploc(victim))
      bprintf("%s is not here.\n", pname(victim));
    else if (pstr(victim) < 0)
      bprintf("%s is dead.\n", pname(victim));
    else if (pmagic(mynum) < spell->mana)
      bprintf("You are to weak to cast this spell.\n");
    else if (ltstflg (ploc (victim), LFL_NO_MAGIC))
      bprintf("Something about this area made you to fumble the magic.\n");
    else
      cast_spell (mynum, victim, spell); 
  }
  else
    cast_spell (mynum, victim, spell);
}

Boolean cast_spell (int caster, int victim, struct SPELL *spell) {
  int chance, damage;
  int factor = 1;

  Boolean miss = False;
  damage = 0;

  if (plev (caster) < LVL_WIZARD) {
    if (spell->type == ATTACK) {
      if (testpeace (caster)) {
        sendf (caster, "No, that's violent!\n");
        return False;
      }
      else if (victim < max_players && linkdead(victim)) {
        sendf(caster, "That player is linkdead.\n");
        return False;
      }
    }

   if (caster < max_players && victim < max_players && 
      !ltstflg(ploc(caster), LFL_PKZONE) && spell->type == ATTACK ) {
      bprintf("You cannot attack another player here!\n");
      send_msg(DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
                "&#&+R[&+Y%s &+Rhas tried to kill &+Y%s&+R]\n",
                pname(caster), pname(victim));
      sendf(victim, "%s tried to attack you!\n", pname(caster));
      return False;
    }

    chance = spell->chance;

#ifdef LOCMIN_BLIZZARD
    if (carries_obj_type (caster, OBJ_BLIZZARD_POWERSTONE) > -1)
      ++chance;
    if (carries_obj_type (caster, OBJ_BLIZZARD_POWERSTONE1) > -1)
      ++chance;
    if (carries_obj_type (caster, OBJ_BLIZZARD_POWERSTONE2) > -1)
      ++chance;
#endif

#ifdef LOCMIN_FANTASY
    if (carries_obj_type (caster, OBJ_FANTASY_MANA) > -1)
      chance += 2;
#endif

    if (caster < max_players) {
      players[caster].cast_spell = True;
      if (randperc () > (chance + 4 * plev(caster)))
        miss = True;
    }
    else 
     if (randperc() > (chance * CHANCE_MULTIPLIER))
       miss = True;

    if (ptstflg (victim, PFL_NOHASSLE)) {
      sendf (caster, "Something prevents you from casting your spell.\n");
      return False;
    }
  }

  /* Attack type spell */
  if (spell->type == ATTACK) {
    if (caster == victim) {
      sendf (victim, "You're supposed to be killing others, not yourself.\n");
      return False;
    }
    if( caster < max_players )
      setpmagic (caster, pmagic (caster) - spell->mana);

    send_magic_msg (caster, caster, victim, spell->spell_msg, CASTER);
    send_magic_msg (victim, caster, victim, spell->spell_msg_vic, VICTIM);
    send_magic_msg (ploc (caster), caster, victim, spell->to_room, ROOM);

    if (miss) {
      send_magic_msg (caster, caster, victim, spell->to_casterm, TOCAST);
      send_magic_msg (victim, caster, victim, spell->to_victimm, TOVIC);
      send_magic_msg (ploc(caster), caster, victim, spell->to_othersm, TOROOM);
      return False;
    }
    if (plev(caster) < LVL_WIZARD && wears_antimagic(victim)) {
      if (randperc() < 50) { 
        sendf(caster, "The spell flashes harmlessly past %s!\n",
              his_or_her (victim));
        send_msg (sendloc(caster), 0, LVL_MIN, LVL_MAX, caster, victim,
		  "\001p%s\003 casts a spell on \001p%s\003. It flashes "
                  "harmlessly past %s!\n", pname (caster), pname (victim),
                  his_or_her (victim));
        sendf(victim, "%s casts a spell on you, but if flashes past "
                      "harmlessly!", see_name (victim, caster));
        return(False);
      }
      else {
        sendf(caster, "%s's antimagic item fails to protect %s!\n",
          pname(victim), psex(victim) ? "her" : "him");
        sendf(victim, "Your antimagic item fails to protect you.\n");
      }
    }
    if (spell->immune_flag != NOFLAG && etstflg (victim, spell->immune_flag)) {
      send_magic_msg (caster, caster, victim, spell->to_casteri, TOCAST);
      send_magic_msg (victim, caster, victim, spell->to_victimi, TOVIC);
      send_magic_msg (ploc(caster), caster, victim, spell->to_othersi, TOROOM);
      return False;
    }
    if (spell->fear_flag != NOFLAG && etstflg (victim, spell->fear_flag))
      factor += 4;

    if (check_object (caster, spell->obj_flag))
      factor += 2;

    send_magic_msg (caster, caster, victim, spell->to_caster, TOCAST);
    send_magic_msg (victim, caster, victim, spell->to_victim, TOVIC);
    send_magic_msg (ploc (caster), caster, victim, spell->to_others, TOROOM);

    /* calculate damage */

    if (plev (caster) < LVL_WIZARD)
      damage = spell->damage * factor;
    else
      damage = pstr (victim) + 1;

    /* vtouch : a special attack spell */

    if (spell->verb == VERB_VTOUCH) {
      if (etstflg(victim, EFL_N_VTOUCH)) {
	 sendf (caster, "%s drains your life instead!\n", pname(victim));
         pscore (caster) = pscore (caster) - 100 * damage;
         pscore (victim) = pscore (victim) + 100 * damage;
      }
      else {
        pscore (caster) = pscore (caster) + 100 * damage;
        pscore (victim) = pscore (victim) - 100 * damage;
        sendf (caster, "&*You gain &+W%d &*experience points!\n", damage * 100);
      }
      if (pfighting(victim) == -1)
        hit_player (victim, caster, pwpn(victim));
      return(True);
    }

    /* account for all ATTACK spells here, all require Neg entry */


    if (spell->verb == VERB_FIREBALL && etstflg(victim, EFL_N_FIREBALL))
       sendf (caster, "%s enjoys the &+Rheat&N and gets STRONGER!\n", pname(victim));
    else if (spell->verb == VERB_SHOCK && etstflg(victim, EFL_N_SHOCK))
       sendf (caster, "%s grows stronger from the charge!\n", pname(victim));
    else if (spell->verb == VERB_MISSILE && etstflg(victim, EFL_N_MISSILE))
       sendf (caster, "%s absorbs the &+Wmissile&N and grows STRONGER!\n", pname(victim));
    else if (spell->verb == VERB_FROST && etstflg(victim, EFL_N_FROST))
       sendf (caster, "%s likes the &+Ccold&N!\n", pname(victim));
    else if (spell->verb == VERB_ICESTORM && etstflg(victim, EFL_N_ICESTORM))
       sendf (caster, "%s relaxes and enjoys the cold weather.\n", pname(victim));
    else {
      switch (spell->verb) {
        case VERB_DEAFEN:
          add_duration(victim, VERB_DEAFEN, spell->duration, 0);
          ssetflg(victim, SFL_DEAF);
          break;
        case VERB_CRIPPLE:
          add_duration(victim, VERB_CRIPPLE, spell->duration, 0);
          ssetflg(victim, SFL_CRIPPLED);
          break;
        case VERB_MUTE:
          add_duration(victim, VERB_MUTE, spell->duration, 0);
          ssetflg(victim, SFL_DUMB);
          break;
        case VERB_BLIND:
          add_duration(victim, VERB_BLIND, spell->duration, 0);
          ssetflg(victim, SFL_BLIND);
          break;
        default:
          setpstr(victim, pstr(victim) - damage);         /* a normal spell: */
          if (pstr(victim) < 0)                           /* this place is */
            player_died(caster, victim, spell->verb);     /* where all damage */
        }
        if (pfighting(victim) == -1 && plev(caster) < LVL_WIZARD)
          hit_player (victim, caster, pwpn(victim));
        return(True);
    }

    setpstr(victim, pstr(victim) + damage);  /* fall thru -  spell negated */
    if (pfighting(victim) == -1)
      hit_player (victim, caster, pwpn(victim));
    return True;
  }

  /* Heal type spell */

  if (spell->type == HEAL) {
    setpmagic (caster, pmagic (caster) - spell->mana);

    send_magic_msg (caster, caster, victim, spell->spell_msg, CASTER);
    send_magic_msg (victim, caster, victim, spell->spell_msg_vic, VICTIM);
    send_magic_msg (ploc (caster), caster, victim, spell->to_room, ROOM);

    if (miss) {
      send_magic_msg (caster, caster, victim, spell->to_casterm, TOCAST);
      send_magic_msg (victim, caster, victim, spell->to_victimm, TOVIC);
      send_magic_msg (ploc (caster), caster, victim, spell->to_othersm, TOROOM);
      return False;
    }
    if (check_object (caster, spell->obj_flag))
      factor += 2;

    send_magic_msg (caster, caster, victim, spell->to_caster, TOCAST);
    send_magic_msg (victim, caster, victim, spell->to_victim, TOVIC);
    send_magic_msg (ploc (caster), caster, victim, spell->to_others, TOROOM);

    damage = spell->damage * factor;
    setpstr (victim, pstr (victim) + damage);
  }

  if (spell->type == SPECIAL) {
    if (victim >= max_players) {
      if (spell->verb != VERB_CRIPPLE && spell->verb != VERB_DEAFEN &&
         spell->verb != VERB_MUTE && spell->verb != VERB_BLIND)
        sendf (caster, "That doesn't make any sense.\n");
      return(False);
    }
    if (check_duration (victim, spell->verb)) {
      sendf(caster, "%s are already under the effects of that spell.\n",
	     caster == victim ? "You" : "They");
      return(False);
    }

    setpmagic(caster, pmagic (caster) - spell->mana);
    send_magic_msg (caster, caster, victim, spell->spell_msg, CASTER);
    send_magic_msg (victim, caster, victim, spell->spell_msg_vic, VICTIM);
    send_magic_msg (ploc (caster), caster, victim, spell->to_room, ROOM);

    if (miss) {
      send_magic_msg (caster, caster, victim, spell->to_casterm, TOCAST);
      send_magic_msg (victim, caster, victim, spell->to_victimm, TOVIC);
      send_magic_msg (ploc (caster), caster, victim, spell->to_othersm, TOROOM);
      return(False);
    }
    if (plev (mynum) < LVL_WIZARD) {
      if (spell->immune_flag != NOFLAG && etstflg(victim, spell->immune_flag)) {
	send_magic_msg (caster, caster, victim, spell->to_casteri, TOCAST);
	send_magic_msg (victim, caster, victim, spell->to_victimi, TOVIC);
	send_magic_msg (ploc (caster), caster, victim, spell->to_othersi, TOROOM);
	return False;
      }

      if (spell->fear_flag != NOFLAG && etstflg (victim, spell->fear_flag))
	factor += 4;
      if (check_object (caster, spell->obj_flag))
	factor += 2;

      send_magic_msg (caster, caster, victim, spell->to_caster, TOCAST);
      send_magic_msg (victim, caster, victim, spell->to_victim, TOVIC);
      send_magic_msg (ploc (caster), caster, victim, spell->to_others, TOROOM);
      damage = spell->damage * factor;
    }

    /* spell has succeeded by this point */

    switch (spell->verb) {
      case VERB_LIT:
        if (add_duration(victim, VERB_LIT, spell->duration * factor, 0))
  	  ssetflg(victim, SFL_LIT);
        break;
      case VERB_DAMAGE:
        if (add_duration(victim, VERB_DAMAGE, spell->duration * factor, pdam(victim)))
  	  setpdam(victim, pdam (victim) + 10);
        break;
      case VERB_ARMOR:
        if (add_duration(victim, VERB_ARMOR, spell->duration * factor,
            parmor (victim))) {
  	  setparmor (victim, parmor (victim) + 10);
          calc_ac(victim);
        }
        break;
      case VERB_BLUR:
        add_duration (victim, VERB_BLUR, spell->duration, 0);
        break;
      case VERB_BABEL:
        add_duration(victim, VERB_BABEL, spell->duration, 0);
        ssetflg(victim, SFL_BABEL);
        send_msg(DEST_ALL, 0, LVL_ARCHWIZARD, LVL_MAX, mynum, victim,
	         "&+r[&+Y%s has cast babel spell on %s&+r]\n", pname(mynum),
	         pname(victim));
        break;
   
      case VERB_MAKEPIG:
        add_duration(victim, VERB_MAKEPIG, spell->duration, 0);
        ssetflg(victim, SFL_ISPIG);
        send_msg(DEST_ALL, 0, LVL_ARCHWIZARD, LVL_MAX, mynum, victim,
	         "&+r[&+y%s has cast makepig spell on %s&+r]\n", pname(mynum),
	         pname(victim));
        break;
    }
  }
  return(True);
}

/* improved & more-random */

void mob_cast_spell (int caster, int victim) {
  struct SPELL *spell;
  struct SPELL *choices[32];
  int len;

  if (ststflg(caster, SFL_DUMB))
    return; 

  for (spell = spell_table, len = 0 ; spell->verb >= 0 ; spell++) {
    if (etstflg(caster, spell->flag) && spell->type == ATTACK) {
      choices[len] = spell;
      len++;
    }
  }

  if (!len || plev(victim) >= LVL_WIZARD ||
     (randperc() > SPELL_CHANCE + len * SPELL_INCR))
    return;
  else
    cast_spell(caster, victim, choices[randperc() % len]);
}

Boolean
check_object (int plr, int flag)
{
  int i;

  if (flag == NOFLAG)
    return False;

  for (i = 0; i < pnumobs (plr); i++)
    if (iscarrby (pobj_nr (i, plr), flag) && otstbit (i, flag))
      return True;

  return False;
}

void send_magic_msg (int dest, int caster, int victim, char *msg, int type)
{
  char xx[120];
  char c[30], v[30];

  if (msg == NULL)
    return;

  sprintf (c, "\001p%s\003", pname (caster));
  sprintf (v, "\001p%s\003", pname (victim));

  if (type == CASTER && caster == victim)
    sprintf (v, "yourself");
  if (type == ROOM && caster == victim)
    sprintf (v, "%sself", psex (caster) ? "her" : "him");
  if (type == VICTIM && caster == victim)
    return;
  if (type == TOCAST && caster == victim)
    return;

  if (type == ROOM || type == TOROOM)
    send_msg(int2idx(dest, LOC), 0, LVL_MIN, LVL_MAX, caster, victim,
	      make_magic_msg (xx, msg, c, v));
  else
    send_msg(int2idx(dest, MOB), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
	      make_magic_msg (xx, msg, c, v));
}

char *
make_magic_msg (char *b, char *s, char *c, char *v)
{
  char *p, *q, *r;

  for (p = b, q = s; *q != 0;) {
    if (*q != '%')
      *p++ = *q++;
    else {
      switch (*++q) {
      case 'c':		/* Caster */
	if (c == NULL)
	  return NULL;
	for (r = c; *r != 0;)
	  *p++ = *r++;
	break;
      case 'v':		/* Victim */
	if (v == NULL)
	  return NULL;
	for (r = v; *r != 0;)
	  *p++ = *r++;
	break;
      case 0:
	--q;
	break;
      default:
	*p++ = *q;
      }
      ++q;
    }
  }
  *p = 0;
  return b;
}

/************************************************************************
 * Spell Duration Handler Functions					*
 ************************************************************************/

/* This function wipes out all of the duration data. */

void wipe_duration (int plr) {
  SPELL_DURATION *ptr, *fptr;

  ptr = players[plr].duration;

  while (ptr) {
    duration_end(plr, ptr->spell, ptr->tmp);
    fptr = ptr;
    ptr = ptr->next;
    FREE (fptr);
  }
}

void duration_end (int plr, int spell, int tmp) {
  switch (spell) {
    case VERB_LIT:
      sendf (plr, "&+MYour &+Ylit &+Mspell expires.\n");
      sclrflg (plr, SFL_LIT);
      break;
    case VERB_DAMAGE:
      sendf (plr, "&+MYou feel your &+Ystrength &+Mspell wearing off.\n");
      setpdam (plr, tmp);
      break;
    case VERB_ARMOR:
      sendf (plr, "&+MYou feel your protective &+Yarmor &+Mspell wearing off.\n");
      setparmor (plr, tmp);
      calc_ac(plr);
      break;
    case VERB_BLUR:
      sendf (plr, "&+MYou seem to be more visible as the &+Yblur &+Mspell "
             "wears off.\n");
      break;
    case VERB_BABEL:
      sendf (plr, "You don't feel like talking as much anymore!\n");
      sclrflg(plr, SFL_BABEL);
      break;
    case VERB_MAKEPIG:
      sendf (plr, "You don't feel like a pig anymore!\n");
      sclrflg(plr, SFL_ISPIG);
      break;
    case VERB_DEAFEN:
      sendf (plr, "You can hear again!\n");
      sclrflg(plr, SFL_DEAF);
      break;
    case VERB_CRIPPLE:
      sendf (plr, "You can walk again!\n");
      sclrflg(plr, SFL_CRIPPLED);
      break;
    case VERB_BLIND:
      sendf (plr, "You can see again!\n");
      sclrflg(plr, SFL_BLIND);
      break;
    case VERB_MUTE:
      sendf (plr, "You can talk again!\n");
      sclrflg(plr, SFL_DUMB);
      break;
  }
}

/* This function adds a spell to a player's duration linklist. */

Boolean add_duration (int plr, int spell, int duration, int tmp) {
  /* Convert minutes to seconds. Since the MUD interrupts
   * once every 2 seconds though, we multiply by 30. 
   */
  duration = duration * 30;

  /* Ptr to see if the spell is already being used */
  if (!check_duration (plr, spell)) {
    push_duration (plr, spell, duration, tmp);
    return True;
  } 
  else
    return False;
}

/* This function checks if a spell is currently being used.
 */
Boolean
check_duration (int plr, int spell)
{
  SPELL_DURATION *ptr;

  if (plr >= max_players)
    return False;

  ptr = players[plr].duration;

  /* Check to see if the spell is already being used */
  while (ptr != NULL) {
    if (ptr->spell == spell)
      return True;
    ptr = ptr->next;
  }
  return False;
}

/* This function handles going through the linklist to decrement the
 * duration and check the spells */

void handle_duration (int plr)
{
  SPELL_DURATION *ptr;
  SPELL_DURATION *prev;
  SPELL_DURATION *temp;

  /* Decrement all spells, disable if it ran out, and FREE memory */

  ptr = players[plr].duration;
  prev = NULL;

  while (ptr != NULL) {
    if (!ptr->duration--) {
      duration_end (plr, ptr->spell, ptr->tmp);
      if (!prev) {                          /* prev = NULL */
	temp = ptr;
	ptr = ptr->next;
	players[plr].duration = ptr;        /* new first node */     
        FREE (temp);
      } 
      else {                                /* prev unchanged */
	prev->next = ptr->next;
	temp = ptr;
	ptr = ptr->next;
	FREE (temp);
      }
    }
    else {                                  /* new prev */
      prev = ptr;
      ptr = ptr->next;
    }
  }
}

/* This function pushes the linklist to add another duration pointer
 * into it.
 */
void
push_duration (int plr, int spell, int duration, int tmp)
{
  SPELL_DURATION *new;

  new = NEW (SPELL_DURATION, 1);
  new->spell = spell;
  new->duration = duration;
  new->tmp = tmp;

  new->next = players[plr].duration;
  players[plr].duration = new;
}
