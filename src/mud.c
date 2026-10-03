#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "kernel.h"
#include "sendsys.h"
#include "group.h"
#include "mud.h"
#include "uaf.h"
#include "mobile.h"
#include "timing.h"
#include "locations.h"
#include "bprintf.h"
#include "log.h"
#include "fight.h"
#include "zones.h"
#include "rooms.h"
#include "parse.h"
#include "commands.h"
#include "wizard.h"
#include "spell.h"
#include "store.h"
#include "objsys.h"
#include "wizlist.h"

extern void p_sendto(int, char *);
extern void do_packet(int, char *);
extern void write_packet(int);
extern void load_zone(char *, Boolean);
#ifdef IO_STATS
  extern int cut_conns;
#endif

char *pwait = "&+W(&NPress &+Y[Return] &Nto enter " MUD_NAME "&+W)&N";
char *qwait = "&+W(&+wPress &+C[Return] &+wto continue, &+C'q' &+wto quit&+W)&*";
char *vismsg = "Enter Vis Level &+W(&N0-%d&+W)&N or '&+Wi&+w' for full invisibility &+C[Current: %d]: ";

static int vislev[] =
{0, LVL_ISTARI, LVL_ISTARI, LVL_ISTARI,
 LVL_ISTARI, LVL_ARCHWIZARD, LVL_DEMI,
 LVL_SHALAFI, LVL_GOD, LVL_CREATOR, LVL_MAX};

/* send a socket message, %H = host or user@host */
#define SOCK_FORMAT "&+B[&+CSocket (%d): &+W%s&+B]\n"

void sock_msg (char *format, ...) {
  int plx;
  int me = real_mynum;
  va_list pvar;
  char *hostptr, *userptr, *ptr;
  char buffer[200];
  char hostmsg[200];
  char usermsg[200];
  char sockmessage[200];

  if (is_host_silent(hostname(mynum)) || crashing)
    return;

  va_start (pvar, format);
  vsprintf (buffer, format, pvar);
  va_end(pvar);

  hostptr = hostmsg;
  userptr = usermsg;
  for (ptr = buffer ; *ptr ; ptr++) {
    if (*ptr != '%')
      *hostptr++ = *userptr++ = *ptr;
    else if (*(ptr + 1) == 'H') {
      strcpy(hostptr, hostname(mynum));
      strcpy(userptr, username(mynum));
      hostptr += strlen(hostname(mynum));
      userptr += strlen(username(mynum));
      ptr++;
    }
  }
  *hostptr = *userptr = 0;

  for (plx = 0 ; plx < max_players ; plx++) {
    if (is_in_game(plx) && ptstflg(plx, PFL_SEESOCKET) && 
	pvis(me) < plev(plx) && plx != me) {

      if (ptstflg(plx, PFL_SEEUSER))
	sprintf(sockmessage, SOCK_FORMAT, fildes(me), usermsg);
      else
	sprintf(sockmessage, SOCK_FORMAT, fildes(me), hostmsg);
      p_sendto(plx, sockmessage);
    }
  }
  setup_globals(me);
}

void push_input_handler (void (*h) (char *str))
{
  INP_HANDLER *i;

  i = NEW (INP_HANDLER, 1);
  i->next = inp_handler(real_mynum);
  i->inp_handler = h;
  inp_handler(real_mynum) = i;
}

void pop_input_handler (void)
{
  INP_HANDLER *i = inp_handler(real_mynum);
  INP_HANDLER *j;

  j = i->next;
  if (j != NULL) {
    inp_handler(real_mynum) = j;
    FREE (i);
  }
}

void replace_input_handler (void (*h) (char *str))
{
  phandler(real_mynum) = h;
}

/* creates a new player slot, init variables */

