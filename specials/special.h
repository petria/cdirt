
  setprndsinroom( i, ( prndsinroom( i ) + 1 ) );

#ifdef LOCMIN_PYRAMID /* Mancini */
  if( ploc(i) == LOC_PYRAMID_PYRAMID6 && plev(i) < LVL_WIZARD ) {
    if( prndsinroom( i ) == 3 ) 
      sendf(i, "U-oh, the sand seems to be covering your knees.\n" );
    else if( prndsinroom( i ) == 9 )
      sendf(i, "Watch out, the sand has reached your waist!\n" );
    else if( prndsinroom( i ) > 12 ) {
      sendf(i, "You gasp for your very last breath as the sand swallows you whole.\n" );
      sillycom( "%s gasps for their very last breath before the sand swallows them whole!\n");
      quit_msg("Swallowed by the sand", "&+YStayed In The Quicksand Too Long");
      quit_player(False);
    }
    continue;
  }

  if( ploc(i) == LOC_PYRAMID_PYRAMID11 && plev(i) < LVL_WIZARD ) {
    sendf(i, "Half way up the dune, you lose your footing on the sand and tumble down.\n" );
    trapch( LOC_PYRAMID_PYRAMID4 );
    send_msg(sendloc(i), 0, pvis(i), LVL_MAX, i, NOBODY,
              "You jump out of the way as a wave of sand washes towards you!\n" );
    continue;
  }
#endif /* LOCMIN_PYRAMID */

#ifdef LOCMIN_RAINFOREST /* Mancini */
      if((( ploc(i) == LOC_RAINFOREST_RIVER5 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER6 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER8 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER9 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER11 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER12 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER14 ) ||
          ( ploc(i) == LOC_RAINFOREST_RIVER15 )) && randperc() <= 3 ) {
          switch( rand() % 4 ) {
	  case 0: sendf(i, "The bushes part.  A brown face appears and stares at you, then vanishes!\n" );
                    break;
	  case 1: sendf(i, "You hear a shrill 'whizz' as an arrow flies past you!\n" );
                    break;
	  case 2: sendf(i, "An arrow comes out of nowhere and thuds into the side of your raft!\n" );
                    break;
	  case 3: sendf(i, "A rustling in the bushes warns you of the presence of hostile Indians.\n" );
                    break;
          }
        continue;
	}

#endif /* RAINFOREST */

#ifdef LOCMIN_NOXYPICKLE /* Mancini */
    if( ploc(i) == LOC_NOXYPICKLE_STEP &&
        !iswornby( OBJ_NOXYPICKLE_HELMET, i ) && plev(mynum) < LVL_DEMI ) {
        sendf(i, "As you climb the steps the birds overhead start to frantically drop guano on\n"
                 "you.  You scream, briefly, as the corrosive droppings eat through your skull...\n" );
        quit_msg("Brains melted by guano - what a way to go....",
                 "Smothered By Bird Droppings");
	quit_player(False);
        continue;
    }
#endif

#ifdef LOCMIN_THYRANNEN
      if (plev(i) < LVL_WIZARD &&
          alive (MOB_THYRANNEN_DRAKNOR+max_players) &&
          ploc(MOB_THYRANNEN_DRAKNOR+max_players) ==
           LOC_THYRANNEN_THYRANNEN58 &&
          ploc(i) == ploc(MOB_THYRANNEN_DRAKNOR+max_players) &&
          !iswornby (OBJ_THYRANNEN_CHARM, i)) {
        int draknor=MOB_THYRANNEN_DRAKNOR+max_players;

        sendf(i, "\001p%s\003 points his finger at you and utters the "
               "word: DIE!!!.\n", pname(draknor));

        send_msg(sendloc(i), 0, LVL_MIN, LVL_MAX, i, draknor,
                  "\001p%s\003 destroys \001p%s\003 - you may be next!\n",
                  pname(draknor), pname(i));
        if (pvis(draknor) > plev(mynum))
          bprintf("Someone has destroyed you! You are DEAD!.");
        else
          bprintf("Draknor has destroyed you! You are DEAD!.");
        quit_msg("Draknor Doesn't Like you...",
                 "Annialated by Draknor");
	quit_player(False);
	continue;
      }
#endif

