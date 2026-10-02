#ifdef LOCMIN_TOWER
  if (ploc (mynum) == LOC_TOWER_TREASURE && state(OBJ_TOWER_DOOR_TREASURE) == 0
      && ishere (a)) {
    setobjstate (OBJ_TOWER_DOOR_TREASURE, 1);
    strcpy (ar, "The door clicks shut....\n");
    sendl(LOC_TOWER_TREASURE, ar);
    sendl(oloc(OBJ_TOWER_DOOR_SHAZARETH), ar);
  }
#endif

#ifdef LOCMIN_EVOLUTION
  if(c == OBJ_EVOLUTION_SHARKJAW ) {
    int b;
    switch( a )
      {
      default: break;
      case OBJ_EVOLUTION_SHARKTOOTH1:
      case OBJ_EVOLUTION_SHARKTOOTH2:
      case OBJ_EVOLUTION_SHARKTOOTH3:
      case OBJ_EVOLUTION_SHARKTOOTH4:
      case OBJ_EVOLUTION_SHARKTOOTH5:
      case OBJ_EVOLUTION_SHARKTOOTH6:
      case OBJ_EVOLUTION_SHARKTOOTH7:
      case OBJ_EVOLUTION_SHARKTOOTH8:
      if( ( ( iscontin( OBJ_EVOLUTION_SHARKTOOTH1, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH2, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH3, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH4, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH5, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH6, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH7, OBJ_EVOLUTION_SHARKJAW ) == True ) &&
            ( iscontin( OBJ_EVOLUTION_SHARKTOOTH8, OBJ_EVOLUTION_SHARKJAW ) == True ) ) )
        {
        /* Empty it first.  :)  */
        for (b = 0; b < numobs; b++)
	  {
            if (iscontin(b, a))
              destroy(b);
	  }
        osetdamage( OBJ_EVOLUTION_SHARKJAW, 30 );
        oclrbit( OBJ_EVOLUTION_SHARKJAW, OFL_CONTAINER );
        bprintf( "As you fit the last tooth into the %s, you realize that you now have\n"
                 "a formidable weapon indeed!\n\n", oname( c ) );
      }
      return;
      }
  }

#endif
