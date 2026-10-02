#ifndef _MUD_H
#define _MUD_H

Boolean login_ok (char *name);
void    talker (void);
void    remove_from_game(void);
void    close_sock(int);
void    get_color(char *);
void    get_class(char *);
void    check_files(void);
void	sock_msg(char *format, ...);
void	push_input_handler(void (*h)(char *str));
void	pop_input_handler(void);
void	replace_input_handler(void (*h)(char *str));
int	find_free_player_slot(void);
int	find_pl_index(int fd);
void	setup_globals(int plx);
void	new_player(void);
void	get_command(char *cmd);
void	quit_player(int);
void	enter_vis(char *v);
void	do_motd(char *cont);
void	do_issue(char *cont);
Boolean check_host_bans(void);

void    get_pname1(char *name);
void    get_pname2(char *reply);
void    get_new_pass1(char *pass);
void    get_new_pass2(char *pass);
void    get_passwd1(char *pass);
void    kick_out_yn(char *answer);

char *show_user(int, int, char *);
char *show_host(int, int, char *);

#define NORMAL_QUIT     0
#define CONNECTION_CUT -1
#define RE_LOGIN       -2
#define INPUT_LOST     -3
#define MUD_UPDATE     -4
#define TOUT           -5
#endif
