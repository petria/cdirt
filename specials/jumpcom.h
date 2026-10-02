switch (ploc(mynum)) {

#ifdef LOCMIN_CHLYON
case LOC_CHLYON_CHLYON19:
   bprintf("You jump up and grab a good hold of the crack.\n"
"Moments later, you pull through into the room upstairs.\n");
   teletrap(LOC_CHLYON_CHLYON20);
   return;
break;
#endif

#ifdef LOCMIN_ABYSS
case LOC_ABYSS_SLIMY: 
  if (iswornby(OBJ_ABYSS_RING, mynum)) {
    bprintf( "Light as a feather, you jump over the boulder!\n" );
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, 0,
	      "Light as a feather, %s jumps over the boulder!\n", pname(mynum) );
    trapch( LOC_ABYSS_L663 );
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, 0,
	      "%s lands lightliy onto %s feet!\n", pname(mynum), his_or_her(mynum) );
    return; } 
break; 

case LOC_ABYSS_L663: 
  if (iswornby(OBJ_ABYSS_RING, mynum)) {
    bprintf( "Light as a feather, you jump over the boulder!\n" );
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, 0,
	      "Light as a feather, %s jumps over the boulder!\n", pname(mynum) );
    trapch( LOC_ABYSS_SLIMY );
    send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, 0,
	      "%s lands lightliy onto %s feet!\n", pname(mynum), his_or_her(mynum) );
    return;  } 
break;  
#endif


#ifdef LOCMIN_NOXYPICKLE
case LOC_NOXYPICKLE_BOUGH: {
    bprintf( "You try to jump out of the tree but get tangled up in the branches.\n" );
    return;  break;
  }
case LOC_NOXYPICKLE_CESSPIT: 
    bprintf( "You leap into the cesspit and submerge yourself in the sewage.  The horrendous\n"
             "stink clears your head completely and you come out of your trance-like state.\n"
             "When you open your eyes again you are in a familiar place...\n" );
    teletrap( LOC_NOXYPICKLE_TRAP );
    return;  
break;
#endif

#ifdef LOCMIN_FAREAST
case LOC_FAREAST_GOALROOM:
    if (iswielded(OBJ_FAREAST_YINYANG) && iscarrby(OBJ_FAREAST_PIECE, mynum)) {
       bprintf("You step through a rip in the fabric of time.\n");
       trapch(LOC_FAREAST_SHORELINE);
    return;  }
break;
#endif

#ifdef LOCMIN_ABYSS
case LOC_ABYSS_CLOSET: 
    bprintf("You leap upward and grab hold of the shelf, pulling yourself "
            "up.\n");
    teletrap(LOC_ABYSS_SHELF);
    return;   
break;

case LOC_ABYSS_HEAD: 
case LOC_ABYSS_WICKET:
    if (!iswornby(OBJ_ABYSS_RING, mynum)) {
      quit_msg("You rapidly fall to your death.....S P L A T !!",
               "Fell Into an Abyss");
      quit_player(False);
    }    else    {
      bprintf("You float slowy downward.  Falling.. falling..\n");
      teletrap(LOC_ABYSS_BANK1);  }
    return;   
break;
#endif

#ifdef LOCMIN_TALON
case LOC_TALON_TALON25:  
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX, mynum, NOBODY,
              "%s makes a small jump off the edge of the cliff and disappears "
              "with a\nsmall popping noise.\n", pname(mynum));
    send_msg(sendloc(mynum), MODE_NODEAF, LVL_MIN, pvis(mynum)-1, mynum, NOBODY,
              "You hear a small popping noise.\n");
    bprintf ("You step off the edge of the cliff and feel a sudden change in "
             "pressure at\nabout the same time you realize you're back in "
             "the study.\n");
    trapch (LOC_TALON_TALON4);
    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX, mynum, NOBODY,
              "%s appears from nowhere, accompanied by a small popping "
              "noise.\n", pname(mynum));
    send_msg(sendloc(mynum), MODE_NODEAF, LVL_MIN, pvis(mynum)-1, mynum, NOBODY,
              "You hear a small popping noise.\n");
    return;   
break;
#endif


#ifdef LOCMIN_MITHDAN
case LOC_MITHDAN_MITHDAN40: 
    bprintf ("You leap over the sleeping dragon, and plunge into the fountain...\n");
    bprintf ("As you touch the water, you find yourself teleported!\n");
    send_msg(sendloc(mynum), 0,
              pvis (mynum) > 0 ? pvis (mynum) : LVL_MIN, LVL_MAX,
              mynum, NOBODY,
            "%s leaps over the dragon into the fountain, and dissapears!\n",
              pname (mynum));
    teletrap (LOC_MITHDAN_MITHDAN51);
    return;  
break;   
#endif

default:
   break;
}
