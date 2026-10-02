#ifndef _ABERCHAT_INTERFACE_H_
#define _ABERCHAT_INTERFACE_H_
#include <time.h>
#include "sflags.h"
#include "levels.h"
#include "verbs.h"

Boolean aberchat_shutdown(void);
int     aberchat_boot(void);
void    aberchat_readpacket(int);
Boolean aberchat_parse(int verb);
void    aberchat_newconn(int slot);
void    aberchat_writepacket(int);
void    aberchat_autorecon(void);
void	aberchat_socketdetails(time_t now);

#endif
