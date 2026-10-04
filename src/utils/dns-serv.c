/**************************************************
 * DNS Server 1.0  (C) 1997, Giancarlo Castrataro *
 **************************************************/

#include "dns-serv.h"
#include "../../include/MACHINE.H"
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
static FILE *logfp;

int daemonize(void);

char host_ip[100];

int main() {
  int main_sock, i, fd, numavail;
  fd_set input_set, output_set;
  struct hostent *h;
  int fdused;
  FILE   *fp;
  
  for (i = 0 ; i < MAX_CONNECT ; i++) 
    init_conn(i);

  if (daemonize() == -1) {  
    printf("Could not daemonize the Server.\n");
    return;
  }

  if ((main_sock = makesock()) == -1)
    return;           

  width = main_sock + 1;
	
  while (1) {
    FD_ZERO(&input_set);
    FD_ZERO(&output_set);
    FD_SET(main_sock, &input_set);

    for (i = 0 ; i < MAX_CONNECT ; i++) {
      if (fdesc(i) != -1) {
	FD_SET(fdesc(i), &input_set);
        if (output(i))
	  FD_SET(fdesc(i), &output_set);
      }
    }
    
    numavail = select(width, &input_set, &output_set, NULL, NULL);
    
    for (fd = 0 ; numavail > 0 ; fd++) {
      fdused = 0;

      if (FD_ISSET(fd, &input_set)) {
        if (fd == main_sock)
          new_conn(main_sock);
        else
          read_packet(fd);
        numavail--;
      }
      if (FD_ISSET(fd, &output_set)) {
        write_packet(fd);
        numavail--;
      }
    }
  }
}

void init_conn(int i) {
  output(i) = False;
  fdesc(i) = -1;
  *readbuff(i) = *writebuff(i) = 0;
  readpos(i) = readbuff(i);
  writepos(i) = writebuff(i);
}

int makesock(void) {
  int sock;
  char opt = 1;
  struct sockaddr_in sin;
  struct hostent *h;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("Unable to create socket.\n");
    return(-1);
  }

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
  if (!(h = gethostbyname(_HOSTNAME_))) {
    fprintf(stderr, "Unable to bind to your hostname. Run configure.\n");
    return -2;
  }

  sin.sin_family = AF_INET;
  sin.sin_port = htons (SERVER_PORT);
  bcopy (h->h_addr_list[0], &(sin.sin_addr), h->h_length);

  if (bind(sock, (struct sockaddr *) &sin, sizeof (sin)) == -1) {
    printf("Dns server already running [not an error].\n");
    return(-1);
  }
  listen(sock, 5);
  return(sock);
}

void write_packet(int fd) {
  ssize_t n;
  size_t len;
  conn = find_index(fd);
  if (conn < 0) return;
  len = strlen(writepos(conn)) + 1;
  n = write(fd, writepos(conn), len);
  if (n < 0) {
    if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) end_conn(fd);
    return;
  }
  if (!n) return;
  if ((size_t)n == len) {
    *writebuff(conn) = 0; writepos(conn) = writebuff(conn); output(conn) = False;
  } else writepos(conn) += n;
}
int escapechars(int conn) {
  char buff[READLEN * 2];
  char *p, *b;

  for (p = connects[conn].readbuff, b = buff ; *p ; p++, b++) {
    if (*p != '%')
      *b = *p;
    else {
      *b++ = '%';
      *b = '%';
    }
  }
  *b = 0;

  if (strlen(buff) >= READLEN)
    return(-1);
  else {
    strcpy(connects[conn].readbuff, buff);
    return(0);
  }
}

int ht_pos(char *s) {
  char *ptr;
  int sum = 0;  

  for (ptr = s ; *ptr ; ptr++)
    sum += *ptr;

  return(sum % HT_SIZE);
}

void ht_add(char *ip_addr, char *hostname) {
  ht_elem *elem;
  ht_elem *nelem;
  int i;

  i = ht_pos(ip_addr);

  nelem = memory_alloc(1, sizeof(ht_elem));
  nelem->next = NULL;
  nelem->ip_addr = COPY(ip_addr);
  nelem->hostname = COPY(hostname);

  if (!htable[i])
    htable[i] = nelem;
  else {
    elem = htable[i];
    while (elem->next) elem = elem->next;
    elem->next = nelem;
  }
}

