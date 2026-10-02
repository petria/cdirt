#include <stdlib.h>
#include "kernel.h"
#include "macros.h"
#include "bprintf.h"

char *s;
char b[256];
char g[256];
char n[256];

/* get rid of this */

int num_levels[] =
{1, LVL_WIZARD, LVL_EMERITI, LVL_STDWIZ,
 LVL_ARCHWIZARD, LVL_DEMI, LVL_SHALAFI, LVL_GOD,
 LVL_CREATOR};


static Boolean or_ok ();

static Boolean ok (int lev, long int bits[], char *verb) {
  Boolean j;
  int k;
  char *t;

  while (*s == ' ' || *s == '\t')
    ++s;
  switch (*s++) {
  case '(':
    j = or_ok (lev, bits, verb);
    if (*s != ')') {
      mudlog("HELP: Missing ')' at %s.\n", s);
      bprintf("Help file unavailable\n");
      return(False); 
    }
    ++s;
    return j;
  case '!':
    return !ok (lev, bits, verb);
  case 'U':
    return (lev >= LVL_CREATOR);
  case 'G':
    return (lev >= LVL_GOD);
  case 'D':
    return (lev >= LVL_DEMI);
  case 'V':
    return (lev >= LVL_SHALAFI);
  case 'A':
    return (lev >= LVL_ARCHWIZARD);
  case 'R':
    return (lev >= LVL_ISTARI);
  case 'W':
    return (lev >= LVL_STDWIZ);
  case 'X':
    return (lev >= LVL_WIZARD);
  case 'L':
    if (*s == '-' || isdigit (*s)) {
      k = strtol (s, &t, 10);
      s = t;
    } else {
      for (t = n; isalpha (*s);)
	*t++ = *s++;
      *t = 0;
      if ((k = tlookup (n, MWizLevels)) >= 0 ||
	  (k = tlookup (n, FWizLevels)) >= 0)
	k = num_levels[k];
      else if ((k = tlookup (n, MageLevels)) >= 0 ||
               (k = tlookup (n, ThiefLevels)) >= 0 ||
               (k = tlookup (n, PriestLevels)) >= 0 ||
               (k = tlookup (n, WarriorLevels)) >= 0);
      else {
        bprintf("Help file unavailable.\n");
	mudlog("HELP: Unknown name of level: %s.\n", n);
        return(False);
      }
    }
    return (lev >= k);
  case 'P':
    for (t = n; isalpha (*s);)
      *t++ = *s++;
    *t = 0;
    if ((k = tlookup (n, Pflags)) < 0) {
      mudlog("HELP: Unknown name of Pflags: %s.\n", n);
      bprintf("Help file unavailable.\n");
      return(False);
    }
    return(ptst_flg(bits, k, PFLAGS));
  case 'M':
    for (t = n; isalpha (*s);)
      *t++ = *s++;
    *t = 0;
    if ((k = tlookup (n, Pflags)) < 0) {
      mudlog("HELP: Unknown name of Pflags: %s.\n", n);
      bprintf("Help file unavailable.\n");
      return(False);
    }
    return(ptst_flg(bits, k, PMASK));
  default:
    if (!isalpha (*s))
      return True;
    mudlog("HELP: Illegal code, %c at %s\n", s[-1], s - 1);
    bprintf("Help file unavailable.\n");
    return(False);
  }
}

static Boolean and_ok (int lev, long int bits[], char *verb) {
  Boolean j = True;

  do {
    if (!ok (lev, bits, verb))
      j = False;
    while (*s == ' ' || *s == '\t')
      ++s;
    if (*s == '&')
      while (*++s == ' ' || *s == '\t') ;
  }
  while (isalpha (*s) || *s == '!' || *s == '(');
  return(j);
}

static Boolean or_ok (int lev, long int bits[], char *verb) {
  Boolean j = False;

  while (True) {
    if (and_ok (lev, bits, verb))
      j = True;
    if (*s != '|')
      return(j);
    ++s;
  }
}

static Boolean all_ok (int lev, long int bits[], char *verb, char *t) {
  Boolean j;

  s = t;
  j = or_ok (lev, bits, verb);
  if (*s == 0)
    return(j);
  mudlog("HELP: Illegal syntax in string at %s.\n", s);
  bprintf("Help file unavailable.\n");
  return(False);
}

void helpcopy(char *dest, char *src) {
  char *p, *q;
 
  for (p = src, q = dest ; *p ; p++) {
    if (isspace(*p) || *p == '\n') {
      *q = 0;
      break;
    }
    else
      *q++ = *p;
  }
  *q = 0;
}

int filter (int lev, long int bits[], char *verb, FILE *F) {
  char buff[256];
  char *t, *p;
  Boolean pr_help = False;
  Boolean test = True;
  Boolean new = False;

  while (fgets (b, sizeof (b), F)) {
    if (test) {
      if (*b == '[' && (p = strchr(b, ']')))
        p++;
      else
        p = b;

      helpcopy(buff, p);

      if (EQ(buff, verb)) {
        pr_help = True;
        new = True;
      }
      else {
        if (pr_help)
          return(-1);
        pr_help = False;
      }
      test = False;
    }
    if (strchr(b, '^'))
      test = True;

    if (*b != '[' || (t = strchr (b, ']')) == NULL) {
      if (pr_help && *b != '^') {
        if (new) {
          bprintf("\n%s\n", b);
          new = False;
        }
        else
          bprintf("%s", b);
      }
      continue;
    }

    *t = 0;
    if (strlen(t+1) > 1 && *++t == '\\' && t[1] == '\n') {
      if (!fgets (g, sizeof (g), F))
	break;
      t = g;
    }
    if (all_ok (lev, bits, verb, b + 1) && pr_help && *t != '^')
      if (pr_help) {
        if (new) {
          bprintf("\n%s\n", t);
          new = False;
        }
        else
          bprintf("%s", t);
      }
  }
  return(0);
}

int help (int lev, long int bits[], char *verb) {
  FILE *F;
  int rslt;

  if ((F = FOPEN(DATA_DIR "/" HELP_DIR "/" FULLHELP, "r"))) {
    rslt = filter(lev, bits, verb, F);
    FCLOSE(F);
    return(rslt);
  }
  else
    bprintf("Error opening help file.\n");
  return(-1);
}
