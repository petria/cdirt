#include <unistd.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include "sys/file.h"
#include "kernel.h"
#include "mobiles.h"
#include "sendsys.h"
#include "bprintf.h"
#include "mobile.h"
#include "parse.h"
#include "comm.h"
#include "uaf.h"
#include "log.h"
#include "rooms.h"

#ifdef BOB
  #include <sys/file.h>
  extern char response[400];
  extern char my_nick[20];
  #define NAME "Bob"
  void bobcom (char *text, int plx, Boolean is_tell);
  extern void init(void);
  extern void ask(char *, char *);
#endif

char *make_com_text (char *b, char *s, char *t, int send_plr, int recv_plr) {
  char *p, *q, *r;

  for (p = b, q = s; *q != 0;) {
    if (*q != '%')
      *p++ = *q++;
    else {
      switch (*++q) {
      case 'n':		/* Player Name */
	if (pvis (send_plr) > 0 && see_player (recv_plr, send_plr))
	  *p++ = '(';
	for (r = see_name (recv_plr, send_plr); *r != 0;)
	  *p++ = *r++;
	if (pvis (send_plr) > 0 && see_player (recv_plr, send_plr))
	  *p++ = ')';
	break;
      case 't':		/* Message Text */
	if (t == NULL)
	  return NULL;
	for (r = t; *r != 0;)
	  *p++ = *r++;
	break;
      case 0:
	--q;
	break;
      default:
	*p++ = *q;
      }
      ++q;
    }
  }
  if (p[-1] == '\n')
    --p;
  *p = 0;
  if (!ststflg(recv_plr, SFL_SEENUMBERS))
    return b + 8;
  else
    return b;
}

void com_handler (char *format, char *linename, int lvl, int flg) {
  char xx[MAX_COM_LEN], txt[MAX_COM_LEN];
  int plr, to;

  getreinput (txt);

  if (ststflg(mynum, SFL_DUMB)) {
    bprintf("You're a mute now, sorry.\n");
    return;
  }
  if (EMPTY (txt)) {
    bprintf ("Well, what do you want to say?\n");
    return;
  }
  if (plev (mynum) < lvl) {
    bprintf ("Such advanced conversation is beyond you.\n");
    return;
  }
  if (ststflg (mynum, flg)) {
    bprintf ("You must be listening to the %s &Nchannel to talk on it.\n", 
	     linename);
    return;
  }
  for (plr = 0; plr < max_players; plr++) {
    if (is_in_game (plr)) {
      if (aliased (plr))
	to = aliasto(plr);
      else
	to = plr;

      if (plev(to) >= lvl && !ststflg (to, flg) && !ststflg(to, SFL_DEAF))
	sendf(to, "%s\n", make_com_text (xx, format, txt, mynum, to));
    }
  }
}

void
nolinecom (int lvl, int flg, char txt[20])
{
  if (plev (mynum) < lvl) {
    erreval ();
    return;
  }
  if (!ststflg (mynum, flg)) {
    ssetflg (mynum, flg);
    send_msg (DEST_ALL, 0, max (lvl, pvis (mynum)), LVL_MAX, mynum, NOBODY,
	    "&+C[&+W%s &*has left the %s &*line&+C]\n", pname (mynum), txt);
    bprintf ("You are no longer listening to the %s &Nline.\n", txt);
  } else {
    sclrflg (mynum, flg);
    send_msg (DEST_ALL, 0, max (lvl, pvis (mynum)), LVL_MAX, mynum, NOBODY,
	  "&+C[&+W%s &*has joined the %s &*line&+C]\n", pname (mynum), txt);
    bprintf ("You are once again listening to the %s &Nline.\n", txt);
  }
}

