#include "xrweather.h"
#include "store.h"
#include "flags.h"

#ifndef _MUDTYPES_H
#define _MUDTYPES_H

#include <stdint.h>

#define TITLE_LEN	300	/* Max. length of a players title */
#define PROMPT_LEN	60	/* Max. length of the prompt */
#define PNAME_LEN	13	/* Max. length of a player-name */
#define MNAME_LEN	32	/* Max length, mobile-name */
#define ONAME_LEN	32	/* Max length, object-name */
#define SETIN_MAX	80	/* Max length, travel-message */
#define TTY_LEN		64	/* Max length, name of players host */
#define MSG_LEN		240	/* Max length of a message */
#define PASSWD_LEN	32	/* Max chars of passwd */
#define MAX_COM_LEN	300	/* Max chars in a command line */
#define LINE_LEN        300
#define MAX_COM_LEN     300
#define MAX_FDS         1024

#define NUM_CONDS	6
#define NUM_ACTS	4

typedef struct _a_inp_h {
  struct _a_inp_h *next;
  void            (*inp_handler)(char *input_string);
} INP_HANDLER;

typedef short ACTION;

/* mail structure */

struct M_message {
  char subject[LINE_LEN], mailfrom[LINE_LEN], mailto[LINE_LEN], date[LINE_LEN];
  char status;
  off_t text;

  struct M_message *next;
  struct M_message *prev;
};
typedef struct M_message Message;
typedef Message *Messageptr;

typedef struct _quest {
  int     pnt;                      /* How many points this quest is worth */
  Boolean crit;                     /* True if quest must be done */
  char    *name;                    /* Name of the quest, replaces Quests[] */
} QUEST;

struct Fd_Entry {                  /* redo this */
  int plx;
};

typedef struct Fd_Entry Fd_entry;

struct txt_line {
  char line[MAX_COM_LEN];
  struct txt_line *next;
};
typedef struct txt_line Text_Line;
typedef Text_Line *Text_Ptr;

typedef struct __t_elem T_elem;
typedef T_elem *T_elemptr;
struct __t_elem {
  T_elemptr left;
  T_elemptr right;
  int num;
  char *str;
};

/* structures for plr/mob's bodies : */

struct _Bodypart {
  Boolean attach;
  int armor;
  short int vital;
  char *name;
  int loc;
};
typedef struct _Bodypart Bodypart;

struct _body {
  Bodypart r_leg;
  Bodypart l_leg;
  Bodypart r_arm;
  Bodypart l_arm;
  Bodypart r_foot;
  Bodypart l_foot;
  Bodypart r_hand;
  Bodypart l_hand;
  Bodypart head;
  Bodypart chest;
  Bodypart back;
  Bodypart face;
  Bodypart neck;
};
typedef struct _body Body;

/* Pager Structure */

typedef struct _PAGER {
  FILE          *file;
  INP_HANDLER   *old_handler;
  char          prompt[PROMPT_LEN+40];
  int           len;
} PAGER;

/* Spell Duration Structure */

typedef struct _SPELL_DURATION {
  int		spell;
  int		duration;
  int		tmp;
  struct	_SPELL_DURATION	*next;
} SPELL_DURATION;

/* Zone table entry  */

typedef struct _ZONE {
  char	*z_name;		/* Zone name */
  int		maxlocs;
  int		maxmobs;
  int		maxobjs;
  int		latitude;
  int		rainfall;
  int_set	locations;
  int_set	mobiles;
  int_set	objects;
} ZONE;

/* Location data in the rooms arrray as read from locations file  */

typedef struct {
  int 		r_exit[NEXITS];        /* Exits */
  long int      bits[LOCLEN];          /* All room flags */ 
  char 		*r_name;               /* Zone name */
  char		*r_short;              /* Short description */
  char		*r_long;               /* Long desc */
  int		r_altitude;	       /* Altitude of room.  */
  int_set	objects;	       /* Set of objects in this loc. */
  int_set	mobiles;	       /* Players/mobs in this loc. */
  int_set	exits_to_me;	       /* Locations that have exits to this */
  Boolean	unlinked;              /* Loaded from disk but not linked */
  int		zone;		       /* To which zone do we belong ? */
  intptr_t      r_exit_reset[NEXITS];  /* reset exit index or pending link */
  int		r_altitude_reset;      /* Altitude reset value */
} Location;

