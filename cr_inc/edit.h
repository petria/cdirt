#include <stdio.h>
#include <sys/file.h>
#include "kernel.h"
#include "mail.h"
#include "mud.h"
#include "log.h"
#include "bprintf.h"
#include "uaf.h"
#include "change.h"
 
#define txt_start   cur_player->txt_start    /* aliases make things simpler */
#define txt_curr    cur_player->txt_curr
#define numlines    cur_player->edit_lines
#define edit_status cur_player->edit_status
#define curr_line   cur_player->edit_curr_line
#define eprompt     cur_player->edit_prompt

#define EDIT_PROMPT  "&+W(&+BEditor&+W) &N[&+M? for &+Yhelp&N]: "
#define MAIL_PROMPT  "&+Bq&+W)&Nuit/send, &+Bc&+W)&Nancel msg, &+Bh&+W)&Nelp: "
#define DESC_PROMPT  "&+Bq&+W)&Nuit/save, &+Bc&+W)&Nancel, &+Bh&+W)&Nelp: "
#define E_TITLE      "Welcome to the &+BC-dirt&N Editor. Type ** to exit\n"
#define CANCELED     "&+y[&+YCancelled&+y]\n"
#define CANCEL_MSG   "You have made changes to this buffer.  Confirm (Y/N): "
#define SAVE_MSG     "&+C[&+WSaved %d line%s of text&+C]\n"
#define SAVE_MSG2    "&+C[&+WWrote empty file&+C]\n"
#define READ_MSG     "&+C[&+WRead %d line%s of text&+C]\n"
#define READ_MSG2    "&+C[&+WPrevious file was empty&+C]\n"
#define GOD_READ_MSG "&+C[&+WRead %d line%s of text from %s&+C]\n"
#define ADD_MSG      "&+C[&+WInserting after line %d, ** exits edit-mode&+C]\n"
#define ADD_MSG2     "&+C[&+WEditing new file, ** to exit edit-mode&+C]\n"
#define DELETE_MSG   "&+C[&+WLine %d &+Rdeleted&+C]\n"
#define VIEW_MSG     "&+C[&+WCurrent file&+C]:\n\n"
#define QUOTE_MSG    "&+C[&+WQuoted %d lines of text from message&+C]\n"
#define DESC_MSG     "&+C[&+WRead previous description&+C]\n"
#define RETURN_MSG   "&+C[&+WReturning you to mailer&+C]\n"

#define MENU_MODE -1
#define ADD_MODE -2
#define NORMAL_MODE 0
#define TYPE_MAIL 0
#define TYPE_APPEND 1
#define TYPE_CREATE 2
#define TYPE_WRITE 3
#define TYPE_DESC 4
#define TYPE_CHDESC 5

void edit1 (char *);
void save_buff (char *, int);
void load_buff (char *, Boolean);
void new_line (char *);
void show_prompt(void);
void start_editor(char *message, char *prompt, char *infile,
                  char *outfile, Boolean quote, int type);
void view_text (void);
extern void store_header(int);
extern Boolean is_internet_email(char *);