static char *
shout_test (int player, intptr_t sender, char *text)
{
  static char buff[MAX_COM_LEN];

  if (player == sender ||
      (plev (player) >= LVL_WIZARD && ststflg (player, SFL_NOSHOUT)) ||
      (plev (player) < LVL_WIZARD && ststflg (player, SFL_DEAF)) ||
      (ltstflg (ploc (player), LFL_SOUNDPROOF) &&
       ploc (sender) != ploc (player)))
    return NULL;

  if (plev (player) >= LVL_WIZARD || plev (sender) >= LVL_WIZARD
      || ploc (player) == ploc (sender)) {
    sprintf (buff, "&+r%s%s%s &+wshouts, &+W\"&+R%s&+W\"\n",
	     pvis (sender) > 0 && see_player (player, sender) ? "(" :"",
	     see_name (player, sender),
	     pvis (sender) > 0 && see_player (player, sender) ? ")" :"",
	     text);
  } else {
    sprintf (buff, "A voice shouts, &+W\"&+R%s&+W\"\n", text);
  }

  return buff;
}

void
shoutcom (void)
{
  char blob[MAX_COM_LEN];

  if (plev (mynum) < LVL_WIZARD && ststflg (mynum, SFL_NOSHOUT)) {
    bprintf ("I'm sorry, you can't shout anymore.\n");
    return;
  } else {
    getreinput (blob);
    if (EMPTY (blob)) {
      bprintf ("What do you want to shout?\n");
      return;
    } else {
      send_g_msg (DEST_ALL, shout_test, mynum, blob);
      if (ststflg (mynum, SFL_HEARBACK))
	bprintf ("You shout, &+W\"&+R%s&+W\"\n", blob);
      else
	bprintf ("Ok.");
    }
  }
}

void saycom (void) {
  char foo[MAX_COM_LEN];
  char lang[30];

  getreinput (foo);
  if (EMPTY (txt1)) {
    bprintf ("What do you want to say?\n");
    return;
  } 
  else if (ststflg(mynum, SFL_DUMB)) {
    bprintf("You're a mute now, sorry.\n");
    return;
  }
  else {
    sprintf (lang, " in &+C%s&+w", Nflags[plang (mynum)]);

    lsend_msg(sendloc(mynum), MODE_LANG | ML (plang (mynum)),
               MODE_NODEAF, LVL_MIN, LVL_MAX, mynum, NOBODY,
	       "\001p%s\003 &+wsays%s&N, &+W\"&+Y%s&+W\"\n", 
	       pname (mynum), is_eng (mynum) ? "" : lang, foo);

    lsend_msg(sendloc(mynum), MODE_NLANG | ML (plang (mynum)),
               MODE_NODEAF,
	       LVL_MIN, LVL_MAX, mynum, NOBODY, "&+G\001p%s\003 &*says "
	       "something in a foreign tongue.\n", pname (mynum));

    if (ststflg (mynum, SFL_HEARBACK))
      bprintf ("You say%s, &+W\"&+Y%s&+W\"\n", 
	       is_eng (mynum) ? "" : lang, foo);

#ifdef BOB
    if (ploc(mynum) == ploc(MOB_START_BOB + max_players))
      bobcom(txt1, mynum, False);
#endif
  }
}

void
saytocom (void)
{
  char lang[30];
  int plr;

  if ((plr = pl1) == -1) {
    bprintf ("Say what to who?\n");
    return;
  }
  if (ploc (plr) != ploc (mynum)) {
    bprintf ("They aren't here.\n");
    return;
  }
  if (plr == mynum) {
    bprintf ("You mumble to yourself.\n");
    return;
  }
  if (EMPTY (txt2)) {
    bprintf ("What do you want to say?\n");
    return;
  }
  if (ststflg(mynum, SFL_DUMB)) {
    bprintf("You're a mute now, sorry.\n");
    return;
  }
  sprintf (lang, " in &+C%s&+w", Nflags[plang (mynum)]);

  lsend_msg(sendloc(mynum), MODE_LANG | ML (plang (mynum)), 
             MODE_NODEAF,
	     LVL_MIN, LVL_MAX, mynum, plr, "&+G\001p%s\003 &+wsays to "
	     "&+C\001p%s\003%s, &+W\"&+Y%s&+W\"\n", pname (mynum), pname (plr),
	     is_eng (mynum) ? "" : lang, txt2);

  lsend_msg(sendloc(mynum), MODE_NLANG | ML (plang (mynum)),
             MODE_NODEAF,
	     LVL_MIN, LVL_MAX, mynum, plr, "&+G\001p%s\003 &*says "
	     "something to &+C\001p%s\003 &*in a foreign tongue.\n",
	     pname (mynum), pname (plr));

  lsend_msg(int2idx(plr, MOB), MODE_LANG | ML (plang (mynum)), MODE_NODEAF, LVL_MIN,
	     LVL_MAX, NOBODY, NOBODY, "&+G\001p%s\003 &*says to you %s"
	     "&+W\"&+Y%s&+W\"\n", pname (mynum), is_eng (mynum) ? "" : 
	     lang, txt2);
  lsend_msg(int2idx(plr, MOB), MODE_NLANG | ML (plang (mynum)), MODE_NODEAF, LVL_MIN,
	     LVL_MAX, NOBODY, NOBODY, "&+G\001p%s\003 &*says something to "
	     "you in a foreign tongue.\n", pname (mynum));

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You say to &+G%s&+w%s, &+W\"&+Y%s&+W\"\n", pname (plr),
	     is_eng (mynum) ? "" : lang, txt2);
}