/* An object */

typedef struct {

/* Unalterable, no need to reset on a reset: */

  char          *ozname;        /* objects zone-set name */
  char		*oname;		/* objects name */
  char		*oaltname;	/* objects alternate name */
  char		*odesc[4];	/* descriptions for each state  */
  char		*oexam_text;	/* examine for in-game objects */
  intptr_t      linked;        /* linked object or pending link string */
  int		omaxstate;	/* max state a wiz can SET an obj to */
  int		zone;		/* To which zone do we belong? */
  Boolean       unlinked;       /* loaded from disk but not linked yet */
  int_set	objects;	/* set of objects inside this object */

  int		ovalue;		/* base value */
  int		osize;
  int           oweight;
  int		oloc;
  int		ovis;		/* the objects visibility level */
  int		odamage;
  int		oarmor;
  int		ocarrf;
  int		ostate;
  long int 	bits[OLEN];

  intptr_t      oloc_reset;
  int		osize_reset;
  int		ovalue_reset;
  int		ovis_reset;
  int		odamage_reset;
  int		oarmor_reset;
  int		ocarrf_reset;
  int		ostate_reset;
  int           oweight_reset;
} OBJECT;

/* A record describing a player or mobile in the world. */

typedef struct {
  time_t        death_time;     /* time the player/mobile died */
  Body          body;           /* the player/mobile's body */
  char          *pzname;        /* the zone-name of the mob */
  char		pname[MNAME_LEN+1];
  int           ploc;
  char 		*phome;		/* players start-loc. and home */
  int           angry;          /* who are we angry at? */
  int		pdam;		/* Base damage to be added to */
  int           pac;            /* total AC of items worn */
  int		parmor;         /* Base armor to be added to */
  int		pagg;		/* Agression */
  int		pspeed;		/* Speed */
  int		pstr;           /* Hit points (strength) */
  int           planguage;      /* Language Selected */
  int		zone;		/* To which zone do we belong ? */
  int           class;          /* Class of player or mobile */
  int		pwimpy;
  int		pvis;
  int		plev;
  int		pweapon;
  int		psitting;
  int		phelping;
  int		pfighting;
  long int 	bits[MLEN];
  unsigned int	pscore;
  char		*pftxt;		/* Mobile's one-line description */
  char		*p_exam;	/* exam-text for mobs (may also be in DESC)*/
  int_set	objects;	/* set of objects carried by this character */
  Boolean       ftimer;         /* fighting timer */
  Boolean       unlinked;       /* Loaded from disk but not linked yet */

/* Reset data for mobiles: */

  char		*pname_reset;
  intptr_t      ploc_reset;
  int		pstr_reset;
  int		pvis_reset;
  int		plev_reset;
  int		pagg_reset;	/* Agression */
  int		pspeed_reset;	/* Speed */
  int		pdam_reset;
  int		parmor_reset;
  int		pwimpy_reset;
} UBLOCK_REC;

typedef UBLOCK_REC Mobile;
typedef UBLOCK_REC Player;

/* stuff to preserve over a reboot */

