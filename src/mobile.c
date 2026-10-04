#include <stdlib.h>
#include <unistd.h>
#include "kernel.h"
#include "quests.h"
#include "levels.h"
#include "locations.h"
#include "timing.h"
#include "sendsys.h"
#include "objects.h"
#include "mobiles.h"
#include "rooms.h"
#include "mobile.h"
#include "fight.h"
#include "puff.h"
#include "bprintf.h"
#include "parse.h"
#include "zones.h"
#include "objsys.h"
#include "uaf.h"
#include "mud.h"
#include "parse.h"
#include "log.h"
#include "wizlist.h"
#include "commands.h"
#include "move.h"
#include "codernames.h"
#include "wizard.h"
#include "clone.h"

#define SECS_IN_A_MIN 60
extern char *ocorpse[];

extern HASH_TABLE ublock_n[];

char *make_prompt (char *, char *, char *, char *, char *, char *, char *);
char *makepad(char *, int);
char *makenewline (char *, int);
void check_mobile(int);

#ifdef LOCMIN_VALLEY
static char *mindy[] =
  {
    "barks at you playfully.",
    "nibbles at her paw and then scratches her head.",
    "comes up to you and licks your face playfully.",
    "looks quite silly as she runs around in a circle trying to get her tail.",
    "looks at you playfully as she drops her chew toy at your side.",
    "rolls around on the ground playfully.",
    "curls up around your feet and closes her eyes.",
    "looks around for a tasty milk bone."
  };
#endif

#ifdef LOCMIN_SHERWOOD
static char *emmy[] =
  {
    "\001pEmmy\003 bounces around happily.",
    "\001pEmmy\003 giggles as she plays peek-a-boo with you!",
    "&+G\001pEmmy\003 &+wsays &+W'&+wKA SAWN!!&+W'"
  };
#endif

/* Caller owns the returned string. */
char *time_format(int t) {
  return text_format("%02d:%02d:%02d", t/3600, (t%3600)/60, t%60);
}

int cmp_player (const void *a, const void  *b)
{
  int c1, c2;

  c1 = *(int *) a;
  c2 = *(int *) b;

  if (plev(c1) > plev(c2))
    return (-1);
  else if (plev(c1) < plev(c2))
    return (1);
  else
    return (strcmp(pname(c1), pname(c2)));
}

/* save a list of players in html form for web access */

#ifdef SAVE_HTML
void storeplayers() {
  int a[max_players], i;
  char *buff;
  FILE *htmlptr;  

  int len = 0;

  unlink(HTML_FILE);
  if ((htmlptr = FOPEN(HTML_FILE, "w")) == NULL)
    return;

  for (i = 0 ; i < max_players ; i++)
    if (is_in_game(i) && pvis(i) == 0)
      a[len++] = i;

  qsort (a, len, sizeof (int), cmp_player);

  fprintf(htmlptr, "<html>\n" 
          "<head><title>Players on " MUD_NAME "</title></head>\n"
          "<meta http-equiv=Refresh content=\"20; url=players.html\">\n");
  if (len == 0) {
    fprintf(htmlptr, "<h2>Note: you may need to hit the reload button to update<BR>"
                     "Current status: no players online</h2><br>\n");
    fprintf(htmlptr, "</body></html>\n");
    FCLOSE(htmlptr);
    return;
  }
  fprintf(htmlptr, 
          "<h2>Note: you may need to hit the reload button to update this list<BR>"
          "Currently there are %d player%c online</h2>\n"
          "<br><br><br>\n"
          "<table border cellspacing=3 cellpadding=3>\n"
          "<tr><td>Name</td><td>Level</td><td>Title</td></tr>\n", 
          len, (len > 1) ? 's' : ' ');
  
  for (i = 0 ; i < len ; i++) {
    buff = COPY(make_title(ptitle(a[i]), pname(a[i])));
    strip_color(buff, buff);
    fprintf(htmlptr, "<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", 
            pname(a[i]), player_level(a[i]), buff);
    free(buff);
  }
 fprintf(htmlptr, "</table></body></html>\n");
 FCLOSE(htmlptr);
}
#endif

void whocom (void) {
  int i, tlen, clen, txt_len;
  char level[30], *pad = NULL, *mud;
  Text rendered = {0};
  char *title;
  int a[max_players], a_len = 0;
  char *ptr;

  for (i = 0; i < max_players; i++)
    if (is_in_game(i) && (!pvis(i) || pvis(i) <= plev(mynum) || i == mynum))
      a[a_len++] = i;
  qsort (a, a_len, sizeof (int), cmp_player);

   if (a_len == 0) {
     bprintf("No users online.\n");
     return;
   }

bprintf(
COL_1 "+" COL_2 "-------------" COL_1 "+" COL_2 "---------------------------------------------------------------" COL_1 "+\n"
COL_2 "|&+C Level       " COL_2 "| &+CTitle                                                         " COL_2 "|\n"
COL_1 "I" COL_2 "-------------" COL_1 "I" COL_2 "---------------------------------------------------------------" COL_1 "I\n");

  for (i = 0; i <= a_len; ++i) {
    if (i == a_len)
      strcpy (level, "&+WAI&N          ");
    else if (ststflg (a[i], SFL_FREAQ))
      strcpy (level, "&+GFreaQ&N       ");
    else
      strcpy (level, player_level(a[i]));

    free(rendered.data); rendered = (Text){0};
    if (i == a_len)
      text_append(&rendered, "&+BBob&N wants to talk to you!");
    else if (ststflg (a[i], SFL_FREAQ))
      { char *part = text_format("&+w%s, tHe ToTAL aNd AbSoLuTE Frea&+GQ&*!",pname(a[i])); text_append(&rendered, part); free(part); }
    else
      { char *part = text_format("&+w%s", make_title (ptitle (a[i]), pname (a[i]))); text_append(&rendered, part); free(part); }

    if (i != a_len) {
      if (!ststflg (mynum, SFL_NOFLAG)) {
        if (ststflg (a[i], SFL_NOCHAT))
	  text_append(&rendered, " &+R[&+WNo&+cChat&+R]");
        if (ststflg (a[i], SFL_NOANON))
	  text_append(&rendered, " &+R[&+WNo&NAnon&+R]");
        if (ststflg (a[i], SFL_AWAY))
          text_append(&rendered, " &+R[&+YAway&+R]");
        if (ststflg (a[i], SFL_BUSY))
	  text_append(&rendered, " &+R[&+MBusy&+R]");
        if (ststflg (a[i], SFL_CODING))  
	  text_append(&rendered, " &+R[&+CCoding&+R]");
        if (linkdead(a[i]))
          text_append(&rendered, " &+C[&=WmLinkDead&N&+C]&N");
        if (players[a[i]].inmailer)
          text_append(&rendered, " &+Y[In Mailer]&N");
	if (newplr(a[i]))
	  text_append(&rendered, " &+C[New Player]");
	
	if (plev (mynum) >= LVL_WIZARD) {
	  if (ststflg (a[i], SFL_QUIET))
	    text_append(&rendered, " &+R[&+cQuiet&+R]");
	  if (ststflg (a[i], SFL_NOWIZ))
	    text_append(&rendered, " &+R[&+WNo&+MWiz&+R]");
	  if (ststflg (a[i], SFL_NOWISH))
	    text_append(&rendered, " &+R[&+WNo&+BWish&+R]");
	}
	if (plev (mynum) > LVL_ISTARI && ststflg (a[i], SFL_NOISTARI))
	  text_append(&rendered, " &+R[&+WNo&+GIstari&+R]");
	
	if (plev (mynum) > LVL_ARCHWIZARD && ststflg (a[i], SFL_NOAWIZ))
	  text_append(&rendered, " &+R[&+WNo&+rAWiz&+R]");
	
	if (plev (mynum) > LVL_DEMI &&ststflg (a[i], SFL_NODEMI))
	  text_append(&rendered, " &+R[&+WNo&+yDemi&+W]");
	
	if (plev (mynum) > LVL_SHALAFI && ststflg (a[i], SFL_NOGOD))
	  text_append(&rendered, " &+R[&+WNo&+BGod&+R]");
	
	if (plev (mynum) > LVL_GOD && ststflg (a[i], SFL_NOUPPER))
	  text_append(&rendered, " &+R[&+WNo&+bUpper&+R]");
      }
    }
    title = rendered.data;
    tlen = strlen(title);
    clen = count_colors(title);
    txt_len = tlen - clen;

    if (txt_len < 61) {
      free(pad); pad = makepad(title, 60);
      bprintf(COL_2 "| &N%-12s" COL_2 "|&N %s " COL_2 "|\n", level, pad);
    }
    else {
      ptr = makenewline(title, 60);
      *ptr = 0;
      free(pad); pad = makepad(title, 60);
      bprintf(COL_2 "| &N%-12s" COL_2 "|&N %s " COL_2 "|\n", level, pad);
      txt_len -= 60;

      while(txt_len > 0) {
        free(pad); pad = makepad(ptr + 1, 60);
        bprintf(COL_2 "|             |&N %s " COL_2 "|\n", pad);
        ptr = makenewline(ptr + 1, 60);
        *ptr = 0;
        txt_len -= 60;
      }
    }
  }


  if (strlen(MUD_NAME) > 33) {
    mudlog("ERROR: MUD_NAME too long");
    free(pad); free(rendered.data); return;
  }

  mud = makepad(MUD_NAME, 33);

bprintf(COL_1 "I" COL_2 "-------------" COL_1 "+" COL_2 
       "---------------------------------------------------------------"
        COL_1 "I\n" COL_2 
       "| &+cThere %s currently &+Y%-2d &+cvisible player%s &Non &+G%s%s"
        COL_2 "|\n" COL_1 "+" COL_2
"-----------------------------------------------------------------------------" 
        COL_1 "+\n", a_len > 1 ? "are" : "is", a_len, a_len > 1 ? "s" : "", 
        mud, a_len > 1 ? "" : "  ");
  free(mud); free(pad); free(rendered.data);
}

