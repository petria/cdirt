#include <dirent.h>
#include <stdlib.h>
#include "kernel.h"
#include "parse.h"
#include "verbs.h"
#include "sendsys.h"
#include "locations.h"
#include "objects.h"
#include "parse.h"
#include "mobile.h"
#include "move.h"
#include "commands.h"
#include "climate.h"
#include "wizard.h"
#include "change.h"
#include "clone.h"
#include "objsys.h"
#include "bprintf.h"
#include "uaf.h"
#include "actions.h"
#include "spell.h"
#include "viewcom.h"
#include "log.h"
#include "exit.h"
#include "wizlist.h"
#include "timing.h"
#include "frob.h"
#include "acct.h"
#include "zones.h"
#include "magic.h"
#include "mail.h"
#include "fight.h"
#include "rooms.h"
#include "mobiles.h"
#include "babel.h"
#include "god.h"
#include "comm.h"
#include "parse.h"
#include "pig.h"
#include "group.h"
#include "client.h"
#include "ban.h"
#include "store.h"
#include "xrscore.h"
#include "update.h"

#define NOISECHAR(c)	((c) == ' ' || (c) == '.' || (c) == ',' || (c) == '%')

char *preptable[] =
{ NULL, "at", "on", "with", "to", "in", "into", "from", "out", TABLE_END };

int     tree_lookup(T_elemptr, char *); 
void    hostscom(void);

Boolean allspaces(char *line) {
  char *p;
 
  for (p = line ; *p ; p++)
    if (!isspace(*p))
      return(False);

  return(True);
}

void gamecom (char *txt, Boolean savecom) {
  Actionptr act = NULL;
  Boolean ismacro = False;
  int a, p;

  if (txt == NULL || *txt == 0)
    return;
  else if (EQ (txt, "!")) {
    if (!plastcom(real_mynum)) {
      bprintf("No last command.\n");
      return;
    }
    return(gamecom(plastcom(real_mynum), False));
  }
  else if (!allspaces(txt)) {
    if (savecom) {
      if (plastcom(real_mynum))
        FREE(plastcom(real_mynum));
      plastcom(real_mynum) = COPY(txt);
    }
    if (mynum < max_players && pconv(mynum) != -1) {
      if (!is_in_game(pconv(mynum))) {
	bprintf("The person with you in converse mode has quit.\n"
                "Exiting Converse Mode\n\n");
	setpconv(mynum, -1);
	return;
      }
      if (*txt == '*' && *(txt + 1) == '*') {
	bprintf("Exiting Converse Mode\n\n");
	setpconv(mynum, -1);
	return;
      } 
      else if (*txt == '!')
	strcpy (strbuf, txt + 1);
      else
	sprintf (strbuf, "tell %s %s", pname (pconv(mynum)), txt);
    }
    else {
      ismacro = True;
      switch(*txt) {
        case '[':
          strcpy(strbuf, "gchat ");
          break;
        case '\"': case '\'':
          strcpy(strbuf, "say ");
          break;
        case '?':
          strcpy(strbuf, "help");
          break;
        case ':': case ';':
          strcpy(strbuf, "emote ");
          break;
        case '0':
          strcpy(strbuf, "chat ");
          break;
        case '1':
          strcpy(strbuf, "wiz ");
          break;
        case '2':
          strcpy(strbuf, "istari ");
          break;
        case '3':
          strcpy(strbuf, "awiz ");
          break;
        case '4':
          strcpy(strbuf, "demi ");
          break;
        case '5':
          strcpy(strbuf, "god ");
          break;
        case '6':
          strcpy(strbuf, "upper ");
          break;
        default:
          ismacro = False;
          break;
      }

      if (ismacro)
        strcat(strbuf, txt + 1);
      else
        strcpy(strbuf, txt);
    }

    stp = 0;

    brkword();

    if ((a = tree_lookup(verb_t, wordbuf)) != -1)
      doverb(a);
    else if ((act = find_act(wordbuf, actions)))
      do_action(act);
    else if ((p = find_mob(False, wordbuf)) != -1)  /* search only plrs */
      tellcom(p);
    else
      bprintf("Pardon?\n");
  }
  special_events(mynum);
  return;
}

/* replace it, me and etc with PN values */

/* position text after whitespace */

char *postxt(char *str, char c) {
  static char *cpos;

  for (cpos = str ; *cpos ; cpos++)
    if (isspace(*cpos))
      return cpos + 1;
  return NULL;
}

/* copy up to a whitespace from from to to */

void wstrcpy(char *to, char *from) {
  int i;

  for (i = 0 ; from[i] && !isspace(from[i]); i++)
    to[i] = from[i];
  to[i] = 0;
}

/* setup all globals */

