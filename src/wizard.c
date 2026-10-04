
#include <unistd.h>
#include <stdlib.h>
#include "kernel.h"
#include <time.h>
#include <string.h>
#include "levels.h"

#include <stdarg.h>

#include "timing.h"
#include "log.h"
#include "uaf.h"
#include "bprintf.h"
#include "verbs.h"
#include "wizard.h"
#include "parse.h"
#include "sendsys.h"
#include "mobile.h"
#include "objsys.h"
#include "rooms.h"
#include "locations.h"
#include "fight.h"
#include "wizlist.h"
#include "commands.h"
#include "clone.h"
#include "mud.h"
#include "log.h"
#include "files.h"

char *host_name(int);

extern HASH_TABLE oalts_n[];
extern HASH_TABLE objects_n[];
extern HASH_TABLE ublock_n[];
extern HASH_TABLE ualts_n[];

/* SNOOP allows players to spy on others */
void snoopcom (void) {
  int plr;

  if (!ptstflg (mynum, PFL_SNOOP)) {
    erreval ();
    return;
  }
  if (snooptarget(mynum) >= 0) {
    bprintf ("Stopped snooping on %s.\n", pname (snooptarget(mynum)));
    if (plev(mynum) < LVL_CREATOR)
      mudlog("%s has stopped snooping %s.", pname(mynum), pname(snooptarget(mynum)));
    snoop_off (mynum);
    return;
  }
  if ((plr = pl1) < 0)
    return; 
  else if (plr >= max_players) {
    bprintf ("You can't snoop %s!\n", pname (plr));
    return;
  } 
  else if (plr == mynum) {
    bprintf ("You can't snoop yourself!\n");
    return;
  }
  if (!do_okay (mynum, plr, PFL_NOSNOOP)) {
    bprintf ("Your magical vision is obscured.\n");
    return;
  }
  if (ltstflg (ploc (plr), LFL_PRIVATE) && plev (mynum) < LVL_ARCHWIZARD) {
    bprintf ("I'm sorry, %s, but the room is private.\n",
             psex (mynum) ? "Madam" : "Sir");
    return;
  }
  if (ltstflg (ploc (plr), LFL_NO_SNOOP) && plev(mynum) < LVL_CREATOR) {
    bprintf ("I'm sorry, %s, but the room is &+W[NO_SNOOP]!!\n",
             psex (mynum) ? "Madam" : "Sir");
    return;
  }
  snooptarget(mynum) = plr;
  ++(snooped(plr));

  bprintf ("Started to snoop on %s.\n", pname (plr));
  if (plev(mynum) < LVL_CREATOR)
    mudlog("%s has started snooping %s.", pname(mynum), pname(plr));
}

/* Make player no longer snoop his target
 */
void snoop_off (int plr) {
  int target;

  if ((target = snooptarget(plr)) >= 0) {

    snooptarget(plr) = -1;

    if (--snooped(target) < 0)
      snooped(target) = 0;
  }
}

/* The SYSTEM command
 */
void
systemcom ()
{
  char x[MAX_COM_LEN], y[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_RAW) && plev (mynum) < LVL_ARCHWIZARD) {
    erreval ();
    return;
  }
  getreinput (x);
  sprintf (y, "&+W[SYSTEM] &+C: &*%s&#&#\n", x);
  broad (y);

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_SHALAFI, LVL_MAX, mynum,
	  NOBODY, "&+B[&+CSystem &*by &+W\001p%s\003&+B]\n", pname (mynum));
}

/* Call For Reset
 */
void
cfrcom ()
{
  char x[MAX_COM_LEN], y[MAX_COM_LEN];

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  getreinput (x);
  sprintf (y, "&+W[Call For Reset by \001p%s\003] &+C: &*%s&#&#\n", 
    pname(mynum), x);
  broad (y);
}

/* The RAW command
 */
void
rawcom ()
{
  char x[MAX_COM_LEN], y[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_RAW)) {
    erreval ();
    return;
  }
  getreinput (x);
  sprintf (y, "&+W[\001p%s\003] &+C: &*%s&#&#\n", pname (mynum), x);
  broad (y);

}

void textrawcom () {
  char send[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_TEXTRAW)) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf ("Textraw what?\n");
    return;
  }
  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_GOD, LVL_MAX, mynum,
         NOBODY, "&+B[&+CTextRaw &*by &+W\001p%s\003&+B]\n", pname(mynum));

  sprintf (send, "&+W[%s&+W] &+C: &+w%s&#&#\n", item1, txt2);
  broad (send);
}

void  tournamentcom ()  {
  if (plev (mynum) < LVL_ARCHWIZARD) {
    erreval ();
    return;
  }
  else {
    the_world->w_tournament = !the_world->w_tournament;

    bprintf ("Tournament-mode is now &+W%s&*.\n",
      the_world->w_tournament ? "ON" : "OFF");

    mudlog ("&+WSYSTEM:&N Tournament mode turned %s by %s",
      the_world->w_tournament ? "On" : "Off", pname (mynum));

    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	    "&+B[&*Tournament mode turned &+C%s &*by &+W\001p%s\003&+B]\n",
	the_world->w_tournament ? "On" : "Off", pname (mynum));
  }
}

void syslogcom (void) {
  FILE *sysfp, *pagefp;
  char buffer[300], search[MAX_COM_LEN], tmpbuf[300];
  char tmpname[100];
  struct tm *now;

  int lines = 0;
  int sysloglen = fnumlines (CLOG_FILE);
  Boolean found = False;

  if (!ptstflg (mynum, PFL_SYSLOG)) {
    erreval ();
    return;
  }
  sprintf (tmpname, "TMP/SYSLOG.%s", pname (mynum));

  if ((pagefp = FOPEN (tmpname, "wt")) == NULL) {
    bprintf ("Error: Cannot create temporary file.\n");
    mudlog ("ERROR: Cannot create temporary file for syslog pager");
    return;
  }
  if ((sysfp = FOPEN (CLOG_FILE, "rt")) == NULL) {
    bprintf ("Error: Cannot read syslog file.\n");
    mudlog ("ERROR: Cannot read syslog file.");
    return;
  }
  if (EMPTY (txt1)) {
    now = localtime(&global_clock);
    strftime(search, 20, "%b%d", now);
  } 
  else {
    getreinput (search);
    if ((lines = atoi (search)) > 0) {
      fileseek (sysfp, (sysloglen - lines));
      strcpy (search, " ");
    }
    if (search[0] == '*') {
      fileseek (sysfp, 1);
      strcpy (search, " ");
    }
  }

  while (1) {
    fgets (buffer, sizeof (buffer), sysfp);
    if (feof (sysfp))
      break;
    strcpy (tmpbuf, buffer);
    if (strstr (uppercase (tmpbuf), uppercase (search))) {
      fputs(buffer, pagefp);
      found = True;
    }
  }
  FCLOSE (sysfp);
  FCLOSE (pagefp);

  if (found)
    bprintf("\001f%s\003", tmpname);
  else
    bprintf("No pattern matches search.\n");
}


/* (C) Rassilon (Brian Preble) */
void
echocom ()
{
  char x[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_ECHO)) {
    bprintf ("You hear echos.\n");
    return;
  }
  getreinput (x);
  if (EMPTY (x)) {
    bprintf ("ECHO what?\n");
    return;
  }
  send_msg(sendloc(mynum), MODE_NPFLAG | MP (PFL_NOECHO), LVL_MIN, LVL_MAX, mynum,
	    NOBODY, "%s\n", x);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You echo: %s\n", x);
  else
    bprintf ("Ok\n");

  send_msg(sendloc(mynum), MODE_SFLAG | MS (SFL_SEEEXT), LVL_ARCHWIZARD, LVL_MAX, mynum,
	    NOBODY, "&+B[&+CEcho &*by &+W\001p%s\003&+B]\n", pname (mynum));
}


