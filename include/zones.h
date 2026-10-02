#ifndef _ZONES_H
#define _ZONES_H

int	get_zone_by_name(char *zname);
int	get_wizzone_by_name(char *name);
int	loc2zone(int loc);
int	findzone(int loc, char *str);
int	getlocid(int z,int off);
int	getlocnum(char *zname,int off);
void	reset_zone(int z);
void	zonescom(void);
void	locationscom(void);

#endif