/* Caller owns the padded string; preserve the historical extra space. */
char *makepad(char *text, int width) {
  size_t len, copied;
  char *pad;
  if (width < 0) memory_failure();
  len = memory_add((size_t)width, (size_t)count_colors(text));
  pad = memory_alloc(memory_add(len, 2), 1);
  memset(pad, ' ', len + 1);
  copied = strlen(text); if (copied > len) copied = len;
  memcpy(pad, text, copied);
  return pad;
}

/* go to the LEN-th non-colorcode character, back up to space */

char *makenewline (char *str, int len) {
  char *ptr;
  int pos = 0;

  for (ptr = str ; *ptr && pos != len ; ptr++) {
    if (*ptr == '&')
      switch(*(ptr + 1)) {
      case 'N': case '*': case 'B': case '#': case 'n':
        ptr++;
        break;
      case '+': case '-':
        ptr += ptr[2] ? 2 : 1;
        break;
      case '=':
        ptr += ptr[2] && ptr[3] ? 3 : strlen(ptr) - 1;
        break;
      case '>':     /* not working properly */
        *ptr = 0;
        return(ptr + 1);
        break;
      default:
        pos++;
    }
    else
      pos++;
  }

  /* back up to the last whitespace */
  for ( ; ptr > str && *ptr ; ptr--)
    if (isspace(*ptr))
      return(ptr);
  return(str);
}

void whoncom (void)
{
  int i;
  int a[max_players], a_len = 0;

  for (i = 0; i < max_players; i++)
    if (is_in_game (i) && (pvis (i) <= plev (mynum) || i == mynum)) {
      a[a_len++] = i;
    }
  qsort (a, a_len, sizeof (int), cmp_player);

  bprintf ("&+CName             Title\n");
  bprintf ("&+b---------------- --------------------------------------------------------------\n");

  for (i = 0; i < a_len; ++i) {

    bprintf ("&+B[&+W%12s&+B]", pname (a[i]));

    bprintf ("%s", pvis (a[i]) > 0 ? "&+R*" : " ");

    if (plev (mynum) < LVL_WIZARD)
      bprintf ("  ");
    else
      bprintf ("%s ", aliased (a[i]) ? "&+C*" : " ");

    if (ststflg (a[i], SFL_FREAQ))
      bprintf ("&+w%s, tHe ToTAL aNd AbSoLuTE Frea&+GQ&*!", pname (a[i]));
    else
      bprintf ("&+w%s", make_title (ptitle (a[i]), pname (a[i])));

    if (!ststflg (mynum, SFL_NOFLAG)) {
      if (ststflg (a[i], SFL_NOCHAT))
	bprintf (" &*[NOCHAT]");
      if (ststflg (a[i], SFL_NOANON))
	bprintf (" &*[NoAnon]");
      if (ststflg (a[i], SFL_AWAY))
	bprintf (" &*[Away]");
      if (ststflg (a[i], SFL_BUSY))
	bprintf (" &*[Busy]");
      if (ststflg (a[i], SFL_CODING))
	bprintf (" &*[Coding]");
      if (newplr(a[i]))
	bprintf (" &+C[New Player]");

      if (plev (mynum) >= LVL_WIZARD) {
	if (ststflg (a[i], SFL_QUIET))
	  bprintf (" &*[Quiet]");
	if (ststflg (a[i], SFL_NOWIZ))
	  bprintf (" &*[NoWiz]");
	if (ststflg (a[i], SFL_NOWISH))
	  bprintf (" &*[NoWish]");
      }
      if (plev (mynum) > LVL_ISTARI) {
	if (ststflg (a[i], SFL_NOISTARI))
	  bprintf (" &*[NoIstari]");
      }
      if (plev (mynum) > LVL_ARCHWIZARD) {
	if (ststflg (a[i], SFL_NOAWIZ))
	  bprintf (" &*[NoAWiz]");
      }
      if (plev (mynum) > LVL_DEMI) {
	if (ststflg (a[i], SFL_NODEMI))
	  bprintf (" &*[NoDemi]");
      }
      if (plev (mynum) > LVL_SHALAFI) {
	if (ststflg (a[i], SFL_NOGOD))
	  bprintf (" &*[NoGod]");
      }
      if (plev (mynum) > LVL_GOD) {
	if (ststflg (a[i], SFL_NOUPPER))
	  bprintf (" &*[NoUpper]");
      }
    }
    bprintf ("\n");
  }

  bprintf ("&+b---------------- --------------------------------------------------------------\n");

  bprintf ("&+wThere %s &+C%d &+wvisible player%s currently on the game.\n",
	   (a_len > 1 ? "are" : "is"), a_len, (a_len > 1 ? "s" : ""));
}


/* The MWHO command
 */
#define COLUMNS (72/MNAME_LEN)
void
mwhocom (void)
{
  char buff[64];
  int i;
  int ct;
  int zone;
  int nr;

  int first = max_players;
  int last = numchars;
  zone = -1;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (brkword () != -1) {
    if ((zone = get_zone_by_name (wordbuf)) == -1) {

      bprintf ("No such zone: %s\n", wordbuf);
      return;
    }
    first = 0;
    last = znumchars (zone);
  }
  bprintf ("Live Mobiles\n------------\n\n");

  for (ct = first, nr = 0; ct < last; ++ct) {

    i = (zone == -1) ? ct : zmob_nr (ct, zone);

    if (!EMPTY (pname (i)) && pstr (i) >= 0 &&
	(pvis (i) <= 0 || pvis (i) <= plev (mynum))) {
      ++nr;
      if (pvis (i) > 0)
	sprintf (buff, "(%s)", pname (i));
      else
	strcpy (buff, pname (i));

      bprintf ("%-*s%c",
	       PNAME_LEN + 3, buff, (nr % COLUMNS) ? ' ' : '\n');
    }
  }
  bprintf ("\n\nThere are %d mobiles left (from %d)\n",
	   nr, last - first);
}


int player_damage (int player) {
  int w;
  int damage = pdam (player);

  if ((w = pwpn(player)) != -1 && iscarrby(w, player) && iswielded(w))
    damage += odamage (w);

  if (damage < MAX_WEAPON_DAM)
    return(damage);
  else
    return(MAX_WEAPON_DAM);
}

void usercom (void) {
  static char *WizLevs[] = {
    NULL, "App", "Necr", "Emrt", "Wiz", "Ist",
    "AWiz", "Demi", "Shal", "God", "Crtr"
  };

  int i, j;
  int a[max_players], a_len = 0;
  char idlebuff[64];

  for (i = 0; i < max_players; i++)
    if (is_in_game (i) && (!pvis(i) || pvis (i) <= plev (mynum) || i == mynum))
      a[a_len++] = i;

  qsort (a, a_len, sizeof (int), cmp_player);

  if (a_len == 0) {
    bprintf("No users currently online.\n");
    return;
  }

  if (plev(mynum) < LVL_WIZARD)
    bprintf("&+CLevel  Name        Sex Idle\n"
    "&+g--------------------------------\n" );
  else
    bprintf("&+CLevel  Name        Sex Str/Max Magic DP AC Vis Status   Location      Idle\n"
    "&+g----------------------------------------------------------"
    "--------------------\n" );

  for (j = 0; i = a[j], j < a_len; ++j) {
    if (ststflg (i, SFL_FREAQ))
      bprintf ("&+y[&+W%4.4s&+y]", "FreQ");
    else if( tlookup (pname(i), coders) != -1 )
      bprintf( "&+y[&+W%4.4s&+y]", "Codr" );
    else if (plev (i) <= LVL_WIZARD)
      bprintf ("&+y[&+W%4d&+y]", plev (i));
    else
      bprintf ("&+y[&+W%4.4s&+y]", WizLevs[wlevel (plev (i))]);

    bprintf (" &+Y%-13s%-5s", pname (i), psex(i) ? "&+MF" : "&+yM");

    if( plev(mynum) >=  LVL_WIZARD ) {
      if (plev (i) < LVL_WIZARD)
        bprintf (" %s%3d&+y/&+Y%-3d %s%2d&+y/&+Y%-3d&+c%2d %2d ",
	       	pstr(i) < maxstrength(i) ? "&+m" : "&+Y", 
		pstr (i), maxstrength (i),
	       	pmagic(i) < maxmagic(i) ? "&+m" : "&+Y",
	 	pmagic (i), maxmagic (i),
	       	player_damage (i),
	       	pac (i) > MAXARMOR ?
	       	MAXARMOR : pac (i));
      else
        bprintf (" &+g---&+y/&+g--- --&+y/&+g-- -- -- ");

      if (pvis (i) < 999)
        bprintf ("&+G%3d", pvis (i));
      else
        bprintf ("&+GInv");

      bprintf( " &+c%s", pfighting(i) != -1 ? "&+Rfighting" :
			 psitting(i) ? "sitting " : "standing");
    }

    if( plev(mynum) >= LVL_WIZARD )
    	bprintf (" &+B%-14.14s", showname (ploc (i)));

    if (ptstflg (mynum, PFL_SEEIDLE)) {
      strcpy (idlebuff, sec_to_hhmmss (global_clock - prlast_cmd (i)));
    } else {
      strcpy (idlebuff, sec_to_hhmmss (global_clock - plast_cmd (i)));
    }

    bprintf ("&+R%8.8s\n", idlebuff);

  }
  if( plev(mynum) < LVL_WIZARD )
  	bprintf( "&+g--------------------------------\n" );
  else
  	bprintf( "&+g-------------------------------------------------------------------------------\n" );
  if (EMPTY(item1))
    bprintf ("&+cThere %s &+C%d&+c user%s currently on the game.\n",
             (a_len == 1 ? "is" : "are"), a_len, (a_len == 1 ? "" : "s"));
  else
    bprintf ("&+cThere %s &+C%d&+c user%s on that match that pattern.\n",
             (a_len == 1 ? "is" : "are"), a_len, (a_len == 1 ? "" : "s"));

}