void
levechocom (void)
{
  int lev;
  char x[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_ECHO)) {
    bprintf ("You hear echos.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("Echo to what level?\n");
    return;
  }
  lev = atoi (wordbuf);
  if (lev < 0 || lev > plev (mynum)) {
    bprintf ("You cannot echo to level %d.\n", lev);
    return;
  }
  getreinput (x);

  if (EMPTY (x)) {
    bprintf ("Echo what to level %d?\n", lev);
    return;
  }
  send_msg (DEST_ALL, MODE_NPFLAG | MP (PFL_NOECHO), lev, LVL_MAX, mynum,
	    NOBODY, "%s\n", x);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You lev-echoed (Lvl %d): %s\n", lev, x);
  else
    bprintf ("Ok\n");

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_ARCHWIZARD, LVL_MAX, mynum,
	    NOBODY, "&+B[&+CLevecho (%d) &*by &+W\001p%s\003&+B]\n", lev, pname (mynum));

}

/* (C) Rassilon (Brian Preble) */
void
echoallcom ()
{
  char x[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_ECHO)) {
    bprintf ("You hear echos.\n");
    return;
  }
  getreinput (x);

  if (EMPTY (x)) {
    bprintf ("Echo what?\n");
    return;
  }
  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_ARCHWIZARD, LVL_MAX,
  mynum, NOBODY, "&+B[&+CEchoall &*by &+W\001p%s\003&+B]\n", pname(mynum));

  send_msg (DEST_ALL, MODE_NPFLAG | MP (PFL_NOECHO), LVL_MIN, LVL_MAX, mynum,
	    NOBODY, "%s\n", x);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You echoall: %s\n", x);
  else
    bprintf ("Ok\n");
}

void emoteallcom ()
{
  char x[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_EMOTE)) {
    bprintf ("Your emotions are strictly limited.\n");
    return;
  }
  getreinput (x);

  if (EMPTY (x)) {
    bprintf ("Emoteall what?\n");
    return;
  }
  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_ARCHWIZARD, LVL_MAX,
  mynum, NOBODY, "&+B[&+CEmoteall &*by &+W\001p%s\003&+B]\n", pname(mynum));

  send_msg (DEST_ALL, MODE_NPFLAG | MP (PFL_NOECHO), LVL_MIN, LVL_MAX, mynum,
            NOBODY, "%s %s\n", pname(mynum), x);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You emoteall: %s %s\n", pname(mynum), x);
  else
    bprintf ("Ok\n");
}

/* (C) Rassilon (Brian Preble) */
void
echotocom ()
{
  int b;

  if (!ptstflg (mynum, PFL_ECHO)) {
    bprintf ("You hear echos.\n");
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Echo to who?\n");
    return;
  }
  if ((b = pl1) == -1) {
    bprintf ("No one with that name is playing.\n");
    return;
  }
  if (b == mynum) {
    bprintf ("What's the point?\n");
    return;
  }
  if (EMPTY (txt2)) {
    bprintf ("What do you want to echo to them?\n");
    return;
  }
  if (EQ (pname (mynum), "Oracle"))
    if (strstr (txt2, "tells you") && ststflg (b, SFL_NOORACLE))
      return;

  if (check_busy (b))
    return;

  send_msg (int2idx(b, MOB), MODE_NPFLAG | MP (PFL_NOECHO), LVL_MIN, LVL_MAX, 
    NOBODY, NOBODY, "%s\n", txt2);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You echoto: %s\n", txt2);
  else
    bprintf ("Ok\n");

  if (!EQ(pname (mynum), "Inego") ) {
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_GOD, LVL_MAX,
	      mynum, NOBODY, "&+B[&+CEchoto to &+W\001p%s\003 &*by "
	      "&+W\001p%s\003&+B]\n", pname (b), pname (mynum));
  }
}

/* (C) Jim Finnis  (Yes he really did write one or two routines/A) */
void
emotecom ()
{
  char buff[MAX_COM_LEN + 10];

  if (!ptstflg (mynum, PFL_EMOTE) && !ltstflg (ploc (mynum), LFL_PARTY)) {
    bprintf ("Your emotions are strictly limited!\n");
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("What do you want to emote?\n");
    return;
  }
  getreinput (buff);
  send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
	    "\001p%s\003 %s\n", pname (mynum), buff);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You emote: '%s %s'\n", pname (mynum), buff);
  else
    bprintf ("Ok\n");
}

/* (C) Rassilon (Brian Preble) */
void
emotetocom ()
{
  int a;

  if (!ptstflg (mynum, PFL_EMOTE) && plev(mynum) < LVL_WIZARD 
      && !ltstflg (ploc (mynum), LFL_PARTY)) {
    bprintf ("Your emotions are strictly limited!\n");
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Emote to who?\n");
    return;
  }
  if ((a = pl1) == -1) {
    bprintf ("No one with that name is playing.\n");
    return;
  }
  if (a == mynum) {
    bprintf ("Good trick, that.\n");
    return;
  }
  if( !ptstflg (mynum, PFL_EMOTE) && ltstflg (ploc (mynum), LFL_PARTY)
      && ploc(a) != ploc(mynum) ) {
    bprintf ("Your emotions are strictly limited!\n");
    return;
  }
  if (EMPTY (txt2)) {
    bprintf ("Emote what?\n");
    return;
  }
  if (check_busy (a))
    return;

  sendf(a, "%\001p%s\003%s\n", txt2);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("Your emoteto looked like: %s %s\n", pname (mynum), txt2);
  else
    bprintf ("Ok\n");
}

/* The SET command
 */
/* These properties require numbers:
 */
#define SET_BVALUE  0
#define SET_SIZE    1
#define SET_VIS     2
#define SET_DAMAGE  3
#define SET_ARMOR   4
#define SET_STATE   5

/* These require texts:
 */
#define SET_TEXT_MIN 5
#define SET_DESC0  6
#define SET_DESC1  7
#define SET_DESC2  8
#define SET_DESC3  9
#define SET_NAME   10
#define SET_ANAME  11
#define SET_TEXT_MAX 11

/* Properties
 */
static char *Props[] =
{
 "BaseValue", 		"Size", 	"Visibility", 		"Damage",
 "AC", 			"State", 	"Desc0", 		"Desc1",
 "Desc2", 		"Desc3", 	"Name", 		"AltName",
 TABLE_END
};

