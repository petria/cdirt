#ifndef _FLAGS_H
#define _FLAGS_H

#include "sflags.h"
#include "nflags.h"
#include "eflags.h"
#include "pflags.h"
#include "aflags.h"
#include "oflags.h"
#include "lflags.h"
#include "mflags.h"
#include "quests.h"
#include "cflags.h"
#include "log.h"

extern char *Sflags[];
extern char *Nflags[];
extern char *Eflags[];
extern char *Pflags[];
extern char *Aflags[];
extern char *Oflags[];
extern char *Lflags[];
extern char *Mflags[];

#ifndef GEN
  extern int windex[];
  extern int lindex[];
  extern int oindex[];
  extern int mindex[];
  extern int pflindex[];
#endif

#define MASK_MAX PFL_MAX

/* Pflag defaults bits */

#define PFLAGS		0
#define	MASKS		1
#define MAX_PFL_FLAGS   2

#define PFLLEN PFL_MAX/32 + MASK_MAX/32 + MAX_PFL_FLAGS

/* World default bits */

#define QDFLAGS         0
#define QDFLAGS_RESET   1
#define MAX_WORLD_FLAGS 2

#define WLEN Q_MAX/32 + Q_MAX/32 + MAX_WORLD_FLAGS

/* Location bits */

#define LFLAGS		0
#define LFLAGS_RESET    1
#define MAX_LOC_FLAGS   2

#define LOCLEN LFL_MAX/32 + LFL_MAX/32 + MAX_LOC_FLAGS

/* Object bits */

#define AFLAGS 		0       /* Armor flags 		 */
#define AFLAGS_RESET	1
#define OFLAGS		2	/* Basic object flags 	 */
#define OFLAGS_RESET	3
#define MAX_OBJ_FLAGS	4

#define OLEN  AFL_MAX/32 + AFL_MAX/32 + OFL_MAX/32 + OFL_MAX/32 + MAX_OBJ_FLAGS

#define PFLAGS		0	/* Player privlidges 	 */
#define PFLAGS_RESET	1
#define PMASK		2	/* Ability to set pflags */
#define PMASK_RESET     3
#define MFLAGS		4	/* Mobile flags 	 */
#define MFLAGS_RESET    5
#define SFLAGS          6	/* Basic player toggles  */
#define SFLAGS_RESET    7
#define NFLAGS          8	/* Mobile languages 	 */
#define NFLAGS_RESET    9
#define EFLAGS         10	/* Spell flags 		 */
#define EFLAGS_RESET   11
#define QFLAGS         12	/* Quest bits set 	 */
#define MAX_MOB_FLAGS  13

#define MLEN  PFL_MAX/32 + PFL_MAX/32 + MASK_MAX/32 + MASK_MAX/32 + \
              MFL_MAX/32 + SFL_MAX/32 + SFL_MAX/32 + NFL_MAX/32 + \
              NFL_MAX/32 + EFL_MAX/32 + EFL_MAX/32 + Q_MAX/32 + MAX_MOB_FLAGS

void    copy_bits(long int[], int[], int, int);
Boolean test_bit(long int[], int[], int, int);
void    set_bit(long int[], int[], int, Boolean, int);
void	show_bits(long int[], int[], char *flnames[], int, Boolean);
char    *dump_bits(long int[], int[], char *[], int);
Boolean bitschanged(long int[], int[], char *[], int);
Boolean dump_pflags (void);
Boolean flags_parse(int);
void    store_loc_flags(Boolean, char *, FILE *, int);
void    store_mob_flags(Boolean, char *, FILE *, int);
void    store_obj_flags(Boolean, char *, FILE *, int);
void    set_xpflags(long int[], int);
void	pflagscom(void);
void	maskcom(void);
void	lflagscom(void);
void	mflagscom(void);
void	sflagscom(void);
void	eflagscom(void);
void	nflagscom(void);
void    aflagscom(void);
int     boot_pflags(void);
#endif

