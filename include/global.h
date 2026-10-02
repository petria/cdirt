#ifndef _GLOBAL_H
#define _GLOBAL_H

FILE *log_fl;


char	**messages;
char	**verbtxt;
int	levels[LVL_WIZARD + 1];
char    nserv_addr[100];

char			*progname;
char			**envp;

Location        *room_data;
OBJECT          *objs;
ZONE            *zones;
UBLOCK_REC      *ublock;
PLAYER_REC      *players;
RPLR_REC        *rplrs;
IO_REC          *iplrs;
WORLD_REC       the_world_rec;
WORLD_REC	*the_world = &the_world_rec;
XRWEATHER_REC	global_weather_record;
XRWEATHER_REC	*global_weather = &global_weather_record;

int	    mynum;          /* current player slot-number */
int         real_mynum;     /* real mynum if mynum is fake due to aliasing */
int         quit_list;      /* real mynum of player to quit */
PLAYER_REC *cur_player;     /* Current player info. */
UBLOCK_REC *cur_ublock;     /* Current ublock info. */

int	max_players = 32;   /* Max number of players */
int     num_const_chars;    /* Number of non-cloned mobiles */
int	numchars;           /* Number of players + mobiles */

int	num_const_obs;      /* Number of non-cloned objects */
int	numobs;		    /* Number of objects in the game */

int     numzon;		    /* Number of zones in the world */
int     num_const_zon;      /* Number of non-cloned zones */

int     num_const_locs;     /* Number of non-cloned locations */
int	numloc;		    /* Number of locations */

int	*verbnum;

char	item1[MAX_COM_LEN];		/* first non-preposition word */
char	item2[MAX_COM_LEN];		/* second word */
char	item3[MAX_COM_LEN];		/* third word */
char	strbuf[MAX_COM_LEN];		/* the entire buffer */
char	wordbuf[MAX_COM_LEN];		/* word stored after brkword() */

int	ob1;            /* first obj assigned by parser (excluding preps) */
int	ob2;            /* second obj*/
int     ob3;            /* third obj */
int	pl1;            /* first mob assigned by parser (exlucing preps) */
int	pl2;            /* second mob */
int	pl3;            /* third mob */
char	*txt1;          /* text from first word on */
char	*txt2;		/* text from second word on */
char    *txt3;		/* text from third word on */

int	pptr;		/* The parameter pointer --- ??? */
int	prep;           /* prep -? */

int	stp;
int	verbcode;
time_t	next_event;     /* check mud.c */
time_t	last_reset;     /* Last reset time */
time_t	global_clock;   /* current time */
time_t  last_startup;   /* Last startup time */
time_t  last_healall;   /* Last healall */
time_t  last_autosave;  /* Autosave counter */
int     numresets;      /* Number of resets */
int     numreboots;     /* Number of reboots */
int     numcrashes;     /* Number of crashes */
long    tourn_prize;    /* Tournament Prize */

Boolean norun;          /* Do not allow players to go anywhere */
Boolean	breset;         /* Reboot the game on the next reset   */
Boolean crashing;       /* Game is crashing                    */
Boolean updating;       /* Game is in the process of updating  */
Actionptr actions;      /* List of actions */

int       sock_fds[MAX_FDS] = {-1};		    /* socket fd index */
T_elemptr verb_t;				    /* verbs */
FILE      *logptr;                                  /* b&w mudlog */
FILE      *clogptr;                                 /* color mudlog */

#ifdef LOG_INPUT
  FILE    *plogptr;				    /* player logfile */
#endif

#ifdef BOB
  FILE *dfile = NULL;				    /* bob dictionary file */
#endif

fd_set input_set, output_set, exception_set;        /* io stuff from main.c */
Boolean clear_syslog_file = CLEAR_SYSLOG;
Boolean stay_foreground   = FOREGROUND;
Boolean sig_term_happened = False;
Boolean dns_output        = False;
Boolean kill_other_mud    = KILL_MUD;
Boolean quiet             = QUIET;
char *data_dir            = DATA_DIR;
int mud_port              = PORT;
int old_proc_num          = 0;
int update                = 0;
int main_socket           = -1;
int width                 = 0;

#ifdef IO_STATS
  int bytes_sent          = 0;                   /* total bytes sent */
  int bytes_read          = 0;                   /* total bytes read */
  int unres_hosts         = 0;                   /* unresolved hosts */
  int cut_conns           = 0;                   /* cut connections */
  int sock_conns          = 0;                   /* total connections */
#endif

#endif

