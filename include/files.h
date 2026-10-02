#ifndef _FILES_H
#define _FILES_H

#define DESC_DIR	"DESC"		/* Character descriptions	*/
#define MAIL_DIR	"XRMAIL"	/* Directory to keep mail	*/
#define INFO_DIR	"INFO"		/* Information files (*.i)	*/
#define QINFO_DIR	"QINFO"		/* quest info files (.qinfo)    */
#define HELP_DIR	"HELP"		/* Directory for help files	*/
#define BULL_DIR	"BULL"		/* Directory for bulletin files	*/
#define WIZ_ZONES       "WIZ_ZONES"     /* Wizard's personal creations	*/
#define	LOG_DIR		"LOGS"		/* Player logging directory	*/
#define POLICY_DIR	"POLICY"	/* Directory for policies	*/
#define	TEMP_DIR	"TMP"		/* Temporary data directory	*/

#define CRASH_UPDATE    "crashupdate"   /* File displayed on crash  upd */
#define UPDATE          "update"        /* File displayed on normal upd */
#define HOST_BANNED     "bannedmsg"     /* Message for a banned host    */
#define IP_BANNED       "bannedmsg"     /* Message for a banned ip      */
#define USERNAME_BANNED "bannedmsg"     /* Mesage for a banned user     */
#define PLAYER_BANNED   "playerbanmsg"  /* Message for a banned player  */
#define WELCOME		"title"		/* Title screen			*/
#define GOODBYE		"goodbye"	/* Exit screen (quit game)	*/
#define CRASHED         "crashed"       /* Thing to display on crash    */
#define BAN_HOSTS       "banned_hosts"  /* List of banned hosts         */
#define BAN_CHARS	"banned_chars"	/* List of banned characters	*/
#define BAN_ACCTS       "banned_accts"  /* Banned user@host             */
#define BAN_WHOS        "banned_whos"   /* Banned hosts to type who     */
#define ILLEGAL_FILE    "illegal"       /* Illegal character names	*/
#define SILENT_LOGIN	"silent_login"	/* Hosts w/out socket messages	*/
#define BOOTSTRAP	"bootstrap"	/* Points to text files		*/
#define CREDITS		"credits"	/* Author credits		*/
#define EXTERNS		"actions"	/* External actions		*/
#define OBJECTS		"objects"	/* Object names and desc	*/
#define GWIZ		"wiz.txt"	/* "Welcome to Wizard" message	*/
#define FULLHELP	"help"		/* Detailed command description	*/
#define HELP1		"help.mortal"	/* Commands for Mortals		*/
#define HELP2		"help.wizard"	/* Commands for Wizards		*/
#define HELP3		"help.prophet"	/* Commands for Prophets	*/
#define HELP4		"help.awiz"	/* Commands for ArchWizards	*/
#define HELP5		"help.advisor"	/* Commands for Advisors	*/
#define HELP6		"help.avatar"	/* Commands for Avatars		*/
#define HELP7		"help.upper"	/* Commands for Gods		*/
#define COUPLES		"couples"	/* Couples list			*/
#define INFO		"info"		/* Brief information on game	*/
#define QINFO		"qinfo"		/* Brief quest info  		*/
#define POLICY		"policy"	/* List of policies		*/
#define BULLETIN1	"bull.mortal"	/* Bulletins for Mortals	*/
#define BULLETIN2	"bull.wizard"	/* Bulletins for Wizards	*/
#define BULLETIN3	"bull.awiz"	/* Bulletins for AWiz/Advisor	*/
#define BULLETIN4	"bull.upper"	/* Bulletins for Avatar+	*/
#define LOG_FILE	"syslog"        /* The system's B&W log-file	*/
#define CLOG_FILE       "csyslog"       /* Color syslog file            */
#define PLAYER_LOGFILE  "psyslog"       /* Optional player logging      */
#define TEST_LOG_FILE   TEMP_DIR "/syslog"       /* Syslog file for temp mud */
#define TEST_CLOG_FILE  TEMP_DIR "/csyslog"      /* color syslog for tmp mud */
#define MOTD		"motd"		/* Message of the day		*/
#define ISSUE           "issue"		/* ANSI mud pic                 */
#define IMMORTAL_MOTD   "imotd"         /* Immortal motd                */
#define UPPER_MOTD      "umotd"         /* Upper motd                   */
#define UAF_RAND	"uaf_rand"      /* User database		*/
#define PRIVED_FILE     "prived"        /* Priveliged players		*/
#define MONITOR_CHAR    "monitor_chars" /* Players to monitor           */
#define MONITOR_HOST    "monitor_hosts" /* Hosts to monitor             */
#define PID_FILE        "pid"           /* PID of MUD Daemon running	*/
#define	HOSTS		"hosts"		/* Aliased hosts for MUD	*/
#define	SIGNAL1		"sigmsg1"	/* SIGUSR1 crash message	*/
#define	SIGNAL2		"sigmsg2"	/* SIGUSR2 crash message	*/
#define	FROBCHT		"frobcht"	/* Frobbing chart for Frob	*/
#define WEATHER_FILE    "weather_data"	/* Data file for weather	*/

#define ID_COUNTER      WIZ_ZONES"/ID_COUNTER"

#endif
