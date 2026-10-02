#include "kernel.h"
#include "locations.h"
#include "objects.h"
#include "mobiles.h"
#include "mudmacros.h"
#include "ctype.h"
#include "bprintf.h"
#include "utils.h"
#include "uaf.h"
#include "mobile.h"
#include "objsys.h"
#include "sendsys.h"
#include "questnames.h"
#include "parse.h"
#include "log.h"
#include "quests.h"
#include "qlevels.h"

int quest_index[][2] = {
#ifdef LOCMIN_EFOREST
  {LOCMIN_EFOREST, LOCMAX_EFOREST},
#endif
#ifdef LOCMIN_TOWER
  {LOCMIN_TOWER, LOCMAX_TOWER},
#endif
#ifdef LOCMIN_SEA
  {LOCMIN_SEA, LOCMAX_SEA},
#endif
#ifdef LOCMIN_CATACOMB
  {LOCMIN_CATACOMB, LOCMAX_CATACOMB},
#endif
#ifdef LOCMIN_WASTE
  {LOCMIN_WASTE, LOCMAX_WASTE},
#endif
#ifdef LOCMIN_OAKTREE
  {LOCMIN_OAKTREE, LOCMAX_OAKTREE},
#endif
#ifdef LOCMIN_FROBOZZ
  {LOCMIN_FROBOZZ, LOCMAX_FROBOZZ},
#endif
#ifdef LOCMIN_THYRANNEN
  {LOCMIN_THYRANNEN, LOCMAX_THYRANNEN},
#endif
#ifdef LOCMIN_EVOLUTION
  {LOCMIN_EVOLUTION, LOCMAX_EVOLUTION},
#endif
#ifdef LOCMIN_ZODIAC
  {LOCMIN_ZODIAC, LOCMAX_ZODIAC},
#endif
#ifdef LOCMIN_ANCIENT
  {LOCMIN_ANCIENT, LOCMAX_ANCIENT},
#endif
#ifdef LOCMIN_VOLCANO
  {LOCMIN_VOLCANO, LOCMAX_VOLCANO},
#endif
#ifdef LOCMIN_PIRATE
  {LOCMIN_PIRATE, LOCMAX_PIRATE},
#endif
#ifdef LOCMIN_RAINFOREST
  {LOCMIN_RAINFOREST, LOCMAX_RAINFOREST},
#endif
#ifdef LOCMIN_NOXYPICKLE
  {LOCMIN_NOXYPICKLE, LOCMAX_NOXYPICKLE},
#endif
#ifdef LOCMIN_TALON
  {LOCMIN_TALON, LOCMAX_TALON},
#endif
#ifdef LOCMIN_ABYSS
  {LOCMIN_ABYSS, LOCMAX_ABYSS},
#endif
#ifdef LOCMIN_MITHDAN
  {LOCMIN_MITHDAN, LOCMAX_MITHDAN},
#endif
#ifdef LOCMIN_ORCHOLD
  {LOCMIN_ORCHOLD, LOCMAX_ORCHOLD},
#endif
#ifdef LOCMIN_COTTAGE
  {LOCMIN_COTTAGE, LOCMAX_COTTAGE},
#endif
#ifdef LOCMIN_NIBELUNG
  {LOCMIN_NIBELUNG, LOCMAX_NIBELUNG},
#endif
#ifdef LOCMIN_CHLYON
  {LOCMIN_CHLYON, LOCMAX_CHLYON},
#endif
#ifdef LOCMIN_CAVE
  {LOCMIN_CAVE, LOCMAX_CAVE},
#endif
#ifdef LOCMIN_GROVE
  {LOCMIN_GROVE, LOCMAX_GROVE}
#endif
};

extern int count_players_in_zone(int zone);
extern int count_players_between_locs(int, int);

char *quest_status(int quest) {
  int i = quest;

  if (qdtstflg(i))
    return("&+RDone ");
  else if (count_players_between_locs(quest_index[i][0], quest_index[i][1]) > 0)
    return("&+YIn Progress");
  else
    return("&+GAvailable");
}

