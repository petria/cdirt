#ifdef LOCMIN_FANTASY
  if (a == OBJ_FANTASY_MANA &&
      ploc (mynum) == LOC_FANTASY_TREE2) {
    bprintf ("As you put the Pendant of Mana around your "
             "neck the world begins to blur and\nshake around "
             "as your eyes close. You feel yourself floating in "
             "air and then\nthe moving stops. You open your eyes "
             "in a different part of the world..\n\n");
    setploc (mynum, LOC_FANTASY_START);
    trapch (ploc (mynum));
    bprintf ("You have freed the mana tree from the grasp of the "
             "evil Lord Glaive.\n\n");
    set_quest (mynum, Q_MANA);
  }
#endif

#ifdef LOCMIN_GROVE
  if((a == OBJ_EFOREST_CROWN) && (ploc(mynum)==LOC_EFOREST_INSIDE))
    {
    bprintf("Your eyes swim and your head swirls.. You suddenly are "
            "clearminded again.\n"
            "BUT.. something has changed.\n");
    teletrap(LOC_GROVE_GRAND);
  }
  if((a == OBJ_GROVE_ELVENCLOAK) && (ploc(mynum)>=LOCMAX_GROVE) &&
     (ploc(mynum)<LOCMIN_GROVE))
    {
    bprintf("As you wear the elvencloak, you get dizzy and close your eyes. "
            "When you open\n"
            "them, you stand in the Grand Oak in your present time.\n");
    teletrap(LOC_EFOREST_INSIDE);
  }
#endif


#ifdef LOCMIN_ANCIENT
  if (a == OBJ_ANCIENT_HEALBALSAM) {
    if (pstr (mynum) < maxstrength (mynum) - 20) {
      setpstr (mynum, pstr (mynum) + 20);
      bprintf ("You feel some of your wounds dissappear.\n");
    } else if (pstr (mynum) < maxstrength (mynum)) {
      setpstr (mynum, maxstrength (mynum));
      bprintf ("The balsam heals all your wounds!\n");
    } else {
      bprintf ("The balsam has a nice cooling effect.\n");
    }
    destroy (a);
    return;
  }
  if (a == OBJ_ANCIENT_EMBBALSAM) {
    bprintf ("You start applying the embalming balsam ...\n");
    bprintf ("You begin to feel sleepy, and after a while some mummies\n"
             "turn up helping you with the embalming ...\n");
    destroy (a);
    quit_msg ("The mummies carry you away to a safe restingplace.  "
              "You are dead...", "Mummified Alive");
    quit_player(False);
    return;
  }
#endif