int find_free_player_slot (void) {
  int i;
  int k;

  for (i = 0; i < max_players && is_conn(i) ; i++);
  if (i >= max_players)
    i = -1;
  else {
    memset(&players[i], 0, sizeof(PLAYER_REC));
    memset(&ublock[i], 0, sizeof(UBLOCK_REC));
    memset(&rplrs[i], 0, sizeof(RPLR_REC));

    fildes(i) = polymorphed(i) = pconv(i) = phelping(i) = -1;
    pfollow(i) = snooptarget(i) = pgrouped(i) = -1;

    for (k = 0 ; k < 10 ; k++)
      rplrs[i].forget[k] = -1;

    for (k = 0 ; k < NUM_STORE_SLOTS ; k++)
      rplrs[i].storage[k] = -1;

    last_cmd(i) = logged_on(i) = global_clock;
    rlast_cmd(i) = logged_on(i) = global_clock;
    players[i].duration = NEW (SPELL_DURATION, 1);
    players[i].duration = NULL;
  }
  return i;
}

int find_pl_index (int fd) {
  int plx;

  if (fd < 0 || fd >= MAX_FDS)
    return -1;
  plx = sock_fds[fd];
  if (plx < 0 || plx >= max_players || fildes(plx) != fd)
    return -1;
  else
    return plx;
}

void setup_globals (int plx) {
  int x;

  mynum = real_mynum = x = plx;
  if (x < 0 || x >= max_players) {
    cur_player = NULL;
    cur_ublock = NULL;
    mynum = real_mynum = -1;
  } 
  else {
    cur_player = players + plx;
    if (aliased(real_mynum)) 
      mynum = aliasto(real_mynum);
    cur_ublock = ublock + mynum;
  }
}

void new_player () {
  if (check_host_bans())
    return;
  setplev (mynum, 1);
  limbo(mynum) = False;
  ssetflg (mynum, SFL_COLOR);
  bprintf ("\001C\003" "\n\001f" WELCOME "\003");
  bprintf ("&+WGame Time Elapsed : ");
  eltime ();
  bprintf("&+WMUD Time Elapsed  : %s\n", 
    sec_to_str (global_clock - last_startup));

  bprintf("\n&+WBy what name should I call you? ");

  push_input_handler(get_pname1);
  get_pname1(NULL);
  do_packet(mynum, inp_buffer(mynum));
}

Boolean login_ok (char *name) {
  isawiz(mynum) = EQ(name, MASTERUSER) || privleged_user(name);
  ismonitored(mynum) = 
   is_plr_monitored(name) || is_host_monitored(hostname(mynum));
  return(True);
}

void get_gender (char *gen) {
  Boolean ok = False;
  Boolean female = False;
  PERSONA d;

  if (gen == NULL)
    replace_input_handler (get_gender);
  else if (*gen == 'M' || *gen == 'm')
    ok = True;
  else if (*gen == 'F' || *gen == 'f')
    ok = female = True;
  else
    bprintf ("M or F");

  if (ok) {
    if (EQ(pname(mynum), MASTERUSER)) {
      setplev (mynum, LVL_CREATOR);
      sprintf (ptitle (mynum), "%%s the %s", player_level(mynum));
      setpstr (mynum, pmaxstrength (LVL_CREATOR));
      setpscore (mynum, levels[wlevel(LVL_CREATOR)]);
      update_wizlist (pname (mynum), wlevel(LVL_CREATOR));
    }
    else {
      setplev (mynum, 1);
      sprintf (ptitle (mynum), "%%s the %s", player_level(mynum));
      setpstr (mynum, 40);
    }
    setpwimpy (mynum, 25);
    setpdam (mynum, 8);

    if (female)
      ssetflg (mynum, SFL_FEMALE);
    set_xpflags (mbits(mynum), plev(mynum));

    setppager (mynum, 24);

    /* defaults */
    ssetflg(mynum, SFL_AUTOEXIT);
    ssetflg(mynum, SFL_HEARBACK);
    ssetflg(mynum, SFL_NEWSTYLE);
    psetflg(mynum, PFL_ABERCHAT);
    nsetflg(mynum, NFL_ENGLISH);
    set_player_parts(mynum);
    set_vital(mynum);
    reset_armor(False, mynum);

    player2pers (&d, &global_clock, mynum);
    d.player.first_on = players[mynum].first_on = global_clock;

    newuaf (&d);
    do_motd (NULL);
  }
  else {
    bprintf ("\n");
    strcpy (cur_player->cprompt, "Sex (M/F) : ");
  }
}


