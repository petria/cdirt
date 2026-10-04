/*********************************************************************
 * AberChat Client 4.20.7 (C) Giancarlo Castrataro --- CDirt Version *
 *********************************************************************/

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#ifndef _OLD_LINUX_
  #include <crypt.h>
#endif

#include "kernel.h"
#include "mobile.h"
#include "sflags.h"
#include "pflags.h"
#include "sendsys.h"
#include "log.h"
#include "mud.h"
#include "parse.h"
#include "bprintf.h"
#include "commands.h"
#include "verbs.h"
#include "timing.h"
#include "client.h"
#include "client_int.h"

Boolean auto_restart = True;
Boolean auth = False;
Boolean aber_output = False;
int aberfd = -1;
char abuffer[WRITELEN];
time_t boottime = 0;		/* when we opened the socket to the server */
time_t socktime = 0;		/* the last time anything went over socket */

/* Chat sequences for all communications lines */

char *chat_chat = "&=WB[%s&=RB@&=WB%s]&* %s\n";
char *chat_emote = "&=WB->%s&=RB@&=WB%s %s<-&*\n";
char *god_chat = "&=WrAGod: %s@%s -->&* %s\n";
char *god_emote = "&=WrAGod: %s@%s %s&*\n";
char *coder_chat = "&=WyACode: %s@%s -->&* %s\n";
char *coder_emote = "&=WyACode: %s@%s %s&*\n";
char *atell_fmt = "&+R%s&+W@&+c%s&* tells you&+W, &+W'&*%s&+W'&*\n";
char *atell_send_fmt = "You tell &+R%s&+W@&+c%s&*, &+W'&*%s&+W'&*\n";
char *atell_emote_send_fmt = "You aberemote to &+R%s&+W@&+c%s&*: %s&*\n";
char *atell_emote_fmt = "&+R%s&+W@&+c%s&* %s&*\n";
char *ainfo_fmt = "&+b[&+WAberchat info from &+c%s&+W: &*%s&+b]&*\n";
char *areply_fmt = "You reply, &+W'%s&+W'&*\n";
char *amessage_fmt = "&+cReply from %s&*: &+W%s\n";

/* the local functions */
static void cmd_aautocon(void);
static void cmd_aboot(void);
static void cmd_achat(int type);
static void cmd_acommstats(void);
static void cmd_acopyright(void);
static void cmd_ainfo(void);
static void cmd_akick(void);
static void cmd_amasslist(void);
static void cmd_amudlist(void);
static void cmd_noaberchat(void);
static void cmd_noachat(void);
static void cmd_noagod(void);
static void cmd_noainfo(void);
static void cmd_noatell(void);
static void cmd_apolicy(void);
static void cmd_areply(void);
static void cmd_ashutdown(void);
static void cmd_atell(void);
static void cmd_aversion(void);
static void cmd_awho(void);

static void cmd_achat(int type) {
  char buff[MAX_COM_LEN];

  if (!abercheck(mynum, type)) {
    bprintf("Sorry, you can't talk on that AberChat line.\n");
    return;
  }
  getreinput(buff);
  if (brkword() == -1)
    bprintf("What would you like to chat?\n");
  else {
    aprintf("%c" D "%c" D "%s" D "%s" D "%s",
	    TO_ALL, type, MYMUD, pname(mynum), buff);
  }
}

static void cmd_atell(void) {
  char *mud, *ctl;

  if (!abercheck(mynum, TELL)) {
    bprintf("Sorry, you can't atell at the moment.\n");
    return;
  }
  if (!(mud = strchr(item1, '@'))) {
    bprintf("Usage: ATell player@mud [message]\n");
  } 
  else {
    *(mud++) = 0;
    if (EMPTY(txt2)) {
      bprintf("What do you want to atell them?\n");
      return;
    }
    set_areply(mynum, item1, mud);
    aprintf("%c" D "%s" D "%c" D "%s" D "%s" D "%s" D "%s\0",
	    TO_MUD, mud, TELL, pname(mynum), item1, MYMUD, txt2);
    ctl = (*txt2 == ':') ? atell_emote_send_fmt : atell_send_fmt;
    if (*txt2 == ':')
      txt2++;
    for (; *txt2 == ' '; txt2++);
    if (ststflg(mynum, SFL_HEARBACK)) {
      sendf(mynum, ctl, item1, mud, txt2);
    }
  }
}

