#include <stdlib.h>
#include "kernel.h"
#include "zones.h"
#include "levels.h"
#include "mobile.h"
#include "rooms.h"
#include "objsys.h"
#include "clone.h"
#include "bprintf.h"
#include "log.h"
#include "parse.h"
#include "uaf.h"
#include "locations.h"

/* returns the number of players in a zone, excludes aliased mobs */

void resize_zones(void);

int count_players_in_zone(int zone) {
  int i;
  int plrs = 0;

  for (i = 0 ; i < max_players ; i++)
    if (is_in_game(i) && loc2zone(ploc(i)) == zone)
      plrs++;
  return(plrs);
}

/* give this two numbers, LOCMIN & LOCMAX */

int count_players_between_locs(int loca, int locb) {
  int i;
  int plrs = 0;

    for (i = 0 ; i < max_players ; i++)
      if (is_in_game(i) && ploc(i) <= loca && ploc(i) >= locb)
        plrs++;
    return(plrs);
}

/* Return the zone index for a given zone name (abbreviation).
 * -1 if no such zone is found.
 */
int
get_zone_by_name (char *name)
{
  int x;
  int w = strlen (name);

  for (x = 0; x < numzon; x++) {
    if (strncasecmp (name, zname (x), w) == 0)
      return x;
  }

  return -1;
}

/* Return the zone index among the non-permanent zones which matches
 * the given name exactly. If none exists, create one and return that index.
 */

int get_wizzone_by_name(char *zonename){
  int i;

  for (i = num_const_zon; i < numzon; i++) {    /* already here */
    if (!strcasecmp(zonename, zname (i)))
      return(i);
  }

  resize_zones();                               /* new wizzone */

  zname(numzon) = COPY(zonename);
  init_intset (zlocs (numzon), 5);
  init_intset (zmobs (numzon), 3);
  init_intset (zobjs (numzon), 3);

  return numzon++;
}

/* Return the ZONE entry index for the zone that 'loc' is member of.
 * Return -1 if invalid loc number.
 */
int
loc2zone (int loc)
{
  return exists (loc) ? lzone (loc) : -1;
}

/* Take a 'loc' as argument and return the zone name and the offset within
 * the zone for this loc.
 * If buff = NULL, return the name in a static buffer.
 */
int
findzone (int loc, char *buff)
{
  int z, x;

  if ((z = loc2zone (loc)) == -1) {
    strcpy (buff, "TCHAN");
    return 0;
  }
  strcpy (buff, zname (z));

  if (!(x = find_int (loc, zlocs (z)))) {
    mudlog ("ERROR: Location %d was not in its zone %d.", loc, z);
  }
  return x;
}

/* This function is inverse of findzone in that it
 * from a zone index and offset number finds the loc that it makes up.
 * 0 is returned on error.
 */
int getlocid (int z, int off) {
  int a;

  if (z == -1 || z >= numzon)
    return 0;

  if (off == 0)
    off = 1;
  else if (off < 0)
    return 0;

  return (a = find_int_number (off - 1, zlocs (z))) == SET_END ? 0 : a; 
}

void reset_zone (int z) {
  int i, ct, loc;

    /* reset data for each room/obj/mob, if it moves from the intset,   *
     * then decrement ct so that when ct++ happens we are at the proper *
     * place in the link list (where the old one was deleted            */

    for (ct = 0 ; ct < znumchars(z) ; ct++) {
      i = zmob_nr(ct, z);
      loc = pzone(i);
      reset_mobile(i);
      if (loc != pzone(i)) ct--;
    }
 
    for (ct = 0 ; ct < znumobs(z) ; ct++) {
      i = zobj_nr(ct, z); 
      loc = ozone(i);
      reset_object(i);
      if (loc != ozone(i)) ct--;
    }

    for (ct = 0 ; ct < znumloc(z) ; ct++) {
      i = zloc_nr(ct, z);
      loc = lzone(i);
      reset_location(i);
      if (loc != lzone(i)) ct--;
    }
}


