#ifndef _EXTERN_H
#define _EXTERN_H

extern char     *pwait;
extern char     *qwait;

extern FILE	*log_fl;	/* NULL if no logging	*/
extern char    *Exits[];
extern char    *exits[];
extern char    *entertxt[];
extern char    *exittxt[];
extern char    *mobenter[];

extern Location	   *room_data;
extern OBJECT	   *objs;
extern UBLOCK_REC  *ublock;

extern char	**messages;
extern char	**verbtxt;
extern char	*txt1;
extern char	*txt2;
extern char     *txt3;
extern char	item1[];
extern char	item2[];
extern char     item3[];
extern char	*TF[];
extern char	*OO[];
extern int	levels[LVL_WIZARD + 1];
extern char	strbuf[MAX_COM_LEN];
extern char	wordbuf[MAX_COM_LEN];
extern char     nserv_addr[100];

extern char     		*progname;
extern char			*data_dir;
extern char			**envp;

extern ZONE             *zones;
extern PLAYER_REC       *players;
extern RPLR_REC         *rplrs;
extern IO_REC           *iplrs;

extern WORLD_REC        the_world_rec;
extern WORLD_REC	*the_world;

#ifndef XR_WEATHER
extern CLIMATE_REC	the_climate_rec;
extern CLIMATE_REC	*the_climate;
#else
extern XRWEATHER_REC	global_weather_record;
extern XRWEATHER_REC	*global_weather;
#endif
 
extern int	   mynum;         /* current player slot-number */
extern int         real_mynum;    /* real mynum, see global.h */
extern int         quit_list;     /* List of players to quit */
extern PLAYER_REC *cur_player;    /* Current player info. */
extern UBLOCK_REC *cur_ublock;    /* Current ublock info. */

extern int	max_players;
extern int      num_const_chars;
extern int	numchars;       /* Number of players + mobiles */

extern int	numobs;		/* Number of objects in the game */
extern int	num_const_obs;	/* Number of constant (not created in-game) */

extern int      numzon;		/* Number of zones in the world		*/
extern int      num_const_zon;

extern int	numloc;		/* Number of locations */
extern int      num_const_locs;
extern int      loc_array_len;

extern long int id_counter;
extern int_table  id_table;

extern int	*verbnum;
extern int	ob1;
extern int	ob2;
extern int      ob3;
extern int	pl1;
extern int	pl2;
extern int      pl3;
extern int	pptr;		/* The parameter pointer		*/
extern int	prep;

extern int	stp;
extern int	verbcode;
extern time_t   next_event;     /* check mud.c */
extern time_t   last_reset;
extern time_t   global_clock;
extern time_t   last_startup;
extern time_t   last_healall;
extern time_t   last_autosave;
extern int      numresets;
extern int      numreboots;
extern int      numcrashes;
extern Boolean  breset;
extern Boolean  norun;
extern Boolean  updating;

extern char *MLevels[]; /* Male mortal level names */
extern char *FLevels[]; /* Female mortal level names */
extern QUEST Quests[];

extern int mud_port;
extern int sock_fds[MAX_FDS];
extern T_elemptr verb_t;                        /* this tree contains verbs */
extern FILE *logptr;                            /* b&w syslog file */
extern FILE *clogptr;                           /* color syslog file */
extern FILE *plogptr;				/* player syslog file */
extern Boolean quiet;                           /* don't print stuff on boot */
extern Boolean crashing;			/* game flushing/crashing */

extern Actionptr actions;                       /* list of actions */

#ifdef SAVE_HTML
  extern FILE *htmlptr;                         /* HTML player save file */
#endif
#ifdef BOB                                      /* Bob data file */
  extern FILE *dfile;
#endif

extern char *WarriorLevels[];
extern char *PriestLevels[];
extern char *ThiefLevels[];
extern char *MageLevels[];
extern char *MWizLevels[];
extern char *FWizLevels[];
extern char *Classes[];
extern int qlevels[];

extern fd_set input_set, output_set, exception_set;
extern Boolean clear_syslog_file;
extern Boolean stay_foreground;
extern Boolean sig_term_happened;
extern Boolean kill_other_mud;
extern Boolean quiet;
extern char *data_dir;
extern int mud_port;
extern int old_proc_num;
extern int update;
extern int main_socket;
extern int width;
extern Boolean dns_output;

#ifdef IO_STATS
  extern int bytes_sent;			/* total bytes sent */
  extern int bytes_read;                   	/* total bytes read */
  extern int unres_hosts;                   	/* unresolved hosts */
  extern int cut_conns;                  	/* cut connections */
  extern int sock_conns;                   	/* total connections */
#endif

#endif