char *match(char *token, ht_elem *start) {
  ht_elem *elem;

  elem = start;

  while (elem) {
    if (!strcmp(elem->ip_addr, token))
      return(elem->hostname);
    else
      elem = elem->next;
  }

  return(NULL);  
}

char *ht_lookup(char *token) {
  int i;

  i = ht_pos(token);
  return(match(token, htable[i]));
}

void read_packet(int fd) {
  int num_read, plrnum;
  unsigned long int inetnum;
  struct hostent *h;
  char ip_addr[16] = "\0";
  char *rslt;

  conn = find_index(fd);
  if (conn < 0) return;
  if (readpos(conn) - readbuff(conn) >= 100) { end_conn(fd); return; }
  num_read = read(fd, readpos(conn), 100 - (readpos(conn) - readbuff(conn)));
  if (num_read < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) return;
  if (num_read < 1) {
    end_conn(fd);
    return;
  }

  readpos(conn) += num_read;
  if (readpos(conn) - readbuff(conn) == 100) {
    if (readbuff(conn)[99]) { end_conn(fd); return; }

    readpos(conn) = readbuff(conn);

    if (escapechars(conn) == -1)
      return;

    plrnum = -1;
    sscanf(readbuff(conn), "%d%15s", &plrnum, ip_addr);

    if (!*ip_addr || plrnum == -1)
      return;
    else {
      if (!(rslt = ht_lookup(ip_addr))) {
        inetnum = inet_addr(ip_addr);
        h = gethostbyaddr((char *) &inetnum, sizeof(inetnum), AF_INET);
        if (h) {
          rslt = (char *) h->h_name;
          ht_add(ip_addr, rslt);
        }
        else
          rslt = ip_addr;
      } 
      snprintf(writebuff(conn), WRITELEN, "%-3d%-96.96s", plrnum, rslt);
      output(conn) = True;
    }
  }
}

int find_index(int fd) {
  int i;

  for (i = 0 ; i < MAX_CONNECT ; i++)
    if (fdesc(i) == fd)
      return(i);
  return(-1);
}

int find_new_index(int fd) { 
  int i;

  for (i = 0 ; i < MAX_CONNECT ; i++)
    if (fdesc(i) == -1) {
      fdesc(i) = fd;
      return(i);
    }

  return(-1);
}

void new_conn(int mainfd) {
  int i, fd, sin_len;
  struct sockaddr_in sin;
  struct hostent *h;

  bzero ((char *) &sin, sizeof (struct sockaddr_in));
  sin_len = sizeof (struct sockaddr_in);

  if ((fd = accept (mainfd, (struct sockaddr *) &sin, &sin_len)) < 0) {
    perror("accept()");
    return;
  }
  if (fcntl (fd, F_SETFL, FNDELAY) == -1) { 
    perror("fcntl()");
    return;
  }
  if (fd >= FD_SETSIZE || (i = find_new_index(fd)) == -1) { close(fd); return; }

  h = gethostbyaddr((char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
 
  if (!h)
    end_conn(fd);
  else if (strcmp(h->h_name, _HOSTNAME_) && strcmp(h->h_name, _IPNAME_))
    end_conn(fd);
   else if (fd >= width)
    width = fd + 1;
}

void end_conn(int fd) {
  char buff[200];
  int mud;

  if (fd >= width)
    width = fd;

  close(fd);

  mud = find_index(fd);
  if (mud >= 0) init_conn(mud);
}

/**************************************************************************
 ** Signal Handler routines.
 **************************************************************************/

void remove_connections(void)
{   int i;

    for (i = 0; i < width; i++)
    {   if (fdesc(i) != -1)
           close(fdesc(i));
    }
}

void ee_handler(int sig)
{
    remove_connections();
    exit(0);
}

void abort_handler(int sig)
{   char message[255];
    int  i;

    remove_connections();
    exit(0);
}

void set_sighandlers(void)
{   signal(SIGALRM,SIG_IGN);
    signal(SIGSEGV,ee_handler);
    signal(SIGBUS,ee_handler);
    signal(SIGABRT,abort_handler);
    signal(SIGQUIT,abort_handler);
    signal(SIGINT,abort_handler);
}

int daemonize(void)
{   int pid;

    set_sighandlers();

    switch((pid = fork())) {
    case 0 : break;
    case -1: perror("Deamonize()"); return -1;
    default:
	fflush(stdout);
	fflush(stderr);
        exit(0);		/* Die silently */
    }
    return 0;
}

