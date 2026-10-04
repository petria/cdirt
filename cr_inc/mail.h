#include <time.h>
#include <unistd.h>
#include "timing.h"
#include "sflags.h"
#include "sendsys.h"
#include "utils.h"
#include "parse.h"
#include "uaf.h"
#include "mobile.h"
#include "sys/file.h"
#include "bprintf.h"

#define M_BUFLEN 4096
#define FALSE 0
#define TRUE 1
#define TYPE_MAIL 0
#define DELIM "***"
#define MAILPROMPT "&+W(&+BMailer&+W)&+w [&+M?&+w for &+Rh&+Ge&+Bl&+Cp&+w]: "
#define mail_prompt() bprintf(MAILPROMPT)
#define MAIL_STATUS_NEW 'N'
#define MAIL_STATUS_OLD 'O'
#define MAIL_STATUS_DELETED 'D'
#define MAIL_STATUS_READ 'R'


void mailcom(void);
void free_work_message(void);
void free_mail_list(void);
void new(char *);
void viewmsg(void);
void list(char *inp);
void loadmail(void);
void savemail(void);
void listmail(void);
void save(Messageptr message, int fd);
void delete(Messageptr);
Messageptr queue_msg(Messageptr, int);
void remove_newline(char *line);
void get_mailto(char *input);
void get_subject(char *input);
void get_notes(char *input);
void check_for_internet_mail( );
void prompt_print(char *);
void replymsg(char *);
int cdirt_getline(int fd, char *str, size_t capacity);

/* macros */
#define work_msg(C)               players[C].work_msg
#define cur_msg(C)                players[C].cur_msg
#define first_msg(C)              players[C].first_msg