void showplayer (void) {
  extern char *Mflags[];
  PERSONA d;
  int b;
  int max_str, max_mag;
  int armor;
  long w;
  char *title, *name;
  Boolean in_file, is_mobile;
  Boolean is_female;

  if (!ptstflg (mynum, PFL_STATS)) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    usercom();
    return;
  }
  if ((b = fmbn(wordbuf)) != -1) {
    in_file = False;
    is_mobile = b >= max_players;
    player2pers (&d, NULL, b);
    name = pname (b);
    title = is_mobile ? NULL : ptitle (b);
    max_str = is_mobile ? pstr_reset (b) : maxstrength (b);
    max_mag = is_mobile ? 0 : maxmagic (b);
    is_female = ststflg (b, SFL_FEMALE);
  } else if (getuaf (wordbuf, &d)) {
    in_file = True;
    is_mobile = False;
    title = d.player.ptitle;
    name = d.ublock.pname;
    max_str = pmaxstrength (d.ublock.plev);
    max_mag = pmaxmagic (d.ublock.plev);
    is_female = ptst_flg(d.ublock.bits, SFL_FEMALE, SFLAGS);
  } else {
    bprintf ("Who's that?\n");
    return;
  }

  bprintf ("&+WName       &+C: &+w%s", name);

  if (!is_mobile) {
    bprintf ("\n&+WTitle      &+C: &+w%s", make_title (title, "<name>"));
    bprintf ("\n&+WScore      &+C: &+w%d", d.ublock.pscore);
    bprintf ("\n&+WLevel      &+C: %s - %s (level %d)", 
      Classes[d.ublock.class], levnam(d.ublock.plev, d.ublock.class, is_female),
      d.ublock.plev);
  }
  bprintf ("\n&+WStrength   &+C: &+w%d / %d", d.ublock.pstr, max_str);

  if (!is_mobile)
    bprintf ("\n&+WMagic      &+C: &+w%d / %d", d.player.pmagic, max_mag);

  if (!in_file && (w = pwpn (b)) != -1 && iscarrby (w, b) && iswielded (w)) {
    int w_damage = is_mobile ? odamage (w) / 2 : odamage (w);

    w_damage += d.ublock.pdam;
    bprintf ("\n&+WDamage     &+C: &+w%d (Wielding: %s)", w_damage, oname (w));
  } 
  else
    bprintf ("\n&+WDamage     &+C: &+w%d", d.ublock.pdam);

  if (!in_file) {
    if (pac (b) > d.ublock.parmor)
      bprintf ("\n&+WArmor      &+C: &+w%d%% / %d%% (Worn AC: %d%%)",
	       pac (b), MAXARMOR, pac (b) - d.ublock.parmor);
    else
      bprintf ("\n&+WArmor      &+C: &+w%d%% / %d%%", armor = d.ublock.parmor, MAXARMOR);

  } 
  else
    bprintf ("\n&+WArmor      &+C: &+w%d%% / %d%%", armor = d.ublock.parmor, MAXARMOR);


  bprintf ("\n&+WVisibility &+C: &+w%d", d.ublock.pvis);

  if (is_mobile) {
    bprintf ("\n&+WAggression &+C: &+w%d %%", pagg (b));
    bprintf ("\n&+WSpeed      &+C: &+w%d", pspeed (b));
    bprintf ("\n&+WStart      &+C: &+w%s@%s", 
      lname((int) d.ublock.ploc), zname(d.ublock.zone));
    bprintf ("\n&+W%s      &+C: &+w%s",
	     zpermanent (pzone (b)) ? "Zone " : "Owner", zname (pzone (b)));
  }
  else {
    bprintf ("\n&+WKill/Death &+C: &+w%d / %d", d.player.pkilled, d.player.pdied);
    bprintf ("\n&+WStart      &+C: &+w%s", d.ublock.phome ?
      d.ublock.phome : "<default>");
  }
  bprintf ("\n&+WWimpy      &+C: &+w%d", d.ublock.pwimpy);
  
  if (!in_file) {
    bprintf ("\n&+WLocation   &+C: &+w%s &+B(&*%s&+B)",
	     sdesc (ploc (b)), showname (ploc (b)));

    bprintf ("\n&+WCondition  &+C:&+w");

    if (pfighting (b) >= 0)
      bprintf (" Fighting %s", pname (pfighting (b)));
    else if (phelping (b) >= 0)
      bprintf (" Helping %s", pname (phelping (b)));
    else if (psitting (b))
      bprintf (" Sitting");
    else
      bprintf (" Standing");

    if (is_mobile) {
      bprintf ("\n&+WBehavior   &+C: &+w");
      show_bits (mbits(b), mindex, Mflags, MFLAGS, False);
    } 
    else if (ptstflg (mynum, PFL_SEEUSER)) 
      bprintf ("\n&+WUser/Host  &+C: &+W%s", username(b));  
    else if (ptstflg (mynum, PFL_SEEHOST)) 
      bprintf ("\n&+WFrom Host  &+C: &+w%s", hostname(b));
    if (!is_mobile && plev (mynum) > LVL_GOD) {
      if (*os(b))
        bprintf ("\n&+WOS         &+C: &+w%s", os(b));
      bprintf("\n");
      if (*keyboard(b))
        bprintf ("&+WCharset    &+C: &+w%s\t", keyboard(b));
      bprintf("&+WMynum      &+C: &+W%d\t"
	      "&+WPort       &+C: &+W%d", b, port(b));
     }
  } 
  else {
    bprintf ("\n&+WLast on    &+C: &+w%s", time2ascii (d.player.last_on));
    if (ptstflg (mynum, PFL_SEEUSER)) 
      bprintf ("\n&+WUser/Host  &+C: &+W%s", d.rplr.usrname);
    else if (ptstflg (mynum, PFL_SEEHOST))
      bprintf ("\n&+WFrom Host  &+C: &+w%s", d.rplr.hostname);
  }
  bprintf ("\n");
}


/* The MOBILE command
 */
void
mobilecom (void)
{
  int i;
  int ct;
  int zone = 0;
  int live = 0;
  int dead = 0;

  Boolean list_all = True;
  int first = max_players;
  int last = numchars;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  if (brkword () != -1) {
    list_all = False;
    if ((zone = get_zone_by_name (wordbuf)) == -1) {
      bprintf ("No such zone: %s\n", wordbuf);
      return;
    }
    first = 0;
    last = znumchars (zone);
  }
  bprintf ("Mobiles");

  if (the_world->w_mob_stop)
    bprintf ("   [Currently STOPped]");

  bprintf ("\n\n");

  for (ct = first ; ct < last; ++ct) {
    i = list_all ? ct : zmob_nr (ct, zone);

    bprintf ("[%d] %-*s %c", int2idx(i, MOB), MNAME_LEN,
	     EMPTY (pname (i)) ? pname_reset (i) : pname (i),
	     ststflg (i, SFL_OCCUPIED) ? '*' : ' ');

    if (EMPTY (pname (i))) {
      bprintf ("<QUIT or KICKED OFF>\n");
      ++dead;
    } else {
      desrm (ploc (i), IN_ROOM);
      if (pstr (i) < 0)
	++dead;
      else
	++live;
    }
  }
  bprintf ("\nA total of %d live mobile(s) + %d dead = %d.\n",
	   live, dead, live + dead);
}


/* list people in a room */

void list_people (void) {
  char xx[SETIN_MAX + 200];
  int i, j;
  int room = ploc (mynum);

  for (j = 0; j < lnumchars (room) ; j++) {
    i = lmob_nr(j, room);
    list_parts(i);
    if (i != mynum && is_in_game (i) && seeplayer (i)) {
      if (plev (mynum) >= LVL_WIZARD && pvis (i) > 0)
	bprintf ("(");

      if (i >= max_players) {
	if (pstr(i) > 0)
	  bprintf ("%s", pftxt (i));
	else
          {
            int t_elapsed;
            time_t cur_time;
	    int p;
	    char buff[100];

            time(&cur_time);
            t_elapsed = cur_time - ublock[i - max_players].death_time;
	    if (t_elapsed < 2 * SECS_IN_A_MIN)       p = 0;
	    else if (t_elapsed < 5 * SECS_IN_A_MIN)  p = 1;
	    else if (t_elapsed < 8 * SECS_IN_A_MIN)  p = 2;
	    else if (t_elapsed < 11 * SECS_IN_A_MIN) p = 3;
	    else if (t_elapsed < 15 * SECS_IN_A_MIN) p = 4;
	    else if (t_elapsed < 20 * SECS_IN_A_MIN) p = 5;
            else {
		 setploc (i, LOC_DEAD_DEAD);
		 continue;
	    }
	    sprintf(buff, "&+C%s", ocorpse[p]);
	    bprintf(buff, pname(i));
	  }
      } 
      else {                              /* i < max_players */
	bprintf ("%s%s", psitting (i) ?
	       build_setin (SETIN_SETSIT, xx, players[i].setsit, pname (i), NULL, NULL) :
	       build_setin (SETIN_SETSTAND, xx, players[i].setstand, pname (i), NULL, NULL),
		 ststflg (i, SFL_LIT) ? " &+Y(glowing)&*" : "");
        setname(i);
      }

      if (plev (mynum) >= LVL_WIZARD && pvis (i) > 0)
	bprintf (")");

      bprintf("\n");
      if (!ststflg (mynum, SFL_NOINV) && gotanything(i) && pstr(i) > 0 &&
          i != mynum) {
	bprintf("%s is carrying:\n", psex(i) ? "She" : "He");
	mlobjsat(i, 4);
      }
    }
  }
}

