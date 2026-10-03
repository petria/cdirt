/* here is where all of the specials are when a mobile dies */
switch (pnum(victim)) {

#ifdef LOCMIN_COTTAGE
case MOB_COTTAGE_RAMSES:
     sendf(attacker, "You have destroyed the evil Pharaoh Ramses the IV!\n");
     set_quest(attacker, Q_RAMSES);
     break;
#endif

#ifdef LOCMIN_RUINS
case MOB_RUINS_MARGOYLE: 
      sendf(attacker, "As the monster collapses over, his horn falls off.\n");
      setoloc (OBJ_RUINS_HORN, ploc (attacker), IN_ROOM);
break;    
#endif

#ifdef LOCMIN_NIBELUNG
case MOB_NIBELUNG_FAFFNER: 
	sendf(attacker,"Faffner is DEAD!  R.I.P.!!!\n");
        set_quest(attacker, Q_FAFFNER);
break;   
#endif 

#ifdef LOCMIN_DOOM
case MOB_DOOM_DOOM: 
    sendf(attacker,"Doom is DEAD!  R.I.P.!!!\n");
    set_quest(attacker, Q_DOOM);
    break;   
#endif

#ifdef LOCMIN_ROME
case MOB_ROME_JUPITER:
    sendf(attacker, "Jupiter is DEAD!  R.I.P.!!!\n");
    set_quest(attacker, Q_JUPITER);
    break;
#endif

#ifdef LOCMIN_FANTASY
case MOB_FANTASY_SILVERMONSTER: 
      sendf (attacker, "The wall to the west suddenly collapses as the "
             "monster lets out its\nlast breath..\n");
      setobjstate (OBJ_FANTASY_CAVEWALLOUT, 0);
break;    

case MOB_FANTASY_JULIUS: 
      sendf (attacker, "You grab Julius and order him to tell you where "
             "Lord Glaive is as he\nlets out his last breath.. "
             "Julius waves his hands and you start to\nfeel faint.. you "
             "awaken finding yourself in a different place..\n");
      setploc (attacker, LOC_FANTASY_TREE1);
      setup_globals (attacker);
      trapch (ploc (attacker));
      setup_globals (victim);
break;     
#endif

#ifdef LOCMIN_CAVE
case MOB_CAVE_DIAMAR: 
  if (iswornby(OBJ_CAVE_WHISTLE, max_players+MOB_CAVE_DIAMAR)) {
    sendl(ploc(victim), "%s crushes the whistle as he falls to the floor.\n",
          pname(victim));
    destroy(OBJ_CAVE_WHISTLE); }
    break;  

case MOB_CAVE_KEEPER:
  if (ploc(max_players+MOB_CAVE_KEEPER) == LOC_CAVE_ARMORY) {
    sendl(ploc(victim), "The Keeper heaves his last breath as he falls against"
          " the north wall,\ncausing it to shift.\n");
       setobjstate(OBJ_CAVE_HOLE, 0);  }
    break;
#endif

#ifdef LOCMIN_VOLCANO
case MOB_VOLCANO_QUEEN:  {
sendf(attacker, "\nBy killing the Queen, you have ended her reign of terror "
	      "and freed the people\nof Yorrin!\n" );
      set_quest( attacker, Q_VOLCANO );
	break;
    }
#endif

#ifdef LOCMIN_PYRAMID
case MOB_PYRAMID_NEFERTERI:  {
    sendf(attacker, "Through courage and a bravery of spirit, you have overcome the &+Lcurse&N\nof Neferteri.\n" );
    broad( "&+BThe people in the desert rejoice as the &+Lcurse &+Bof Neferteri is lifted from\n&+Bthe lands forever.\n" );
    set_quest( attacker, Q_MIRAGE ); 
    break;
  }
#endif

#ifdef LOCMIN_OAKTREE
case MOB_OAKTREE_SAPLING:  {
      send_msg(sendloc(attacker), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
        "The Sapling bursts into flames and crumbles to ashes.\n"
        "A silvery object falls to the ground.\n");
      create(OBJ_OAKTREE_SPIKE);
      setoloc(OBJ_OAKTREE_SPIKE, ploc(attacker), IN_ROOM);
      break;
    }
#endif

#ifdef LOCMIN_THYRANNEN
case MOB_THYRANNEN_DRAKNOR:
      if (alive (max_players+MOB_THYRANNEN_BROTHER1) == -1 &&
        alive (max_players+MOB_THYRANNEN_BROTHER2) == -1 &&
        pscore (max_players+MOB_THYRANNEN_BROTHER1) == attacker &&
        pscore (max_players+MOB_THYRANNEN_BROTHER2) == attacker) {
        sendf(attacker, "With a hateful look on his face \001p%s\003 tells you "
             "'Power such as mine is\nincomprehensible to the likes of you.  "
             "I will return, and make you learn what\nPAIN is!'\n",
             pname(victim));
        send_msg(sendloc(attacker), 0, LVL_MIN, LVL_MAX, attacker, victim,
                "As \001p%s\003 delivers the killing blow to \001p%s\003, "
                "\001p%s\003 curses \001p%s\003 with unending pain in "
                "\001p%s\003's next life\n",
                pname(attacker), pname(victim), pname(victim),
                pname(attacker), his_or_her(attacker));
        set_quest (attacker, Q_DRAKNOR);  }
      else { 
         sendf(attacker, "Draknor laughs, and disappears in a wall of "
                         "flame.\nIt seems you have unwittingly allowed "
                         "Draknor a means to escape...for now.\n"); 
      }
	break;
#endif

#ifdef LOCMIN_ABYSS
case MOB_ABYSS_GUXX:  {
      sendf(attacker, "Guxx is DEAD!  R.I.P.!!!\n");
      set_quest (attacker, Q_GUXX);   
	break;  }
#endif


default:   /* These go in default, cause they're handled very differently. */
#ifdef LOCMIN_EFOREST  
    if ((q = victim - max_players) == MOB_EFOREST_LICH)
      x = max_players + MOB_EFOREST_ASMODEUS;
    else if (q == MOB_EFOREST_ASMODEUS)
      x = max_players + MOB_EFOREST_LICH;
    else
      x = -1;
    if (x >= 0) {
      if (alive (x) == -1) {
        send_msg(DEST_ALL, MODE_NODEAF, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
                  "A Great Evil has departed from the land.\n");
        if (pscore (x) == attacker) {
          sendf(attacker, "You have avenged the Elven Forest!\n");
          set_quest (attacker, Q_EFOREST);
        }
      }
    }
#endif

#ifdef LOCMIN_ZODIAC
  if ( (q = victim - max_players) == MOB_ZODIAC_INFINITY) 
     x = max_players +MOB_ZODIAC_ETERNITY;
  else if ( (q = MOB_ZODIAC_ETERNITY) )
     x = max_players + MOB_ZODIAC_INFINITY;
  else  x = -1;
  if( x>=0 ){
      if( alive (x) == -1 && pscore(x) ==attacker){
        set_quest(attacker, Q_ZODIAC);
      }
  }
#endif

break;
} /* end of switch statement */
