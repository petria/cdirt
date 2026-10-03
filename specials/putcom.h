switch(c) {

#ifdef LOCMIN_ZODIAC
  case OBJ_ZODIAC_PENTAGRAM:
    switch (a) {
    default:
      bprintf ("You try to place the %s on the pentagram, but it falls "
               "through \nto the floor!\n", oname (a));
      send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
         "%s try's to place the %s on the pentagram, but it falls through\n"
                "to the floor!\n", pname (mynum), oname (a));
      send_msg(sendloc(mynum), 0, LVL_MIN, pvis (mynum), mynum, NOBODY,
                "Someone try's to place the %s on the pentagram, but "
                "it falls through\nto the floor!\n", oname (a));
      setoloc (a, ploc (mynum), IN_ROOM);
      return;
      break;
    case OBJ_ZODIAC_FIREBALL:
    case OBJ_ZODIAC_WATERBALL:
    case OBJ_ZODIAC_AIRBALL:
    case OBJ_ZODIAC_DIRTBALL:
    case OBJ_ZODIAC_WINE:
      setobjstate (a, 1);
      setoloc (a, ploc (mynum), IN_ROOM);
      if (((state (OBJ_ZODIAC_FIREBALL) == 1) &&
           (state (OBJ_ZODIAC_WATERBALL) == 1) &&
           (state (OBJ_ZODIAC_AIRBALL) == 1) &&
           (state (OBJ_ZODIAC_DIRTBALL) == 1) &&
           (state (OBJ_ZODIAC_WINE) == 1))) {
        destroy( OBJ_ZODIAC_FIREBALL );
        destroy( OBJ_ZODIAC_WATERBALL );
        destroy( OBJ_ZODIAC_AIRBALL );
        destroy( OBJ_ZODIAC_DIRTBALL );
        destroy( OBJ_ZODIAC_WINE );
        bprintf ("As you place the %s upon the altar, the altar suddenly "
                 "flares to an\nintense blinding light!  When your vision "
                 "clears, you are somewhere else!\n\n", oname (a));
        send_msg(sendloc(mynum), MODE_NSFLAG | MS (SFL_BLIND), pvis(mynum),
                  LVL_MAX, mynum, NOBODY,
                 "When %s places the %s upon the altar, the altar suddenly "
                 "flares to\nan intense blinding flash!  When your vision "
                 "clears, you see that %s is gone!",
                  pname (mynum), oname (a), psex (mynum) ? "she" : "he");
        trapch (LOC_ZODIAC_FRONTDOOR);
        send_msg(sendloc(mynum), MODE_NSFLAG | MS (SFL_BLIND), pvis(mynum),
          LVL_MAX, mynum, NOBODY, "%s suddenly appears beside you rubbing %s "
                  "eyes!\n", pname (mynum), his_or_her (mynum));
        set_quest( mynum, Q_ZODIAC );
      }
      else {
        bprintf ("The pentagram lights briefly as you place the %s on the "
                 "pentagram.\n", oname (a));
        send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
                  "The pentagram lights briefly as %s places the %s on the "
                  "pentagram.\n", pname (mynum), oname (a));
        send_msg(sendloc(mynum), 0, LVL_MIN, pvis (mynum), mynum, NOBODY,
                  "The pentagram lights briefly as someone places the %s "
                  "on the pentagram.\n", oname (a));
      }
      return;
    }
    break;
#endif

#ifdef LOCMIN_CAVE
case OBJ_CAVE_HOLE:
    if (a == OBJ_CAVE_WARHAMMER) {
    bprintf("The Keeper of the Gates suddenly appears.\n"
            "The Keeper says 'Only those that prove themselves in mortal "
            "combat may enter\nthrough the gates of Thorbadin.'\n");
    setploc(max_players + MOB_CAVE_KEEPER, ploc(mynum));
    setpfighting(mynum, max_players + MOB_CAVE_KEEPER);
    return;
  } 
  break;
#endif

#ifdef LOCMIN_PYRAMID
case OBJ_PYRAMID_POOL:
    if (otstbit( a, OFL_CONTAINER ) && ploc(mynum) == LOC_PYRAMID_PYRAMID9) {
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
              "After dipping the %s into the pool, %s instantly vanishes.\n",
	      oname(a), pname(mynum) );
    bprintf( "As you fill your %s with water, everything grows hazy.\n",
             oname(a) );
    trapch( LOC_PYRAMID_PYRAMID10 );
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
              "%s appears with a dripping %s\n", pname(mynum), oname(a) );
    return;
  } break;