/* this now handles mobile movement, mob/player fighting, fleeing */

void loop_all_players (void) {
  int mon;

  for (mon = 0; mon < max_players; mon++)
    if (is_in_game(mon))
      check_player(mon);

  for (mon = max_players; mon < numchars; mon++)
    if (pstr(mon) > 0) {
      if (ublock[mon].pspeed && rand() % 30 < pspeed(mon) &&
        !ststflg(mon, SFL_CRIPPLED))
        movemob(mon);
      check_mobile(mon);
      causefight(mon);
    }
}

void check_player (int i) {

  if (pfighting(i) != -1) {
    if (pstr(i) >= 0 && pstr(i) < pwimpy(i) && pstr(pfighting(i)) >= 0) {
      sendf(i, "You run for your life as your health reaches %d.\n",pstr(i));
      flee(i, False);
    }
    else
      hit_player (i, pfighting(i), pwpn(i));
  }

  if (phelping(i) != -1)
    helpchkr (i);

#ifdef LOCMIN_CASTLE
  if (carries_obj_type(i, OBJ_CASTLE_RUNESWORD) > -1)
    dorune(i);
#endif
  if (wears_obj_type(i, OBJ_LIMBO_POUNCIE) > -1)
    dopouncie(i);
}

void causefight (int mon) {
  int plx, ct;

  if (!pagg(mon) || ststflg(mon, SFL_OCCUPIED) || ststflg(mon, SFL_BLIND) ||
       testpeace(mon) || pfighting(mon) != -1) 
          return;

  for (ct = 0 ; ct < lnumchars(ploc(mon)) ; ct++) {
    plx = lmob_nr(ct, ploc(mon));

    if (plx < max_players && is_in_game(plx) && !ptstflg(plx, PFL_NOHASSLE) 
          && !pvis(plx) && !linkdead(plx)) {

      if (randperc() >= pagg(mon) && pangry(mon) == -1)
        continue;

      if (pangry(mon) != -1 && ploc(pangry(mon)) != ploc(mon))
        continue;

  #ifdef LOCMIN_CHURCH
      else if (mtstflg(mon, MFL_CROSS) &&
        carries_obj_type(plx, OBJ_CHURCH_CROSS) > -1)
          sendf(plx, "%s seems afraid of your holy symbol.\n", pname(mon));
  #endif

      else if (mtstflg (mon, MFL_NOHEAT) && hasobjtype(plx, OFL_LIT))
	sendf (plx, "%s seems frightened by your light source.\n", pname(mon));

  #ifdef LOCMIN_THYRANNEN
      else if (iswornby(OBJ_THYRANNEN_CHARM, plx) &&
               (mon == MOB_THYRANNEN_SLIMY   + max_players ||
                mon == MOB_THYRANNEN_PUDDLE  + max_players ||
                mon == MOB_THYRANNEN_GHASTLY + max_players ||
                mon == MOB_THYRANNEN_GHOULY  + max_players ||
                mon == MOB_THYRANNEN_GRIMY   + max_players ||
                mon == MOB_THYRANNEN_PLATO   + max_players ||
                mon == MOB_THYRANNEN_SLART   + max_players ||
                mon == MOB_THYRANNEN_BILL    + max_players ||
                mon == MOB_THYRANNEN_DANIEL  + max_players ||
                mon == MOB_THYRANNEN_CALLOUS + max_players))
        sendf(plx, "\001p%s\003 is frightened by the charm around your "
                   "neck.\n", pname(mon));
  #endif
      else
        hit_player (mon, plx, pwpn (mon));	/* Start the fight */
    }
  }
}

void check_mobile (int mon) {
  int j;
  int pct = randperc();

  /* mobile fight control */

  if ((j = pfighting(mon)) != -1) {
    if (j < 0 || j >= numchars || !is_in_game(j) || ploc(mon) != ploc(j) || testpeace(mon)) {
      pfighting(mon) = -1;
      if (j >= 0 && j < numchars) pfighting(j) = -1;
    }
    else if (j != -1)
      hit_player(mon, j, pwpn(mon));
    return;
  }

  if (mtstflg(mon, MFL_THIEF) && pct < 10) {
    stealstuff(mon);
    return;
  }
  
  if (mtstflg(mon, MFL_PICK) && pct < 20) {
    shiftstuff(mon);
    return;
  }

#ifdef LOCMIN_VALLEY
  if (pnum(mon) == MOB_VALLEY_CHICKEN && pct < 8)
    sendl(ploc(mon), "\001p%s\003 says 'The sky is about to fall in.'\n",
      pname(mon));
#endif

#ifdef LOCMIN_CATACOMB
  if (pnum (mon) == MOB_CATACOMB_GHOST && pct < 12)
    sendl(ploc(mon), "The Ghost moans, sending chills down your spine.\n");
#endif
  if (pnum (mon) == MOB_START_PUFF && pct < 10)
    puffcom(mon);

#ifdef LOCMIN_VALLEY
  if (pnum(mon) == MOB_VALLEY_MINDY && pct < 10)
    sendl(ploc(mon), "&+w\001p%s\003 %s\n",
	   pname (mon), mindy[my_random () % arraysize (mindy)]);
#endif

#ifdef LOCMIN_SHERWOOD
  if (pnum(mon) == MOB_SHERWOOD_EMMY && pct < 10)
    sendl(ploc(mon), "%s\n", emmy[my_random () % arraysize(emmy)]);
#endif
}

void dopouncie (int plx) {
  int ply, ct, me = real_mynum;
  char pounce[MAX_COM_LEN];

  if (pfighting (plx) >= 0)
    return;

  if (randperc () <= 40) {
    for (ct = 0 ; ct < lnumchars(ploc(plx)) ; ct++) {
      ply = lmob_nr(ct, ploc(plx));

      if (ply != plx && ply < max_players && is_in_game(ply) && randperc() <= 10) {
	sprintf (pounce, "pounce %s", pname (ply));
	sendf (plx, "The pouncie takes control of your body!\n");
	send_msg(sendloc(plx), 0, LVL_MIN, LVL_MAX, plx, NOBODY,
		  "\001p%s\003 pounces out of control!\n", pname (plx));
	setup_globals (plx);
	gamecom (pounce, True);
	setup_globals (me);
	return;
      }
    }
  }
}

#ifdef LOCMIN_CASTLE
void dorune (int plx) {
  int ply, ct;

  if (pfighting (plx) >= 0 || testpeace (plx))
    return;

  for (ct = 0 ; ct < lnumchars(ploc(plx)) ; ct++) {
    ply = lmob_nr(ct, ploc(plx));

    if (ply != plx && is_in_game(ply) && plev(ply) < LVL_WIZARD && 
        ply < max_players && randperc () >= 9 * plev (plx)) {
      sendf (plx, "The Runesword twists in your hands, lashing out savagely!\n");
      hit_player (plx, ply, OBJ_CASTLE_RUNESWORD);
      return;
    }
  }
}
#endif

void helpchkr (int plx) {
  int x = phelping (plx);

  if (!is_in_game (x)) {
    sendf (plx, "You can no longer help.\n");
    setphelping (plx, -1);
  } 
  else if (ploc (x) != ploc (plx)) {
    sendf (plx, "You can no longer help %s.\n", pname (x));
    sendf (x, "%s can no longer help you.\n", pname (plx));
    setphelping (plx, -1);
  }
}

void movemob (int x) {
  int n, r;

  if (the_world->w_mob_stop || ststflg(x, SFL_OCCUPIED) || pfighting(x) != -1)
    return;

  r = get_rand_exit_dir(ploc(x));

  if (r == -1)
    return;

  if ((n = getexit(ploc(x), r)) > NEXITS)  /* trying to leave zone */
    return;

  if (ltstflg(n, LFL_NO_MOBILES) || ltstflg(n, LFL_DEATH))
    return;

  if (count_players(ploc(x), COUNT_PLAYERS) > 0)
    send_msg(sendloc(x), 0, pvis(x), LVL_MAX, x, NOBODY,
      "\001p%s\003 has gone %s.\n", pname(x), exits[r]);

  setploc(x, n);

  if (count_players(ploc(x), COUNT_PLAYERS) > 0)
    send_msg(int2idx(n, LOC), 0, pvis(x), LVL_MAX, x, NOBODY, 
      "\001p%s\003 %s.\n", pname(x), mobenter[r]);
}

void
stopcom (void)
{
  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  the_world->w_mob_stop = 1;

  send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
	    "&+B[&+wMobiles Stopped&+B]\n");
}

void
startcom (void)
{
  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  the_world->w_mob_stop = 0;

  send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
	    "&+B[&+wMobiles Started&+B]\n");
}



