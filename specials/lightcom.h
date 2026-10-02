switch (a) {

#ifdef LOCMIN_CHLYON
case OBJ_CHLYON_FORGE:
   if (iscontin(OBJ_CHLYON_STONE_FIRESTONE, OBJ_CHLYON_FORGE) &&
	iscontin(OBJ_CAVE_CLAYMORE, OBJ_CHLYON_FORGE)) {
     bprintf("The firestone and the claymore melt together.\n"
"Moments later, the forge is engulfed in a blue force field and\n"
"the fire goes out...\n");
     setoloc(OBJ_CHLYON_FIREBLADE, OBJ_CHLYON_FORGE, IN_CONTAINER);
     destroy(OBJ_CHLYON_STONE_FIRESTONE);
     destroy(OBJ_CAVE_CLAYMORE);
     set_quest(mynum, Q_CHLYON);
} else { bprintf("The forge doesn't ignite.\n");
  }  return;
 break;
#endif

default:  break;
}

#ifdef LOCMIN_PIRATE
  if (a == OBJ_PIRATE_KEG) {
    if (ocarrf(a) >= CARRIED_BY)
      {
       bprintf( "As you light the keg, you belatedly realize that you are"
                "still holding it.\n" );
       bprintf( "When the keg explodes, a piece of wood is driven deep within "
                "your heart.\n" );
       send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX,
                mynum, NOBODY, "As %s lights the keg, %s belatedly realizes "
                "that %s are still holding\n"
                "it.  When the keg explodes, a piece of wood is driven deep "
                "within %s heart!\n",
                pname(mynum), psex(mynum) ? "she" : "he", psex(mynum) ? "she" :
                "he", psex(mynum) ? "her" : "his" );
       quit_player(False);
       broad( "You hear an explosion in the distance followed by a muffled scream.\n" );
       quit_msg("Sorry, you seem to have died", "Impaled by an Exploding Keg");
       destroy(OBJ_PIRATE_KEG);
       return;
     }

    send_msg(sendloc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX, mynum, NOBODY,
              "%s lights the keg.\n", pname(mynum));

    if (oloc(a) == LOC_PIRATE_PIRATE30) /* blow up boulder */
      {
      send_msg(int2idx(oloc(a), LOC), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
        "The explosion shatters the boulder!\n");
      setobjstate(OBJ_PIRATE_BOULDER1, 0);
    }
    destroy(OBJ_PIRATE_KEG);
    bprintf("You are hurt by the flying pieces of debris!\n");
    if (pstr(mynum) <= 10) /* Do 10 points damage at most but do NOT kill */
      {
      setpstr(mynum, 1);
    }
    else
      {
      setpstr(mynum, (pstr(mynum) - 10));
    }
    return;
  }
#endif /* PIRATE ZONE */

#ifdef LOCMIN_CATHEDRAL
  if (a == OBJ_CATHEDRAL_SARCOPHAGUS) {
    if (!otstbit(OBJ_CATHEDRAL_SARCOPHAGUS, OFL_LIT)) {
      bprintf("You set the sarcophagus a fire!\n");
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
               "\001p%s\003 sets the sarcophagus a blaze!\n", pname(mynum));
      send_msg(int2idx(LOC_CATHEDRAL_CRYPTENT, LOC), 0, LVL_MIN, LVL_MAX,
               NOBODY, NOBODY,
               "The door to the west magically opens.\n");
      osetbit(OBJ_CATHEDRAL_SARCOPHAGUS, OFL_LIT);
      setobjstate(OBJ_CATHEDRAL_SARCOPHAGUS, 0);
      setobjstate(OBJ_CATHEDRAL_DOOR_CRYPT, 0);
      return;
    } else { 
      bprintf("The sarcophagus is already lit!\n");
      return;
    }
  }
#endif