static void cmd_areply(void) {
  if (EMPTY(txt1)) {
    bprintf("Reply what? You need to type something after it.\n");
    return;
  } 
  else if (!amud(mynum) || !atarget(mynum)) {
    bprintf("You have no-one to reply to.\n");
    return;
  } 
  else if (!abercheck(mynum, TELL)) {
    bprintf("Sorry, you can't areply at the moment.\n");
    return;
  }
  if (ststflg(mynum, SFL_HEARBACK))
    sendf(mynum, areply_fmt, txt1);
  aprintf("%c" D "%s" D "%c" D "%s" D "%s" D "%s" D "%s\0", TO_MUD,
	  amud(mynum), TELL, pname(mynum), atarget(mynum), MYMUD, txt1);
}

static void cmd_akick(void) {
  char *mud;

  if (plev(mynum) < LEVEL_ADMIN)
    bprintf("You aren't powerful enough.\n");
  else if (!(mud = strchr(item1, '@')))
    bprintf("Usage: AKick player@mud\n");
  else {
    *(mud++) = 0;
    aprintf("%c" D "%s" D "%c" D "%s" D "%s" D "%s",
	    TO_MUD, mud, KICK, pname(mynum), item1, MYMUD);
    mudlog("ABERCHAT: %s has akicked %s@%s", pname(mynum), item1, mud);
  }
}

static void cmd_acommstats(void) {
  int i;

  if (plev(mynum) < LEVEL_ADMIN) {
    if (!ptstflg(mynum, PFL_ABERCHAT)) {
      bprintf("Pardon?\n");
      return;
    } 
    else if (ststflg(mynum, SFL_NOABERCHAT)) {
      bprintf("You have NoAberChat turned on. (noaberchat to toggle it)\n");
      return;
    }

    if (ptstflg(mynum, PFL_CODER))
      bprintf("%-*s : Chat Tell Info Code\n", PNAME_LEN, "Name"); 
    else
      bprintf("%-*s : Chat Tell Info\n", PNAME_LEN, "Name");

    bprintf("&+b-------------------------------------------------------------------------------\n");
    bprintf("&+W%-*s&* : %4s %4s %4s%4s\n", PNAME_LEN, pname(mynum)
	    ,ststflg(mynum, SFL_NOACHAT) ? "&+rOff &*" : "&+G On &*"
	    ,ststflg(mynum, SFL_NOATELL) ? "&+rOff &*" : "&+G On &*"
	    ,ststflg(mynum, SFL_NOAINFO) ? "&+rOff &*" : "&+G On &*"
	    ,ptstflg(mynum, PFL_CODER) ? ststflg(mynum, SFL_NOACODE) ? " &+rOff &*" : " &+G On &*" : ""
	);
    bprintf("&+b-------------------------------------------------------------------------------\n");
    return;
  }

  bprintf("%-*s : Chat Tell Info Code  God\n", PNAME_LEN, "Name");
  bprintf("&+b-------------------------------------------------------------------------------\n");
  for (i = 0; i < max_players; i++) {
    if (is_in_game(i) && see_player(mynum, i)) {
      if (!ptstflg(i, PFL_ABERCHAT))
	bprintf("&+W%-*s&* : ---- ---- ---- ---- ----\n", PNAME_LEN, pname(i));
      else if (ststflg(i, SFL_NOABERCHAT)) {
	bprintf("&+W%-*s&* : NoAc NoAc NoAc %4s %4s&*\n", PNAME_LEN, pname(i)
		,ptstflg(i, PFL_CODER) ? "NoAc" : "----"
		,plev(i) >= LEVEL_ADMIN ? "NoAc" : "----");
      } 
      else {
	bprintf("&+W%-*s&* : %4s %4s %4s %4s %4s\n", PNAME_LEN, pname(i)
		,ststflg(i, SFL_NOACHAT) ? "&+rOff &*" : "&+G On &*"
		,ststflg(i, SFL_NOATELL) ? "&+rOff &*" : "&+G On &*"
		,ststflg(i, SFL_NOAINFO) ? "&+rOff &*" : "&+G On &*"
		,ptstflg(i, PFL_CODER) ? ststflg(i, SFL_NOACODE) ? "&+rOff &*" : "&+G On &*" : "----"
		,plev(i) >= LEVEL_ADMIN ? ststflg(i, SFL_NOAGOD) ? "&+rOff &*" : "&+G On &*" : "----"
	    );
      }
    }
  }
  bprintf("&+b-------------------------------------------------------------------------------\n");
}