#endif

#ifdef LOCMIN_NOXYPICKLE
case OBJ_NOXYPICKLE_STATUE:
  if (a == OBJ_NOXYPICKLE_EYE ) {
    char buf[200];
    setobjstate(OBJ_NOXYPICKLE_STATUE, 0);
    destroy( a );
    bprintf( "The eye fits perfectly.  You have restored Polyphemus to his former glory!\n" );
    sprintf( buf, "%s puts the eye into the statue restoring Polyphemus to his former glory!\n",
             pname(mynum) );
    sillycom(buf );
    set_quest( mynum, Q_NOXYPICKLE );
    return; 
  } break;
#endif

#ifdef LOCMIN_CALEDONIA
case OBJ_CALEDONIA_COWPAT:
    if (a == OBJ_CALEDONIA_PINECONE ) {
    bprintf( "A young pine tree begins to grow in the manure.\n" );
    destroy( c );
    destroy( a );
/*    setpfavor(mynum, pfavor(mynum) + 50); */
    create( OBJ_CALEDONIA_SAPLING );
    setpscore(mynum, pscore(mynum) + 300 );
    return;
  } break;
#endif

#ifdef LOCMIN_OAKTREE
case OBJ_OAKTREE_MOUTH1: {
    if ((a != OBJ_OAKTREE_QUARTZ) && (state(OBJ_OAKTREE_MOUTH1) != 0)) { 
      bprintf("It won't fit.\n");
    } else {
    setobjstate(OBJ_OAKTREE_MOUTH1, 0);
    destroy(OBJ_OAKTREE_QUARTZ);
    bprintf("The quartz slides into its mouth and fits perfectly.\n");
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
      "%s slides the quartz into the lions mouth.\n",pname(mynum));
    send_msg(sendloc(mynum), 0, LVL_MIN, pvis(mynum), mynum, NOBODY,
      "Someone slides the quartz into the lions mouth.\n"); }
    return;
    break; } 
#endif

#ifdef LOCMIN_PIRATE
case OBJ_PIRATE_TABLE:
    if ((a == OBJ_VILLAGE_MAP) && (state(OBJ_PIRATE_PLANK1) != 0)) { /* Already done once */
      if (state(OBJ_PIRATE_SAIL) == 1) { /* Sail raised, switch exits */ 
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
        "A ghost pirate fades into view. After carefully studying "
        "the map, he goes\n"
        "outside and sails the ship to a new land.\n");
      setobjstate(OBJ_PIRATE_LADDER2COVE, 1);
      setobjstate(OBJ_PIRATE_PLANK1, 0);
      destroy(OBJ_VILLAGE_MAP);
      } else  {                          /* Sail not raised. */
        send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
          "A ghostly pirate fades into view. After testing the wind "
          "carefully, he\nshakes his head sadly and fades away.\n");  }
    } else {
      bprintf("Nothing happens...\n");    }
    return;  break;


case OBJ_PIRATE_SABRE:
    if (a == OBJ_PIRATE_RUBY &&
        (iscarrby(OBJ_PIRATE_SABRE, mynum) ||
         ishere(OBJ_PIRATE_SABRE))) /* Repair an accessible sabre */ {
    bprintf("You put the ruby in the hilt, the sabre glows an eerie green.\n");
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
      "%s puts the ruby in the hilt, the sabre glows an eerie green.\n",
      pname(mynum));
    send_msg(sendloc(mynum), 0, LVL_MIN, pvis(mynum), mynum, NOBODY,
      "Someone puts the ruby in the hilt, the sabre glows an eerie green.\n");
    osetdamage(OBJ_PIRATE_SABRE, 25); /* aberIV normal is 28 */
    osetbit( OBJ_PIRATE_SABRE, OFL_LIT );
    destroy(OBJ_PIRATE_RUBY);
    set_quest(mynum, Q_SABRE);
    return;
  } break;
#endif