void lsaycom (void)
{
  Boolean is_english;
  char foo[MAX_COM_LEN];
  char lang[30];
  int x;

  if (brkword () == -1) {
    bprintf ("Speak in what language?\n");
    return;
  }
  if ((x = tlookup (wordbuf, Nflags)) < 0) {
    bprintf ("That language doesn't exist!\n");
    return;
  }
  if (!ntstflg (mynum, x)) {
    bprintf ("You don't know how to speak that language!\n");
    return;
  }
  getreinput (foo);

  if (EMPTY (txt2)) {
    bprintf ("What do you want to say?\n");
    return;
  } else {
    sprintf (lang, " in &+C%s&+w", Nflags[x]);
    is_english = (x == NFL_ENGLISH);

    lsend_msg (sendloc(mynum), MODE_LANG | ML (x), MODE_NODEAF,
	       LVL_MIN, LVL_MAX, mynum, NOBODY, 
	       "&+G\001p%s\003 &+wsays%s, &+W\"&+Y%s&+W\"\n", 
	       pname (mynum), is_english ? "" : lang, foo);

    lsend_msg (sendloc(mynum), MODE_NLANG | ML (x), MODE_NODEAF,
	       LVL_MIN, LVL_MAX, mynum, NOBODY, "&+G\001p%s\003 &*says "
	       "something in a foreign tongue.\n", pname (mynum));

    if (ststflg (mynum, SFL_HEARBACK))
      bprintf ("You say%s, &+W\"&+Y%s&+W\"\n", lang, foo);
  }
}


