#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "kernel.h"

#define NSERV_PORT 9001

int dnsfd = -1;
extern Boolean dns_output;
extern int unres_hosts;

void dnsboot(void) {
  dnsfd = makesock(_IPNAME_, NSERV_PORT);  
}

void send_dns(int fd) {
  static char dnsbuff[100];
  static char *bufloc = dnsbuff;
  int plx, nwrote, num_rslv;

  if (bufloc == dnsbuff) {                       /* grab next plr for dns */
    for (plx = 0 ; plx < max_players ; plx++)
      if (is_conn(plx) && !resolved(plx))
        break;

    sprintf(dnsbuff, "%-3d%-96s", plx, ip_addr(plx));
  }

  nwrote = write(fd, bufloc, 100 - (bufloc - dnsbuff));

  if (nwrote + (bufloc - dnsbuff) == 100) {     /* wrote 1 record */
    bufloc = dnsbuff;
    num_rslv = 0;

    for (plx = 0 ; plx < max_players ; plx++)   /* more to resolve? */
      if (is_conn(plx) && !resolved(plx))
        num_rslv++;

    if (num_rslv == 1)
      dns_output = False;
  }
  else
    bufloc += nwrote;
}

#ifdef IO_STATS
Boolean is_ipaddr(char *str) {
  char *p;

  for (p = str ; *p ; p++)
    if (!isdigit(*p) && *p != '.')
      return(False);

  return(True);
}
#endif

void read_dns(int fd)  {
  static char readbuff[100];
  static char *bufloc = readbuff;
  char hostbuff[100];
  int plx, n_read;

  if ((n_read = read(fd, bufloc, 100 - (bufloc - readbuff))) < 1) {
    close(dnsfd);
    dnsfd = -1;
    return;
  }

  if (n_read + (bufloc - readbuff) == 100) { 
    sscanf(readbuff, "%d %s", &plx, hostbuff);
    strcpy(hostname(plx), hostbuff);
    if (!strcmp(username(plx), ip_addr(plx)))
      strcpy(username(plx), hostbuff);

  #ifdef IO_STATS
    if (is_ipaddr(hostname(plx)))
      unres_hosts++;
  #endif

    resolved(plx) = True;
    bufloc = readbuff;
  }
  else
    bufloc += n_read;
}
