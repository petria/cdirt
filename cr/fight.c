#include "kernel.h"
#include "mobile.h"
#include "sflags.h"
#include "sendsys.h"
#include "stdlib.h"
#include "mud.h"
#include "group.h"
#include "bprintf.h"
#include "mudmacros.h"
#include "cflags.h"
#include "parse.h"
#include "commands.h"
#include "rooms.h"
#include "uaf.h"
#include "mobiles.h"
#include "objects.h"
#include "locations.h"
#include "verbs.h"
#include "mflags.h"
#include "pflags.h"
#include "lflags.h"
#include "oflags.h"
#include "fight.h"
#include "fightmsg.h"
#include "quests.h"
#include "move.h"
#include "aflags.h"
#include "objsys.h"
#include "clone.h"
#include "spell.h"

void __fightmsg(int attacker, int victim, 
                Bodypart *area, char *msg[], int num_ent) 
{
   int i;

   i = my_random() % num_ent;
   GMSG(msg[i]);
}

void __generalmsg(int attacker, int victim, Bodypart *area, char *msg) {
  char *text = __add_codes(attacker, victim, area, ATTACKER, msg);
  sendf(attacker, "%s", text); free(text);
  text = __add_codes(attacker, victim, area, VICTIM, msg);
  sendf(victim, "%s", text); free(text);
  text = __add_codes(attacker, victim, area, OTHER, msg);
  send_msg(sendloc(attacker), MODE_NSFLAG | MS(SFL_NOFIGHT),
           LVL_MIN, LVL_MAX, attacker, victim, "%s", text);
  free(text);
}

void set_player_parts(int plr) {
  bodyname(head(plr)) = plr_head;
  bodyname(r_arm(plr)) = plr_right_arm;
  bodyname(l_arm(plr)) = plr_left_arm;
  bodyname(r_leg(plr)) = plr_right_leg;
  bodyname(l_leg(plr)) = plr_left_leg;
  bodyname(r_foot(plr)) = plr_right_foot;
  bodyname(l_foot(plr)) = plr_left_foot;
  bodyname(r_hand(plr)) = plr_right_hand;
  bodyname(l_hand(plr)) = plr_left_hand;
  bodyname(chest(plr)) = plr_chest;
  bodyname(back(plr)) = plr_back;
  bodyname(face(plr)) = plr_face;
  bodyname(neck(plr)) = plr_neck;
}

void setpfighting (int x, int y) {
  int ct, mob, loc = ploc (x);

  pfighting (x) = y;

  if (y == -1) {
    if (exists (loc)) {
      for (ct = 0; ct < lnumchars (loc); ct++) {
        mob = lmob_nr(ct, loc);
        if (pfighting (mob) == x)
          pfighting(mob) = -1;
      }
    }
  } 
  else
    pfighting (y) = x;
}

Boolean set_weapon (int plr, int wpn) {
  int owpn, i;

  /* setoloc uses -1 as a temporary location while removing objects. */
  if (plr < 0 || plr >= numchars)
    return False;

  /* Erase any weapon we were allready wielding */

  if ((owpn = pwpn (plr)) != -1 && oloc (owpn) == plr) {
    if (ocarrf (owpn) == BOTH_BY)
      setcarrf (owpn, WORN_BY);
    else if (ocarrf (owpn) == WIELDED_BY)
      setcarrf (owpn, CARRIED_BY);
  }
  if (wpn < 0 || odamage (wpn) == 0) {
    setpwpn (plr, -1);
    return False;
  }
  i = WIELDED_BY;

  if (ocarrf (wpn) == WORN_BY)
    i = BOTH_BY;

  setpwpn (plr, wpn);
  setcarrf (wpn, i);

  return(True);
}


int dambyitem (int pl, int it) {
  if (it == -1)
    return pdam (pl);
  return !otstbit (it, OFL_WEAPON) ? -1 : odamage (it) + pdam (pl);
}

void wieldcom (void) {
  int a;

  if ((a = cantake(ob1, "Wield what?", "What's that?")) == -1)
    return;
  else if (oloc(a) == ploc(mynum)) {
    bprintf("You have to GET it first, silly.\n");
    return;
  }
  else if (!otstbit (a, OFL_WEAPON)) {
    bprintf ("It's not a weapon.\n");
    set_weapon (mynum, -1);
    return;
  }
  set_weapon (mynum, a);

  send_msg (sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
            "%s wields the %s.\n", pname (mynum), oname (a));

  bprintf ("You are now wielding the %s.\n", oname (a));
}

char *str_color (int plr) {
  if (plr < max_players)
    return (pstr (plr) == maxstrength (plr) ? "&+W" :
            pstr (plr) < 20 ? "&+R" : "&+Y");
  else
    return (pstr (plr) == pstr_reset (plr) ? "&+W" :
            pstr (plr) < 20 ? "&+R" : "&+Y");
}

char *mag_color (int plr) {
  if (plr < max_players)
    return (pmagic (plr) == maxmagic (plr) ? "&+W" :
            pmagic (plr) < 20 ? "&+R" : "&+Y");
  else
    return(0);
}


void breakitem (int x) {
  #include "breakitem.h"
}

void restorecom(void) {
  int plr;

  if (plev(mynum) < LVL_WIZARD) {
    bprintf("Go to the hospital to heal.\n");
    return;
  }

  if (!*txt1)
    set_vital(mynum);
  else if ((plr = fpbn(txt1)) == -1)
    bprintf("No such player.\n");
  else
    set_vital(plr);
}

int set_vital_helper(Bodypart *part, int newval, FILE *notused) {
  part->vital = newval;
  part->attach = True;
  part->loc = 0;
  return(0);
}

void set_vital(int plr) {
  int newvital;

  if (plr > max_players)
    newvital = pstr_reset(plr) / MOB_VITAL_DIVISOR;
  else
    newvital = MAX_PLAYER_VITAL;    

  applyfunc(plr, set_vital_helper, newvital, NULL);
}

#define ARMOR(x) (oarmor(x) * ARMOR_MULT > 100 ? 100 : oarmor(x) * ARMOR_MULT)

