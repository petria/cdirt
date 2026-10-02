switch (a) {
#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_BRIDGE_WAND:
    if ((state (OBJ_BLIZZARD_BRIDGE_FIRE) == 1)
        && (oloc (OBJ_BLIZZARD_BRIDGE_FIRE) == ploc (mynum))) {
      setobjstate (OBJ_BLIZZARD_BRIDGE_HALL, 0);
      sendl(ploc(mynum), "The drawbridge is lowered!\n");
      return;
    }
    break;
  case OBJ_BLIZZARD_ROD:
    if (iscarrby (OBJ_CATACOMB_CUPSERAPH, mynum)) {
      bprintf ("Something prevents the rod's functioning.\n");
      return;
    }
    if (oarmor (a) >= 3) {
      bprintf ("The rod crumbles to dust!\n");
      destroy (a);
      return;
    }
    oarmor (a)++;
    bprintf ("You are teleported!\n");
    teletrap (LOC_BLIZZARD_PENTACLE);
    return;
#endif
#ifdef LOCMIN_TOWER
  case OBJ_TOWER_WAND:
    if (oarmor (a) > 0) {
      osetarmor (a, oarmor (a) - 1);
      ivct(mynum) = 60;
      setpvis (mynum, 10);
      bprintf ("You seem to shimmer and blur.\n");
      return;
    }
#endif
#ifdef LOCMIN_TALON
  case OBJ_TALON_FIRESTAFF:
    if (ploc (mynum) == LOC_TALON_TALON25 && state (OBJ_TALON_DOOR2)) {
      bprintf ("The air seems to tingle with magical sensations as you "
               "wave the firestaff,\nand momentarily the smooth cliff wall "
               "before you slides slowly and\nsoundlessly to one side, "
               "revealing a small cave set into the mountainside.\n");
      send_msg(sendloc(mynum), MODE_NOBLIND, pvis (mynum), LVL_MAX,
                mynum, NOBODY, "%s waves the firestaff, and immediately "
                "the smooth cliff wall before you\nslides slowly and "
                "soundlessly to one side, revealing a small cave set into\n"
                "the mountainside.\n", pname (mynum));
      send_msg(sendloc(mynum), MODE_NOBLIND, LVL_MIN, pvis (mynum) - 1,
                mynum, NOBODY, "Suddenly the smooth cliff wall before you "
                "slides slowly and soundlessly to\none side, revealing a "
                "small cave set into the mountainside.\n");
      setobjstate (OBJ_TALON_DOOR2, 0);
      return;
    }
    break;
#endif
}

