switch (x) {
#ifdef LOCMIN_VILLAGE
  case OBJ_VILLAGE_PEBBLE:
    bprintf ("The pebble gets annoyed and goes to philosophize elsewhere.\n");
    setoloc (OBJ_VILLAGE_PEBBLE, LOC_DEAD_DESTROYED, IN_ROOM);
    return;
#endif
#ifdef LOCMIN_BLIZZARD
  case OBJ_BLIZZARD_RESET_STONE:
    sys_reset ();
    break;
#endif
#ifdef LOCMIN_QUARRY
  case OBJ_QUARRY_ROCK:
    bprintf ("You smash it apart to reveal a gem inside.\n");
    create (OBJ_QUARRY_GEM);
    setoloc (OBJ_QUARRY_GEM, oloc (OBJ_QUARRY_ROCK), ocarrf(OBJ_QUARRY_ROCK));
    destroy (OBJ_QUARRY_ROCK);
    break;
#endif
  case -1:
    bprintf ("What's that?\n");
    break;
  default:
    bprintf ("You can't do that.\n");
    break;
}
