#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "kernel.h"
#include "zones.h"
#include "update.h"
#include "sendsys.h"
#include "mud.h"
#include "bprintf.h"
#include "spell.h"
#include "exit.h"
#include "clone.h"
#include "mobile.h"
#include "client.h"
#include "uaf.h"
#include "objsys.h"
#include "god.h"

extern char *Mobflags[];  /* why is this needed? */
extern char *Objflags[];
extern char **lflagsindex[];
extern char **mobflagsindex[];
extern char **oflagsindex[];

extern void save_pid(void);
extern int bootstrap(void);
extern void load_zone(char *player, Boolean silent);
extern void setup_io(int plx, int fd);
extern int mob_search(int, int);
extern int obj_search(int, int);
extern int room_search(int, int);
extern void write_packet(int);
extern void main_loop(int);
extern void rm_pid(void);
extern int proc_flags(int, long int *, int [], char *, char *[], char *);
extern HASH_TABLE ublock_z[];
extern HASH_TABLE objects_z[];
extern HASH_TABLE locations_z[];

void rebootcom (void) {
  if (!ptstflg (mynum, PFL_REBOOT)) {
    bprintf ("You cannot reboot the MUD.\n");
    return;
  }
  run_reboot (False, False);
}

void updatecom (void) {
  if (!ptstflg (mynum, PFL_REBOOT)) {
    bprintf ("You cannot update the MUD.\n");
    return;
  }
  run_reboot (False, True);
}

void run_reboot (Boolean crash, Boolean will_update) {
  int i, plx, new, nplayers;
  char newpath[256], oldpath[256], datafile[256], bindir[256], port[10];
  char max[10], pid[10], *prog;
  REBOOT_REC reboot_rec;
  RPLR_REC rplr_rec;
  FILE *fp;

  if ((prog = strrchr(progname, '/')))   prog++;
  else prog = progname;

  sprintf(newpath, "%s/bin/%s.new", BASE_LOC, prog);
  sprintf(oldpath, "%s/bin/%s", BASE_LOC, prog);
  sprintf(bindir, "%s/bin", BASE_LOC);

  if (access(newpath, F_OK) != -1)
    new = 1;
  else if (access(oldpath, F_OK) != -1)
    new = 0;
  else {
    bprintf("Error: could not boot mud from requested executable:\n\n");
    mudlog ("REBOOT: Error: Could not find executable");
    return;
  }

  if (crash)
    new = 0;

  for (i = 0 ; i < max_players ; i++)
    if (players[i].iamon)
      wipe_duration(i);

  autosave();

  for (i = num_const_zon ; i < numzon ; i++) {
    storecom(zname(i), True);
    if ((plx = fpbn(zname(i))) != -1)
      sendf(plx, "&+G[Zone AutoSaved]\n");
  }

  if (aberfd != -1)
    aberchat_shutdown();

  if (dnsfd != -1)
    close(dnsfd);

  if (crash) {
    send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
              "&+W[&+CEmergency %s To Prevent Crash&+W]\n",
              will_update ? "Update" : "Reboot");
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              "\001f%s\003", CRASH_UPDATE);
    mudlog ("&+RREBOOT:&N Crash has occured, rebooting system");
  }
  else if (will_update) {
    send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
              "&+W[&+wUpdate %sby &+C\001p%s\003&+W]\n",
              new ? "&+WNew &+w" : "", pname (mynum));
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              "\001f%s\003", UPDATE);
    mudlog ("&+WUPDATE:&N Update %sby %s", new ? "New " : "", pname(mynum));
  }
  else if (breset) {
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              MUD_NAME "\001f%s\003", UPDATE);
    mudlog ("&+WREBOOT:&N Rebooted %sby BootReset", new ? "New " : "");
  }
  else {
    send_msg (DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
              "&+W[&+wReboot %sby &+C\001p%s\003&+W]\n",
              new ? "&+WNew &+w" : "", pname (mynum));
    send_msg (DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              "\001f%s\003", UPDATE);
    mudlog ("&+WREBOOT:&N Rebooted %sby %s", new ? "New " : "", pname(mynum));
  }

  if (crash)
    ++numcrashes;
  else
    ++numreboots;

  if (!will_update)
    qdresetflgs();

  sprintf (datafile, "%s/reboot_file.%d", TEMP_DIR, getpid());

  for (i = 0, nplayers = 0; i < max_players; ++i)
    if (players[i].iamon)
      nplayers++;

  snprintf (reboot_rec.version, sizeof (reboot_rec.version), "%s", VERSION);
  reboot_rec.main_socket = main_socket;
  reboot_rec.last_startup = last_startup;
  reboot_rec.numresets = numresets;
  reboot_rec.numreboots = numreboots;
  reboot_rec.numcrashes = numcrashes;
  reboot_rec.last_reset = last_reset;
  reboot_rec.nplayers = nplayers;

  for (i = 0 ; i < WLEN ; i++)
    reboot_rec.bits[i] = the_world->wbits[i];