typedef struct _reboot {
  int           aliasto;                       /* number of mobile aliased */
  int           replyplr;                      /* reply messages to        */
  Boolean       isawiz;                        /* wizard?                  */
  Boolean       ismonitored;                   /* monitored?               */
  Boolean       aliased;                       /* Are we aliased?          */
  Boolean       linkdead;		       /* Player has lost connect  */
  int           polymorphed;                   /* Polymorphed turns        */
  int           pretend;                       /* Who aliased or poly to   */
  int           i_follow;                      /* following plr or -1      */
  int           me_ivct;                       /* Invisible for #cmds       */
  int           phelping;                      /* Who are we helping?       */
  int           snooped;                       /* How many are snooping us? */
  int           snooptarget;                   /* Who are we snooping?    */
  int           rounds_in_room;                /* How long in room?       */
  int           pconverse;                     /* Player conversing with  */
  int           forget[10];                    /* Forget list             */
  int           storage[NUM_STORE_SLOTS];      /* storage system          */
  Boolean       newplr;                        /* Are we new?             */
  Boolean       logged;                        /* Are we logged?          */
  Boolean       needsquests;                   /* Needs quests to wiz     */
  int           oldscore;                      /* Log-in score            */
  time_t        last_cmd;                      /* "last" command          */
  time_t        rlast_cmd;                     /* Real last command       */
  time_t        logged_on;                     /* Time we logged on       */
  int           fil_des;                       /* File descriptor         */
  int           port;                          /* Port we are using       */
  char          os[20];                        /* OS of connecting mach   */
  char          keyb[20];                      /* Keyboard type of mach   */
  char          ip_addr[16];                   /* Numeric IP addy         */
  char          hostname[100];                 /* Alphanumeric hostname   */
  char          usrname[200];                  /* Username@Host  (ident)  */

/* things which are copied from other structures before reboot finishes */

  int           plx;                           /* player's number         */
  int           ploc;                          /* players location        */
  int           pvis;                          /* players visibility      */
  int           grouped;
  int           pweapon;                       /* player's weapon         */
  char          pname[MNAME_LEN+1];            /* player's name           */
  char          *atarget;                      /* aberchat reply target   */
  char          *amud;                         /* aberchat reply mud      */
} RPLR_REC;

typedef struct _iorec {
  INP_HANDLER   *inp_handler;                  /* Func to pass input to   */
  char          inp_buffer[MAX_COM_LEN];       /* Read buffer             */
  Boolean       output;                        /* have output?            */
  size_t        out_size;                      /* size of the output buffer */
  char          *inp_ptr;                      /* player's position in inbuf */
  Boolean       ignore_input;                  /* status of input buffer    */
  unsigned char *out_buffer;                   /* this player's output buff */
  unsigned char *out_write;                    /* current writepos /outbuf  */
  unsigned char *out_read;                     /* current readpos /outbuf   */
} IO_REC;

/* Player data that are not shared by mobiles. */

