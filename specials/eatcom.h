#ifdef LOCMIN_OAKTREE
  case OBJ_OAKTREE_TART:
  case OBJ_OAKTREE_CAKES:
  case OBJ_OAKTREE_TOAST:
    bprintf ("That was delicious, but not very filling.\n");
    eat (b);
    setpstr (mynum, pstr (mynum) + 6);
    calibme ();
    break;
  case OBJ_OAKTREE_SOUP:
    bprintf ("As you finish off the last of the caterpillar consume\n");
    bprintf ("you notice a small diamond in the bottom of the cup.\n");
    eat (b);
    create (OBJ_OAKTREE_CUPDIAMOND);
    setoloc (OBJ_OAKTREE_CUPDIAMOND, mynum, CARRIED_BY);
    create (OBJ_OAKTREE_CUPCHINA);
    setoloc (OBJ_OAKTREE_CUPCHINA, mynum, CARRIED_BY);
    setpstr (mynum, pstr (mynum) + 6);
    calibme ();
    break;
#endif

#ifdef LOCMIN_MOOR
  case OBJ_MOOR_CHALICE:
    bprintf ("However much blood you drink from the chalice it "
             "stays just as stained!\n");
    break;
#endif

#ifdef LOCMIN_NOXYPICKLE
  case OBJ_NOXYPICKLE_PUDDLE:
    bprintf( "The water tastes foul!  You cough and splutter, then collapse on to the ground.\n"
             "You feel yourself floating, as if you are being carried far away....\n"
             "Consciousness returns slowly, and somewhat groggily you open your eyes.\n" );
    teletrap( LOC_NOXYPICKLE_TREE );
    break;
#endif

#ifdef LOCMIN_VALLEY
  case OBJ_VALLEY_SPRING:
    bprintf ("Very refreshing.\n");
    break;
#endif

#ifdef LOCMIN_TREEHOUSE
  case OBJ_TREEHOUSE_WAYBREAD:
    if (plev (mynum) < LVL_WIZARD && !aliased(real_mynum)) {
      pl1 = (my_random () >> 3) % (numchars - 1);
      if (ststflg (pl1, SFL_OCCUPIED) || pl1 < max_players) {
        bprintf ("There is a sudden feeling of failure...\n");
        break;
      }
      polymorph (pl1, 25);      /* aliased for 25 moves */
    }
    setpstr (mynum, pstr (mynum) + 16);
    eat (b);
    break;
#endif

#ifdef LOCMIN_ICECAVE
  case OBJ_ICECAVE_FOUNTAIN:
    if (plev (mynum) >= 1 && plev (mynum) < 3) {
      setpscore (mynum, pscore (mynum) + 40);
      calibme ();
      bprintf ("You feel a wave of energy sweeping through you.\n");
    } else {
      bprintf ("Faintly magical by the taste.\n");
      if (plev (mynum) >= 3 && pstr (mynum) < 10)
        setpstr (mynum, pstr (mynum) + 4);
      calibme ();
    }
    break;
#endif

#ifdef LOCMIN_ANCIENT
  case OBJ_ANCIENT_FOUNTAIN_OF_YOUTH:
    if (pscore (mynum) >= 20) {
      setpscore (mynum, pscore (mynum) - 20);
      calibme ();
      bprintf ("You feel younger and less experienced...\n");
    }
    break;
#endif


#ifdef LOCMIN_TOWER
  case OBJ_TOWER_CAULDRON:
    bprintf ("You feel funny and pass out....\n");
    bprintf ("You wake up elsewhere....\n");
    teletrap (LOC_TOWER_MAGICAL);
    break;
  case OBJ_TOWER_POTION:
    setpstr (mynum, maxstrength (mynum));
    setpmagic (mynum, maxmagic (mynum));
    bprintf ("You feel much much stronger!\n");
    setoloc (b, LOC_DEAD_EATEN, IN_ROOM);
    destroy (b);
    break;
#endif