/*
 * Steal random object from random player in the same room
 */

Boolean stealstuff (int m) {
  int p[50];			/* Room for mortals and objects */
  int i, j, k;
  int nr = 0;

  /* Count the number of mortals in the same room: */
  for (j = 0; j < lnumchars (ploc (m)) && nr < 50; j++) {
    i = lmob_nr (j, ploc (m));

    if (is_in_game (i) && i < max_players && plev (i) < LVL_WIZARD)
      p[nr++] = i;
  }
  if (nr == 0)
    return False;

  /* Select a random one of those in the same room: */
  i = p[randperc () % nr];

  /* Count the number of objects he carries that we can take: */
  for (nr = k = 0; k < pnumobs (i) && nr < 50; k++) {
    j = pobj_nr (k, i);

    if (iscarrby (j, i)) {
      if (((ocarrf (j) == WORN_BY || ocarrf (j) == BOTH_BY)
	   && !mtstflg (m, MFL_SWORN))
	  || otstbit(j, OFL_NEWBIE) ||
	  ((ocarrf (j) == WIELDED_BY || ocarrf (j) == BOTH_BY)
	   && !mtstflg (m, MFL_SWPN)))
	continue;
      else
	p[nr++] = j;
    }
  }

  if (nr == 0)
    return False;

  /* Select random object from those he carries
   */
  j = p[randperc () % nr];

  sendf (i, "\001p%s\003 steals the %s from you!\n", pname (m), oname (j));

  send_msg(sendloc(i), 0, LVL_MIN, LVL_MAX, i, NOBODY,
	    "\001p%s\003 steals the %s from \001p%s\003!\n",
	    pname (m), oname (j), pname (i));

  setoloc (j, m, CARRIED_BY);

  if (otstbit(j, OFL_WEAPON) &&
      (!mtstflg(m, MFL_SMART) || odamage(j) > odamage(pwpn(m)))) {
    set_weapon(m, j);
    sendl(ploc(m), "\001p%s\003 wields the %s.\n",
           pname(m), oname(j));
  }

  if (otstbit(j, OFL_ARMOR) &&
      (!mtstflg(m, MFL_SMART) || oarmor(j) > armor_avg(m)))
    wearcom(m, j);

  return True;
}

int armor_avg(int m) {
  int b, ct;
  int tot, num;
  
  tot = num = 0;

  for (ct = 0 ; ct < pnumobs(m) ; ct++) {
    b = pobj_nr(ct, m);
    if (iswornby(b, m)) {
      num++;
      tot += oarmor(b); 
    }
  }

   if (num)
     return(tot/num);
   else
     return(0);
}

void shiftstuff(int m) {
  int a, b;

  for (b = 0; b < lnumobs(ploc(m)) ; b++) {
    a = lobj_nr (b, ploc (m));

    if (otstbit(a, OFL_NEWBIE))
      continue;

    if (!oflannel (a)) {
      sendl(ploc (m), "\001p%s\003 takes the %s.\n",
	    pname (m), oname (a));

      setoloc (a, m, CARRIED_BY);

      if (otstbit(a, OFL_WEAPON) && 
          (!mtstflg(m, MFL_SMART) || odamage(a) > odamage(pwpn(m)))) {
        set_weapon(m, a);
        sendl(ploc(m), "\001p%s\003 wields the %s.\n",
	       pname(m), oname(a));
      }

      if (otstbit(a, OFL_ARMOR) &&
          (!mtstflg(m, MFL_SMART) || oarmor(a) > armor_avg(m)))
        wearcom(m, a);
    }
  }
}

char *xname (char *n){
  char *t;

  if (n != NULL && (t = strrchr (n, ' ')) != NULL)
    return t + 1;
  else
    return n;
}

void setname (int plx) {
  if (psex (plx)) {
    if (wd_her(real_mynum))
      FREE(wd_her(real_mynum));
    wd_them(real_mynum) = wd_her(real_mynum) = COPY(pname(plx));
  }
  else {
    if (wd_him(real_mynum))
      FREE(wd_him(real_mynum));
    wd_them(real_mynum) = wd_him(real_mynum) = COPY(pname(plx));
  }
}

Boolean see_player (int pla, int plb) {
  if (pla < 0 || pla >= numchars)
    return False;
  if (plb == pla || plb < 0 || plb >= numchars)
    return True;
  if (pvis (plb) > 0 && plev (pla) < pvis (plb))
    return False;
  if (ststflg (pla, SFL_BLIND))
    return False;
  if (ploc (pla) == ploc (plb) && r_isdark (ploc (pla), pla))
    return False;
  return True;
}


char *see_name (int pla, int plb) {
  return see_player (pla, plb) ? pname (plb) : "Someone";
}

Boolean seeplayer (int plx) {
  if (plx == mynum || plx < 0 || plx >= numchars)
    return(True);
  else if (!see_player (mynum, plx))
    return(False);
  else
    return(True);
}

int game_mobsearch(int a, int b) {
  if (is_in_game(a))
    return(True);
  return(False);
}

/* return a player/mob index. use find_mob() to check visibility */

int find_mob_by_name (Boolean chkmob, char *name) {
  char *mobname, *wd;
  int mob, i, loc;

  if (EQ(name, "him"))           /* apply abbreviations */
    wd = wd_him(real_mynum);
  else if (EQ(name, "her"))
    wd = wd_her(real_mynum);
  else if (EQ(name, "them"))
    wd = wd_them(real_mynum);
  else if (EQ(name, "me"))
    wd = pname(real_mynum);
  else
    wd = name;

  if (!wd)
    return(-1);
  else if ((mob = idtxt2int(wd, MOB)) != -1)
    return(mob);

  mobname = xname(wd);

  if (chkmob) {           /* check plr/mob in room and all mobs */
    loc = ploc(mynum); 
    if (exists(loc))
      for (i = 0; i < lnumchars (loc); i++) {
        mob = lmob_nr(i, loc);
        if (EQ (mobname, xname(pname(mob))) && is_in_game(mob))
          return(mob);
      }
    if ((mob = ht_lookup(ublock_n, mobname, 0, -1, game_mobsearch)) != -1) 
      return(mob);
  }

  for (mob = 0; mob < max_players; mob++) {
    if (!strncasecmp(mobname, pname(mob), strlen(mobname)))
      if (is_in_game(mob))
        return(mob);
  }
  return -1;
}

/* similar to find_mob_by_name although checks visibility */

int find_mob(Boolean chkmob, char *name) {
  int n;
  
  n = find_mob_by_name(chkmob, name);

  if (n != -1 && mynum != -1) {
    if (!seeplayer(n))
      return(-1);
    else
      setname(n);
  }
  return(n);
}

/* Find player or mobile, return number if in game.
 * Set f to true if in file, False if in game. If exists, put
 * the data in the object pointed to by p.
 */

int find_player (char *name, PERSONA * p, Boolean * f) {
  int plr;

  *f = False;
  if ((plr = fmbn(name)) >= 0) {
    player2pers (p, NULL, plr);
    return plr;
  } 
  else if (ptstflg (mynum, PFL_UAF) != 0 && getuaf (name, p)) {
    *f = True;
    return -2;
  }
  return -1;
}

int alive (int i) {
  if (pstr (i) < 0 || EMPTY (pname (i)))
    return -1;
  else
    return i;
}

Boolean do_okay_l (int p, int v, Boolean c) {
  int lev_p = wlevel (p);
  int lev_v = wlevel (v);

  if (lev_v > lev_p && lev_p > LEV_NEG) {
    return False;
  }
  return (c || (lev_v < lev_p && lev_p > LEV_WIZARD) || lev_p >= LEV_GOD);
}


/* Can p(layer) do XX to v(ictim) ?
 * ** prot_flag protects against it.
 */

Boolean do_okay (int p, int v, int prot_flag) {
  return do_okay_l (plev (p), plev (v),
		    (prot_flag < PFL_MAX && !ptstflg (v, prot_flag)));
}


void
setpsex (int chr, Boolean v)
{
  if (v)
    ssetflg (chr, SFL_FEMALE);
  else
    sclrflg (chr, SFL_FEMALE);
}


void setploc (int plr, int room) {
  /* already there */
  if (ploc(plr) == room)
    return;

  /* First remove plr from his current location: */
  if (exists (ploc (plr)))
    remove_int (plr, lmobs (ploc (plr)));

  /* Then add him to the new room: */
  if (exists (room))
    add_int (plr, lmobs (room));

  ploc (plr) = room;

  if (plr < max_players && room) {
    setprndsinroom(plr, 0);

    /* destruct wizard objects if necessary & set new zone : */
    if (pzone(plr) != lzone(room)) {
      destruct_clones(plr);
      pzone(plr) = lzone(room);
    }
  }
}

int
ptothlp (int pl)
{
  int ct;

  for (ct = 0; ct < numchars; ct++) {
    if (ploc (ct) == ploc (pl) && phelping (ct) == pl)
      return ct;
  }
  return -1;
}

char *make_title(char *title, char *name) {
  static char *scratch; /* borrowed until next call */
  Text text = {0};
  const char *p = *title ? title : "%s the Unknown";
  for (; *p; p++) {
    if (*p == '%' && p[1] == 's') { text_append(&text, name); p++; }
    else if (*p == '%' && p[1] == '%') { text_char(&text, '%'); p++; }
    else text_char(&text, *p);
  }
  free(scratch); scratch = text_take(&text);
  return scratch;
}


/* Try to reset a mobile. */