void setcom ()
{
  int o, p, v;			/* Object, Property, Value */

  Boolean is_oflag;

  if (!ptstflg (mynum, PFL_OBJECT)) {
    erreval ();
    return;
  }
  if (brkword () == -1 || (o = fobn (wordbuf)) == -1) {
    bprintf ("Set what??\n");
    return;
  }
  if (brkword () == -1 || ((is_oflag = (p = tlookup (wordbuf, Props)) == -1)
			   && (p = tlookup (wordbuf, Oflags)) == -1)) {

    bprintf ("Set what property on the %s?\n", oname (o));
    return;
  }
  if ((is_oflag || p <= SET_TEXT_MIN || p > SET_TEXT_MAX)
      && brkword () == -1) {

    bprintf ("Set the %s property to what ??\n",
	     is_oflag ? Oflags[p] : Props[p]);
    return;
  }
  if (is_oflag) {

    if ((v = tlookup (wordbuf, OO)) == -1
	&& (v = tlookup (wordbuf, TF)) == -1) {

      bprintf ("Value must be On or Off (or True/False).\n");
      return;
    }
    if (v)
      osetbit (o, p);
    else
      oclrbit (o, p);
  } 
  else if (p <= SET_TEXT_MIN || p > SET_TEXT_MAX) {

    int limit = -1;

    if (!isdigit (*wordbuf)) {
      bprintf ("Value must be a number >= 0.\n");
      return;
    } else
      v = atoi (wordbuf);

    switch (p) {
    case SET_BVALUE:
      if (v > O_BVALUE_MAX ||
	  (v > obaseval (o) && v > 400
	   && !ptstflg (mynum, PFL_FROB)))
	limit = O_BVALUE_MAX;
      else
	osetbaseval (o, v);
      break;
    case SET_SIZE:
      if (v > O_SIZE_MAX)
	limit = O_SIZE_MAX;
      else
	osetsize (o, v);
      break;
    case SET_VIS:
      if (v > O_VIS_MAX)
	limit = O_VIS_MAX;
      else
	osetvis (o, v);
      break;
    case SET_DAMAGE:
      if (v > O_DAM_MAX)
	limit = O_DAM_MAX;
      else
	osetdamage (o, v);
      break;
    case SET_ARMOR:
      if (v > O_ARMOR_MAX)
	limit = O_ARMOR_MAX;
      else
	osetarmor (o, v);
      break;
    case SET_STATE:
      if (v > omaxstate (o))
	limit = omaxstate (o);
      else
	setobjstate (o, v);
      break;
    default:
      bprintf ("Internal error\n");
      mudlog ("ERROR: Internal errror in setcom(): p = %d", p);
      return;
    }

    if (limit > -1) {
      bprintf ("Sorry, value must be <= %d.\n", limit);
      return;
    }
  } else {
    char **q;

    if (opermanent (o)) {
      bprintf ("You can only change %s on non-permanent"
	       " (wiz-created) objects.\n", Props[p]);
      return;
    }
    q = p == SET_DESC0 ? &olongt (o, 0)
      : p == SET_DESC1 ? &olongt (o, 1)
      : p == SET_DESC2 ? &olongt (o, 2)
      : p == SET_DESC3 ? &olongt (o, 3)
      : p == SET_NAME ? &oname (o)
      : p == SET_ANAME ? &oaltname (o)
      : NULL;

    if (q == NULL) {
      bprintf ("Internal Error\n");
      mudlog ("ERROR: Internal error in setcom(), p = %d", p);
      return;
    }
    if (strchr (getreinput (wordbuf), '^') != NULL) {
      bprintf ("Illegal character(s) (^) in text.\n");
      return;
    }
    if (p == SET_NAME || p == SET_ANAME) {
      char *s = wordbuf;

      if (strlen (wordbuf) > ONAME_LEN) {
	bprintf ("Name too long. Max = %d chars.\n",
		 ONAME_LEN);
	return;
      }
      while (*s != '\0' && isalpha (*s))
	s++;

      if (*s != '\0') {
	bprintf ("Name must only contain latters.\n");
	return;
      }
      if (EMPTY (wordbuf)) {
        if (p == SET_ANAME) 
          ht_remove(oalts_n, oaltname(o), o); 
	if (p != SET_ANAME) {
	  bprintf ("Name is missing.\n");
	  return;
	}
      }
    }
    if (*q != NULL)
      free (*q);

    if (p == SET_ANAME && !EMPTY(wordbuf)) {
      ht_remove(oalts_n, oaltname(o), o);
      ht_add(oalts_n, wordbuf, o);
    }

    *q = COPY (wordbuf);
  }
  bprintf ("Ok.\n");
}

void exorcom () {
  int x, y = real_mynum;

  if (!ptstflg (mynum, PFL_EXOR)) {
    bprintf ("You couldn't exorcise your way out of a paper bag.\n");
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Exorcise who?\n");
    return;
  }
  if ((x = pl1) == -1) {
    bprintf ("They aren't playing.\n");
    return;
  }
  if (!do_okay (mynum, x, PFL_NOEXOR)) {
    bprintf ("They don't want to be exorcised.\n");
    return;
  }
  if (x < max_players)
    mudlog ("&+WSYSTEM:&N %s exorcised %s", pname (mynum), pname (x));

  send_msg (DEST_ALL, MODE_QUIET, pvis (x), LVL_MAX, x, NOBODY,
	    "%s has been kicked off.\n", pname (x));

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_WIZARD, LVL_MAX,
	mynum, x, "&+B[&+W\001p%s\003 &*has exorcised &+W\001p%s\003&+B]\n",
	    pname (mynum), pname (x));

  if (ststflg(x, SFL_OCCUPIED)) {
    sendf(x, "You have been kicked off!\n");
    unalias (x);
  }
  if (x >= max_players) {
    dumpstuff (x, ploc (x));
    setpfighting (x, -1);
    if (!ppermanent (x))
      destruct_mobile (x);
    else
      setpname (x, "");
  } 
  else {
    setup_globals (x);
    quit_msg("You have been kicked off!", NULL);
    quit_player(False);
    setup_globals (y);
  }
}


void setstart (void) {
  PERSONA d;
  int loc, p;
  char buff[80];
  char *who;
  char *where;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  else if (brkword () == -1) {
    who = pname (mynum);
    where = NULL;
  } 
  else if (getuaf (strcpy (buff, wordbuf), &d)) {
    who = d.ublock.pname;
    where = (brkword () == -1) ? NULL : wordbuf;

    if (!EQ (who, pname (mynum))) {
      if (!ptstflg (mynum, PFL_UAF)) {
	bprintf ("You can only set your own start-location.\n");
	return;
      }
      if (!do_okay_l (plev (mynum), d.ublock.plev, False)) {
	bprintf ("That is beyond your powers.\n");
	return;
      }
    }
  } 
  else if (brkword () == -1) {
    who = pname (mynum);
    where = buff;
  } 
  else {
    bprintf ("No such player: %s\n", buff);
    return;
  }

  if (where == NULL)
    loc = 0;
  else if ((loc = findroomnum (where)) == 0 || !exists (loc)) {
    bprintf ("No such location.\n");
    return;
  }

  if (loc)
    sprintf(buff, "%s@%s", lname(loc), zname(lzone(loc)));

  if ((p = fmbn(who)) > -1) {
    if (loc)
      phome(p) = COPY(buff);
    else {
      bprintf("Start location reset to default.\n");
      if (phome(p)) {
        FREE(phome(p));
        phome(p) = 0;
      }
      return;
    }
  }
  else {
    if (loc)
      d.ublock.phome = COPY(buff);
    else
      d.ublock.phome = 0;
    putuaf (&d);
  }
  bprintf ("Ok.\n");
}

void noshoutcom (){
  int x;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if ((x = pl1) == -1 || x == mynum) {
    if (ststflg (mynum, SFL_NOSHOUT)) {
      bprintf ("You can hear shouts again.\n");
      sclrflg (mynum, SFL_NOSHOUT);
      send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX, mynum,
      NOBODY, "&+B[&+W%s &*is listening to &+CShouts&+B]\n", pname (mynum));
    } else {
      bprintf ("From now on you won't hear shouts.\n");
      ssetflg (mynum, SFL_NOSHOUT);
      send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX, mynum,
		NOBODY, "&+B[&+W%s &*is no longer listening to &+CShouts&+B]\n", pname (mynum));
    }
  } else if (plev (x) >= LVL_WIZARD) {
    if (ststflg (x, SFL_NOSHOUT)) {
      bprintf ("%s can hear shouts again.\n", pname (x));
      sclrflg (x, SFL_NOSHOUT);
    } else {
      bprintf ("From now on %s won't hear shouts.\n", pname (x));
      ssetflg (x, SFL_NOSHOUT);
    }
  } else if (ststflg (x, SFL_NOSHOUT)) {
    bprintf ("%s is allowed to shout again.\n", pname (x));
    sclrflg (x, SFL_NOSHOUT);
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum,
     NOBODY, "&+B[&+W\001p%s\003 &*has made &+W%s &*able to &+CShout&+B]\n",
	      pname (mynum), pname (x));
  } else {
    bprintf ("From now on %s cannot shout.\n", pname (x));
    ssetflg (x, SFL_NOSHOUT);
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum,
    NOBODY, "&+B[&+W\001p%s\003 &*has made &+W%s &*unable to &+CShout&+B]\n",
	      pname (mynum), pname (x));
  }
}