Bodypart *calc_hit(int attacker, int victim, Boolean *missed,
                   int *damage, int *hitpct) 
{
  int val, w, i;
  float absorbval, hitval;
  Bodypart *hitloc = 0;
  int locpct = randperc();

  w = pwpn(attacker);
  *hitpct = randperc();
  *missed = False;
  *damage = i = 0;

  #include "hitchance.h"

  if (locpct < (val = HEAD_CHANCE) && attached(head(victim)))
    hitloc = &head(victim);
  else if (locpct < (val += RARM_CHANCE) && attached(r_arm(victim)))
    hitloc = &r_arm(victim);
  else if (locpct < (val += LARM_CHANCE) && attached(l_arm(victim)))
    hitloc = &l_arm(victim);
  else if (locpct < (val += RLEG_CHANCE) && attached(r_leg(victim)))
    hitloc = &r_leg(victim);
  else if (locpct < (val += LLEG_CHANCE) && attached(l_leg(victim)))
    hitloc = &l_leg(victim);
  else if (locpct < (val += RFOOT_CHANCE) && 
           attached(r_foot(victim)) && attached(r_leg(victim)))
    hitloc = &r_foot(victim);
  else if (locpct < (val += LFOOT_CHANCE) && 
           attached(l_foot(victim)) && attached(l_leg(victim)))
    hitloc = &l_foot(victim);
  else if (locpct < (val += RHAND_CHANCE) && 
           attached(r_hand(victim)) && attached(r_arm(victim)))
    hitloc = &r_hand(victim);
  else if (locpct < (val += LHAND_CHANCE) && 
           attached(l_hand(victim)) && attached(l_arm(victim)))
    hitloc = &l_hand(victim);
  else if (locpct < (val += CHEST_CHANCE) && attached(chest(victim)))
    hitloc = &chest(victim);
  else if (locpct < (val += BACK_CHANCE) && attached(back(victim)))
    hitloc = &back(victim);
  else if (locpct < (val += FACE_CHANCE) && attached(face(victim)))
    hitloc = &face(victim);
  else if (locpct < (val += NECK_CHANCE) && attached(neck(victim)))
    hitloc = &neck(victim);
  else
    return(hitloc);                   /* failure */

  if (*hitpct - parmor(victim) < CTH_BASE)
    *missed = True;
  else if (hasarmr(hitloc) && (ARMOR(hitloc->armor) / *hitpct > ABSORB_VAL))
    *damage = 0;
  else if (pac(victim) / *hitpct > AC_ABSORB_VAL)
    *damage = 0;
  else {    
    hitval = (float) *hitpct / 100;
    if (hasarmr(hitloc))
      absorbval = (float) (100 - ARMOR(hitloc->armor)) / 100;
    else
      absorbval = (float) 1;

    *damage =  (float) hitval * player_damage(attacker) * absorbval;

    if (victim < max_players && check_duration (victim, VERB_BLUR))
      *damage -= 5;

    #include "damage.h"

    if (*damage < 0)
      *damage = 0;
  }
  setpscore(attacker, pscore(attacker) + *damage * 2);
  return(hitloc);
}

void wreckdoor (void) {
  int a;
  char ms[80];

  if ((a = objloc(mynum, "door")) == -1)
    bprintf ("Sorry, no doors here.\n");
  else if (state (a) == 0)
    bprintf ("It's already open.\n");
  else {
    setobjstate (a, 0);
    bprintf ("The door flies open!\n");
    sprintf (ms, "%s hits the door...  It flies open!\n", pname (mynum));
    sillycom (ms);
    sprintf (ms,
         "\001dYou hear the distinctive crunch of %s meeting a door.\n\003",
             pname (mynum));
    broad (ms);
  }
}

void challengecom(void) {
  int plr;
  int attacker = real_mynum;

  if (brkword() == -1) {
    bprintf("Challenge who?");
    return;
  }
  else if ((plr = fpbn(wordbuf)) == -1) {
    bprintf("I don't know of any human character named %s.\n", wordbuf);
    return;
  }
  else if (phandler(plr) != get_command) {
    bprintf("They are busy doing something else.  Sorry.\n");
    return;
  }
  else if (testpeace(mynum)) {
    bprintf("Sorry, it's too peaceful for violence.\n");
    return;
  }
  else {
    bprintf("You challenge %s to a duel!\n", pname(plr));
    setup_globals(plr);
    cur_player->challenger = attacker;
    sprintf(cur_player->cprompt, 
           "%s challenges you to a duel.  Do you accept? ", pname(attacker));
    bprintf(cur_player->cprompt);
    push_input_handler(confirm_challenge);
    confirm_challenge(NULL);
    setup_globals(attacker);
  }
}

void confirm_challenge(char *input) {
  if (input == NULL)
    return;
  else if (tolower(*input) == 'y') {
    sendf(mynum, "Engarde!\n");
    sendf(cur_player->challenger, "Your challenge is accepted!\n");
    hit_player(mynum, cur_player->challenger, pwpn(mynum));
    strcpy(cur_player->cprompt, build_prompt(mynum));
    pop_input_handler();
  }
  else if (tolower(*input) == 'n') {
    bprintf("Until a later date, then.\n");
    strcpy(cur_player->cprompt, build_prompt(mynum));
    pop_input_handler();
  }
  else {
    bprintf("I don't understand.\n");
    return;
  }
}

/* function to loot a corpse */
void lootcom(void) {
  int i, j, mob;

  if (EQ(item1, "corpse")) {
    for (i = 0, j = -1 ; i < lnumchars(ploc(mynum)) ; i++) {
       mob = lmob_nr(i, ploc(mynum));
       if (pstr(mob) < 0 && j == -1)
         j = mob;
       else if (pstr(mob) < 0) {
         bprintf("Well who do you mean? Use loot all to search everyone.\n");
         return;
       }
     }
     if (j == -1)
       bprintf("There is no corpse here.\n");
     else
       loot(j);
  }
  else if (EQ(item1, "all")) {
    for (i = 0 ; i < lnumchars(ploc(mynum)) ; i++) {
      mob = lmob_nr(i, ploc(mynum));
      if (pstr(mob) < 0)
        loot(mob);
    }
  }
  else if (seeplr("Loot who?", "You can't loot them; they aren't here!") == -1)
    return;
  else if (pstr(pl1) > -1)
    bprintf("They wouldn't like that.\n");
  else
    loot(pl1);
}

void loot(int plr) {
  if (pnumobs(plr) == 0)
    bprintf("You madly tear apart %s's corpse, but find nothing.\n",
      pname(plr));
  else {
    bprintf("You take the following items from %s:\n", pname(plr));
      
    dumpstuff(plr, ploc(mynum));
    gamecom("get all", True);
  }
}

/* function to kill/remove a player or mobile */