static void cmd_noaberchat(void) {
  if (!ptstflg(mynum, PFL_ABERCHAT))
    bprintf("You can't use aberchat.\n");
  else
    togglecom(SFL_NOABERCHAT, "Aberchat Disabled.", "Aberchat Enabled.");
}

static void cmd_noachat(void) {
  if (!ptstflg(mynum, PFL_ABERCHAT))
    bprintf("You can't use the achat line.\n");
  else
    togglecom(SFL_NOACHAT, "AChat line disabled.", "AChat line enabled.");
}

static void cmd_noacode(void) {
  if (!ptstflg(mynum, PFL_ABERCHAT) || !ptstflg(mynum, PFL_CODER))
    bprintf("You can't use the acode line.\n");
  else
    togglecom(SFL_NOACODE, "ACode line disabled.", "ACode line enabled.");
}

static void cmd_noagod(void) {
  if (!ptstflg(mynum, PFL_ABERCHAT) || plev(mynum) < LEVEL_ADMIN)
    bprintf("You can't use the agod line.\n");
  else
    togglecom(SFL_NOAGOD, "AGod line disabled.", "AGod line enabled.");
}

static void cmd_noainfo(void) {
  if (!ptstflg(mynum, PFL_ABERCHAT))
    bprintf("You can't see the ainfo line.\n");
  else
    togglecom(SFL_NOAINFO, "AInfo line disabled.", "AInfo line enabled.");
}

static void cmd_noatell(void) {
  if (!ptstflg(mynum, PFL_ABERCHAT))
    bprintf("You can't see atells.\n");
  else
    togglecom(SFL_NOATELL, "You won't receive atells.", "You will receive atells.");
}

static void cmd_ainfo(void) {
  char buff[MAX_COM_LEN];

  if (!abercheck(mynum, INFOMSG) || plev(mynum) < LEVEL_ADMIN) {
    bprintf("Sorry, you can't send AINFO messages.\n");
    return;
  }
  getreinput(buff);
  if (brkword() == -1)
    bprintf("What should the message be?\n");
  else
    aprintf("%c" D "%c" D "%s" D "%s", TO_ALL, INFOMSG, MYMUD, buff);
}

static void cmd_aautocon(void) {
  if (plev(mynum) < LEVEL_ADMIN) {
    bprintf("You aren't powerful enough.\n");
    return;
  }
  auto_restart = auto_restart ? False : True;

  bprintf("AberChat Auto Connect is %s.\n", auto_restart ? "On" : "Off");
}

static void cmd_aboot(void) {
  if (plev(mynum) < LEVEL_ADMIN) {
    bprintf("You aren't powerful enough.\n");
    return;
  }
  switch (aberchat_boot()) {
  case 0:
    bprintf("Failed.\n");
    break;
  case 1:
    bprintf("Connected to AberChat Server.\n");
    break;
  case 2:
    bprintf("AberChat already running.\n");
    break;
  default:
    mudlog("ABERCHAT: strange result returned from aberchat_boot()");
    break;
  }
  return;
}

static void cmd_ashutdown(void) {
  if (plev(mynum) < LEVEL_ADMIN) {
    bprintf("You aren't powerful enough.\n");
    return;
  }
  if (!aberchat_shutdown())
    bprintf("Aberchat not running.\n");
  else {
    bprintf("Disconnected from AberChat server.\n");
    if (auto_restart)
      bprintf("Server will reconnect in 5 minutes. (AAUTOCON ON)\n");
    send_msg(DEST_ALL, 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
     "&+WAberchat has been disconnected by %s.\n", pname(mynum));   
  }
}

/* Mudlist request */

static void cmd_amudlist(void) {
  aprintf("%c" D "%s" D "%c" D "%s", TO_MUD, MYMUD, MUDLIST, pname(mynum));
}

static void cmd_amasslist(void) {
  if (cur_player->writer) {                       /* writer uses work2 too */
    bprintf("Finish whatever you're writing first.\n");
    return;
  }
  if (!abercheck(mynum, 0)) {
    bprintf("Can't do that.\n");
    return;
  }
  strcpy((char *) cur_player->work2, "masslist");
  afilepagecom(NULL);
}

