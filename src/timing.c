/************************************************************************
 *  timing.c : launch operations that happen each clock tick, and       *
 *  set open_state boolean to correct status, based on the "hours"      *
 *  file which governs the operational hours of the mud                 *
 ***********************************************************************/

#include <sys/signal.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "kernel.h"
#include "mobile.h"
#include "climate.h"
#include "move.h"
#include "timing.h"
#include "bprintf.h"
#include "sendsys.h"
#include "log.h"
#include "fight.h"
#include "mud.h"
#include "locations.h"
#include "objects.h"
#include "idlookup.h"
#include "client.h"
#include "wizard.h"

#define  SECS_IN_A_MIN          60
#define  SECS_IN_AN_HOUR        (SECS_IN_A_MIN*60)
#define  SECS_IN_A_DAY          (SECS_IN_AN_HOUR*24)
#define  SECS_IN_A_WEEK         (SECS_IN_A_DAY*7)

extern void storeplayers(void);

#ifdef IO_STATS
  extern int bytes_read;
  extern int bytes_sent;
  extern int sock_conns;
  extern int cut_conns;
  extern int unres_hosts;
#endif

extern int aberfd;
extern int dnsfd;
extern Boolean auto_restart;

char *time2ascii (time_t t) {
  static char timestr[30];

  if (t == 0)
    t = global_clock;

  strcpy(timestr, ctime(&t));
  *strchr(timestr, '\n') = 0;

  return(timestr);
}

/* The TIME command. List game time elapsed, current time, last reset
 */

void timecom (void) {
  bprintf ("Elapsed Time : ");
  eltime ();

  bprintf ("Current Time : %s\n", time2ascii (TIME_CURRENT));

  bprintf ("Last Reset   : %s", ctime (&last_reset));
}

void uptimecom (void) {
  bprintf ("Elapsed Time             : %s\n", sec_to_str (global_clock-last_startup));
  bprintf ("Been Up Since            : %s", ctime (&last_startup));
  bprintf ("AberChat Status          : %s\n", 
    (aberfd != -1) ? "Operational": "Down");
#ifdef IO_STATS 
  if (plev(mynum) > LVL_WIZARD) {
    bprintf ("Unresolved hosts         : %d [DNS Server: %s]\n",
      unres_hosts, dnsfd != -1 ? "Operational" : "Down");
    bprintf ("Bytes [in/out]           : %d / %d\n", bytes_read, bytes_sent);
    bprintf ("Connects [cut/total]     : %d / %d\n", cut_conns, sock_conns);
  }
#endif
  bprintf ("Number of Resets         : %d\n", numresets);
  bprintf ("Number of Reboots        : %d\n", numreboots);
  bprintf ("Recovered Crashes        : %d\n", numcrashes);
}

/* Prints the game time elapsed */

void eltime (void) {
  long int et = global_clock - last_reset;

  if (et > SECS_IN_A_DAY)
    bprintf ("&+WOver a day!");
  else
    bprintf ("&+W%s", sec_to_str (et));
  bprintf ("\n");
}


/*  Takes a number of seconds as input and converts this to seconds,
 *  minutes, hours, days, which is returned if the pointers != NULL.
 */
static void split_time (long int sec, int *secs, int *min, int *hrs, int *days)
{
  int s = 0, m = 0, h = 0, d = 0;

  if (sec >= SECS_IN_A_DAY) {
    d = sec / SECS_IN_A_DAY;

    sec -= d * SECS_IN_A_DAY;
  }
  if (sec >= SECS_IN_AN_HOUR) {
    h = sec / SECS_IN_AN_HOUR;

    sec -= h * SECS_IN_AN_HOUR;
  }
  if (sec >= SECS_IN_A_MIN) {
    m = sec / SECS_IN_A_MIN;

    sec -= m * SECS_IN_A_MIN;
  }
  s = sec;

  /* Assign return values:
   */
  if (secs != NULL)
    *secs = s;
  if (min != NULL)
    *min = m;
  if (hrs != NULL)
    *hrs = h;
  if (days != NULL)
    *days = d;
}