#ifdef LOCMIN_EFOREST

  case OBJ_EFOREST_HOLE:
    if (a != OBJ_EFOREST_HOPE) {
      bprintf ("Nothing happens.\n");
      return;
    }
    if (state (c) == 0) {
      bprintf ("You hear a 'click' sound but nothing seems to happen.\n");
      return;
    }
    bprintf ("The gem clicks into place...\n...and the door opens!\n");
    send_msg(int2idx(LOC_EFOREST_HOT, LOC), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
              "You hear a 'click' sound and the door opens!\n");
    setobjstate (OBJ_EFOREST_HOLE, 0);
    return;

  case OBJ_EFOREST_LAKE:
    if (a != OBJ_EFOREST_SPONGE)
      bprintf ("Nothing happens.\n");
    else if (state (OBJ_EFOREST_LAKE) == 0)
      bprintf ("What lake ? It is dried up!\n");
    else {
      setobjstate (OBJ_EFOREST_LAKE, 0);
      setobjstate (OBJ_EFOREST_SPONGE, 1);
      bprintf ("The sponge seems to miraculously suck up the"
               " water in the lake!\n");
      bprintf ("It has dried the entire lake...wow!\n");
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
                "A sponge seems to suck up the water in the lake!\n");
    }
    return;
#endif

#ifdef LOCMIN_FROBOZZ
  case OBJ_FROBOZZ_WINDOW_OUTSIDE:
    if (a != OBJ_FROBOZZ_LEAFLET_MAILBOX)
      bprintf ("Nothing happens.\n");
    else if (state (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE) == 2) {
      setobjstate (OBJ_FROBOZZ_VAULTDOOR_OUTSIDE, 1);
      bprintf ("You hear a nearly inaudible click from "
               "the southern wall.\n");
    }
    else
      bprintf ("Nothing happens.\n");
    return;
#endif

#ifdef LOCMIN_MOOR
  case OBJ_MOOR_ALTAR:
    if (a == OBJ_MOOR_CHALICE) {
      setobjstate (OBJ_MOOR_ALTAR, 1);
      destroy (OBJ_MOOR_CHALICE);
      bprintf("You carefully put the chalice on the altar.\n");
      return;
    }
    break;
#endif



#ifdef LOCMIN_ANCIENT
  case OBJ_ANCIENT_PEDESTAL:
    if (a != OBJ_ANCIENT_SUNDISC)
      bprintf ("Nothing happens.\n");
    else {
      bprintf ("The sundisc fits perfectly on top of the pedestal with a loud "
               "click!\n");
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
                "You hear a loud click as %s puts the sundisc on "
                "the pedestal.", pname (mynum));

      setoloc (OBJ_ANCIENT_SUNDISC, ploc (mynum), IN_ROOM);

      osetbit (OBJ_ANCIENT_SUNDISC, OFL_NOGET);
      setobjstate (OBJ_ANCIENT_SUNDISC, 2);
      setobjstate (OBJ_ANCIENT_PEDESTAL, 0);
      setpscore (mynum, pscore (mynum) + ovalue (OBJ_ANCIENT_SUNDISC) * 3);
      set_quest (mynum, Q_SUNDISC);
    }
    return;
#endif

#ifdef LOCMIN_ABYSS
case OBJ_ABYSS_FORGE:
   if (state(c)==1) {
    if (a == OBJ_ABYSS_LANCE && state(a)) {
      setobjstate(a, 0);
      bprintf("The lance heats up quickly and begins to glow.\n");
      osetdamage(a,25); /* Max damage is 25 for mortal weapons norm=26 */
      return; }
    } break;

case OBJ_ABYSS_BOX:  {
switch (a) {
#ifdef LOCMIN_HEAVEN
    case OBJ_EFOREST_PURSE_COIN:
    case OBJ_ABYSS_COIN1:      
      bprintf("An unseen magical force whisks you away...\n");
      destroy(a);
      teletrap(LOC_HEAVEN_HEAVEN1); /* Pearly Gates */
      break; 
#else
    case OBJ_EFOREST_PURSE_COIN:
    case OBJ_ABYSS_COIN1:      
      bprintf("There is a slight magical sensation, but nothing happens\n");
      destroy(a);
      break;    
#endif
    case OBJ_EFOREST_TRASH_COIN:
    case OBJ_ABYSS_COIN2:      
      bprintf("An unseen magical force whisks you away...\n");
      destroy(a);
      teletrap(LOC_WASTE_BEFORE);
      break;    
    default:
	break; }

case OBJ_ABYSS_SLOT1:
    if (a == OBJ_ABYSS_CARD2) {
      setobjstate(OBJ_ABYSS_HOMEDOOR, 0);
      bprintf("When you slide the card into the slot, the front door swings "
              "open\n");
      return;
    } break;

case OBJ_ABYSS_SLOT2:
    if (a == OBJ_ABYSS_CARD1)  {
      setobjstate(OBJ_ABYSS_CELLWALL, 0);
      bprintf("As you slide the card into the slot, you hear a soft click,\n");
      bprintf("and the west wall slides back.\n");
      return;
    } break;
}
#endif