static void cmd_apolicy(void) {
  char buff[1024];

  if (cur_player->writer) {
    bprintf("Finish whatever you're writing first.\n");
    return;
  }
  if (!abercheck(mynum, 0)) {
    bprintf("Can't do that.\n");
    return;
  }
  getreinput(buff);

  if (brkword() == -1)
    strcpy((char *) cur_player->work2, "policy");
  else
    sprintf((char *) cur_player->work2, "policy.%s", buff);

  afilepagecom(NULL);
}

static void cmd_acopyright(void) {
  if (cur_player->writer) {
    bprintf("Finish whatever you're writing first.\n");
    return;
  }
  if (!abercheck(mynum, 0)) {
    bprintf("Can't do that.\n");
    return;
  }
  strcpy((char *) cur_player->work2, "copyright");
  afilepagecom(NULL);
}

/****************************************************************
 * cmd_awho() sends a WHO request to the specified mud.  The mud *
 * then sends a response packet, in send_awho(), and the packet *
 * is received/displayed in receive_awho()                      *
 ****************************************************************/

static void cmd_awho()
{
  if (EMPTY(txt1))
    bprintf("Which mud?\n");
  else
    aprintf("%c" D "%s" D "%c" D "%s" D "%s",
	    TO_MUD, txt1, WHO, MYMUD, pname(mynum));
}

/********************************************************************
 * cmd_aversion() sends the current version to the aberchat server, *
 * or makes a request to another mud for it's version.              *
 ********************************************************************/

static void cmd_aversion(void)
{
  if (EMPTY(txt1))
    aprintf("%c" D "%s" D "%c" D "%s" D "%s",
	    TO_MUD, MYMUD, AVERSION, pname(mynum), CLIENT_VERSION);
  else {
    bprintf("Version for MUD %s (if no response, client is pre-4.20):\n", txt1);
    aprintf("%c" D "%s" D "%c" D "%s" D "%s",
	    TO_MUD, txt1, AVERREQ, MYMUD, pname(mynum));
  }
}

/* Checks to see if a player will hear a line */

Boolean abercheck(int plx, int type) {
  if (!is_in_game(plx) || !ptstflg(plx, PFL_ABERCHAT) || 
       ststflg(plx, SFL_NOABERCHAT) || !auth)
    return False;
  else {
    switch (type) {
    case CHAT:
      if (ststflg(plx, SFL_NOACHAT))
	return False;
      break;
    case ACODE:
      if (!ptstflg(plx, PFL_CODER) || ststflg(plx, SFL_NOACODE))
	return False;
      break;
    case AGOD:
      if (plev(plx) < LEVEL_ADMIN || ststflg(plx, SFL_NOAGOD))
	return False;
      break;
    case INFOMSG:
      if (ststflg(plx, SFL_NOAINFO))
	return False;
      break;
    case TELL:
      if (ststflg(plx, SFL_NOATELL))
	return False;
      break;
    }
  }
  return True;
}

/* Pops one argument from a string */

char *get_arg(char *txt) {
  char *ptr;

  if (txt != NULL && (ptr = strchr(txt, DCHAR))) {
    *ptr = 0;
    return (++ptr);
  } 
  else
    return (NULL);
}

int aberchat_boot(void) {
#ifndef ABERCHAT
  /* Respect the build option even when an administrator invokes aboot. */
  return 0;
#else
  if (aberfd == -1) {
    /* Pending packets belong to one connection, never to its successor. */
    *abuffer = 0;
    aber_output = False;
    auth = False;
    if ((aberfd = makesock(SERVER, SERVER_PORT)) == -1)
      return 0;
    else {
      authenticate();
      time(&socktime);
      time(&boottime);
      return 1;
    }
  }
  return 2;
#endif
}

Boolean aberchat_shutdown(void) {
  if (aberfd != -1) {
    ainfo(MYMUD, "Exited Network");
    mudlog("ABERCHAT: Shutdown");
    close(aberfd);
    aberfd = -1;
    auth = False;
    aber_output = False;
    *abuffer = 0;
    return True;
  }
  auth = False;
  aber_output = False;
  *abuffer = 0;
  return False;
}

void aberchat_autorecon(void) {
  mudlog("ABERCHAT: auto reconnect");
  aberchat_boot();
}

/* a little function to print out the aberchat socket details in a format
 * that would fit in nicely with socketcom in god.c
 */