Boolean reset_mobile (int m) {
  presetflgs(m);
  mresetflgs(m);
  sresetflgs(m);
  eresetflgs(m);
  presetmsk(m);
  set_vital(m);

  setpsitting (m, 0);
  setpfighting (m, -1);
  setpangry (m, -1);
  setphelping (m, -1);
  setpwpn (m, -1);
  setplev (m, plev_reset (m));
  setpagg (m, pagg_reset (m));
  setpstr (m, pstr_reset (m));
  setpvis (m, pvis_reset (m));
  setpspeed (m, pspeed_reset (m));
  setpdam (m, pdam_reset (m));
  setparmor (m, parmor_reset (m));
  setpwimpy (m, pwimpy_reset (m));
  reset_armor (False, m);

  if (EMPTY (pname (m)))
    setpname (m, pname_reset (m));

  setploc (m, ploc_reset(m));
  return(True);
}

int tscale (void)
{
  int a = 0;
  int b;

  for (b = 0; b < max_players; b++)
    if (is_in_game (b) && plev (b) < LVL_WIZARD)
      a++;
  if (a < 2)
    return 1;
  else
    return (a > 9 ? 9 : a);
}

Boolean
chkdumb (void)
{
  if (!ststflg (mynum, SFL_DUMB))
    return False;
  bprintf ("You are mute.\n");
  return True;
}

Boolean
chksitting (void)
{
  if (!psitting (mynum))
    return False;
  bprintf ("You'll have to stand up, first.\n");
  return True;
}

void destroy_mobile (int mob)
{
  setploc (mob, LOC_DEAD_DEAD);
}

void set_msg (char **b, Boolean dir_ok, Boolean sum) {
  char k[MAX_COM_LEN];

  if (plev(mynum) < LVL_WIZARD) {
    bprintf("You must be a wizard to set travel messages.\n");
    return;
  }

  getreinput (k);

  if (!*k) {
    FREE(*b); *b = NULL;
    bprintf("Message reset to default.\n");
  }
  else if (check_setin (k, dir_ok, sum)) {
    FREE(*b);
    *b = COPY(k);
  }
  else
    bprintf ("Not changed, wrong format.\n");
}

Boolean
check_setin (char *s, Boolean d, Boolean v)
{
  char *p;
  int nn = 0, dd = 0, vv = 0;

  for (p = s; (p = strchr (p, '%')) != NULL;) {
    switch (*++p) {
    case 'n':
    case 'N':
      ++nn;
      break;
    case 'd':
      ++dd;
      break;
    case 'v':
      ++vv;
      break;
    }
  }

  if (v)
    return (vv > 0 && strlen (s) < SETIN_MAX);
  else
    return (nn > 0 && (dd == 0 || d) && strlen (s) < SETIN_MAX);
}

Boolean
check_nooracle (int plx)
{
  if (ststflg (plx, SFL_NOORACLE) && EQ (pname (mynum), "Oracle"))
    return True;
  else
    return False;
}

Boolean
check_busy (int plx)
{
  if (ststflg (plx, SFL_BUSY) && (plev (plx) > plev (mynum))) {
    bprintf ("%s is busy, try later!\n", pname (plx));
    return True;
  }
  return False;
}

Boolean
check_coding (int plx)
{
  if (ststflg (plx, SFL_CODING)) {
    return True;
  }
  return False;
}

Boolean
check_away (int plx)
{
  if (ststflg (plx, SFL_AWAY))
    return True;
  else
    return False;
}

Boolean
check_forget (int p1, int p2)
{
  int i;
  Boolean found = False;

  if (p1 >= max_players)
    return found;
  if (p2 >= max_players)
    return found;

  for (i = 0; i < 10; ++i) {
    if (rplrs[p1].forget[i] != -1) {
      if (!is_in_game (rplrs[p1].forget[i]))
	rplrs[p1].forget[i] = -1;
      else if (rplrs[p1].forget[i] == p2)
	found = True;
    }
  }
  return found;
}

void
wipe_forget (int plr)
{
  int i, j;

  for (i = 0; i < max_players; ++i) {
    for (j = 0; j < 10; ++j) {
      if (rplrs[i].forget[j] == plr)
	rplrs[i].forget[j] = -1;
    }
  }
  for (i = 0; i < 10; ++i)
    rplrs[plr].forget[i] = -1;
}

Boolean
check_armor (int plx, int obj)
{
  if ((pac (plx) + oarmor (obj)) > MAXARMOR)
    return True;
  else
    return False;
}

char *
make_prompt (char *buff, char *prompt, 
	     char *h, char *c, char *l, char *n, char *m)
{
  char *p, *q, *r;

  if (!prompt)
    return(DEFAULT_PROMPT);

  for (p = buff, q = prompt; *q != 0;) {
    if (*q != '%')
      *p++ = *q++;
    else {
      switch (*++q) {
      case 'h':		/* Health */
	if (h == NULL)
	  return NULL;
	for (r = h; *r != 0;)
	  *p++ = *r++;
	break;
      case 'c':		/* Score */
	if (c == NULL)
	  return NULL;
	for (r = c; *r != 0;)
	  *p++ = *r++;
	break;
      case 'm':		/* Mana  */
	if (m == NULL)
	  return NULL;
	for (r = m; *r != 0;)
	  *p++ = *r++;
	break;
      case 'l':		/* Level */
	if (l == NULL)
	  return NULL;
	for (r = l; *r != 0;)
	  *p++ = *r++;
	break;
      case 'n':		/* Name  */
	if (n == NULL)
	  return NULL;
	for (r = n; *r != 0;)
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
  if (p > buff && p[-1] == '\n')
    --p;
  *p = 0;
  return buff;
}

char *
build_prompt (int plx)
{
  static char b[EXPANDED_PROMPT_LEN];
  char xx[EXPANDED_PROMPT_LEN];
  char h[40];
  char c[40];
  char l[40];
  char n[40];
  char m[40];

  *b = 0;

  if (!players[plx].inmailer && !players[plx].inpager) {
    if (pconv (plx) != -1) {
      if (is_in_game (pconv (plx))) {
	sprintf (b, "&+B[&+WCONV: &+C%s&+B] &+w", pname (pconv (plx)));
	return b;
      }
    }
  }

  sprintf (h, "%d/%d", pstr (plx), maxstrength (plx));	/* Health */
  sprintf (c, "%u", pscore (plx));	                /* Score  */
  sprintf (l, "%d", plev (plx));	                /* Level  */
  sprintf (n, "%s", pname (plx));               	/* Name   */
  sprintf (m, "%d/%d", pmagic (plx), maxmagic (plx));	/* Mana   */

  sprintf (b, "%s%s%s%s",
	   pvis (plx) ? "(" : "",
	   aliased (plx) ? "@" : "",
	   make_prompt (xx, players[plx].prompt, h, c, l, n, m),
	   pvis (plx) ? ")" : "");

  return b;
}

int
doorthru (int x)
{
  if (x < DOOR)
    return x;
  if (state (x - DOOR))
    return 0;
  return oloc ((x - DOOR) ^ 1);
}

void do_follow () {
  int ct;
  int i;
  int me = mynum;
  int c = 0;

  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && i != mynum && pfollow(i) == mynum) {

      if (pfighting (i) >= 0) {
	sendf(i, "You can not follow %s out of a fight!\n", pname (mynum));
	sendf(i, "You can no longer follow %s.\n", pname (mynum));
	pfollow(i) = -1;
	continue;
      }
      if (!has_foot(i) && has_hand(i)) {
        bprintf("You can't follow anymore; you don't have any feet.\n");
        pfollow(i) = -1;
        continue;
      }
      if ((!has_foot(mynum) && !has_hand(mynum)) ||
        ststflg(mynum, SFL_CRIPPLED)) {
        bprintf("You're totally crippled; you can't follow.\n");
        pfollow(i) = -1;
        continue;
      }
      if (ltstflg (ploc (mynum), LFL_ON_WATER) && !carries_boat (i)) {
	sendf(i, "You can not follow %s into water!\n", pname (mynum));
	sendf(i, "You can no longer follow %s.\n", pname (mynum));
	pfollow(i) = -1;
	continue;
      }
      if (ploc (i) == ploc (mynum))
	continue;

      for (ct = 0; ct < 6; ++ct) {
	if (getexit (ploc (i), ct) == ploc (mynum) ||
	    doorthru (getexit (ploc (i), ct)) == ploc (mynum)) {
	  ++c;
	  setup_globals (i);
	  dodirn (False, ct + 2);
	  setup_globals (me);
	  continue;
	}
      }
      if (!c) {
	sendf(i, "You can no longer follow %s.\n", pname (mynum));
	pfollow(i) = -1;
      }
    }
  }
}

void
chk_fol (int plx)
{
  int ct;

  for (ct = 0; ct < 6; ++ct) {
    if (!(getexit (ploc (plx), ct) == ploc (mynum) ||
	  doorthru (getexit (ploc (plx), ct)) == ploc (mynum))) {
      pfollow(plx) = -1;
      sendf(plx, "You can no longer follow %s.\n", pname (mynum));
      return;
    }
  }
}

void
check_follow (void)
{
  int i;

  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && i != mynum && pfollow(i) == mynum) {
      chk_fol (i);
    }
  }
}