void tellcom (int plr) {
  char *ptr, *name, text[MAX_COM_LEN];
  int p;

  if (plr != -1) {
    if (!(ptr = strchr(strbuf, ' ')))
      *text = 0;
    else
      strcpy(text, ptr + 1);
    name = pname(plr);
    p = plr;
  }
  else {
    strcpy(text, txt2);
    name = item1;
    p = pl1;
  }

  if (EMPTY(name) && plr == -1)
    bprintf ("Tell who?\n");

  else if (p  == -1)
    bprintf ("No one with that name is playing.\n");

  else if (p == mynum)
    bprintf ("You talk to yourself.\n");

  else if (EMPTY(text))
    bprintf ("What would you like to tell %s?\n", pname(p));

  else if (ststflg(mynum, SFL_NOORACLE) && 
    EQ(pname(p), "Oracle") && strstr(text, "tell"))
    bprintf("You are NoOracle, and cannot use Oracle Tell.\n");

  else if (check_forget(p, mynum))
    bprintf("&+W%s &*has forgotten you and will not receive your messages.\n",
    pname(p));

  else if (check_busy(p))
    bprintf("They are busy.\n");

  else if (ltstflg (ploc(p), LFL_SOUNDPROOF) && plev(mynum) < LVL_WIZARD
           && ploc(p) != ploc(mynum))
    bprintf("%s seems to be in a soundproof room.\n", pname(p));

  else {                  /* send tell & other msgs */
    if (check_coding(p))
      bprintf("%s is marked as &+Ccoding &*and might not respond right away.\n",
        pname(p));

    if (check_away(p)) {
      if (pawaymsg(p))
        bprintf("%s is marked as &+Caway&*:\n%s\n", pname(p), pawaymsg(p));
      else
        bprintf("%s is marked as &+Caway &*and might not respond right "
          "away.\n", pname(p));
    }

    if (p < max_players && linkdead(p))
      bprintf("%s is linkdead and may not receive your message.\n",
      pname(p));
    
    if (check_coding(mynum))
      bprintf("Don't forget that you are marked as &+Ccoding&*.\n");

    if (check_away(mynum))
      bprintf("Don't forget that you are marked as &+Caway&*.\n");

    if (ltstflg(ploc(mynum), LFL_SOUNDPROOF) &&
        ploc(mynum) != ploc(p) && plev(p) < LVL_WIZARD)
      bprintf("You are in a soundproof room and %s can't talk back.\n",
         pname(p));

#ifdef LOCMIN_CATACOMB
    if (p == (max_players + MOB_CATACOMB_SERAPH)) {
      if (strchr (text, '?')) {
        switch (my_random () % 4) {
        case 0:
          strcpy (text, "Charity");
          break;
        case 1:
          strcpy (text, "Faith");
          break;
        case 2:
          strcpy (text, "Wisdom");
          break;
        case 3:
          strcpy (text, "Courage");
          break;
        }
      } 
      else
        sprintf(text, "A blessing be upon your house.");
    }
#endif
    sendf(p, "%s&+B%s&N tells you, &+W\"&+G%s&+W\"\n",
           (ststflg(p, SFL_AWAYBEEPS) && check_away(p)) ? "\a" : "",
           see_name(p, mynum), text);

    if (ststflg(mynum, SFL_HEARBACK))
      bprintf ("You tell &+B%s&N, &+W\"&+G%s&+W\"\n", pname (p), text);

#ifdef BOB
    if (p == (MOB_START_BOB + max_players))
      bobcom(text, mynum, True);
#endif
  }
}

#ifdef BOB
void bobcom (char *text, int plx, Boolean is_tell)
{
  char question[MAX_COM_LEN];

  if (!dfile)
    init();

  if (pstr(MOB_START_BOB + max_players) < 0) {
    if (is_tell)
      bprintf("Bob is currently dead, please drive through...\n");
    return;
  }
  else if (!is_tell && count_players(ploc(mynum), COUNT_PLAYERS) > 1)
    return;

  strcpy(my_nick, NAME);
  sprintf(question, "%s: %s", NAME, text);
  ask(pname(plx),  question);
  response[0] = toupper(response[0]);

  if (is_tell)
    sendf (plx, "&+B&NBob tells you, &+W\"&+G%s&+W\"\n", response);
  else
    send_msg(int2idx(ploc(MOB_START_BOB + max_players), MOB), 0, LVL_MIN,
              LVL_MAX,
              NOBODY, NOBODY, "Bob says, &+W\"&+Y%s&+W\"\n", response);
}
#endif  /* BOB */

static char *anon_test (int plr, intptr_t snd, char *txt)
{
  static char buff[MAX_COM_LEN];

  if (ststflg (plr, SFL_NOANON))
    return NULL;

  if (ptstflg (plr, PFL_SEEANON))
    sprintf (buff, "&+w[&+WAnon &+B(&*\001p%s\003&+B)&+w]&+W: &+C'&+w%s&+C'\n",
	     pname (snd), txt);
  else
    sprintf (buff, "&+w[&+WAnon&+w]&+W: &+C'&+w%s&+C'\n", txt);

  return buff;
}

void
anoncom (void)
{
  if (ststflg (mynum, SFL_NOANON)) {
    bprintf ("You must be listening to the anon channel to talk on it.\n");
    return;
  }
  if (EMPTY (txt1)) {
    bprintf ("What would you like to say?\n");
    return;
  }
  send_g_msg (DEST_ALL, anon_test, mynum, txt1);
}

