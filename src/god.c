
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"
#include "locations.h"
#include "sendsys.h"
#include "wizlist.h"
#include "bprintf.h"
#include "actions.h"
#include "mobile.h"
#include "timing.h"
#include "fight.h"
#include "rooms.h"
#include "verbs.h"
#include "parse.h"
#include "log.h"
#include "god.h"
#include "uaf.h"
#include "mud.h"

extern void flush_mud(Boolean);

void deletecom () {
  if (plev (mynum) < LVL_SHALAFI) {
    erreval ();
    return;
  }
  if (brkword () == -1 || strlen (wordbuf) > PNAME_LEN) {
    bprintf ("Delete who?\n");
    return;
  }
  mudlog ("DELETE: %s deleted %s", pname (mynum), wordbuf);

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_SHALAFI, LVL_MAX, mynum,
            NOBODY, "&+B[&+W\001p%s\003 &*has &+Cdeleted &+W%s&+B]\n",
            pname (mynum), wordbuf);

  deluaf (wordbuf);
  update_wizlist (wordbuf, LEV_MORTAL);

  bprintf ("Deleted %s.\n", wordbuf);
}

/* check is to see if a use has the crash ability */

void crashcom (Boolean check) {
  char s[MAX_COM_LEN];
  char pidfile[50];
  int i;

  if (check) {
    if (!ptstflg (mynum, PFL_CRASH)) {
      erreval ();
      return;
    }
    getreinput (s); 
  }

  mudlog ("&+WSYSTEM:&N Crash by %s", pname(mynum));

  for (i = 0; i < max_players; i++)
    if (is_in_game (i)) {
      setup_globals(i);
      if (plev(i) > LVL_GOD && check) {
        if (EMPTY(s))
          bprintf ("MUD is now crashing.\n");
        else
          bprintf("%s\n", s);
      }
      else if (check)
        bprintf("\001f%s\003", CRASHED);
      quit_player(False);
    }
 
  sprintf (pidfile, "%s/%s.%d", data_dir, PID_FILE, mud_port);
  remove (pidfile);
  flush_mud(True);
}

void bresetcom (void) {
  if (!ptstflg (mynum, PFL_REBOOT)) {
    erreval ();
    return;
  }
  if (breset) {
    bprintf ("Cancelling reboot for next full reset.\n");
    mudlog ("BRESET: BootReset turned off by %s", pname (mynum));
    breset = False;
  } else {
    bprintf ("The MUD will now reboot during the next full reset.\n");
    mudlog ("BRESET: BootReset turned on by %s", pname (mynum));
    breset = True;
  }
}

void idlecom (void)
{
  if (plev (mynum) < LVL_SHALAFI) {
    bprintf ("Pardon?\n");
    return;
  }
  if (ptstflg (mynum, PFL_IDLE)) {
    bprintf ("You will no longer pretend to be idle.\n");
    pclrflg (mynum, PFL_IDLE);
    return;
  } else {
    bprintf ("You are now pretending to be idle.\n");
    psetflg (mynum, PFL_IDLE);
    return;
  }
}

void
seeidlecom (void)
{
  if (plev (mynum) < LVL_GOD) {
    bprintf ("Pardon?\n");
    return;
  }
  if (ptstflg (mynum, PFL_SEEIDLE)) {
    bprintf ("You will no longer see real idle times.\n");
    pclrflg (mynum, PFL_SEEIDLE);
    return;
  } else {
    bprintf ("You will now see real idle times.\n");
    psetflg (mynum, PFL_SEEIDLE);
    return;
  }
}

void
toggleseesocket (void)
{
  if (!ptstflg (mynum, PFL_SOCKET)) {
    bprintf ("Pardon?\n");
    return;
  }
  if (!ptstflg (mynum, PFL_SEESOCKET)) {
    psetflg (mynum, PFL_SEESOCKET);
    bprintf ("You can now see socket messages.\n");
  } else {
    pclrflg (mynum, PFL_SEESOCKET);
    bprintf ("You will no longer see socket messages.\n");
  }
}

