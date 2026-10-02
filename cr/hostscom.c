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
#include "timing.h"

extern char *MLevels[];
extern char *FLevels[];
extern char *coders[];

char *host_name(plx) {
  if (plev(plx) >= LVL_GOD && plev(mynum) < LVL_GOD)
    return("(unavailable)");
  else if (ptstflg(mynum, PFL_SEEUSER))
    return(username(plx));
  else if (ptstflg(mynum, PFL_SEEHOST))
    return(hostname(plx));
  else
    return("");
}

void hostscom(void) {
  int a[max_players], a_len=0, i;
  time_t now;
  char connect[30], loginbuff[10], idlebuff[10];

  if (!ptstflg(mynum, PFL_SEEHOST) && !ptstflg(mynum, PFL_SEEUSER))
    bprintf ("&+RPlayer        Connect              On For    Idle\n"
        "&+B-----------------------------------------------------\n");

  else
    bprintf ("&+RPlayer        Hostname                  Connect              "
             "On For    Idle\n&+B------------------------------"
             "-------------------------------------------------\n");

  for (i = 0; i < max_players; i++)
    if (is_in_game(i) && (!pvis(i) || pvis(i) <= plev(mynum) || i == mynum) &&
        !strncasecmp(pname(i), item1, strlen(item1)))
      a[a_len++] = i;
  qsort (a, a_len, sizeof(int), cmp_player);

  time (&now);
  for (i = 0; i < a_len; i++) {
    strcpy(connect, (char *) time2ascii(plogged_on(a[i])));
    connect[19] = 0;                                      /* eliminate year */
    strcpy(loginbuff, (char *) sec_to_hhmmss(global_clock - plogged_on(a[i])));

    if (ptstflg (mynum, PFL_SEEIDLE))
      strcpy (idlebuff, sec_to_hhmmss (global_clock - prlast_cmd (a[i])));
    else
      strcpy (idlebuff, sec_to_hhmmss (global_clock - plast_cmd (a[i])));

    if (!ptstflg(mynum, PFL_SEEHOST) && !ptstflg(mynum, PFL_SEEUSER))
      bprintf ("&+Y%-12s  %s  %s  %s\n", pname(a[i]),
              connect, loginbuff, idlebuff);
    else
      bprintf ("&+Y%-12s  %-24.24s  %s  %s  %s\n", pname(a[i]),
               host_name(a[i]), connect, loginbuff, idlebuff);
  }
  if (!a_len) {
    if (EMPTY(item1))
      bprintf ("&+BThere are no players currently on the game.\n");
    else
      bprintf ("&+BThere are no players on that match that pattern.\n");
  }
  if (!ptstflg(mynum, PFL_SEEHOST) && !ptstflg(mynum, PFL_SEEUSER))
    bprintf ("&+B-----------------------------------------------------\n");
  else
    bprintf ("&+B-----------------------------------------------------------"
             "--------------------\n");
  if (a_len) {
    if (EMPTY(item1))
      bprintf ("&+BThere %s &+R%d&+B player%s currently on the game.\n",
               a_len == 1 ? "is" : "are", a_len, a_len == 1 ? "" : "s");
    else
      bprintf ("&+BThere %s &+R%d&+B player%s on that match that pattern.\n",
               a_len == 1 ? "is" : "are", a_len, a_len == 1 ? "" : "s");
  }
}