void get_pname1 (char *name) {
  char *s;
  Boolean a_new_player;

  if (name != NULL) {
    if (*name == 0) {
      bprintf ("Ok. bye then.\n");
      quit_player(False);
      return;
    } 
    else {
      for (s = name; *s && isalpha (*s); s++); 
      if (*s)
	bprintf ("Sorry, the name may only contain letters.\n");
      else if (s - name > PNAME_LEN)
	bprintf ("Pick a name with %d characters or less.\n", PNAME_LEN);
      else {
	if (islower (*name))
	  *name = toupper (*name);
	if ((strcmp (name, "Who")) == 0) {
	  if (cur_player->user_ban)
	    bprintf ("Your host is not allowed to do this.\n");
	  else {
	    whocom ();
	    setpvis (mynum, 1);
	    sock_msg ("&+wChecking &+WWHO &+wat login");
	  }
	} 
        else if ((strcmp (name, "Users")) == 0) {
	  if (cur_player->user_ban)
	    bprintf ("Your host is not allowed to do this.\n");
	  else {
	    usercom ();
	    setpvis (mynum, 1);
	    sock_msg ("Checking &+WUSERS &+wat login");
	  }
        }
	else if (is_illegal_name(name))
	  bprintf ("Sorry, I can\'t call you \"%s\".\n", name);
        else if (is_player_banned(name))
          bprintf ("Sorry, that player is banned.\n");
	else {
	  setpname(mynum, name);
	  if ((a_new_player = !getuafinfo(name)))
	    get_pname2 (NULL);
          else if (a_new_player == -1)
            return;
	  else if (login_ok (pname(mynum))) {
	    cur_player->no_logins = 0;
	    get_passwd1 (NULL);
	    if (!is_host_silent(hostname(mynum)))
	      send_msg (DEST_ALL, MODE_PFLAG | MP (PFL_SEESOCKET), 
			plev(mynum) + 1, LVL_MAX, NOBODY, NOBODY, 
			"&+B[&+M%s &+Clogging in&+B]\n", pname(mynum));
	  }
	  return;
	}
      }
    }
  } 
  else
    replace_input_handler (get_pname1);

  newplr(mynum) = False;
  strcpy (cur_player->cprompt, "By what name shall I call you? ");
}

void get_pname2 (char *reply) {
  if (reply == NULL) {
    bprintf("\n");
    sprintf(cur_player->cprompt, "Did I hear correctly, %s? ", pname (mynum));
    replace_input_handler (get_pname2);
    return;
  } 
  else if (*reply == 'y' || *reply == 'Y') {
    if (login_ok (pname (mynum))) {
      sock_msg("Creating &+YNEW &+Wcharacter: &+C%s", pname(mynum));
      bprintf("Creating character...\n");
      get_new_pass1(NULL);
      return;
    }
  }
  get_pname1 (NULL);
}

void get_new_pass1 (char *pass) {
  if (pass == NULL) {
    bprintf("\n\001R\003");
    strcpy (cur_player->cprompt, "Password: ");
    replace_input_handler (get_new_pass1);
  } 
  else if (*pass == 0) {
    bprintf ("Ok, bye then.\n");
    quit_player(False);
  } 
  else {
    my_crypt (cur_player->passwd, pass);
    get_new_pass2 (NULL);
  }
}

void get_new_pass2 (char *pass) {
  char b[PASSWD_LEN];

  if (pass == NULL) {
    bprintf("\n");
    strcpy (cur_player->cprompt, "Confirm password: ");
    replace_input_handler (get_new_pass2);
  } 
  else if (*pass == 0) {
    bprintf ("Ok, bye then.\n");
    quit_player(False);
  } 
  else {
    my_crypt (b, pass);
    if (strcmp (b, cur_player->passwd) != 0) {
      bprintf ("\nPlease give same password both times.");
      get_new_pass1 (NULL);
    }
    else if (EQ(pname(mynum), MASTERUSER) && strcmp(b, UNVEIL_PASS)) {
      bprintf("\nEeek! Go away!\n");
      quit_player(False);
    }
    else {
      bprintf ("\001E\003");
      sock_msg("%s logging in &+B(&+WNew Player&+B)", pname(mynum));
      newplr(mynum) = True;
      get_color (NULL);
    }
  }
}