#ifdef LOCMIN_ABYSS
 case OBJ_ABYSS_SODA:
    setpstr(mynum, maxstrength(mynum));
    bprintf("As the soda gurgles down your dry throat you "
            "feel invigourated!\n");
    eat (OBJ_ABYSS_SODA);
    break;

 case OBJ_ABYSS_POTION1:
    ivct(mynum) = 60;
    setpvis(mynum, 15);
    bprintf("You seem to shimmer and blur.\n");
    eat (OBJ_ABYSS_POTION1);
    break;

 case OBJ_ABYSS_POTION2:
    if (ststflg(MOB_ABYSS_QBERT, SFL_OCCUPIED) || pstr(MOB_ABYSS_QBERT) < 0)
      {  bprintf("There is a sudden feeling of failure...\n");
         return;   }
    else {
      polymorph(max_players + MOB_ABYSS_QBERT, 10); /* aliased for 10 moves */
    setpstr(mynum, pstr(mynum) + 16);
    eat (OBJ_ABYSS_POTION2); }
    break;
#endif

#ifdef LOCMIN_GROVE
 case OBJ_GROVE_BEAKER:
    if(!state(OBJ_GROVE_BEAKER)) {
      bprintf("Eating an empty beaker? Get glass in your teeth!\n");
      return;
    }
    bprintf("Your entire body morphs into a somewhat large pink elephant. You "
            "feel odd\n"
            "walking on four legs and with a trunk, but.. hey, you're not "
            "drunk, are you?\n");
    eat(OBJ_GROVE_BEAKER);
    send_msg(DEST_ALL, 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
            "You hear the trumpeting of a drunk elephant in the distance.\n");
    sprintf(s, "%s the large &+MPINK&n elephant.", pname(mynum));
    setptitle(mynum, s);
    oarmor(OBJ_GROVE_BEAKER)=mynum;
    set_quest(mynum, Q_PINKELEPHANT);
    break;
#endif

#ifdef LOCMIN_NIBELUNG  /* Potion must eat to go down from bench */
 case OBJ_NIBELUNG_POTION:
    setpstr(mynum, ((randperc() % 21) + 1)); /* strength from 1 - 22 */
    osetbit(b, OFL_DESTROYED);
    setoloc(b, mynum, CARRIED_BY);
    break;
#endif

#ifdef LOCMIN_TALON
 case OBJ_TALON_POTION:
    setpstr(mynum, maxstrength(mynum));
    bprintf ("You feel magical energies coursing through your veins like "
             "fire, and when the\nsensations subside, you notice your "
             "physical wounds have disappeared.\n");
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
              mynum, NOBODY, "%s guzzles a potion, and shortly thereafter you "
              "notice %s physical\nwounds disappearing.\n", pname(mynum),
              his_or_her(mynum));
    eat (OBJ_TALON_POTION);
    break;

 case OBJ_TALON_SPRING:
    bprintf ("You feel a shifting of the ground under your feet and a "
             "slight wind on your\nface, and looking about you you find "
             "you have been transported.\n");
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
              mynum, NOBODY, "%s bends to take a drink from the spring, and "
              "no sooner than %s does\nso, %s is suddenly gone.  The whisper "
              "of a cold breeze blows against your\ncheek.\n",
              pname(mynum), psex(mynum) ? "she" : "he",
              psex(mynum) ? "she" : "he");
    send_msg(sendloc(mynum), 0, LVL_MIN, pvis(mynum)-1, mynum, NOBODY,
              "The whisper of a cold breeze blows against your cheek "
              "suddenly, with no\nexplanation for its presence.\n");
    trapch (LOC_TALON_TALON25);
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
              mynum, NOBODY, "%s suddenly appears beside you in a kneeling "
              "position.  %s stands up\nand brushes %sself off, surveying "
              "%s situation.\n", pname(mynum), he_or_she(mynum),
              him_or_her(mynum), his_or_her(mynum));
    break;
#endif
