#ifdef LOCMIN_NEWBIE
    if (otstbit(ob, OFL_NEWBIE)) {
      if (plev(mynum) > 3 && plev(mynum) < LVL_WIZARD) {
        bprintf("You can't, it's for new players.\n");
        return(1);
      }
      else if (ob < num_const_obs) {
        if (objinv(mynum, oname(ob)) != -1)
          bprintf("You already have it.\n");
        else {
          clone_object(ob, -1, NULL, mynum, CARRIED_BY);
          bprintf("As you take the %s, a new one flashes into existance!\n",
                  oname(ob));
        }
        return(1);
      }
    }
#endif

#ifdef LOCMIN_CAVE
  if (plev (mynum) < LVL_WIZARD)
    if ((l = alive ((max_players + MOB_CAVE_DRAGON))) != -1 &&
        ploc (l) == ploc (mynum)) {
      bprintf ("The dragon makes it clear that he doesn't want "
               "his treasure borrowed!\n");
      return(-2);
    }
#endif


      
#ifdef LOCMIN_MITHDAN
  if ((ob == OBJ_MITHDAN_CRYSTAL) && (state (OBJ_MITHDAN_CRYSTAL) == 1)) {
    setploc (MOB_MITHDAN_DRUID + max_players, ploc(mynum));
    setploc (MOB_MITHDAN_DRUID2 + max_players, ploc(mynum));
    bprintf("The druids, angered at you for defiling their sacred relic, shout "
            "an\nancient oath and leap towards you.\n");
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
      "A pair of druids appear on each side of %s and attack!\n", pname(mynum));
    setpagg (MOB_MITHDAN_DRUID + max_players, 100);
    setpagg (MOB_MITHDAN_DRUID2 + max_players, 100);
    setpspeed (MOB_MITHDAN_DRUID + max_players, 0);
    setpagg (MOB_MITHDAN_DRUID2 + max_players, 100);
    setpspeed (MOB_MITHDAN_DRUID + max_players, 0);
    setpspeed (MOB_MITHDAN_DRUID2 + max_players, 0);
    setobjstate (OBJ_MITHDAN_CRYSTAL, 0);
    hit_player (MOB_MITHDAN_DRUID + max_players, mynum, -1);
    hit_player (MOB_MITHDAN_DRUID2 + max_players, mynum, -1);
    return -1;
}
#endif

#ifdef LOCMIN_PYRAMID
  if(ploc(mynum) == LOC_PYRAMID_PYRAMID33 &&
      (alive( max_players+MOB_PYRAMID_NEFERTERI) != -1)
      && plev(mynum) < LVL_ARCHWIZARD  &&
      state(OBJ_PYRAMID_DOORFROMTREAS) == 0 ) {
    bprintf( "As you pick up the %s, the room to the door slams tightly "
             "shut!\n", oname( ob ) );
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
              "As %s reaches down and for the %s, the door to the room "
              "slams tightly closed!\n", pname(mynum), oname(ob) );
    broad( "&+RThe soft sound of satisfied feminine laughter floats in the "
           "air.&N\n" );
    setobjstate( OBJ_PYRAMID_DOORFROMTREAS, 1 );
  }
#endif


#ifdef LOCMIN_RAINFOREST
  if (ploc(mynum) == LOC_RAINFOREST_CITY3) {
    bprintf("The hand of an ancient god strikes from above, crushing you for "
      "your\ninsolence!  You black out, and when you open your eyes again, you "
      "are in\ndifferent surroundings...\n\n");
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
      "%s tries to take the %s and the hand of an ancient god strikes from\n"
      "above crushing them for their insolence!  Suddenly %s disappears...\n",
      pname(mynum), oname(ob), his_or_her(mynum));

    trapch(LOC_HEAVEN_HEAVEN1);
    broad("You hear a squeal of pain as the foot of an ancient god descends with anger.\n" );

    return -1;
  }
#endif

#ifdef LOCMIN_ABYSS
  if (ob == OBJ_ABYSS_BANK1TIMBER) {
    setobjstate(OBJ_ABYSS_BANK1TIMBER, 1);
    ob = OBJ_ABYSS_TIMBER;
    create(ob);
  }
  if (ob == OBJ_ABYSS_BANK2TIMBER) {
    setobjstate(OBJ_ABYSS_BANK2TIMBER, 1);
    ob = OBJ_ABYSS_TIMBER;
    create(ob);
  }
#endif

#ifdef LOCMIN_HEAVEN
  if (ob == OBJ_HEAVEN_VASE  && state(ob)) {
    bprintf("The vase slips from your hand and crashes to the floor.\n");
    bprintf("The revererations caused by the crash cause the ceiling to "
            "cave in!\n");
    destroy(ob);
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
             "%s has left.",pname(mynum));
    setploc(mynum,LOC_HEAVEN_HEAVEN1);
    lookin(ploc(mynum),0);
    broad("You hear ambulance sirens in the distance.\n");
    return -1;
  }
#endif

