#include <unistd.h>
#include "kernel.h"
#include "locations.h"
#include "objects.h"
#include "mobiles.h"
#include "sflags.h"
#include "pflags.h"
#include "oflags.h"
#include "lflags.h"
#include "cflags.h"
#include "quests.h"
#include "sendsys.h"
#include "levels.h"
#include "commands.h"
#include "rooms.h"
#include "objsys.h"
#include "mobile.h"
#include "flags.h"
#include "bprintf.h"
#include "parse.h"
#include "uaf.h"
#include "clone.h"
#include "acct.h"
#include "mud.h"

extern char *MLevels[];
extern char *FLevels[];

/*
**  Digcom: replace database.c. By Vitastjern (Anna Eklund) 1993
*/
void digcom(void)
{
  int obj;

  obj = ((ob1 == -1) ? ob2 : ob1);    /* Some use dig <DIR> <WEAPON> */

#ifdef LOCMIN_RAINFOREST /* Mancini */
  if( ploc( mynum ) == LOC_RAINFOREST_CITY1 ) {
    if( obj == -1 ) {
      bprintf( "Dig with what?\n" );
      return;
    }
    if( obj == OBJ_RAINFOREST_MACHETE ) {
        if( iscarrby( OBJ_RAINFOREST_MACHETE, mynum ) ) {
      bprintf( "You hack through the branches blocking your way.\n" );
      send_msg (ploc(mynum), MODE_NOBLIND, pvis(mynum), LVL_MAX, mynum, NOBODY,
                "%s hacks through the branches blocking the exit to the west.\n",
                pname(mynum) );
      send_msg (ploc(mynum), MODE_NOBLIND, 0, pvis(mynum), mynum, NOBODY,
                "Someone hacks through the branches blocking the exit to the west.\n" );
      setobjstate( OBJ_RAINFOREST_BRANCHES, 0 );
    }
      else
        bprintf( "You do not have the %s!\n", oname(obj) );
      }
    else
      bprintf( "Your %s is not equeal to the task.\n", oname( obj ) );
    return;
  }

#endif /* LOCMIN_RAINFOREST */

#ifdef LOCMIN_EVOLUTION /* Mancini */
  if( ( ( ploc( mynum ) == LOC_EVOLUTION_ORDO3 ) ) &&
      ( oloc( OBJ_EVOLUTION_ASTRASPIS ) == LOC_EVOLUTION_ORDO3 ) &&
      ( odamage( OBJ_EVOLUTION_ASTRASPIS ) == 0 ) )
    {
    create( OBJ_EVOLUTION_ASTRASPIS );
    osetdamage( OBJ_EVOLUTION_ASTRASPIS, 1 );
    bprintf( "You find an astraspis buried here!\n" );
    return;
  }

  if( ( ( ploc( mynum ) == LOC_EVOLUTION_PERM6 ) ) &&
      ( oloc( OBJ_EVOLUTION_DIPLOCAULUS ) == LOC_EVOLUTION_PERM6 ) &&
      ( odamage( OBJ_EVOLUTION_DIPLOCAULUS ) == 0 ) )
    {
    create( OBJ_EVOLUTION_DIPLOCAULUS );
    osetdamage( OBJ_EVOLUTION_DIPLOCAULUS, 1 );
    bprintf( "You find a diplocaulus buried here!\n" );
    return;
  }

#endif /* LOCMIN_EVOLUTION */

#ifdef LOCMIN_ICECAVE
  if (ploc(mynum) == oloc(OBJ_ICECAVE_HOLE_SNOW_CAVE) ||
      ploc(mynum) == oloc(OBJ_ICECAVE_HOLE_G_HALL))
    {
    if (state(OBJ_ICECAVE_HOLE_G_HALL)==1)
      {
      if (iscarrby(obj, mynum) && otstbit(obj, OFL_WEAPON))
	{
        bprintf("You rapidly dig through to another passage.\n");
        setobjstate(OBJ_ICECAVE_HOLE_G_HALL, 0);
        return;
      }
      else
	{
        bprintf("The ice and snow is thick, you'll need something to "
                "hack through it with.\n");
        return;
      }
    }
    else
      {
      bprintf("You widen the hole but with little effect.");
      return;
    }
  }
#endif

#ifdef LOCMIN_LEDGE
  if ((ploc(mynum) == oloc(OBJ_LEDGE_TUNNWEST) ||
       ploc(mynum) == oloc(OBJ_LEDGE_TUNNEAST)) &&
      state(OBJ_LEDGE_TUNNEAST) == 1)
    {
    bprintf("You dig your way through the rockfall, and soon clear "
            "the passage.\n");
    setobjstate(OBJ_LEDGE_TUNNEAST, 0);
    return;
  }
#endif
#ifdef LOCMIN_ABYSS
  if ((ploc(mynum) == oloc(OBJ_ABYSS_BONESSTAIRS) ||
       ploc(mynum) == oloc(OBJ_ABYSS_PLAINSTAIRS)) &&
      state(OBJ_ABYSS_PLAINSTAIRS) == 1)
    {
    bprintf("You dig your way through the rockfall, and soon clear "
            "the passage.\n");
    setobjstate(OBJ_ABYSS_PLAINSTAIRS, 0);
    return;
  }
#endif

#ifdef LOCMIN_CHURCH
  if (ploc(mynum) == oloc(OBJ_CHURCH_SLAB_TOP) &&
      otstbit(OBJ_CHURCH_SLAB_TOP, OFL_DESTROYED))
    {
    bprintf("You uncover a stone slab!\n");
    oclrbit(OBJ_CHURCH_SLAB_TOP, OFL_DESTROYED);
    return;
  }
#endif
#ifdef LOCMIN_OAKTREE
  if (ploc(mynum) == oloc(OBJ_OAKTREE_TOPFOXHOLE) &&
      state(OBJ_OAKTREE_TOPFOXHOLE) == 1)
    {
    bprintf("Although there are fox tracks in the grass, you can't "
            "find the fox's hole.\n");
    return;
  }
#endif
#ifdef LOCMIN_OCEAN
  if (ploc(mynum) == oloc(OBJ_OCEAN_TOP_FAULT) &&
      otstbit(OBJ_OCEAN_TOP_FAULT, OFL_DESTROYED))
    {
    bprintf("You uncover a fault in the ocean floor!\n");
    oclrbit(OBJ_OCEAN_TOP_FAULT, OFL_DESTROYED);
    setobjstate(OBJ_OCEAN_TOP_FAULT, 0);
    return;
  }
#endif

#ifdef LOCMIN_NIBELUNG
  if (((ploc(mynum) == oloc(OBJ_NIBELUNG_AZURITE)) == LOC_NIBELUNG_NIBELUNG5)
      && (otstbit(OBJ_NIBELUNG_AZURITE, OFL_DESTROYED)))
    {
    bprintf("You dig up the azurite\n");
    oclrbit(OBJ_NIBELUNG_AZURITE, OFL_DESTROYED);
    return;
  }
#endif

  bprintf("You find nothing.\n");

  return;
}
