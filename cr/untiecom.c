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

extern char *WizLevels[];
extern char *MLevels[];
extern char *FLevels[];

/*
**  get unused rope from limbo (preferable) (remove destroyed flag if needed)
**  place this rope in the location of mynum, set state 'linked rope'
**  to 1 (closed door)
**  Code thanks to Vitastjern
*/
void untiecom(void)
{
  if (brkword() == -1) {
    bprintf("Untie what?\n");
    return;
  }
  else if (ploc(mynum) != oloc(ob1))
    {
    bprintf("It isn't here.\n");
    return;
  }
  switch (ob1)
    {
#if defined(LOCMIN_VALLEY) && defined(LOCMIN_TREEHOUSE)
    case OBJ_VALLEY_LADDER2ELF:
    case OBJ_TREEHOUSE_LADDER:
      if ((alive(MOB_TREEHOUSE_ELF+max_players) != -1) &&
          (state(OBJ_VALLEY_LADDER2ELF) == 0))
	{
        send_msg(sendloc(mynum),MODE_NODEAF,LVL_MIN,LVL_MAX,NOBODY,NOBODY,
            "The Elf yells 'Go away and leave my rope ladder alone'\n");
        return;
      }
      if (state(OBJ_VALLEY_LADDER2ELF) == 1)
	{
        sendf(mynum, "You cannot untie that!\n");
        break;
      }
      if (oloc(OBJ_LIMBO_ROPE) == LOC_DEAD_DESTROYED ||
          oloc(OBJ_LIMBO_ROPE) == LOC_LIMBO_LIMBO)
	{
        create(OBJ_LIMBO_ROPE);
        setoloc(OBJ_LIMBO_ROPE, ploc(mynum), IN_ROOM);
      }
#endif
#ifdef LOCMIN_ABYSS
      else
	{
        create(OBJ_ABYSS_ROPE);
        setoloc(OBJ_ABYSS_ROPE, ploc(mynum), IN_ROOM);
      }
#endif
#ifdef LOCMIN_VALLEY
      setobjstate(OBJ_VALLEY_LADDER2ELF, 1);
      bprintf("You untie the rope.\n");
      break;
#endif

#if defined(LOCMIN_LEDGE) && defined(LOCMIN_VALLEY)
    case OBJ_LEDGE_ROPEWEST:
    case OBJ_VALLEY_ROPEEAST:
      if (state(OBJ_VALLEY_ROPEEAST) == 1)
	{
        sendf(mynum, "You cannot untie that!\n");
        break;
      }
      if (oloc(OBJ_LIMBO_ROPE) == LOC_DEAD_DESTROYED ||
          oloc(OBJ_LIMBO_ROPE) == LOC_LIMBO_LIMBO)
	{
        create(OBJ_LIMBO_ROPE);
        setoloc(OBJ_LIMBO_ROPE, ploc(mynum), IN_ROOM);
        osetbit(OBJ_LEDGE_ROPEWEST, OFL_DESTROYED);
        osetbit(OBJ_VALLEY_ROPEEAST, OFL_DESTROYED);
      }
#endif
#ifdef LOCMIN_ABYSS
      else
	{
        create(OBJ_ABYSS_ROPE);
        setoloc(OBJ_ABYSS_ROPE, ploc(mynum), IN_ROOM);
      }
#endif
#ifdef LOCMIN_VALLEY
      setobjstate(OBJ_VALLEY_ROPEEAST, 1);
      bprintf("You untie the rope.\n");
      break;
#endif

#if defined(LOCMIN_MOOR) && defined(LOCMIN_LEDGE) && defined(LOCMIN_DEAD)
    case OBJ_MOOR_ROPETOPPIT:
    case OBJ_LEDGE_ROPEBOTPIT:
      if (state(OBJ_MOOR_ROPETOPPIT) == 1)
	{
        sendf(mynum, "You cannot untie that!\n");
        break;
      }
      if (oloc(OBJ_LIMBO_ROPE) == LOC_DEAD_DESTROYED ||
          oloc(OBJ_LIMBO_ROPE) == LOC_LIMBO_LIMBO)
	{
        create(OBJ_LIMBO_ROPE);
        setoloc(OBJ_LIMBO_ROPE, ploc(mynum), IN_ROOM);
      }
#endif
#ifdef LOCMIN_ABYSS
      else
	{
        create(OBJ_ABYSS_ROPE);
        setoloc(OBJ_ABYSS_ROPE, ploc(mynum), IN_ROOM);
      }
#endif
#if defined(LOCMIN_LEDGE) && defined(LOCMIN_MOOR)
      osetbit(OBJ_LEDGE_ROPEBOTPIT, OFL_DESTROYED);
      osetbit(OBJ_MOOR_ROPETOPPIT, OFL_DESTROYED);
      setobjstate(OBJ_MOOR_ROPETOPPIT, 1);
      bprintf("You untie the rope.\n");
      break;
#endif

#if defined(LOCMIN_ABYSS) && defined(LOCMIN_DEAD)
    case OBJ_ABYSS_L664ROPE:
    case OBJ_ABYSS_L665ROPE:
      if (state(OBJ_ABYSS_L664ROPE) == 1)
	{
        sendf(mynum, "You cannot untie that!\n");
        break;
      }
      if (oloc(OBJ_LIMBO_ROPE) == LOC_DEAD_DESTROYED ||
          oloc(OBJ_LIMBO_ROPE) == LOC_LIMBO_LIMBO)
	{
        create(OBJ_LIMBO_ROPE);
        setoloc(OBJ_LIMBO_ROPE, ploc(mynum), IN_ROOM);
      }
      else
	{
        create(OBJ_ABYSS_ROPE);
        setoloc(OBJ_ABYSS_ROPE, ploc(mynum), IN_ROOM);
      }
      setobjstate(OBJ_ABYSS_L664ROPE, 1);
      bprintf("You untie the rope.\n");
      break;
#endif

    default:
      sendf(mynum, "You can't untie that!\n");
      break;
    }
  return;
}
