#ifndef _CLONE_H
#define _CLONE_H

int	clone_object(int obj, int new_zone, char *new_name, int loc , int carrf);
Boolean	destruct_object(int obj);
int	clone_mobile(int mob, int new_zone, char *new_name);
Boolean	destruct_mobile(int mob);
int	clone_location(int l, int new_zone, char *new_name);
Boolean	destruct_location(int l);
void	clonecom(void);
void    linkcom(void);
void	loadcom(void);
void    destruct_clones(int);
void	storecom(char *, Boolean);
void	destructcom(char *args);
void	maxstatecom(void);
void    erasezonecom(void);
#endif
