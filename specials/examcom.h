#include "mflags.h"

#ifdef LOCMIN_PYRAMID
  case OBJ_PYRAMID_HIEROGLYPHICS:
    if( !odamage( OBJ_PYRAMID_ROSETTA ) ) {
      bprintf( "You can not understand the hieroglyphics.\n" );
      return;
    }
    break;

  case OBJ_PYRAMID_ROSETTA:
    if( !odamage( OBJ_PYRAMID_ROSETTA ) ) {
      bprintf( "You feel you are now able to translate complex hieroglyphics "
               "with this stone.\n" );
      osetdamage( OBJ_PYRAMID_ROSETTA, 1 );
      return;
    }
    break;
#endif

#ifdef LOCMIN_CHLYON
    case OBJ_CHLYON_DESK:
	if (find_stuff (OBJ_CHLYON_DESK, OBJ_CHLYON_BOOK_DIARY,
		"You find a diary on one of the drawers.\n"))
		return;
	break;
#endif

#ifdef LOCMIN_EVOLUTION
 case OBJ_EVOLUTION_LEAVES:
  if( find_stuff( OBJ_EVOLUTION_LEAVES, OBJ_EVOLUTION_GLEVOSAURUS,
		 "Within the leaves, you find a glevosaurus!\n" ) )
    return;
  break;
  
 case OBJ_EVOLUTION_TREE:
  if( find_stuff( OBJ_EVOLUTION_TREE, OBJ_EVOLUTION_ICARONYCTERIS,
		 "Peering into the tree, you find a icaronycteris!\n" ) )
    return;
  break;
  
#endif
  
#ifdef LOCMIN_ABYSS
 case OBJ_ABYSS_HUSKS:
  if (find_stuff(OBJ_ABYSS_HUSKS, OBJ_ABYSS_RING,
		 "Under one of the husks you find a ring.\n"))
    return;
  break;
  
 case OBJ_ABYSS_MAT:
  if (find_stuff(OBJ_ABYSS_MAT, OBJ_ABYSS_CARD2,
		 "Under the mat you find a plastic card.\n"))
    return;
  break;
#endif
  
#ifdef LOCMIN_GROVE
 case OBJ_GROVE_PIPETMAN:
  if(state(OBJ_GROVE_PIPETMAN)) {
      bprintf("It looks like there has been some kind of fluid in it.\n");
      return;
    }
  else
    {
      bprintf("The end is full of an odd fluid, and you wonder what from.\n");
      return;
    }
  break;
  
 case OBJ_GROVE_BEAKER:
  if(state(OBJ_GROVE_PIPETMAN))
    {
      bprintf("The beaker is filled with a sickeningly pink fluid.\n");
      return;
    }
  else
    {
      bprintf("It is empty. Seems like something could be added to it.\n");
      return;
    }
  break;
  
#endif
  
#ifdef LOCMIN_ARENA
 case OBJ_ARENA_CRACK:
  if ((state(OBJ_ARENA_CRACK) == 0) &&
      (alive((max_players + MOB_ARENA_CRAWLER) != -1)))
    {
      bprintf("As you peer down into the hole, a worm like beast lunges out "
              "at you attacking\nwith 8 long tentacles");
      sillycom("\001s%s\003A worm like creature lunges from the hole "
               "onto %s!\n\003");
      setploc((max_players + MOB_ARENA_CRAWLER), ploc(mynum));
      hit_player(max_players + MOB_ARENA_CRAWLER ,mynum,-1);
      setobjstate (OBJ_ARENA_CRACK, 1);
      return;
    }
  break;
#endif  

#ifdef LOCMIN_NIBELUNG
 case OBJ_NIBELUNG_COFFIN:
  if ((state(a) == 0) && (alive(max_players+MOB_NIBELUNG_VAMPIRE) != -1))
    {
      char text[100];
      bprintf ("A Vampire leaps out os the coffin onto you!\n");
      sprintf (text,"A Vampire leaps out of the coffin onto %s!\n",
               pname(mynum));
      sillycom (text);
      setploc(max_players+MOB_NIBELUNG_VAMPIRE, ploc(mynum));
      hit_player (max_players+MOB_NIBELUNG_VAMPIRE, mynum, -1);
      return;
    }
  break;
 case OBJ_NIBELUNG_FIRE:
  if (state(a) == 0 && iscarrby(OBJ_NIBELUNG_FIRESTONE, mynum)) {
    setobjstate (a, randperc() % 3 + 1);
    switch (state(a)) {
    case 1:
      bprintf ("The fire appears a muddy brown through the firestone.\n");
      break;
    case 2:
      bprintf ("The fire burns with a purple incandescence.\n");
      break;
    case 3:
      bprintf ("A mauve haze tints the edges of the flames.\n");
      break;
    }
    return;
  }
  break;
#endif
  
#ifdef LOCMIN_TALON
 case OBJ_TALON_DESK:
  if (otstbit(OBJ_TALON_RUBY, OFL_DESTROYED) &&
      oloc(OBJ_TALON_RUBY) == LOC_TALON_TALON4) {
    create (OBJ_TALON_RUBY);
    bprintf ("You find a large, fiery ruby in one of the drawers.\n");
    send_msg (sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
	      mynum, NOBODY, "%s rummages around in the desk and turns up "
	      "a large, fiery ruby.\n", pname(mynum));
    return;
  }
  break;
#endif

