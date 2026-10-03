#include <unistd.h>
#include <stdlib.h>
#include "kernel.h"
#include "mobile.h"
#include "sendsys.h"
#include "objsys.h"
#include "zones.h"
#include "parse.h"
#include "fight.h"
#include "exits.h"
#include "clone.h"
#include "commands.h"
#include "rooms.h"
#include "bprintf.h"
#include "uaf.h"
#include "log.h"

extern HASH_TABLE oalts_n[];
extern HASH_TABLE objects_n[];
extern HASH_TABLE objects_z[];
extern HASH_TABLE ublock_n[];
extern HASH_TABLE ublock_z[];
extern HASH_TABLE ualts_n[];
extern HASH_TABLE locations_z[];

extern int room_search(int, int);
extern int mob_search(int, int);
extern int obj_search(int, int);
extern void resize_objects(void);
extern void resize_ublock(void);
extern void resize_locs(void);
extern void load_zone(char *, Boolean);

int clone_object (int obj, int new_zone, char *new_name, int loc, int carrf) {
  int i, newobj;
  char buff[MAX_COM_LEN];

  resize_objects();

  newobj = numobs++;

  memcpy(&objs[newobj], &objs[obj], sizeof(OBJECT));

  if (new_name != NULL)
    oname(newobj) = COPY(new_name);
  else
    oname(newobj) = COPY(oname(obj));

  ht_add(objects_n, oname(newobj), newobj);

  if (oaltname(obj) != NULL) {
    oaltname(newobj) = COPY(oaltname(obj));
    ht_add(oalts_n, oaltname(newobj), newobj);
  }

  strcpy(buff, ozname(obj));
  if (ht_lookup(objects_z, ozname(obj), 0, new_zone, obj_search) != -1)
    for (i = 1 ; ; i++) {
      sprintf(buff + strlen(ozname(obj)), "%d", i);
      if (ht_lookup(objects_z, buff, 0, new_zone, obj_search) == -1)
        break;
    }

  ozname(newobj) = COPY(buff);
  ht_add(objects_z, ozname(newobj), newobj);

  for (i = 0; i < 4; i++)
    if (olongt(obj, i) != NULL)
      olongt(newobj, i) = COPY(olongt(obj, i));

  if (oexam_text(obj) != NULL)
    oexam_text(newobj) = COPY(oexam_text(obj));

  oclrbit (newobj, OFL_DESTROYED);

  init_intset(oinv(newobj), otstbit(newobj, OFL_CONTAINER) ? 15 : 0);

  oloc_reset(newobj) = loc;
  ocarrf_reset(newobj) = IN_ROOM;
  oloc(newobj) = numobs + 1; /* set to illegal loc, so setoloc doesnt remove */
  setoloc(newobj, loc, carrf);

  if (new_zone > -1) {
    ozone(newobj) = new_zone;
    zadd_obj(newobj, new_zone);
    zmaxobjs(new_zone)++;
  }
  else {
    zadd_obj(newobj, ozone (obj));
    zmaxobjs(ozone(obj))++;
  }

  return(newobj);
}

