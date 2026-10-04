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
#include <inttypes.h>
#define NOVAL NULL
#define INT 0
#define CHAR 1
#define PTR 2
#define UINT 3
#define TIME 4

typedef struct __iorec {
  short type;
  char *token;
  void *field;
  size_t size;
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

    {CHAR, "Title", &(d->player.ptitle), sizeof(d->player.ptitle)},
    {CHAR, "Passwd", &(d->player.passwd), sizeof(d->player.passwd)},
    {CHAR, "Username", &(d->rplr.usrname), sizeof(d->rplr.usrname)},
    {CHAR, "LastHost", &(d->rplr.hostname), sizeof(d->rplr.hostname)},

    /* pointers to char */

    {PTR, "Awaymsg", &(d->player.awaymsg), sizeof(d->player.awaymsg)},
    {PTR, "Prompt", &(d->player.prompt), sizeof(d->player.prompt)},
    {PTR, "Setin", &(d->player.setin), sizeof(d->player.setin)},
    {PTR, "Setout", &(d->player.setout), sizeof(d->player.setout)},
    {PTR, "Setmin", &(d->player.setmin), sizeof(d->player.setmin)},
    {PTR, "Setmout", &(d->player.setmout), sizeof(d->player.setmout)},
    {PTR, "Setvin", &(d->player.setvin), sizeof(d->player.setvin)},
    {PTR, "Setvout", &(d->player.setvout), sizeof(d->player.setvout)},
    {PTR, "Setqin", &(d->player.setqin), sizeof(d->player.setqin)},
    {PTR, "Setqout", &(d->player.setqout), sizeof(d->player.setqout)},
    {PTR, "Setsit", &(d->player.setsit), sizeof(d->player.setsit)},
    {PTR, "Setstand", &(d->player.setstand), sizeof(d->player.setstand)},
    {PTR, "Setsum", &(d->player.setsum), sizeof(d->player.setsum)},
    {PTR, "Setsin", &(d->player.setsumin), sizeof(d->player.setsumin)},
    {PTR, "Setsout", &(d->player.setsumout), sizeof(d->player.setsumout)},
    {PTR, "Home", &(d->ublock.phome), sizeof(d->ublock.phome)},

    /* integers */

    {INT, "PLines", &d->player.pager.len, sizeof(d->player.pager.len)},
    {INT, "CarryCap", &d->player.pcarry, sizeof(d->player.pcarry)},
    {UINT, "Score", &d->ublock.pscore, sizeof(d->ublock.pscore)},
    {INT, "Strength", &d->ublock.pstr, sizeof(d->ublock.pstr)},
    {INT, "Damage", &d->ublock.pdam, sizeof(d->ublock.pdam)},
    {INT, "Armor", &d->ublock.parmor, sizeof(d->ublock.parmor)},
    {INT, "Visibility", &d->ublock.pvis, sizeof(d->ublock.pvis)},
    {INT, "Level", &d->ublock.plev, sizeof(d->ublock.plev)},
    {INT, "Wimpy", &d->ublock.pwimpy, sizeof(d->ublock.pwimpy)},
    {INT, "Magic", &d->player.pmagic, sizeof(d->player.pmagic)},
    {INT, "Channel", &d->player.pchannel, sizeof(d->player.pchannel)},
    {INT, "Killed", &d->player.pkilled, sizeof(d->player.pkilled)},
    {INT, "Died", &d->player.pdied, sizeof(d->player.pdied)},
    {INT, "Coins", &d->player.coins, sizeof(d->player.coins)},
    {INT, "Class", &d->ublock.class, sizeof(d->ublock.class)},

    /* long ints */

    {TIME, "FirstOn", &d->player.first_on, sizeof(d->player.first_on)},
    {TIME, "TimeOn", &d->player.time_on, sizeof(d->player.time_on)},
    {TIME, "MortalTime", &d->player.mortal_time, sizeof(d->player.mortal_time)},
    {TIME, "WizTime", &d->player.wiz_time, sizeof(d->player.wiz_time)},
    {TIME, "LastOn", &d->player.last_on, sizeof(d->player.last_on)}};

  if (!valid_fname(player) || !*player || strlen(player) >= sizeof(d->ublock.pname)) {
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
      if (pptr->type == INT && *(int *)pptr->field)
        fprintf(fptr, "%-11s%d\n", pptr->token, *(int *)pptr->field);
      else if (pptr->type == UINT && *(unsigned *)pptr->field)
        fprintf(fptr, "%-11s%u\n", pptr->token, *(unsigned *)pptr->field);
      else if (pptr->type == TIME && *(time_t *)pptr->field)
        fprintf(fptr, "%-11s%jd\n", pptr->token, (intmax_t)*(time_t *)pptr->field);
      else if (pptr->type == CHAR && *(char *)pptr->field)
        fprintf(fptr, "%-11s%s\n", pptr->token, (char *)pptr->field);
      else if (pptr->type == PTR && *(char **)pptr->field)
        fprintf(fptr, "%-11s%s\n", pptr->token, *(char **)pptr->field);
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

    while (fgets(line, sizeof(line), fptr)) {

      if (strlen(line) < TOKEN_LEN + 1) {
        mudlog("UAF: Error in player record %s", player);
        goto corrupt;
      }
      if (!strchr(line, '\n') && !feof(fptr)) goto corrupt;
      line[strcspn(line, "\r\n")] = 0;
      
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
            if (pptr->type == INT || pptr->type == UINT || pptr->type == TIME) {
              char *end;
              intmax_t number;
              errno = 0;
              number = strtoimax(value, &end, 10);
              if (errno || end == value || *end) goto corrupt;
              if (pptr->type == INT) {
                if (number < INT_MIN || number > INT_MAX) goto corrupt;
                *(int *)pptr->field = (int)number;
              } else if (pptr->type == UINT) {
                if (number < 0 || (uintmax_t)number > UINT_MAX) goto corrupt;
                *(unsigned *)pptr->field = (unsigned)number;
              } else {
                time_t stamp = (time_t)number;
                if ((intmax_t)stamp != number) goto corrupt;
                *(time_t *)pptr->field = stamp;
              }
            } else if (pptr->type == CHAR) {
              if (strlen(value) >= pptr->size) goto corrupt;
              memcpy(pptr->field, value, strlen(value) + 1);
            } else if (pptr->type == PTR) {
              if ((!strcmp(pptr->token, "Prompt") && strlen(value) > PROMPT_LEN) ||
                  (!strncmp(pptr->token, "Set", 3) && strlen(value) >= SETIN_MAX)) goto corrupt;
              free(*(char **)pptr->field);
              *(char **)pptr->field = COPY(value);
            }
            break;
          }
        if (pptr == p_rec + NUM_ENT) {
          mudlog("UAF: Unknown token in %s", player);
          goto corrupt;
        }
      } 
    }
  }

  if (ferror(fptr)) goto corrupt;
  FCLOSE(fptr);
  return(1);
