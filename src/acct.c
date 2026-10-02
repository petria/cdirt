#include <strings.h>
#include "kernel.h"
#include "locations.h"
#include "objects.h"
#include "mobiles.h"
#include "bprintf.h"
#include "parse.h"
#include "acct.h"
#include "zones.h"
#include "mobile.h"
#include "objsys.h"
#include "rooms.h"
#include "fight.h"

#define Line1 "============================================================="
#define Line2 "-------------------------------------------------------------"

void
acctcom (void)
{
  char text[80];
  int zone = -1;
  int i;

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("You can't do that now.\n");
    return;
  }
  if (brkword () != -1) {
    if ((zone = get_zone_by_name (wordbuf)) == -1) {
      bprintf ("No Such Zone: %s\n", wordbuf);
      return;
    }
  }
  if (zone == -1)
    sprintf (text, "Current System Accounting");
  else
    sprintf (text, "Current System Accounting (Zone: %s)", zname (zone));

  for (i = 0; i < (60 - strlen (text)) / 2; ++i)
    bprintf (" ");

  bprintf ("&+W%s\n", text);
  bprintf ("&+C%s\n", Line1);

  countrooms (zone);
  bprintf ("\n");
  countmobiles (zone);
  bprintf ("\n");
  countobjects (zone);
}

/* Show percentage: max is maximum, exist is how many of max are left */
int
dopc (int max, int exist)
{
  int proc;
  int t;

  if (exist > max) {		/* Someone passed them backwards */
    t = max;			/* so we fix that here. */
    max = exist;
    exist = t;
  }
  if (!(max))
    proc = 0;
  else
    proc = (exist * 100) / max;

  return (proc);
}

void
countmobiles (int zone)
{
  int bas, j = 0, i;
  int a;
  int NoHassle, Possessed, DeadMobs;
  int proc;

  NoHassle = Possessed = DeadMobs = 0;
  a = max_players;

  if (zone == -1) {
    bas = numchars;
  } else {
    a = 0;
    bas = znumchars (zone);
  }

  for (; a < bas; a++) {
    i = (zone == -1) ? a : zmob_nr (a, zone);

    if (EMPTY (pname (i)))
      continue;

    if (ptstflg (i, PFL_NOHASSLE))
      NoHassle++;

    if (ststflg (i, SFL_OCCUPIED))
      Possessed++;

    if (alive (i) != -1)
      j++;

    if (zone == -1)
      DeadMobs = ((numchars - max_players) - j);
    else
      DeadMobs = (znumchars (zone) - j);
  }
  if (zone == -1)
    proc = dopc (numchars - max_players, j);
  else
    proc = dopc (znumchars (zone), j);

  if (zone == -1)
    bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Total Mobiles", numchars - max_players, "Living Mobiles", j);
  else
    bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Total Mobiles", znumchars (zone), "Living Mobiles", j);

  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n",
	   "Dead Mobiles", DeadMobs, "Percent Living", proc);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n",
	   "Possessed", Possessed, "NoHassle", NoHassle);
}

void countobjects (int zone)
{
  int proc;
  long totalval = 0;
  int aa, oc = 0;
  int destroyed, weapons, armor, food;
  int lightable, container, key, noget;
  int destweap, destarm, destfood, destlight, destcont, destkey;
  int objnum;

  destroyed = weapons = armor = food = lightable
    = container = key = noget = destweap = destarm = destfood =
    destlight = destcont = destkey = 0;

  if (zone == -1) {
    objnum = numobs;
  } 
  else {
    aa = zfirst_obj (zone);
    objnum = znumobs (zone);
  }

  for (aa = 0; aa < objnum; aa++) {
    if (zone != -1)
      oc = zobj_nr (aa, zone);

    if (!otstbit (oc, OFL_NOGET) && !otstbit (oc, OFL_DESTROYED))
      totalval += ovalue (oc);

    if (otstbit (oc, OFL_NOGET))
      noget++;

    if (otstbit (oc, OFL_DESTROYED))
      destroyed++;

    if (otstbit (oc, OFL_WEAPON)) {
      weapons++;
      if (otstbit (oc, OFL_DESTROYED))
	destweap++;
    }
    if (otstbit (oc, OFL_ARMOR)) {
      armor++;
      if (otstbit (oc, OFL_DESTROYED))
	destarm++;
    }
    if (otstbit (oc, OFL_FOOD)) {
      food++;
      if (otstbit (oc, OFL_DESTROYED))
	destfood++;
    }
    if (otstbit (oc, OFL_CONTAINER) &&
	!otstbit (oc, OFL_NOGET)) {
      container++;
      if (otstbit (oc, OFL_DESTROYED))
	destcont++;
    }
    if (otstbit (oc, OFL_KEY)) {
      key++;
      if (otstbit (oc, OFL_DESTROYED))
	destkey++;
    }
    if (otstbit (oc, OFL_LIGHTABLE)) {
      lightable++;
      if (otstbit (oc, OFL_DESTROYED))
	destlight++;
    }
    if (zone == -1)
      oc++;
  }

  if (zone == -1)
    proc = dopc (oc - destroyed, oc);
  else
    proc = dopc (znumobs (zone) - destroyed, znumobs (zone));

  if (zone == -1)
    bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Total Objects", oc, "Remaining", oc - destroyed);
  else
    bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Total Objects", znumobs (zone), "Remaining", znumobs (zone) - destroyed);

  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Destroyed", destroyed, "Percent Left", proc);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n\n", "NoGet", noget, "Value of Objects", totalval);
  bprintf ("&+W         Detail List of Objects Available to Mortals\n");
  bprintf ("&+C%s\n", Line2);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Weapons", weapons, "Destroyed", destweap);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Armor", armor, "Destroyed", destarm);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Food", food, "Destroyed", destfood);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Containers", container, "Destroyed", destcont);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Lightable", lightable, "Destroyed", destlight);
  bprintf ("&+w%-18s: %8d\t%-18s: %8d\n", "Keys", key, "Destroyed", destkey);
  bprintf ("&+C%s\n", Line1);
}

