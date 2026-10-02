#ifndef _PARSE_H
#define _PARSE_H

void	gamecom(char *str,Boolean savecom);
char	*getreinput(char *b);
void    doverb (int vb);
int	brkword(void);
int	my_brkword(void);
int	chkverb(void);
int	chklist(char *word, char *lista[], int listb[]);
int	Match(char *x, char *y);
void	doaction(int n);
void	quit_game(void);
void	erreval(void);
Boolean	parse_2(int vb);
Boolean allspaces(char *);
int	findprep(char *t);

extern void wallcom(char *);
extern void muserscom(void);
extern void iuserscom(void);
extern void qlistcom(void);
extern void tiecom(void);
extern void untiecom(void);
extern void opencom(void);
extern void throwcom(void);
extern void digcom(void);
extern void configurecom(void);
extern void editcom(void);
extern void votecom(char *);
extern void pigcom(void);
void awaymsgcom(void);
void qinfocom(void);
void lastoncom(void);
void emoteallcom(void);
void fdlistcom(FILE *);
void bongcom(void);

#ifdef BOB
extern void bobcom (char *, int, Boolean);
#endif

#endif