void player_died(int attacker, int victim, int hit_type) {
  int q, x;
  Bodypart *area = 0;

  pstr(victim) = -1;

  for (x = 0 ; x < numchars ; x++)
    if (pfighting(x) == victim)
      pfighting(x) = -1;

  pfighting(victim) = -1;

  if (hit_type >= 0 && hit_type != VERB_ZAP)
    switch(hit_type) {
    case VERB_FIREBALL:
      GMSG("%d get%t burned to a charred black pile of dust!");
      break;
    case VERB_FROST:
      GMSG("%d get%t frozen into a block of ice!");
      break;
    case VERB_MISSILE:
      GMSG("%a cast%s a missle directly through %p chest!");
      break;
    case VERB_SHOCK:
      GMSG("%d get%t electricuted and has seizures on the ground!");
      break;
    case VERB_ICESTORM:
      GMSG("%p body is buried in an avalanche!");
      break;
    }

  if (the_world->w_tournament)
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              "&+r[&+YTournament: &+W%s &*has died&+r]", pname (victim));

  if (victim >= max_players) {                     /* VICTIM MOBILE */
    if ((q = aliased_mob (victim)) >= 0) {
      sendf(victim, "*** You have died. ***\n");
      unalias(q);
    }
    if (attacker < max_players) {
      kill_bonus(attacker, victim);
      setpkilled(attacker, pkilled (attacker) + 1);
      if (randperc() > 66 || pmaxstrength(attacker) > pmaxstrength(victim)*3) {
	pcoins(attacker)++;
        sendf(attacker,  "A shiny &+Ygold&N coin falls from the sky.  "
	                 "The gods are pleased.\n"); 
      }
      if (pkilled(attacker) == 1 && plev(attacker) < LVL_WIZARD)
        sendf(attacker, "Congratulations.  You have killed your first "
          "mobile. Now woulld be a good\ntime to learn the LOOT command.  "
          "To take items from a corpse, type LOOT CORPSE\nTo search "
          "all corpses from a room, type LOOT ALL.\n");

      if (pnumobs(victim) > 0 && ploc(victim) == ploc(attacker))
        sendf(attacker, "&+C%s appears to have had something with %s "
          "when %s died.\n&N", pname(victim), psex(victim) ? "her" : "him",
            psex(victim) ? "she" : "he");

      time(&ublock[victim - max_players].death_time);

      if (((q = is_grouped(attacker)) >= 0) && (ploc(q) == ploc(attacker)))
        setpscore(victim, q);          /* leader of group gets kill */
      else
        setpscore(victim, attacker);

      if (ststflg (attacker, SFL_AUTOHEAL)) {
        sendf(attacker, "The very essense of &$ itself "
           "reaches up to heal your wounds.\n");
        setpstr(attacker, maxstrength (attacker));
        setpmagic(attacker, maxmagic (attacker));
      }
    }
    else                                   /* MOBILE VS MOBILE */
      setpscore (victim, -1);

  #include "mobdeath.h"
  }

  else {                                  /* VICTIM: MORTAL */
    if (attacker < max_players)
      kill_bonus(attacker, victim);

    setpstr(victim, 40);
    setpscore(victim, pscore(victim) / 2);
    set_vital(victim);

    if (!the_world->w_tournament)
      setpdied (victim, pdied(victim) + 1);

    if (attacker >= max_players)
      switch (rand() % 3) {
      case 0:
	send_msg(SLAIN, "&+r%s&N shouts, &+W\"&+RThe head of %s now hangs as "
	     "a trophy on my wall!&+W\"\n", pname (attacker), pname (victim));
	break;
      case 1:
        send_msg(SLAIN, "&+r%s&N shouts, &+W\"&+RBy my hand, %s has been "
                         "slain!&+W\"\n", pname (attacker), pname (victim));
	break;
      case 2:
	send_msg(SLAIN, "&+r%s&N shouts, &+W\"&+RBy the power given to me by "
       "the gods, I have slain %s!&+W\"\n", pname (attacker), pname (victim));
	break;
      }

#ifdef LOCMIN_PYRAMID
    if (ploc(victim) < LOCMIN_PYRAMID && ploc(victim) > LOCMAX_PYRAMID)
      broad("&+rThe soft sound of satisfied feminine laughter "
	    " floats in the air.\n");
#endif

    calib_player(victim);
    setup_globals(victim);
#ifdef BODYPART_COMBAT
    if (!attached(neck(victim))) {
      quit_msg("Oh dear... you seem to have lost your head!", "Decapitated");
      quit_player(False);
      deluaf(pname(victim));
    }
    else {
      quit_msg("Oh dear... you seem to be slightly dead", NULL);
      quit_player(False);
    }
#else
    quit_msg("Oh dear... you seem to be slightly dead", NULL);
    quit_player(False);
#endif
  }

  if (attacker < max_players) {
    calib_player(attacker);
    send_msg(WIZSLAIN,"&+r[&+W%s &*has been &+Rslain &*%sby &+W%s "
      "&N(&+GKill:&+Y%d&N)&+r]\n", pname (victim), (hit_type == -1 ? "" :
         "magically "), pname (attacker), pkilled(attacker) + 1);
  }
  if (victim < max_players) {
    mudlog ("SYSTEM: %s has been slain %sby %s", pname (victim),
            (hit_type == -1 ? "" : "magically "), pname(attacker));
    send_msg(WIZSLAIN, "&+r[&+W%s &Nhas &+Rbrutally slain &+W%s "
             "&N(&+YDeath:&+Y%d&N)&+r]\n", pname(attacker), pname(victim), 
               pdied(victim) + 1);
  }
}

char *__add_codes(int attacker, int victim, Bodypart *area, int type, char *msg) {
  Text text = {0};
  char *ptr, *result;
  for (ptr = msg; *ptr; ptr++) {
    if (*ptr != '%') text_char(&text, *ptr);
    else {
      if (!ptr[1]) break;
       switch (*(ptr + 1)) {
       case 'b':
         text_append(&text, area->name);
         break;
       case 'w':
         text_append(&text, oname(pwpn(attacker)));
         break;
       case 'a':
         if (type == ATTACKER)
           text_append(&text, "you");
         else
           { char *part = text_format("\001p%s\003", pname(attacker)); text_append(&text, part); free(part); }
         break;
       case 'g':
         if (type == ATTACKER)
           text_append(&text, "your");
         else if (ststflg(attacker, SFL_FEMALE))
           text_append(&text, "her");
         else
           text_append(&text, "his");
         break;
       case 'd':
         if (type == VICTIM)
           text_append(&text, "you");
         else
           { char *part = text_format("\001p%s\003", pname(victim)); text_append(&text, part); free(part); }
         break;
       case 'p':
         if (type == VICTIM)
           text_append(&text, "your");
         else
           { char *part = text_format("\001p%s\003's", pname(victim)); text_append(&text, part); free(part); }
         break;
       case 'e':
         if (type == ATTACKER)
           text_append(&text, "your");
         else
           { char *part = text_format("\001p%s\003's", pname(attacker)); text_append(&text, part); free(part); }
         break;
       case 'r':
         if (hasarmr(area)) {
           if (type == VICTIM)
             { char *part = text_format("your %s", oname(area->armor)); text_append(&text, part); free(part); }
           else
             { char *part = text_format("%s %s", psex(victim) ? "her":"his",oname(area->armor)); text_append(&text, part); free(part); }
         }
         else {
           if (type == VICTIM)
             { char *part = text_format("your %s", bodyname(*area)); text_append(&text, part); free(part); }
           else
             { char *part = text_format("%s %s", psex(victim) ? "her":"his", bodyname(*area)); text_append(&text, part); free(part); }
         }
         break;
       case 's':
         if (type == ATTACKER)
           text_append(&text, "");
         else
           text_append(&text, "s");
         break;
       case 't':
         if (type == ATTACKER || type == OTHER)
           text_append(&text, "s");
         else
           text_append(&text, "");
         break;
       case 'S':
         if (type == ATTACKER)
           text_append(&text, "");
         else
           text_append(&text, "S");
         break;
       case 'T':
         if (type == ATTACKER || type == OTHER)
           text_append(&text, "S");
         else
           text_append(&text, "");
         break;
       case 'q':
         if (type == VICTIM)
           text_append(&text, "you");
         else {
           if (ststflg(victim, SFL_FEMALE))
             text_append(&text, "she");
           else
             text_append(&text, "he");
         }
         break;
       }
      ptr++;
    }
  }
  text_char(&text, '\n');
  result = text_take(&text);
  *result = toupper((unsigned char)*result);
  return result;
}
/* bodypart primitives */

Boolean has_foot (int plr) {
  if (!attached(r_foot(plr)) && !attached(l_foot(plr)))
     return(False);
  else
     return(True);
}

Boolean has_hand (int plr) {
  if (!attached(r_hand(plr)) && !attached(l_hand(plr)))
    return(False);
  else
    return(True);
}

Boolean has_arm (int plr) {
  if (!attached(r_arm(plr)) && !attached(l_arm(plr)))
    return(False);
  else
    return(True);
}

