#ifndef _ABERCHAT_INTERNAL_H_
#define _ABERCHAT_INTERNAL_H_
#include <stdarg.h>
#include "config.h"
#include "client.h"

/* this is the address and port of the server, 
 * either mudmail Kroll@SMiLE  or email smile@mirai.net
 * and tell him/them:
 * 1. Your muds ip. e.g. 195.166.28.7   for SMiLE
 * 2. Your muds port. e.g. 6715 for SMiLE
 * 3. Your password for connection e.g. YourPassword (ha! bet you thought i'd
 *                                                   type in SMiLE's password!)
 * or, for dumb people:
 * tell them your ip,
 * the contents of #define PORT in config.h
 * the contents of #define PASS in client.h (just below)
 */

#define SERVER		"195.166.28.7"
#define SERVER_PORT	6969
#define PASS		ACHAT_PASS
#define MUDLIB		VERSION
#define MYMUD		MUD_NAME

/* this must be defined as the LOWEST level that isnt mortal,
 * basically if trying to use the player level array with this number 
 * as the first one that would crash things, if you havent a clue what this is
 * all about, you should be fine to leave it alone on idirt 1.82 ;0
 */
#define LVL_FIRST_NONMORTAL	LVL_WIZARD

/* this should be defined as the lowest level that should be allowed to use
 * the various admin type commands on your mud (e.g. akick, agod, ashutdown..)
 * some muds might want to set this lower, but a lot of people feel very
 * strongly about giving lower levels these commands
 */
#define LEVEL_ADMIN		LVL_CREATOR

/* shouldn't need to change anything below */

/* Ideally i'd prefer this not to be altered, as then i can tell which version
 * of the client people are using.
 */
#define PATCHLEVEL	"7"
#define CLIENT_VERSION	"AberChat Client 4.20(pl " PATCHLEVEL ")"

/* numerous functions ;) */

Boolean aberchat_filter(char, char *, char *, char *, char *);
void afilepagecom(char *);
Boolean abercheck(int plx, int type);

void set_areply(int, char *, char *);
void aprintf(char *, ...);
void abercmd(char, char *);
char *get_arg(char *);
void amudlist(char *);

void atell(char *, char *, char *, char *);
void kick_player(char *, char *, char *);
void amessage(char *, char *, char *);
void achat(int, char *, char *, char *);
void afilepage(char *, char *, char *);
void ainfo(char *, char *);
void aversion(char *, char *, char *);
void aver_reply(char *, char *);
void send_awho(char *, char *);
void receive_awho(char *);
void auth_received(Boolean);
void authenticate(void);

#define READLEN    4096
#define WRITELEN   4096
#define TO_MUD     '0'
#define TO_ALL     '1'
#define AUTH       '0'
#define MUDLIST    '1'
#define AECHO      '2'
#define TELL       '3'
#define CHAT       '4'
#define WHO        '5'
#define RWHO       '6'
#define INFOMSG    '7'
#define MSG        '8'
#define KICK       '9'
#define AVERSION   ':'
#define FILEPAGE   ';'
#define AGOD       '<'
#define ACODE      '='
#define AVERREQ    '>'

#define D       "\007"
#define DCHAR     007 
#define DASHES "&+b------------------------------------------------" \
               "-------------------------------\n"


#endif
