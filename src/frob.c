#include <stdlib.h>
#include "kernel.h"
#include "levels.h"
#include "sendsys.h"
#include "frob.h"
#include "uaf.h"
#include "mobile.h"
#include "log.h"
#include "parse.h"
#include "bprintf.h"
#include "mud.h"
#include "wizlist.h"
#include "level.h"

#define legal(lev) (lev >= LVL_MIN && lev <= LVL_MAX)
#define cant_frob(lev) (!legal(lev) || lev > plev(mynum))

struct _f {
  int state;
  intptr_t oldwork;
  int oldlev;
  int level;
  int strength;
  int score;
  char *oldprompt;
  char name[MNAME_LEN + 1];
};


static void
log (char *n, int lev, int sco, int str)
{
  mudlog ("FROB: %s by %s: Lev = %d, Sco = %d, Str = %d",
	  n, pname (mynum), lev, sco, str);
}

void frobcom (char *line) {
  PERSONA p;
  struct _f *f;
  int x;

  if (line == NULL) {
    if (!ptstflg(mynum, PFL_FROB) || !ptstflg(mynum, PFL_CH_SCORE) ||
	!ptstflg(mynum, PFL_CH_LEVEL)) {
      erreval ();
      return;
    }
    if (brkword () == -1
	|| ((x = fmbn (wordbuf)) == -1 && !ptstflg (mynum, PFL_UAF))) {\
      bprintf ("Frob who?\n");
      return;
    }
    if (x == -1) {
      if (!getuaf (wordbuf, &p)) {
	bprintf ("No such persona in system.\n");
	return;
      }
      if (p.ublock.plev > plev(mynum)) {
	bprintf ("%s is too powerful!\n", wordbuf);
	return;
      }
    } 
    else if (plev(x) > plev(mynum)) {
      bprintf ("%s is too powerful!\n", wordbuf);
      return;
    } 
    else {
      p.ublock.plev = plev (x);
      p.ublock.pstr = pstr (x);
      p.ublock.pscore = pscore (x);
      strcpy (p.ublock.pname, pname (x));
    }
    f = NEW (struct _f, 1);

    strcpy (f->name, p.ublock.pname);
    f->state = 0;
    f->level = p.ublock.plev;
    f->oldlev = p.ublock.plev;
    f->strength = p.ublock.pstr;
    f->score = p.ublock.pscore;
    f->oldprompt = COPY (cur_player->cprompt);
    strcpy (cur_player->cprompt, "New Level: ");
    f->oldwork = cur_player->work;
    cur_player->work = (intptr_t) f;
    bprintf ("\001f" FROBCHT "\003");
    bprintf ("Level is: %d\n", f->oldlev);
    push_input_handler (frobcom);
  } 
  else {
    while (*line == ' ' || *line == '\t')
      ++line;
    f = (struct _f *) cur_player->work;

    switch (f->state) {
    case 0:
      if (*line == '\0')
	x = f->level;
      else 
	x = atoi (line);

      if (x < 1 || x > LVL_MAX) {
	bprintf ("Level must be between %d and %d\n", 1, LVL_MAX);
	f->state = 20;
      } 
      else if (cant_frob (x)) {
	bprintf ("You can't do that, sorry.\n");
	f->state = 20;
      } 
      else {
	f->level = x;
	f->state = 1;
	bprintf ("Score is: %d\n", f->score);
	strcpy (cur_player->cprompt, "New Score: ");
      }
      break;

    case 1:
      if (*line == '\0')
	x = f->score;
      else
	x = atoi (line);

      f->score = x;
      f->state = 2;
      bprintf ("Strength is: %d\n", f->strength);
      strcpy (cur_player->cprompt, "New Strength: ");
      break;

    case 2:
      if (*line == '\0')
	x = f->strength;
      else
	x = atoi (line);

      if (x <= 0) {
	bprintf ("Strength must be positive.\n");
	f->state = 20;
      }
      else {
	f->strength = x;
	if ((x = fpbn(f->name)) == -1) {
	  if (!ptstflg (mynum, PFL_UAF)) {
	    bprintf ("%s isn't here.\n", f->name);
	    f->state = 20;
	  } 
          else if (!getuaf (f->name, &p)) {
	    bprintf ("No player named %s.\n", f->name);
	    f->state = 20;
	  }
	} 
        else {
	  p.ublock.plev = plev (x);
	  p.ublock.pstr = pstr (x);
	  p.ublock.pscore = pscore (x);
	}
	if (f->state == 2) {
	  log (f->name, f->level, f->score, f->strength);

	  if (x >= 0) {
	    setpstr (x, f->strength);
       
            if (f->level < LVL_WIZARD) {
              if (f->level !=  plev(x))
                setpscore(x, levels[f->level]);
              else
                setpscore(x, f->score);
            }
            
            setplev(x, f->level);
            calib_player(x);

            if (f->oldlev < plev(x))
              give_skills(mbits(x), x, f->oldlev, plev(x));
            else
              take_skills(mbits(x), x, f->oldlev, plev(x));

            setptitle(x, std_title(plev(x), psex(x), pclass(x)));
	    set_xpflags (mbits(x), f->level);
            if (f->level > LVL_WIZARD)
              players[x].wiz_time = global_clock;
	  } 
          else {
	    p.ublock.plev = f->level;
	    p.ublock.pstr = f->strength;
	    p.ublock.pscore = f->score;
	    if (f->level > LVL_WIZARD)
              p.player.wiz_time = global_clock;

            if (f->oldlev < plev(x))
              give_skills(p.ublock.bits, NOBODY, f->oldlev, f->level);
            else
              take_skills(p.ublock.bits, NOBODY, f->oldlev, f->level);

            set_xpflags(p.ublock.bits, f->level);
	    strcpy (p.player.ptitle, std_title (f->level, 
              ptst_flg(p.ublock.bits, SFL_FEMALE, SFLAGS), p.ublock.class));
	    putuaf(&p);
	  }
	  update_wizlist(f->name, wlevel (f->level));
	  bprintf("Ok.\n");
	  f->state = 20;
	}
      }
      break;
    }
    if (f->state == 20) {
      strcpy (cur_player->cprompt, f->oldprompt);
      FREE (f->oldprompt);
      cur_player->work = f->oldwork;
      FREE (f);
      pop_input_handler ();
    }
  }
}
