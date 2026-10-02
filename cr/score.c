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
#include "mud.h"
#include "xrscore.h"
#include "smess.h"
#include "fight.h"

#define LINE "&+B+&+b-----------------------------------------" \
               "------------------------------------&+B+\n"

extern char *MWizLevels[];
extern char *FWizLevels[];
extern char *MLevels[];
extern char *FLevels[];

void pmess( int plx, Boolean self ) {
  int p;
  char b[150];
  int is_mobile = plx >= max_players;

  if (plev(plx) >= LVL_WIZARD) p = 12;
  else if (pstr(plx) <= 0) p = 0;
  else {
    if (is_mobile)   p = (int) (pstr(plx) * 10) / pstr_reset (plx);
    else             p = (int) (pstr(plx) * 10) / maxstrength (plx);
    if (p >= 11)     p = 11;
    else p++;
  }
  if (self == True)    sprintf(b, smess[p]);
  else                 sprintf(b, omess[p], pname(plx));

  if (plev(plx) < LVL_WIZARD)
    colorsel(pstr(plx), is_mobile? pstr_reset (plx) : maxstrength(plx), b);

  bprintf("&+Wo &N%s\n", b);

  if (!is_mobile) {
    if (plev(plx) >= LVL_WIZARD)
      p = 5;
    else if (player_damage(plx) <= 8) p = 0;
    else {
      p = (int) ((player_damage(plx) - 8) * 3) / (MAX_WEAPON_DAM - 8);
      if (p >= 4) p = 4;
      else p++;
    }
    if (self == True)
      sprintf(b, sweapon[p]);
    else
      sprintf(b, oweapon[p], pname(plx));
    colorsel(player_damage(plx) - 8, MAX_WEAPON_DAM - 8, b);
    bprintf("&+Wo &N%s\n", b);
  }

  /* armor */
  if (!is_mobile) {
    if (plev(plx) >= LVL_WIZARD) p = 6;
    else if (pac(plx) < 0) p = 0;
    else {
      p = (int) (pac(plx) * 4 / MAXARMOR);
      if (p > 5) p = 5;
      else p++;
  }

    if (self == True)  sprintf(b, sarmors[p]);
    else               sprintf(b, oarmors[p], pname(plx));

    colorsel(pac(plx), MAXARMOR, b);
    bprintf("&+Wo &N%s\n\n", b);
  }
}

void xrscorecom(void) {
  int plx=mynum, brk;
  char level[30];
  int is_mobile;

  if ((brk = brkword()) != -1) {
    if (plev(plx) >= LVL_WIZARD) {
      if ((plx = fmbn(item1)) < 0) {
        bprintf ("No one with that name is playing.\n");
        return;
      }
    }
    else {
      bprintf("You can't do that\n");
      return;
    }
  }
  is_mobile = plx >= max_players;

  bprintf(LINE);
  bprintf("&+b| &+W%-10s&+w: &+B%-14s", "Name", pname(plx));

  if (is_mobile)
    strcpy(level, "Mobile");
  else if (plev(plx))
    strcpy(level, player_level(plx));
  else
    strcpy(level, "Guest");

  colorsel(plev(plx), LVL_WIZARD, level);
  bprintf("&+W%-10s&+w: %-20s", "Level", level);

  if (!is_mobile && plev(plx) < LVL_WIZARD)
    bprintf("&+W%-10s&+w: &+c%-11d &+b|\n", "Score", pscore(plx));
  else
    bprintf("&+W%-10s&+w: &+c%-11s &+b|\n", "Score", "-------");

  if (plev(plx) < LVL_WIZARD)
    bprintf("&+b| &+W%-10s&+w:&+y%3d&+W/&+y%-11d", "Strength", pstr(plx),
            (is_mobile) ? pstr_reset (plx) : maxstrength (plx));
  else
    bprintf("&+b| &+W%-10s&+w: &+y%-3s&+W/&+y%-10s", "Strength",
            "---", "---");

  if (!is_mobile && plev(plx) < LVL_WIZARD && 
      (pclass(plx) == MAGE || pclass(plx) == PRIEST))
    bprintf("&+W%-10s&+w: &+m%-2d/%-11d", "Mana", pmagic(plx),
            maxmagic(plx));
  else
    bprintf("&+W%-10s&+w: &+m%-2s/%-10s", "Mana", "---", "---");

  if (!is_mobile) {
    bprintf("&+W%-10s&+w: &+g%-11d &+b|\n", "Kills", pkilled(plx));
    bprintf("&+b| &+W%-10s&+w: &+r%-14d", "Deaths", pdied(plx));

    if (pwimpy(plx))
      bprintf("&+W%-10s&+w: &+Y%-14d", "Wimpy", pwimpy(plx));
    else
      bprintf("&+W%-10s&+w: &+Y%-17s", "Auto-flee", "&+Goff");

#if QTYPE > 1
    bprintf("&+W%-10s&+w: &+R%-11d &+b|\n", "Q-Points", qpoints(plx));
#endif

    bprintf("&+b| &+W%-10s&+w: &+C%-14d", "Age", 
      18 + players[plx].time_on / (60 * 60 * 24));
    bprintf("&+W%-10s&+w: &+m%-14d", "Coins", pcoins(plx));
    bprintf("&+W%-10s&+w: &+M%-2d%%         &+b|\n", "AC Avg", pac(plx));
  }
  if (is_mobile)
    bprintf("                        &+b|\n");
  bprintf(LINE);

#ifdef BODYPART_COMBAT
  if (plev(plx) < LVL_WIZARD && pclass(plx) != PRIEST) {
    show_vital(plx);
    bprintf(LINE);
  }
#endif
}