#ifdef LOCMIN_OAKTREE
  if ((l = alive (max_players + MOB_OAKTREE_COSIMO)) != -1 &&
      ploc (l) == ploc (mynum)) {
    bprintf ("Cosimo guards his treasure jealously.\n");
    return(-2);
  }
  if ((ob == OBJ_OAKTREE_PICTURE) && (state(OBJ_OAKTREE_PICTURE) == 1)) {
    bprintf("You uncover a wall safe behind the picture.\n");
    send_msg(int2idx(LOC_OAKTREE_OAKTREE37, LOC), 0, pvis(mynum), LVL_MAX,
      mynum, NOBODY,
      "%s uncovers a wall safe behind the picture.\n",pname(mynum));
    send_msg(int2idx(LOC_OAKTREE_OAKTREE37, LOC), 0, LVL_MIN, pvis(mynum),
      mynum, NOBODY,
      "Someone uncovers a wall safe behind the picture.\n");
    create(OBJ_OAKTREE_SAFE);
    setoloc(OBJ_OAKTREE_SAFE, LOC_OAKTREE_OAKTREE37, IN_ROOM);
  }
  if (ob == OBJ_OAKTREE_SPIKE) {
    if (!iswornby(OBJ_OAKTREE_GLOVE, mynum)) {
      bprintf("As you touch the spike, you feel a powerful spirit take "
              "control of\nyour body very briefly before your life winks "
              "out like a light...");
      quit_msg("You need the right tool for the job...",
               "Absorbed by a Tree Spirit");
      quit_player(False);
      return -1;
    }
    else if ((alive(max_players+MOB_OAKTREE_SAPLING) == -1) &&
             (pscore(max_players+MOB_OAKTREE_SAPLING) == mynum) &&
              !qtstflg(mynum, Q_SPIKE))
      set_quest(mynum,Q_SPIKE);
  }
#endif

#ifdef LOCMIN_ZODIAC
if (container == OBJ_ZODIAC_PENTAGRAM)
  switch(ob) {
    default:
    break;
    case OBJ_ZODIAC_FIREBALL:
    case OBJ_ZODIAC_WATERBALL:
    case OBJ_ZODIAC_AIRBALL:
    case OBJ_ZODIAC_DIRTBALL:
    case OBJ_ZODIAC_WINE:
      setobjstate(ob, 0);
      setoloc(ob, mynum, CARRIED_BY);
      send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
        "\001p%s takes the %s from the pentagram.\n", pname(mynum), oname(ob));
      bprintf("Ok\n");
      return -1;
   }
#endif

#ifdef LOCMIN_ANCIENT
  if (ob == OBJ_ANCIENT_SUNDISC && state (ob) == 1) {
    if (!iscarrby (OBJ_ANCIENT_ESTONE, mynum) ||
        !iscarrby (OBJ_ANCIENT_QFEATHER, mynum)) {
      bprintf ("You feel that you need more magical equipment "
               "than just your hands for this job.\n");
      return -1;
    } 
    else
      setobjstate (OBJ_ANCIENT_SUNDISC, 0);
  }
#endif

#ifdef LOCMIN_BLIZZARD
  if (ob == OBJ_BLIZZARD_SHIELD) {
    if (ishere (OBJ_BLIZZARD_SHIELD1))
      ob = OBJ_BLIZZARD_SHIELD1;
    else if (ishere (OBJ_BLIZZARD_SHIELD2))
      ob = OBJ_BLIZZARD_SHIELD2;
    
    if( container == -1 ) {    
      if (otstbit (OBJ_BLIZZARD_SHIELD1, OFL_DESTROYED))
        ob = OBJ_BLIZZARD_SHIELD1;
      else if (otstbit (OBJ_BLIZZARD_SHIELD2, OFL_DESTROYED))
        ob = OBJ_BLIZZARD_SHIELD2;
      if (ob == OBJ_BLIZZARD_SHIELD1 || ob == OBJ_BLIZZARD_SHIELD2)
        create (ob);
      else {
        bprintf ("The shields are too firmly secured to the walls.\n");
        return -1;
      }
    }
  }
#endif


#ifdef LOCMIN_FANTASY
  if (ob == OBJ_FANTASY_GOLDSWORD && state (ob) == 1) {
    bprintf ("As you go to take the sword the shopkeeper mentions he would "
        "like a gold\nnugget in exchange for it. He then mentions that "
        "in the mountains behind\nhis shop there are many gold mines.\n\n");
    return -1;
  }
  if (ob == OBJ_FANTASY_SILVERSWORD && state (ob) == 1) {
    bprintf ("As you go to take the sword the shopkeeper mentions he would "
             "like a bag\nof silver in exchange for it. He then speaks of "
             "the dwarven cave to the west\nthat is said to have many hidden "
             "treasures guarded by a monster.\n\n");
    return -1;
  }
  if (ob == OBJ_FANTASY_SILVERARMOR && state (ob) == 1) {
    bprintf ("As you go to take the armor the shopkeeper mentions he would "
             "like a bag\nof silver in exchange for it. He then speaks of "
             "the dwarven cave to the west\nthat is said to have many hidden "
             "treasures guarded by a monster.\n\n");
    return -1;
  }
#endif

#ifdef LOCMIN_CATACOMB
  if (ob == OBJ_CATACOMB_CUPSERAPH &&
      (l = alive ((max_players + MOB_CATACOMB_SERAPH))) != -1 &&
      ploc (l) == ploc (mynum)) {
    bprintf ("\001pThe Seraph\003 says 'Well done, my %s.  "
             "Truly you are a %s of virtue.'\n",
         psex (mynum) ? "daughter" : "son", psex (mynum) ? "woman" : "man");
    bprintf ("The Seraph gestures and you are transported to ...\n");
    send_msg (sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
              "%s vanishes, taking \001p%s\003 with him!\n",
              pname (l), pname (mynum));
    sendl(LOC_START_CHURCH,
           "\001p%s\003 appears, accompanied by an angel!\n", pname (mynum));
    setploc ((max_players + MOB_CATACOMB_SERAPH), LOC_START_CHURCH);
    set_quest (mynum, Q_GRAIL);
    trapch (LOC_START_CHURCH);
  }
#endif

#ifdef LOCMIN_CASTLE
  if (ob == OBJ_CASTLE_RUNESWORD && state (ob) == 1 &&
      ptothlp (mynum) == -1) {
    bprintf ("It's too well embedded to shift alone.\n");
    return -1;
  }
#endif