Boolean destruct_object (int obj) {
  int i, ct, plr, new_loc, new_carrf;
  int from, to;

  if (opermanent(obj) || obj >= numobs)
    return(False);

  if ((plr = oloc(obj)) < max_players && plr > -1) {
    if (iswornby(obj, plr))
      removeobj(False, obj, plr);
    else if (iswieldby(obj, plr))
      setpwpn(plr, -1);
  }

  /* if container, empty it's contents : */

  if (!is_empty (oinv (obj))) {
    switch (ocarrf (obj)) {
    case IN_CONTAINER:
    case IN_ROOM:
      new_loc = oloc(obj);
      new_carrf = ocarrf(obj);
      break;
    case CARRIED_BY:
    case WORN_BY:
    case WIELDED_BY:
    case BOTH_BY:
      new_loc = ploc (oloc (obj));
      new_carrf = IN_ROOM;
      break;
    default:                        /* makes the compiler happy */
      new_loc = new_carrf = -1;
    }
    for (ct = 0 ; ct < onumobs(obj) ; ct++) {
      i = oobj_nr(ct, obj);
      if (new_loc != oloc(i))
        ct--;
      setoloc(i, new_loc, new_carrf);
    }
  }

  /* remove the object from the world : */

  setoloc (obj, -1, IN_ROOM);
  ht_remove(objects_n, oname(obj), obj);
  ht_remove(objects_z, ozname(obj), obj);
  if (oaltname(obj))
    ht_remove(oalts_n, oaltname(obj), obj);

  /* free memory to avoid memory leak : */

  free_intset(oinv(obj));

  FREE(oname(obj));
  FREE(ozname(obj));

  if (oaltname(obj)) {
    ht_remove(oalts_n, oaltname(obj), obj);
    FREE(oaltname(obj));
  }
  if (oexam_text(obj))
    FREE(oexam_text(obj));

  for (i = 0; i < 4; i++)
    if (olongt(obj, i))
      FREE(olongt(obj, i));

  /* if this was one side of a door, shut the other side */

  if (olinked (obj) > -1) {
    olinked (olinked (obj)) = -1;
    state (olinked (obj)) = EX_CLOSED;
  }

  zmaxobjs(ozone(obj))--;
  zremove_obj(obj, ozone(obj));

  /* move down last element in objects array : */

  from = numobs - 1;
  to = obj;

  if (to != from) {
    zremove_obj(from, ozone(from));
    zadd_obj(to, ozone(from));

    if (olinked(from) != -1)
      olinked(olinked(from)) = to;

    for (ct = 0 ; ct < onumobs(from) ; ct++) {
      i = oobj_nr(ct, from);
      if (to != ploc(i))
        ct--;
      setoloc(i, to, ocarrf(i));
    }

    /* flippy flop : */
  
    ht_remove(objects_n, oname(from), from);
    ht_remove(objects_z, ozname(from), from);
    ht_add(objects_n, oname(from), to);
    ht_add(objects_z, ozname(from), to);
    if (oaltname(from)) {
      ht_remove(oalts_n, oaltname(from), from);
      ht_add(oalts_n, oaltname(from), to);
    }

    new_loc = oloc(from);
    setoloc(from, -1, ocarrf(from)); 
    memcpy(&objs[to], &objs[from], sizeof(OBJECT));
    setoloc(to, new_loc, ocarrf(to));
  }

  numobs--;
  memset(&objs[numobs], 0, sizeof(OBJECT));
  return(True);
}
 
int clone_mobile (int mob, int new_zone, char *new_name) {
  char buff[MAX_COM_LEN];
  int i;

  resize_ublock();

  memcpy(&ublock[numchars], &ublock[mob], sizeof(UBLOCK_REC));

  if (new_name != NULL) {
    setpname(numchars, new_name);
    pname_reset(numchars) = COPY(new_name);
  } 
  else {
    setpname(numchars, pname(mob));
    pname_reset(numchars) = COPY(pname_reset(mob));
  }

  if (pexam(mob) != NULL)
    pexam(numchars) = COPY(pexam(mob));
  if (pftxt (mob) != NULL) {
    char b[128];
    pftxt(numchars) = COPY (new_name == NULL ? pftxt (mob)
	: (sprintf (b, "%s is here.", new_name), b));
  }

  init_intset(pinv(numchars), 4);

  if (new_zone > -1) {
    pzone(numchars) = new_zone;
    zadd_mob(numchars, new_zone);
    zmaxmobs(new_zone)++;
  }
  else {
    zadd_mob(numchars, pzone (mob));
    zmaxmobs(pzone(mob))++;
  }

  /* ensures a new unique zone name : */

  strcpy(buff, pzname(mob));

  if (ht_lookup(ublock_z, pzname(mob), 0, new_zone, mob_search) != -1)
    for (i = 1 ; ; i++) {
      sprintf(buff + strlen(pzname(mob)), "%d", i);
      if (ht_lookup(ublock_z, buff, 0, new_zone, mob_search) == -1)
	break;
    }

  pzname(numchars) = COPY(buff);
      
  ht_add(ublock_z, pzname(numchars), numchars);
  ht_add(ublock_n, pname(numchars), numchars);

  return(numchars++);
}