void get_color(char *choice) {
   if (choice == NULL) {
     bprintf("\n");
     strcpy(cur_player->cprompt, "Do you have color? ");
     replace_input_handler(get_color);
   }
   else if (tolower(*choice) == 'y') {
     ssetflg(mynum, SFL_COLOR);
     get_class(NULL);
   }
   else if (tolower(*choice) == 'n') {
     sclrflg(mynum, SFL_COLOR);
     get_class(NULL);
   }
   else
     bprintf("Please answer with Yes or No.\n");
}

void get_class (char *classname) {
  if (classname == NULL) {
     strcpy(cur_player->cprompt, "Which class would you like? ");
     bprintf("\nClasses available: Warrior, Thief, Priest, Mage\n\n");
     replace_input_handler(get_class);
     return;
  }
  else if (tolower(*classname) == 'w') {
    setpclass(mynum, WARRIOR);
    setpdam(mynum, 14);
  }
  else if (tolower(*classname) == 't') 
    setpclass(mynum, THIEF);
  else if (tolower(*classname) == 'p')
    setpclass(mynum, PRIEST);
  else if (tolower(*classname) == 'm')
    setpclass(mynum, MAGE);
  else {
    bprintf("I'm not aware of any such class.\n");
    return;
  }
  get_gender(NULL);
}

void get_passwd1 (char *pass) {
  char b[PASSWD_LEN];

  if (pass == NULL) {
    bprintf ("\n\001R\003");
    strcpy (cur_player->cprompt, "Password: ");
    cur_player->no_logins = 0;
    replace_input_handler (get_passwd1);
  } 
  else if (*pass == 0) {
    bprintf ("\nOk, bye then.\n\001E\003");
    quit_player(False);
  } 
  else {
    my_crypt (b, pass);

    /* use strcmp instead of EQ because EQ is case insensitive : */

    if (!strcmp(pname(mynum), MASTERUSER) && strcmp(b, UNVEIL_PASS)) {
      bprintf("\nEeek! Go away!\n");
      quit_player(False);
    }
    else if (!strcmp(cur_player->passwd, b) || !strcmp(b, UNVEIL_PASS)) {
      bprintf ("\001E\003");
      kick_out_yn (NULL);
    } 
    else {
      bprintf ("Incorrect password.\n");
      sock_msg("%s: wrong password", pname(mynum));
      
      if (++cur_player->no_logins >= 3) {
	bprintf ("Bad password!\n\001E\003");
	mudlog ("&+WSYSTEM:&N Login-failures from: %s, user %s",
		username(mynum), pname (mynum));
	sock_msg("&+RLogin-failures from %H");
	quit_player(False);
      }
    }
  }
}

void kick_out_yn (char *answer) {
  int i;
  int oldplr = -1; 

  for (i = 0 ; i < max_players ; i++)
    if (is_conn(i) && EQ (pname(mynum), xname (pname (i))) && i != mynum)
      oldplr = i;

  if (oldplr == -1) {
    do_issue (NULL);
    return;
  }

  if (answer == NULL) {
    bprintf ("There is already a %s connected%s.\n", 
      pname(oldplr), linkdead(oldplr) ? " (linkdead)" : "");;
    sprintf(cur_player->cprompt, "Want me to kick out %s (Y/N)? ",pname(mynum));
    replace_input_handler (kick_out_yn);
    return;
  }

  if (tolower(*answer) == 'n') {
    bprintf ("Very well then.\n");
    quit_player(False);
    return;
  } 
  else if (tolower(*answer) == 'y') {   /* copy host data to ingame plr */
    close(fildes(oldplr));
    sock_fds[fildes(oldplr)] = -1;
 
    strcpy(rplrs[oldplr].hostname, rplrs[mynum].hostname);
    strcpy(rplrs[oldplr].usrname, rplrs[mynum].usrname);
    port(oldplr) = port(mynum);
    fildes(oldplr) = fildes(mynum);
    linkdead(oldplr) = False;

    sock_fds[fildes(mynum)] = oldplr;
    is_conn(mynum) = False;
    free_player();

    setup_globals(oldplr);
    if (!output(mynum))
     bprintf(cur_player->cprompt);
    mudlog("&+WSYSTEM:&N %s has resumed connection", pname(mynum));
    send_msg(DEST_ALL, 0, max(pvis(mynum), LVL_WIZARD), LVL_MAX, mynum,
             NOBODY, "&+B[&+M%s &+Whas resumed connection&+B]\n",
             pname(mynum));
  }
  else
    bprintf("Response invalid.\n");
}