/*  Takes a number of seconds as input and returns a pointer to a string
 *  containing the amount of time in english, ex: "2 hours, 3 minutes"..etc..
 */
char *sec_to_str (long int seconds) {
  static char str[50];
  int sec, min, hrs, days;
  char aux[15];

  *str = '\0';

  split_time (seconds, &sec, &min, &hrs, &days);

  if (days > 0) {
    sprintf (str, "%d day%s", days, (days == 1) ? "" : "s");
  }
  if (hrs > 0) {
    if (days > 0)
      strcat (str, ", ");
    sprintf (aux, "%d hour%s", hrs, (hrs == 1) ? "" : "s");
    strcat (str, aux);
  }
  if (min > 0) {
    if (days > 0 || hrs > 0)
      strcat (str, ", ");
    sprintf (aux, "%d minute%s", min, (min == 1) ? "" : "s");
    strcat (str, aux);
  }
  if (sec > 0) {
    if (days > 0 || hrs > 0 || min > 0)
      strcat (str, ", ");
    sprintf (aux, "%d second%s", sec, (sec == 1) ? "" : "s");
    strcat (str, aux);
  }
  return (str);
}


/*  Takes a number of seconds as input and returns a pointer to a string
 *  containing hh:mm:ss.
 */
char *sec_to_hhmmss (long int seconds) {
  static char str[25];

  int sec, min, hrs, days;

  split_time (seconds, &sec, &min, &hrs, &days);

  if (days > 0) {
    hrs += days * 24;
  }
  sprintf (str, "%02d:%02d:%02d", hrs, min, sec);

  return (str);
}


/* Returns time t rounded to the nearest minute.
 */
time_t round_to_min (time_t t) {
  t += SECS_IN_A_MIN / 2;

  return (t -= t % SECS_IN_A_MIN);
}


/* Like ctime(3) but strips the seconds, year-part and the \n.
 */
char *my_ctime (time_t * clock) {
  char *t = ctime (clock);

  t[16] = '\0';
  return t;
}

void on_timer (void) {
  static long int n = 0;
  static const int interupts_per_hour = 3600 / TIMER_INTERRUPT;
  int plx;

  setup_globals(-1);

  fflush(logptr);
  fflush(clogptr);

#ifdef ABERCHAT
  if (auto_restart && aberfd == -1 && !(n % 150))
    aberchat_autorecon();
#endif

  if (++n % interupts_per_hour != 0 || time (&global_clock) == -1)
    global_clock += TIMER_INTERRUPT;

  loop_all_players ();
  regenerate ();
  special_events (SP_ALL);
  update_weather();

 #ifdef SAVE_HTML
  if (!(n % 30))                          /* store players html every min */
    storeplayers();
 #endif

  for (plx = 0; plx < max_players; plx++) {
    if (is_in_game(plx))
      do_ingame(plx);
    else if (is_conn(plx)) {
      if ((global_clock - logged_on(plx)) > RES_TM && limbo(plx))
        enter_game(plx);
      else if ((global_clock - logged_on(plx)) > CONNECT_TM)
        close_sock(fildes(plx));
    }
  }
}

void do_ingame(int plx) {
  setup_globals (plx);
  handle_duration (plx);
  cur_player->cast_spell = False;
  if (False);
#ifdef LOCMIN_VILLAGE
  else if (ploc(mynum) == LOC_VILLAGE_EMERGENCY && 
      prndsinroom(mynum) >= cur_player->work)
    exit_hospital();
#endif
  else if (cur_player->crawl > 10)
    cur_player->crawl -= 10;
  else if (linkdead(real_mynum) && global_clock - prlast_cmd(mynum) > LD_TM)
    auto_tout();
  else if (global_clock - prlast_cmd(real_mynum) > IDLE_TM)
    auto_tout();
  else if (cur_player->crawl > 0) {
    dodirn(False, cur_player->crawl);
    cur_player->crawl = 0;
  }
}
