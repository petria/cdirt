#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include "kernel.h"
#include "bprintf.h"
#include "log.h"

int open_logfile (Boolean clear_flag)
{
  char type[2] = "a";

  if (clear_flag)
    *type = 'w';

  if ((logptr = FOPEN(LOG_FILE, type)) == NULL)
    return -1;
  if ((clogptr = FOPEN(CLOG_FILE, type)) == NULL)
    return -1;
  #ifdef LOG_INPUT
  if ((plogptr = FOPEN(PLAYER_LOGFILE, type)) == NULL)
    return -1;
  #endif

  return(0);
}

void close_mudfiles (void) {
  if (logptr != NULL)
    FCLOSE(logptr);
  if (clogptr != NULL)
    FCLOSE(clogptr);
#ifdef BOB
  if (dfile != NULL)
    FCLOSE(dfile);
#endif
#ifdef LOG_INPUT
  if (plogptr != NULL)
    FCLOSE(plogptr);
#endif
}

void
progerror (char *name)
{
  mudlog ("PERROR %s: [%d] %s", name, errno, strerror (errno));
}

void mudlog (char *format,...) {
  char buff[256], buff2[256], timestr[20];
  va_list pvar;
  struct tm *now;
  time_t timenum;

  time(&timenum);
  now = localtime(&timenum);
  strftime(timestr, 20, "%b%d %H:%M", now);

  va_start (pvar, format);
  vsprintf (buff, format, pvar);
  va_end (pvar);

  fprintf(clogptr, "%s %s\n", timestr, buff);
  strip_color(buff2, buff);
  fprintf(logptr, "%s %s\n", timestr, buff2);
}

void open_plr_log (void)
{
  char filename[100];

  sprintf (filename, "%s/%s", LOG_DIR, pname (mynum));

  if ((cur_player->log = FOPEN(filename, "a")) == NULL) {
    islogged(mynum) = False;
    return;
  }
  islogged(mynum) = True;
  write_plr_log("LOG: Log Started\n");
  mudlog ("MONITOR: Player Log Opened For %s", pname (mynum));
}

void close_plr_log (void)
{
  if (islogged(mynum)) {
    write_plr_log ("LOG: Log Closed\n");
    FCLOSE(cur_player->log);
    islogged(mynum) = False;
    mudlog ("MONITOR: Player Log Closed For %s", pname (mynum));
  }
}

void write_plr_log (char *text) {
  char buff[4096];

  strip_color(buff, text);
  fprintf(cur_player->log, buff);
  fflush (cur_player->log);
}
