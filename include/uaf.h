#ifndef _UAF_H
#define _UAF_H

void	get_gender(char *gen);
void	pers2player(PERSONA *d,int plr);
void	player2pers(PERSONA *d,time_t *last_on,int plr);
void	get_gender(char *gen);
void	saveother(void);
void	saveme(Boolean);
void	saveallcom(void);
int     save_load_plr(PERSONA *, char *, Boolean, Boolean);
int	deluaf(char *name);
int	getuafinfo(char *name);
char	*build_setin(int type, char *b, char *s, char *n, char *d, char *v);
char	*ipname(int plr);
void    free_player(void);
extern  void store_parts(FILE *, PERSONA *);
extern  void load_parts(char *, PERSONA *);

/* Types for build_setin() */
#define SETIN_SETIN 	0
#define SETIN_SETOUT 	1
#define SETIN_SETMIN 	2
#define SETIN_SETMOUT 	3
#define SETIN_SETVIN	4
#define SETIN_SETVOUT	5
#define SETIN_SETQIN	6
#define SETIN_SETQOUT	7
#define SETIN_SETSIT	8
#define SETIN_SETSTAND	9
#define SETIN_SETSUM	10
#define SETIN_SETSUMIN	11
#define SETIN_SETSUMOUT	12

#endif
