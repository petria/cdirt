switch(mob) {

#ifdef LOCMIN_CAVE
case MOB_CAVE_DRAGON:
 if (iswielded(OBJ_ICECAVE_DAGGER) && iscarrby(OBJ_ICECAVE_DAGGER, attacker)) {
   bprintf ("The Dragon glares at you and prepares to blast you "
 	    "out of existence with his\nfiery breath... but the "
	    "icy blade sinks deep into the dragon's hide.\nThe "
	    "Dragon crumples to a pile on the floor.\n");
   setpscore (mynum, pscore (mynum) + 500);
   player_died (attacker, victim, -1);
   return;
 }
 break;
#endif

#ifdef LOCMIN_VALLEY
case MOB_VALLEY_CHICKEN:
  if (!psex (victim)) {
    sendf(attacker,
	   "As you look up you see the sky is somehow different.\n"
	   "The next second you're crushed to death as the sky hits you!\n");
    send_msg(sendloc(attacker), 0,
	    pvis (attacker) > 0 ? pvis (attacker) : LVL_MIN, LVL_MAX,
	    attacker, NOBODY,
	    "%s is squashed as %she attacks Chicken Licken.\n",
	    pname (attacker), psex (attacker) ? "s" : "");
    quit_msg("Chicken Licken was right!", "Crushed By The Sky");
    quit_player(False);
    return; 
  } 
  break;
#endif

#ifdef LOCMIN_CATACOMB
case MOB_CATACOMB_SERAPH:
  sendf(attacker,
	 "The Seraph is amused by your foolhardiness.  For your "
	 "impudence, he draws his\nflaming sword and slays you with a "
	 "single blow.\n");
  send_msg(sendloc(attacker), 0,
	    pvis (attacker) > 0 ? pvis (attacker) : LVL_MIN, LVL_MAX,
	    attacker, NOBODY,
	    "%s is sliced in half by the Seraph!\n\003",
	    pname (attacker));
  
  quit_msg("Never pick a fight with an angel..", "Sliced in Half");
  quit_player(False);  return;  break;
  return;

case MOB_CATACOMB_BEGGAR:
  sendf(attacker,
	 "You drop everything as you are summoned by The Seraph.\n"
	 "The Seraph scowls at you contemptuously.  For your lack of "
	 "compassion, he \ndraws his flaming sword and slays you with "
         "a single blow.\n");
  send_msg(sendloc(attacker), 0,
	    pvis (attacker) > 0 ? pvis (attacker) : LVL_MIN, LVL_MAX,
	    attacker, NOBODY,
	    "%s is sliced in half by the Seraph!\n\003",
	    pname (attacker));
  quit_msg("Remember the ten commandments: Thou Shalt Not Kill",
	   "Sliced in Half");
  quit_player(False);
  return;
#endif

#ifdef LOCMIN_SHERWOOD
case MOB_SHERWOOD_EMMY:
  sendf(attacker, "Emmy cries out and you look around "
	 "nervously. Puff then appears with an \near splitting bang, "
         "shaking her head at you. The next thing you know, Puff\n"
	 "pounces on top of you!\n");
  send_msg(sendloc(attacker), 0, pvis (attacker) > 0 ?
	    pvis (attacker) : LVL_MIN, LVL_MAX, attacker, NOBODY, 
            "As %s goes to attack Emmy she cries out! Puff then appears "
            "and pounces\non top of %s!\n", pname (attacker), 
            him_or_her (attacker));
  quit_msg("That's what you get for picking on innocent, sweet, little Emmy!",
	   "Squashed by PUFF");
  quit_player(False);
  return;
#endif

#ifdef LOCMIN_RUINS
case MOB_RUINS_ZEPHERE:
  sendf(attacker, "The mage waves his arms and you don't feel "
	 "like attacking him anymore.\n");
  return;
#endif

#ifdef LOCMIN_ABYSS
case MOB_ABYSS_GUXX:
	if ((weapon == OBJ_ABYSS_LANCE) && (!state(OBJ_ABYSS_LANCE)))  {
            bprintf("Guxx says 'Guxx will win, this demon knows,\n");
            bprintf("          'for with every rhyme...Errrk!\n");
            bprintf("Guxx collapses in a heap as you hit him with your "
                    "lance.\n");
            setpscore(mynum, pscore(mynum) + 750);
            player_died(attacker, victim, -1);
            setpscore(victim, mynum+max_players); 
	    /*remember who and with lance*/  /*QuestSet(mynum,Q_GUXX);*/
            return;
          } 
          break;
#endif

#ifdef LOCMIN_NIBELUNG
case MOB_NIBELUNG_VAMPIRE:
	if (weapon == OBJ_NIBELUNG_KRYSTAL)  {
          send_msg(sendloc(victim), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
                  "The krystal sinks into the vampire's chest.\n"
                  "The vampire shrieks in pain, as his flesh ages, his eyes"
                  " recess into their\n"
                  "sockets, and his body contorts into a horrid shape.  There "
                  "is a pop, and the\n"
                  "Vampire crumbles to dust at your feet.\n");
          setpscore(mynum, pscore(mynum) + 400);
          player_died(attacker, victim, -1);
          return;
        } 
        break;
#endif
 
#ifdef LOCMIN_MITHDAN
case MOB_MITHDAN_RED_DRAGON: {
  sendf(attacker, "The Dragon snarls as you wake him, turns around, "
                   "and swallows you in one gulp!\n");
  send_msg (sendloc(attacker), 0, pvis (attacker) > 0 ? pvis (attacker) :
            LVL_MIN, LVL_MAX, attacker, NOBODY, "%s pokes the dragon. "
            "The dragon wakes up, snarls, and eats %s!\n",
            pname (attacker), him_or_her (attacker));
  quit_msg("Never poke a dragon", "Poked a Sleeping Dragon");
  quit_player(False);
  return;
}
#endif

}
