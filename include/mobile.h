#ifndef _MOBILE_H
#define _MOBILE_H

char	*make_prompt(char *b, char *s, char *h, char *c, 
		     char *l, char *n, char *m);

int	cmp_player(const void *a, const void *b);
void    check_player(int);
void	usercom(void);
void	whocom(void);
void	whoncom(void);
void	mwhocom(void);
void	mobilecom(void);
void    timestatscom(void);
int     armor_avg(int);

void    causefight(int);
int	player_damage(int player);
int	player_armor(int player);
Boolean	check_armor(int plx, int obj);

void    loop_all_players(void);
void	list_people(void);
void	onlook(void);
void	do_follow (void);
void	check_follow (void);
void	dorune(int plx);
Boolean	dragget(void);
void	helpchkr(int plx);
void	movemob(int x);
void	stopcom(void);
void	startcom(void);
Boolean	stealstuff(int m);
void	shiftstuff(int m);
char	*xname(char *n);
void	setname(int plx);
Boolean	see_player(int pla, int plb);
Boolean	seeplayer(int plx);
char	*see_name(int pla, int plb);
int	find_mob(Boolean, char *);
int	find_mob_by_name(Boolean, char *name);
int	find_player(char *name, PERSONA *p, Boolean *f);
int	alive(int i);
int	wlevel(int lev);
Boolean	do_okay_l( int p, int v, Boolean c );
Boolean	do_okay( int p, int v, int prot_flag );
void	setpsex(int chr, Boolean v);
void	setploc(int plr, int room);
int	ptothlp(int pl);
void	destroy_mobile(int mob);
char	*make_title(char *title, char *name);
char	*make_rank(int player);
char	*std_title(int, Boolean, int);
Boolean	reset_mobile(int mobile);
void    quit_player(int);
void    quit_msg(char *, char *);
char	*lev2s(char *b,int lvl);
int	tscale(void);
Boolean	chkdumb(void);
Boolean	chkcrip(void);
Boolean	chksitting(void);
void	calib_player(int pl);
void    calib_player_without_skills(int pl);
void	calibme(void);
int	levelof(int score,int lev);
Boolean	check_setin(char *s, Boolean d, Boolean v);
Boolean	check_nooracle(int plx);
Boolean	check_busy(int plx);
Boolean	check_coding(int plx);
Boolean	check_away(int plx);
Boolean	check_forget(int p1, int p2);
void	wipe_forget(int plr);
char	*build_prompt(int plx);
int	vicf2(int fl,int chance);
int	vichere(void);
int	vicbase(void);
void	jumpcom(void);
void	special_events(int player);
void	regenerate(void);
void	set_quest(int plx, int quest);
void	dopouncie(int plx);
extern  char *levnam(int, int, Boolean);
void    set_msg (char **, Boolean, Boolean);
void	showplayer(void);

/* Values for crapup */
#define CRAP_SAVE    0x01
#define CRAP_RETURN  0x02
#define CRAP_UNALIAS 0x04

#define SAVE_ME   CRAP_SAVE
#define NO_SAVE   0

/* Values for vicf2 */
#define     SPELL_PEACEFUL 0
#define     SPELL_VIOLENT  1
#define     SPELL_REFLECTS 2

/* Values for special_events() */
#define     SP_ALL (-1)
#include <time.h>

#endif
