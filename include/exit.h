/* iDiRT Exit Handler */

#ifndef	_EXIT_H
#define _EXIT_H

#include "rooms.h"
#include "types.h"

void	__exit(int status);
void	sig_exit(char *sig, int signal);
void	autosave(void);
void	debug(void);
void	signalcom(void);
void    sigterm_exit(void);
#endif