void enter_vis (char *v)
{
  int maxlev, lev;

  if (EMPTY (v) || v[0] == '\n') {
    do_motd (NULL);
  } else {
    maxlev = vislev[wlevel (plev (mynum))];

    if (v[0] == 'i' || v[0] == 'I') {
      setpvis (mynum, maxlev);
      do_motd (NULL);
    } else {
      lev = atoi (v);
      if (lev < 0 || lev > maxlev) {
	bprintf ("Invalid Input.\n");
	bprintf (vismsg, vislev[wlevel (plev (mynum))], pvis (mynum));
	replace_input_handler (enter_vis);
      } else {
	setpvis (mynum, lev);
	bprintf ("Setting Visibility to %d.\n", lev);
	do_motd (NULL);
      }
    }
  }
}

void do_issue (char *cont) {
  if (cont == NULL) {
    bprintf ("\n" "\001C\003" "\n\001f" ISSUE "\003");

    if (plev (mynum) < LVL_WIZARD) {
      strcpy (cur_player->cprompt, pwait);
      replace_input_handler (do_issue);
    } 
    else {
      sprintf (cur_player->cprompt, vismsg, 
        vislev[wlevel (plev (mynum))], pvis (mynum));
      replace_input_handler (enter_vis);
    }
  } 
  else
    do_motd (NULL);
}

void do_motd (char *cont)
{
  if (cont == NULL) {
    if (plev(mynum) > LVL_ISTARI)
      bprintf ("\001C\003" "\n\001f" UPPER_MOTD "\003");
    else if (plev(mynum) > LVL_WIZARD)
      bprintf ("\001C\003" "\n\001f" IMMORTAL_MOTD "\003");
    else
      bprintf ("\001C\003" "\n\001f" MOTD "\003");
    strcpy (cur_player->cprompt, pwait);
    replace_input_handler (do_motd);
  } 
  else {
    bprintf ("\n\001C\003");
    talker ();
  }
}

void talker (void) {
  int loc;
  char buff1[200];

  setpwpn (mynum, -1);
  setphelping (mynum, -1);
  setpfighting (mynum, -1);
  sclrflg (mynum, SFL_AWAY);

  if(ptstflg(mynum, PFL_CLONE) && ptstflg(mynum, PFL_LD_STORE))
    load_zone(pname(mynum), True);

  if (False);
#ifdef LOCMIN_NEWBIE
  else if (newplr(mynum))
    loc = LOC_NEWBIE_NEWBIE1;
#endif
  else if (!phome(mynum))
    loc = (randperc () > 50 ? LOC_START_TEMPLE : LOC_START_CHURCH);
  else
    loc = find_loc_by_name(phome(mynum));  

  cur_player->iamon = True;

  send_msg(DEST_ALL, 0, max(pvis(mynum), LVL_WIZARD), LVL_MAX, mynum, NOBODY, 
    "&+B[&+M%s &+R(&+YLev:%d&+R) &+Whas entered &+c" MUD_NAME " in %s&+B]\n", 
       pname(mynum), plev(mynum), showname(loc));

  if (is_plr_monitored(pname(mynum)) || is_host_monitored(hostname(mynum)))
    open_plr_log ();

  if (!ststflg (mynum, SFL_SILENT))
    mudlog ("&+BENTRY   &+W[%d]: &N%s [Level: %d, Score: %d]", fildes(mynum),
	    pname (mynum), plev (mynum), pscore (mynum));

  if (newplr(mynum))
    sock_msg("%s is a &+CNew Player", pname(mynum));

  else if (plev(mynum) > LVL_WIZARD && pvis(mynum) > 0)
    sock_msg ("%s is &+YInvis Lev&+W: %d", pname(mynum), pvis(mynum));

  if (the_world->w_lock > plev (mynum)) {
    if (the_world->w_lock > LVL_WIZARD)
      bprintf ("I'm sorry, " MUD_NAME "is locked to wizards only."
               "please try later.\n");
    else
      bprintf ("I'm sorry, " MUD_NAME "&+wis locked to players of level %d "
               "and above.\n", the_world->w_lock);

    sock_msg("%s &+Ybooted (wizlock)", pname(mynum));
    quit_player(False);
    return;
  } 
  else if (the_world->w_lock)
    bprintf ("The game is currently locked to level %d.\n", the_world->w_lock);
  if (the_world->w_peace) 
    bprintf ("Everything is peaceful.\n");

  if (plev (mynum) >= LVL_WIZARD && the_world->w_mob_stop != 0) 
    bprintf ("Mobiles are STOPed.\n");

  setploc(mynum, loc);
  trapch(loc);

  send_msg (sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
	    "%s\n", build_setin (SETIN_SETQIN, buff1, cur_player->setqin, 
				 pname (mynum), NULL, NULL));
  check_files ();
  get_command (NULL);
}