#ifdef IO_STATS
  reboot_rec.bytes_read = bytes_read;
  reboot_rec.bytes_sent = bytes_sent;
  reboot_rec.unres_hosts = unres_hosts;
  reboot_rec.sock_conns = sock_conns;
  reboot_rec.cut_conns = cut_conns;
#endif
  if ((fp = FOPEN(datafile, "wb")) == NULL)
    mudlog ("REBOOT: Error writing reboot datafile");

  fwrite (&reboot_rec, sizeof (reboot_rec), 1, fp);

  for (plx = 0; plx < max_players; ++plx) {
    if (!is_conn(plx))
      continue;

    setup_globals (plx);

    if (!cur_player->iamon) {
      bprintf ("%s is currently rebooting.\n", MUD_NAME);
      quit_player(-4);
    }
    else {
      write_packet(fildes(plx));
      memcpy(&rplr_rec, &rplrs[plx], sizeof(RPLR_REC));
      strcpy (rplr_rec.pname, pname(plx));
      rplr_rec.plx = plx;
      rplr_rec.ploc = ploc (plx);
      rplr_rec.pvis = pvis (plx);
      rplr_rec.pweapon = pwpn(plx);
      fwrite (&rplr_rec, sizeof (rplr_rec), 1, fp);
    }
  }
  FCLOSE(fp);

  if (will_update)
    dump_world();
  close_mudfiles();

  if (new) {
    unlink (oldpath);
    rename(newpath, oldpath);
  }

  sprintf (port, "%d", mud_port);
  sprintf (max, "%d", max_players);
  sprintf (pid, "%d", getpid());
  chdir(bindir);
  execl(prog, prog, "-p", port, "-n", max, will_update ? "-u" : "-r",pid,NULL);
}

void setsignals(void) {
  signal (SIGHUP, SIG_IGN);
  signal (SIGPIPE, SIG_IGN);            /* Broken pipe */
  signal (SIGTSTP, SIG_DFL);
  signal (SIGQUIT, SIG_IGN);
  signal (SIGCONT, SIG_DFL);
  signal (SIGTTOU, SIG_DFL);
  signal (SIGTTIN, SIG_DFL);
  signal (SIGTERM, sig_handler);        /* CTRL-C or kill -2 */
  signal (SIGSEGV, sig_handler);        /* Segmentation fault */
  signal (SIGFPE, sig_handler);         /* Floating point exception */
  signal (SIGBUS, sig_handler);         /* Bus error */
  signal (SIGSYS, sig_handler);         /* Bad argument to system call */
  signal (SIGUSR1, sig_handler);        /* User defined signal */
  signal (SIGUSR2, sig_handler);        /* User defined signal */
}

/* This function is run instead of xmain, after the mud boots, on an update */