void
showlocation (int o)
{
  int uc, gotroom;

  uc = 1;
  gotroom = 0;
  while (!gotroom) {
    switch (ocarrf (o)) {
    case IN_ROOM:
      o = oloc (o);
      gotroom = 1;
      break;
    case IN_CONTAINER:
      bprintf (" %cnside the %s", (uc ? 'I' : 'i'), oname (oloc (o)));
      uc = 0;
      o = oloc (o);
      break;
    case CARRIED_BY:
      bprintf (" %carried by %s", (uc ? 'C' : 'c'), pname (oloc (o)));
      uc = 0;
      o = ploc (oloc (o));
      gotroom = 1;
      break;
    case WORN_BY:
      bprintf (" %corn by %s", (uc ? 'W' : 'w'), pname (oloc (o)));
      uc = 0;
      o = ploc (oloc (o));
      gotroom = 1;
      break;
    case WIELDED_BY:
      bprintf (" %cielded by %s", (uc ? 'W' : 'w'), pname (oloc (o)));
      uc = 0;
      o = ploc (oloc (o));
      gotroom = 1;
      break;
    case BOTH_BY:
      bprintf (" %corn and wielded by %s", (uc ? 'W' : 'w'), pname (oloc (o)));
      uc = 0;
      o = ploc (oloc (o));
      gotroom = 1;
      break;
    default:
      bprintf ("\n");
      return;
    }
  }
  bprintf (" %cn ", (uc ? 'I' : 'i'));
  bprintf ("%s\n", showname (o));
}

void showitem (void) {
  int x, i;

  if (!ptstflg (mynum, PFL_STATS)) {
    erreval ();
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Show what?\n");
    return;
  }
  if ((x = fobn (item1)) == -1) {
    bprintf ("What's that?\n");
    return;
  }
  bprintf ("\nItem [%d]:  %s", int2idx(x, OBJ), oname (x));
  if (oaltname (x))
    bprintf (" (%s)", oaltname (x));

  bprintf ("\nLocation:   ");
  showlocation (x);

  if (olinked (x) != -1) {
    bprintf ("Linked to:   ");
    bprintf ("%s", oname (olinked (x)));
    showlocation (olinked (x));
  }
  bprintf ("Zone/Owner:  %s\n", zname (ozone (x)));

  bprintf ("\nState:      %-6d State Reset:   %-5d Max State:     %-6d"
           "Vis Level: %d\n",
	   state (x), state_reset(x), omaxstate (x), ovis (x));
  bprintf ("Damage:     %-6d Armor Class:   %-6dSize:          %d\n",
	   odamage (x), oarmor (x), osize (x));
  bprintf ("Base Value: %-6d Current Value: %-6d\n\n",
	   obaseval (x), ovalue (x));

  bprintf("Oflags:      ");
  show_bits(obits(x), oindex, Oflags, OFLAGS, False);

  bprintf("OflagsReset: ");
  show_bits(obits(x), oindex, Oflags, OFLAGS_RESET, False);

  if (otstbit(x, OFL_WEARABLE)) {
    bprintf("Aflags:      ");
    show_bits(obits(x), oindex, Aflags, AFLAGS, False);

    bprintf("AflagsReset: ");
    show_bits(obits(x), oindex, Aflags, AFLAGS_RESET, False);
  }

  bprintf ("\nState   Description:\n");

  for (i = 0; i < 4; i++) {
    if (olongt(x, i))
      bprintf ("[%d]     %s\n", i, olongt (x, i));
  }
}

void wizlock () {
  char s[100];
  int v;

  if (brkword () == -1) {
    if (!the_world->w_lock)
      bprintf ("The game is currently unlocked.\n");
    else
      bprintf ("The game is locked to level %d.\n", the_world->w_lock);
    return;
  }
  if (!ptstflg (mynum, PFL_LOCK) || (v = plev (mynum)) <= 0) {
    erreval ();
    return;
  }

  the_world->w_lock = atoi(txt1);
  sprintf (s, "&+W[SYSTEM] &+C: &*&$ now locked to level "
	      "%d.&#&#\n", the_world->w_lock);
  broad (s);

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_WIZARD, LVL_MAX,
	    mynum, NOBODY, "&+B[&+CWizlock &*by &+W\001p%s\003&+B]\n",
	    pname (mynum));
}

void warcom() {
  if (plev (mynum) < LVL_WIZARD)
    erreval ();
  else if (!the_world->w_peace)
    bprintf("The game is already at war.\n");
  else {
    the_world->w_peace = 0;
    broad ("The air of peace and friendship lifts.\n");
    mudlog("&+RWAR:&N set by player %s", pname(mynum));
  }
}

void peacecom () {
  int m;

  if (plev (mynum) < LVL_WIZARD)
    erreval ();
  else if (the_world->w_peace == 1)
    bprintf("The world is already at peace.\n");
  else {
    mudlog("&+RPEACE:&N set by player %s", pname(mynum));
    the_world->w_peace = 1;
    for (m = 0; m < max_players; m++)
      setpfighting (m, -1);
    broad ("Peace and tranquility descend upon the world.\n");
  }
}

void zapcom (void) {
  int vic, x;

  if (!ptstflg (mynum, PFL_ZAP)) {
    bprintf ("The spell fails.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("Zap who?\n");
    return;
  }
  if ((vic = pl1) == -1) {
    bprintf ("There is no one on with that name.\n");
    return;
  }
  send_msg(sendloc(vic), 0, pvis(vic), LVL_MAX, vic, NOBODY,
	  "\001cA massive lightning bolt strikes %s!\n\003", pname(vic));

  if (!do_okay(mynum, vic, PFL_NOZAP))
    sendf (vic, "%s casts a lightning bolt at you!\n",
	   see_player (vic, mynum) ? pname (mynum) : "Someone");
  else {
    if (vic < max_players) {
      mudlog ("&+WSYSTEM:&N %s zapped %s", pname (mynum), pname (vic));

      if (plev (vic) >= LVL_WIZARD)
	update_wizlist (pname (vic), LEV_MORTAL);

      send_msg(sendloc(vic), 0, pvis (vic), LVL_MAX, vic, NOBODY,
		"\001s%s\003%s has just died.\n\003",
		pname (vic), pname (vic));

      send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, vic, NOBODY,
		"&+B[&+W\001p%s\003 &*has &+Czapped &+W\001p%s\003&+B]\n",
		pname (mynum), pname (vic));

      x = real_mynum;
      setup_globals (vic);
      bprintf("&+WA massive lightning bolt strikes you between the eyes!\n"
              "%sYou have been utterly destroyed by \001p%s\003.\n",
               cur_player->cprompt, pname(x));
      quit_msg("Bye Bye.... Slain by a Thunderbolt", NULL);
      quit_player(False);
      deluaf(pname(vic));
      setup_globals (x);
    }
    else
      player_died (mynum, vic, VERB_ZAP);
  }
  send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, vic, NOBODY, 
    "\001dYou hear an ominous clap of thunder in the distance.\n\003");
}

void pzapcom (void) {
  int vic, x;

  if (!ptstflg (mynum, PFL_PZAP)) {
    bprintf ("Pardon?\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("P-Zap who?\n");
    return;
  }
  if ((vic = pl1) == -1) {
    bprintf ("There is no one on with that name.\n");
    return;
  }

  send_msg(sendloc(vic), 0, pvis (vic), LVL_MAX, vic, NOBODY,
	    "\001A\033[1m\003\001cA massive lightning "
	    "bolt strikes \003\001D%s\003\001c!\n\003\001A\033[0m\003",
	    pname (vic));

  if (!do_okay (mynum, vic, PFL_NOZAP))
    sendf (vic, "\001p%s\003 casts a lightning bolt at you!\n", pname(mynum));
  else {
    if (vic < max_players) {
      mudlog ("&+WSYSTEM:&N %s P-zapped %s", pname (mynum), pname (vic));
      send_msg(sendloc(vic), 0, pvis (vic), LVL_MAX, vic, NOBODY,
		"\001s%s\003%s has just died.\n\003",
		pname (vic), pname (vic));

      send_msg (DEST_ALL, MODE_NPFLAG | MP (PFL_PZAP), LVL_WIZARD, LVL_MAX, vic, NOBODY,
		"&+B[&+W\001p%s\003 &*has &+Czapped &+W\001p%s\003&+B]\n",
		pname (mynum), pname (vic));

      send_msg (DEST_ALL, MODE_PFLAG | MP (PFL_PZAP), LVL_WIZARD, LVL_MAX, vic, NOBODY,
		"&+B[&+W\001p%s\003 &*has &+CP-zapped &+W\001p%s\003&+B]\n",
		pname (mynum), pname (vic));

      x = real_mynum;
      setup_globals (vic);
      bprintf("&+WA massive lightning bolt strikes you between the eyes!\n"
              "%sYou have been utterly destroyed by \001p%s\003.\n",
               cur_player->cprompt, pname(x));
      quit_msg("Bye Bye.... Slain by a Thunderbolt", NULL);
      quit_player(False);
      setup_globals (x);
    } 
    else
      player_died (mynum, vic, VERB_ZAP);
  }
  send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, vic, NOBODY,
    "\001dYou hear an ominous clap of thunder in the distance.\n\003");
}

