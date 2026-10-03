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

void throwcom(void) {
  if (brkword() == -1) {
    bprintf("Throw what?\n");
    return;
  }
  else if (ob1 == -1) {
    bprintf("You cant throw that!\n");
    return;
  }
  else if (!iscarrby(ob1, mynum))
    {
    bprintf("You don't have it!\n");
    return;
  }
#ifdef LOCMIN_FAREAST
  if (ob1 == OBJ_FAREAST_KAGINAWA) {
    if (ploc(mynum) == LOC_FAREAST_ENOGATE) {
      bprintf("You throw the kaginawa over the wall and climb over the gate.\n");
      trapch(LOC_FAREAST_COURTYARD);
      return;
    }
    else if (ploc(mynum) == LOC_FAREAST_COURTYARD) {
      bprintf("You throw the kaginawa back over the city walls and climb over them.\n");
      trapch(LOC_FAREAST_ENOGATE);
      return;
    }
  }
#endif
  if (ob1 == OBJ_LIMBO_ROPE 
    #ifdef LOCMIN_ABYSS
     || ob1 == OBJ_ABYSS_ROPE
    #endif
    ) {

#ifdef LOCMIN_LEDGE
    if (ploc(mynum) == LOC_LEDGE_ON) {
      bprintf("The rope fails to catch on the ledge.\n");
      return;
    }
#endif
#ifdef LOCMIN_VALLEY
    if (ploc(mynum) == LOC_VALLEY_STEPS)
      {
      if (state(OBJ_VALLEY_ROPEEAST) == 1)
	{
        bprintf("The rope catches on the other side of the ledge "
                "and seems secure.\n");
        destroy(ob1);
        oclrbit(OBJ_VALLEY_ROPEEAST, OFL_DESTROYED);
        oclrbit(OBJ_LEDGE_ROPEWEST, OFL_DESTROYED);
        setobjstate(OBJ_VALLEY_ROPEEAST, 0);
        return;
      }
      else
	{
        bprintf("Why do you want to do that for? There is already "
                "a rope here.\n");
        return;
      }
    }
#endif
    if (0  
#ifdef LOCMIN_LEDGE
      || ploc(mynum) == LOC_LEDGE_PIT
#endif
#ifdef LOCMIN_MOOR
      || ploc(mynum) == LOC_MOOR_PIT
#endif
      ) {
#if defined(LOCMIN_LEDGE) && defined(LOCMIN_MOOR)
      if (state(OBJ_LEDGE_ROPEBOTPIT) == 1)
	{
        if (ploc(mynum) == LOC_LEDGE_PIT)
          bprintf("The rope catches high on the lip of the pit.\n");
        else
          bprintf("The rope now leads down the pit.\n");
        destroy(ob1);
        oclrbit(OBJ_LEDGE_ROPEBOTPIT, OFL_DESTROYED);
        oclrbit(OBJ_MOOR_ROPETOPPIT, OFL_DESTROYED);
        setobjstate(OBJ_LEDGE_ROPEBOTPIT, 0);
        return;
      }
#endif
      bprintf("Why do you want to do that for? There is already "
                "a rope here.\n");
        return;
    }
  }
#ifdef LOCMIN_NIBELUNG
  if (ob1 == OBJ_NIBELUNG_ARROW ||
      ob1 == OBJ_NIBELUNG_ARROW2 ||
      ob1 == OBJ_NIBELUNG_ARROW3) {
    if ((ob1 == OBJ_NIBELUNG_ARROW && state(OBJ_NIBELUNG_FIRE) == 1) ||
        (ob1 == OBJ_NIBELUNG_ARROW2 && state(OBJ_NIBELUNG_FIRE) == 2) ||
        (ob1 == OBJ_NIBELUNG_ARROW3 && state(OBJ_NIBELUNG_FIRE) == 3)) {
      bprintf ("The arrow arcs cleanly through the air and into the hole.\n");
      send_msg (sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
                "%s throws an arrow right through the hole on the "
                "south wall.\n", pname(mynum));
      send_msg (sendloc(mynum), 0, LVL_MIN, pvis(mynum)-1, mynum, NOBODY,
                "An arrow suddenly materializes in the room and flies right "
                "through the hole\non the south wall.\n");
      send_msg (sendloc(mynum), 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
                "The entire south wall slides to one side, revealing beyond "
                "it a large\ncircular room.\n");
      setoloc (ob1, LOC_NIBELUNG_NIBELUNG68, IN_ROOM);
      setobjstate (OBJ_NIBELUNG_HOLE, 0);
    } 
    else {
      bprintf ("The arrow lands against the wall and falls to the floor.\n");
      send_msg (sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
                "%s throws an arrow against the south wall.\n", pname(mynum));
      send_msg (sendloc(mynum), 0, LVL_MIN, pvis(mynum)-1, mynum, NOBODY,
                "An arrow suddenly materializes in the room and flies into "
                "the south wall.\n");
      setoloc (ob1, LOC_NIBELUNG_NIBELUNG67, IN_ROOM);
    }
    return;
  }
#endif
  bprintf("You can't throw that!\n");
}