void aberchat_socketdetails(time_t now) {
  char idlebuff[64], loginbuff[64];
  time_t idletime,logintime;
  if (aberfd != -1) {
    idletime=now-socktime;
    logintime=now-boottime;
    if(idletime < 0) idletime=0;
    if(logintime < 0) logintime=0;
    strcpy(idlebuff, sec_to_hhmmss(idletime));
    strcpy(loginbuff, sec_to_hhmmss(logintime));
    bprintf("&+G%-19s&* %2d  %8.8s  %8.8s  %-34.34s\n"
	    ,"Aberchat socket", aberfd, idlebuff, loginbuff, SERVER);
  }
  return;
}

void aprintf(char *format,...) {
  char buffer[WRITELEN];
  int length;
  size_t queued;
  Boolean ok = False;
  va_list pvar;

  if (mynum != -1) {
    if (aberfd == -1)
      bprintf("Aberchat not running.\n");
    else if (!auth)
      bprintf("Aberchat not authenticated.\n");
    else if (!ptstflg(mynum, PFL_ABERCHAT))
      bprintf("You aren't authorized to use aberchat.\n");
    else if (ststflg(mynum, SFL_NOABERCHAT))
      bprintf("You don't have AberChat enabled.\n");
    else
      ok = True;
  } 
  else if (aberfd != -1)
    ok = True;

  if (ok) {
    va_start(pvar, format);
    length = vsnprintf(buffer, sizeof(buffer), format, pvar);
    va_end(pvar);
    queued = strlen(abuffer);
    if (length < 0 || (size_t)length >= sizeof(buffer) ||
        (size_t)length >= sizeof(abuffer) - queued) {
      mudlog("ABERCHAT: Output buffer capacity exceeded; closing connection");
      aberchat_shutdown();
      return;
    }
    memcpy(abuffer + queued, buffer, (size_t)length + 1);
    aber_output = True;
  }
  return;
}

void aberchat_writepacket(int fd) {
  size_t pending;
  ssize_t n_wrote;

  if (fd != aberfd || fd < 0 || !aber_output)
    return;
  pending = strlen(abuffer) + 1;
  n_wrote = write(fd, abuffer, pending);
  if (n_wrote < 0) {
    if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
      aberchat_shutdown();
    return;
  }
  if (n_wrote == 0)
    return;
  if ((size_t)n_wrote == pending) {
    *abuffer = 0;
    aber_output = False;
  }
  else
    memmove(abuffer, abuffer + n_wrote, pending - (size_t)n_wrote);
}

void aberchat_readpacket(int fd) {
  static char buff[READLEN];
  static char *pos = buff;
  int n_read, data_type;

  n_read = read(fd, pos, READLEN - (pos - buff));

  if (n_read <= 0) {
    if (auth) {
      ainfo("AberChat Server", "Aber Server has crashed");
      mudlog("ABERCHAT: Connection closed by Server.");
    }
    aberchat_shutdown();
  } 
  else if (*(pos + n_read - 1) == 0) {
    data_type = *buff;
    abercmd(*buff, buff + 2);
    *buff = 0;
    pos = buff;
  } 
  else
    pos += n_read;

  time(&socktime);
  return;
}

void abercmd(char type, char *packet) {
  char *arg1, *arg2, *arg3, *arg4;
  int x;

  switch (type) {		/* commands with unlimited args here */
  case MUDLIST:
    amudlist(packet);
    return;
  case RWHO:
    receive_awho(packet);
    return;
  }

  arg1 = packet;
  arg2 = get_arg(arg1);
  arg3 = get_arg(arg2);
  arg4 = get_arg(arg3);

  if(aberchat_filter(type,arg1,arg2,arg3,arg4))
    return;

  switch (type) {
  case KICK:
    kick_player(arg1, arg2, arg3);
    return;
  case FILEPAGE:
    afilepage(arg1, arg2, arg3);
    return;
  case AVERSION:
    aversion(arg1, arg2, arg3);
    return;
  case MSG:
    amessage(arg1, arg2, arg3);
    return;
  case TELL:
    atell(arg1, arg2, arg3, arg4);
    return;
  }
  x = mynum;
  mynum = -1;

  switch (type) {
  case AVERREQ:
    aver_reply(arg1, arg2);
    break;
  case WHO:
    send_awho(arg1, arg2);
    break;
  case CHAT:
  case ACODE:
  case AGOD:
    achat(type, arg1, arg2, arg3);
    break;
  case INFOMSG:
    ainfo(arg1, arg2);
    break;
  case AUTH:
    if (*packet == 'Y')
      auth_received(True);
    else
      auth_received(False);
    break;
  }
  mynum = x;
}