int vicf2 (int fl, int i) {
  int plr;

  if (ltstflg (ploc (mynum), LFL_NO_MAGIC) != 0 && plev (mynum) < LVL_WIZARD) {
    bprintf ("Something about this location has drained your mana.\n");
    return -1;
  }
  if (fl >= SPELL_VIOLENT && plev (mynum) < LVL_WIZARD && testpeace (mynum)) {
    bprintf ("No, that's violent!\n");
    return -1;
  }
  if ((plr = vicbase ()) < 0)
    return -1;

  if (pmagic (mynum) < 1 && plev(mynum) < LVL_WIZARD) {
    bprintf ("You are too weak to cast magic.\n");
    return -1;
  }
  if (plev (mynum) < LVL_WIZARD)
    setpstr (mynum, pstr (mynum) - 1);

#ifdef LOCMIN_BLIZZARD
  if (carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE) > -1)
    i++;
  if (carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE1) > -1)
    i++;
  if (carries_obj_type (mynum, OBJ_BLIZZARD_POWERSTONE2) > -1)
    i++;
#endif

#ifdef LOCMIN_FANTASY
  if (carries_obj_type (mynum, OBJ_FANTASY_MANA) > -1)
    i = i + 2;
#endif

  if (plev (mynum) < LVL_WIZARD && randperc () > i * plev (mynum)) {
    bprintf ("You fumble the magic.\n");
    if (fl == SPELL_REFLECTS) {
      bprintf ("The spell reflects back.\n");
      return mynum;
    }
    return -1;
  }
  if (plev (mynum) < LVL_WIZARD) {
    if (fl >= SPELL_VIOLENT && wears_antimagic( plr ) ) { 
      bprintf ("The spell is absorbed by %s shield!\n", his_or_her (plr));
      return -1;
    } else
      bprintf ("The spell succeeds!\n");
  }
  return plr;
}

int
vichfb (int cth)
{
  int a;

  if ((a = vicf2 (SPELL_VIOLENT, cth)) < 0)
    return a;
  if (ploc (a) != ploc (mynum)) {
    bprintf ("%s isnt here.\n", psex (a) ? "She" : "He");
    return -1;
  }
  return a;
}

int
vichere (void)
{
  int a;

  if ((a = vicbase ()) == -1)
    return -1;
  if (ploc (a) != ploc (mynum)) {
    bprintf ("They aren't here.\n");
    return -1;
  }
  return a;
}

int
vicbase (void)
{
  int a;

  do {
    if (brkword () == -1) {
      bprintf ("Who?\n");
      return -1;
    }
  }
  while (EQ (wordbuf, "at"));
  if ((a = fpbn(wordbuf)) < 0) {
    bprintf ("That person isn't playing now.\n");
    return -1;
  }
  return a;
}

void jumpcom (void) {
  int a, b, j, x;
  char ms[128];

  /* Where is it possible to jump from, and where do
   * * we land...
   */
  static int jumtb[] =
  {
  /* FROM,                    TO */

#if defined(LOCMIN_CASTLE) && defined(LOCMIN_VALLEY)
    LOC_CASTLE_LEAP,          LOC_VALLEY_NPATH,
#endif

#if defined(LOCMIN_TOWER) && defined(LOCMIN_CASTLE)
    LOC_TOWER_LEDGE,          LOC_CASTLE_WALLS,
#endif

#if defined(LOCMIN_TREEHOUSE) && defined(LOCMIN_VALLEY)
    LOC_TREEHOUSE_PORCH,      LOC_VALLEY_ESIDE,
#endif

#if defined(LOCMIN_MOOR) && defined(LOCMIN_LEDGE)
    LOC_MOOR_PIT,             LOC_LEDGE_PIT,
#endif

#ifdef LOCMIN_BLIZZARD
    LOC_BLIZZARD_WINDOW,      LOC_BLIZZARD_CHAMBER,
    LOC_BLIZZARD_CHAMBER,     LOC_BLIZZARD_WINDOW,
#endif

#ifdef LOCMIN_NOXYPICKLE
    LOC_NOXYPICKLE_TREE,      LOC_NOXYPICKLE_SLIDE, 
#endif

#ifdef LOCMIN_CATACOMB
    LOC_CATACOMB_CHASM1,      LOC_CATACOMB_CAVERN,
    LOC_CATACOMB_CHASM2,      LOC_CATACOMB_CAVERN,
    LOC_CATACOMB_SLEDGE2,     LOC_CATACOMB_CAVERN,
    LOC_CATACOMB_NLEDGE2,     LOC_CATACOMB_CAVERN,
    LOC_CATACOMB_SLEDGE1,     LOC_CATACOMB_CHASM1,
    LOC_CATACOMB_NLEDGE1,     LOC_CATACOMB_CHASM2,
#endif
    0, 0};

  j = -1;  /* makes compiler happy */

  if (psitting (mynum)) {
    bprintf ("You have to stand up first.\n");
    return;
  }
#ifdef LOCMIN_FROBOZZ
  if (ploc (mynum) == LOC_FROBOZZ_ENDING) {
    sprintf (ms, "%s jumps into the hole and disappears!\n", pname (mynum));
    sillycom (ms);
    setploc (mynum, LOC_BLIZZARD_GLITTERING);
    bprintf ("You jump into the hole...\n\n\n\nAnd fall....."
	     "Until the tunnel stops abruptly\n"
	     "and ends as a hole in a ceiling, "
	     "through which you fall and land\n");
    sprintf (ms, "%s comes falling through a hole in the ceiling and lands\n",
	     pname (mynum));
    sillycom (ms);
    if (ploc (mynum) == ploc (max_players + MOB_BLIZZARD_GIANT)) {
      bprintf ("on the stomach of a snoring giant!\n");
      sillycom ("on the stomach of the the snoring giant!\n");
      hit_player (max_players + MOB_BLIZZARD_GIANT, mynum, -1);
    } 
    else {
      bprintf ("on the ground with a thud, hurting yourself badly!\n");
      sillycom ("on the ground with a loud thud, being badly hurt!\n");
      if (plev (mynum) < LVL_WIZARD)
	setpstr (mynum, pstr (mynum) / 2);
    }
    return;
  }
#endif

#include "jumpcom.h"

  /* Search the jump-table for special locations..
   */
  for (a = 0, b = 0; jumtb[a]; a += 2) {
    if (jumtb[a] == ploc (mynum)) {
      b = jumtb[a + 1];
      break;
    }
  }

#ifdef LOCMIN_CATACOMB
  if (ploc(mynum) == LOC_CATACOMB_SLEDGE2 ||
      ploc(mynum) == LOC_CATACOMB_NLEDGE2) {
    if (oloc(OBJ_START_UMBRELLA) == mynum &&
        ocarrf(OBJ_START_UMBRELLA) == CARRIED_BY) {
      bprintf("You grab hold of the umbrella and step off the ledge.\n\n"
               "You are drifting slowly through the darkness of the "
               "chasm, falling towards an\nuncertain fate. The rocky "
               "walls slide past you, as does another ledge, just\n"
               "beyond your reach.  After several minutes, your descent "
               "comes to an end and\nyou alight in ...\n");
      trapch(b);
      return;
    } 
    else {
      bprintf("You step off the ledge, but instead of plummeting into the "
               "darkness below,\nyour motion is stopped and you find "
               "yourself ...\n");
      trapch((ploc (mynum) == LOC_CATACOMB_SLEDGE2) ?
              LOC_CATACOMB_CHASM1 : LOC_CATACOMB_CHASM2);
      return;
    }
  }
  if (ploc (mynum) == LOC_CATACOMB_SLEDGE1 ||
      ploc (mynum) == LOC_CATACOMB_NLEDGE1) {
    bprintf ("You make a leap of Faith, throwing yourself off the ledge. "
             "You are hurtling\nrapidly through the darkness of the chasm, "
             "falling to almost certain doom.\nThe rocky walls rush past "
             "you ...\n");
    trapch (b);
    return;
  }
#endif

  /* Are we by a pit ? If so we'll jump into it. */

  if (False);
#ifdef LOCMIN_START
  else if (ploc(mynum) == oloc(OBJ_START_PIT)
      || ploc(mynum) == oloc(OBJ_START_CHURCH_PIT) 
  #ifdef LOCMIN_ORCHOLD
      || ploc(mynum) == oloc(OBJ_ORCHOLD_HOLEORCS)
  #endif
    ) {
    b = LOC_PIT_PIT;
    if (plev(mynum) < LVL_WIZARD) {
      quit_msg("You plundge into the infinate voids...", "Suicide");
      quit_player(False);
    }
  }
#endif
#ifdef LOCMIN_CATACOMB
  else if (ploc(mynum) == ploc(OBJ_CATACOMB_PIT_NORTH))
    j = OBJ_CATACOMB_PIT_NORTH;
  else if (ploc(mynum) == ploc(OBJ_CATACOMB_PIT_SOUTH))
    j = OBJ_CATACOMB_PIT_SOUTH;
  else if (ploc(mynum) == ploc(OBJ_CATACOMB_PIT_EAST))
    j = OBJ_CATACOMB_PIT_EAST;
  else if (ploc(mynum) == ploc(OBJ_CATACOMB_PIT_WEST))
    j = OBJ_CATACOMB_PIT_WEST;
  else if (state (j) == 0)
    b = LOC_CATACOMB_RIPS;
#endif

  if (b == 0) {
    bprintf ("Wheeeeee....\n");
    return;
  }
  if ((x = carries_obj_type (mynum, OBJ_START_UMBRELLA)) > -1 && !state(x)) {
    sprintf (ms, "%s jumps off the ledge.\n", pname (mynum));
    bprintf ("You grab hold of the %s and fly down like "
	     "Mary Poppins.\n", oname (x));
  } 
  else if (plev (mynum) < LVL_WIZARD) {
    sprintf (ms, "%s makes a perfect swan dive off the ledge.\n",
	     pname (mynum));
#ifdef LOCMIN_TOWER
    if (ploc(mynum) == LOC_TOWER_LEDGE) {
      send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum, NOBODY, ms);
      setploc (mynum, b);
      bprintf ("Wheeeeeeeeeeeeeeeee       <<<<SPLAT>>>>\n");
      quit_msg ("I suppose you could be picked up with a spatula!",
                "Jumped off a Ledge, Got Splattered Everywhere");
      quit_player(False);
    }
#endif
  } 
  else {
    if (False);
#ifdef LOCMIN_NOXYPICKLE
    else if (ploc(mynum) == LOC_NOXYPICKLE_TREE)
      sprintf(ms, "%s jumps out of the tree!\n", pname(mynum));
#endif
    else
      sprintf (ms, "%s dives off the ledge and floats down.\n", pname (mynum));
  }

  send_msg (sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum,
    NOBODY, ms);

  setploc (mynum, b);

  if (iscarrby (OBJ_START_UMBRELLA, mynum))
    sprintf (ms, "%s flys down, clutching an umbrella.\n", pname (mynum));
  else
    sprintf (ms, "%s has just dropped in.\n", pname (mynum));

  send_msg(sendloc(mynum), 0, pvis (mynum), LVL_MAX, mynum,
    NOBODY, ms);

  trapch (b);
}




