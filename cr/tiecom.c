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

void tiecom(void) {
  if (brkword() == -1) {
    bprintf("Tie what?\n");
    return;
  }

  switch (ob1) {
    case OBJ_LIMBO_ROPE:
      switch (ploc(mynum)) {
#if defined(LOCMIN_TREEHOUSE) && defined(LOCMIN_VALLEY)
        case LOC_TREEHOUSE_PORCH:
        case LOC_VALLEY_ESIDE:
          if (state(OBJ_VALLEY_LADDER2ELF) == 0) {
            bprintf("Why? There is already a rope there!\n");
            break;
          }
          setobjstate(OBJ_TREEHOUSE_LADDER, 0);
          destroy(OBJ_LIMBO_ROPE);
          bprintf("You tie the rope to the tree.\n");
          break;
#endif
#if defined(LOCMIN_LEDGE) && defined(LOCMIN_MOOR)
        case LOC_LEDGE_PIT:
        case LOC_MOOR_PIT:
          if (state(OBJ_LEDGE_ROPEBOTPIT) == 0)
	    {
            bprintf("Why? There is already a rope there!\n");
            break;
          }
          oclrbit(OBJ_LEDGE_ROPEBOTPIT, OFL_DESTROYED);
          oclrbit(OBJ_MOOR_ROPETOPPIT, OFL_DESTROYED);
          setobjstate(OBJ_LEDGE_ROPEBOTPIT, 0);
          destroy(OBJ_LIMBO_ROPE);
          bprintf("You tie the rope to the pit.\n");
          break;
#endif

#ifdef LOCMIN_ABYSS
        case LOC_ABYSS_L664:
        case LOC_ABYSS_L665:
          if (state(OBJ_ABYSS_L664ROPE) == 0)
	    {
            bprintf("Why? There is already a rope there!\n");
            break;
          }
          setobjstate(OBJ_ABYSS_L664ROPE, 0);
          destroy(OBJ_LIMBO_ROPE);
          bprintf("You tie the rope over the gap.\n");
          break;
#endif

        default:
          bprintf("You have nothing to tie the rope to.\n");
          break;
	}
      break;

#ifdef LOCMIN_ABYSS
    case OBJ_ABYSS_ROPE:
      switch (ploc(mynum))
	{
#if defined(LOCMIN_TREEHOUSE) && defined(LOCMIN_VALLEY)
        case LOC_TREEHOUSE_PORCH:
        case LOC_VALLEY_ESIDE:
          if (state(OBJ_VALLEY_LADDER2ELF) == 0)
	    {
            bprintf("Why? There is already a rope there!\n");
            break;
          }
          setobjstate(OBJ_TREEHOUSE_LADDER, 0);
          destroy(OBJ_ABYSS_ROPE);
          bprintf("You tie the rope to the tree.\n");
          break;
#endif
#if defined(LOCMIN_LEDGE) && defined(LOCMIN_MOOR)
        case LOC_LEDGE_PIT:
        case LOC_MOOR_PIT:
          if (state(OBJ_LEDGE_ROPEBOTPIT) == 0)
	    {
            bprintf("Why? There is already a rope there!\n");
            break;
          }
          setobjstate(OBJ_LEDGE_ROPEBOTPIT, 0);
          destroy(OBJ_ABYSS_ROPE);
          bprintf("You tie the rope to the pit.\n");
          break;
#endif
        case LOC_ABYSS_L664:
        case LOC_ABYSS_L665:
          if (state(OBJ_ABYSS_L664ROPE) == 0)
	    {
            bprintf("Why? There is already a rope there!\n");
            break;
          }
          setobjstate(OBJ_ABYSS_L664ROPE, 0);
          destroy(OBJ_ABYSS_ROPE);
          bprintf("You tie the rope over the gap.\n");
          break;

        default:
          bprintf("You have nothing to tie the rope to.\n");
          break;
	}
      break;
#endif

    default:
      bprintf("You cannot tie that!\n");
      break;
    }
  return;
}