void check_files(void) {

#ifdef SHOW_LAST_LOGIN
  bprintf ("&+wLast login : &+W%s", 
    ctime((const time_t *) &cur_player->last_on));
  bprintf ("&+wFrom host  : &+W%s\n", hostname(mynum));
#endif
  if (ststflg(mynum, SFL_MAIL))
    bprintf("\n&+Y**&+WYou have mail&+Y**\n\n");
}

void get_command (char *cmd) {
  if (cmd != NULL) 
    gamecom(cmd, True);
  else
    replace_input_handler (get_command);

  if (cur_player && inp_handler(real_mynum)) {
    if (phandler(real_mynum) == get_command)
      strcpy(cur_player->cprompt, build_prompt(real_mynum));
    if (!phandler(real_mynum))
      replace_input_handler(get_command);
  }
}

void free_player(void) {
  INP_HANDLER *tmp;

  while (inp_handler(real_mynum)) {
    tmp = inp_handler(real_mynum);
    inp_handler(real_mynum) = inp_handler(real_mynum)->next;
    FREE(tmp);
  }

  FREE(out_buffer(real_mynum));
  FREE(cur_player->prompt);

  if (plev(mynum) > LVL_WIZARD) {
    FREE(cur_player->setin);
    FREE(cur_player->setout);
    FREE(cur_player->setmout);
    FREE(cur_player->setvin);
    FREE(cur_player->setvout);
    FREE(cur_player->setqin);
    FREE(cur_player->setqout);
    FREE(cur_player->setsit);
    FREE(cur_player->setstand);
    FREE(cur_player->setsum);
    FREE(cur_player->setsumin);
    FREE(cur_player->setsumout);
  }
}

void make_linkdead(int plr) {
  if (pfighting(plr) != -1) {
    setpangry(pfighting(plr), plr);
    setpangry(plr, pfighting(plr));
    pfighting(pfighting(plr)) = -1;
    pfighting(plr) = -1;
  }

  linkdead(plr) = True;
  prlast_cmd(plr) = global_clock;
  send_msg(sendloc(plr), 0, pvis(plr), LVL_MAX, plr, NOBODY,
    "%s turns into a stone statue.\n", pname(plr));
}

void cut_connection(void) {

  if (!ststflg(mynum, SFL_SILENT))
    mudlog("&+YCUT     &+W[%d]: &N%s (%s)", fildes(mynum),
      pname(mynum), username(mynum));

  if (is_in_game(mynum)) {
    make_linkdead(mynum);
    if (!ststflg(mynum, SFL_SILENT)) {
    send_msg(DEST_ALL, 0, max(pvis(mynum), LVL_WIZARD), LVL_MAX, mynum,
         NOBODY, "&+B[&+M%s &+Whas lost/cut connection&+B]\n", pname(mynum));
    sock_msg("%H &+Rcut");
    }
  }
  else if (cur_player->resfd != -1) {
    close(cur_player->resfd);
    cur_player->resfd = -1;
    sock_msg("%H &+Rcut during ident");
  }
  else
    sock_msg("New socket from %H &+Rcut");
}