void mkglobals(char *inpstr) {
  char *ptr;
  static char c = 0;
  int numwords = 0;
  char mkbuff[MAX_COM_LEN];

  if ((txt1 = postxt(inpstr, ' ')))
    if ((txt2 = postxt(txt1, ' ')))
      txt3 = postxt(txt2, ' ');

  if (!txt1)
    txt1 = &c;
  if (!txt2)
    txt2 = &c;
  if (!txt3)
    txt3 = &c;

  *item1 = *item2 = *item3 = 0;
  pl1 = pl2 = pl3 = -1;
  ob1 = ob2 = ob3 = -1;

  if ((ptr = postxt(inpstr, ' ')) == NULL)  /* no whitespace found */
    return;
  
  do {
    wstrcpy(mkbuff, ptr);
    if (glookup(mkbuff, 0, preptable, xstrcasecmp) != -1)
      continue;
    else if (numwords == 0) {
      strcpy(item1, mkbuff);
      pl1 = fmbn(mkbuff);
      ob1 = fobn(mkbuff);
    }
    else if (numwords == 1) {
      strcpy(item2, mkbuff);
      pl2 = fmbn(mkbuff);
      ob2 = fobn(mkbuff);
    }
    else {
      strcpy(item3, mkbuff);
      pl3 = fmbn(mkbuff);
      ob3 = fobn(mkbuff);
      return;
    }
    numwords++;
  } while ((ptr = postxt(ptr, ' ')));
}

char *getreinput (char *b) {
  while (strbuf[stp] == ' ')
    stp++;
  return b != NULL ? strcpy (b, &strbuf[stp]) : &strbuf[stp];
}

/* i really hate this function : */

int brkword (void) {
  int worp;

  while (1) {
    while (NOISECHAR(strbuf[stp]))
      stp++;

    worp = 0;
    while (strbuf[stp] && !NOISECHAR(strbuf[stp]))
      wordbuf[worp++] = strbuf[stp++];

    wordbuf[worp] = 0;
    lowercase(wordbuf);

    if (EQ(wordbuf, "the") || EQ(wordbuf, "a") || EQ(wordbuf, "an") ||
        EQ(wordbuf, "of") || EQ(wordbuf, "with"))
      continue;
    else
      break;
  }
  return (worp ? 0 : -1);
}

int chklist (char *word, char *lista[], int listb[]) {
  int a;

  for (a = 0; lista[a]; a++)
   if (!strncasecmp(word, lista[a], strlen(word)))
     break;
  
  if (!lista[a])
    return(-1);
  else
    return (listb[a]);
}