/*****************************************************************************
 *  To set a quest flag. Makes check to see if gaining quest allows player to
 *  level, as well as sets the_world->q_done flags.
 *****************************************************************************/

void set_quest(int p, int v) {
  if (mynum < max_players)
    sendf(p, "Congratulations! You have completed the quest %s.\n", 
      Quests[v].name);

  mudlog ("Quest &+M%s&N completed by %s", Quests[v].name, pname(p));

  send_msg(DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
           "&+Y[Quest &+M%s&+Y completed by %s]\n", Quests[v].name, pname(p));

  if (mynum < max_players)
    qsetflg(p, v);
  else
    bprintf("Really useful, completing quests as a mobile..\n");

  qdsetflg(v);
  return;
}

/*
**  QLIST shows current list of quests and what points they are worth.
**  Points are only appropriate to certain quests systems, as is this command.
*/

void qlistcom() {
  int i = 0;
  int qpoints = 0;
  Boolean nl;

  nl = False;
  bprintf("                     &-BQuest Status for This Reset\n\n");
  bprintf("&+WQUEST        PTS  CRIT  STATUS            "
             "QUEST        PTS  CRIT  STATUS\n");
  while(Quests[i].name != TABLE_END) {
    if( Quests[i].name[0] ) {
      bprintf("%-13s %-4d%s  %-14s&N",
              Quests[i].name, Quests[i].pnt, 
              (Quests[i].crit ? "&+WYes&N " : "No  "), quest_status(i));
      qpoints += Quests[i].pnt;
      bprintf( "%s", nl ? "\n" : "       &N" );
      if( !nl ) 
        nl = True;
      else
        nl = False;
    }
    i++;
  }
  if( nl )
    bprintf( "\n" );
#if QTYPE <= 1
  bprintf("\nPoints, and critical have no meaning, please ignore.\n");
#elif QTYPE == 2
  bprintf("\nCritical has no meaning, please ignore\n");
#elif QTYPE == 4
bprintf( "\nYou must have %d (of %d) quest points and all the critical quests done to wiz.\n",
         WIZ_QPOINTS, qpoints );
#endif
  bprintf("\n");
  return;
}

/*
 *  Returns level a player could have according to quests/quest points they 
 *  have. qpoints() decides if it is dependent on points or just number of
 *  quests.
 */

int qcheck(int pl) {
  int pnt, i;

  i = -1;

  if ((plev(pl) >= LVL_WIZARD) || (plev(pl) < 1))
    return plev(pl);

#if QTYPE == 0
  if (!tstbits(qflags(pl), Q_ALL))
    return LVL_WIZARD - 1;
  else
    return LVL_WIZARD;

#elif (QTYPE == 1 || QTYPE == 2)
  pnt = qpoints(pl);
  for(i = LVL_WIZARD; i > 0; i--) {
    if (pnt >= qlevels[i])
      return i;
  }

#elif QTYPE == 3
  pnt = qpoints(pl);
  if (pnt >= WIZ_QPOINTS  && crit_qtest(pl))
    return LVL_WIZARD;
  else
    for(i = LVL_WIZARD; i > 0; i--) {
     if (pnt >= qlevels[i])
       return i;
   }

#elif QTYPE == 4
  pnt = qpoints(pl);
  if(pnt >= WIZ_QPOINTS  && crit_qtest(pl))
    return LVL_WIZARD;
  else
    return LVL_WIZARD - 1;
#endif
}

/*
**  For quests systems with critical quests, that is ones players MUST do
*/
Boolean crit_qtest(int pl)
{
  int i = 0;

  while(Quests[i].name != TABLE_END)
  {
    if (Quests[i].crit && !qtstflg(pl, i))
      return False;
    i++;
  }
  return True;
}