Boolean destruct_mobile (int mob) {
  int i, ct, from, to, new_loc;

  if (ppermanent (mob) || mob >= numchars)
    return False;

  setpfighting (mob, -1);

  /* If someone is aliasing it, lose them. */

  if (ststflg(mob, SFL_OCCUPIED))
    unalias(mob);

  /* If it carries something, drop it */

  for (ct = 0 ; ct < pnumobs(mob) ; ct++) {
    i = pobj_nr(ct, mob);
    setoloc (i, ploc (mob), IN_ROOM);
  } 

  /* Removes mobile from the game : */

  setploc(mob, -1);
  zmaxmobs(pzone(mob))--;
  zremove_mob(mob, pzone(mob));
  setpstr(mob, -1);
  ht_remove(ublock_n, pname(mob), mob);
  ht_remove(ublock_z, pzname(mob), mob);

  /* free memory to avoid leak : */

  FREE(pzname(mob));
  free_intset(pinv(mob));

  to = mob;
  from = numchars - 1;

  /* Moves down last element in mobile array : */

  if (to != from) {
    ht_remove(ublock_n, pname(from), from);
    ht_remove(ublock_z, pzname(from), from);
    ht_add(ublock_n, pname(from), to);
    ht_add(ublock_z, pzname(from), to);

    zremove_mob (from, pzone(from));
    zadd_mob (to, pzone(from));

    /* Change the references of its inventory to its new number */

    for (ct = 0 ; ct < pnumobs(from) ; ct++) {
      i = pobj_nr(ct, from);
      if (oloc(i) != from)
        ct--;
      setoloc(i, to, ocarrf(i));
    }

    /* Change the references to it for its fight opponents & helpers */

    for (i = 0; i < lnumchars (ploc(from)); i++) {
      if (phelping(lmob_nr (i, ploc(from))) == from)
        setphelping(lmob_nr (i, ploc(from)), from);
      if (pfighting(lmob_nr (i, ploc(from))) == from)
        setpfighting(lmob_nr (i, ploc(from)), from);
    }

    new_loc = ploc(from);
    setploc (from, -1);
    memcpy(&ublock[to], &ublock[from], sizeof(UBLOCK_REC));
    setploc (to, new_loc);
  } 

  memset(&ublock[from], 0, sizeof(UBLOCK_REC));
  numchars--;

  return(True);
}


int clone_location (int l, int new_zone, char *new_name) {
  int i;
  char buff[MAX_COM_LEN];
  int c_numloc = numloc;

  resize_locs();

  memcpy(&room_data[numloc], &room_data[l], sizeof(Location));

  for (i = 0; i < NEXITS; i++)
    setexit (c_numloc, i, lexit (c_numloc, i));

  lshort (c_numloc) = new_name != NULL ? COPY (new_name)
    : lshort (l) != NULL ? COPY (lshort (l)) : NULL;

  if (llong (l) != NULL)
    llong (c_numloc) = COPY (llong (l));

  init_intset (linv (c_numloc), 7);
  init_intset (lmobs (c_numloc), 5);
  init_intset (lexits_to_me (c_numloc), 4);

  if (new_zone > -1) {
    zadd_loc (c_numloc, new_zone);
    lzone(c_numloc) = new_zone;
    zmaxlocs(new_zone)++;
  }
  else  {
    zadd_loc (c_numloc, lzone (l));
    zmaxlocs(lzone(l))++;
  }
  ++numloc;

  strcpy(buff, lname(l));

  if (ht_lookup(locations_z, lname(l), 0, new_zone, room_search) != -1) {
    for (i = 1 ; ; i++) {
      sprintf(buff + strlen(lname(l)), "%d", i);
      if (ht_lookup(locations_z, buff, 0, new_zone, room_search) == -1)
        break;
    }
  }

  lname(c_numloc) = COPY(buff);
  ht_add(locations_z, lname(c_numloc), c_numloc);
  return(c_numloc);
}

