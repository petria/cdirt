#ifndef _WIZLIST_H
#define _WIZLIST_H

#include <stdint.h>

void	set_wizfile(char *f);
int	boot_wizlist(void);
void	dump_wizlist(void);
void	update_wizlist(char *name, int new_wlevel);
int	parse_wizlevel(char *s,intptr_t *low,intptr_t *high);
void	wizlistcom(void);

#endif
