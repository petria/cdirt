#ifndef _UPDATE_H
#define _UPDATE_H

#define WIZ_BUFF_LEN 300
#define FLUSH_TIMEOUT 5

typedef struct _reboot_record {
  char		version[sizeof(VERSION)];
  int		main_socket;
  time_t	last_startup;
  int		numresets;
  int		numreboots;
  int		numcrashes;
  time_t	last_reset;
  int		climate_time;
  int		climate_day;
  int		climate_month;
  int		nplayers;
#ifdef IO_STATS
  int           bytes_sent;
  int           bytes_read;
  int           unres_hosts;
  int           sock_conns;
  int           cut_conns;
#endif
  long int      bits[WLEN];
} REBOOT_REC;

int     xmain_reboot (void);
void	run_reboot (Boolean crash, Boolean will_update);
void	rebootcom (void);
void	updatecom (void);
void    dump_world (void);
void    initialize_slot (int);
void    update_world (int);
void    store_locs (void);
void    load_locs (void);
void    store_mobs (void);
void    load_mobs (void);
void    store_objs (void);
void    load_objs (void);
void    sig_handler (int);
void    setsignals(void);

extern int 	aberfd;
extern int 	dnsfd;
extern int	main_socket;
extern int	bytes_read;
extern int	bytes_sent;
extern int	unres_hosts;
extern int	sock_conns;
extern int	cut_conns;
extern Boolean	clear_syslog_file;
extern int	old_proc_num;
extern int      update;

extern char	*Locflags[];
#define F_OK 0

#endif

