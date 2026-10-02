/* these things affect the chance of hitting something */

#if defined( LOCMIN_CASTLE ) && defined( LOCMIN_GROVE )
if (carries_obj_type (attacker, OBJ_CASTLE_HORSESHOE) > -1 ||
    carries_obj_type (attacker, OBJ_GROVE_GLOVE) > -1 )
   hitpct += 5;
#endif

#ifdef LOCMIN_CRYSTALCAVE
  if (carries_obj_type (attacker, OBJ_CRYSTALCAVE_SEASTONE) > -1)
    hitpct += 5;
#endif

#if defined( LOCMIN_PYRAMID ) && defined( LOCMIN_CRYSTALCAVE )
  if( attacker == max_players+MOB_PYRAMID_HORUS &&
    ( !iscarrby( OBJ_CRYSTALCAVE_EARTHSTONE, victim ) ||
      !iscarrby( OBJ_CRYSTALCAVE_SUNSTONE, victim ) ||
      !iscarrby( OBJ_CRYSTALCAVE_SEASTONE, victim ) ||
      !iscarrby( OBJ_CRYSTALCAVE_WINDSTONE, victim ) ) ) {
    hitpct += 25;
  }
#endif