int xmain_reboot (void) {
  int i, s, nplayers, plx;
  char newname[256], zonename[30];
  REBOOT_REC r_rec;
  RPLR_REC p_rec;
  FILE *fp, *zfp;

  setsignals();

  if (open_logfile (clear_syslog_file) < 0)
    return -1;

  save_pid();

  sprintf (newname, "%s/reboot_file.%d", TEMP_DIR, old_proc_num);
  if ((fp = FOPEN(newname, "rb")) == NULL) {
    mudlog ("REBOOT: Datafile %s could not be opened for reading", newname);
    return -1;
  }
  fread (&r_rec, sizeof (r_rec), 1, fp);

  if (!EQ (r_rec.version, VERSION))
    mudlog ("UPGRADE: Old Ver: %s; New Ver: %s", r_rec.version, VERSION);

  s = r_rec.main_socket;
  last_startup = r_rec.last_startup;
  numresets = r_rec.numresets;
  numreboots = r_rec.numreboots;
  numcrashes = r_rec.numcrashes;
  last_reset = r_rec.last_reset;
  nplayers = r_rec.nplayers;

  for (i = 0 ; i < WLEN ; i++)
    the_world->wbits[i] = r_rec.bits[i];

#ifdef IO_STATS
  bytes_read = r_rec.bytes_read;
  bytes_sent = r_rec.bytes_sent;
  unres_hosts = r_rec.unres_hosts;
  sock_conns = r_rec.sock_conns;
  cut_conns = r_rec.cut_conns;
#endif

  if (bootstrap ()) {
    mudlog("REBOOT: Bootstrap failed on reboot");
    return -1;
  }
  main_socket = s;

  sprintf (newname, "%s/update.zones.%d", TEMP_DIR, old_proc_num);
  if ((zfp = FOPEN(newname, "r")) != NULL) {
    while (!feof(zfp)) {
      fscanf(zfp, "%s\n", zonename);
      load_zone(zonename, True);
    }
    FCLOSE(zfp);
  }
  for (i = 0; i < nplayers; i++) {
    fread (&p_rec, sizeof (p_rec), 1, fp);
    plx = p_rec.plx;
    memcpy(&rplrs[plx], &p_rec, sizeof(RPLR_REC));
    memset(&players[plx], 0, sizeof(PLAYER_REC));
    memset(&ublock[plx], 0, sizeof(UBLOCK_REC));

    setup_io(plx, fildes(plx));
    setup_globals(plx);
    getuafinfo(p_rec.pname);
    initialize_slot(plx);
    setploc(plx, p_rec.ploc);
    setpvis(plx, p_rec.pvis);
    setpwpn(plx, p_rec.pweapon);
    push_input_handler (get_command);
    get_command (NULL);
  }

  FCLOSE(fp);
  unlink (newname);

  if (update) {
    for (i = 0; i < numzon; i++)
      reset_zone(i);

    update_world(old_proc_num);

    send_msg(DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              "You open your eyes and see " MUD_NAME " beginning "
              "to fade back\ninto reality. Suddenly, the world rearranges "
              "itself back to the way it was.\n");

    send_msg(DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
              "&+W[&+CUpdate Completed&+W]\n");
  }
  else {
    send_msg(DEST_ALL, 0, LVL_MIN, LVL_WIZARD, NOBODY, NOBODY,
              "You open your eyes and see " MUD_NAME " beginning "
              "to fade back\ninto reality. It all seems the same, yet "
              "somehow different.\n");

    send_msg(DEST_ALL, 0, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
              "&+W[&+CReboot Completed&+W]\n");
  }

  main_loop (s);

  mudlog ("REBOOT: Closing listening socket");
  close (s);
  return(0);
}

/* controls the loading of the dumped world data differences */

void update_world(int procnum) {
  updating = True;

  load_locs();
  load_mobs();
  load_objs();

  updating = False;
}


void store_exits(int rm, FILE *f) {
  int ex;

  fprintf(f, "E: %s@%s ", lname(rm), zname(lzone(rm)));
  for (ex = 0 ; ex < NEXITS ; ex++) {
    if (lexit(rm, ex) > DOOR)
      fprintf(f, "^%s@%s", ozname(lexit(rm, ex) - DOOR),
	      zname(ozone(lexit(rm, ex) - DOOR)));
    else if (lexit(rm, ex))
      fprintf(f, "%s@%s ", lname(lexit(rm, ex)),
              zname(lzone(lexit(rm, ex))));
    else
      fprintf(f, "0 ");
  }
  fprintf(f, ";\n");
}