Boolean destruct_location (int l) {
  int i, j, ct, from, to;

  if (lpermanent (l))
    return(False);

  /* If it contains players, don't destruct. */
  for (i = 0; i < lnumchars(l); i++)
    if (lmob_nr(i, l) < max_players)
      return(False);

  /* If it contains mobiles, destroy them. */

  while (lnumchars(l) > 0)
    destroy_mobile(lfirst_mob(l));

  /* If it contains objects, destroy them. */

  while (lnumobs(l) > 0)
    destroy(lfirst_obj(l));

  for (i = 0; i < NEXITS; i++)
    setexit (l, i, 0);

  for (ct = 0; ct < set_size(lexits_to_me(l)); ct++) {
    i = int_number(ct, lexits_to_me(l));

    for (j = 0; j < NEXITS; j++)
      if (lexit (i, j) == l)
	setexit (i, j, 0);
  }

  free_intset (linv (l));
  free_intset (lmobs (l));
  free_intset (lexits_to_me (l));

  FREE(lname(l));
  if (lshort(l))
    FREE(lshort(l));
  if (llong(l))
    FREE(llong(l));

  ht_remove(locations_z, lname(l), l);
  zremove_loc(l, lzone(l));
  zmaxlocs(lzone(l))--;

  /* flippy flop : */

  from = numloc - 1;
  to = l;

  if (to != from) {
    zremove_loc(from, lzone(from));
    zadd_loc(to, lzone(from));

    ht_remove(locations_z, lname(from), from);
    ht_add(locations_z, lname(from), to);

    for (ct = 0 ; ct < lnumobs(from) ; ct++) {
      i = lobj_nr(ct, from);
      if (from != ploc(i))
        ct--;
      setoloc (i, to, ocarrf(i));
    }

    for (ct = 0 ; ct < lnumchars(from) ; ct++) {
      i = lmob_nr(ct, from);
      if (from != ploc(i))
        ct--;
      setploc (i, to);
    }
    memcpy(&room_data[to], &room_data[from], sizeof(Location));
  }

  numloc--;
  memset(&room_data[numloc], 0, sizeof(Location));
  return(True);
}

void clonecom (void) {
  int id, a, zone;
  char name[MAX_COM_LEN], loc_name[MAX_COM_LEN], new_name[MAX_COM_LEN];
  char *p, *q;
  PERSONA P, P2;

  if (!ptstflg (mynum, PFL_CLONE)) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    bprintf ("Clone what?\n");
    return;
  }
  strcpy (name, wordbuf);

  if (EMPTY (p = getreinput (new_name)))
    p = NULL;
  else {
    for (q = p; *q ; q++) {
      if (*q == '^' || *q == '\"') {
        bprintf("You can't use a ^ or \" in the new name.\n");
	return;
      }
      else if (isdigit(*q)) {
        bprintf("You can't use any numbers in the new name.\n");
        return;
      }
    }
    /* Remove trailing spaces: */
    while (--q > p && *q == ' ');
    *++q = 0;
  }
  zone = get_wizzone_by_name(pname(mynum));

  strcpy(loc_name, name);
  if (find_loc_by_name_ex(loc_name, &a)) {
    id = clone_location(a, zone, p);

    bprintf("[%s@%s]\n", lname(id), showname(id));
    mudlog("CLONE: Location: %s cloned %s to %s", pname(mynum), p, lname(id));
  } 
  else if ((a = fmbn(name)) != -1) {
    if ((p == NULL && getuaf(pname(a), &P)) || (p != NULL && getuaf(p, &P2)))
      bprintf("There is a player with that name!\n");
    else if (p != NULL && strlen(p) > MNAME_LEN)
      bprintf("Mobile name %s too long (max = %d chars.)\n", p, MNAME_LEN);
    else if (strstr (pname (a), "Puff") != NULL)
      bprintf("Sorry, we don't need more fractal dragons running around!\n");
    else if (!EQ(zname(pzone(mynum)), pname(mynum)))
      bprintf("You must clone mobiles in your own zone.\n");
    else {
      id = clone_mobile(a, zone, p);

      ploc_reset(id) = ploc(mynum);
      ploc(id) = 0;                 /* makes setploc work */
      setploc(id, ploc(mynum));
      bprintf("%s (%d) appears before you!\n", pname (id), id);
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
	       "%s appears before you!\n", pname (id));
      mudlog ("CLONE: Mobile: %s has cloned %s to %s", pname (mynum), 
	      pname(a), pname(id));
    }
  } 
  else if ((a = fobn(name)) != -1) {
    if (p != NULL && strlen(p) > ONAME_LEN)
      bprintf("Object name %s too long (max = %d chars.)\n", p, ONAME_LEN);
    else if (olinked(a) > -1)
      bprintf("That's a door-type object and can't be cloned (yet)\n");
    else if (!EQ(zname(pzone(mynum)), pname(mynum)))
      bprintf("You must clone objects in your own zone.\n");
    else {
      id = clone_object(a, zone, p, ploc(mynum), IN_ROOM);

      bprintf("The %s (%d) is created before you!\n", oname(id), id);
      send_msg(sendloc(mynum), 0, LVL_MIN, LVL_MAX, mynum, NOBODY,
		"The %s is created before you!\n", oname(id));
      mudlog("CLONE: Object: %s cloned %s to %s", pname(mynum), 
	      oname(a), oname(id));
    } 
  }
  else
    bprintf ("I don't know any %s.\n", name);
}


