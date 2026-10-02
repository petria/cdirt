#include "kernel.h"
#include "idlookup.h"
#include <stdlib.h>

#define IDENT_BUFFSIZE 8192

int find_res_index(int fd) {
  int i;

  for (i = 0 ; i < max_players ; i++)
    if (players[i].resfd == fd)
      return(i);
  return(-1);
}

void ident_player(void) {
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

  if (strcmp(username(mynum), hostname(mynum)))
    sprintf(username(mynum), "%s@%s", username(mynum), hostname(mynum));

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
  setup_globals(plx);

  sprintf(buffer, "%d , %d\n", port(mynum), mud_port);
  pos = buffer + cur_player->respos;

  nwrote = write(fd, pos, strlen(pos) + 1);
 
  if (nwrote == strlen(pos) + 1 || nwrote < 1)
    cur_player->respos = -1;              /* disable re-sending of ident */
  else
    cur_player->respos = nwrote;
}

void read_ident(int fd)  {
  char reply_type[81], opsys_or_error[81], identifier[1024];
  char charset[81], opsys[81], readbuff[8192], *ptr;
  int port, mport, plx;

  *opsys = *charset = 0; 
  plx = find_res_index(fd);
  setup_globals(plx);

  ident(plx) = True;

  if (read(fd, readbuff, 8192) < 1)
    return;

  if (sscanf(readbuff, " %d , %d : %[^ \t\n\r:] : %[^\t\n\r:] : %[^\n\r]",
     &port, &mport, reply_type, opsys_or_error, identifier) < 3) {
    return;
  }

  if (sscanf(opsys_or_error, " %s , %s", opsys, charset) != 2)
    strcpy(opsys, opsys_or_error);
  
  if (strcasecmp(reply_type, "USERID"))
    return;
      
  if (*identifier) {
    if ((ptr = strchr(identifier, ','))) /* fix bug for some idents */
      *ptr = 0;
    strcpy(username(mynum), identifier);
  }
  strcpy(os(mynum), opsys);
  strcpy(keyboard(mynum), charset);
}