/* Stuff that should be done both after every command and at every
 * i/o-interrupt at the latest.
 */

void special_events (int player)
{
  int start = (player == SP_ALL) ? 0 : player;
  int stop = (player == SP_ALL) ? max_players - 1 : player;	/*max_chars ? */
  int i;

  if (player >= max_players)
    return;

  for (i = start; i <= stop; ++i)
    if (is_in_game (i)) {

      setup_globals(i);

#ifdef LOCMIN_RUINS
      if (ploc (i) == LOC_RUINS_DEATHROOM) {
	broad ("A muffled cry is heard before the sound of shattering bones "
	       "drowns it out.\n");
	bprintf("You should really avoid those bottomless pits.\n");
        quit_msg("You should really avoid those bottomless pits",
                 "Took A Wrong Turn, Fell in a Pit, Got Eaten "
                 "Alive &+g(&+GK&+Ca&+GK&+g)");
	quit_player(False);
	continue;
      }
#endif

#ifdef LOCMIN_CASTLE
      if (ploc (i) == LOC_CASTLE_MAIDEN && state (OBJ_CASTLE_IN_MAIDEN) == 1) {
	broad ("There is a long drawn out scream in the distance..\n");
	quit_msg("The iron maiden closes....... &+RS Q U I S H !",
                 "Squished by the Iron Maiden");
	quit_player(False);
	continue;
      }
#endif

      if (plev (i) < LVL_WIZARD && ltstflg (ploc (i), LFL_ON_WATER)) {
	if (!carries_boat(i)) {
	  broad ("A loud gurgling is heard, then a crashing of waves..\n");
	  quit_msg("You plunge beneath the waves....",
                   "Forgot a Boat");
	  quit_player(False);
	  continue;
	}
      }

      /* 33% of losing health in NEGREGEN room */
      if (plev (i) < LVL_WIZARD && ltstflg (ploc (i), LFL_NEGREGEN) &&
	  randperc () < 33) {
	sendf(i, "You feel weaker.\n");
	if ((pstr (i) - 1) == 0) {
	  setpstr (i, 50);
	  quit_msg("You should have been more careful when your life "
		   "started slipping away..", "Died of Natural Causes");
	  quit_player(False);
	}
	setpstr (i, (pstr (i) - 1));
      }

#ifdef LOCMIN_FOREST
      /* 10% chance of the tree swallowing you */
      if (ploc(i) == LOC_FOREST_INTREE && 
          plev(i) < LVL_WIZARD && randperc() < 10) {

        quit_msg("You have been absorbed, and crushed to death by the " 
                 "tree....", "Tree Wins Again");
        broad ("There is a hideous scream and a grinding of bone...\n");
        quit_player(False);
        continue;
      }

      /* 40% chance that tree will suck you in */
      if (ploc (i) == oloc (OBJ_FOREST_TREEEATING) && randperc () < 40) {
	int me = real_mynum;

	sendf(i, "You are suddenly grabbed and taken into the tree!\n");
	setup_globals (i);
	teletrap (LOC_FOREST_INTREE);
	setup_globals (me);
      }

#endif

#ifdef LOCMIN_VILLAGE
      /* 10% chance that the floorboards will squeak */
      if (ploc (i) == oloc (OBJ_VILLAGE_TOP_BOARDS) && randperc () < 10)
        sendf (i, "The floorboards creak alarmingly as you move around.\n");
#endif

#ifdef LOCMIN_OAKTREE
      /* 10% chance of the fox barking */
      if (ploc (i) == oloc (OBJ_OAKTREE_TOPFOXHOLE) && randperc () < 10)
        sendf (i, "The sharp bark of a fox reverberates off the oaks.\n");
#endif

#if defined(LOCMIN_MOOR) && defined(LOCMIN_CHURCH)
      if (ploc (i) == oloc (OBJ_MOOR_ALTAR) && plev (i) < LVL_WIZARD) {
        if (oloc (OBJ_CHURCH_CROSS) == i) {
          sendf (i, "The altar and cross suddenly start sparking and "
                 "flashing, as if in some kind of\nconflict.  With "
                 "a massive bang, the altar cracks.\n");
          destroy (OBJ_MOOR_ALTAR);
        }
      }
#endif

#ifdef LOCMIN_OAKTREE
      if (ploc (i) == oloc (OBJ_OAKTREE_TOPFOXHOLE) &&
          ploc (i) == ploc (pnum (MOB_OAKTREE_OTTIMO)) &&
          state (OBJ_OAKTREE_TOPFOXHOLE) == 1) {
        sendl(ploc (i), "Ottimo digs up the fox hole.\n");
        setobjstate (OBJ_OAKTREE_TOPFOXHOLE, 0);
      }
#endif

      /* Turn off snoop */
      if (snooptarget(i) != -1) {
	if (!is_in_game(snooptarget(i))) {
	  sendf(i, "You can no longer snoop.\n");
	  snoop_off(i);
	}
        else if (ltstflg(ploc(snooptarget(i)), LFL_PRIVATE) && 
          plev(i) < LVL_ARCHWIZARD) {
	  sendf(i, "%s went into a PRIVATE room, you can no longer snoop.\n",
            pname(snooptarget(i)));
	  snoop_off(i);
	}
        else if (ltstflg(ploc(snooptarget(i)), LFL_NO_SNOOP) &&
          plev(i) < LVL_CREATOR) {
          sendf(i, "%s went into a NO SNOOP room, you can no longer snoop.\n",
            pname(snooptarget(i)));
          snoop_off(i);
        }
      }

      if (i < max_players && aliased(i)) {
        if (pstr(i) < 0) {
          sendf (i, "You've just died.\n");
          unalias (i);
        }
        else if (polymorphed(i) == 0)
  	  unalias(i);
        else if (polymorphed(i) > -1)
	 polymorphed(i)--;
      }
                                             /* if invis, count down */
      if (ivct(i) > 0 && --ivct(i) == 0)
        setpvis (i, 0);

#include "special.h"
  }
}

void regenerate (void)
{
  int i, obj, chance, mchance, ct;

  for (i = 0; i < max_players; ++i) {

    if (!is_in_game(i) || linkdead(i))
      continue;
    
    if ((ltstflg(ploc(i), LFL_NOREGEN) || ltstflg(ploc(i), LFL_NEGREGEN)) 
           && plev (i) < LVL_WIZARD) {
      sendf (i, "You cannot recuperate here.\n");
      continue;
    }
    else if (pclass(i) == PRIEST && plev(i) >= 4) {
      chance = 30;
      mchance = 45;
    }
    else {
      chance = 15;
      mchance = 22;
    }

    for (ct = 0 ; ct < pnumobs(i) ; ct++) {
      obj = pobj_nr(ct, i);
      if (otstbit (obj, OFL_REGENHEALTH))
        chance += 10;
      if (otstbit (obj, OFL_REGENMANA))
        mchance += 10;
    }

    chance += psitting (i) ? 20 : 0;
    mchance += psitting (i) ? 20 : 0;
    chance += ltstflg (ploc (i), LFL_FASTHEAL) ? 15 : 0;
    mchance += ltstflg (ploc (i), LFL_FASTMANA) ? 15 : 0;
    chance = (chance >= 100) ? 90 : chance;
    mchance = (mchance >= 100) ? 90 : mchance;

    if (is_in_game (i) && pfighting (i) < 0 && pstr (i) < maxstrength (i)) {
      if (randperc () < chance) {
        if (maxstrength (i) == pstr (i) + 1)
          sendf (i, "&+GYour strength has returned to you.\n");
        setpstr (i, pstr (i) + 1);
        if (players[i].iamon)
          calib_player (i);
      }
    }
    if (is_in_game (i) && (pclass(i) == MAGE || pclass(i) == PRIEST) && 
        pfighting (i) < 0 && pmagic (i) < maxmagic (i)) {
      if (randperc () < mchance) {
        if (maxmagic (i) == pmagic (i) + 1)
          sendf (i, "&+MYour mana has been replenished.\n");
        setpmagic (i, pmagic (i) + 1);
        if (players[i].iamon)
          calib_player (i);
      }
    }
  }
}

