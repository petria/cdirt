/* these are adjustments to damage.. ie, a hit has already been made */

#ifdef LOCMIN_TALON 
  if (attacker == max_players+MOB_TALON_TALON &&
      iswornby(OBJ_TALON_TALONSHIELD, victim))
    *damage -= 45;               /* natural damage goes from 50 to 5 */
#endif

#if defined( LOCMIN_PYRAMID ) && defined( LOCMIN_CRYSTALCAVE )
  if( attacker == max_players+MOB_PYRAMID_HORUS &&
    ( !iscarrby( OBJ_CRYSTALCAVE_EARTHSTONE, victim ) ||
      !iscarrby( OBJ_CRYSTALCAVE_SUNSTONE, victim ) ||
      !iscarrby( OBJ_CRYSTALCAVE_SEASTONE, victim ) ||
      !iscarrby( OBJ_CRYSTALCAVE_WINDSTONE, victim ) ) ) {
    *damage += 35;
  }
#endif

#ifdef LOCMIN_ARENA                               
  if (attacker == max_players+MOB_ARENA_YMOLD) {
    if (randperc() < 50) {
      bprintf("The mold does nothing.\n");
      *missed = True;
      return(hitloc);
    }
    send_msg(sendloc(attacker), 0, LVL_MIN, LVL_MAX, attacker, NOBODY,
             "The yellow mold releases a cloud of spore into the air\n");
    for(i = 0; i < max_players; i++) {
      if ((ploc(max_players + MOB_ARENA_YMOLD) == ploc(i)) &&
          (plev(i) < LVL_WIZARD) && is_in_game(i)) {
        if (randperc() < 30) {
          sendf(i, 
 "You take in a deep breath just at the wrong moment, the cloud of spores\n"
 "surrounds you. Suddenly you begin to feel short of breath, but can't seem\n"
 "to force air into your lungs. You drop your weapon, and other belongings and"
 "\nclutch your throat as you gasp for air, turning slowly blue, you sink to\n"
 "your knees, helpless.\n\n");
          quit_msg("Oh dear, you seemed to have chocked to death", 
		   "Choked by Poisonous Spores");
          quit_player(False);
        }
        else
	sendf(i,"You manage to avoid the deadly chocking spores this time.\n");
      }
    }
  }
  else if (victim == max_players + MOB_ARENA_YMOLD) {
    if ((w < 0) || (!otstbit(w, OFL_LIT))) {
      bprintf("Your weapon seems to have no effect on the mold\n");
      *missed = True;
      return(hitloc);
    }
  }
#endif