void iofunc
(PERSONA *p, void func(Bodypart *, FILE *, char *), FILE *fp, char *txt) 
{
  func(&(p->ublock.body.head), fp, txt);
  func(&(p->ublock.body.r_arm), fp, txt);
  func(&(p->ublock.body.l_arm), fp, txt);
  func(&(p->ublock.body.r_leg), fp, txt);
  func(&(p->ublock.body.l_leg), fp, txt);
  func(&(p->ublock.body.r_foot), fp, txt);
  func(&(p->ublock.body.l_foot), fp, txt);
  func(&(p->ublock.body.r_hand), fp, txt);
  func(&(p->ublock.body.l_hand), fp, txt);
  func(&(p->ublock.body.chest), fp, txt);
  func(&(p->ublock.body.back), fp, txt);
  func(&(p->ublock.body.face), fp, txt);
  func(&(p->ublock.body.neck), fp, txt);
}

void store_parts_helper(Bodypart *part, FILE *outfile, char *foo) {
  fprintf(outfile, "%d %d %d ", (int) vital(*part),
    loc(*part), (int) attached(*part));
  return;
}

void store_parts(FILE *outfp, PERSONA *p) {
  fprintf(outfp, "Bodyparts  ");
  iofunc(p, store_parts_helper, outfp, NULL);
  fprintf(outfp, "\n");
}

void load_parts_helper(Bodypart *part, FILE *foo, char *parts) {
  static char *curpart = NULL;
  static char buff[256];

  if (part == NULL) {
    strcpy(buff, parts);
    curpart = buff;
    return;
  }
  part->vital = atoi(curpart);
  curpart = strchr(curpart, ' ') + 1;
  part->loc = atoi(curpart);
  curpart = strchr(curpart, ' ') + 1;
  part->attach = (Boolean) atoi(curpart);
  curpart = strchr(curpart, ' ') + 1;
  return;
}

void load_parts(char *parts, PERSONA *p) {
  load_parts_helper(NULL, NULL, parts);
  iofunc(p, load_parts_helper, NULL, parts);
}

int applyfunc(int plr, int func(Bodypart *, int, FILE *), 
              int passedval, FILE *fp) 
{
  return(func(&head(plr), passedval, fp) +
         func(&r_arm(plr), passedval, fp) +
         func(&l_arm(plr), passedval, fp) +
         func(&r_leg(plr), passedval, fp) +
         func(&l_leg(plr), passedval, fp) +
         func(&r_foot(plr), passedval, fp) +
         func(&l_foot(plr), passedval, fp) +
         func(&r_hand(plr), passedval, fp) +
         func(&l_hand(plr), passedval, fp) +
         func(&chest(plr), passedval, fp) +
         func(&back(plr), passedval, fp) +
         func(&face(plr), passedval, fp) +
         func(&neck(plr), passedval, fp));
}

/* checks to see if the armor applies to more than one body part */

Boolean multi_covered(int item, char *area) {
   int ct = 0;

   if (!item)
     return(False);
   
   if (armor(r_foot(mynum)) == item &&
       armor(l_foot(mynum)) == item)    ct++;
   if (armor(r_hand(mynum)) == item &&
       armor(l_hand(mynum)) == item)    ct++;
   if (armor(r_arm(mynum)) == item &&
       armor(l_arm(mynum)) == item)     ct++;
   if (armor(r_leg(mynum)) == item && 
       armor(l_leg(mynum)) == item)     ct++;
   if (armor(head(mynum)) == item)      ct++;
   if (armor(chest(mynum)) == item)     ct++;
   if (armor(back(mynum)) == item)      ct++;
   if (armor(face(mynum)) == item)      ct++;
   if (armor(neck(mynum)) == item)      ct++;

   if (ct > 1) {
     bprintf("The %s already covers your %s and some other body areas.\n"
	     "Please remove it first.\n", oname(item), area);
     return(True);
   }
   return(False);
}

int list_parts_helper(Bodypart *area, int plr, FILE *notused) {
  if (!attached(*area) && loc(*area) == ploc(plr)) {
    if (plr == mynum)
      bprintf("Your %s is resting on the ground here.\n", bodyname(*area));
    else
      bprintf("%s's %s is here.\n", pname(plr), bodyname(*area));
  }
  return(0);
}

/* list body parts */
void list_parts(int plr) {
  if (is_in_game(plr))
    applyfunc(plr, list_parts_helper, plr, NULL);
}

int svital(Bodypart *area, int plr, FILE *notused) {
  static int shown;

  if (area == NULL)
    shown = 0;
  else {
    if (!(shown % 3))
      bprintf("&+b| ");

    if ((shown % 3) != 2)
      bprintf("%-15s&N: &+c%3d&+W/&+c%-10d", bodyname(*area), 
        vital(*area), plr < max_players ? MAX_PLAYER_VITAL : 
          pstr_reset(plr) / MOB_VITAL_DIVISOR);
    else
      bprintf("%-15s&N: &+c%3d&+W/&+c%-8d&+b|\n", bodyname(*area), 
        vital(*area), plr < max_players ? MAX_PLAYER_VITAL :
          pstr_reset(plr) / MOB_VITAL_DIVISOR);
    shown++;
    return(vital(*area));
  }
  return(-1);
}

void show_vital (int plx) {
  int tot;

  svital(NULL, 0, NULL);
  tot = applyfunc(plx, svital, plx, NULL);
  bprintf("&+Yavg&N       : &+c%3d&+W/&+c%-3d       &+Ytot&N       :  "
   "&+c%-4d       &+b|\n", tot/NUM_PARTS, 
    plx >= max_players ? (tot / MOB_VITAL_DIVISOR) : 100, tot);
}

int sarmor(Bodypart *area, int plr, FILE *notused) {
  char locnam[40];

  strcpy(locnam, area->name);
  *(locnam + 3) = toupper(*(locnam + 3));

  if (!attached(*area)) {
    if (!area->loc)
      bprintf("%-20s: &+Win inventory\n", locnam);
    else if (area->loc == ploc(mynum))
      bprintf("%-20s: &+Wsevered on the ground here\n", locnam);
    else
      bprintf("%-20s: &+Win %s\n", locnam, sdesc(area->loc));
  }
  else if (hasarmr(area)) 
    bprintf("%-20s: &+W%s [%d%%]\n", locnam, oname(area->armor), 
     oarmor(area->armor));
  else
    return(0);
  return(1);
}

void armorcom (void) {
  bprintf("This command has been changed to EQUIPMENT (EQU)\n");
}

void equcom (void) {
  Boolean armor = False;
  char buff[80];
  int shield, i;

  if (brkword() == -1)
    pl1 = mynum;
  else if (pl1 == -1) {
    bprintf("Who's that?\n");
    return;
  }
  else if (plev(mynum) < LVL_WIZARD && ploc(mynum) != ploc(pl1)) {
    bprintf("They aren't here.\n");
    return;
  }

  sprintf(buff, "%s%s Equipment\n", pl1 == mynum ? "Your" : pname(pl1),
                                    pl1 == mynum ? "" : "'s");

  for (i = strlen(buff) -1 ; i > 0 ; i--)
    strcat(buff, "-");
  strcat(buff, "\n");

  bprintf(buff);

  if (pwpn(pl1) == -1 || oloc(pwpn(pl1)) != pl1)
    bprintf("&+YNo weapon.\n");
  else
    bprintf("&+CPrimary weapon : &+W%s [dam: %d]\n", oname(pwpn(pl1)),
      odamage(pwpn(pl1)));

  armor = applyfunc(pl1, sarmor, pl1, NULL);
  if ((shield = wears_shield(pl1)) != -1) 
    bprintf("%-15s: %s [%d%%]\n", "Around arm", oname(shield),
      oarmor(shield));
  else if (!armor)
    bprintf("&+CNo armor worn.&N\n");
}

