#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include "kernel.h"
#include "mud.h"

/* mudfd.c: mfopen, mfclose, mopen, mclose         */
/* store more information about opened fds         */
/* usage: mfopen("file", line, normal fopen cmds); */
/*        mopen("file", line, normal open cmds);   */
/*        mfclose(fptr);                           */
/*        mclose(fd);                              */

void * open1(char *, char *, int, mode_t, char *);
int close1(int, FILE *) ;
void __mfclose(FILE *);
void __mclose(int);
FILE *__mfopen (char *, int, char *, char *);
int __mopen (char *, int, char *, int, mode_t); 

extern void bprintf (char *, ...);
extern void mudlog (char *, ...);

struct __fd_entry {
  struct __fd_entry *next;
  char msg[20];
  char path[256];
  char stdmode[3];
  int mode;
  int fd;
  FILE *fptr;
};

typedef struct __fd_entry Fd_ent;
typedef Fd_ent *Fdptr;

Fdptr fd_first = NULL;

int close1(int fd, FILE *fptr) {
  Fdptr ptr, pptr, temp;

  for (ptr = pptr = fd_first ; ptr ; ptr = ptr->next) {
    if (ptr->fd == fd) {
      temp = ptr;

      if (ptr == fd_first) {
	if (fd_first->next)
	  fd_first = fd_first->next;
	else
	  fd_first = NULL;
      }
      else if (ptr->next == NULL)
        pptr->next = NULL;
      else
	pptr->next = ptr->next;

      if (fptr)
	fclose(fptr);
      else
	close(fd);

      free(temp);
      return 0;
    }
    pptr = ptr;     /* keep pptr one before ptr */
  }
  return -1;
}

void __mfclose (FILE *file) {
  if (close1(fileno(file), file) == -1) 
    mudlog("FD: error in mfclose(), file not opened with mfopen()");
}

void __mclose (int fd) {
  if (close1(fd, NULL) == -1)
    mudlog("FD: error in mclose(), file not opened with mopen()");
}

FILE *__mfopen (char *file, int line, char *path, char *mode) {
  char message[80];
  sprintf(message, "%s:%d", file, line);
  return((FILE *) open1(message, path, 0, 0, mode));
}

int __mopen (char *file, int line, char *pathname, int flags, mode_t mode) {
  char message[80];
  sprintf(message, "%s:%d", file, line);
  return((int) (intptr_t) open1(message, pathname, flags, mode, NULL));
}

void * open1(char *message, char *pathname, 
	     int flags, mode_t mode, char *stdmode) {
  int fd = -1;
  Fdptr ptr, newptr;
  FILE *fptr = NULL;

  if ((newptr = (Fdptr) NEW(Fd_ent, 1)) == NULL) {
    mudlog("Error in open1(), no memory to allocate");
    return((void *) NULL);
  }
  strcpy(newptr->msg, message);
  strcpy(newptr->path, pathname);
  newptr->next = NULL;

  if (stdmode != NULL) {
    if ((fptr = fopen(pathname, stdmode)) == NULL) {
      /* mudlog("fopen: %s", pathname); */
      free(newptr);
      return NULL;
    }
    newptr->fptr = fptr;
    newptr->fd = fileno(fptr);
    strcpy(newptr->stdmode, stdmode);
  }
  else {
    if ((fd = open(pathname, flags, mode)) == -1) {
      perror("open");
      free(newptr);
      return (void *) (intptr_t) -1;
    }
    newptr->fptr = NULL;
    newptr->fd = fd;
    newptr->mode = flags;
  }

  if (fd_first == NULL)
    fd_first = newptr;
  else {
    for (ptr = fd_first ; ptr->next ; ptr = ptr->next);
    ptr->next = newptr;
  }
  if (stdmode)
    return (void *) fptr;
  else
    return (void *) (intptr_t) fd;
}

void fdlistcom (FILE *outptr) {
  Fdptr ptr;

  char type[9] = "[Player]";
  char mode[8] = "unknown";

  for (ptr = fd_first ; ptr != NULL ; ptr = ptr->next) {
    if (find_pl_index(ptr->fd) == -1)  /* there is no player for this fd */
      strcpy(type, "[System]");
    
    if (ptr->fptr) {
      if (ptr->stdmode[1] == '+')
	strcpy(mode, "read/write");
      else if (ptr->stdmode[0] == 'r')
	strcpy(mode, "read");
      else if (ptr->stdmode[0] == 'w')
	strcpy(mode, "write");
    }
    else {
      if (ptr->mode == O_RDONLY)
	strcpy(mode, "read");
      else if (ptr->mode == O_WRONLY)
	strcpy(mode, "write");
      else if (ptr->mode == O_RDWR)
	strcpy(mode, "read/write");
    }
    if (outptr)
      fprintf(outptr, "%c%d\t%s\t%s\t%s\t%s\n",
	    (ptr->fptr) ? '*' : ' ', ptr->fd, ptr->msg, type, ptr->path, mode);
    else
      bprintf("%c%d\t%s\t%s\t%s\t%s\n", 
	    (ptr->fptr) ? '*' : ' ', ptr->fd, ptr->msg, type, ptr->path, mode);
  }
}

void fdclosecom (int fd) {
  Fdptr ptr;

  for (ptr = fd_first ; ptr ; ptr = ptr->next) 
    if (ptr->fd == fd) {
      if (ptr->fptr)
	__mfclose(ptr->fptr);
      else
	__mclose(fd);
      return;
    }
  bprintf("No such fd: %d\n", fd); 
}