#ifdef LOCMIN_GROVE
case OBJ_GROVE_BEAKER:
   if ((a==OBJ_GROVE_PIPETMAN) && (state(OBJ_GROVE_BEAKER)==0)) {
    setobjstate(OBJ_GROVE_BEAKER, 1);
    setobjstate(OBJ_GROVE_PIPETMAN, 1);
    bprintf("You notice something rather odd transpire in the beaker.\n");
    return;  
}  break;
#endif

#ifdef LOCMIN_TALON
case OBJ_TALON_STAFF: 
    if (a == OBJ_TALON_RUBY && otstbit(OBJ_TALON_FIRESTAFF, OFL_DESTROYED)) {
      oclrbit(OBJ_TALON_FIRESTAFF, OFL_DESTROYED);
      setoloc(OBJ_TALON_FIRESTAFF, oloc(OBJ_TALON_STAFF),
              ocarrf(OBJ_TALON_STAFF));
      destroy (OBJ_TALON_STAFF);
      destroy (OBJ_TALON_RUBY);
      bprintf ("A blinding red light fills the room as the ruby clicks into "
               "place.  When you\ncan see again, you find the staff is "
               "glowing a fiery red.\n");
      send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
                mynum, NOBODY, "%s affixes a small ruby on the end of a staff "
                "and suddenly the room is\nfilled with a blinding red light.  "
                "When you can see again, you find the staff\nis glowing a "
                "fiery red.\n", pname(mynum));
      send_msg(sendloc(mynum), MODE_NODEAF, LVL_MIN, pvis(mynum)-1,
                mynum, NOBODY, "You hear a soft click and an 'ouch' "
                "somewhere close by.\n");
    } else
      bprintf ("It doesn't fit, but it looks interesting.  Maybe you could "
               "call it art.\n");
return;
break;
#endif

#ifdef LOCMIN_TOWER
  case OBJ_TOWER_CANDLESTICK:
    if (a != OBJ_TOWER_RED_CANDLE && a != OBJ_TOWER_BLUE_CANDLE 
          && a != OBJ_TOWER_GREEN_CANDLE) {
      bprintf ("You can't do that.\n");
      return;
    }
    if (state (c) != 2) {
      bprintf ("There's already a candle in it!\n");
      return;
    }
    bprintf ("The candle fixes firmly into the candlestick.\n");
    setpscore (mynum, pscore (mynum) + 50);
    destroy (a);
    osetarmor (c, a);
    osetbit (c, OFL_LIGHTABLE);
    osetbit (c, OFL_EXTINGUISH);
    if (otstbit (a, OFL_LIT)) {
      osetbit (c, OFL_LIT);
      setobjstate (c, 0);
      return;
    }
    setobjstate (c, 1);
    oclrbit (c, OFL_LIT);
    return;
  case OBJ_TOWER_BALL:
    if (a == OBJ_TOWER_WAND && oarmor (a) == 0) {
      bprintf ("The wand seems to soak up energy.\n");
      osetarmor (a, 4);
      return;
    }
    bprintf ("Nothing happens.\n");
    return;
#endif

#ifdef LOCMIN_LIMBO
  case OBJ_LIMBO_POUNCIE:
    bprintf ("You can't let go of it!\n");
    return;
#endif

#ifdef LOCMIN_START
  case OBJ_START_UMBRELLA:
  if (state (a) == 1) {
    bprintf ("Close it first...\n");
    return;
  }
  break;
#endif

default:

#ifdef LOCMIN_OAKTREE
  if ((l = alive (max_players + MOB_OAKTREE_COSIMO)) != -1 &&
      ploc(l) == ploc(mynum) && oloc(a) == ploc(mynum)) {
    bprintf("Cosimo guards his treasure jealously.\n");
    return;
  }
#endif

break;  
}