void wishcom (void) {
  char x[MAX_COM_LEN];

  if (EMPTY (item1)) {
    bprintf ("Wish for what?\n");
    return;
  }
  getreinput (x);

  if (ststflg (mynum, SFL_NOWISH)) {
    bprintf ("%s\n", plev (mynum) < LVL_WIZARD ?
	     "You find it hard to wish.." :
	     "You can't wish if you don't want to listen to them!");
    return;
  }
  send_msg (DEST_ALL, MODE_NSFLAG | MS (SFL_NOWISH), LVL_WIZARD, LVL_MAX,
	    NOBODY, NOBODY, "&+B[&+wWish from &+W\001p%s\003&+B]\n"
	    "&+B[&+w%s&+B]\n", pname (mynum), x);

  send_msg (sendloc(mynum), 0, pvis(mynum), LVL_MAX, mynum, NOBODY,
            "\001p%s\003 begs and grovels to the powers that be.\n", 
            pname(mynum));

  if (ststflg (mynum, SFL_HEARBACK) && plev (mynum) < LVL_WIZARD)
    bprintf ("You wish: %s\n", x);
  else
    bprintf ("Ok.\n");
}

void
chatcom (void)
{
  int i;

  if (EMPTY (txt1)) {
    bprintf ("&+C[&+wGlobal Chat Channel : %d&+C]\n", pchannel (mynum));
    bprintf ("&+COn Channel: ");
  }
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && pchannel (i) == pchannel (mynum)) {

      if (EMPTY (txt1))
	bprintf ("&+w%-15s", see_name (mynum, i));

      if (!EMPTY (txt1))
	sendf (i, "&+C-> &+w%s%s%s &+C<- &+B: &+w%s\n",
	       pvis (mynum) > 0 && see_player (i, mynum) ? "(" : "",
	       see_name (i, mynum),
	       pvis (mynum) > 0 && see_player (i, mynum) ? ")" : "",
	       txt1);
    }
  }

  if (EMPTY (txt1))
    bprintf ("\n");
}

void
channelcom (void)
{
  Boolean f, is_me, is_mobile;
  int a, new_channel, i;
  PERSONA p;

  if ((brkword () == -1) && ptstflg (mynum, PFL_CHATMOD)) {
    bprintf ("&+CName                Channel\n");
    bprintf ("&+b---------------------------\n");

    for (i = 0; i < max_players; ++i) {
      if (is_in_game (i))
	bprintf ("&+W%-20s &+C%6d\n", see_name (mynum, i), pchannel (i));
    }

    bprintf ("&+b---------------------------\n");
    return;
  }
  if ((a = find_player (wordbuf, &p, &f)) == -1) {
    bprintf ("Who?\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if (is_mobile) {
    bprintf ("You cannot change the chat channel of a mobile!\n");
    return;
  }
  is_me = !f && a == mynum;
  is_mobile = !f && a >= max_players;

  if (is_mobile) {
    bprintf ("You cannot change the chat channel of a mobile!\n");
    return;
  }
  if (brkword () == -1) {
    if (is_me) {
      bprintf ("&*Your current chat channel is &+W%d\n", pchannel (mynum));
    } else {
      if (ptstflg (mynum, PFL_CHATMOD))
	bprintf ("&+W%s &*is currently using channel &+C%d\n", p.ublock.pname,
		 p.player.pchannel);
      else
	bprintf ("You cannot view another players chat channel selection!\n");
    }
    return;
  }
  if (!is_me && !ptstflg (mynum, PFL_CHATMOD)) {
    bprintf ("You cannot change another players chat channel selection!\n");
    return;
  }
  new_channel = max (0, atoi (wordbuf));

  if (is_me && (pchannel (mynum) == new_channel)) {
    bprintf ("Why did you pick the channel that you are already on?\n");
    return;
  }
  if (new_channel < 0 || new_channel > 100000) {
    bprintf ("The channel must be between 0 and 100000.\n");
    return;
  }
  if (!is_me)
    bprintf ("Changing &+W%s's &*channel to &+C%d&*.\n", p.ublock.pname, 
	     new_channel);
  else
    bprintf ("Changing your channel to &+C%d&*.\n", new_channel);

  if (f) {
    p.player.pchannel = new_channel;
    putuaf (&p);
    return;
  }
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && see_player (i, a) && i != a) {
      if (pchannel (i) == pchannel (a))
	sendf (i, "&+C-> &+W%s &*has left the channel &+C<-\n", pname (a));
      if (pchannel (i) == new_channel)
	sendf (i, "&+C-> &+W%s &*has joined the channel &+C<-\n", pname (a));
    }
  }

  setpchannel (a, new_channel);
}