/* stores location differences */

void store_locs(void) {
  FILE *f;
  char path[256], buff[256];
  int i, ex;

  sprintf(path, "%s/reboot_locs.%d", TEMP_DIR, getpid());
  
  if (!(f = FOPEN(path, "w")))
    return;
  else {
    for (i = 0 ; i < numloc ; i++) {
      for (ex = 0 ; ex < NEXITS ; ex++)
	if (lexit_reset(i, ex) != lexit(i, ex)) {
          store_exits(i, f);
          break;
        }

      if (bitschanged(lbits(i), lindex, Locflags, MAX_LOC_FLAGS)) {
        sprintf(buff, "F: %s@%s ", lname(i), zname(lzone(i)));
        store_loc_flags(True, buff, f, i);
      }
    }
    FCLOSE(f);
  }
}

void load_locs(void) {
  FILE *f;
  char path[256], buff[256], *p, *q;
  int rm, i, flagset;

  sprintf(path, "%s/reboot_locs.%d", TEMP_DIR, getpid());

  if (!(f = FOPEN(path, "r")))
    return;
  else {
    while (1) {
      fgets(buff, 255, f);
      if (feof(f))
	break;
      else {
	p = strchr(buff + 3, ' ');
	*p++ = 0;

	if ((rm = find_loc_by_name(buff + 3)) != -1) {
	  if (*buff == 'F') {
            q = strchr(p, ' ');
            *q++ = 0;
            flagset = tlookup(p, Locflags);
	    proc_flags(flagset, lbits(rm), lindex, p, lflagsindex[flagset], q);
          }
	  else if (*buff == 'E') {
            for (i = 0 ; i < NEXITS ; i++) {
              sscanf(p, "%d", (int *) &(room_data[rm].r_exit[i]));
              p = strchr(p, ' ') + 1;
	    }
	  }
	}
      }
    }
    FCLOSE(f);
  }
}

void store_objs(void) {
  FILE *f;
  char path[256], buff[256];
  int i;

  sprintf(path, "%s/reboot_objs.%d", TEMP_DIR, getpid());

  if (!(f = FOPEN(path, "w")))
    return;
  else {
    for (i = 0 ; i < numobs ; i++) {
      if (ocarrf_reset(i) != ocarrf(i))  /* needs to come before oloc */
        fprintf(f, "C: %s@%s %d\n", ozname(i), zname(ozone(i)), ocarrf(i));
      if (oloc_reset(i) != oloc(i)) {
	fprintf(f, "L: %s@%s ", ozname(i), zname(ozone(i)));

	if (ocarrf(i) == IN_CONTAINER)
	  fprintf(f, "%s@%s\n", ozname(oloc(i)), zname(ozone(oloc(i))));
	else if (ocarrf(i) == IN_ROOM)
	  fprintf(f, "%s@%s\n", lname(oloc(i)), zname(lzone(oloc(i))));
	else if (ocarrf(i) >= CARRIED_BY && oloc(i) < max_players)
          fprintf(f, "%s\n", pname(oloc(i)));
        else
	  fprintf(f, "%s@%s\n", pzname(oloc(i)), zname(pzone(oloc(i))));
      }
      if (osize_reset(i) != osize(i))
	fprintf(f, "Z: %s@%s %d\n",ozname(i), zname(ozone(i)), osize_reset(i));
      if (ovalue_reset(i) != obaseval(i))
	fprintf(f, "V: %s@%s %d\n", ozname(i), zname(ozone(i)), obaseval(i));
      if (ovis_reset(i) != ovis(i))
	fprintf(f, "I: %s@%s %d\n", ozname(i), zname(ozone(i)), ovis(i));
      if (odamage_reset(i) != odamage(i))
	fprintf(f, "D: %s@%s %d\n", ozname(i), zname(ozone(i)), odamage(i));
      if (oarmor_reset(i) != oarmor(i))
	fprintf(f, "A: %s@%s %d\n", ozname(i), zname(ozone(i)), oarmor(i));
      if (state_reset(i) != state(i))
	fprintf(f, "S: %s@%s %d\n", ozname(i), zname(ozone(i)), state(i));
      if (oweight_reset(i) != oweight(i))
	fprintf(f, "W: %s@%s %d\n", ozname(i), zname(ozone(i)), oweight(i));
      if (bitschanged(obits(i), oindex, Objflags, MAX_OBJ_FLAGS)) {
        sprintf(buff, "F: %s@%s ", ozname(i), zname(ozone(i)));
        store_obj_flags(True, buff, f, i);
      }
    }
    FCLOSE(f);
  }
}

