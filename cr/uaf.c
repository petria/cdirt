#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include "pflags.h"
#include "kernel.h"
#include "sflags.h"
#include "mflags.h"
#include "eflags.h"
#include "clone.h"
#include "nflags.h"
#include "uaf.h"
#include "mobile.h"
#include "flags.h"
#include "exit.h"
#include "sendsys.h"
#include "mud.h"
#include "bprintf.h"
#include "wizlist.h"
#include "log.h"
#include "parse.h"
#include "locations.h"
#include "quests.h"
#include "utils.h"
#include "cflags.h"
#include "levels.h"
#include "stdlib.h"
#include "objsys.h"
#include "fight.h"
#include "flags.h"

extern int errno;
time_t time (time_t * v);


#define UAF_VERSION 6                /* increment to add changes         */
#define NUM_ENT     40               /* number of variables to save/load */
#define LLEN        4096              /* max length of a pfile line      */
#define TOKEN_LEN   11               /* Length of space before value     */
#define NOVAL      (long int *) 0
#define INT         0
#define CHAR        1
#define PTR         2

typedef struct __iorec {
  short int type;            /* Type of value: int char ptr */
  char *token;               /* Name of text token */
  char **sval;               /* String value to load */
  long int *ival;            /* Integer to load */
} IORec;

extern char *levnam(int lev, int class, Boolean sex);
extern void store_flags(FILE *, PERSONA *);
extern void load_flags(char *, PERSONA *);

int save_load_plr(PERSONA *d, char *player, Boolean save, Boolean new) {
  int i;
  FILE *fptr = NULL;
  IORec *pptr = NULL;
  char *token = NULL;
  char file[256], line[LLEN], *nptr, *value;

  IORec p_rec[NUM_ENT] = {

    /* arrays */

    {CHAR, "Title",       (char **) &(d->player.ptitle),    NOVAL},
    {CHAR, "Passwd",      (char **) &(d->player.passwd),    NOVAL},
    {CHAR, "Username",    (char **) &(d->rplr.usrname),     NOVAL},
    {CHAR, "LastHost",    (char **) &(d->rplr.hostname),    NOVAL},

    /* pointers to char */

    {PTR,  "Awaymsg",     &(d->player.awaymsg),             NOVAL},
    {PTR,  "Prompt",      &(d->player.prompt),              NOVAL},
    {PTR,  "Setin",       &(d->player.setin),               NOVAL},
    {PTR,  "Setout",      &(d->player.setout),              NOVAL},
    {PTR,  "Setmin",      &(d->player.setmin),              NOVAL},
    {PTR,  "Setmout",     &(d->player.setmout),             NOVAL},
    {PTR,  "Setvin",      &(d->player.setvin),              NOVAL},
    {PTR,  "Setvout",     &(d->player.setvout),             NOVAL},
    {PTR,  "Setqin",      &(d->player.setqin),              NOVAL},
    {PTR,  "Setqout",     &(d->player.setqout),             NOVAL},
    {PTR,  "Setsit",      &(d->player.setsit),              NOVAL},
    {PTR,  "Setstand",    &(d->player.setstand),            NOVAL},
    {PTR,  "Setsum",      &(d->player.setsum),              NOVAL},
    {PTR,  "Setsin",      &(d->player.setsumin),            NOVAL},
    {PTR,  "Setsout",     &(d->player.setsumout),           NOVAL},
    {PTR,  "Home",        &(d->ublock.phome),               NOVAL},

    /* integers */

    {INT,  "PLines",      NULL,                (long *) &d->player.pager.len},
    {INT,  "CarryCap",    NULL,                (long *) &d->player.pcarry},
    {INT,  "Score",       NULL,                (long *) &d->ublock.pscore},
    {INT,  "Strength",    NULL,                (long *) &d->ublock.pstr},
    {INT,  "Damage",      NULL,                (long *) &d->ublock.pdam},
    {INT,  "Armor",       NULL,                (long *) &d->ublock.parmor},
    {INT,  "Visibility",  NULL,                (long *) &d->ublock.pvis},
    {INT,  "Level",       NULL,                (long *) &d->ublock.plev},
    {INT,  "Wimpy",       NULL,                (long *) &d->ublock.pwimpy},
    {INT,  "Magic",       NULL,                (long *) &d->player.pmagic},
    {INT,  "Channel",     NULL,                (long *) &d->player.pchannel},
    {INT,  "Killed",      NULL,                (long *) &d->player.pkilled},
    {INT,  "Died",        NULL,                (long *) &d->player.pdied},
    {INT,  "Coins",       NULL,                (long *) &d->player.coins},
    {INT,  "Class",       NULL,                (long *) &d->ublock.class},

    /* long ints */

    {INT,  "FirstOn",     NULL,                     &d->player.first_on},
    {INT,  "TimeOn",      NULL,                     &d->player.time_on},
    {INT,  "MortalTime",  NULL,                     &d->player.mortal_time},
    {INT,  "WizTime",     NULL,                     &d->player.wiz_time},
    {INT,  "LastOn",      NULL,                     &d->player.last_on}};

  if (!valid_fname(player) && (save || new)) {
    bprintf("Sorry, that name has illegal characters.\n");
    return(0);
  }

  for (nptr = player; *nptr; nptr++)
    if (isalpha(*nptr))
      *nptr = tolower(*nptr);
  *player = toupper(*player);
  sprintf(file, "%s/%c/%s", TEXT_PFILES_BASE, *player, player);

  if (save) {
    if (access(file, F_OK) && !new)  /* player zapped */
      return(0);
    else if (!(fptr = FOPEN(file, "w"))) {
      mudlog("UAF: Cannot store %s", file);
      return(-1);
    }

    for (pptr = p_rec ; pptr < p_rec + NUM_ENT ; pptr++) {
      if (pptr->type == INT && *(pptr->ival))
        fprintf(fptr, "%-11s%ld\n", pptr->token, *(pptr->ival));
      else if (pptr->type == CHAR && *(pptr->sval))
        fprintf(fptr, "%-11s%s\n", pptr->token, (char *) pptr->sval);
      else if (pptr->type == PTR && *(pptr->sval))
        fprintf(fptr, "%-11s%s\n", pptr->token, *(pptr->sval));
    }
    store_parts(fptr, d);
    store_flags(fptr, d);

    for (i = 0 ; i < NUM_STORE_SLOTS ; i++)
      if (d->rplr.storage[i] != -1)
        fprintf(fptr, "Storage    %s\n", oname(d->rplr.storage[i]));
  }
  else {                                        /* load */
    memset(d, 0, sizeof(PERSONA));
    if (!(fptr = FOPEN(file, "r"))) {
      if (!access(file, F_OK)) {
        mudlog("UAF: Cannot load %s", file);
        return(-1);
      }
      else
        return(0);
    }

    strcpy(d->ublock.pname, player);

    for (i = 0 ; i < NUM_STORE_SLOTS ; i++)
      d->rplr.storage[i] = -1;

    while (1) {
      fgets(line, LLEN - 1, fptr);

      if (feof(fptr))
        break;

      if (strlen(line) < TOKEN_LEN + 1) {
        mudlog("UAF: Error in player record %s", player);
        FCLOSE(fptr);
        return(-1);
      }
      line[strlen(line) - 1] = 0;
      
      token = line;
      value = line + TOKEN_LEN;

      if (!strncmp(token, "Bodyparts", 9))
        load_parts(value, d);

      else if (!strncmp(token, "Storage", 7)) {
        for (i = 0 ; i < NUM_STORE_SLOTS ; i++)    /* free storage slot */
          if (d->rplr.storage[i] == -1) {
            d->rplr.storage[i] = fobn(value);
            break;
          }
      }
      else if (*token == '%')
	load_flags(line, d);
      else {                                                     /* table */
        for (pptr = p_rec ; pptr < p_rec + NUM_ENT ; pptr++)
          if (!strncmp(pptr->token, token, strlen(pptr->token))) {
            if (pptr->type == INT)
              *(pptr->ival) = atol(value);
            else if (pptr->type == CHAR)
              strcpy((char *) pptr->sval, value);
            else if (pptr->type == PTR)
              *(pptr->sval) = COPY(value);
            break;
          }
        if (pptr == p_rec + NUM_ENT) {
          mudlog("UAF: Unknown token in %s", player);
          FCLOSE(fptr);
          return(-1);
        }
      } 
    }
  }

  FCLOSE(fptr);
  return(1);
}