void
countrooms (int zone)
{
  int roomct;

  if (zone == -1)
    roomct = num_const_locs;
  else
    roomct = znumloc (zone);

  bprintf ("&+w%-18s: %8d\n", "Total Rooms", roomct);
}

void objectscom (void)
{
  int zone = -1;
  int i, c, ct, oc = 0;
  char b[60], d[60];

  if (plev (mynum) < LVL_WIZARD) {
    bprintf ("You can't do that now.\n");
    return;
  }
  if (brkword () != -1) {
    if ((zone = get_zone_by_name (wordbuf)) == -1) {
      bprintf ("No Such Zone: %s\n", wordbuf);
      return;
    }
  }
  if (zone == -1) {
    for (i = 0; i < numobs; ++i) {
      oc++;
      c = findzone (oloc (i), b);
      sprintf (d, "%s%d", b, c);

      if (ocarrf (i) >= CARRIED_BY)
	strcpy (d, "Carried");
      else if (ocarrf (i) == IN_CONTAINER)
	strcpy (d, "In Container");

      bprintf ("&+w%-12s&+W%-13s", oname (i), d);
      if (oc % 3 == 0)
	bprintf ("\n");
    }
    if (oc % 3 != 0)
      bprintf ("\n");

    bprintf ("\n&+wTotal of &+W%d &+wobjects.\n", oc);
    return;
  }
  bprintf ("&+wObject List For Zone: &+W%s\n", zname (zone));

  for (ct = 0 ; ct < znumobs(zone) ; ct++) {
    i = zobj_nr(ct, zone);
    oc++;
    c = findzone (oloc (i), b);
    sprintf (d, "%s%d", b, c);

    if (ocarrf (i) >= CARRIED_BY)
      strcpy (d, "Carried");
    else if (ocarrf (i) == IN_CONTAINER)
      strcpy (d, "In Container");

    bprintf ("&+w%-12s&+W%-14s", oname (i), d);
    if (oc % 3 == 0)
      bprintf ("\n");
  }
  if (oc % 3 != 0)
    bprintf ("\n");

  bprintf ("\n&+wTotal of &+W%d &+wobjects.\n", oc);
}

void
mstatcom ()
{
  int i, j = 0;
  char cond[50];

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  bprintf ("&+CLvl Name          Score  Str/Max Mag/Max DP AC Cnd Who/Where\n");
  bprintf ("&+b--------------------------------------------------------------------------------\n");
  for (i = 0; i < max_players; ++i) {
    if (is_in_game (i) && plev (i) < LVL_WIZARD) {
      ++j;

      if (pfighting (i) >= 0)
	sprintf (cond, "&+RFgt &+w%s: %d (%s)", pname (pfighting (i)), pstr (pfighting (i)), showname (ploc (i)));
      else if (phelping (i) >= 0)
	sprintf (cond, "&+BHlp &+w%s (%s)", pname (phelping (i)), showname (ploc (i)));
      else if (psitting (i))
	sprintf (cond, "&+CSit &+w(%s)", showname (ploc (i)));
      else
	sprintf (cond, "&+wStd &+w(%s)", showname (ploc (i)));

      bprintf ("&+w%3d &+W%-13.13s &+c%-6d %s%3d&+C/&+W%-3d %s%3d&+C/&+W%-3d &+B%2d %2d &+w%-32.32s\n",
	       plev (i), pname (i), pscore (i),
	       str_color (i), pstr (i), maxstrength (i),
	       mag_color (i), pmagic (i), maxmagic (i), player_damage (i),
	       pac (i), cond);
    }
  }
  if (!j)
    bprintf ("&+W No Mortals On-Line\n");
  bprintf ("&+b--------------------------------------------------------------------------------\n");
}