corrupt:
  mudlog("UAF: Error in player record %s", player);
  FCLOSE(fptr);
  if (!save) {
    for (pptr = p_rec; pptr < p_rec + NUM_ENT; pptr++)
      if (pptr->type == PTR) { free(*(char **)pptr->field); *(char **)pptr->field = NULL; }
  }
  return -1;
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
  static char *buff; /* borrowed until the next build_setin call */

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

  {
    Text text = {0};
    for (q = s; *q;) {
      if (*q != '%') { text_char(&text, *q++); continue; }
      q++;
      switch (*q) {
      case 'n': case 'v':
        r = *q == 'n' ? n : v;
        if (r) { text_append(&text, "\001p"); text_append(&text, r); text_char(&text, 003); }
        break;
      case 'd': text_append(&text, d); break;
      case 'N': if (n) text_append(&text, xname(n)); break;
      case 'f': text_append(&text, psex(mynum) ? "her" : "his"); break;
      case 'F': text_append(&text, psex(mynum) ? "her" : "him"); break;
      case 0: break;
      default: text_char(&text, *q); break;
      }
      if (*q) q++;
    }
    if (text.len && text.data[text.len - 1] == '\n') text.data[--text.len] = 0;
    free(buff);
    buff = text_take(&text);
  }
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
