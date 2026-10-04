/* do not include this file anywhere */

#ifndef _BOOTSTRAP_H
#define _BOOTSTRAP_H

extern int max_players;

#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "MACHINE.H"
#include "config.h"
#include "levels.h"
#include "exits.h"
#include "types.h"
#include "utils.h"
#include "flags.h"
#include "mudtypes.h"
#include "mudmacros.h"
#include "files.h"

#include "memory.h"
#define NEW(t, c) ((t *)memory_alloc((c), sizeof(t)))
#define BCOPY(s, l) memory_copy((s), (l))
#define COPY(s) memory_string(s)
#define EMPTY(p)		(*(p) == '\0')
#define EQ(a, b)		(strcasecmp((a), (b)) == 0)

#define BASEMULT  10     /* use this to calculate initial sizes of arrays,  */
                         /* set this to the avg [objects|mobs|locs] in zone */ 
#define BASE_INCR 100    /* grow the arrays by this amount */
#define PATHLEN   256
#define BUFFLEN   256
#define PAGELEN   4096

#define PROC_OBJECTS   0
#define PROC_LOCATIONS 1
#define PROC_MOBILES   2

#define LOCATIONS_H   H_DIR "/locations.h"
#define MOBILES_H     H_DIR "/mobiles.h"
#define OBJECTS_H     H_DIR "/objects.h"

HASH_TABLE ublock_z[BASE_SIZE];     /* zone names -> mobile # */
HASH_TABLE ublock_n[BASE_SIZE];     /* pnames -> mobile #     */
HASH_TABLE ualts_n[BASE_SIZE];      /* mob altnames -> mobile # */

HASH_TABLE objects_z[BASE_SIZE];    /* zone names -> object # */
HASH_TABLE objects_n[BASE_SIZE];    /* pnames -> object # */
HASH_TABLE oalts_n[BASE_SIZE];      /* obj altnames -> object # */

HASH_TABLE locations_z[BASE_SIZE];  /* zone names -> location # */

#include "exitnames.h"
#include "lflagnames.h"
#include "aflagnames.h"
#include "oflagnames.h"
#include "pflagnames.h"
#include "mflagnames.h"
#include "sflagnames.h"
#include "nflagnames.h"
#include "eflagnames.h"

#ifdef GEN
int main(void);

int numloc = 0;
int numchars = 0;
int numzon = 0;
int numobs = 0;
ZONE             *zones;
UBLOCK_REC       *ublock;
OBJECT           *objs;
Location         *room_data;

typedef struct _XTABLE {
  struct _XTABLE *next;
  int code;
  char name[30];
} XTABLE;

#else

extern int numloc;
extern int numchars;
extern int numzon;
extern int numobs;
extern char *Exits[];
extern char *Lflags[];
extern char *Locflags[];
extern char *Objflags[];
extern char *Mobflags[];
extern char **mobflagsindex[];
extern char **oflagsindex[];
extern char **lflagsindex[];
extern int max_players;
extern int num_const_locs;
extern int num_const_chars;
extern int num_const_obs;
extern T_elemptr verb_t;
extern PLAYER_REC       *players;
extern RPLR_REC         *rplrs;
extern IO_REC           *iplrs;
extern ZONE             *zones;
extern UBLOCK_REC       *ublock;
extern OBJECT           *objs;
extern Location         *room_data;

#endif

int ublock_len;
int objects_len;
int locations_len;
int zones_len;
int xzon;
int xloc;
int xobj;
int xchar;
int loading = -1;

char *objtab[] = {
  "Name",
  "Pname",
  "AltName",
  "Location",
  "Linked",
  "State",
  "MaxState",
  "Desc[0]",
  "Desc[1]",
  "Desc[2]",
  "Desc[3]",
  "Armor",
  "BValue",
  "Size",
  "Weight",
  "Examine",
  "Damage",
  "End",
  TABLE_END
};

char *mobtab[] = {
  "Name",
  "Pname",
  "Location",
  "Strength",
  "Damage",
  "Aggression",
  "Armor",
  "Speed",
  "Description",
  "End",
  "Examine",
  TABLE_END
};

char *loctypes[] = {
  "IN_CONTAINER",
  "IN_ROOM",
  "CARRIED_BY",
  "WIELDED_BY",
  "WORN_BY",
  "BOTH_BY",
  TABLE_END
};

void set_flagpoints(void);
void init_room(void);
int bootstrap(void);
int boot_arrays(int);
int boot_verbs(void);
int set_zone_global(char *str);
char *prepass(Boolean *, Boolean *, Boolean *, char *);
int get_key_value(char *buff, char **key, char **value);
int string2num(int *num, char *str);
Boolean valid_zone(char *);
int store_headers(void);
int store_data(void);
void ucase(char *dest, char *src);
int process_zone(char *, Boolean, Boolean);
void load_zone(char *, Boolean);
void setup_zone(Boolean, char *);
int proc_mobile(char *buff);
int proc_object(char *buff);
int proc_location(char *buff);
int obj_search(int, int);
int room_search(int, int);
int mob_search(int, int);
int llookup (char *, char **);
int link_zones(void);
int link_zone(int);
int hashfun(char *);
void resize_zones(void);

extern int  glookup(char *elem,int n,char **table,
            int (*strcmpfun)(const char *s1, const char *s2, size_t n));

#endif