void store_mobs(void) {
  FILE *f;
  char path[256], buff[256];
  int i;

  sprintf(path, "%s/reboot_mobs.%d", TEMP_DIR, getpid());

  if (!(f = FOPEN(path, "w")))
    return;
  else {
    for (i = 0 ; i < max_players ; i++) {
      if (is_in_game(i))
        fprintf(f, "P: %s %s@%s\n", pname(i), lname(ploc(i)), 
          zname(lzone(ploc(i))));      
    }
    for (i = max_players ; i < numchars ; i++) {
      if (pstr_reset(i) != pstr(i))
        fprintf(f, "S: %s@%s %d\n", pzname(i), zname(pzone(i)), pstr(i));
      if (pvis_reset(i) != pvis(i))
        fprintf(f, "V: %s@%s %d\n", pzname(i), zname(pzone(i)), pvis(i));
      if (plev_reset(i) != plev(i))
        fprintf(f, "L: %s@%s %d\n", pzname(i), zname(pzone(i)), plev(i));
      if (pagg_reset(i) != pagg(i))
        fprintf(f, "A: %s@%s %d\n", pzname(i), zname(pzone(i)), pagg(i));
      if (pspeed_reset(i) != pspeed(i))
        fprintf(f, "Q: %s@%s %d\n", pzname(i), zname(pzone(i)), pspeed(i));
      if (pdam_reset(i) != pdam(i))
        fprintf(f, "D: %s@%s %d\n", pzname(i), zname(pzone(i)), pdam(i));
      if (parmor_reset(i) != parmor(i))
        fprintf(f, "R: %s@%s %d\n", pzname(i), zname(pzone(i)), parmor(i));
      if (pwimpy_reset(i) != pwimpy(i))
        fprintf(f, "W: %s@%s %d\n", pzname(i), zname(pzone(i)), pwimpy(i));
      if (!EQ(pname(i), pname_reset(i)))
        fprintf(f, "N: %s@%s %s\n", pzname(i), zname(pzone(i)), pname(i));
      if (ploc_reset(i) != ploc(i))
        fprintf(f, "X: %s@%s %s@%s\n", pzname(i), zname(pzone(i)),
          lname(ploc(i)), zname(lzone(ploc(i))));
      if (bitschanged(mbits(i), mindex, Mobflags, MAX_MOB_FLAGS)) {
        sprintf(buff, "F: %s@%s ", pzname(i), zname(pzone(i)));
        store_mob_flags(True, buff, f, i);
      }
    }    
    FCLOSE(f);
  }
}

int check_zone(char *zoname) {
  int zone;

  for (zone = 0 ; zone < numzon ; zone++)
    if (EQ(zname(zone), zoname))
      break;
  
  if (zone == numzon)        /* not found, must be a wiz zone */
    load_zone(zoname, True);
  return(zone);
}