void
debugcom (void)
{
  return;
}

void toutcom (void) {
  int a;

  if (!ptstflg (mynum, PFL_TIMEOUT)) {
    bprintf ("Pardon?\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("Who?\n");
    return;
  }
  if ((a = fpbn(wordbuf)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  if (ptstflg (a, PFL_NOTIMEOUT) && a != mynum && (plev(mynum) < plev(a))) {
    bprintf ("Something prevents you from doing this to that person.\n");
    return;
  }
  mudlog ("&+WSYSTEM:&N %s has tout'd %s", pname (mynum), pname (a));

  send_msg (DEST_ALL, MODE_PFLAG | MP (PFL_TIMEOUT), LVL_WIZARD, LVL_MAX, a, mynum,
	    "&+B[&+W\001p%s\003 &+whas tout'd &+W\001p%s\003&+B]\n",
	    pname (mynum), pname (a));

  send_msg (DEST_ALL, MODE_NPFLAG | MP (PFL_TIMEOUT), LVL_MIN, LVL_MAX, a, mynum,
	    "&+B[&+W\001p%s\003 &+whas been timed-out&+B]\n", pname (a));

  setup_globals(a);
  quit_player(-5);
}

void auto_tout(void) {
  if (!is_in_game(mynum))
    return;

  send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, mynum,
            "&+B[&+W\001p%s\003 &+whas been auto-touted%s&+B]\n",
            pname(mynum), linkdead(mynum) ? " (linkdead)" : "");
  mudlog("&+WSYSTEM:&N %s auto-touted%s", pname(mynum), 
    linkdead(mynum) ? " (linkdead)" : "");
  quit_player(-5);
}

void
nowishcom (void)
{
  int a;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (brkword () != -1) {
    if ((a = fpbn(wordbuf)) == -1) {
      bprintf ("Cannot find that player.\n");
      return;
    }
    if (plev (a) < LVL_WIZARD) {
      if (ststflg (a, SFL_NOWISH)) {
	bprintf ("%s can now wish for things.\n", pname (a));
	sendf (a, "You are hit by a bolt of light and feel you've regained your link to the Gods.\n");
	sclrflg (a, SFL_NOWISH);
	send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
		  "&+B[&+W\001p%s\003 &*has made %s able to &+CWish&+B]\n",
		  pname (mynum), pname (a));
      } 
      else {
	bprintf ("%s can no longer wish for things.\n", pname (a));
	sendf (a, "You are hit by a bolt of light and feel you've lost your link to the Gods.\n");
	ssetflg (a, SFL_NOWISH);
	send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
		  "&+B[&+W\001p%s\003 &*has made %s unable to &+CWish&+B]\n",
		  pname (mynum), pname (a));
      }
      return;
    }
    else {
      bprintf ("You can't seem to be able to make a Wizard not listen to wishes.\n");
      return;
    }
  }
  if (ststflg (mynum, SFL_NOWISH)) {
    bprintf ("You are once again listening to wishes.\n");
    sclrflg (mynum, SFL_NOWISH);
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	      "&+B[&+W\001p%s\003 &*is now listening to &+CWishes&+B]\n", pname (mynum));
    return;
  } 
  else {
    bprintf ("You will no longer hear wishes.\n");
    ssetflg (mynum, SFL_NOWISH);
    send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	      "&+B[&+W\001p%s\003 &*is no longer listening to &+CWishes&+B]\n", pname (mynum));
    return;
  }
}

void
noslaincom (void)
{
  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (ststflg (mynum, SFL_NOSLAIN)) {
    bprintf ("You will see slain messages again.\n");
    sclrflg (mynum, SFL_NOSLAIN);
    return;
  } else {
    bprintf ("You will no longer see slain messages.\n");
    ssetflg (mynum, SFL_NOSLAIN);
    return;
  }
}

