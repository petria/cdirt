#ifndef _LOG_H
#define _LOG_H

#include <stdarg.h>

int	open_logfile(Boolean clear_flag);
void	close_logfile(void);
void	progerror(char *name);
void	vmudlog( char *format, va_list pvar);
void	mudlog( char *format, ...);
void    open_plr_log(void);
void    close_plr_log(void);
void    write_plr_log(char *);
void    close_mudfiles(void);
#endif