void load_mobs(void) {
  FILE *f;
  char path[256], buff[256];
  char *p1, *p2, *p3, *p4;
  int x, plr, zone, flagset;

  sprintf(path, "%s/reboot_mobs.%d", TEMP_DIR, getpid());

  if (!(f = FOPEN(path, "r")))
    return;
  else {
    while (1) {
      fgets(buff, 255, f);
      if (feof(f))
        break;
      else {
	*strchr(buff, '\n') = 0;

        switch(*buff) {
        case 'F':
            p1 = buff + 3;
            p2 = strchr(p1, '@');
            *p2++ = 0;
            p3 = strchr(p2, ' ');
            *p3++ = 0;
            p4 = strchr(p3, ' ');
            *p4++ = 0;
            flagset = tlookup(p3, Mobflags);
	    zone = check_zone(p2);

	    if ((plr = ht_lookup(ublock_z, p1, 0, zone, mob_search)) == -1)
              continue;

            proc_flags(flagset, mbits(plr), mindex, p3,
              mobflagsindex[flagset], p4);
          break;
	case 'P':
	  p1 = buff + 3;
	  p2 = strchr(p1, ' ');
	  *p2++ = 0;
	  p3 = strchr(p2, '@');
          p3++;
	  if ((plr = fpbn(p1)) == -1)
            continue;

          check_zone(p3);
	  setploc(plr, find_loc_by_name(p2));
	  break;
	case 'N':
          p1 = buff + 3;
	  p2 = strchr(p1, '@');
          *p2++ = 0;
          p3 = strchr(p2, ' ');
	  *p3++ = 0;

	  zone = check_zone(p2);
	  if ((plr = ht_lookup(ublock_z, p1, 0, zone, mob_search)) == -1)
            continue;

	  setpname(plr, p3);
	  break;
	case 'X':
	  p1 = buff + 3;
	  p2 = strchr(p1, '@');
	  *p2++ = 0;
	  p3 = strchr(p2, ' ');
	  *p3++ = 0;
	  
	  zone = check_zone(p2);
          if ((plr = ht_lookup(ublock_z, p1, 0, zone, mob_search)) == -1)
            continue;

	  check_zone(strchr(p3, '@'));
	  setploc(plr, find_loc_by_name(p3));
	  break;
	default:                     /* default is a standard int value */
	  p1 = buff + 3;

          p2 = strchr(p1, '@');
          *p2++ = 0;
 
          p3 = strchr(p2, ' ');
          *p3++ = 0;

          x = atoi(p3);              /* x is val to set */
	  zone = check_zone(p2);
	  if ((plr = ht_lookup(ublock_z, p1, 0, zone, mob_search)) == -1)
            continue;

	  switch(*buff) {
	  case 'S':
	    setpstr(plr, x);
	    break;
	  case 'V':
	    setpvis(plr, x);
	    break;
	  case 'L':
	    setplev(plr, x);
	    break;
	  case 'A':
	    setpagg(plr, x);
	    break;
	  case 'Q':
	    setpspeed(plr, x);
	    break;
	  case 'D':
	    setpdam(plr, x);
	    break;
	  case 'R':
	    setparmor(plr, x);
	    break;
	  case 'W':
	    setpwimpy(plr, x);
	    break;
	  }
	}
      }
    }
    FCLOSE(f);
  }
}

