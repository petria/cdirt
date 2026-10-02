#include <unistd.h>
#include "kernel.h"
#include "pflags.h"
#include "bprintf.h"
#include "parse.h"
#include "log.h"
#include "mud.h"

void banlistcom (void) {
  bprintf("The following hosts have been banned:\n\n");
  bprintf("\001f%s\003", BAN_HOSTS);
  bprintf("\n\nThe following hots have been user/who banned:\n\n");
  bprintf("\001f%s\003", BAN_WHOS);
  bprintf("\n\nThe following accounts have been banned:\n\n");
  bprintf("\001f%s\003", BAN_ACCTS);
  bprintf("\n\nThe following characters have been banned:\n\n");
  bprintf("\001f%s\003", BAN_CHARS);

}

void bancom (char filename[100], int type) {
  static char *log[] =
  {
    "BANHOST", "BANCHAR", "BANACCT", "BANWHO"
  };
  static char *info[] =
  {
    "host", "user", "account", "login checks from"
  };

  int val;
 char text[80], orig[80];

  if (!ptstflg (mynum, PFL_BAN)) {
    erreval ();
    return;
  }
  getreinput (text);

  if (EMPTY (text)) {
    bprintf ("Ban/Unban who or what?\n");
    return;
  }
  sprintf (orig, "%s", text);

  if (type == BANCHAR)
    orig[0] = toupper (orig[0]);

  val = addordel (filename, text);

  if (val == 0) {
    bprintf ("Error has occured in ban.\n");
    mudlog ("%s: Error has occured!", log[type]);
    return;
  }
  if (val == ADDED) {
    bprintf ("You have banned %s %s.\n", info[type], orig);
    mudlog ("%s: %s banned %s.", log[type], pname (mynum), orig);
    return;
  }
  if (val == DELETED) {
    bprintf ("You have unbanned %s %s.\n", info[type], orig);
    mudlog ("%s: %s unbanned %s.", log[type], pname (mynum), orig);
    return;
  }
}


Boolean check_host_bans (void) {
  if (is_host_who_banned(hostname(mynum)))
    cur_player->user_ban = True;
  else if (is_host_who_banned(ip_addr(mynum)))
    cur_player->user_ban = True;

  if (is_host_banned(hostname(mynum))) {
    sock_msg ("&+RHostname banned: %s", hostname(mynum));
    bprintf ("\001f%s\003", HOST_BANNED);
  }
  else if (is_host_banned(ip_addr(mynum))) {
    sock_msg ("&+RIP banned: %s", ip_addr(mynum));
    bprintf ("\001f%s\003", IP_BANNED);
  }
  else if (is_host_username_banned(username(mynum))) {
    sock_msg ("&+RUser@host banned: %s", username(mynum));
    bprintf ("\001f%s\003", USERNAME_BANNED);
  }
  else
    return False;

  quit_player(False);
  return(True);
}

int addordel (char file_name[80], char check[80])
{
  FILE *fl;

  if ((fl = FOPEN(file_name, "r")) != NULL)
    if (infile (file_name, check)) {
      FCLOSE(fl);
      return delname (file_name, check);
    } else {
      FCLOSE(fl);
      return addname (file_name, check);
  } else
    return 0;
}

int addname (char file_name[80], char name[80])
{
  FILE *fl;

  if ((fl = (FOPEN(file_name, "a"))) == NULL) {
    return 0;
  }
  fprintf (fl, name);
  fprintf (fl, "\n");
  FCLOSE(fl);
  return ADDED;
}

int delname (char file_name[80], char name[80])
{
 FILE *a, *b;
  char name_in_file[80];

  if ((a = FOPEN(file_name, "r")) == NULL)
    return 0;
  if ((b = FOPEN("TMP_FILE", "w")) == NULL) {
    FCLOSE(a);
    return 0;
  }
  strcat (name, "\n");
  while (fgets (name_in_file, sizeof name_in_file, a))
    if (!EQ (name, name_in_file))
      fprintf (b, "%s", name_in_file);
  FCLOSE(a);
  FCLOSE(b);
  unlink (file_name);
  rename ("TMP_FILE", file_name);
 return DELETED;
}

Boolean infile (char *file, const char *line)
{
  FILE *fl;
  char buff[80];

  if ((fl = FOPEN(file, "r")) == NULL)
    return False;

  while(1) {
    fgets(buff, 80, fl);
    if (feof(fl))
      break;
    else if (match (buff, line)) {
      FCLOSE(fl);
      return(True);
    }
  }

  FCLOSE(fl);
  return(False);
}
