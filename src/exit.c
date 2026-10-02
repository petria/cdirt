/************************************************************************
 * The following functions handle all exit() calls from the MUD and 	*
 * handle them itself making sure that the MUD shuts down nicely and 	*
 * with as little problems as possible.					*
 ************************************************************************/

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "kernel.h"
#include "mobile.h"
#include "sendsys.h"
#include "timing.h"
#include "time.h"
#include "exit.h"
#include "mudmacros.h"
#include "log.h"
#include "commands.h"
#include "bprintf.h"
#include "mud.h"
#include "uaf.h"
#include "parse.h"
#include "objsys.h"
#include "update.h"

extern void rm_pid(void);
void fdlistcom(FILE *);

/* sig_exit(): Handles the shutdown of the MUD due to signal error. */

void sig_exit (char *sig, int signal) {
  debug();

  if (signal == SIGUSR1)
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
	      "\001f" SIGNAL1 "\003");
  else if (signal == SIGUSR2)
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
	      "\001f" SIGNAL2 "\003");
  else {
    send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
	      "&+Y[&+RSignal Error: &+W%s&+Y]\n", sig);

    send_msg (DEST_ALL, 0, LVL_MIN, (LVL_WIZARD - 1), NOBODY, NOBODY,
	      "&+Y[&+RInternal error has occured&+Y]\n");
  }

#ifndef CORE
  if (updating)
    run_reboot(False, False);     /* give up on updating mud */
  else
    run_reboot(True, True); 
#endif
}

/* autosave(): Saves all players */

void autosave (void) {
  int plx;
  static PERSONA d;

  for (plx = 0; plx < max_players; ++plx) {
    if (is_in_game (plx)) {
      if (aliased(plx)) {
	unalias (plx);
	setup_globals (plx);
	sendf (plx, "&+W[Unaliasing You]\n");
      }
      sendf (plx, "&+M[Character Autosaved]\n");
      player2pers (&d, &global_clock, plx);   
      putuaf (&d);
    }
  }
}

/* debug(): Write last commands entered to crash log */

void debug (void) {
  int plx;
  int i;
  char *t;
  char nt[100];
  FILE *bugptr;
  time_t cur_time;
  struct tm *now;
  char timestr[80];

  time (&cur_time);
  now = localtime(&cur_time);
  strftime(timestr, 80, "CRASHES/%b%d.%I:%M%p", now);

  if ((bugptr = FOPEN(timestr, "w")) == NULL) {
    mudlog("ERROR: in debug(), unable to open output file");
    return;
  }

#ifdef DEBUG_IO
  fprintf(bugptr, "OPEN FILE DESCRIPTORS:\n");
  for (i = 0 ; i < 80 ; i++)
    fputc('-', bugptr);
  fdlistcom(bugptr);
  fprintf(bugptr, "\n");
  for (i = 0 ; i < 80 ; i++)
    fputc('-', bugptr);
#endif

  if (mynum == -1)
    fprintf(bugptr, "Crash happened during internal function.\n");

  for (plx = 0; plx < max_players; ++plx)
    if (is_conn (plx)) {
      t = ctime (&plast_cmd (plx));

      t[19] = '\0';
      for (i = 0; i < 8; ++i)
	nt[i] = t[i + 11];

      nt[8] = 0;

      fprintf (bugptr, "Name: %-12s Time: %s Location: %s\n", pname(plx),
 	       nt, showname(ploc(plx)));
      fprintf(bugptr, "Input buffer: %s%s\n\n", inp_buffer(plx),
	      players[plx].in_cmd ? " (Caused crash)" : "");
    }
  FCLOSE(bugptr);
}

void signalcom (void) {

  char *SigTable[] = {
    "list", "sigsegv", "sigterm", "sigbus",
    "sigint", "sigusr1", "sigusr2", TABLE_END
  };

  int sig = 0, x = 0;

  if (!ptstflg (mynum, PFL_SIGNAL)) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf ("Send what signal?\n");
    return;
  }
  if ((x = tlookup (wordbuf, SigTable)) < 0)
    bprintf ("Invalid signal.\n");

  switch (x) {
  case 0:
    sig = -1;
    break;
  case 1:
    sig = SIGSEGV;
    break;
  case 2:
    sig = SIGTERM;
    break;
  case 3:
    sig = SIGBUS;
    break;
  case 4:
    sig = SIGINT;
    break;
  case 5:
    sig = SIGUSR1;
    break;
  case 6:
    sig = SIGUSR2;
    break;
  }

  if (sig == -1) {
    bprintf ("Signals: SIGSEGV, SIGTERM, SIGBUS, SIGINT, SIGUSR1, SIGUSR2.\n");
    return;
  }
  send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
	    "&+B[&+CSignal &*has been called: &+W%s&+B]\n",
	    SigTable[x]);

  mudlog ("SIGNAL: %s has called signal %s", pname (mynum), SigTable[x]);
  kill (getpid (), sig);
}

/* shuts down the mud on a sigterm or for a crash (exit) */

void sigterm_exit(void) {
  int plx;

  rm_pid();
  for (plx = 0; plx < max_players; plx++) {
    static int val, ob;

    if (is_in_game(plx)) {
      setup_globals (plx);
      bprintf ("\001f%s\003", CRASHED);
      if (plev(mynum) < LVL_WIZARD) {
        for (ob = 0, val = 0; ob < numobs; ob++)
          if (iscarrby(ob, plx) && ovalue(ob) >= 0) {
            dropinpit (ob);
            val += ovalue(ob);
          }
        if (val > 0)
          bprintf ("You receive credit for items you are carrying "
                   "(%d points).\n", val+1);
      }
      quit_player(False);
    }
  }
}