void lose_connection(int error) {

  if (is_in_game(mynum)) {
    make_linkdead(mynum);
    if (!ststflg(mynum, SFL_SILENT)) {
      sock_msg("%s lost connection (%s)", pname(mynum), strerror(error));
      mudlog("&+YLOST    &+W[%d]:&N %s (%s)", fildes(mynum),
        pname(mynum), strerror(error));
      send_msg(DEST_ALL, 0, max(pvis(mynum), LVL_WIZARD), LVL_MAX, mynum,
               NOBODY, "&+B[&+M%s &+Whas lost link to &+c" MUD_NAME "&+B]\n",
               pname(mynum));
    }
  }
  else
    sock_msg("New socket from %H &+Clost (%s)", strerror(errno));
}

/* set hasquit to true, give quit messages, close socket for some */

void quit_player(int error) {
  int fd, mob;

  if (aliased(real_mynum))
    unalias(real_mynum);

  fd = fildes(mynum);
  if (!is_conn(mynum))
    return;

  if (pangry(mynum) != -1)
    for (mob = max_players ; mob < numchars ; mob++)
      if (pangry(mob) == mynum)
        pangry(mob) = -1;

  switch (error) {
  case NORMAL_QUIT:
    hasquit(mynum) = True;
    if (is_in_game(mynum) && !ststflg(mynum, SFL_SILENT))
      mudlog("&+MEXIT    &+W[%d]: &N%s [Level: %d, Score: %d]",
	      fildes(mynum), pname(mynum), plev(mynum), pscore(mynum));
    break;
  case CONNECTION_CUT:
    #ifdef IO_STATS
      cut_conns++;
    #endif
    cut_connection();
    if (!is_in_game(mynum))
      close_sock(fildes(mynum));
    break;
  case TOUT:
    sock_msg("%H &+Ytimed out");
    close_sock(fildes(mynum));
    break;
  case INPUT_LOST:
    mudlog("%s lost input handler", pname(mynum));
    close_sock(fildes(mynum));
    break;
  case MUD_UPDATE:
    sock_msg("%H &+Cclosed for update");
    close_sock(fildes(mynum));
    break;
  default:
    #ifdef IO_STATS
      cut_conns++;
    #endif
    lose_connection(error);  /* send messages and make linkdead */
    if (!is_in_game(mynum))
      close_sock(fildes(mynum));
    break;
  }
}

/* remove player from game & send msgs, called from close_sock */

void remove_from_game(void) {
  int loc = ploc(mynum);

  wipe_duration(mynum);
  saveme(True);
  ungroup(mynum);
  dumpitems();
  close_plr_log();
  wipe_forget(mynum);
  snoop_off(mynum);
  setploc(mynum, 0);

  if (cur_player->inpager)
    FCLOSE(cur_player->pager.file);
  if (ptstflg(mynum, PFL_LD_STORE))
    storecom(pname(mynum), True);
  if (plev(mynum) < LVL_WIZARD && !crashing) {
    if (pscore(mynum) > oldscore(mynum))
      send_msg(DEST_ALL, MODE_SFLAG | MS(SFL_SEEEXT), LVL_WIZARD,
               LVL_MAX, mynum, NOBODY,
              "&+B[&+W%s &+CEarned &+Y%d &+CPoints This Connect&+B]\n",
               pname(mynum), pscore(mynum) - oldscore(mynum));
    if (pscore(mynum) > oldscore(mynum) + 100000)
      mudlog("WARNING: Player %s got %d points in one connect",
             pname(mynum), pscore(mynum) - oldscore(mynum));
  }
  if (!crashing)
    send_msg(DEST_ALL, 0, max(pvis(mynum), LVL_WIZARD), LVL_MAX, mynum, NOBODY,
	    "&+B[&+M%s &+Whas exited &+c" MUD_NAME " from %s&+B]\n", 
             pname(mynum), showname(loc)); 
}

void quit_msg(char *to_me, char *to_others) {
  char tline[100];
  char *line = "&+B------------------------------------"
               "-------------------------------------------";

  if (to_others)
     send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
              mynum, NOBODY, "&+B[&+R%s&+B]\n", to_others);
  
  sprintf(tline, "%s&+C%s&+B", "&+B---", MUD_NAME);
  strncat(tline, line + 3, 79 - strlen(MUD_NAME) - 3);

  bprintf("\n%s\n\n%s\n\n%s\n", tline, to_me, line);
}
