/* ====================== Special cases for GIVE =========================== */

p = pl - max_players;

switch (p) {

#ifdef LOCMIN_FANTASY
   case MOB_FANTASY_SHOPKEEPERONE: 
     switch (ob) {
        case OBJ_FANTASY_GOLDNUGGET:
        case OBJ_QUARRY_NUGGET:
        case OBJ_QUARRY_NUGGET1: {
          bprintf ("The shopkeeper smiles at you as he places the golden "
             "sword on the counter\nfor you to take.\n\n");
          setobjstate (OBJ_FANTASY_GOLDSWORD, 0);
          break; 
        }
	 default:
	   bprintf("He doesn't seem to be interested in that.\n");
	   return; 
	   break;  
      }
      break;
    case MOB_FANTASY_SHOPKEEPERTWO:
      if (ob == OBJ_FANTASY_SILVER) {
        bprintf ("The shopkeeper smiles at you as he places the silver sword "
             "and the silver\narmor on the counter for you to take.\n\n");
        setobjstate (OBJ_FANTASY_SILVERSWORD, 0);
        setobjstate (OBJ_FANTASY_SILVERARMOR, 0); 
      }
      break; 
#endif


#ifdef LOCMIN_CATACOMB
  case MOB_CATACOMB_BEGGAR: 
    if (otstbit (ob, OFL_FOOD)) {
    bprintf("The Beggar thanks you and greedily devours the %s.\n",
             oname(ob));
    setpscore (mynum, pscore (mynum) + 50);
    eat (ob);
    bprintf ("After finishing his meal, the beggar stares at you and says '");
    switch (my_random () % 4) {
    case 0:
      bprintf ("Charity");
      o = OBJ_CATACOMB_PIT_NORTH;
      break;
    case 1:
      bprintf ("Faith");
      o = OBJ_CATACOMB_PIT_EAST;
      break;
    case 2:
      bprintf ("Wisdom");
      o = OBJ_CATACOMB_PIT_SOUTH;
      break;
    case 3:
      bprintf ("Courage");
      o = OBJ_CATACOMB_PIT_WEST;
      break;
    }
    bprintf ("'\n");

/* BUG here?
    for (i = 0; (j = pits[i++]) != -1;)
      setobjstate (j, 0);
*/
    setobjstate (o, 1);
    sendl(oloc(o), "A faint glow emanates from the pit.\n");
    return; }
    break;
  
#endif

#ifdef LOCMIN_FOREST
#ifdef LOCMIN_BLIZZARD
case MOB_BLIZZARD_SEAMAS:
  if (ob == OBJ_FOREST_PIPES) {
    setoloc (ob, pl, CARRIED_BY);
    setpscore (mynum, pscore (mynum) + 50);
    bprintf ("Seamas takes the pipes, thanks you, and begins to play a "
             "haunting melody.\n");
    broad ("A haunting pipe melody echoes through the air.\n");
    return; }
    break;
  
#endif
#endif

#ifdef LOCMIN_CAVE
#ifdef LOCMIN_VALLEY
case MOB_VALLEY_PIPER:
  if (ob == OBJ_CAVE_BAGPIPES)  {
    setoloc (ob, pl, CARRIED_BY);
    bprintf ("The Piper thanks you and begins to play.\n");
    setpscore (mynum, pscore (mynum) + 50);
    broad ("In the distance you hear a stirring bagpipe rendition of "
           "'Amazing Grace.'\n");
    return; }
    break;
  
#endif
#endif

#if defined(LOCMIN_START) && defined(LOCMIN_VALLEY)
case MOB_VALLEY_CHICKEN:
  if (ob == OBJ_START_UMBRELLA)  {
    setoloc (ob, pl, CARRIED_BY);
    bprintf ("It looks confused.\n");
    bprintf ("It probably doesn't know how to use it, but it takes the "
             "umbrella anyway.\n");
    setpscore (mynum, pscore (mynum) + 50);
    return;  
  }
	break;
#endif

#ifdef LOCMIN_CAVE
case MOB_CAVE_FORGEMASTER:
    if (ob == OBJ_CAVE_WARHAMMER || ob == OBJ_CAVE_ORE) {
    if (iscarrby(OBJ_CAVE_WARHAMMER, pl) ||
        iscarrby(OBJ_CAVE_ORE, pl)) {
          bprintf("With the Warhammer and Ore, The Forgemaster "
                  "creates and Axe of Mithril.\n");
          create(OBJ_CAVE_MITHRIL);
          destroy(OBJ_CAVE_ORE);
          broad("\001dThe ringing of metal being pounded can be "
                "heard in the distance.\n\003");
          set_quest(mynum, Q_MITHRIL);
        }
  }
break;

case MOB_CAVE_DIAMAR:
  if (ob == OBJ_CAVE_AXE)
    if (iswornby(OBJ_CAVE_WHISTLE, pl)) {
      bprintf("Diamar thanks you for returning his axe.\n"
              "In return he offers you his whistle as a reward.\n");
      setoloc(OBJ_CAVE_WHISTLE, mynum, CARRIED_BY);
      setoloc(OBJ_CAVE_AXE, pl, WIELDED_BY);
      return;
    }
break;
#endif

#ifdef LOCMIN_EVOLUTION
case MOB_EVOLUTION_PALAEONTOLOGIST: {
     int objs = 0;
     setoloc(ob, pl, CARRIED_BY);
     switch( ob )
       {
       default: return;  break;
       case OBJ_EVOLUTION_ASTRASPIS:
       case OBJ_EVOLUTION_DIPLOCAULUS:
       case OBJ_EVOLUTION_MEGAZOSTRODON:
       case OBJ_EVOLUTION_ARDEOSAURUS:
       case OBJ_EVOLUTION_GLEVOSAURUS:
       case OBJ_EVOLUTION_ICARONYCTERIS:
       case OBJ_EVOLUTION_HYLONOMUS:
       case OBJ_EVOLUTION_ARCHAEOPTERYX:
       case OBJ_EVOLUTION_ALPHADON:
       case OBJ_EVOLUTION_MICROTUS:
          if( iscarrby( OBJ_EVOLUTION_ASTRASPIS, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_DIPLOCAULUS, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_MEGAZOSTRODON, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_ARDEOSAURUS, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_GLEVOSAURUS, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_ICARONYCTERIS, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_HYLONOMUS, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_ARCHAEOPTERYX, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_ALPHADON, pl) )
            objs++;
          if( iscarrby( OBJ_EVOLUTION_MICROTUS, pl) )
            objs++;
          if( objs >= 7)
	    {   
                bprintf( "He says \"Thank you!!  Now I can create my display case!!!\n" );
                strcpy(pname(pl), "");
                set_quest(mynum, Q_EVOLUTION);
	      }
        else
          {
          if( objs < 7 )
            bprintf( "He says \"Thank you, just what I needed, now if I just "
		    "had some more!!\"\n" );
          else
            {
	      bprintf("He says \"Thank you!  I'll add these to the others in "
		      "my display case.\n" );
            setpscore(mynum, pscore(mynum) + 200);
          }
        }
        break;
       }
      return;
   }
break;
#endif

#ifdef LOCMIN_OAKTREE
case MOB_OAKTREE_ENT:
  if (ob == OBJ_OAKTREE_WATER) {
    sendl(ploc(mynum), "The Ent says 'Ahhh! I needed that.  Thanks you. A "
      "word of advice to you:'\n             'Beware, to touch the silver "
      "spriti is death.'");
    setploc(pl, LOC_DEAD_DEAD);
    strcpy(pname(pl), "");
    eat(ob);
    dumpstuff(pl, ploc(pl));
    sendl(ploc(mynum), "The Ent makes like a tree and leaves east.\n");
    return;
  }
break;

case MOB_OAKTREE_DRYAD:
  if (ob == OBJ_BLIZZARD_GARLAND) {
    sendl(ploc(mynum), "The Dryad says 'Thank you!  It's very lovely.'\n");
    setobjstate(OBJ_OAKTREE_PORTAL1, 0);
    setploc(pl, LOC_DEAD_DEAD);
    strcpy(pname(pl), "");
    dumpstuff(pl, ploc(pl));
    destroy(ob);
    sendl(ploc(mynum),"The Dryad opens a portal in her tree and goes inside.\n");
    return;
  }
break;
  
case MOB_OAKTREE_VIOLA:
    if (ob == OBJ_OAKTREE_FAN) {
    bprintf("Viola kisses you%s.\n", psex (mynum) == 0 ? "" : " on the cheek");
    bprintf("Viola says 'Thank you, %s.  Won't you please come in?'\n",
             psex (mynum) == 0 ? "kind sir" : "madame");
    setpscore (mynum, pscore (mynum) + 50); }
    break;

#endif

#ifdef LOCMIN_GROVE
case MOB_GROVE_DINTHIAR:
  if (ob == OBJ_EFOREST_CROWN) {
    bprintf("Dinthiar says 'How kind of you for returning my crown! Please, "
           "let me give you\n"
            "something in return to help you find your way back.'\n"
            "Dinthiar gives you the elvencloak.\n"
            "He puts the crown upon his stately brow and smiles.\n");
    setoloc(OBJ_EFOREST_CROWN, MOB_GROVE_DINTHIAR+max_players, WORN_BY);
    create(OBJ_GROVE_ELVENCLOAK);
    setoloc(OBJ_GROVE_ELVENCLOAK, mynum, CARRIED_BY);
    return; }
break;
#endif

#ifdef LOCMIN_VOLCANO
case MOB_VOLCANO_MAYOR:
   if (ob == OBJ_VOLCANO_NOTICE) {
    sendl(ploc(mynum), "The Mayor says, 'You have decided to help us? Thank "
                       "you!  Here is something for your kindness!'\n" );
    create( OBJ_VOLCANO_GOLDPIECE );
    setoloc(OBJ_VOLCANO_GOLDPIECE, mynum, CARRIED_BY);
    destroy(ob);
    return;
  }
break;

case MOB_VOLCANO_DWARF:
   if ( ob == OBJ_VOLCANO_GOLDPIECE ) {
    sendl(ploc(mynum), "The Dwarf says, 'Take this in return.  It will "
                       "help you defeat the ant queen.'\n" );
    create( OBJ_VOLCANO_CRYSKNIFE );
    setoloc( OBJ_VOLCANO_CRYSKNIFE, mynum, CARRIED_BY );
    destroy(ob);
    return;
  }
break;
#endif /* LOCMIN_VOLCANO */

#ifdef LOCMIN_THYRANNEN
case MOB_THYRANNEN_BROTHER1: {
  if (ob == OBJ_THYRANNEN_POT && state (OBJ_THYRANNEN_POT) == 1) {
      bprintf ("He refuses.\n");
      return; }
  if (ob == OBJ_THYRANNEN_POT ||
      ob == OBJ_THYRANNEN_SOIL ||
      ob == OBJ_THYRANNEN_SPADE ||
      ob == OBJ_THYRANNEN_SEED) {
    int i = 0;    
    if( iscarrby( OBJ_THYRANNEN_POT, p+max_players) ) i++;
    if( iscarrby( OBJ_THYRANNEN_SOIL, p+max_players) ) i++;
    if( iscarrby( OBJ_THYRANNEN_SPADE, p+max_players) ) i++;
    if( iscarrby( OBJ_THYRANNEN_SEED, p+max_players) ) i++;
    
    if (i < 3)
      bprintf ("The old man thanks you.\n");
    else {                                     /* he has everything */
      setobjstate (OBJ_THYRANNEN_POT, 1);
      destroy (OBJ_THYRANNEN_SOIL);
      destroy (OBJ_THYRANNEN_SPADE);
      destroy (OBJ_THYRANNEN_SEED);
      setoloc (OBJ_THYRANNEN_POT, mynum, CARRIED_BY);
      send_msg (sendloc(pl), 0, LVL_MIN, LVL_MAX, pl, NOBODY,
		"\001p%s\003 smiles, and gently puts the soil, and the seed "
		"in the pot. Then he\npacks it with the spade, and waters "
		  "it.\n", pname(pl));
      bprintf ("\001p%s\003 gives you the %s.\n", pname(pl),
	       oname(OBJ_THYRANNEN_POT));
      send_msg (sendloc(pl), 0, LVL_MIN, LVL_MAX, pl, mynum,
		"\001p%s\003 gives \001p%s\003 the %s.\n",
		pname(pl), pname(mynum), oname(OBJ_THYRANNEN_POT));
      setpscore (pl, mynum);
    }
  }
  else {         /* not a special object */ 
    bprintf("You keep it, thanks.\n");
    return;
  }
break;
}

case MOB_THYRANNEN_BROTHER2: {
  if (ob == OBJ_THYRANNEN_POT ||
      ob == OBJ_THYRANNEN_GUANO ||
      ob == OBJ_THYRANNEN_GOLD ||
      ob == OBJ_THYRANNEN_IRON) {
    i = 0;
    if( iscarrby( OBJ_THYRANNEN_POT, p+max_players ) ) i++;
    if( iscarrby( OBJ_THYRANNEN_GUANO, p+max_players) ) i++;
    if( iscarrby( OBJ_THYRANNEN_GOLD, p+max_players) ) i++;
    if( iscarrby( OBJ_THYRANNEN_IRON, p+max_players) ) i++;
    
    if (i  < 3)
      bprintf("The old alchemist thanks you.\n");
    else {                                     /* he has everhthing he needs */
      setobjstate (OBJ_THYRANNEN_CAULDRON, 1);
      destroy (OBJ_THYRANNEN_POT);
      destroy (OBJ_THYRANNEN_GUANO);
      destroy (OBJ_THYRANNEN_GOLD);
      destroy (OBJ_THYRANNEN_IRON);
      setoloc (OBJ_THYRANNEN_CHARM, oloc(OBJ_THYRANNEN_CAULDRON), IN_ROOM);
      send_msg (sendloc(pl), 0, LVL_MIN, LVL_MAX, pl, NOBODY,
		"\001p%s\003 gently takes each petal from the flower, and "
		"puts them in the\ncauldron.  Next he drops in the iron ore, "
		"then the gold ore, and finally the\nbat guano.  He utters "
		"some incantations, and suddenly the water begins to\nboil.  "
		"The alchemist repeats the chant, over and over, louder and "
		"louder,\nuntil your ears ring with his chants... Then he "
		"stops, seconds pass, then\nwith a loud crash the cauldron "
		"cracks!  It's boiling contents drain onto the\nground, and "
		"the old alchemist drops to the ground in exhaustion.\n",
		pname(pl));
      bprintf ("The old man tells you, \"Now take the charm, and defeat "
               "draknor!\"\n");
      setpscore (pl, mynum);
    }
  }
  else {
    bprintf("No thanks, you keep it.\n");
    return;
  }
break;
}
#endif


default:
	break; 
}
#ifdef LOCMIN_VALLEY
  if (ob == OBJ_VALLEY_ROSE && psex (mynum) != psex (pl)) {
    bprintf ("You give %s the %s.\n", him_or_her (pl), oname (ob));
    setpscore (mynum, pscore (mynum) + 60);
    setpscore (pl, pscore (pl) + 50);
    sendf(pl, "%s gives you the %s.\n", see_name (pl, mynum), oname(ob));
    setoloc (ob, pl, CARRIED_BY);
    if (++odamage (ob) >= 2) {
      sendl(ploc(pl), "The %s turns to dust.\n", oname (ob));
      destroy (ob);
    }
    return;
  }
#endif

#ifdef LOCMIN_CASTLE
  if (ob == OBJ_CASTLE_RUNESWORD && plev(mynum) < LVL_WIZARD) {
    bprintf ("The Runesword does not wish to be given away.\n");
    return;
  }
#endif

