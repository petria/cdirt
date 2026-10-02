#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "kernel.h"
#include "bprintf.h"
#include "mud.h"
#include "log.h"

void enter_game(int);
void send_ident(int);
void read_ident(int);
void send_dns(int);
void read_dns(int);
void ident_player(void);