void afilepagecom(char *input) {
  if (!input) {			/* first run */
    if (cur_player->writer) {
      bprintf("Finish whatever you're writing first.\n");
      return;
    }
    if (!auth || aberfd == -1) {
      bprintf("Aberchat is not running.\n");
      return;
    }
    cur_player->work = 0;
    strcpy(cur_player->cprompt, "");
    push_input_handler(afilepagecom);
  }
  aprintf("%c" D "%s" D "%c" D "%s" D "%s" D "%d", TO_MUD, MYMUD,
	  FILEPAGE, pname(mynum), cur_player->work2, cur_player->work);
  if (!auth || aberfd == -1) {
    strcpy(cur_player->cprompt, build_prompt(mynum));
    bprintf(cur_player->cprompt);
    pop_input_handler();
  }
}

void afilepage(char *player, char *txt, char *more) {
  int plx;

  if ((plx = fpbn(player)) == -1)
    return;
  else {
    setup_globals(plx);
    bprintf("%s", txt);

    if (*more == '1') {
      cur_player->work++;
      bprintf("[File: %s] ", cur_player->work2);
    } 
    else {
      cur_player->work = 0;
      strcpy(cur_player->cprompt, build_prompt(mynum));
      bprintf("%s", cur_player->cprompt);
      pop_input_handler();
    }
  }
}

/* Prototype: FROM_PLAYER:TO_PLAYER:FROM_MUD:TEXT */

void atell(char *f_plr, char *to_plr, char *f_mud, char *text) {
  int plx;
  char *ctl;

  if ((plx = fpbn(to_plr)) != -1 && !pvis(plx)) {
    if (abercheck(plx, TELL)) {
      set_areply(plx, f_plr, f_mud);
      ctl = (*text == ':') ? atell_emote_fmt : atell_fmt;
      if (*text == ':')
	text++;
      for (; *text == ' '; text++);
      sendf(plx, ctl, f_plr, f_mud, text);
      if (check_away(plx)) {
	aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D
		"&*%s is marked as &+Caway&*: %s",
		TO_MUD, f_mud, MSG, f_plr, MYMUD, pname(plx), 
                players[plx].awaymsg);
      } 
      else if (check_coding(plx)) {
	aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D
	"&*%s is marked as &+Ccoding &*and might not respond right away.",
		TO_MUD, f_mud, MSG, f_plr, MYMUD, pname(plx));
      }
    } 
    else {
      int x = mynum;
      mynum = -1;
      aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D 
              "You can't ATell %s at the moment", TO_MUD, f_mud, MSG, 
               f_plr, MYMUD, to_plr);
      mynum = x;
    }
  } 
  else {
    int x = mynum;
    mynum = -1;
    aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D "%s is not online",
	    TO_MUD, f_mud, MSG, f_plr, MYMUD, to_plr);
    mynum = x;
  }
}

void amessage(char *to_plr, char *f_mud, char *text) {
  int plx;

  if ((plx = fpbn(to_plr)) != -1)
    sendf(plx, amessage_fmt, f_mud, text);
}

void ainfo(char *f_mud, char *text) {
  int plx;

  for (plx = 0; plx < max_players; plx++)
    if (abercheck(plx, INFOMSG))
      sendf(plx, ainfo_fmt, f_mud, text);
}

void achat(int type, char *f_mud, char *f_plr, char *text) {
  char *ctl;
  int plx;

  switch (type) {
  case CHAT:
    ctl = (*text == ':') ? chat_emote : chat_chat;
    break;
  case ACODE:
    ctl = (*text == ':') ? coder_emote : coder_chat;
    break;
  case AGOD:
    ctl = (*text == ':') ? god_emote : god_chat;
    break;
  default:
    ctl = "%s - %s - %s\n"; /* dummy format in case something goes wrong :) */
    break;
  }
  if (*text == ':')
    text++;
  for (; *text == ' '; text++);

  for (plx = 0; plx < max_players; plx++) {
    if (abercheck(plx, type)) {
      sendf(plx, ctl, f_plr, f_mud, text);
    }
  }
}

extern int cmp_player(const void *, const void *);