typedef struct _a_player {
  Boolean       is_conn;                       /* Player is connected      */
  Boolean       iamon;                         /* Am I on? Init to false.  */
  Boolean       ident;                         /* received ident username  */
  Boolean       resolved;                      /* received DNS hostname    */
  Boolean       hasquit;                       /* End this connection?     */
  void          (*old_inp_handler)(char *str); /* old input handler for mail */
  void          (*edit_inp_handler)(char *str);/* old input handler for edit */
  char          edit_outfile[256];             /* where the edit output goes */
  int           edit_mode;                     /* the mode for output file */
  int           edit_lines;                    /* number of lines in buffer */
  int           edit_status;                   /* status of editor        */
  int           edit_curr_line;                /* current line in editor  */
  char          edit_prompt[64];               /* editor prompt           */
  int           resfd;                         /* FD for resolver         */
  int           respos;                        /* Position for resolv snd */
  int           dnsfd;                         /* FD for DNS lookup       */
  int           dnspos;                        /* Position for dns snd    */
  Boolean       limbo;                         /* Plr is resolving/dnsing */
  int           pcarry;                        /* Carrying capacity       */
  char          *awaymsg;		       /* Your away message       */
  time_t        mortal_time;                   /* Time alive as mortal    */
  time_t        wiz_time;                      /* Time alive as wizard    */
  time_t        first_on;                      /* first connect           */
  time_t        time_on;                       /* On for how long?        */
  Boolean       in_cmd;                        /* are we execing a mud-cmd? */
  Boolean       cast_spell;                    /* cast a spell this round? */
  int           challenger;                    /* who will be fighting us */
  Boolean       inmailer;                      /* In mailer?              */
  int           coins;                         /* coin system             */
  int           crawl;                         /* Crawl timer             */
  Boolean	writer;	               	       /* Current writer */
  int		no_logins;	               /* No. of failed passwd    */
  intptr_t	work;		               /* General work value/pointer */
  Text_Ptr      txt_curr;                      /* current position in editor */
  Text_Ptr      txt_start;                     /* Start of the editor text  */
  Text_Ptr      tmp_ptr;                       /* Tmp ptr for the editor   */
  intptr_t     work2[64];                     /* More general storage     */
  int		isforce;	               /* Pl index of forcer       */
  Messageptr    cur_msg;                       /* Message ptr for curr mail*/
  Messageptr    first_msg;                     /* First mail message      */
  Messageptr    work_msg;                      /* Outgoing mail and such  */
  char		passwd[PASSWD_LEN];            /* Passwd                  */
  char		cprompt[PROMPT_LEN+30];        /* Color prompt            */
  char		old_cprompt[PROMPT_LEN+30];    /* Place to store old one */
  char          *wd_them;                      /* Them --> plr           */
  char		*prompt;                       /* prompt to build        */
  char		*setin;
  char 		*setout;
  char 		*setmin;
  char 		*setmout;
  char 		*setvin;
  char 		*setvout;
  char 		*setqin;
  char		*setqout;
  char		*setsit;
  char 		*setstand;
  char		*setsum;
  char		*setsumin;
  char		*setsumout;
  char          *prev_com;              /* last command                 */
  char          *wd_her;                /* Her-->plr                    */ 
  char          *wd_him;                /* Him-->plr                    */
  char          *wd_it;                 /* It -->item                   */
  char		ptitle[TITLE_LEN+1];    /* title, shows up in who       */
  int		pmagic;			/* Magic points			*/
  int		pchannel;		/* Chat channel			*/
  int		pkilled;		/* Number of mobiles killed	*/
  int		pdied;			/* Number of times died		*/
  SPELL_DURATION *duration;             /* Spell Duration               */
  PAGER         pager;                  /* Pager structure              */
  Boolean       inpager;                /* Is player in the pager?      */
  FILE          *log;                   /* Logfile for player           */
  Boolean	user_ban;		/* User/Who at login ban	*/
  time_t       	last_on;
} PLAYER_REC;

typedef struct _persona {
  UBLOCK_REC ublock;
  PLAYER_REC player;
  RPLR_REC rplr;
} PERSONA;

#ifndef XR_WEATHER
/* Weather and climate structure
 */
typedef struct _climate {
  int		weather;
  Boolean	daytime;
  int		time;
  int		day;
  int		month;
  int		season;
} CLIMATE_REC;
#else

typedef struct _xrweather {

               /* Global weather stuff */
  Boolean sunny;      /* Is it sunny or cloudy?  :-) */
  int windspeed;      /* How fast is the wind blowing? */
  int rain;           /* How hard it is raining.  Lower is harder */
  int timer;          /* total timer (ticks) since bootup */
  int counter;        /* Counter for how often to update weather */
  int last_hurricane; /* Number of ticks since last hurricane */

                      /* Weather pattern stuff */
  int weatherlen; /* Tick number that the current weather ends (sunny/cloudy) */
  int bottom;     /* Lowest point that the var "rain" will reach this spell */
  int duration;   /* Time that the current period of RAIN ends (ticks) */
  int standby;    /* Tick number it will start to decline */
  int decline;    /* Rain is hardest from end of decline to end of steady. */
  int steady;     /* Tick number it will start to go from bottom -> 10000 */

} XRWEATHER_REC;

#endif

typedef struct _blist {
    char          *name;
    struct _blist *next;
  } Blist;

  
/* The world. Contains some global variables  */
typedef struct _a_world {
  Blist         *bchars;
  Blist         *bhosts;

  int		w_msg_low;
  int		w_msg_high;
  int		w_lock;
  int		w_mob_stop;
  int		w_peace;
  int		w_max_users;
  int		w_tournament;
  long int      wbits[WLEN];
  long int 	pbits[LEV_CREATOR+1][PFLLEN];
} WORLD_REC;

struct action {
  char *name;
  char *toall;
  char *tome;
  char *totarget;
  char *tosender;
  char *toothers;
  int flags;
  struct action *nxt;
};

typedef struct action Action;
typedef Action *Actionptr;

#endif