void removecom(void) {
  if (EMPTY(item1))
    bprintf("Remove what?\n");
  else if (ob1 == -1 || !iswornby(ob1, mynum))
    bprintf("You're not wearing it.\n");
  else
    removeobj(True, ob1, mynum);
}

int removeobj_helper(Bodypart *bodyloc, int object, FILE *notused) {
  if (bodyloc->armor == object) {
    bodyloc->armor = -1;
    return(1);
  }
  else
    return(0);
}

void removeobj(Boolean echo, int ob, int plx) {
  if (ob == -1)
    return;

  applyfunc(plx, removeobj_helper, ob, NULL);
  setcarrf(ob, CARRIED_BY);

  if (echo) {
    sendf(plx, "You remove the %s.\n", oname(ob)); 
    send_msg (sendloc(plx), 0, pvis (plx), LVL_MAX, plx, NOBODY,
            "%s removes the %s.\n", pname(plx), oname(ob1));
  }

  calc_ac(mynum);    /* recalculate AC */
}

void wearcom(int plr, int item) {
  int a;

  if (item != -1)    a = item;
  else               a = ob1;

  if (cant_wear(plr, a))
    return;

  #include "wearcom.h"

  if (otstbit(a, OFL_SHIELD)) {
    bprintf("You wear the shield over your right arm.\n");
    wear_item(a, plr);
  }
  else {
    if (test_wear(plr, a)) {
      wear_item(a, plr);
      send_msg(sendloc(plr), 0, pvis(plr), LVL_MAX, plr, NOBODY,
        "\001p%s\003 wears the %s.\n", pname(plr), oname(a));
    }
  }
}

int set_armor_helper(Bodypart *part, int newval, FILE *notused) {
  part->armor = -1;
  return(0);
}

/* init armor, set worn armor if update or mobile */

void reset_armor(Boolean update, int plr) {
  int obj, i;

  applyfunc(plr, set_armor_helper, -1, NULL);
  for (i = 0; i < pnumobs(plr); i++) {
    obj = pobj_nr(i, plr);
    if (obj > numobs || obj < 0)     /* kludge */
      break;
    if (iswornby(obj, plr) && ozone(obj) < num_const_zon) {
      if (plr < max_players && !update)
        removeobj(False, obj, plr);
      else
        wear_item(obj, plr);
    }
  }
}

Boolean test_wear(int plr, int a) {
  char bodyparts[200];
  int removed[NUM_PARTS];
  int i, j;

  i = *bodyparts = 0;

  /* basically this part tests conditions for failure and generates *
   * two strings which are a list of armors removed and a list of   *
   * body parts covered; actual wearing done in wear_item()         */

  if (is_head(a)) {
    strcat(bodyparts, "your head, ");
    if (wears_head(plr)) {
      if (multi_covered(armor(head(plr)), "head"))
	return(False);
      removed[i++] = armor(head(plr));
    }
  }

  if (is_foot(a)) {
    strcat(bodyparts, "a foot, ");
    if (wears_feet(plr)) {
      bprintf("Both feet currently wear armor; remove it first.\n");
      return(False);
    }
  }

  if (is_feet(a)) {
    strcat(bodyparts, "your feet, ");
    if (wears_rfoot(plr)) {
      if (multi_covered(armor(r_foot(plr)), "right foot"))
	return(False);
      removed[i++] = armor(r_foot(plr));
    }
    if (wears_lfoot(plr)) {
      if (multi_covered(armor(l_foot(plr)), "left foot"))
        return(False);
      removed[i++] = armor(l_foot(plr));
    }
  }

  if (is_hand(a)) {
    strcat(bodyparts, "a hand, ");
    if (wears_hands(plr)) {
      bprintf("Both hands currently wear armor.\n");
      return(False);
    }
  }

  if (is_hands(a)) {
    strcat(bodyparts, "both hands, ");
    if (wears_rhand(plr)) {
      if (multi_covered(armor(r_hand(plr)), "right hand"))
	return(False);
      removed[i++] = armor(r_hand(plr));
    }
    if (wears_lhand(plr)) {
      if (multi_covered(armor(l_hand(plr)), "left hand"))
        return(False);
      removed[i++] = armor(l_hand(plr));
    }
  }

  if (is_face(a)) {
    strcat(bodyparts, "your face, ");
    if (wears_face(plr)) {
      if (multi_covered(armor(face(plr)), "face"))
	return(False);
      removed[i++] = armor(face(plr));
    }
  }

  if (is_back(a)) {
    strcat(bodyparts, "your back, ");
    if (wears_back(plr)) {
      if (multi_covered(armor(back(plr)), "back"))
	return(False);
      removed[i++] = armor(back(plr));
    }
  }

  if (is_arm(a)) {
    strcat(bodyparts, "an arm, ");
    if (wears_arms(plr)) {
      bprintf("Both arms are occupied.  Remove armor from them first.\n");
      return(False);
    }
  }

  if (is_arms(a)) {
    strcat(bodyparts, "both arms, ");
    if (wears_rarm(plr)) {
      if (multi_covered(armor(r_arm(plr)), "right arm"))
        return(False);
      removed[i++] = armor(r_arm(plr));
    }
    if (wears_larm(plr)) {
      if (multi_covered(armor(l_arm(plr)), "left arm"))
        return(False);
      removed[i++] = armor(l_arm(plr));
    }
  }

  if (is_leg(a)) {
    strcat(bodyparts, "a leg, ");
    if (wears_legs(plr)) {
      bprintf("Both legs are wearing something.  Remove something from one.\n");
      return(False);
    }
  }

  if (is_legs(a)) {
    strcat(bodyparts, "your legs, ");
    if (wears_rleg(plr)) {
      if (multi_covered(armor(r_leg(plr)), "your right leg"))
        return(False);
      removed[i++] = armor(r_leg(plr));
    }
    if (wears_lleg(plr)) {
      if (multi_covered(armor(l_leg(plr)), "your left leg"))
        return(False);
      removed[i++] = armor(l_leg(plr));
    }
  }

  if (is_chest(a)) {
    strcat(bodyparts, "your chest, ");
    if (wears_chest(plr)) {
      if (multi_covered(armor(chest(plr)), "chest"))
        return(False);
      removed[i++] = armor(chest(plr));
    }
  }
  
  if (is_neck(a)) {
    strcat(bodyparts, "your neck, ");
    if (wears_neck(plr)) {
      if (multi_covered(armor(neck(plr)), "neck"))
        return(False);
      removed[i++] = armor(neck(plr));
    }
  }

  if (!*bodyparts) {
    bprintf("This armor has no bodyparts defined.\n"
            "An error message has been added to the system log\n");
    mudlog("ERROR: no bodypart defined for %s", oname(a));
    return(False);
  }

  *(bodyparts + strlen(bodyparts) - 2) = 0;

  if (strlen(bodyparts) > 45)
    bprintf("You wear the %s around:\n  %s\n", oname(a), bodyparts);
  else
    bprintf("You wear the %s on %s.\n", oname(a), bodyparts); 

  if (i > 0) {
    bprintf("This forces you to remove: ");
    for (j = 0 ; j < i ; j++) {
      if (j == i - 1 && i != 1)
        bprintf("and the %s", oname(removed[j]));
      else
        bprintf("the %s%s", oname(removed[j]), (i == 1) ? "" : ", ");
      removeobj(False, removed[j], plr);
    }
    bprintf(".\n");
  }
  return(True); /* success */
}