void
togglecoding (void)
{
  if (plev (mynum) < LVL_GOD) {
    erreval ();
    return;
  }
  if (!ststflg (mynum, SFL_CODING)) {
    ssetflg (mynum, SFL_CODING);
    bprintf ("You are marked as coding.\n");
    send_msg (DEST_ALL, 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
              "&+G[&+W%s &*is going away to code&+G]\n", pname (mynum));
  } else {
    sclrflg (mynum, SFL_CODING);
    bprintf ("You are no longer marked as coding.\n");
    send_msg (DEST_ALL, 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
              "&+G[&+W%s &*is back from coding&+G]\n", pname (mynum));
  }
}

void
wloadcom ()
{
  char work[100];
  char ps1[100];
  char ps2[300];

  FILE *fp;
  char command[50];

  if (plev (mynum) < LVL_GOD) {
    erreval ();
    return;
  }
  sprintf (command, "ps -ux | grep 'aberd' | grep '%d'", getpid ());
  if ((fp = popen ("uptime; ps -ux", "r")) == NULL) {
    bprintf ("Error getting system work load statistics.\n");
    return;
  }
  fgets (work, sizeof (work), fp);
  fgets (ps1, sizeof (ps1), fp);
  pclose (fp);

  if ((fp = popen (command, "r")) == NULL) {
    bprintf ("Error getting system work load statistics.\n");
    return;
  }
  fgets (ps2, sizeof (ps2), fp);
  pclose (fp);

  bprintf ("System statistics for %s:\n", MUD_NAME);
  bprintf ("%s\n", work);

  bprintf ("CDirt Daemon Statistics\n");
  bprintf ("-----------------------\n");
  bprintf ("%s", ps1);
  bprintf ("%-79.79s\n", ps2);
}
void
silentcom (void)
{
  if (plev (mynum) < LVL_GOD) {
    erreval ();
    return;
  }
  if (ststflg (mynum, SFL_SILENT)) {
    bprintf ("Disabling Silent Entry\n");
    sclrflg (mynum, SFL_SILENT);
  } else {
    bprintf ("Enabling Silent Entry\n");
    ssetflg (mynum, SFL_SILENT);
  }
}

void
ploccom (void)
{
  int x, loc;
  int me = real_mynum;

  if (plev (mynum) < LVL_GOD) {
    bprintf ("Pardon?\n");
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Who do you want to relocate?\n");
    return;
  }
  if (EMPTY (item2)) {
    bprintf ("Where do you want to relocate them to?\n");
    return;
  }
  if ((x = pl1) == -1) {
    bprintf ("That person can't be found.\n");
    return;
  }
  if ((loc = findroomnum (item2)) == 0) {
    bprintf ("Location does not exist!\n");
    return;
  }
  bprintf ("You relocate %s to %s (%s).\n",
           pname (x), sdesc (loc), showname(loc));

  send_msg (DEST_ALL, 0, LVL_GOD, LVL_MAX, mynum, x,
       "&+B[&+W\001p%s\003 &*has located &+W\001p%s\003 &*to %s (%s)&+B]\n",
            pname (mynum), pname (x), sdesc (loc), showname(loc));

  if (x < max_players) {
    setup_globals (x);
    setploc (mynum, loc);
    setup_globals (me);
  } else
    setploc (x, loc);
}

