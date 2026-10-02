
#ifdef LOCMIN_EVOLUTION
 case OBJ_EVOLUTION_STONE:
    if( !find_stuff( OBJ_EVOLUTION_STONE, OBJ_EVOLUTION_ARDEOSAURUS,
                "When you move the stone, you find an ardeosaurus!\n" ) )
        bprintf( "You move the stone but nothing happens.\n" );
    return;
    break;

 case OBJ_EVOLUTION_BRANCH:
    if( !find_stuff( OBJ_EVOLUTION_BRANCH, OBJ_EVOLUTION_MEGAZOSTRODON,
                "You move the branch and find a megazostrodon!\n" ) )
        bprintf( "You move the branch but nothing happens.\n" );
    return;
    break;

#endif /* LOCMIN_EVOLUTION */


#ifdef LOCMIN_ABYSS
 case OBJ_ABYSS_BUTTON1:
      if (!state(OBJ_ABYSS_BUTTON1)) {
        setobjstate(OBJ_ABYSS_CLOSETPANEL, 0);
        setobjstate(OBJ_ABYSS_BUTTON1, 1);
      }
      else {
        setobjstate(OBJ_ABYSS_CLOSETPANEL, 1);
        setobjstate(OBJ_ABYSS_BUTTON1, 0);
      }
      bprintf("You hear a soft swish come from below.\n");
      return;
 case OBJ_ABYSS_BUTTON2:
      if (state(OBJ_ABYSS_HALLELEVATORDOOR)) {
        setobjstate(OBJ_ABYSS_HALLELEVATORDOOR, 0);
        bprintf("The elevator door swishes open.\n");
      }
      return;
 case OBJ_ABYSS_BUTTON3:
      if (!state(OBJ_ABYSS_ELEVATORDOOR)) {
        setobjstate(OBJ_ABYSS_ELEVATORDOOR, 1);
        bprintf("The elevator door swishes closed.\n");
        bprintf("The elevator zips upward at an astonishing pace and deposits "
                "you here.\n");
        teletrap(LOC_CHURCH_N_BARROW);
      }
      return;
 case OBJ_ABYSS_DUST:
      if (!state(OBJ_ABYSS_DUST)) {
        setobjstate(OBJ_ABYSS_DUST, 1);
        create(OBJ_ABYSS_BUTTON1);
        bprintf("You sweep the dust aside to reveal a square button on the "
                "shelf.\n");
        return;
      }
      break;
 case OBJ_ABYSS_L663BOULDER:
      if (ptothlp(mynum) == -1) {
        bprintf("You can't seem to get enough leverage to move it.\n");
        break;
      }

      sendf(ptothlp(mynum), "\001P%s\003 pushes the boulder aside with your "
                           "help.\n", pname(mynum));
      setobjstate(x, 1 - state(x));
      oplong(x);
      return;
#endif


#ifdef LOCMIN_PIRATE
 case OBJ_PIRATE_MASTROPE:
      if (state(OBJ_PIRATE_SAIL) == 0) {
        bprintf("You raise the sail, which promptly catches the wind.\n");
        setobjstate(OBJ_PIRATE_SAIL, 1);
      }
      else {
        bprintf("You lower and furl the sail.\n");
        setobjstate(OBJ_PIRATE_SAIL, 0);
      }
      return;
#endif

#ifdef LOCMIN_TALON
 case OBJ_TALON_CHINA_FIG:
      if (state(x)) {
        bprintf ("A large stone slab slides away in the floor, revealing a "
                 "passage leading down.\n");
        send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
                  mynum, NOBODY, "As %s pushes the china figurine, a large "
                  "stone slab slides away in the\nfloor, revealing a passage "
                  "leading down.\n", pname(mynum));
        send_msg(sendloc(mynum), MODE_NOBLIND, LVL_MIN, pvis(mynum)-1,
                  mynum, NOBODY, "The china figurine suddenly leans over and "
                  "a large stone slab slides away in\nthe floor, revealing a "
                  "passage leading down.\n");
        setobjstate(x, 0);
        setobjstate(OBJ_TALON_DOOR, 0);
        return;
      } else {
        bprintf ("The slab of stone slides over the hole in the floor, "
                 "blocking the passage.\n");
        send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
                  mynum, NOBODY, "%s returns the china figurine to an upright "
                  "position, and the slab of\nstone slides over the hole, "
                  "blocking the passage.\n", pname(mynum));
        send_msg(sendloc(mynum), MODE_NOBLIND, LVL_MIN, pvis(mynum)-1,
                  mynum, NOBODY, "The china figurine suddenly returns to an "
                  "upright position and the slab of\nstone slides over the "
                  "hole, blocking the passage.\n");
        setobjstate(x, 1);
        setobjstate(OBJ_TALON_DOOR, 1);
        return;
      }
      break;
#endif

#ifdef LOCMIN_FAIRYTALE
  case OBJ_FAIRYTALE_NULL_DRAWBRIDGE:
    setobjstate(x, 1 - state(x));
    if (state(x) == 1)
        sendl(oloc(x), "The drawbridge is extended with a mighty shove.\n");
    else
    	sendl(oloc(x), "The drawbridge is retracted with a mighty heave.\n");
    return;
    break;
#endif