void doverb (int vb) {
  char *ptr;
  char buf[MAX_COM_LEN];

#ifdef LOCMIN_VILLAGE
  if (ploc(mynum) == LOC_VILLAGE_EMERGENCY && plev(mynum) < LVL_WIZARD) {
    bprintf("You're unconscious.\n");
    return;
  }
#endif

  switch(vb) {
  case VERB_GOSSIP:
  case VERB_GCHAT:
  case VERB_WIZ:
  case VERB_AWIZ:
  case VERB_ISTARI:
  case VERB_DEMI:
  case VERB_GOD:
  case VERB_UPPER:
  case VERB_ANON:
  case VERB_SAY:
  case VERB_SHOUT:

    if (ststflg(mynum, SFL_BABEL)) {
      if ((ptr = strchr(strbuf, ' ')) == NULL)
	break;

      strcpy(ptr + 1, babeltext[my_random() % arraysize(babeltext)]); 
    }

    if (ststflg(mynum, SFL_ISPIG))
      {
	char buff[MAX_COM_LEN * 3] = {'\0'};

	if ((ptr = strchr(strbuf, ' '))  == NULL)
	  break;

	eng2pig(buff, ptr);

	if (strlen(buff) > MAX_COM_LEN - 20) {
	  bprintf("Input too long.  Oink.\n");
	  return;
	}
	else {
	  strcpy(ptr, "&+M(pig)&+W");
	  ptr += 11;
	  strcpy(ptr, buff);
	}
      }
  }

  mkglobals(strbuf);

  if (vb >= VERB_NORTH && vb <= VERB_DOWN) {
    if (!has_foot(mynum) && has_hand(mynum)) {
      bprintf("You're missing legs and feet.  You must CRAWL in a direction\n");
      return;
    }
    else if ((!has_foot(mynum) && !has_hand(mynum)) || 
      ststflg(mynum, SFL_CRIPPLED)) {
      bprintf("You're totally crippled.  Prey to the gods you live.\n");
      return;
    }
    else if (mynum < max_players && pfollow(mynum) >= 0) {
      bprintf("You stopped following.\n");
      pfollow(mynum) = -1;
    }

    dodirn(False, vb);
    return;
  }

  if (cur_player->inmailer) {
    switch (vb) {
    case VERB_ALIAS:
    case VERB_MAIL:
    case VERB_BECOME:
    case VERB_QUIT:
      bprintf ("You can't do that in the mailer.\n");
      return;
    }
  }

  if (aliased(real_mynum)) {
    switch(vb) {               /* things that don't work aliased */
    case VERB_GCHAT:
    case VERB_CHANNEL:
    case VERB_INFO:
    case VERB_WIZLIST:
    case VERB_ACTIONS:
    case VERB_POLICY:
    case VERB_CONVERSE:
    case VERB_OPTIONS:
    case VERB_KLOCK:
    case VERB_SAVE:
    case VERB_GROUP:
    case VERB_LOSE:
    case VERB_FLEE:
      bprintf ("Not while aliased!\n");
      return;
    }
  }
  if (cur_player->isforce) {

    switch (vb) {
    case VERB_QUIT:
    case VERB_BECOME:
    case VERB_BUG:
    case VERB_TYPO:
    case VERB_FORCE:
    case VERB_MAIL:
    case VERB_KLOCK:


      bprintf ("You can't be forced to do that.\n");
      return;
    }
  }
 
  if (aberchat_parse(vb) || flags_parse(vb))
    return;


  switch (vb) {
  case VERB_GO:
    if (mynum < max_players && pfollow(mynum) >= 0) {
      bprintf ("You stopped following %s.\n", pname (pfollow(mynum)));
      pfollow(mynum) = -1;
    }
    dogocom (False);
    break;

  case VERB_ICESTORM:
    spellcom(VERB_ICESTORM);
    break;
  case VERB_PROMOTE:
    promote();
    break;
  case VERB_GTELL:
    gtell();
    break;
  case VERB_GROUP:
    group();
    break;
  case VERB_REPORT:
    reportcom();
    break;
  case VERB_GLIST:
    glist();
    break;
  case VERB_MUSERS:
    muserscom();
    break;
  case VERB_IUSERS:
    iuserscom();
    break;
  case VERB_HOSTS:
    hostscom();
    break;
  case VERB_QLIST:
    qlistcom();
    break;                                                                     
  case VERB_BANLIST:
    banlistcom();
    break;
  case VERB_FILL:
    fillcom();
    break;
  case VERB_RUB:
    rubcom();
    break;
  case VERB_TIE:
    tiecom();
    break;
  case VERB_UNTIE:
    untiecom();                                                                
    break;
  case VERB_OPEN:
    opencom();
    break;
  case VERB_THROW:
    throwcom();
    break;
  case VERB_DIG:
    digcom();
    break;
  case VERB_ERASEZONE:
    erasezonecom();
    break;
  case VERB_CONFIGURE:
    if (plev(mynum) > LVL_WIZARD)
      configurecom();
    break;
  case VERB_EDIT:
    editcom();
    break;
  case VERB_VOTE:
    votecom(NULL);
    break;
  case VERB_NOPIG:
    nolinecom (LVL_MIN, SFL_NOPIG, "&+MPiglatin");
    break;
  case VERB_PIG:
    pigcom();
    break;
  case VERB_MAKEPIG:
    spellcom(VERB_MAKEPIG);
    break;
  case VERB_BABEL:
    spellcom(VERB_BABEL);
    break;
  case VERB_NOSWEAR:
    togglecom(SFL_NOSWEAR, "NoSwear Enabled", "NoSwear Disabled");
    break;
#ifdef DEBUG_IO
  case VERB_FDLIST:  
    fdlistcom(NULL);
    break; 
#endif 
  case VERB_SAY:
    saycom();
    break;
  case VERB_TELL:
    tellcom(-1);
    break;
  case VERB_SUGGEST:
    suggestcom();
    break;
  case VERB_LSAY:
    lsaycom ();
    break;
  case VERB_LANG:
    langcom ();
    break;
  case VERB_SAYTO:
    saytocom ();
    break;
  case VERB_CHALLENGE:
    challengecom ();                                                           
    break;
  case VERB_ARMOR:
    armorcom ();
    break;
  case VERB_EQUIPMENT:
    equcom ();
    break;
  case VERB_LOOT:
    lootcom ();
    break;
  case VERB_BUY:
    buycom ();
    break;
  case VERB_RESTORE:
    restorecom ();
    break;
  case VERB_CRAWL:
    crawlcom ();
    break;
  case VERB_BONG:
    bongcom();
    break;
#ifdef BOB
  case VERB_BOB:
    bobcom(txt1, mynum, True);
    break;
#endif
  case VERB_BACKSTAB:
    backstabcom();
    break;
  case VERB_SNEAK:
    sneakcom();
    break;                                                                     
  case VERB_PICK:
    pickcom();
    break;
  case VERB_BASH:
    bashcom();
    break;                                                                     
  case VERB_PLAY:
    playcom();
    break;
  case VERB_DEBUG:
    debugcom();
    break;
  case VERB_NORUN:
    noruncom ();
    break;
  case VERB_REPLY:
    replycom ();
    break;
  case VERB_ROSE:
    rosecom ();
    break;
  case VERB_WIPE:
    wipecom ();
    break;
  case VERB_FLUSH:
    flushcom ();
    break;
  case VERB_USE:
    usecom ();
    break;
  case VERB_UNLOCK:
    unlockcom ();
    break;
  case VERB_LOCK:
    lockcom ();
    break;
  case VERB_CLOSE:
    closecom ();
    break;
  case VERB_PROBATION:
    probationcom ();
    break;
  case VERB_LINK:
    linkcom ();
    break;
  case VERB_RING:
    ringcom ();
    break;
  case VERB_SHOUT:
    shoutcom ();
    break;
  case VERB_LOOK:
    lookcom ();
    break;
  case VERB_SCORE:
    xrscorecom ();
    break;
  case VERB_INVENTORY:
    inventory ();
    break;
  case VERB_GIVE:
    givecom ();
    break;
  case VERB_VALUE:
    valuecom ();
    break;
  case VERB_COMPARE:
    comparecom ();
    break;
  case VERB_WEARALL:
    wearall ();
    break;
  case VERB_BLUR:
    spellcom (VERB_BLUR);
    break;
  case VERB_BHANDS:
    spellcom (VERB_BHANDS);
    break;
  case VERB_AWAYMSG:
    awaymsgcom();
    break;
  case VERB_PROTECT:
    spellcom (VERB_PROTECT);
    break;
  case VERB_QINFO:
    qinfocom();
    break;
  case VERB_DAMAGE:
    spellcom (VERB_DAMAGE);
    break;
  case VERB_SPELLS:
    spellscom ();
    break;
  case VERB_PUNTALL:
    puntallcom ();
    break;
  case VERB_FREAQ:
    freaqcom ();
    break;
  case VERB_CONVERSE:
    conversecom ();
    break;
  case VERB_AID:
    spellcom (VERB_AID);
    break;
  case VERB_VTOUCH:
    spellcom (VERB_VTOUCH);
    break;
  case VERB_BANHOST:
    bancom (BAN_HOSTS, BANHOST);
    break;
  case VERB_BANCHAR:
    bancom (BAN_CHARS, BANCHAR);
    break;
  case VERB_BANACCT:
    bancom (BAN_ACCTS, BANACCT);
    break;
  case VERB_BANWHO:
    bancom (BAN_WHOS, BANWHO);
    break;
  case VERB_WEATHER:
    weathercom ();
    break;
  case VERB_MAXSTATE:
    maxstatecom ();
    break;
  case VERB_LOG:
    writelog ();
    break;
  case VERB_ATVIS:
    atviscom ();
    break;
  case VERB_SIC:
    siccom ();
    break;
  case VERB_POLICY:
    policycom ();
    break;
  case VERB_PLOC:
    ploccom ();
    break;
  case VERB_CFR:
    cfrcom ();
    break;
  case VERB_BURN:
    burncom ();
    break;
  case VERB_SILENT:
    silentcom ();
    break;
  case VERB_WLOAD:
    wloadcom ();
    break;
  case VERB_VIEWCOM:
    viewcom ();
    break;
  case VERB_KLOCK:
    klockcom (NULL);
    break;
  case VERB_OPTIONS:
    optionscom ();
    break;
  case VERB_PAGER:
    setpager ();
    break;
  case VERB_FAKEQUIT:
    fakequitcom ();
    break;
  case VERB_SETIN:
    set_msg (&(cur_player->setin), False, False);
    break;
  case VERB_SETOUT:
    set_msg (&(cur_player->setout), True, False);
    break;
  case VERB_SETMIN:
    set_msg (&(cur_player->setmin), False, False);
    break;
  case VERB_SETMOUT:
    set_msg (&(cur_player->setmout), False, False);
    break;
  case VERB_SETVIN:
    set_msg (&(cur_player->setvin), False, False);
    break;
  case VERB_SETVOUT:
    set_msg (&(cur_player->setvout), False, False);
    break;
  case VERB_SETQIN:
    set_msg (&(cur_player->setqin), False, False);
    break;
  case VERB_SETQOUT:
    set_msg (&(cur_player->setqout), False, False);
    break;
  case VERB_SETSIT:
    set_msg (&(cur_player->setsit), False, False); 
    break;
  case VERB_SETSTND:
    set_msg (&(cur_player->setstand), False, False);
    break;
  case VERB_SETSUM:
    set_msg (&(cur_player->setsum), False, False);
    break;
  case VERB_SETSIN:
    set_msg (&(cur_player->setsumin), False, True);
    break;
  case VERB_SETSOUT:
    set_msg (&(cur_player->setsumout), False, True);
    break;
  case VERB_NOPUNT:
    nopuntcom ();
    break;
  case VERB_FIND:
    findcom ();
    break;
  case VERB_FOLLIST:
    follist ();
    break;
  case VERB_FORCEALL:
    forceallcom ();
    break;
  case VERB_SEEEXT:
    toggleseeext ();
    break;
  case VERB_SIGNAL:
    signalcom ();
    break;
  case VERB_REBOOT:
    rebootcom ();
    break;
  case VERB_UPDATE:
    updatecom ();
    break;
  case VERB_CODING:
    togglecoding ();
    break;
  case VERB_SOCKET:
    socketcom ();
    break;
  case VERB_SEESOCKET:
    toggleseesocket ();
    break;
  case VERB_LIT:
    if (plev (mynum) < LVL_WIZARD)
      spellcom (VERB_LIT);
    else
      litcom ();
    break;
  case VERB_UPTIME:
    uptimecom ();
    break;
  case VERB_HEALALL:
    healallcom ();
    break;
  case VERB_SAVEALL:
    saveallcom ();
    break;
  case VERB_SHALL:
    if (plev (mynum) > LVL_WIZARD && ptstflg (mynum, PFL_HEAL)) {
      healallcom ();
      saveallcom ();
    } 
    else 
      erreval ();
    break;
  case VERB_LASTON:
    lastoncom ();
    break;
  case VERB_GCHAT:
    chatcom ();
    break;
  case VERB_CHANNEL:
    channelcom ();
    break;
  case VERB_FORGET:
    forgetcom ();
    break;
  case VERB_FLIST:
    forgetlist ();
    break;
  case VERB_WHON:
    whoncom ();
    break;
  case VERB_PZAP:
    pzapcom ();
    break;
  case VERB_BEEP:
    beepcom ();
    break;
  case VERB_SEEIDLE:
    seeidlecom ();
    break;
  case VERB_CLEAR:
    bprintf ("\001C\003");
    break;
  case VERB_LEVECHO:
    levechocom ();
    break;
  case VERB_PUNT:
    puntcom ();
    break;
  case VERB_IDLE:
    idlecom ();
    break;
  case VERB_NOSLAIN:
    noslaincom ();
    break;
  case VERB_NOWISH:
    nowishcom ();
    break;
  case VERB_BRESET:
    bresetcom ();
    break;
  case VERB_TXTRAW:
    textrawcom ();
    break;
  case VERB_SYSTEM:
    systemcom ();
    break;
  case VERB_RAW:
    rawcom ();
    break;
  case VERB_MSTAT:
    muserscom();
    break;
  case VERB_ACCT:
    acctcom ();
    break;
  case VERB_OBJECTS:
    objectscom ();
    break;
  case VERB_SEENUMBERS:
    togglecom( SFL_SEENUMBERS, "You will now see numbers on comm lines.", 
                              "You will no longer see numbers on comm lines.");
    break;
  case VERB_NOPUFF:
    togglecom (SFL_NOPUFF, "NoPuff Enabled", "NoPuff Disabled");
    break;
  case VERB_NOORACLE:
    togglecom (SFL_NOORACLE, "NoOracle Enabled", "NoOracle Disabled");
    break;
  case VERB_NOBEEP:
    togglecom (SFL_NOBEEP, "NoBeep Enabled", "NoBeep Disabled");
    break;
  case VERB_NOFLAG:
    togglecom (SFL_NOFLAG, "NoFlags Enabled", "NoFlags Disabled");
    break;
  case VERB_NOWET:
    togglecom (SFL_NOWET, "NoWeather Enabled", "NoWeather Disabled");
    break;
  case VERB_NOBLINK:
    togglecom (SFL_NOBLINK, "Blinking Disabled", "Blinking Enabled");
    break;
  case VERB_NEWSTYLE:
    togglecom (SFL_NEWSTYLE, "NewStyle Enabled", "NewStyle Disabled");
    break;
  case VERB_AUTOEXIT:
    togglecom (SFL_AUTOEXIT, "AutoExit Enabled", "AutoExit Disabled");
    break;
  case VERB_HEARBACK:
    togglecom (SFL_HEARBACK, "Hearback Enabled", "Hearback Disabled");
    break;
  case VERB_NOINV:
    togglecom (SFL_NOINV, "NoInventory Enabled", "NoInventory Disabled");
    break;
  case VERB_NOFIGHT:
    togglecom (SFL_NOFIGHT, "NoFight Enabled", "NoFight Disabled");
    break;
  case VERB_AWAY:
    getreinput(buf);
    if (!EMPTY(buf))
      awaymsgcom();
    togglecom(SFL_AWAY, "Marking you as away from keyboard.",
             	        "Marking you as returned to keyboard.");
    if (ststflg(mynum, SFL_AWAY) && EMPTY(buf)) {
      if (pawaymsg(mynum)) 
        bprintf("Using the following awaymsg:  %s\n", pawaymsg(mynum));
    }
    break;
  case VERB_AWAYBEEPS:
    togglecom( SFL_AWAYBEEPS, "You will hear a beep if away on all tells from now on.",
			      "You will no longer hear a beep if someone talks to you while your away." );
    break;
  case VERB_BUSY:
    togglecom (SFL_BUSY, "Marking you as busy to lower levels.",
	       "Marking you as no longer busy.");
    break;
  case VERB_BRIEF:
    togglecom (SFL_BRIEF, "Brief Mode Enabled", "Brief Mode Disabled");
    break;
  case VERB_GOSSIP:
    com_handler ("&=cl[0] &+c%n&N chats, &+W\"%t&+W\"\n", "&+CChat", 
                 LVL_MIN, SFL_NOCHAT);
    break;
  case VERB_ANON:
    anoncom ();
    break;
  case VERB_NOCHAT:
    nolinecom (LVL_MIN, SFL_NOCHAT, "&+MChat");
    break;
  case VERB_NOANON:
    nolinecom (LVL_MIN, SFL_NOANON, "&+WAnon");
    break;
  case VERB_NOWIZ:
    nolinecom (LVL_WIZARD, SFL_NOWIZ, "&+YWizard");
    break;
  case VERB_NOISTARI:
    nolinecom (LVL_ISTARI, SFL_NOISTARI, "&+WIstari");
    break;
  case VERB_NOAWIZ:
    nolinecom (LVL_ARCHWIZARD, SFL_NOAWIZ, "&+CArchWizard");
    break;
  case VERB_NDGOD:
    nolinecom (LVL_DEMI, SFL_NODEMI, "&+RDemigod");
    break;
  case VERB_NOGOD:
    nolinecom (LVL_SHALAFI, SFL_NOGOD, "&+BGod");
    break;
  case VERB_NOUPPER:
    nolinecom (LVL_GOD, SFL_NOUPPER, "&+GUpper");
    break;
  case VERB_WIZ:
    com_handler ("&=cl[1] &+c<%n> &+C%t\n", "&+YWizard", 
		 LVL_WIZARD, SFL_NOWIZ); 
    break;
  case VERB_ISTARI:
    com_handler ("&=CB[2] &+C&-B(%n) %t&=wl\n", "&+WIstari", 
		 LVL_ISTARI, SFL_NOISTARI);
    break;
  case VERB_AWIZ:
    com_handler ("&=WR[3] &-R[%n] %t&=wl\n", "&+CAWiz", 
		 LVL_ARCHWIZARD, SFL_NOAWIZ);
    break;
  case VERB_DEMI:
    com_handler ("&=WM[4] &-M|%n| %t&=wl\n", "&+RDemigod", 
		 LVL_DEMI, SFL_NODEMI);
    break;
  case VERB_GOD:
    com_handler ("&=WB[5] &=WB*%n* %t&=wl\n", "&+BGod", LVL_SHALAFI, SFL_NOGOD);
    break;
  case VERB_UPPER:
    com_handler ("&=WB[6] &=WB*%n*&=wl %t\n", "&+MUpper", 
		 LVL_GOD, SFL_NOUPPER);
    break;
  case VERB_FLOWERS:
    flowercom ();
    break;
  case VERB_DESCRIPTION:
    change_desc ();
    break;
  case VERB_CLONE:
    clonecom ();
    break;
  case VERB_DESTRUCT:
    destructcom (NULL);
    break;
  case VERB_LOAD:
    loadcom ();
    break;
  case VERB_STORE:
    storecom (pname(mynum), False);
    break;
  case VERB_TICKLE:
    ticklecom ();
    break;
  case VERB_PET:
    petcom ();
    break;
  case VERB_QUIT:
    quit_game ();
    break;
  case VERB_SIT:
    sitcom ();
    break;
  case VERB_STAND:
    standcom (mynum);
    break;
  case VERB_GET:
    getcom ();
    break;
  case VERB_DROP:
    dropobjcom (-1);
    break;
  case VERB_WHO:
    whocom ();
    break;
  case VERB_MWHO:
    mwhocom ();
    break;
  case VERB_RESET:
    resetcom (RES_TEST);
    break;
  case VERB_ZAP:
    zapcom ();
    break;
  case VERB_EAT:
    eatcom ();
    break;
  case VERB_SAVE:
    saveother ();
    break;
  case VERB_GLOBAL:
    globalcom ();
    break;
  case VERB_STEAL:
    stealcom ();
    break;
  case VERB_REVIEW:
    reviewcom ();
    break;
  case VERB_LEVELS:
    levelscom ();
    break;
  case VERB_COUPLES:
    bprintf("\001f%s\003", COUPLES);
    break;
  case VERB_WIZLIST:
    wizlistcom ();
    break;
  case VERB_MAIL:
    mailcom ();
    break;
  case VERB_PROMPT:
    promptcom ();
    break;
  case VERB_TOUT:
    toutcom ();
    break;
  case VERB_INFO:
    infocom ();
    break;
  case VERB_QUESTS:
    questcom ();
    break;
  case VERB_TOURNAMENT:
    tournamentcom ();
    break;
  case VERB_SYSLOG:
    syslogcom ();
    break;
  case VERB_DELETE:
    deletecom ();
    break;
  case VERB_HELP:
    helpcom ();
    break;
  case VERB_STATS:
    showplayer ();
    break;
  case VERB_EXAMINE:
    examcom ();
    break;
  case VERB_EXORCISE:
    exorcom ();
    break;
  case VERB_SUMMON:
    sumcom ();
    break;
  case VERB_WIELD:
    wieldcom ();
    break;
  case VERB_KILL:
    killcom ();
    break;
  case VERB_POSE:
    posecom ();
    break;
  case VERB_SET:
    setcom ();
    break;
  case VERB_PRAY:
    praycom ();
    break;
  case VERB_TIPTOE:
    gotocom (True);
    break;
  case VERB_GOTO:
    gotocom (False);
    break;
  case VERB_WEAR:
    wearcom (mynum, -1);
    break;
  case VERB_REMOVE:
    removecom ();
    break;
  case VERB_PUT:
    putcom ();
    break;
  case VERB_WAVE:
    wavecom ();
    break;
  case VERB_FORCE:
    forcecom ();
    break;
  case VERB_LIGHT:
    lightcom ();
    break;
  case VERB_EXTINGUISH:
    extinguishcom ();
    break;
  case VERB_CRIPPLE:
    spellcom(VERB_CRIPPLE);
    break;
  case VERB_CURE:
    curecom ();
    break;
  case VERB_HEAL:
    healcom ();
    break;
  case VERB_MUTE:
    spellcom(VERB_MUTE);
    break;
  case VERB_CHANGE:
    changecom ();
    break;
  case VERB_MISSILE:
    spellcom (VERB_MISSILE);
    break;
  case VERB_SHOCK:
    spellcom (VERB_SHOCK);
    break;
  case VERB_FIREBALL:
    spellcom (VERB_FIREBALL);
    break;
  case VERB_FROST:
    spellcom (VERB_FROST);
    break;
  case VERB_BLOW:
    blowcom ();
    break;
  case VERB_EXITS:
    exitcom ( True );
    break;
  case VERB_PUSH:
    pushcom ();
    break;
  case VERB_IN:
    incom (True);
    break;
  case VERB_AT:
    incom (False);
    break;
  case VERB_INVISIBLE:
    inviscom ();
    break;
  case VERB_VISIBLE:
    viscom ();
    break;
  case VERB_DEAFEN:
    spellcom(VERB_DEAFEN);
    break;
  case VERB_RESURRECT:
    resurcom ();
    break;
  case VERB_TITLE:
    change_title ();
    break;
  case VERB_SETSTART:
    setstart ();
    break;
  case VERB_LOCATIONS:
    locationscom ();
    break;
  case VERB_ZONES:
    zonescom ();
    break;
  case VERB_USERS:
    usercom ();
    break;
  case VERB_BECOME:
    becom (NULL);
    break;
  case VERB_SNOOP:
    snoopcom ();
    break;
  case VERB_ROLL:
    rollcom ();
    break;
  case VERB_CREDITS:
    bprintf ("\001f%s\003", CREDITS);
    break;
  case VERB_JUMP:
    jumpcom ();
    break;
  case VERB_WHERE:
    wherecom();
    break;
  case VERB_FLEE:
    flee (real_mynum, True);
    break;
  case VERB_BUG:
    bugcom ();
    break;
  case VERB_TYPO:
    typocom ();
    break;
  case VERB_ACTIONS:
    actionscom ();
    break;
  case VERB_PN:
    pncom ();
    break;
  case VERB_BLIND:
    spellcom(VERB_BLIND);
    break;
  case VERB_FROB:
    frobcom (NULL);
    break;
  case VERB_CRASH:
    crashcom (True);
    break;
  case VERB_EMOTE:
    emotecom ();
    break;
  case VERB_EMOTETO:
    emotetocom ();
    break;
  case VERB_EMPTY:
    emptycom ();
    break;
  case VERB_TIME:
    timecom ();
    break;
  case VERB_TREASURES:
    treasurecom ();
    break;
  case VERB_WAR:
    warcom ();
    break;
  case VERB_PEACE:
    peacecom ();
    break;
  case VERB_QUIET:
    if (plev (mynum) < LVL_WIZARD) {
      erreval ();
      break;
    }
    bprintf ("Quiet mode ");
    if (ststflg (mynum, SFL_QUIET)) {
      bprintf ("off.\n");
      sclrflg (mynum, SFL_QUIET);
    } 
    else {
      bprintf ("on.\n");
      ssetflg (mynum, SFL_QUIET);
    }
    break;
  case VERB_NOSHOUT:
    noshoutcom ();
    break;
  case VERB_COLOR:
    if (ststflg (mynum, SFL_COLOR)) {
      bprintf ("Back to black and white.\n");
      sclrflg (mynum, SFL_COLOR);
    }
    else {
      bprintf ("Color mode &+WON&N.\n");
      ssetflg (mynum, SFL_COLOR);
    }
    break;
  case VERB_ALOOF:
    if (plev (mynum) < LVL_WIZARD) {
      erreval ();
      break;
    }
    if (ststflg (mynum, SFL_ALOOF)) {
      bprintf ("You feel less distant.\n");
      sclrflg (mynum, SFL_ALOOF);
    } 
    else {
      bprintf ("From now on you'll ignore some mortals' actions.\n");
      ssetflg (mynum, SFL_ALOOF);
    }
    break;
  case VERB_SHOW:
    showitem ();
    break;
  case VERB_WIZLOCK:
    wizlock ();
    break;
  case VERB_FOLLOW:
    followcom ();
    break;
  case VERB_LOSE:
    losecom ();
    break;
  case VERB_ECHO:
    echocom ();
    break;
  case VERB_ECHOALL:
    echoallcom ();
    break;
  case VERB_EMOTEALL:
    if (plev(mynum) > LVL_WIZARD)
      emoteallcom ();
    break;
  case VERB_ECHOTO:
    echotocom ();
    break;
  case VERB_WISH:
    wishcom ();
    break;
#if 0
  case VERB_TRACE:
    tracecom ();
    break;
#endif
  case VERB_START:
    startcom ();
    break;
  case VERB_STOP:
    stopcom ();
    break;
  case VERB_MOBILES:
    mobilecom ();
    break;
  case VERB_UNVEIL:
    unveilcom (NULL);
    break;
  case VERB_BANG:
    if (plev (mynum) < 10 || pclass (mynum) != PRIEST)
      erreval ();
    else
      broad ("\001dA huge crash of thunder echoes through the land.\n\003");
    break;
  case VERB_TRANSLOCATE:
    sumcom ();
    break;
  case VERB_ALIAS:
    aliascom ();
    return;
  case VERB_VERSION:
    bprintf ("&+W%s (%s/%s)\n", VERSION, _ARCH_, _OS_);
    bprintf ("1997, G.Castrataro (gcastrat@indiana.edu)\n");
    break;
  case VERB_MEDITATE:

#ifdef LOCMIN_ANCIENT
    if (ploc (mynum) == LOC_ANCIENT_ANC59) {
      if (oarmor (OBJ_ANCIENT_LBOOK) == 0)
	bprintf ("You meditate, but nothing seems to happen.\n"); 
      else {
	bprintf ("You are teleported!\n\n\n");
	trapch (LOC_ANCIENT_ANC35);
      }
      break;
    }
#endif
    bprintf ("You meditate quietly in the corner.\n");
    break;

  default:
    mudlog ("No match for verb = %d", vb);
    bprintf ("You can't do that now.\n");
    break;
  }
}


void quit_game (void)
{
  char xx[128];

  if (pfighting (mynum) >= 0) {
    bprintf ("Not in the middle of a fight!\n");
    return;
  }
  if (aliased(real_mynum)) {
    unalias (real_mynum);
    return;
  }
  if (polymorphed(real_mynum) >= 0) {
    bprintf ("A mysterious force won't let you quit.\n");
    return;
  }
  if(ltstflg(ploc(mynum), LFL_NO_QUIT) && plev(mynum) < LVL_ARCHWIZARD) {
    bprintf( "Something about this room won't let you quit!\n" );
    return;
  }

  send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY,
  "%s\n", build_setin (SETIN_SETQOUT, xx, cur_player->setqout, pname (mynum), NULL, NULL));

  send_msg (DEST_ALL, MODE_QUIET, max (pvis (mynum), LVL_WIZARD), LVL_MAX,
            mynum, NOBODY, "&+B[&+CQuitting Game: &+W%s&+B]\n", 
	    pname (mynum));
  quit_msg("Thank you for playing \001$\003!", NULL);
  quit_player(False);
}

void
erreval (void)
{
  bprintf ("You can't do that now.\n");
}