void
probationcom (void)
{
  int b;

  if (!ptstflg (mynum, PFL_PROBATION)) {
    erreval ();
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Put whom on probation?\n");
    return;
  }
  if ((b = pl1) == -1) {
    bprintf ("Who is that?\n");
    return;
  }
  if (b >= max_players) {
    bprintf ("I'm afraid that you can't place a mobile on probation.\n");
    return;
  }
  if (!do_okay (mynum, b, PFL_NOPROBATION)) {
    bprintf ("You cannot place that player under probation.\n");
    return;
  }
  if (!ststflg (b, SFL_ONPROBATION)) {
    mudlog ("PROBATION: %s placed %s ON probation", pname (mynum), pname
(b));
    bprintf ("Placing %s on probation.\n", pname (b));
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, b,
              "&+B[&+WProbation (On): &+C\001p%s\003 &*by &+C\001p%s\003"
              "&+B]\n", pname (b), pname (mynum));
    ssetflg (b, SFL_ONPROBATION);

    if (plev (b) < LVL_WIZARD)
      return;

    pclrflg (b, PFL_EXOR);
    pclrflg (b, PFL_ZAP);
    pclrflg (b, PFL_MFLAGEDIT);
    pclrflg (b, PFL_OBJECT);
    pclrflg (b, PFL_HEAL);
    pclrflg (b, PFL_ALIAS);
    pclrflg (b, PFL_ROOM);
    pclrflg (b, PFL_CLONE);
    pclrflg (b, PFL_PUNT);
    pclrflg (b, PFL_TIMEOUT);
    pclrflg (b, PFL_LD_STORE);
    pclrflg (b, PFL_BURN);
    pclrflg (b, PFL_SIC);

    if (wlevel (b) < LEV_ISTARI)
      return;

    pclrflg (b, PFL_RAW);
    pclrflg (b, PFL_CH_SCORE);
    pclrflg (b, PFL_CH_LEVEL);
  } else {
    mudlog ("PROBATION: %s placed %s OFF probation", pname (mynum), pname
(b));
    bprintf ("Taking %s off probation.\n", pname (b));
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, b,
              "&+B[&+WProbation (Off): &+C\001p%s\003 &*by &+C\001p%s\003"
              "&+B]\n", pname (b), pname (mynum));
    sclrflg (b, SFL_ONPROBATION);

    if (plev (b) < LVL_WIZARD)
      return;

    psetflg (b, PFL_EXOR);
    psetflg (b, PFL_ZAP);
    psetflg (b, PFL_MFLAGEDIT);
    psetflg (b, PFL_OBJECT);
    psetflg (b, PFL_HEAL);
    psetflg (b, PFL_ALIAS);
    psetflg (b, PFL_ROOM);
    psetflg (b, PFL_CLONE);
    psetflg (b, PFL_PUNT);
    psetflg (b, PFL_TIMEOUT);
    psetflg (b, PFL_LD_STORE);
    psetflg (b, PFL_BURN);
    psetflg (b, PFL_SIC);

    if (wlevel (b) < LEV_ISTARI)
      return;

    psetflg (b, PFL_RAW);
    psetflg (b, PFL_CH_SCORE);
    psetflg (b, PFL_CH_LEVEL);
  }
}

void forgetlist (void) {
  int b, i;

  if (plev (mynum) < LVL_GOD) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf ("Who?\n");
    return;
  }
  if ((b = fpbn(wordbuf)) != -1) {
    bprintf ("%s is currently forgetting:\n", pname (b));
    for (i = 0; i < 10; ++i) {
      if (rplrs[b].forget[i] != -1) {
        if (!is_in_game (rplrs[b].forget[i]))
          rplrs[b].forget[i] = -1;
        else
          bprintf ("%s\n", pname (rplrs[b].forget[i]));
      }
    }
    return;
  }
  bprintf ("That player is not online.\n");
}

void
noruncom (void)
{
  int i, me = real_mynum;

  if (plev (mynum) < LVL_GOD) {
    erreval ();
    return;
  }
  if (norun) {
    norun = False;
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
              "&+B[&+WRunning Is Allowed Again&+B]\n");
    mudlog ("NORUN: Turned Off by %s", pname (mynum));
  } else {
    norun = True;
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
              "&+B[&+WRunning Is Being Disabled&+B]\n");
    mudlog ("NORUN: Turned On by %s", pname (mynum));
  }
  for (i = 0; i < max_players; i++) {
    if (is_in_game (i) && plev (i) < LVL_WIZARD) {
      setploc (i, norun ? LOC_LIMBO_NORUN : randperc () < 50 ?
               LOC_START_TEMPLE : LOC_START_CHURCH);
      setup_globals (i);
      trapch (ploc (mynum));
    }
  }
  setup_globals (me);
}