Boolean cant_wear(int plr, int a) {
  if (a == -1 && EMPTY(item1)) {
    bprintf("Wear what exactly?\n");
    return(True);
  }
  else if (a == -1 || !iscarrby(a, plr)) {
    bprintf("You don't have it.\n");
    return(True);
  }
  else if (iswornby(a, plr)) {
    bprintf("You're already wearing it.\n");
    return(True);
  }
  else if (!otstbit(a, OFL_WEARABLE)) {
    bprintf("Is this a new fashion?\n");
    return(True);
  }
  else if (otstbit(a, OFL_SHIELD)) {
    if (wears_shield(plr) != -1) {
      bprintf("You're already wearing a shield.\n");
      return(True);
    }
    else
      return(False);
  }
  else if (!has_hand(plr) || !has_arm(plr)) {
    bprintf("You don't have any hands. Must suck to be you.\n");
    return(True);
  }
  else
    return(False);
}

void hit_player(int attacker, int victim, int weapon) {
  int damage, mob, hitpct;
  Bodypart *area;
  Boolean missed, tick;

  if (pfighting(victim) == -1 || pfighting(attacker) == -1) {
    ftimer(attacker) = 0;
    pfighting(attacker) = victim;
    ftimer(victim) = 0;
    pfighting(victim) = attacker;
  }

  tick = ftimer(attacker);
  ftimer(attacker) = !tick;

  if (tick) 
    return;

  if (testpeace(attacker) || ploc(attacker) != ploc(victim) || 
      !is_in_game(attacker) || !is_in_game(victim)) {
        pfighting(attacker) = -1;
        pfighting(victim) =  -1;
        return;
    }

  mob = victim - max_players;

  if (weapon < 0)                             /* set weapon */
    weapon = pwpn (attacker);
  else if (!iscarrby (weapon, attacker)) {
    sendf(attacker, "You belatedly realize you don't have the %s,\nand "
         "are forced to use your hands instead.\n", oname(weapon));
    weapon = -1;
  }
  set_weapon (attacker, weapon);

  if (plev(attacker) < LVL_WIZARD && victim >= max_players &&
      (attacker < max_players || ststflg(attacker, SFL_OCCUPIED))) 
    {
      #include "killmob.h"
    }

    /* attacker is a human */

  if (attacker < max_players || ststflg (attacker, SFL_OCCUPIED)) {
#if defined(LOCMIN_CASTLE) && defined(LOCMIN_TOWER)
    if (weapon >= 0 && weapon == OBJ_CASTLE_RUNESWORD &&
        carries_obj_type (victim, OBJ_TOWER_STAFF) > -1) {
      sendf (attacker,
           "The Runesword flashes back from its target, growling in anger!\n");
      return;
    }
#endif
    if (weapon >= 0 && dambyitem (attacker, weapon) < 0) {
      sendf (attacker, "You can't attack %s with a %s!\n",
             pname (victim), oname (weapon));
      set_weapon (attacker, -1);
      return;
    }
    if (psitting (attacker))
      standcom (attacker);
  }
  else if (!wears_antimagic(victim))
    mob_cast_spell(attacker, victim);
  
  area = calc_hit(attacker, victim, &missed, &damage, &hitpct);

  if (!area)
    return;

  if (missed) {
    if (weapon == -1)
      FMSG(handmiss, arraysize(handmiss));
    else
      FMSG(miss, arraysize(miss));
    return;
  }

  if (weapon == -1) {
    if (!damage) {
      FMSG(handhit0, arraysize(handhit0));
      return;
    }
    else if (hitpct < 33)
      FMSG(handhit1, arraysize(handhit1));
    else if (hitpct < 66)
      FMSG(handhit2, arraysize(handhit2));
    else
      FMSG(handhit3, arraysize(handhit3));
  }
  else {
    if (!damage) {
      FMSG(hit0, arraysize(hit0));
      return;
    }
    else if (hitpct < 33)
      FMSG(hit1, arraysize(hit1));
    else if (hitpct < 66)
      FMSG(hit2, arraysize(hit2));
    else
      FMSG(hit3, arraysize(hit3));
  }

  setpstr(victim, pstr(victim) - damage);

  if (victim < max_players && (vital(*area) - damage <= 0) &&
     (pclass(victim) == PRIEST || plev(victim) > LVL_WIZARD)) {
       GMSG("By divine will, god heals %p %b!");
       vital(*area) = 100;
  }

#ifdef BODYPART_COMBAT
  if (vital(*area) - damage < 0) {
    vital(*area) = -1;
    check_parts(victim, attacker, area, weapon != -1 ? True : False);
    vital(*area) = 0;
  }
  else
    vital(*area) -= damage;
#endif

  if (victim < max_players) {
    if (players[victim].prompt && strstr(players[victim].prompt, "%h")) {
      strcpy (players[victim].cprompt, build_prompt (victim));
      if (ststflg (victim, SFL_NEWSTYLE))
        sendf (victim, "\r%s", players[victim].cprompt);
    }
    else
      sendf(victim, "&+W[&*Your strength is now &+C%d&N/&+C%d&+W]\n", 
            pstr(victim), pmaxstrength(plev(victim)));
  }

  if (victim < max_players && plev(victim) > LVL_WIZARD && pstr(victim) < 0) {
    GMSG("God heals all of %p wounds");
    setpstr(victim, pmaxstrength(victim));
  }

  if (pstr(victim) < 0 || !vital(neck(victim))) {
    if (!vital(neck(victim)));
    else if (pwpn(attacker) != -1 && damage > MIN_FATAL_DAMAGE)
      FMSG(death, arraysize(death));
    else
      sendl(ploc(victim), "%s has died.\n", pname(victim));
    player_died(attacker, victim, -1);
  }
}

/* sends message and does action if any part was severed */

