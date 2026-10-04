#include "kernel.h"
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#define NSERV_PORT 9001
int dnsfd = -1;
extern Boolean dns_output;
extern int unres_hosts;
static char dns_send[100], dns_read[100];
static size_t send_offset, read_offset;
static void dns_close(void) {
  if (dnsfd >= 0) close(dnsfd);
  dnsfd = -1; send_offset = read_offset = 0; dns_output = False;
}
void dnsboot(void) {
  send_offset = read_offset = 0;
  dnsfd = makesock(_IPNAME_, NSERV_PORT);
}
void send_dns(int fd) {
  int plx, remaining = 0;
  ssize_t n;
  if (!send_offset) {
    for (plx = 0; plx < max_players; plx++)
      if (is_conn(plx) && !resolved(plx)) break;
    if (plx == max_players) { dns_output = False; return; }
    memset(dns_send, 0, sizeof(dns_send));
    if (snprintf(dns_send, sizeof(dns_send), "%-3d%-96s", plx, ip_addr(plx)) >= sizeof(dns_send)) {
      dns_close(); return;
    }
  }
  n = write(fd, dns_send + send_offset, sizeof(dns_send) - send_offset);
  if (n < 0) { if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) dns_close(); return; }
  if (!n) return;
  send_offset += n;
  if (send_offset == sizeof(dns_send)) {
    send_offset = 0;
    for (plx = 0; plx < max_players; plx++)
      if (is_conn(plx) && !resolved(plx)) remaining++;
    if (remaining <= 1) dns_output = False;
  }
}
#ifdef IO_STATS
Boolean is_ipaddr(char *str) {
  for (; *str; str++) if (!isdigit((unsigned char)*str) && *str != '.') return False;
  return True;
}
#endif
void read_dns(int fd) {
  ssize_t n;
  int plx;
  char host[100];
  n = read(fd, dns_read + read_offset, sizeof(dns_read) - read_offset);
  if (n < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) return;
  if (n <= 0) { dns_close(); return; }
  read_offset += n;
  if (read_offset != sizeof(dns_read)) return;
  read_offset = 0;
  if (dns_read[99] || sscanf(dns_read, "%d %99s", &plx, host) != 2 ||
      plx < 0 || plx >= max_players || !is_conn(plx)) return;
  strcpy(hostname(plx), host);
  if (!strcmp(username(plx), ip_addr(plx))) strcpy(username(plx), host);
#ifdef IO_STATS
  if (is_ipaddr(hostname(plx))) unres_hosts++;
#endif
  resolved(plx) = True;
}
