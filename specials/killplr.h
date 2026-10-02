/* parts missing from here */

#ifdef LOCMIN_VOLCANO
  if( mob == MOB_VOLCANO_QUEEN && 
     ( iswielded( OBJ_VOLCANO_CRYSKNIFE ) || ob1 == OBJ_VOLCANO_CRYSKNIFE )) {
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
    "%s plunges the crysknife into the queen's head killing her instantly!\n",
        pname( mynum ) );
    bprintf( "\nYou plunge the crysknife into the queen's head, "
	    "killing her instantly!\n" );
    player_died(mynum, pl1, -1);
    return;
  }
#endif /* LOCMIN_VOLCANO */

#ifdef LOCMIN_PIRATE /* Ithor (J.Willie) Orig zone from AberIV */
  if (mob == MOB_PIRATE_MERMAID && !ststflg(mynum,SFL_DEAF)) {
    bprintf("Her music is too beautiful. You can't bring yourself to "
            "attack her.\n");
    return;
  }
#endif /* PIRATE ZONE */

#ifdef LOCMIN_HEAVEN
# ifdef LOCMIN_WASTE
  if (mob == MOB_HEAVEN_PETER) {
    int save_mynum = mynum;

    setup_globals(attacker);
    bprintf ("Peter sighs, waves his hand, and the floor vanishes from "
             "under your feet.\n");
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(victim), LVL_MAX,
              mynum, NOBODY, "Peter sighs, waves his hand, and "
              "\001p%s\003 disappears.\n", pname(mynum));
    trapch (LOC_WASTE_BEFORE);
    bprintf ("You suddenly don't feel so good anymore.\n");
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(attacker), LVL_MAX,
              mynum, NOBODY, "\001p%s\003 suddenly appears.\n",
              pname(mynum));
    send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
              "A voice says '&+YBad Move!&N'\n");
    setup_globals(save_mynum);
    return;
  }
# endif
#endif

#ifdef LOCMIN_VALLEY
  if (mob == MOB_VALLEY_CHICKEN && !psex(victim)) {
    sendf (attacker, "As you look up you see the sky is somehow different.\n"
           "The next second you're crushed to death as the sky hits you!\n");
    send_msg(sendloc(attacker), 0, pvis(attacker), LVL_MAX, attacker, NOBODY,
              "%s is squashed as %she attacks Chicken Licken.\n",
              pname(attacker), psex(attacker) ? "s" : "");
    quit_msg("Chicken Licken was right!", "Squashed by a Chicken");
    quit_player(False);
    return;
  }
#endif

#ifdef LOCMIN_CATACOMBS
  if (mob == MOB_CATACOMB_SERAPH) {
    sendf (attacker, "The Seraph is amused by your foolhardiness.  For your "
           "impudence, he draws his\nflaming sword and slays you with a "
           "single blow.\n");
    send_msg(sendloc(attacker), 0, pvis(attacker), LVL_MAX, attacker, NOBODY,
              "%s is sliced in half by the Seraph!\n\003", pname(attacker));
    quit_msg ("Never pick a fight with an angel..", 
              "Sliced in Half by an Angel");
    quit_player(False);
    return 1;
  }

  if (mob == MOB_CATACOMB_BEGGAR) {
    sendf (attacker, "You drop everything as you are summoned by The Seraph.\n"
           "The Seraph scowls at you contemptuously.  For your lack of "
           "compassion, he\ndraws his flaming sword and slays you with a "
           "single blow.\n");
    send_msg(sendloc(attacker), 0, pvis(attacker), LVL_MAX, attacker, NOBODY,
              "%s is sliced in half by the Seraph!\n\003", pname(attacker));
    quit_msg ("Remember the ten commandments: Thou Shalt Not Kill",
              "Sliced in Half by the Seraph");
    quit_player(False);
    return 1;
  }
#endif /* catacombs */