void zonescom (void) {
  int a;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  bprintf ("&+C     Zone-Name Rooms  Mobiles  Objects  "
	   "Zone-Name  Rooms  Mobiles  Objects\n\n");

  for (a = 0; a < num_const_zon; a++) {
    bprintf ("&*%14s&+W%5d %7d %7d", zname (a), znumloc (a), znumchars (a), znumobs (a));

    if (a % 2 == 1 || a == num_const_zon - 1)
      bprintf ("\n");
  }

  bprintf ("\n&*A total of &+W%d &*permanent zones containing "
	   "&+W%d &*rooms, &+W%d &*mobiles, &+W%d &*objects.\n\n",
	   num_const_zon, num_const_locs, num_const_chars - max_players,
	   num_const_obs);

  if (numzon != num_const_zon) {
    int i = 0;

    for (a = num_const_zon; a < numzon; a++) {
      int locs = znumloc (a);
      int mobs = znumchars (a);
      int objs = znumobs (a);

      if (locs > 0 || mobs > 0 || objs > 0) {
        if (!i)
          bprintf ("&+C     Zone-Name Rooms  Mobiles  Objects  "
	     "Zone-Name  Rooms  Mobiles  Objects\n\n");

	bprintf ("&*%14s&+W%5d %7d %7d", zname (a), locs, mobs, objs);

	if (i++ % 2 == 1)
	  bprintf ("\n");
      }
    }

    if (i) 
      bprintf ("\n&*A total of &+W%d &*Wizard's zones containing "
	       "&+W%d &*rooms, &+W%d &*mobiles, &+W%d &*objects.\n",
	       i,
	       numloc - num_const_locs,
	       numchars - num_const_chars,
	       numobs - num_const_obs);
  }
}

/* The LOCATIONS command
 */
void
locationscom ()
{
  int a, b;

  b = 0;

  if (plev (mynum) < LVL_WIZARD) {
    erreval ();
    return;
  }
  bprintf ("&+CZone           Locations  Zone           Locations  Zone           Locations\n");
  bprintf ("&+c------------------------  ------------------------  ------------------------\n");

  for (a = 0; a < num_const_zon; a++) {
    ++b;
    bprintf ("&*%-15s&+W%9d", zname (a), znumloc (a));
    if (b % 3 == 0 || a == num_const_zon - 1) {
      b = 0;
      bprintf ("\n");
    } else
      bprintf ("  ");
  }

  if (b)
    bprintf ("\n");
  bprintf ("&+c------------------------  ------------------------  ------------------------\n");
  bprintf ("&+wA total of &+W%d &+wpermanent zones, with &+W%d &+wlocations.\n",
	   num_const_zon, num_const_locs);

  if (numzon != num_const_zon) {
    int i = 0;

    b = 0;
    bprintf ("&+c------------------------  ------------------------  ------------------------\n");
    bprintf ("&+CZone           Locations  Zone           Locations  Zone           Locations\n");
    bprintf ("&+c------------------------  ------------------------  ------------------------\n");

    for (a = num_const_zon; a < numzon; a++) {
      int locs = znumloc (a);
      int mobs = znumchars (a);
      int objs = znumobs (a);

      if (locs > 0 || mobs > 0 || objs > 0) {
	bprintf ("&*%-15s&+W%9d", zname (a), znumloc (a));
	++b;
	++i;
	if (b % 3 == 0 || a == num_const_zon - 1) {
	  b = 0;
	  bprintf ("\n");
	} else
	  bprintf ("  ");
      }
    }

    if (b)
      bprintf ("\n");
    bprintf ("&+c------------------------  ------------------------  ------------------------\n");
    bprintf ("&+wA total of &+W%d &+wWizard zones, with &+W%d &+wlocations.\n",
	     i, numloc - num_const_locs);
  }
}