void destructcom (char *args) {
  int a;

  if (!ptstflg (mynum, PFL_CLONE)) {
    erreval ();
    return;
  }
  if (args == NULL && (args = wordbuf, brkword () == -1)) {
    bprintf ("Destruct what ?\n");
    return;
  }
  if ((a = find_loc_by_name (args)) != -1) {
    if (-a < num_const_locs)
      bprintf ("That's a permanent location.\n");
    else if (!destruct_location (a))
      bprintf ("There are players in that location.\n");
    else {
      bprintf ("Ok.\n");
      mudlog ("DESTRUCT: Location: %s destructed %s", pname (mynum), showname (a));
    }
  } 
  else if ((a = fmbn (args)) != -1 && a >= max_players) {
    if (a < num_const_chars) {
      bprintf ("That's a permanent mobile.\n");
      return;
    }
    send_msg(sendloc(a), 0, pvis (a), LVL_MAX, NOBODY, NOBODY,
	      "%s crumbles to dust.\n", pname (a));

    if (!is_empty (pinv (a)))
      send_msg(sendloc(a), 0, pvis (a), LVL_MAX, NOBODY, NOBODY,
		"%s belongings drop to the ground.\n", 
                ststflg(a, SFL_FEMALE) ? "Her" : "His");
    mudlog ("DESTRUCT: Mobile: %s destructed %s", pname (mynum), pname (a));
    destruct_mobile (a);
  } 
  else if ((a = fobn (args)) != -1) {
    if (a < num_const_obs) {
      bprintf ("That's a permanent object.\n");
      return;
    }
    send_msg(int2idx(obj_loc(a), LOC), 0, ovis (a), LVL_MAX, NOBODY, NOBODY,
	      "The %s crumbles to dust.\n", oname (a));

    if (!is_empty (oinv (a)) && ocarrf (a) >= CARRIED_BY)
      send_msg(int2idx(obj_loc(a), LOC), 0, ovis (a), LVL_MAX, NOBODY, NOBODY,
		"Its contents drops to the ground.\n");
    mudlog ("DESTRUCT: Object: %s destructed %s", pname (mynum), oname (a));
    destruct_object (a);
  } 
  else if (a == 1)
    destructcom (strcat (args, "1"));
  else
    bprintf ("I don't know any %s\n", args);
}