void pers2player (PERSONA * d, int plx) {
  int i;

  memcpy(&ublock[plx], &(d->ublock), sizeof(UBLOCK_REC));
  memcpy(&players[plx], &(d->player), sizeof(PLAYER_REC));

  if (plev(plx) < LVL_WIZARD)
    setpvis(plx, 0);

  pangry(plx) = -1; 

  for (i = 0 ; i < NUM_STORE_SLOTS ; i++) {
    rplrs[plx].storage[i] = -1;
    if (d->rplr.storage[i] != -1)
      clone_object(d->rplr.storage[i], -1, NULL, plx, CARRIED_BY);
  }

  reset_armor(False, plx);
  set_player_parts(plx);
  players[plx].resfd = -1;
  oldscore(plx) = pscore(plx);
  pfighting(plx) = -1;
  phelping(plx) = -1;
  is_conn(plx) = True;
}

void player2pers (PERSONA *d, time_t *last_on, int plx)
{
  if (plx < max_players) {
    memcpy(&(d->player), &players[plx], sizeof(PLAYER_REC));
    memcpy(&(d->rplr), &rplrs[plx], sizeof(RPLR_REC));
  }
  memcpy(&(d->ublock), &ublock[plx], sizeof(UBLOCK_REC));

  if (last_on != NULL) {
    d->player.last_on = *last_on;
    if (rplrs[plx].logged_on) {
      d->player.time_on += *last_on - rplrs[plx].logged_on;
      if (plev(plx) < LVL_WIZARD)
        d->player.mortal_time += *last_on - rplrs[plx].logged_on;
    }
  }
}