void check_parts(int victim, int attacker, Bodypart *area, Boolean sever)
{
  int s, removed;

  removed = -1;
  setup_globals(victim);

  if (hasarmr(area) && sever) {
    GMSG("%r is smashed to bits.\n");
    removeobj(False, area->armor, victim);
    setcarrf(area->armor, IN_ROOM);
    destroy(area->armor);
  }

  if (!has_arm(victim) && ((s = wears_shield(victim)) != -1)) {
    GMSG("Your shield falls to the ground.\n");
    removeobj(False, s, victim);
    setcarrf(s, IN_ROOM);
  }

  /* special effects for some non-severable body parts           */
  /* check to see if vital == -1 (which means it was just severed) */

  if (vital(head(victim)) == -1) {
    if (!ststflg(victim, SFL_BLIND)) {
      sendf(victim, "Blood drips in your eyes, blinding you...\n");
      sendf(attacker, "Your last hit blinded %s!\n", pname(victim));
      ssetflg (victim, SFL_BLIND);
    }
  }
  else if (vital(chest(victim)) == -1) {
    if (!ststflg(victim, SFL_CRIPPLED)) {
      sendf(victim, "The wounds on your chest have crippled you.\n");
      sendf(attacker, "You have crippled %s!\n", pname(victim));
      ssetflg (victim, SFL_CRIPPLED);
    }
  }
  else if (vital(back(victim)) == -1) {
    if (!ststflg(victim, SFL_CRIPPLED)) {   
      sendf(victim, "The festering wounds on your back have crippled you.\n");
      sendf(attacker, "You have crippled %s!\n", pname(victim));
      ssetflg (victim, SFL_CRIPPLED);
    }
  }
  else if (vital(neck(victim)) == -1) {
    sendf(attacker, "You have decapitated %s!\n", pname(victim));
    send_msg(sendloc(attacker), 0, LVL_MIN, LVL_MAX, attacker, victim,
      "\001p%s\003 has decapitated \001p%s\003!!\n", 
           pname(attacker), pname(victim));
    sendf(victim,"Your head is severed off and your see your body "
      "crumple to the ground.\n");
  }
  else if (vital(face(victim)) == -1) {
    if (!ststflg(victim, SFL_DUMB)) {
      sendf(victim, "Your face is so scarred you can't talk anymore.\n");
      sendf(attacker, "You have marred %s's face so badly, "
                      "they lose the gift of speech!\n", pname(victim));
      ssetflg (victim, SFL_DUMB);
    }
  }
  else if (sever) {
    attached(*area) = False;
    GMSG("%p severed %b falls to the ground");
    area->loc = ploc(victim);
  }

  /* if arm gets cut off, drop hand too & remove armor */

  if (!attached(r_arm(victim))) {
    attached(r_hand(victim)) = False;
    loc(r_hand(victim)) = ploc(victim);
    removed = armor(r_hand(victim));
  }
  else if (!attached(l_arm(victim))) {
    attached(l_hand(victim)) = False;
    loc(l_hand(victim)) = ploc(victim);
    removed = armor(l_hand(victim));
  }
  else if (!attached(r_leg(victim))) {
    attached(r_foot(victim)) = False;
    loc(r_foot(victim)) = ploc(victim);
    removed = armor(r_foot(victim));
  }
  else if (!attached(l_leg(victim))) {
    attached(l_foot(victim)) = False;
    loc(l_foot(victim)) = ploc(victim);
    removed = armor(l_foot(victim));
  }
  if (removed != -1) {
    removeobj(False, removed, victim);
    setcarrf(removed, IN_ROOM);
  }

  if ((!has_hand(victim) || !has_arm(victim)) && victim < max_players) {
    bprintf("Noticing that you don't have any more available body parts "
	  "to fight with,\nyou make the wise decision to run away.\n");
    flee(victim, False);
  }
}

void flee (int plr, Boolean check_dir) {
  int newloc, me, attacker;

  me = mynum;
  setup_globals(plr);
  attacker = pfighting(mynum);  

  if (check_dir && brkword() != -1)
    newloc = chklist (wordbuf, exits, exitnum) - 1;
  else
    newloc = get_rand_exit_dir (ploc (mynum));

  if (attacker == -1)
    bprintf("Flee from what?\n");
#ifdef LOCMIN_CASTLE
  else if (carries_obj_type (plr, OBJ_CASTLE_RUNESWORD) > -1)
    bprintf("The sword won't let you!\n");
#endif
  else if (newloc < 0 || newloc >= NEXITS)
    bprintf("You can't flee that direction.\n");
  else {
    setpfighting (mynum, -1);
    drop_some_objects (mynum);
 
    send_msg (sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
              "%s drops things as %s make a frantic attempt to escape.\n",
              pname (mynum), psex (mynum) ? "she" : "he");

    send_msg(DEST_ALL, MODE_NSFLAG | MS(SFL_QUIET), LVL_WIZARD, LVL_MAX,
             mynum, attacker,
	     "&+r[&+W%s &N(&+WStr: &+Y%d&N) has &+Yfled "
	     "&*from &+W%s &N(&+WStr: &+Y%d&N)&+r]\n", pname (mynum), 
	     pstr(mynum), pname(attacker), pstr(attacker));

    if (dodirn(False, newloc + 2) == -1) {
      setpfighting (mynum, attacker);
      return;
    }
    else if (pfighting(attacker) == mynum)
      setpfighting(attacker, -1);

    if (pclass(mynum) != THIEF)
      setpscore (mynum, pscore (mynum) - pscore (mynum) / 33);
    calibme ();

    if (mynum > max_players && pfollow(mynum) >= 0) {
      bprintf ("You stopped following %s.\n", pname (pfollow(mynum)));
      pfollow(mynum) = -1;
    }

    if (pangry(attacker) == mynum)
      sendf(ploc(mynum), "%s yells out, &+W\"&+YNext time you won't be so "
        "lucky, bub!&+W\"\n", pname(attacker));
  }
}

int calc_ac_helper(Bodypart *part, int foo, FILE *notused) {
  if (hasarmr(part))
    return(oarmor(part->armor));
  else
    return(0);
}

int wears_shield(int plr) {
  int i, a;

  for (i = 0; i < pnumobs (plr); i++) {
    a = pobj_nr (i, plr);
    if (iswornby (a, plr) && otstbit(a, OFL_SHIELD))
      return(a);
  }
  return(-1);
}

void calc_ac(int plr) {
  int shield = wears_shield(plr);

  pac(plr) = parmor(plr) + applyfunc(plr, calc_ac_helper, 0, NULL);
  if (shield != -1)
    pac(plr) += oarmor(shield);

  pac(plr) /= NUM_PARTS;
  if (pac(plr) > MAXARMOR) {
    sendf(plr, "You have reached the maximum armor class of %d.\n", MAXARMOR);
    pac(plr) = MAXARMOR;
  }
}

/* unconditionally wear an item over multiple body parts & set AC */

void wear_item(int item, int plr) {
  if (otstbit(item, OFL_SHIELD)) {
    oclrbit(item, OFL_SHIELD);
    setoloc(item, plr, WORN_BY);
    osetbit(item, OFL_SHIELD);
    calc_ac(plr);
    return;
  }
  setcarrf(item, WORN_BY);

  if (is_head(item))
    armor(head(plr)) = item;
  if (is_feet(item))
    armor(r_foot(plr)) = armor(l_foot(plr)) = item;
  else if (is_foot(plr)) {
    if (armor(r_foot(plr)))
      armor(l_foot(plr)) = item;
    else
      armor(r_foot(plr)) = item;
  }
  if (is_hands(item))
    armor(r_hand(plr)) = armor(l_hand(plr)) = item;
  else if (is_hand(item)) {
    if (armor(r_hand(plr)))
      armor(l_hand(plr)) = item;
    else
      armor(r_hand(plr)) = item;
  }
  if (is_face(item))
    armor(face(plr)) = item;
  if (is_back(item))
    armor(back(plr)) = item;
  if (is_arms(item))
    armor(r_arm(plr)) = armor(l_arm(plr)) = item;
  else if (is_arm(item)) {
    if (armor(r_arm(plr)))
      armor(l_arm(plr)) = item;
    else
      armor(r_arm(plr)) = item;
  }
  if (is_legs(item))
    armor(r_leg(plr)) = armor(l_leg(plr)) = item;
  else if (is_leg(item)) {
    if (armor(r_leg(plr)))
      armor(l_leg(plr)) = item;
    else
      armor(r_leg(plr)) = item;
  }
  if (is_chest(item))
    armor(chest(plr)) = item;
  if (is_neck(item))
    armor(neck(plr)) = item;
  calc_ac(plr);
}