void load_objs(void) {
  FILE *f;
  char path[256], buff[256];
  char *p1, *p2, *p3, *p4;
  int x, obj, zone, flagset;

  sprintf(path, "%s/reboot_objs.%d", TEMP_DIR, getpid());

  if (!(f = FOPEN(path, "r")))
    return;
  else {
    while (1) {
      fgets(buff, 255, f);
      if (feof(f))
        break;
      else {
	*strchr(buff, '\n') = 0;

        if (*buff == 'L') {
          p1 = buff + 3;
          p2 = strchr(p1, '@');
          *p2++ = 0;
          p3 = strchr(p2, ' ');
          *p3++ = 0;
          if ((p4 = strchr(p3, '@')))  /* if we have no @ it's a player */
            *p4++ = 0;

          zone = check_zone(p2);
          if ((obj = ht_lookup(objects_z, p1, 0, zone, obj_search)) == -1)
	    continue;

          if (p4)
  	    zone = check_zone(p4);
          switch(ocarrf(obj)) {
	  case IN_CONTAINER:
	    if ((x = ht_lookup(objects_z, p3, 0, zone, obj_search)) == -1)
	      continue;
	    break;
	  case IN_ROOM:
	    if ((x = ht_lookup(locations_z, p3, 0, zone, room_search)) == -1)
              continue;
            break;
	  default:
            if (p4) {  /* carried wielded or both by mobile */
  	      if ((x = ht_lookup(ublock_z, p3, 0, zone, mob_search)) == -1)
                continue;
            }
            else       /* carried wielded or both by player */
              x = fpbn(p3);
            break;
          }
          setoloc(obj, x, ocarrf(obj));
        }
        else if (*buff == 'F') {
          p1 = buff + 3;
          p2 = strchr(p1, '@');
          *p2++ = 0;
          p3 = strchr(p2, ' ');
          *p3++ = 0;
          p4 = strchr(p3, ' ');
          *p4++ = 0;
          flagset = tlookup(p3, Objflags);
	  zone = check_zone(p2);
	  if ((obj = ht_lookup(objects_z, p1, 0, zone, obj_search)) == -1)
            continue;
          proc_flags(flagset, obits(obj), oindex, p3,
             oflagsindex[flagset], p4);
        }
        else {
	  p1 = buff + 3;

          p2 = strchr(p1, '@');
          *p2++ = 0;
 
          p3 = strchr(p2, ' ');
          *p3++ = 0;

          x = atoi(p3);              /* x is val to set */
	  zone = check_zone(p2);

          if ((obj = ht_lookup(objects_z, p1, 0, zone, obj_search)) == -1)
            continue;

	  switch(*buff) {
	  case 'C':
	    ocarrf(obj) = x;
	    break;
	  case 'Z':
	    osetsize(obj, x);
	    break;
	  case 'V':
	    osetbaseval(obj, x);
	    break;
	  case 'I':
	    osetvis(obj, x);
	    break;
	  case 'D':
	    osetdamage(obj, x);
	    break;
	  case 'A':
	    osetarmor(obj, x);
	    break;
	  case 'S':
	    state(obj) = x;
	    break;
	  case 'W':
	    osetweight(obj, x);
	    break;
	  }
	}
      }
    }
    FCLOSE(f);
  }
}

/* stores the entire world's differences to data files */

void dump_world(void) {
  updating = True;
  store_locs();
  store_mobs();
  store_objs();
  updating = False;  
}

/* stuff that's not worth keeping over an update */

void initialize_slot(int plx) {
  ublock[plx].angry = -1;
  ublock[plx].pfighting = -1;
  ublock[plx].phelping = -1;
  players[plx].iamon = True;
}

/* Flush all io before a crash */

void flush_mud(Boolean init) {
  int plx;
  static int t_elapsed;

  if (init) {
    t_elapsed = global_clock;
    crashing = True;
  }

  if (global_clock - t_elapsed < FLUSH_TIMEOUT)
    for (plx = 0 ; plx < max_players ; plx++)
      if (output(plx))
        return;
  exit(0);
}

void sig_handler (int sig) {
  char msg[30];

  switch (sig) {
  case SIGTERM:
    mudlog("SIGNAL: SIGTERM");
    sigterm_exit();
    flush_mud(True);
    return;
  case SIGSEGV:
    mudlog("SIGNAL: SIGSEGV[%d]", sig);
    sprintf(msg, "SIGSEGV[%d]", sig);
    break;
  case SIGBUS:
    mudlog("SIGNAL: SIGBUS[%d]", sig);
    sprintf(msg, "SIGBUS[%d]", sig);
    break;
  case SIGINT:
    mudlog("SIGNAL: SIGINT[%d]", sig);
    sigterm_exit();
    flush_mud(True);
    return;
  case SIGUSR1:
    mudlog("SIGNAL: SIGUSR1[%d]", sig);
    sprintf(msg, "SIGUSR1[%d]", sig);
    break;
  case SIGUSR2:
    mudlog("SIGNAL: SIGUSR2[%d]", sig);
    sprintf(msg, "SIGUSR2[%d]", sig);
    break;
  default:
    mudlog("SIGNAL: %d", sig);
    sprintf(msg, "Unknown - %d", sig);
    break;
  }
  rm_pid();
  sig_exit(msg, sig);

  if (numcrashes > MAX_CRASHES) {
    send_msg(DEST_ALL, 0, LVL_MIN, LVL_MAX, NOBODY, NOBODY,
              "The game is being auto-closed [crash:%d]\n", numcrashes);
    crashcom(False);
  }
}
