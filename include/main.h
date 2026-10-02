#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdarg.h>

#include "idlookup.h"
#include "client.h"
#include "mud.h"
#include "log.h"
#include "sendsys.h"
#include "mobile.h"
#include "commands.h"
#include "timing.h"
#include "exit.h"
#include "locations.h"
#include "update.h"
#include "climate.h"
#include "update.h"
#include "objsys.h"
#include "bprintf.h"
#include "zones.h"
#include "uaf.h"
#include "fight.h"
#include "rooms.h"
#include "locations.h"

void check_pid (void);
void do_packet (int, char *);
void rm_pid (void);
void write_packet (int fd);
void close_sock(int);
void setup_io(int, int);
void flush_mud(Boolean);
void main_loop (int listen_socket);
void save_pid(void);

int xmain (void);
int go_background (void);
void exec_cmd (int, char *);
void get_options (int argc, char **argv);
void usage (void);
void fullusage (void);
void new_connection (int fd);
void read_packet (int fd);

extern int aberfd;
extern Boolean auth;
extern Boolean aber_output;
extern int dnsfd;

extern void quit_player (int);
extern void scatter_potions (void);
extern void close_mudfiles (void);
extern int bootstrap (void);