/*
**  Depending on the QTYPE defined, this function returns either the number
**  of quests done, or number of quest points obtained, which ever is 
**  appropriate to the quest system. Default is number of quests
*/
int qpoints(int pl)
{
  int pnt = 0;
  int i = 0;
  int num = 0;
  
  while(Quests[i].name != TABLE_END)
  {
    if (qtstflg(pl, i))
    {
      num++;
      pnt += Quests[i].pnt;
    }
    i++;
  }
#if ((QTYPE == 2) || (QTYPE == 3) || (QTYPE == 4))
  return pnt;
#else
  return num;
#endif
}

/* returns -1, 0, 1 for none, some, all quests done */

int check_quests(long int *bits) {
  Boolean found = False;
  Boolean all = True;
  int i;

  for (i = 0 ; i < Q_MAX ; i++)
    if (ptst_flg(bits, i, QFLAGS))
      found = True;
    else
      all = False;

  if (all)
    return(1);
  else if (found)
    return(0);
  else
    return(-1);
}

void questcom(void) {
  int a, b, l, qstatus;
  char *n;
  Boolean f, c;
  long int *bits;
  PERSONA d;

  f = False;


  if (brkword() == -1) {
    a = mynum;
    n = pname(a);
    l = plev(a);
    bits = mbits(a);
  } 
  else if ((a = pl1) != -1)  {
    if (a != mynum && plev(mynum) < LVL_WIZARD) {
      bprintf("You can only check your own Quest-stats.\n");
      return;
    }
    n = pname(a);
    l = plev(a);
    bits = mbits(a);
  } 
  else if (!getuaf(wordbuf,&d))  {
    bprintf("No such persona in system.\n");
    return;
  } 
  else if (plev(mynum) < LVL_WIZARD) {
    bprintf("You can only check your own Quest-stats.\n");
    return;
  } 
  else {
    f = True;
    bits = d.ublock.bits;
    n = d.ublock.pname;
    l = d.ublock.plev;
  }

  qstatus = check_quests(bits);

  if (brkword() == -1) {
    if (qstatus == 1)
      bprintf("&+C%s has completed all quests!\n", n);
    else if (qstatus == 0) {
      bprintf("\n&+c----------------------------------"
              "--------------------------------------------\n");
      bprintf("                                &+MQUESTS COMPLETED\n\n");
      show_bits(bits, mindex, (char **) Quests, QFLAGS, False);
      bprintf("&+c----------------------------------"
              "--------------------------------------------\n");
    } 
    else
      bprintf("&+W%s has completed no quests!\n", n);

    if (qstatus == 0) {
      bprintf("                               &+YQUESTS NOT COMPLETED\n\n");
      show_bits(bits, mindex, (char **) Quests, QFLAGS, True);
      bprintf("&+c----------------------------------"
              "--------------------------------------------\n");
    }
    return;
  } 
  else if ((b = qlookup(wordbuf)) == -1) {
    bprintf("%s: No such Quest.\n", wordbuf);
    return;
  } 
  else if (brkword() == -1 || plev(mynum) < LVL_WIZARD)  {
    c = ptst_flg(bits, b, QFLAGS);
    bprintf("Value of %s is %s\n", Quests[b].name, TF[c]);
    return;
  } 
  else if (plev(mynum) < LVL_ARCHWIZARD && !EQ(n, pname(mynum))) {
    bprintf("You can't change other players Quest-stats.\n");
    return;
  } 
  else if ((c = tlookup(wordbuf,TF)) == -1) {
    bprintf("Value must be True or False.\n");
    return;
  }

  bprintf("Value of %s set to %s\n", Quests[b].name, TF[c]);
  mudlog("Quest: %s by %s, %s := %s", n, pname(mynum), Quests[b].name, TF[c]);

  if (c == 0) 
    pclr_flg(bits, b, QFLAGS);
  else 
    pset_flg(bits, b, QFLAGS);
  if (f) 
    putuaf(&d);
}

/*
**  Return array index for name in Quests[].name struct
*/
int qlookup(char *name)
{
  int i = 0;

  while(Quests[i].name != TABLE_END)
  {
    if (strncasecmp(Quests[i].name, name, strlen(name)) == 0)
    {
      return i;
    }
    i++;
  }
  return -1;
}
