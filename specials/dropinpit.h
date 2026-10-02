#ifdef LOCMIN_WASTE
  if (o == OBJ_WASTE_THRONE) {
    if (alive (i = max_players + MOB_WASTE_DJINNI) == -1 &&
        pscore (i) == mynum) {
      set_quest (mynum, Q_FIERY_KING);
    }
  }
#endif

#ifdef LOCMIN_TOWER
  if (o == OBJ_TOWER_CROWN) {
    if (alive (i = max_players + MOB_TOWER_SHAZARETH) == -1 &&
        pscore (i) == mynum) {
      set_quest (mynum, Q_TOWER);
    }
  }
#endif
#ifdef LOCMIN_MITHDAN
  if (o == OBJ_MITHDAN_CRYSTAL)
    set_quest (mynum, Q_MITHDAN);
#endif

#ifdef LOCMIN_TALON
  if (o == OBJ_TALON_CLAW) {
    if (alive( i = max_players + MOB_TALON_TALON ) == -1 &&
        pscore(i) == mynum )
      set_quest ( mynum, Q_TALON );
  }
#endif

#ifdef LOCMIN_RAINFOREST
  if (o == OBJ_RAINFOREST_SKULL)
    set_quest(mynum, Q_RAINFOREST );
#endif

#ifdef LOCMIN_ORCHOLD
  if (o == OBJ_ORCHOLD_BLOCK)
    set_quest(mynum, Q_ORCHOLD);
#endif

#ifdef LOCMIN_FROBOZZ
  if (o == OBJ_FROBOZZ_ORIGINAL_VAULT)
    set_quest(mynum, Q_FIND_PAINTING);
#endif