void storecom (char *player, Boolean silent) {
  FILE *f;
  char path[256], loc[256];
  int i, ct, z, j, q, nz;
  
  sprintf(path, "%s/WIZ_ZONES/%s", DATA_DIR, player);
 
  if (!ptstflg (mynum, PFL_LD_STORE) || !ptstflg(mynum, PFL_CLONE)) {
    erreval ();
    return;
  }
  else if (!(f = FOPEN(path, "w"))) {
    if (!silent)
      bprintf("Unable to write your wiz zone, sorry.\n");
    return;
  }
  
  z = get_wizzone_by_name(player);

  fprintf(f, "%%mobiles\n\n");  
  for (ct = 0 ; ct < zmaxmobs(z) ; ct++) {
    i = zmob_nr(ct, z);
    fprintf(f, "Name            = %s\n", pzname(i));
    if (!EQ(pname(i), pzname(i)))
      fprintf(f, "Pname           = %s\n", pname(i));
    if (lzone(ploc_reset(i)) != z)
      fprintf(f, "Location        = %s@%s\n", 
	      lname(ploc_reset(i)), zname(lzone(ploc_reset(i))));
    else
      fprintf(f, "Location        = %s\n", lname(ploc_reset(i)));
    if (pstr_reset(i))
      fprintf(f, "Strength        = %d\n", pstr_reset(i));
    if (pdam_reset(i))
      fprintf(f, "Damage          = %d\n", pdam_reset(i));
    if (pagg_reset(i))
      fprintf(f, "Aggression      = %d\n", pagg_reset(i));
    if (parmor_reset(i))
      fprintf(f, "Armor           = %d\n", parmor_reset(i));
    if (pspeed_reset(i))
      fprintf(f, "Speed           = %d\n", pspeed_reset(i));
    if (pftxt(i))
      fprintf(f, "Description     = \"%s\"\n", pftxt(i));
    store_mob_flags(False, "", f, i);
    fprintf(f, "End             = %s\n\n", pzname(i));
  }

  fprintf(f, "%%objects\n\n");
  for (ct = 0 ; ct < zmaxobjs(z) ; ct++) {
    i = zobj_nr(ct, z);
    fprintf(f, "Name            = %s\n", ozname(i));
    if (!EQ(ozname(i), oname(i)))
      fprintf(f, "Pname           = %s\n", oname(i));
    if (oaltname(i))
      fprintf(f, "AltName         = %s\n", oaltname(i));

    switch (ocarrf_reset(i)) {
    case IN_CONTAINER:
      sprintf(loc, "IN_CONTAINER:%s", ozname(oloc_reset(i))); 
      nz = ozone(oloc_reset(i));
      break;
    case IN_ROOM:
      sprintf(loc, "IN_ROOM:%s", lname(oloc_reset(i)));
      nz = lzone(oloc_reset(i));
      break;
    case CARRIED_BY:
      sprintf(loc, "CARRIED_BY:%s", pzname(oloc_reset(i)));
      nz = pzone(oloc_reset(i));
      break;
    case WORN_BY:
      sprintf(loc, "WORN_BY:%s", pzname(oloc_reset(i)));
      nz = pzone(oloc_reset(i));
      break;
    case BOTH_BY:
      sprintf(loc, "BOTH_BY:%s", pzname(oloc_reset(i)));
      nz = pzone(oloc_reset(i));
      break;
    default:
      nz = -1;             /* makes the compiler happy */
    }

    if (nz != z)
      fprintf(f, "Location        = %s@%s\n", loc, zname(nz)); 
    else
      fprintf(f, "Location        = %s\n", loc);

    if (oarmor(i))
      fprintf(f, "Armor           = %d\n", oarmor_reset(i));
    if (obaseval(i))
      fprintf(f, "BValue          = %d\n", obaseval(i));
    if (osize(i))
      fprintf(f, "Size            = %d\n", osize_reset(i));
    if (oweight(i))
      fprintf(f, "Weight          = %d\n", oweight_reset(i));
    if (state_reset(i))
      fprintf(f, "State           = %d\n", state_reset(i));
    if (omaxstate(i))
      fprintf(f, "MaxState        = %d\n", omaxstate(i));
    for (j = 0 ; j < 4 ; j++)
      if (olongt(i, j))
	fprintf(f, "Desc[%d]         = \"%s\"\n", j, olongt(i, j));
    if (oexam_text(i))
      fprintf(f, "Examine         = \"%s\"\n", oexam_text(i));
    store_obj_flags(False, "", f, i);
    fprintf(f, "End               = %s\n\n", ozname(i));
  }

  fprintf(f, "%%locations\n\n");
  for (ct = 0 ; ct < zmaxlocs(z) ; ct++) {
    i = zloc_nr(ct, z);
    fprintf(f, "%s ", lname(i));
    for (j = 0 ; j < NEXITS ; j++)
      if ((q = lexit_reset(i, j))) {
	if (q > DOOR) {
          q -= DOOR;
          if (ozone(q) != z)
  	    fprintf(f, "%c:^%s@%s ", *exits[j], ozname(q), zname(ozone(q)));
          else
	    fprintf(f, "%c:^%s ", *exits[j], ozname(q));
        }
	else {
          if (lzone(q) != z)
            fprintf(f, "%c:%s@%s ", *exits[j], lname(q), zname(lzone(q)));
          else
  	    fprintf(f, "%c:%s ", *exits[j], lname(q));
        }
      }
    fprintf(f, ";\n");
    store_loc_flags(False, "", f, i);
    fprintf(f, "%s^\n%s^\n\n", sdesc(i), ldesc(i));
  }
  if (!silent) {
    if (!zmaxobjs(z) && !zmaxmobs(z) && !zmaxlocs(z))
      bprintf("Zone %s: nothing to store.\n", pname(mynum));
    else
      bprintf("Zone %s stored: [%d object%s | %d mobile%s | %d location%s]\n",
        pname(mynum), zmaxobjs(z), zmaxobjs(z) != 1 ? "s" : "",
           zmaxmobs(z), zmaxmobs(z) != 1 ? "s" : "",
           zmaxlocs(z), zmaxlocs(z) != 1 ? "s" : "");
  }
  FCLOSE(f);
}

