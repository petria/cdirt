#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#ifdef _OLD_LINUX_
  #include <bstring.h>
#endif
#include <netinet/in.h>


#define SERVER_PORT             9001
#define READLEN    		4096
#define WRITELEN  		4096
#define MAX_CONNECT             32
#define HT_SIZE                 1000

typedef enum { False, True } Boolean;

/* Structure for a connection to the server: */

struct connect {
  int fd;
  Boolean output;
  char readbuff[READLEN];
  char writebuff[WRITELEN];
  char *readpos;
  char *writepos;
};

typedef struct connect Connection;

struct _ht_elem {
  char *ip_addr;
  char *hostname;
  struct _ht_elem *next;
};

typedef struct _ht_elem ht_elem;

/* Macros: */

#define fdesc(x)     connects[x].fd
#define writepos(x)  connects[x].writepos
#define writebuff(x) connects[x].writebuff
#define readpos(x)   connects[x].readpos
#define readbuff(x)  connects[x].readbuff
#define output(x)    connects[x].output

#define COPY(s)                 strcpy((char *) malloc(strlen(s) + 1), s)
#define False                   0
#define True                    1

int conn;                             /* current connection */
int width = 1;                        /* highest connection */

Connection connects[MAX_CONNECT];
ht_elem *htable[HT_SIZE];

int makesock(void);
void write_packet(int);
void read_packet(int);
void new_conn(int);
void end_conn(int);
void init_conn(int i);
int  find_index(int targetfd);
int  find_new_index(int);
int  find_mud(char *text);
Boolean eqmud(char *mudindex, char *mudname);
void read_configuration(FILE *fp); 
int find_index(int targetfd);

