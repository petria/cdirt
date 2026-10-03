#include "kernel.h"
#include "sendsys.h"
#include "mobile.h"
#include "bprintf.h"
#include "mud.h"
#include "commands.h"
#include "log.h"

#include <stdarg.h>

void sillycom(char *txt) {
  send_msg(sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY, txt,
           pname(mynum), pname(mynum));
}

extern void noswear(const char *);

struct _send_msg_box {
  int mode;
  int min;
  int max;
  int x1;
  int x2;
  int lang;
};

void p_sendto (int to, char *text) {
  Boolean newstyle;

  if (!is_in_game(to) || !text)
    return;

  setup_globals (to);
  newstyle = ststflg(mynum, SFL_NEWSTYLE);

  if (cur_player->in_cmd) {
    if (newstyle)
      bprintf ("%s", text);
    else
      bprintf ("\n%s", text);
  }
  else {
    if (!ststflg(mynum, SFL_NOSWEAR)) 
      bprintf ("%c%s\r%s", newstyle ? '\r' : '\n', text, cur_player->cprompt);
    else
      noswear(text);
  }
}

/*
 * Values for destination:
 * Unified identifiers from int2idx(index, LOC/CHAR/OBJ).
 * For a character room use sendloc(character), not the raw ploc index.
 * sendf and sendl accept raw indexes and encode them internally.
 * DEST_ALL                    all players and mobiles.
 */

static Boolean
test_rcv (int player,		/* Who to send to */
	  int mode,		/* Flags to control sending */
	  int min,		/* Minimum level of recipient */
	  int max,		/* Maximum level of recipient */
	  int x1,		/* Do not send to him */
	  int x2,		/* Nor to him */
	  int lang)
{				/* Language */
  Boolean b;

  if (player == x1 || player == x2)
    return False;
  b = (plev (player) >= min && plev (player) < max);

  switch (lang & (MODE_LANG | MODE_NLANG)) {
  case MODE_LANG:
    b = b && ntstflg (player, lang & MODE_FLAGS);
    break;
  case MODE_NLANG:
    b = b && !ntstflg (player, lang & MODE_FLAGS);
    break;
  }

  if (mode & MODE_NOBLIND)
    b = b && !ststflg (player, SFL_BLIND);
  if (mode & MODE_NODEAF)
    b = b && !ststflg (player, SFL_DEAF);
  if (mode & MODE_NODUMB)
    b = b && !ststflg (player, SFL_DUMB);
  if (mode & MODE_NOCRIP)
    b = b && !ststflg (player, SFL_CRIPPLED);
  if (mode & MODE_QUIET)
    b = b && !ststflg (player, SFL_QUIET);
  if (mode & MODE_OUTDOORS)
    b = b && ltstflg (ploc (player), LFL_OUTDOORS);

  switch (mode & (MODE_PFLAG | MODE_NPFLAG)) {
  case MODE_PFLAG:
    b = b && ptstflg (player, mode & MODE_FLAGS);
    break;
  case MODE_NPFLAG:
    b = b && !ptstflg (player, mode & MODE_FLAGS);
    break;
  }

  switch (mode & (MODE_SFLAG | MODE_NSFLAG)) {
  case MODE_SFLAG:
    b = b && ststflg (player, (mode >> MODE_S) & MODE_FLAGS);
    break;
  case MODE_NSFLAG:
    b = b && !ststflg (player, (mode >> MODE_S) & MODE_FLAGS);
    break;
  }

  return b;
}

void trapfunc(void)
{
}