void puntcom (void) {
  int b, i;
  int puntroom = -1;
  int ok = 1;
  int me = real_mynum;

  if (!ptstflg (mynum, PFL_PUNT)) {
    bprintf ("Yeah, you WISH you could punt!\n");
    return;
  }
  if (pl1 == -1) {
    bprintf ("Who do you want to punt?\n");
    return;
  }
  if (!EMPTY (item2)) {
    if ((puntroom = findroomnum (item2)) == 0) {
      bprintf ("Location does not exist!\n");
      return;
    } 
    else
      ok = 0;
  }
  if ((b = pl1) == -1) {
    bprintf("I can't find that person.\n");
    return;
  }
  if (b >= num_const_chars) {
    bprintf("You can't punt around cloned mobiles.\n");
    return;
  }
  if (b == mynum) {
    bprintf ("You really want to punt yourself around? Well, have a nice "
	     "flight!\n");
  }
  if (b != mynum) {
    if (ptstflg (b, PFL_NOPUNT) && plev (mynum) < LVL_GOD) {
      bprintf ("A magical force prevents you from punting that person.\n");
      return;
    }
  }
  if (ok == 0) {
    if (chkroom (puntroom, b)) {
      bprintf ("Something will not let you punt %s there.\n", b == mynum ?
	       "yourself" : pname (b));
      return;
    }
    if (ltstflg (puntroom, LFL_PRIVATE)) {
      i = count_players (puntroom, COUNT_PLAYERS | COUNT_MOBILES); 
      if (i > 1) {
         bprintf ("I'm sorry.  There's a private conference in that room\n");
         return;
      }
    }
  }
  while (ok != 0) {
    puntroom = my_random () % num_const_locs;	/* Get a random room    */

    if (ltstflg (puntroom, LFL_DEATH) ||	/* Cannot be deathroom  */
	ltstflg (puntroom, LFL_ON_WATER) ||	/* Cannot be on water   */
	ltstflg (puntroom, LFL_ONE_PERSON) ||	/* Cannot be one-person */
	ltstflg (puntroom, LFL_PRIVATE) ||	/* Cannot be private    */
	r_isdark (puntroom, b) ||	        /* Cannot be dark       */
	chkroom (puntroom, b))
      ok = 1;
    else {
      ok = 0;
    }
  }
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && i != mynum && i != b && ploc (b) == ploc (i)) {

      if (b != mynum) {
	if (ploc (mynum) == ploc (b))
	  sendf (i, "%s grins evilly then picks up %s and boots %s across "
		 "the world..\n", see_name (i, mynum), see_name (i, b),
		 him_or_her (b));
	else
	  sendf (i, "%s appears, picks up %s and boots %s across the "
		 "world..\n", see_name (i, mynum), see_name (i, b),
		 him_or_her (b));
      } else {
	sendf (i, "\001p%s\003 boots %s across the world..\n", pname (mynum),
	       psex (mynum) ? "herself" : "himself");
      }
    }
    if (is_in_game (i) && i != mynum && i != b && ploc (b) != ploc (i))
      sendf (i, "%s flies overhead screaming for mercy..\n", see_name (i, b));
  }

  if (b != mynum) {
    bprintf ("You pick up %s and boot %s across the world..\n", pname (b),
	     him_or_her (b));
    sendf (b, "%s grins at you then picks you up and boots you across the "
	   "world..\n", see_name (b, mynum));
  } else {
    bprintf ("You pick yourself up and boot yourself around the world..\n");
  }

  if (b < max_players) {
    setup_globals (b);
    setploc (mynum, puntroom);
    setup_globals (me);
  } else
    setploc (b, puntroom);

  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && i != b && ploc (b) == ploc (i))
      sendf (i, "%s lands on the ground with a loud thud.\n", see_name (i, b));
  }

  if (EMPTY(item2))
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_WIZARD, LVL_MAX, b, 
              mynum, "&+B[&+W\001p%s\003 &+whas punted &+W\001p%s\003&+B]\n",
	      pname (mynum), pname (b));
  else
    send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_WIZARD, LVL_MAX, b, 
          mynum, 
          "&+B[&+W\001p%s\003 &+whas punted &+W\001p%s\003&+w to %s (%s)&+B]\n",
          pname (mynum), pname (b), sdesc (puntroom), showname(puntroom));

}
void puntallcom (void) {
  int i;
  int b = -1;
  int puntroom = -1;
  int ok = 1;
  int me = real_mynum;

  if (!ptstflg (mynum, PFL_PUNTALL)) {
    bprintf ("Yeah, you WISH you could puntall!\n");
    return;
  }
  if (!EMPTY (item1)) {
    if ((puntroom = findroomnum (item1)) == 0) {
      bprintf ("Location does not exist!\n");
      return;
    } 
    else
      ok = 0;
  }
  if (ok == 0) {
    for (i = 0; i < max_players; ++i) {
      if (is_in_game (i) && i != mynum &&
	  wlevel (plev (mynum)) < wlevel (plev (b))) {
	if (chkroom (puntroom, i)) {
	  bprintf ("Something will not let you puntall there.\n");
	  return;
	}
      }
    }
  }
  send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
	    "\001p%s\003 gets ready to punt all of you around!\n",
	    pname (mynum));
  bprintf ("You punt everyone around the world!\n");

  if (ok == 0) {
    for (i = 0; i < max_players; ++i) {
      if (is_in_game (i) && i != mynum && plev (i) < LVL_CREATOR) {
	sendf (i, "\001p%s\003 grins evilly and boots you across the world.\n",
	       pname (mynum));
	setup_globals (i);
	setploc (mynum, puntroom);
	setup_globals (me);
      }
    }
  }
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && i != mynum && plev (i) < LVL_CREATOR) {
      ok = 1;
      while (ok != 0) {
	puntroom = my_random () % num_const_locs;	/* Get a random room    */

	if (ltstflg (puntroom, LFL_DEATH) ||
	    ltstflg (puntroom, LFL_ON_WATER) ||
	    ltstflg (puntroom, LFL_ONE_PERSON) ||
	    ltstflg (puntroom, LFL_PRIVATE) ||
	    r_isdark (puntroom, i) ||
	    chkroom (puntroom, i))
	  ok = 1;
	else {
	  ok = 0;
	  sendf (i, "\001p%s\003 grins evilly and boots you across the "
		 "world.\n", pname (mynum));
	  setup_globals (i);
	  setploc (mynum, puntroom);
	  setup_globals (me);
	}
      }
    }
  }

  send_msg (DEST_ALL, MODE_SFLAG | MS (SFL_SEEEXT), LVL_WIZARD, LVL_MAX, b, mynum,
	    "&+B[&+CPuntAll &*by &+W\001p%s\003&+B]\n", pname (mynum));

}

void
litcom ()
{
  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (!ststflg (mynum, SFL_LIT)) {
    ssetflg (mynum, SFL_LIT);
    bprintf ("You are now lit.\n");
  } else {
    sclrflg (mynum, SFL_LIT);
    bprintf ("You are no longer lit.\n");
  }
}

void socketcom () {
  static char *SockTable[] =
  {
    "view", "kill", "write", TABLE_END
  };

  int i, b, x, d;
  int old_mynum = mynum;

  Boolean noton = False;
  char idlebuff[64], loginbuff[64];
  char txt[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_SOCKET)) {
    bprintf ("Pardon?\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("&+CName         Flushed   FD      Idle    On For  Host\n");
    bprintf ("&+B-------------------------------------------------------------------------------\n");

    for (i = 0; i < max_players; ++i) {
      if (see_player (mynum, i) && is_conn (i)) {
	strcpy (idlebuff, sec_to_hhmmss (global_clock - prlast_cmd (i)));
	strcpy (loginbuff, sec_to_hhmmss (global_clock - plogged_on (i)));
	bprintf ("&+w%-12s %7s   %2d  %8.8s  %8.8s  %-32.32s\n",
		 pname (i), iplrs[i].output ? "N" : "Y",
                 fildes(i), idlebuff, loginbuff, host_name(i));
      }
    }
    bprintf ("&+B-------------------------------------------------------------------------------\n");
    return;
  }
  if ((d = atoi (wordbuf)) == 0) {
    if ((b = fpbn(wordbuf)) == -1) {
      bprintf ("Person not on-line.\n");
      return;
    }
    d = fildes(b);
  } else {
    if ((b = find_pl_index (d)) == -1) {
      bprintf ("That descriptor is not in use.\n");
      return;
    }
    if (!is_in_game (b)) {
      if (!is_conn (b)) {
	bprintf ("That descriptor is not in use.\n");
	return;
      }
      noton = True;
    }
  }

  if (is_in_game (b) && (pvis (b) > plev (mynum))) {
    bprintf ("That descriptor is not in use.\n");
    return;
  }
  if (brkword () == -1) {
    if (noton)
      bprintf ("What do you want to do with descriptor %d?\n", d);
    else
      bprintf ("What do you want to do with %s's descriptor?\n", pname (b));
    return;
  }
  if ((x = tlookup (wordbuf, SockTable)) < 0) {
    bprintf ("What are you trying to do?\n");
    return;
  }
  switch (x) {
  case 0:
    strcpy (idlebuff, sec_to_hhmmss (global_clock - prlast_cmd (b)));
    strcpy (loginbuff, sec_to_hhmmss (global_clock - plogged_on (b)));
    bprintf ("&+CDescriptor &+B: &+W%-2d\n", d);
    bprintf ("&+B----------------------------------\n");
    bprintf ("&+wUser Name  &+B: &+w%s\n", pname (b));
    bprintf ("&+wHostname   &+B: &+w%s\n",
	     !ptstflg (mynum, PFL_SEEUSER) ? hostname(b) : username(b));
    bprintf ("&+wIdle Time  &+B: &+w%s\n", idlebuff);
    bprintf ("&+wLogged On  &+B: &+w%s\n", loginbuff);
    bprintf ("&+B----------------------------------\n");
    break;
  case 1:
    bprintf ("Killing Descriptor %d (User: %s)\n", d, pname (b));
    mudlog ("SOCKET: %s killed descriptor %d (User: %s)", pname (mynum), d, 
             pname (b));

    if (noton)
      send_msg (DEST_ALL, MODE_PFLAG | MP (PFL_SOCKET), LVL_WIZARD, LVL_MAX, b,
		mynum, "&+W[&+CSocket: &+w\001p%s\003 has killed descriptor "
		"%d (Logging In)&+W]\n", pname (mynum), d);
    else
      send_msg (DEST_ALL, MODE_PFLAG | MP (PFL_SOCKET), LVL_WIZARD, LVL_MAX, b,
		mynum, "&+W[&+CSocket: &+w\001p%s\003 has killed descriptor "
		"%d (User: \001p%s\003)&+W]\n", pname (mynum), d, pname (b));

    setup_globals (b);
    quit_player(False);

    setup_globals (old_mynum);
    break;
  case 2:
    if (noton)
      bprintf ("Writing text to descriptor %d.\n", d);
    else
      bprintf ("Writing text to %s's descriptor.\n", pname (b));

    getreinput (txt);
    setup_globals (b);
    bprintf ("%s\n", txt);
    setup_globals (old_mynum);
    break;
  }
}