void saveother (void) {
  int p;
  static PERSONA d;

  if (EMPTY (item1))
    p = mynum;
  else {
    if ((p = pl1) == -1) {
      bprintf ("Cannot find player.\n");
      return;
    }
    if (plev (mynum) < LVL_WIZARD && p != mynum) {
      bprintf ("You cannot save another player!\n");
      return;
    }
  }

  if (aliased(p)) {
    bprintf ("Not while aliased.\n");
    return;
  }
  player2pers (&d, &global_clock, p);
  putuaf (&d);

  if (p != mynum)
    sendf (mynum, "&+M[%s Saved]\n", pname (p));
  else
    bprintf ("&+M[Player Saved]\n", pname (mynum));
}

void saveme (Boolean silent) {
  static PERSONA d;

  if (aliased(real_mynum)) {
    bprintf ("Not while aliased.\n");
    return;
  }
  player2pers (&d, &global_clock, mynum);
  putuaf (&d);
  if (!silent) bprintf ("&+M[Player Saved]\n");
}

void saveallcom (void) {
  static PERSONA d;
  int i;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i)) {
      if (aliased(i));
      else {
        player2pers (&d, &global_clock, i);
        putuaf (&d);
      }
    }
  }

  bprintf ("&+M[Saved All Players]\n");
  send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, mynum, NOBODY,
              "&+B[&+CSaveall &*by &+W\001p%s\003&+B]\n", pname (mynum));
}

char *ipname (int plr)
{
  static char name[30];

  sprintf (name, "\001p%s\003", pname (plr));
  return name;
}

char *build_setin (int type, char *notused, char *s, char *n, char *d, char *v){
  char *p, *q, *r;
  char temp[SETIN_MAX];
  static char buff[SETIN_MAX + PNAME_LEN * 2];  /* allows for 2 victims */

  if(s == NULL) {
    switch(type) {
    default:
        mudlog("Unknown type passed to build_setin: %d!", type);
        return NULL;
    case SETIN_SETIN:
        strcpy(temp, DEFAULT_SETIN);
        break;
    case SETIN_SETOUT:
        strcpy(temp, DEFAULT_SETOUT);
        break;
    case SETIN_SETMIN:
        strcpy(temp, DEFAULT_SETMIN);
        break;
    case SETIN_SETMOUT:
        strcpy(temp, DEFAULT_SETMOUT);
        break;
    case SETIN_SETVIN:
        strcpy(temp, DEFAULT_SETVIN);
        break;
    case SETIN_SETVOUT:
        strcpy(temp, DEFAULT_SETVOUT);
        break;
    case SETIN_SETQIN:
        strcpy(temp, DEFAULT_SETQIN);
        break;
    case SETIN_SETQOUT:
        strcpy(temp, DEFAULT_SETQOUT);
        break;
    case SETIN_SETSIT:
        strcpy(temp, DEFAULT_SETSIT);
        break;
    case SETIN_SETSTAND:
        strcpy(temp, DEFAULT_SETSTAND);
        break;
    case SETIN_SETSUM:
        strcpy(temp, DEFAULT_SETSUM);
        break;
    case SETIN_SETSUMIN:
        strcpy(temp, DEFAULT_SETSUMIN);
        break;
    case SETIN_SETSUMOUT:
        strcpy(temp, DEFAULT_SETSUMOUT);
        break;
    }
    s = temp;
  }

  for (p = buff, q = s; *q;) {
    if (*q != '%')
      *p++ = *q++;
    else {
      switch (*++q) {
      case 'n':
        if (n)
          sprintf(p, "\001p%s\003", n);
          p += strlen(n) + 3;
        break;
      case 'v':
        if (v)
          sprintf(p, "\001p%s\003", v);
          p += strlen(v) + 3;
        break;
      case 'd':
        if (d)
          for (r = d; *r;)
            *p++ = *r++;
        break;
      case 'N':
        for (r = xname (n); *r != 0;)
          *p++ = *r++;
        break;
      case 'f':
        for (r = (psex (mynum) ? "her" : "his"); *r != 0;)
          *p++ = *r++;
        break;
      case 'F':
        for (r = (psex (mynum) ? "her" : "him"); *r != 0;)
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
  return buff;
}

int getuafinfo (char *name) {
  PERSONA d;
  int b;

  b = getuaf(name, &d);
  if (b == 1) {
    pers2player (&d, mynum);
    setpname (mynum, d.ublock.pname);
  }
  else if (b == -1) {
    bprintf("Error loading your character; please talk to a power.\n");
    quit_player(False);
  }
  return(b);
}

int deluaf(char *name) {
  char *nptr, file[256];

  if (!valid_fname(name)) {
    bprintf("Player name contains illegal characters.\n");
    return(1);
  }

  for (nptr = name; *nptr; nptr++)
    if (isalpha(*nptr))
      *nptr = tolower(*nptr);

  *name = toupper(*name);
  sprintf(file, "%s/%c/%s", TEXT_PFILES_BASE, *name, name);

  if (unlink(file) == -1)
    return(1);
  return(0);
}
