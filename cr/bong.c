/* Some sort of drug reference somebody wrote... interesting   */
/* from a coding perspective I suppose. GC                     */

#include "kernel.h"
#include "sendsys.h"
#include "bprintf.h"
#include "mobile.h"
#include <stdlib.h>
#include "mud.h"

void initbong(int *hits, int *fire, int *last, int *loc, 
              Boolean *avail, int *num) {
  int i;

  *hits = 10;
  *fire = 0;
  *last = -1;
  *loc = mynum;
  *avail = False;


  for (i = 0 ; i < lnumchars(ploc(mynum)) ; i++)
    if (lmob_nr(i, ploc(mynum)) == mynum)
      *num = i;
}

void bongcom() {
  static Boolean avail = True;
  static int loc, hits, fire, last, num;

  if (!strcmp(txt1, "pack")) {
    if (!avail) {
      bprintf("The virtual bong is being used currently.\n");
      return;
    }
    else {
      initbong(&hits, &fire, &last, &loc, &avail, &num);
      bprintf("You summon the virtual bong, and pack it with Dank!\n");
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
               "%s summons a magical virtual bong!\n", pname(mynum));
    }
  }
  else if (!strcmp(txt1, "fire")) {
    if (fire)
      bprintf("It's already lit, dumbass.\n");
    else if (!hits)
      bprintf("You'd probably want to pack it first.\n");
    else {
      fire = 10;
       if (loc != mynum) {
         bprintf("You light the bong for %s.\n", pname(loc));
         sendf(loc, "%s lights up the bong for you. How nice!\n", pname(mynum));
         send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, loc,
               "%s lights the bong for %s.\n", pname(mynum), pname(loc));
       }
       else {
         bprintf("You fire up the bong!\n");
         send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "With an adept flick of a zippo lighter, %s fires up the bong!\n",
           pname(mynum));
       }
    }
  }
  else if (!strcmp(txt1, "hit")) {
    if (loc != mynum)
      bprintf("You dont have it, HUMPHRY!\n");
    else if (!hits)
      bprintf("You're puffin on ashes, buddy.\n");
    else if (!fire)
      bprintf("You'll need to light it first.\n");
    else if (last == mynum) {
     send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
       "%s bogarts the bong! FATAL mistake, buddy!\n", pname(mynum));
     quit_msg("You BOGART!", "&+WBogarted Dank");
     quit_player(False);
    }
    else {
      switch(rand() % 3) {
        case 0:
          bprintf("You take a HUGE hit, then exhale.\n");
          send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "%s takes a really huge hit!\n", pname(mynum));
          break;
        case 1:
          bprintf("You take a weak little puff.\n");
          send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "%s takes a weak little hit.\n", pname(mynum));
          break;
        case 2:
          bprintf("You take a big hit, but cough your brains out!\n");
          send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "%s takes a hit and coughs up a lung!\n", pname(mynum));
          break;
      }
      hits--;
      last = mynum;
    }
  }
  else if (!strcmp(txt1, "pass")) {
    if (loc != mynum)
      bprintf("Not having the bong, you decide to pass gas instead.\n");
    else {
      if (num >= lnumchars(ploc(mynum)) - 1)
        num = 0;
      else
        num++;
      loc = lmob_nr(num, ploc(mynum));
      sendf(loc, "%s passes the bong to you.\n", pname(mynum));
      bprintf("You happily pass the bong to %s.\n", pname(loc));
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, loc,
        "%s passes the bong to %s! WHOO HOO!\n", pname(mynum), pname(loc));
    }
  }
  else if (!strcmp(txt1, "cash")) {
    bprintf("You tap out the charred contents of the virtual bong.\n");
    if (hits) {
      bprintf("Whoops! Looks like it wasn't cashed!\n");
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
        "%s carelessly dumps out burning embers from the bong.\n",pname(mynum));
    }
    else
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
           "%s cashes out the bong.\n", pname(mynum));
    avail = True;
  }
  else
    bprintf("Invalid BONG command.  See HELP BONG.\n");
}