void
burncom (void)
{
  int vic, x = real_mynum;

  if (!ptstflg (mynum, PFL_BURN)) {
    bprintf ("The spell fails.\n");
    return;
  }
  if (brkword () == -1) {
    bprintf ("Burn who?\n");
    return;
  }
  if ((vic = pl1) == -1) {
    bprintf ("There is no one on with that name.\n");
    return;
  }
  if (!do_okay (mynum, vic, PFL_NOBURN)) {
    bprintf ("You can't send that person to hell!\n");
    return;
  }
  send_msg (DEST_ALL, 0, pvis (vic), LVL_MAX, vic, NOBODY,
	    "Screams are heard in the distance as %s is sent to hell..\n",
	    pname (vic));

  sendf (vic, "You are ripped up and thrown into hell by \001p%s\003..\n",
	 pname (mynum));

  if (vic < max_players)
    mudlog ("BURN: %s has burned %s", pname (mynum), pname (vic));

  send_msg (DEST_ALL, 0, LVL_MIN, LVL_MAX, vic, NOBODY,
	    "&+W\001p%s\003 &*has &+Rpissed off &*the &+WGods&*, and has been sent to burn in &+rhell&*!\n",
	    pname (vic));

  send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, vic, NOBODY,
	    "&+B[&+W\001p%s\003 &+whas burned &+W\001p%s\003&+B]\n",
	    pname (mynum), pname (vic));


  if (vic < max_players) {
    setplev (vic, 1);
    setpscore (vic, 0);
    set_xpflags(mbits(vic), 1);
    setploc (vic, LOC_LIMBO_HELL);

    setup_globals (vic);
    trapch (ploc (mynum));
    setup_globals (x);

  }
}

void
follist (void)
{
  int i, j;
  int a[max_players], a_len = 0;
  int me = real_mynum;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  for (i = 0; i < max_players; ++i)
    if (is_in_game (i) && (pvis (i) <= plev (me) || i == me) && pfollow (i) != -1)
      a[a_len++] = i;

  if (a_len == 0) {
    bprintf ("&+WThere are currently no players using follow.\n");
    return;
  }
  qsort (a, a_len, sizeof (int), cmp_player);

  bprintf ("&+WName            Str/Max DP AC           Following        Str/Max DP AC\n");
  bprintf ("&+B-----------------------------------------------------------------------\n");
  for (i = 0; i < a_len; ++i) {
    j = pfollow (a[i]);
    bprintf ("&+w%-15s %3d/%-3d %2d %2d           %-15s %3d/%-3d %2d %2d\n",
	pname (a[i]), pstr (a[i]), maxstrength (a[i]), player_damage (a[i]),
	     pac (a[i]), pname (j), pstr (j), maxstrength (j),
	     player_damage (j), pac (j));
  }

  bprintf ("&+B-----------------------------------------------------------------------\n");
}

void
toggleseeext (void)
{

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (!ststflg (mynum, SFL_SEEEXT)) {
    ssetflg (mynum, SFL_SEEEXT);
    bprintf ("Extended Messages Enabled\n");
  } else {
    sclrflg (mynum, SFL_SEEEXT);
    bprintf ("Extended Messages Disabled\n");
  }

}

void findcom (void) {
  int i;
  int plr = 0, mob = 0, obj = 0, loc = 0;
  char sst[MAX_COM_LEN];
  char tmp[256];

  getreinput (sst);

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("Pardon?\n");
    return;
  }
  if (EMPTY (txt1)) {
    bprintf ("What text are you looking for?\n");
    return;
  }
  bprintf ("&+WType  %-40.40s   %-18.18s  Number\n", "Name", "Location");
  bprintf ("&+B-------------------------------------------------------------------------------\n");

  for (i = 0; i < max_players; i++) {
    strcpy (tmp, pname (i));
    if (strstr (uppercase (tmp), uppercase (sst)) && see_player (mynum, i)) {
      ++plr;
      if (plev(mynum) > LVL_GOD)
        bprintf ("&+W(Plr) &+w%-40.40s   %-18.18s  %d\n",
  	         pname (i), showname (ploc (i)), i);
      else
        bprintf ("&+W(Plr) &+w%-40.40s   %-18.18s  N/A\n",
  	         pname (i), showname (ploc (i)));
    
    }
  }

  for (i = max_players; i < numchars; i++) {
    strcpy (tmp, pname (i));
    if (strstr (uppercase (tmp), uppercase (sst)) && see_player (mynum, i)) {
      ++mob;
      bprintf ("&+W(Mob) &+w%-40.40s   %-18.18s  %d\n",
	       pname (i), showname (ploc (i)), int2idx(i, MOB));
    }
  }

  for (i = 0; i < numobs; ++i) {
    strcpy (tmp, oname (i));
    if (strstr (uppercase (tmp), uppercase (sst)) && (ovis (i) <= plev (mynum))) {
      ++obj;
      bprintf ("&+W(Obj) &+w%-40.40s   %-18.18s  %d\n", oname (i), 
        odescrm (i), int2idx(i, OBJ));
    }
  }

  for (i = 0; i < numloc; i++) {
    strcpy(tmp, sdesc(i));
    if (strstr(uppercase(tmp), uppercase(sst)))
      bprintf("&+W(Loc) &+w%-40.40s   %-18.18s  %d\n", sdesc(i), 
        showname(i), int2idx(i, LOC));
  }
  bprintf ("&+B-------------------------------------------------------------------------------\n");

  if (plr)
    bprintf ("&+wPlayers: &+W%d  ", plr);
  if (mob)
    bprintf ("&+wMobiles: &+W%d  ", mob);
  if (obj)
    bprintf ("&+wObjects: &+W%d  ", obj);
  if (loc)
    bprintf ("&+wLocations: &+W%d  ", loc);

  if (!plr && !mob && !obj && !loc)
    bprintf ("&+wNothing was found with that text.");

  bprintf ("\n");
}

void
nopuntcom (void)
{
  if (!ptstmsk (mynum, PFL_NOPUNT)) {
    erreval ();
    return;
  }
  if (ptstflg (mynum, PFL_NOPUNT)) {
    bprintf ("NoPunt Disabled\n");
    pclrflg (mynum, PFL_NOPUNT);
  } else {
    bprintf ("NoPunt Enabled\n");
    psetflg (mynum, PFL_NOPUNT);
  }
}

