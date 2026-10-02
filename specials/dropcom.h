#ifdef LOCMIN_NEWBIE
  if (otstbit(a, OFL_NEWBIE)) {
    bprintf("As you drop the %s, it disappears with a flash!\n", oname(a));
    setoloc(a, oloc_reset(a), IN_ROOM);
    destruct_object(a);
    return;
  }
#endif

#ifdef LOCMIN_CASTLE
  if (plev (mynum) < LVL_WIZARD && a == OBJ_CASTLE_RUNESWORD) {
    bprintf ("You can't let go of it!\n");
    return;
  }
#endif

#ifdef LOCMIN_POUNCIE
  if (plev (mynum) < LVL_WIZARD && a == OBJ_LIMBO_POUNCIE) {
    bprintf ("You can't let go of it!\n");
    return;
  }
#endif

#ifdef LOCMIN_CATACOMB
  if (a == OBJ_CATACOMB_CUPSERAPH
      && ploc ((max_players + MOB_CATACOMB_SERAPH)) == LOC_START_CHURCH)
    setplev ((max_players + MOB_CATACOMB_SERAPH), -2);
#endif

#ifdef LOCMIN_QUARRY
  if (l == LOC_QUARRY_TUNNEL || l == LOC_QUARRY_LADDER_UU) {
    bprintf ("The %s falls down the ladder.\n", oname (a));
    l = LOC_QUARRY_LADDER_UD;
  }
  if ((l >= LOC_QUARRY_LADDER_D4 && l <= LOC_QUARRY_LADDER_D2) ||
      (l >= LOC_QUARRY_LADDER_D1 && l <= LOC_QUARRY_LADDER_UD)) {
    bprintf ("The %s falls down the ladder.\n", oname (a));
    l = LOC_QUARRY_MINE;
  }
#endif

#ifdef LOCMIN_OAKTREE
  if ((l >= LOC_OAKTREE_MAGNOLIA && l <= LOC_OAKTREE_TREE1)
      || l == LOC_OAKTREE_ILEX
      || (l >= LOC_OAKTREE_WALNUT && l <= LOC_OAKTREE_FIG)) {
    bprintf ("The %s falls through the leaves to the ground far below.\n",
             oname (a));
    l = LOC_OAKTREE_GROVE;
    sendl(l, "Something falls to the ground.\n");
  }
#endif

#ifdef LOCMIN_CATACOMB
  if (ploc(mynum) == oloc(OBJ_CATACOMB_PIT_NORTH) ||
      ploc(mynum) == oloc(OBJ_CATACOMB_PIT_SOUTH) ||
      ploc(mynum) == oloc(OBJ_CATACOMB_PIT_EAST) ||
      ploc(mynum) == oloc(OBJ_CATACOMB_PIT_WEST)) {
    bprintf ("The %s disappears into the bottomless pit....."
             "and hits bottom.\n", oname (a));
    send_msg (sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
              "\001p%s\003 drops the %s into the pit.\n",
              pname (mynum), oname (a));
    l = LOC_CATACOMB_CHAMBER;
    setoloc (a, l, IN_ROOM);    /* to spherical room in CATACOMB section */
    sendl(l, "Something falls to the ground.\n");
    return;
  }
#endif

#ifdef LOCMIN_ABYSS
  if (a == OBJ_ABYSS_TIMBER) {
    if (ploc(mynum) == LOC_ABYSS_BANK1) {
      setobjstate(OBJ_ABYSS_BANK1TIMBER,0);
      setoloc(a, ploc(mynum), IN_ROOM);
      bprintf("You place the timber across the river of slime.\n");
      destroy(a);
      return;
    }
    else if (ploc(mynum) == LOC_ABYSS_BANK2) {
      setobjstate(OBJ_ABYSS_BANK2TIMBER,0);
      setoloc(a, ploc(mynum), IN_ROOM);
      bprintf("You place the timber across the river of slime.\n");
      destroy(a);
      return;
    }
    else if (ploc(mynum) == LOC_ABYSS_BANK2) {
      setobjstate(OBJ_ABYSS_BANK2TIMBER,0);
      setoloc(a, ploc(mynum), IN_ROOM);
      bprintf("You place the timber across the river of slime.\n");
      destroy(a);
      return;
    }
  }
#endif