void loadcom (void) {
  char *p;
  int zlev;
  PERSONA P;
  int plr;

  if (!ptstflg(mynum, PFL_LD_STORE) || !ptstflg(mynum, PFL_CLONE)) {
    erreval ();
    return;
  }
  if (brkword () == -1) {
    zlev = plev (mynum);
    p = pname (mynum);
  } 
  else if ((plr = fpbn(wordbuf)) >= 0) {
    zlev = plev (plr);
    p = pname (plr);
  } 
  else if (getuaf (wordbuf, &P)) {
    zlev = P.ublock.plev;
    p = P.ublock.pname;
  } 
  else {
    bprintf ("No such player: %s\n", wordbuf);
    return;
  }

  if (!EQ (pname (mynum), p) && !do_okay_l (plev (mynum), zlev, True)) {
    bprintf ("You're not powerful enough.\n");
    return;
  }

  load_zone(p, False);
}

void linkcom (void) {
  int obj1, obj2;

  if (!ptstflg(mynum, PFL_CLONE)) {
    erreval ();
    return;
  }
  if ((obj1 = ob1) == -1) {
    bprintf ("Object 1 does not exist.\n");
    return;
  }
  if ((obj2 = ob2) == -1) {
    bprintf ("Object 2 does not exist.\n");
    return;
  }
  olinked (obj1) = obj2;
  olinked (obj2) = obj1;

  mudlog ("LINK: %s linked %s (%d) to %s (%d)", pname (mynum), oname (obj1),
	  int2idx(obj1, OBJ), oname (obj2), int2idx(obj2, OBJ));

  bprintf ("Linked %s to %s.\n", oname (obj1), oname (obj2));
}

void erasezonecom (void) {
  int z;
  if (!ptstflg(mynum, PFL_CLONE) || !ptstflg(mynum, PFL_LD_STORE)) {
    erreval();
    return;
  }
  if (!EQ(wordbuf, "erasezone")) {
    bprintf("You must type the entire word, ERASEZONE to erase your zone.\n");
    return;
  }
  else {
    z = get_wizzone_by_name(pname(mynum));

    while (zmaxmobs(z))
      destruct_mobile(zfirst_mob(z));
    
    while (zmaxobjs(z))
      destruct_object(zfirst_obj(z));

    while (zmaxlocs(z)) {
      if (!destruct_location(zfirst_loc(z))) {
        bprintf("Error during destruct: players are in your zone.\n");
        return;
      }
    }
    storecom(pname(mynum), True);
    bprintf("Zone erased.\n");
  }
}

void maxstatecom (void) {
  int obj, st;

  if (!ptstflg(mynum, PFL_CLONE)) {
    erreval ();
    return;
  }
  if ((obj = ob1) == -1) {
    bprintf ("Object does not exist.\n");
    return;
  }
  if (EMPTY (txt2)) {
    bprintf ("A new MaxState must be provided.\n");
    return;
  }
  st = max (0, atoi (txt2));

  if (st > 3) {
    bprintf ("MaxState too large: %d.\n", st);
    return;
  } else {
    bprintf ("Setting MaxState to %d.\n", st);
  }

  mudlog ("MAXSTATE: %s set the MaxState of %s (%d) to %d", pname (mynum),
	  oname (obj), obj, st);

  omaxstate (obj) = st;
}

/* get rid of all the wiz zone clones that a player is carrying, which *
 * don't belong to their specific wizzone, if it exists                */

void destruct_clones(int plr) {
  int i, ct;

  for (ct = 0 ; ct < pnumobs(plr) ; ct++) {
    i = pobj_nr(ct, plr);
    if (i >= num_const_obs) {
      if (ozone(i) != -1 && !EQ(zname(ozone(i)), pname(mynum))) {
        sendf(plr, "The %s vanishes as you take it out of it's zone.\n", 
          oname(i));
        destruct_object(i);
        ct--;
      }
    }
  }
}
