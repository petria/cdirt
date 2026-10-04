#include "kernel.h"
#include "idlookup.h"
#include <stdlib.h>
#include <errno.h>

#define IDENT_BUFFSIZE 8192

int find_res_index(int fd) {
  int i;

  for (i = 0 ; i < max_players ; i++)
    if (players[i].resfd == fd)
      return(i);
  return(-1);
}

void ident_player(void) {
  cur_player->ident_len = 0;
  cur_player->ident_reply[0] = 0;
  if ((cur_player->resfd = makesock(ip_addr(mynum), 113)) == -1)
    ident(mynum) = True;
}

void enter_game(int plx) {

  if (is_in_game(plx) || !is_conn(plx))
    return;

  setup_globals(plx);

  cur_player->limbo = False;

  if (cur_player->resfd != -1) {
    shutdown(cur_player->resfd, 2);
    close(cur_player->resfd);
    cur_player->resfd = -1;
  }

  if (strcmp(username(mynum), hostname(mynum))) {
    char *identity = text_format("%s@%s", username(mynum), hostname(mynum));
    if (strlen(identity) < sizeof(rplrs[mynum].usrname)) strcpy(username(mynum), identity);
    free(identity);
  }

  if (!is_host_silent(hostname(mynum)))
    mudlog("&+GCONNECT &+W[%d]: &N%s", fildes(mynum), username(mynum));
  sock_msg("Connect from &+C%H");
  new_player();
}

void send_ident(int fd) {
  char buffer[256];
  char *pos;
  int plx, nwrote;

  plx = find_res_index(fd);
  if (plx < 0) return;
  setup_globals(plx);

  sprintf(buffer, "%d , %d\n", port(mynum), mud_port);
  pos = buffer + cur_player->respos;

  nwrote = write(fd, pos, strlen(pos) + 1);

  if (nwrote == strlen(pos) + 1 || nwrote < 1)
    cur_player->respos = -1;              /* disable re-sending of ident */
  else
    cur_player->respos += nwrote;
}

void read_ident(int fd)  {
  char reply_type[81], opsys_or_error[81], identifier[1024];
  char charset[81], opsys[81], *readbuff, *ptr;
  int port, mport, plx, n;

  *opsys = *charset = *identifier = 0;
  plx = find_res_index(fd);
  if (plx < 0) return;
  setup_globals(plx);

  readbuff = cur_player->ident_reply;
  n = read(fd, readbuff + cur_player->ident_len,
           sizeof(cur_player->ident_reply) - cur_player->ident_len - 1);
  if (n < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) return;
  if (n <= 0) { ident(plx) = True; return; }
  cur_player->ident_len += n;
  readbuff[cur_player->ident_len] = 0;
  if (!strchr(readbuff, '\n') && !strchr(readbuff, '\r')) {
    if (cur_player->ident_len == sizeof(cur_player->ident_reply) - 1) ident(plx) = True;
    return;
  }
  ident(plx) = True;

  if (sscanf(readbuff, " %d , %d : %80[^ \t\n\r:] : %80[^\t\n\r:] : %1023[^\n\r]",
     &port, &mport, reply_type, opsys_or_error, identifier) < 3) {
    return;
  }

  if (sscanf(opsys_or_error, " %80s , %80s", opsys, charset) != 2)
    strcpy(opsys, opsys_or_error);

  if (strcasecmp(reply_type, "USERID"))
    return;

  if (*identifier) {
    if ((ptr = strchr(identifier, ','))) /* fix bug for some idents */
      *ptr = 0;
    if (strlen(identifier) < sizeof(rplrs[mynum].usrname)) strcpy(username(mynum), identifier);
  }
  if (strlen(opsys) < sizeof(rplrs[mynum].os)) strcpy(os(mynum), opsys);
  if (strlen(charset) < sizeof(rplrs[mynum].keyb)) strcpy(keyboard(mynum), charset);
}
