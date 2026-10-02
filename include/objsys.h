#ifndef _OBJSYS_H
#define _OBJSYS_H

Boolean has_key(int);
Boolean	ispit(register int o);
void    usecom(void);
void    unlockcom(void);
void    lockcom(void);
void	givecom(void);
void	dogive(int ob, int pl);
void	stealcom(void);
void    closecom(void);
void	wearcom(int, int);
void	removecom(void);
void	valuecom(void);
void	eatcom(void);
void	putcom(void);
void	inventory(void);
void	aobjsat(int loc, int mode, int marg);
int     findob(int plr, char *txt, int type, int cont);
Boolean	iscontin(int o1, int o2);
int	obj_loc(int obj);
void	getcom(void);
Boolean	iscarrby(int item, int user);
Boolean	in_inventory(int obj, int player);
void	dropobjcom(int);
void	dropinpit(int o);
void	list_objects(void);
void	dumpstuff(int n, int loc);
void	oplong(int x);
Boolean	gotanything(int x);
Boolean	cancarry(int plyr);
Boolean	reset_object(int o);
void	setobjstate(int o, int v);
void	destroy(int ob);
void	eat(int ob);
void	setoloc(int ob, int l, int c);
Boolean	hasobjtype(int, int);
Boolean	p_ishere(int plr,int item);
int	ovalue(int ob);
char	*xdesloc(char *b, int loc, int cf);
void	desloc(int loc, int cf);
Boolean	otstmask(int ob, int v);
Boolean	willhold(int x, int y);
int     seeplr(char*, char*);
int	seeplrhere(char*, char*);
int	cantake(int, char*, char*);
void	drop_some_objects(int plx);
void	desrm(int loc, int cf);
char	*xdesrm(char *b, int loc, int cf);
char	*odescrm(int obj);

Boolean is_antimagic(int obj);
Boolean wears_antimagic(int pl);
Boolean	carries_boat(int pl);
int	carries_obj_type(int pl, int type);
int	wears_obj_type(int pl, int type);

void	move_pouncie(void);
void	wearall(void);

extern void set_vital(int);
extern Boolean has_arm(int);
extern Boolean has_foot(int);
extern Boolean has_hand(int);
extern int take_bodypart(void);

#define O_BVALUE_MAX 10000
#define O_SIZE_MAX 30000
#define O_VIS_MAX LVL_CREATOR
#define O_DAM_MAX 100
#define O_ARMOR_MAX 100

#endif