void send_awho(char *mud, char *player) {
  char *level, *title;
  int i, list[max_players], len;

  for (i = 0, len = 0; i < max_players; i++)
    if (is_in_game(i) && !pvis(i))
      list[len++] = i;

  qsort(list, len, sizeof(int), cmp_player);

  aprintf("%c" D "%s" D "%c" D "%s" D "%s", TO_MUD, mud, RWHO, player, MYMUD);
  for (i = 0; i < len; i++) {
    level = player_level(list[i]);
    title = make_title(ptitle(list[i]), pname(list[i]));
    aprintf(D "%s" D "%s" D "%s", pname(list[i]), level, title);
  }
}

void receive_awho(char *packet) {
  char *player, *name, *level, *title, *mud, *next;
  int plx, i;

  player = packet;
  mud = get_arg(player);
  next = get_arg(mud);

  if ((plx = fpbn(player)) == -1)
    return;
  setup_globals(plx);

  if (!next) {
    bprintf("There are no players online at %s.\n%s", mud, cur_player->cprompt);
    return;
  }
  bprintf("\n&+WName          &+CLevel          &+YTitle\n" DASHES);
  for (i = 0; next; i++) {
    name = next;
    level = get_arg(name);
    title = get_arg(level);
    next = get_arg(title);
    bprintf("%-14s%-15s%-45.47s\n", name, level, title);
  }
  bprintf(DASHES "There %s %d player%s on %s.\n%s", (i > 1) ? "are" : "is",
	  i, (i > 1) ? "s" : "", mud, cur_player->cprompt);
}

/***************************************************************************
 * To connect to the network, authenticate() sends a packet which contains *
 * the password, mudname, mudlib, and port.  This allows the server to     *
 * build a mudlist entry for the connecting mud.  Following function is a  *
 * server response, which enables the mud to send data over aberchat       *
 ***************************************************************************/

void authenticate(void) {
  char passwd[30];
  int oldnum = mynum;

  strcpy(passwd, crypt(PASS, "Ac"));
  auth = True;
  mynum = -1;
  aprintf("%c" D "%s" D "%c" D "%s" D "%s" D "%d" D "%s",
	  TO_MUD, MYMUD, AUTH, MYMUD, MUDLIB, PORT, passwd);
  auth = False;
  mynum = oldnum;
}

void auth_received(Boolean ok) {
  if (ok) {
    auth = True;
    mudlog("ABERCHAT: Successful connection to server");
    ainfo(MYMUD, "Authenticated/Joined Network");
  } 
  else {
    mudlog("ABERCHAT: Connection failed (Bad password)");
    send_msg(DEST_ALL, MODE_SFLAG | MS(SFL_SEEEXT), LVL_WIZARD, LVL_MAX,
	     NOBODY, NOBODY, "&+C[Aberchat Failed: Bad Password]\n");
    aberchat_shutdown();
  }
}

void aver_reply(char *tomud, char *player) {
  aprintf("%c" D "%s" D "%c" D "%s" D "%s",
	  TO_MUD, tomud, AVERSION, player, CLIENT_VERSION);
}

void aversion(char *to_plr, char *client_ver, char *server_ver) {
  int plx;

  if ((plx = fpbn(to_plr)) == -1)
    return;
  sendf(plx, "Server is: %s\n"
	"Client is: %s\n"
	"All code Copyright G. Castrataro (weenrdog@bluemarble.net)\n",
	server_ver, client_ver);
}

void amudlist(char *list) {
  char *toplr, *mudname, *mudlib, *mudaddr, *mudip, *mudport, *next;
  int pl;

  toplr = list;
  next = get_arg(toplr);

  if ((pl = fpbn(toplr)) == -1)
    return;

  setup_globals(pl);

  bprintf("\n&+WMudname         &+CAddress                  &+YIP         "
	  "     &+MPort       &+GCode Base\n" DASHES);

  while (next != NULL) {
    mudname = next;
    mudlib = get_arg(mudname);
    mudaddr = get_arg(mudlib);
    mudip = get_arg(mudaddr);
    mudport = get_arg(mudip);
    next = get_arg(mudport);
    bprintf("%-16.14s%-25.23s%-16.14s%-11s%-11.11s\n",
	    mudname, mudlib, mudaddr, mudip, mudport);
  }
  bprintf("%s%s", DASHES, cur_player->cprompt);
}

void set_areply(int plr, char *target, char *mud) {
  if (atarget(plr))
    FREE(atarget(plr));

  atarget(plr) = COPY(target);

  if (amud(plr))
    FREE(amud(plr));

  amud(plr) = COPY(mud);
}

