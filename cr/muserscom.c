#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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
#include "coders.h"
#include "timing.h"

/* This command shows a list of Mortal Users, and their stats */

void muserscom() {
  char wiznam[128], idlebuff[64];
  int a[max_players], a_len=0, i, j;
  time_t now;

  for (i=0; i<max_players; i++)
    if (is_in_game(i) && (!pvis(i) || pvis(i) <= plev(mynum) || i == mynum) &&
        plev(i) < LVL_WIZARD && !strncasecmp(pname(i), item1, strlen(item1)))
      a[a_len++] = i;

  qsort(a, a_len, sizeof(int), cmp_player);

  if (!a_len) {
    if (EMPTY(item1))
      bprintf("&+cThere are no mortals currently on the game.\n");
    else
      bprintf("&+cThere are no mortals on that match that pattern.\n");
    return;
  }

  if (plev(mynum)<LVL_WIZARD)
    bprintf ("&+CName            Level          Idle\n"
             "&+g---------------------------------------\n");
  else
    bprintf ("&+CName        Sex   Level        Location      "
             "Strength  Mana   Status   Idle\n"
             "&+g---------------------------------------"
             "----------------------------------------\n");

  time (&now);
  for (j = 0; j < a_len ; ++j) {
    i = a[j];
    if (plev(mynum) >= LVL_WIZARD)
      sprintf(wiznam, " %s", showname(ploc(i)));
    if (plev(mynum) < LVL_WIZARD)
      bprintf ("&+Y%-*s&+W%-15s", PNAME_LEN+3, pname(i),
               tlookup (pname(i), coders) != -1 ? "Coder" : player_level(i));
    else  
bprintf("&+Y%-13s%-8s&+W%-12s&+B%-15s%s%3d&+y/&+Y%-5d%s%3d&+y/&+Y%-4d&+c%-9s",
             pname(i), psex(i) ? "&+MF" : "&+yM", 
             tlookup(pname(i), coders) != -1 ? "Coder": player_level(i),wiznam,
             pstr(i) < maxstrength(i) ? "&+m" : "&+Y", pstr(i), maxstrength(i),
             pmagic(i) < maxmagic(i) ? "&+m" : "&+Y", pmagic(i), maxmagic(i),
             pfighting(i) != -1 ? "&+Rfighting&N  " :
             psitting(i) ? "sitting" : "standing");

  if (ptstflg (mynum, PFL_SEEIDLE)) 
    strcpy (idlebuff, (char *) sec_to_hhmmss (global_clock - prlast_cmd (i)));
  else 
    strcpy (idlebuff, (char *) sec_to_hhmmss (global_clock - plast_cmd (i)));
  bprintf ("&+R%s\n", idlebuff);

  }
  if (plev(mynum) < LVL_WIZARD)
    bprintf ("&+g---------------------------------------\n");
  else
    bprintf ("&+g---------------------------------------"
             "----------------------------------------\n");
  if (EMPTY(item1))
    bprintf ("&+cThere %s &+C%d&+c mortal%s currently on the game.\n",
             (a_len == 1 ? "is" : "are"), a_len, (a_len == 1 ? "" : "s"));
  else
    bprintf ("&+cThere %s &+C%d&+c mortal%s on that match that pattern.\n",
             (a_len == 1 ? "is" : "are"), a_len, (a_len == 1 ? "" : "s"));
}

/* This command shows a list of Immortal Users, and their stats */

void iuserscom() {
  char wiznam[128], buff[128], vis[128];
  int a[max_players], a_len=0, i, j;
  time_t now;
  char level[128];

  for (i = 0; i < max_players; i++)
    if (is_in_game(i) && (!pvis(i) || pvis(i) <= plev(mynum) || i == mynum) &&
	plev(i) >= LVL_WIZARD && !strncasecmp(pname(i), item1, strlen(item1)))
      a[a_len++] = i;

  qsort(a, a_len, sizeof(int), cmp_player);

  if (!a_len) {
    if (EMPTY(item1))
      bprintf("&+cThere are no immortals currently on the game.\n");
    else
      bprintf("&+cThere are no immortals on that match that pattern.\n");
    return;
  }

  if (plev(mynum) < LVL_WIZARD) {
    bprintf ("&+CName            Level          Idle\n");
    bprintf ("&+g---------------------------------------\n");
  } 
  else {
    bprintf ("&+CName          Level        Location      "
	     "Visibility          Status     Idle\n");
    bprintf ("&+g---------------------------------------"
	     "---------------------------------------\n");
  }
  time (&now);
  for (j = 0; i = a[j], j < a_len ; ++j) {
    if (plev(mynum) >= LVL_WIZARD)
      sprintf (wiznam, " %s", showname(ploc(i)));
    if (tlookup (pname(i), coders) != -1)
      strcpy( level, "Coder");
    else {
      strcpy(level, player_level(i));
    }
    if (pvis(i) < LVL_WIZARD)
      sprintf (vis, "%d", pvis(i));
    else {
      sprintf (vis, "%d (%s)", pvis(i), 
                               psex(i) ? FWizLevels[wlevel (pvis (i))] :
                               MWizLevels[wlevel (pvis (i))] );
    }
    if (plev(mynum)<LVL_WIZARD)
      bprintf ("&+Y%-*s&+W%-15s", PNAME_LEN+3, pname(i), level);
    else
      bprintf ("&+Y%-*s&+W%-12s&+B%-15s&+G%-19s&+c%-10s", PNAME_LEN+1, 
	       pname(i), level, wiznam, vis,
               pfighting(i) != -1 ? "&+Rfighting&N" :
                 psitting(i) ? "sitting" : "standing");

    if (ptstflg (mynum, PFL_SEEIDLE))
      strcpy (buff, (char *) sec_to_hhmmss (global_clock - prlast_cmd (i)));
    else
      strcpy (buff, (char *) sec_to_hhmmss (global_clock - plast_cmd (i)));
    bprintf( "&+R%s\n", buff );

  }
  if (plev(mynum) < LVL_WIZARD)
    bprintf ("&+g---------------------------------------\n");
  else
    bprintf ("&+g---------------------------------------"
	     "---------------------------------------\n");
  if (a_len) {
    if (EMPTY(item1))
      bprintf ("&+cThere %s &+C%d&+c immortal%s currently on the game.\n",
	       (a_len == 1 ? "is" : "are"), a_len, (a_len == 1 ? "" : "s"));
    else
      bprintf ("&+cThere %s &+C%d&+c immortal%s on that match that pattern.\n",
	       (a_len == 1 ? "is" : "are"), a_len, (a_len == 1 ? "" : "s"));
  }
}