void killcom (void) {
  int l, i, ct;

  if (pl1 == -1 && ob1 == -1)
    bprintf ("Kill who?\n");
   
  else if (!has_hand(mynum) || !has_arm(mynum)) {
    bprintf("Go, mcfly... (nice trick without a hand to fight with)\n");
    return;
  } 
 
  else if (pl1 != -1 && seeplr("Kill who?", "They aren't here.") != -1) {
    if (pfighting (mynum) != -1)
      bprintf ("You're already fighting!\n");

    else if (pl1 == mynum)
      bprintf ("Come on, it'll look better tomorrow...\n");

    else if (ploc (pl1) != ploc (mynum))
      bprintf ("They aren't here.\n");

    else if (testpeace (mynum))
      bprintf ("Sorry, it's too peaceful for violence.\n");

    else if (ptstflg (pl1, PFL_NOHASSLE))
      bprintf ("Something interferes with your attack.\n");

    else if (pl1 < max_players && !the_world->w_tournament &&
             !ltstflg(ploc(mynum), LFL_PKZONE)) {
      bprintf("You cannot attack another player here!\n");
      send_msg(DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
                "&#&+R[&+Y%s &+Rhas tried to kill &+Y%s&+R]\n",
 	       pname(mynum), pname(pl1));
      sendf(pl1, "%s tried to attack you!\n", pname(mynum));
    }
    else if (pstr(pl1) < 0) {
     l = ploc(mynum);

     for (ct = 0 ; ct < lnumchars(l) ; ct++) {
       i = lmob_nr(ct, l);
       if (i != mynum && pstr(i) > 0 && EQ(xname(pname(i)), xname(pname(pl1)))){
         bprintf("You will die, like your puny kin...\n"); 
         killplr(mynum, i);
         return;
       }
     }
      bprintf("How can you kill a corpse?  It's already dead!\n");
    }
    else
      killplr(mynum, pl1);
  }

  else if (ob1 != -1 && cantake(ob1, "What's that?", "It isn't here.") != -1)
    breakitem(ob1);

  else if (EQ (item1, "door")) {
    if (!ptstflg (mynum, PFL_WRECK)) 
      bprintf ("You wish you could kill a door.\n");
    else
      wreckdoor ();
  }
}

void killplr(int attacker, int victim) {
  int mob;
  mob = victim - max_players; 
  #include "killplr.h"

  sendf(victim, "%s charges into battle with you.\n", pname(attacker));
  sendf(attacker, "You charge into battle with %s.\n", pname(victim));

  hit_player (attacker, victim, (ob2 == -1) ? pwpn (attacker) : ob2);

  if (victim < max_players && plev(attacker) < LVL_WIZARD && 
      plev(victim) < LVL_WIZARD && the_world->w_tournament) 
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
              "&+r[&+YTournament: &+W%s &*attacked &+W%s]\n", 
              pname (attacker), pname (victim));
}

int check_vital_helper(Bodypart *part, int notused, FILE *foo) {
  return(MAX_PLAYER_VITAL - part->vital);
}

int check_severed_helper(Bodypart *part, int notused, FILE *foo) {
  if (!loc(*part))
    return(0);
  else
    return(1);
}

void hospital(void) {
  int hurt, severed, cost;

  if (mynum >= max_players || plev(mynum) > LVL_WIZARD)
    return;
  
  hurt = applyfunc(mynum, check_vital_helper, 0, NULL);
  severed = applyfunc(mynum, check_severed_helper, 0, NULL);  

  if (hurt < 50) {
    bprintf("A monk says, \"You call those wounds? Get out of here...\"\n");
    return;
  }
  else if (hurt < 100)
    bprintf("A monk says, \"We'd better take a look at those wounds...\"\n"
	    "You are led south\n");
  else if (hurt < 500) 
    bprintf("A monk says, \"Those look like some pretty bad wounds...\"\n"
	    "You are carried south.\n");
  else
    bprintf("After entering the hospital, you faint from the loss of blood."
	    "\nTwo monks rush you through a door to the south.\n");

  if (plev(mynum) < MAX_FREE_LEVEL)
    bprintf("Since you're new here, this healing is free!\n\n");
  else if (pcoins(mynum) >= (cost = severed * COINS_PER_SEVERED_LIMB)) {
    pcoins(mynum) -= cost;
    bprintf("You are charged %d coins for your missing limbs.\n\n", cost); 
  }
  else {
    setpscore(mynum, pscore(mynum) - severed * SCORE_PER_SEVERED_LIMB);
    bprintf("You can't afford to pay for a good room, so you lose score.\n\n");
  }
  cur_player->work = hurt / 20;
  setpvis(mynum, 13);
#ifdef LOCMIN_VILLAGE
  trapch(LOC_VILLAGE_EMERGENCY);
#endif
}

void exit_hospital(void) {
  bprintf("You are well rested and restored!\n");
  set_vital(mynum);
  setpvis(mynum, 0);
#ifdef LOCMIN_VILLAGE
  trapch(LOC_VILLAGE_HOSPITAL);
#endif
}

int take_bodypart_helper(Bodypart *part, int notused, FILE *foo) {
  if (strstr(bodyname(*part), txt1)) {

    if (attached(*part))
      bprintf("%s: You've already got one.\n", bodyname(*part));
    else if (part->loc != ploc(mynum))
      bprintf("Your %s is in %s.\n", bodyname(*part), sdesc(part->loc));
    else if (part->vital > 0)
      bprintf("You can't take that, it's part of another bodypart!\n");
    else {
      bprintf("You pick up your %s.\n", bodyname(*part));
      loc(*part) = 0;

      /* pick up feet/hands ... */

      if (!loc(r_leg(mynum)) && vital(r_foot(mynum)))
        loc(r_foot(mynum)) = 0;
      if (!loc(l_leg(mynum)) && vital(l_foot(mynum)))
        loc(l_foot(mynum)) = 0;
      if (!loc(r_arm(mynum)) && vital(r_hand(mynum)))
        loc(r_hand(mynum)) = 0;
      if (!loc(l_arm(mynum)) && vital(l_hand(mynum)))
        loc(l_hand(mynum)) = 0;
    }
    return(1);
  }
  else
    return(0);
}

int take_bodypart() {
   return(applyfunc(mynum, take_bodypart_helper, 0, NULL));
}

void scatter_potions(void) {
  int i, newloc;

  for (i = 0 ; i < numobs ; i++) {
    if (otstbit(i, OFL_POTION)) {
      newloc = my_random () % num_const_locs;
      setoloc(i, newloc, IN_ROOM);
    } 
  }
}

void crawlcom(void) {
  if (!*txt1) {
    bprintf("You must crawl in a direction.\n");
    return;
  }
  else if (!has_hand(mynum) && !has_foot(mynum)) {
    bprintf("You're crippled, prey that the gods help you!\n");
    return;
  }
  else if (cur_player->crawl) {
    bprintf("You're already crawling somewhere.\n");
    return;
  }
  switch (tolower(*txt1)) {
    case 'n':
      cur_player->crawl = VERB_NORTH; break;
    case 's':
      cur_player->crawl = VERB_SOUTH; break;
    case 'e':
      cur_player->crawl = VERB_EAST;  break;
    case 'w':
      cur_player->crawl = VERB_WEST;  break;
    case 'u':
      cur_player->crawl = VERB_UP;    break;
    case 'd':
      cur_player->crawl = VERB_DOWN;  break;
    default:
      bprintf("That's not a direction.\n");
      return;
  }
  bprintf("You start crawling.\n");
  cur_player->crawl += CRAWL_SECS;
}
