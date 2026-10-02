#ifdef LOCMIN_NOXYPICKLE
  if (newch == LOC_NOXYPICKLE_STEP) {
    if( iswornby( OBJ_NOXYPICKLE_HELMET, mynum ) ) {
       bprintf( "As you climb the steps the birds overhead start to frantically drop guano on\n"
                "you.  The wonderful helmet of Noxypickle fortunately protects you from the\n"
                "guano's corrosive effects.\n" );
    } 
  }
#endif

#if defined( LOCMIN_PYRAMID ) && defined( LOCMIN_CRYSTALCAVE )
  if( ( newch == ploc( max_players + MOB_PYRAMID_HORUS )) 
      && ( ( alive( (max_players + MOB_PYRAMID_HORUS ) ) ) != -1 ) ) {
    if( ( !iscarrby( OBJ_CRYSTALCAVE_EARTHSTONE, mynum ) ||
          !iscarrby( OBJ_CRYSTALCAVE_SUNSTONE, mynum ) ||
          !iscarrby( OBJ_CRYSTALCAVE_SEASTONE, mynum ) ||
          !iscarrby( OBJ_CRYSTALCAVE_WINDSTONE, mynum ) ) ) { 
      bprintf( "Horus lets out a bray of a laugh.  &+W\"How can a puny mortal like you hope to defeat the Sun of Isis!?!\"\n" );
    }
    else
      bprintf( "Horus eyes you with distaste and loathing.  &+W\"Prepare to meet the God of Death!!!\"\n" );
  }
#endif

#ifdef LOCMIN_CRYSTALCAVE /* Mancini */
  if( newch == LOC_CRYSTALCAVE_CRYSTALCAVE5 ) {
    if( ( alive ((max_players + MOB_CRYSTALCAVE_ARCHAEOLOGIST)) != -1) &&
          plev(mynum) < LVL_WIZARD && 
          LOC_CRYSTALCAVE_CRYSTALCAVE5 == ploc(max_players + MOB_CRYSTALCAVE_ARCHAEOLOGIST ) ) {
      bprintf( "With the last of his strength, the dying man hands you his precious\n"
	       "journal.  With his final breath, he utters, \"&+WRid the land of the curse!\n"
               "&+WGreat treasures and power can be found...\"\n" );
      setoloc (OBJ_CRYSTALCAVE_JOURNAL, mynum, CARRIED_BY);
    } 
  }
#endif

#ifdef LOCMIN_VILLAGE
if (newch == LOC_VILLAGE_HOSPITAL) {
  newch = ploc(mynum);
  setploc(mynum,LOC_VILLAGE_HOSPITAL);
  lookin(ploc(mynum),0);
  send_msg(int2idx(newch, LOC), 0, pvis(mynum), LVL_MAX, NOBODY, NOBODY,
          "%s has entered the hospital.\n",pname(mynum));
  hospital();
  return -1;
}
#endif
