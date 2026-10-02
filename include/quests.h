#ifndef _QUESTS_H
#define _QUESTS_H

#define Q_EFOREST         0
#define Q_TOWER           1
#define Q_EXCALIBUR       2
#define Q_GRAIL           3
#define Q_FIERY_KING      4
#define Q_SPIKE           5
#define Q_FIND_PAINTING   6
#define Q_DRAKNOR         7
#define Q_EVOLUTION	  8
#define Q_ZODIAC	  9
#define Q_SUNDISC	  10
#define Q_VOLCANO	  11
#define Q_SABRE		  12
#define Q_RAINFOREST	  13
#define Q_NOXYPICKLE	  14
#define Q_TALON           15
#define Q_GUXX            16
#define Q_MITHDAN         17
#define Q_ORCHOLD         18
#define Q_RAMSES          19
#define Q_FAFFNER         20
#define Q_CHLYON          21
#define Q_MITHRIL         22
#define Q_PINKELEPHANT    23
#define Q_MAX		  24
#endif

void        qdonecom();
void 	    qlistcom();
void        QuestSet(int p, int v);
void        questcom();
void        show_quests(long int *bits, Boolean inverse);
int         qlookup(char *name);
int         qcheck(int pl);
int         qpoints(int pl);
Boolean     crit_qtest(int pl);