void aberchat_newconn(int plr) {
  if (atarget(plr)) {
    FREE(atarget(plr));
    atarget(plr) = NULL;
  }
  if (amud(plr)) {
    FREE(amud(plr));
    amud(plr) = NULL;
  }
  return;
}

void kick_player(char *from, char *to, char *frommud) {
  int plr;

  if ((plr = fpbn(to)) == -1)
    aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D "%s is not online",
	    TO_MUD, frommud, MSG, from, MYMUD, to);
  else if (plev(plr) > LEVEL_ADMIN)
    aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D "%s is too powerful",
	    TO_MUD, frommud, MSG, from, MYMUD, to);
  else {
    sendf(plr, "You have been kicked off aberchat by %s@%s\n", from, frommud);
    pclrflg(plr, PFL_ABERCHAT);
    pclrmsk(plr, PFL_ABERCHAT);
    mudlog("ABERCHAT: %s has been akicked by %s@%s.", pname(plr), from, frommud);
  }
}

Boolean aberchat_filter(char type,char *arg1,char *arg2,char *arg3,char *arg4) {
  switch(type) {
#if 0
/* example akick filter:
 * arg1	: from_mud
 * arg2 : target_player
 * arg3 : from_player
 */
  case KICK:
    if(EQ(arg3,"foomud")) {
      int x;
      x = mynum;
      mynum = -1;
      aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D "Sorry, your mud isnt allowed to akick anyone here.",
	    TO_MUD, arg3, MSG, arg1, MYMUD);
      mynum = x;
      return True;
    }
    break;
#endif
#if 0
/* example atell filter:
 * arg1	: from_player
 * arg2	: target_player
 * arg3	: from_mud
 * arg4 : text
 */
  case TELL:
    if(EQ(arg3,"foomud") && EQ(arg1,"spammer")) {
      int x;
      x = mynum;
      mynum = -1;
      aprintf("%c" D "%s" D "%c" D "%s" D "MUD %s" D "Sorry, No-one wants to talk to you anymore.",
	    TO_MUD, arg3, MSG, arg1, MYMUD);
      mynum = x;
      return True;
    }
    break;
#endif
#if 0
/* example achat/agod/acode filter:
 * arg1	: from_mud
 * arg2 : from_player
 * arg3 : text
 */
  case CHAT: /* or AGOD/ACODE */
    if((EQ(arg1,"foomud") && EQ(arg2,"fule")) || strstr(arg3,"bottom") ) {
      return True;
    }
    break;
#endif
#if 0
/* example ainfo filter:
 * arg1	: from_mud
 * arg2	: text
 */
  case AINFO:
    if(EQ(arg1,"spammud") || strstr(arg2,"bottom")) {
      return True;
    }
    break;
#endif
  }
  return False;
}

Boolean aberchat_parse(int vb)
{
  switch (vb) {
  case VERB_NOACHAT: cmd_noachat(); break;
  case VERB_NOACODE: cmd_noacode(); break;
  case VERB_NOAGOD: cmd_noagod(); break;
  case VERB_NOAINFO: cmd_noainfo(); break;
  case VERB_NOATELL: cmd_noatell(); break;
  case VERB_ABOOT: cmd_aboot(); break;
  case VERB_ASHUTDOWN: cmd_ashutdown(); break;
  case VERB_AWHO: cmd_awho(); break;
  case VERB_NOABERCHAT: cmd_noaberchat(); break;
  case VERB_AKICK: cmd_akick(); break;
  case VERB_AAUTOCON: cmd_aautocon(); break;
  case VERB_AMUDLIST: cmd_amudlist(); break;
  case VERB_AVERSION: cmd_aversion(); break;
  case VERB_AMASSLIST: cmd_amasslist(); break;
  case VERB_APOLICY: cmd_apolicy(); break;
  case VERB_ACOPYRIGHT: cmd_acopyright(); break;
  case VERB_ATELL: cmd_atell(); break;
  case VERB_AINFO: cmd_ainfo(); break;
  case VERB_ACHAT: cmd_achat(CHAT); break;
  case VERB_ACODE: cmd_achat(ACODE); break;
  case VERB_AGOD: cmd_achat(AGOD); break;
  case VERB_AREPLY: cmd_areply(); break;
  case VERB_ACOMMSTATS: cmd_acommstats(); break;
  default:
    return False;
  }
  return True;
}