void forgetcom (void) {
  int b, i;

  if (brkword () == -1) {
    bprintf ("You are currently forgetting:\n");
    for (i = 0; i < 10; ++i) {
      if (rplrs[mynum].forget[i] != -1) {
	if (!is_in_game (rplrs[mynum].forget[i]))
	  rplrs[mynum].forget[i] = -1;
	else
	  bprintf ("%s\n", pname (rplrs[mynum].forget[i]));
      }
    }
    return;
  }
  if ((b = pl1) != -1) {
    if (ptstflg (b, PFL_NOFORGET)) {
      bprintf ("You cannot forget about that person.\n");
      return;
    }
    if (check_forget (mynum, b)) {
      bprintf ("Remembering %s.\n", pname (b));
      for (i = 0; i < 10; ++i)
	if (rplrs[mynum].forget[i] == b)
	  rplrs[mynum].forget[i] = -1;
      return;
    }
    for (i = 0; i < 10; ++i) {
      if (rplrs[mynum].forget[i] == -1) {
	bprintf ("Forgetting %s.\n", pname (b));
	rplrs[mynum].forget[i] = b;
	return;
      }
    }
    bprintf ("Sorry, you cannot forget anybody else.\n");
    return;
  }
  bprintf ("That player is not online.\n");
}

void langcom (void) {
  int x;

  if (brkword () == -1) {
    bprintf ("Current Language: %s\n", Nflags[plang (mynum)]);
    bprintf ("Known Languages:\n");
    show_bits(mbits(mynum), mindex, Nflags, NFLAGS, False);
    return;
  }
  if ((x = tlookup (wordbuf, Nflags)) < 0) {
    bprintf ("Unknown Language - %s\n", wordbuf);
    bprintf ("Known Languages:\n");
    show_bits(mbits(mynum), mindex, Nflags, NFLAGS, False);
    return;
  }
  if (ntstflg (mynum, x)) {
    bprintf ("Language changed to %s.\n", Nflags[x]);
    setplang (mynum, x);
  } else {
    bprintf ("Unknown Language - %s\n", wordbuf);
    bprintf ("Known Languages:\n");
    show_bits(mbits(mynum), mindex, Nflags, NFLAGS, False);
  }
}

void
conversecom (void)
{
  int b;

  if (EMPTY (item1)) {
    if (pconv (mynum) == -1)
      bprintf ("Converse with who?\n");
    else
      bprintf ("You are conversing with &+W%s&*.\n", pname (pconv (mynum)));
    return;
  }
  if ((b = pl1) == -1) {
    bprintf ("Who?\n");
    return;
  }
  if (b == mynum) {
    bprintf ("You want to converse with yourself? You aren't Sybil.\n");
    return;
  }
  if (b >= max_players) {
    bprintf ("I'm sure that %s has alot to converse about.\n", pname (b));
    return;
  }
  setpconv (mynum, b);
  bprintf ("Entering Converse Mode                  !<command> = MUD Command  ** = Quit\n");
  bprintf ("You are conversing with: %s\n\n", pname (b));
}

void
replycom (void)
{
  if (replyplr(mynum) == -1) {
    bprintf ("You have nobody to reply to.\n");
    return;
  }
  if (!is_in_game (replyplr(mynum))) {
    bprintf ("Person you were replying to has left the game.\n");
    replyplr(mynum) = -1;
    return;
  }
  if (EMPTY (txt1)) {
    bprintf ("What do you wish to reply?\n");
    return;
  }
  sendf (replyplr(mynum), "&+C%s &+wreplies &+W\"&+w%s&+W\"\n",
	 see_name (replyplr(mynum), mynum), txt1);

  if (ststflg (mynum, SFL_HEARBACK))
    bprintf ("You reply, &+C\"&+G%s&+C\"\n", txt1);
}