void send_g_msg (int sendto,
	    char *func (int plx, intptr_t arg, char *t), /* Test function */
	    intptr_t arg,					/* Argument to test */
	    char *text)					/* Text to send */
{
  char *t;
  int p, dest;
  int me = real_mynum;
  int type;

  if (sendto == DEST_ALL) {
    type = DEST_ALL;
    dest = -1;
  }
  else {
    if ((dest = idx2int(sendto, MOB)) != -1)            /* mob */
      type = MOB;
    else if ((dest = idx2int(sendto, LOC)) != -1)       /* loc */
      type = LOC;
    else if ((dest = idx2int(sendto, OBJ)) != -1)    /* plr who holds obj */
      type = OBJ;
    else
      return;
  }
      
  if (type == MOB) {
    t = (func == NULL ? text : func(dest, arg, text));

    if (is_in_game(dest)) {
      if (dest < max_players)
        p_sendto(dest, t);
      else if (ststflg(dest, SFL_OCCUPIED))
        p_sendto(aliased_mob(dest), t);
    }
  } 
  else if (type == LOC || type == DEST_ALL || type == OBJ) {  /* room/all */
    if (type == OBJ) {
      while (ocarrf(dest) != IN_ROOM && ocarrf(dest) < CARRIED_BY)
        dest = oloc(dest);

      if (ocarrf(dest) == IN_ROOM)
        dest = oloc(dest);
      else
        dest = ploc(oloc(dest));
    }

    for (p = 0; p < max_players; ++p)
      if (is_in_game (p) && !aliased(p))
        if (type == DEST_ALL || dest == ploc(p)) {
          t = (func == NULL ? text : func (p, arg, text));
  	  p_sendto (p, t);
        }
  }
  setup_globals (me);
}

char *
check_send_msg (int plx, intptr_t a, char *t)
{
  struct _send_msg_box *b = (struct _send_msg_box *) a;

  if (test_rcv (plx, b->mode, b->min, b->max, b->x1, b->x2, b->lang))
    return t;
  return NULL;
}

void
send_msg (int destination,	/* Where to send to */
	  int mode,		/* Flags to control sending */
	  int min,		/* Minimum level of recipient */
	  int max,		/* Maximum level of recipient */
	  int x1,		/* Do not send to him */
	  int x2,		/* Nor to him */
	  char *format,...)
{				/* Format with args -> text to send */
  va_list pvar;
  char bf[2048];
  struct _send_msg_box b;
  char *bb;

  b.mode = mode;
  b.min = min;
  b.max = max;
  b.x1 = x1;
  b.x2 = x2;
  b.lang = NFL_ENGLISH;
  bb = format;
  va_start (pvar, format);
  vsprintf (bf, bb, pvar);
  va_end (pvar);
  send_g_msg (destination, check_send_msg, (intptr_t) &b, bf);
}

/* sends to a player or mobile */

void sendf (int plr, char *format,...) {
  char b[2048];
  va_list pvar;

  va_start (pvar, format);
  vsprintf (b, format, pvar);
  va_end (pvar);
  send_g_msg(int2idx(plr, MOB), NULL, 0, b);
}

/* sends to a location */

void sendl (int loc, char *format,...) {
  char b[2048];
  va_list pvar;

  va_start (pvar, format);
  vsprintf (b, format, pvar);
  va_end (pvar);
  send_g_msg(int2idx(loc, LOC), NULL, 0, b);
}

void
lsend_msg (int destination,	/* Where to send to */
	   int lang,		/* The language to speak */
	   int mode,		/* Flags to control sending */
	   int min,		/* Minimum level of recipient */
	   int max,		/* Maximum level of recipient */
	   int x1,		/* Do not send to him */
	   int x2,		/* Nor to him */
	   char *format,...)
{				/* Format with args -> text to send */
  va_list pvar;
  char bf[2048];
  struct _send_msg_box b;
  char *bb;

  b.mode = mode;
  b.min = min;
  b.max = max;
  b.x1 = x1;
  b.x2 = x2;
  b.lang = lang;
  bb = format;

  va_start (pvar, format);
  vsprintf (bf, bb, pvar);
  va_end (pvar);

  send_g_msg (destination, check_send_msg, (intptr_t) &b, bf);
}