void
fakequitcom (void)
{
  static char *QuitTableG[] =
  {
    "list", "quit", "cut", "net1", "net2", "net3", "net4",
    "net5", "net6", "net7", TABLE_END
  };

  static char *QuitTable[] =
  {
    "list", "quit", TABLE_END
  };

  static int vislev[] =
  {
    0, LVL_ISTARI, LVL_ISTARI, LVL_ISTARI,
    LVL_ISTARI, LVL_ARCHWIZARD, LVL_DEMI,
    LVL_SHALAFI, LVL_GOD, LVL_CREATOR, LVL_MAX
  };


  int x, maxlev = vislev[wlevel (plev (mynum))];
  char xx[SETIN_MAX + 100];

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("Pardon?\n");
    return;
  }
  if (brkword () == -1) {
    x = 1;
  } else {
    if (plev (mynum) < LVL_GOD) {
      if ((x = tlookup (wordbuf, QuitTable)) < 0) {
	bprintf ("Use which message? (LIST for listing)\n");
	return;
      }
    } else {
      if ((x = tlookup (wordbuf, QuitTableG)) < 0) {
	bprintf ("Use which message? (LIST for listing)\n");
	return;
      }
    }
  }

  switch (x) {
  case 0:
    bprintf ("FakeQuit Listing\n");
    bprintf ("--------------------------------------\n");
    bprintf ("QUIT : Regular quit message\n");

    if (plev (mynum) >= LVL_GOD) {
      bprintf ("CUT  : Connection lost (cut) message\n");
      bprintf ("NET1 : Connection Reset by Peer (ECONNRESET)\n");
      bprintf ("NET2 : No Route to Host (EHOSTUNREACH)\n");
      bprintf ("NET3 : Connection Timed Out (ETIMEDOUT)\n");
      bprintf ("NET4 : Network Unreachable (ENETUNREACH)\n");
      bprintf ("NET5 : Network Dropped Connection on Reset (ENETRESET)\n");
      bprintf ("NET6 : Network is Down (ENETDOWN)\n");
      bprintf ("NET7 : Empty Packets\n");
    }
    return;
  case 1:
    send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	      "%s\n", build_setin (SETIN_SETQOUT, xx, cur_player->setqout, pname (mynum),
				   NULL, NULL));

    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD),
	      LVL_MAX, mynum, NOBODY, "&+B[&+CQuitting Game: &+W%s&+B]\n", pname (mynum));

    break;
  case 2:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY,
	      "&+W[&+CSocket: &+C%s &+whas lost (cut) connection&+W]\n",
	      pname (mynum));
    break;
  case 3:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
    "&+B(&+WConnection Reset by Peer (ECONNRESET)&+B)&+W]\n", pname (mynum));
    break;
  case 4:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
	 "&+B(&+WNo Route to Host (EHOSTUNREACH)&+B)&+W]\n", pname (mynum));
    break;
  case 5:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
	"&+B(&+WConnection Timed Out (ETIMEDOUT)&+B)&+W]\n", pname (mynum));
    break;
  case 6:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
       "&+B(&+WNetwork Unreachable (ENETUNREACH)&+B)&+W]\n", pname (mynum));
    break;
  case 7:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
	      "&+B(&+WNetwork Dropped Connection on Reset (ENETRESET)&+B)&+W]\n", pname (mynum));
    break;
  case 8:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
	      "&+B(&+WNetwork is Down (ENETDOWN)&+B)&+W]\n", pname (mynum));
    break;
  case 9:
    send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
	      mynum, NOBODY, "&+W[&+CSocket: &+C%s &+whas lost link "
	      "&+B(&+WEmpty Packets&+B)&+W]\n", pname (mynum));
    break;
  default:
    bprintf ("What exactly are you trying to do?\n");
    return;
  }

  send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD),
	    LVL_MAX, mynum, NOBODY,
	 "&+B[&+W%s &*has departed from the MUD in &+C%s &*(&+C%s&*)&+B]\n",
	  pname (mynum), sdesc (ploc (mynum)), showname (ploc (mynum)));

  setpvis (mynum, maxlev);

  bprintf ("You have faked quitting the game.\n");

  send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD),
  LVL_MAX, mynum, NOBODY, "&+B[&+CFakequit &*by &+W%s&+B]\n", pname (mynum));
}

void 
siccom (void) 
{
  int mob, plr;

  if (!ptstflg (mynum, PFL_SIC)) {
    erreval ();
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Sic what mobile on what player?\n");
    return;
  }
  if ((mob = pl1) == -1) {
    bprintf ("That mobile can't be found.\n");
    return;
  }
  if (mob < max_players) {
    bprintf ("You can't sic a player on a player!\n");
    return;
  }
  if (EMPTY (item2)) {
    bprintf ("Sic %s on what player?\n", pname (mob));
    return;
  }
  if ((plr = pl2) == -1) {
    bprintf ("That player can't be found.\n");
    return;
  }
  if (plr >= max_players) {
    bprintf ("You can't sic a mobile on a mobile!\n");
    return;
  }
  if (!do_okay (mynum, plr, PFL_NOSIC)) {
    bprintf ("You can't sic a mobile on that person.\n");
    return;
  }
  if (ltstflg (ploc (plr), LFL_PEACEFUL)) {
    bprintf ("It's too peaceful where %s is to use sic.\n", pname (plr));
    return;
  }
  bprintf ("You sic %s on %s.\n", pname (mob), pname (plr));

  send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
	    "&+B[&+CSic &*by &+W\001p%s\003&*: &+W\001p%s\003 &*on &+W\001p%s\003&+B]\n",
	    pname (mynum), pname (mob), pname (plr));

  setploc (mob, ploc (plr));
  sendf (plr, "\001p%s\003 appears and begins to attack you!\n", pname (mob));
  setpangry (mob, plr);
  setpangry (plr, mob);
  hit_player (mob, plr, -1);
}

void
atviscom (void)
{
  static int vislev[] =
  {
    0, LVL_ISTARI, LVL_ISTARI, LVL_ISTARI,
    LVL_ISTARI, LVL_ARCHWIZARD, LVL_DEMI,
    LVL_SHALAFI, LVL_GOD, LVL_CREATOR, LVL_MAX
  };

  int vis, oldvis, maxvis;
  char com[MAX_COM_LEN];

  oldvis = pvis(mynum);
  maxvis = vislev[wlevel (plev (mynum))];

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf ("What visibility level do you want to use?\n");
    return;
  }
  if ((vis = atoi (wordbuf)) == 0) {
    bprintf ("Invalid Visibility Value!\n");
    return;
  }
  getreinput (com);

  if (vis > maxvis) {
    bprintf ("You can't go invisible to that level! Setting you to %d.\n", maxvis);
    vis = maxvis;
  }
  setpvis (mynum, vis);
  gamecom (com, False);
  setpvis (mynum, oldvis);
}

void
writelog (void)
{
  char data[MAX_COM_LEN];

  if (!ptstflg (mynum, PFL_CANLOG)) {
    erreval ();
    return;
  }
  getreinput (data);

  if (EMPTY (data)) {
    bprintf ("What do you want to log?\n");
    return;
  }
  mudlog ("%s", data);
}

void
freaqcom (void)
{
  int b;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (EMPTY (item1)) {
    bprintf ("Who is a Frea&+GQ&*?\n");
    return;
  }
  if ((b = pl1) == -1) {
    bprintf ("Who is that?\n");
    return;
  }
  if (b >= max_players) {
    bprintf ("You can't make a mobile a Frea&+GQ&*!\n");
    return;
  }
  if (wlevel (plev (mynum)) < wlevel (plev (b))) {
    bprintf ("You can't make that player a Frea&+GQ&*!\n");
    return;
  }
  if (b == mynum) {
    if (ststflg (mynum, SFL_FREAQ)) {
      bprintf ("You don't feel like a Frea&+GQ&* anymore.\n");
      sclrflg (mynum, SFL_FREAQ);
      psetflg (mynum, PFL_TITLES);
    } else {
      bprintf ("You start to Frea&+GQ&*out!\n");
      ssetflg (mynum, SFL_FREAQ);
      pclrflg (mynum, PFL_TITLES);
    }
    return;
  }
  if (ststflg (b, SFL_FREAQ)) {
    bprintf ("You return %s to normal.\n", pname (b));
    sendf (b, "You don't feel like a Frea&+GQ&* anymore.\n");
    send_msg (DEST_ALL, MODE_QUIET, max (LVL_WIZARD, pvis (b)), LVL_MAX,
	      mynum, b, "&+B[&+CFreaQ: &+W\001p%s\003 &*has made &+W%s "
	      "&*normal&+B]\n", pname (mynum), pname (b));
    mudlog ("FREAQ: %s made %s normal", pname (mynum), pname (b));
    psetflg (b, PFL_TITLES);
    sclrflg (b, SFL_FREAQ);
  } else {
    bprintf ("You turn %s into a FreaQ.\n", pname (b));
    sendf (b, "You begin to feel like a Frea&+GQ&*.\n");
    send_msg (DEST_ALL, MODE_QUIET, max (LVL_WIZARD, pvis (b)), LVL_MAX,
	      mynum, b, "&+B[&+CFreaQ: &+W\001p%s\003 &*has made &+W%s "
	      "&*a FreaQ&+B]\n", pname (mynum), pname (b));
    mudlog ("FREAQ: %s made %s a FreaQ", pname (mynum), pname (b));
    pclrflg (b, PFL_TITLES);
    ssetflg (b, SFL_FREAQ);
  }
}
