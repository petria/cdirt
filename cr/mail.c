#include <stdlib.h>
#include "kernel.h"
#include "edit.h"
#include "mudmacros.h"
#include "mud.h"
#include "mail.h"
#include "log.h"

#include <sys/ioctl.h>
#include <errno.h>

void free_work_message(void) {
  Messageptr node;
  if (!cur_player->work_msg) return;
  for (node = cur_player->first_msg; node; node = node->next)
    if (node == cur_player->work_msg) { cur_player->work_msg = NULL; return; }
  free(cur_player->work_msg->subject); free(cur_player->work_msg);
  cur_player->work_msg = NULL;
}
void free_mail_list(void) {
  Messageptr node = cur_player->first_msg, next;
  while (node) {
    next = node->next;
    if (cur_player->work_msg == node) cur_player->work_msg = NULL;
    free(node->subject); free(node); node = next;
  }
  cur_player->first_msg = cur_player->cur_msg = NULL;
}

void mailcom(void) {
  if (aliased(real_mynum)) {
    bprintf ("&#You cannot use the mailer while you are aliased.\n");
    return;
  }

  sclrflg(mynum, SFL_MAIL);
  bprintf("&+BCDirt&+w Mailer v.3\n");
  cur_player->old_inp_handler = phandler(mynum);

  if(!EMPTY (getreinput(wordbuf))) {      /* mail <player> */
    new(wordbuf);
    return;
  }
  loadmail();
  if (ststflg(mynum, SFL_AUTOMAIL))
    listmail();
  strcpy(cur_player->cprompt, MAILPROMPT);
  cur_player->inmailer = True;
  push_input_handler(list);
  return;
}

void exit_mail() {
  pop_input_handler();
  replace_input_handler ((void *)cur_player->old_inp_handler);
  cur_player->inmailer = False;
  strcpy(cur_player->cprompt, build_prompt(mynum));
}

void new(char *input){
  time_t t;
  struct tm *tm;

  free_work_message();
  cur_player->work_msg = NEW(Message, 1);
  cur_player->work_msg->next = NULL;
  cur_player->work_msg->prev = NULL;

  strcpy(cur_player->work_msg->mailfrom, pname(mynum)); 
  cur_player->work_msg->status = MAIL_STATUS_NEW;
  time (&t);
  tm = localtime (&t);
  strftime(cur_player->work_msg->date, LINE_LEN, "%a %b %d, %I:%M%P", tm);
  get_mailto(input);
}

#ifdef INTERNET_EMAIL
Boolean is_internet_email( char *input ) {
  if( plev(mynum) >= LVL_ARCHWIZARD ) {
    while( *input != '\0' && *input != '\n' ) {
         if( *input == '@' )
           return True; 
         ++input;
    }
  }
  return False; 
}
#endif /* INTERNET_EMAIL */

void get_mailto(char *input) {
  PERSONA        p;

  if (input == NULL) {
    strcpy(cur_player->cprompt, "To: ");
    replace_input_handler(get_mailto);
  }
  else if (*input == 0) {
    if(cur_player->inmailer) {
      strcpy(cur_player->cprompt, MAILPROMPT);
      replace_input_handler(list);
      return;
    }
    else {
      exit_mail();
      return;
    }
  }
  else if (!getuaf(input, &p)) {

#ifdef INTERNET_EMAIL
    if(is_internet_email(input)) {
      bprintf( "Assuming it's Internet E-Mail.\n" );
      strcpy(cur_player->work_msg->mailto, input );   
      get_subject(NULL);
      return;
    }
#endif /* INTERNET_EMAIL */

    if (cur_player->inmailer) {
      bprintf("No such player. ENTER cancels.\n");
      strcpy(cur_player->cprompt, "To: ");
      replace_input_handler(get_mailto);
      return;
    }
    else {
      bprintf("No such player on system.\n");
      exit_mail();
      return;
    }
  }
  else {
    strcpy(cur_player->work_msg->mailto, p.ublock.pname);
    get_subject(NULL);
  }
}

void write_msg(Boolean is_reply, char *reply_path) {
  char tmppath[256];

  sprintf(tmppath, "%s/SENDMSG.%s", TEMP_DIR, pname(mynum));
  if (cur_player->inmailer)
    replace_input_handler(list);  /* nxt input handler, restored after edit1 */
  else
    replace_input_handler(cur_player->old_inp_handler);
  start_editor("Please type in your message:\n",
	       "&+W[&+BC-Mail %.3d&+W]&N ",
	       reply_path, tmppath, is_reply, TYPE_MAIL);
}
  
void get_subject(char *input) {

  if (input == NULL) {
    bprintf("To: &+G%s\n", cur_player->work_msg->mailto);
    strcpy(cur_player->cprompt, "Subject: " );
    replace_input_handler(get_subject);
  }
  else {
    free(cur_player->work_msg->subject);
    cur_player->work_msg->subject = COPY(*input ? input : "None");
    write_msg(False, NULL);
  }
}

void savemail(void) {
  char mailpath[256], tmppath[256];
  int tmpfd, saved, deleted;
  Messageptr message, next;

  sprintf(mailpath, "%s/%s", MAIL_DIR, pname(mynum));
  sprintf(tmppath, "%s/MAIL.%s", TEMP_DIR, pname(mynum));
  if ((tmpfd = OPEN(tmppath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
    progerror("open");
    return;
  }
  else {
    saved = deleted = 0;
    for (message = cur_player->first_msg ;
	 message != NULL ; message = next) {
      next = message->next;

      if (message->status == MAIL_STATUS_NEW)
	message->status = MAIL_STATUS_OLD;

      if (message->status != MAIL_STATUS_DELETED) {
	save(message, tmpfd);
	saved++;
      }
      else
	deleted++;
      if (cur_player->work_msg == message) cur_player->work_msg = NULL;
      free(message->subject);
      free(message);
    }
    cur_player->first_msg = cur_player->cur_msg = NULL;
    CLOSE(tmpfd);
    if (saved == 0)
      unlink(mailpath);
    else
      rename(tmppath, mailpath);
    if (saved && !deleted)
      bprintf("[Keeping &+Call&N messages]\n");
    else if (deleted && !saved)
      bprintf("[Deleting &+Yall&N messages]\n");
    else if (deleted || saved)
      bprintf( "[&+Y%d&N %s &+Ydeleted&N, &+C%d&N %s &+Csaved&N]\n",
	      deleted, (deleted == 1) ? "message" : "messages",
	      saved, (saved == 1) ? "message" : "messages" );
  }
} 

void help_msg (void) {
  bprintf("\nHelp for the &+BCDirt&+w mailer:\n\n"
	  "&+WS&+gend [message]&+w........ Send a message\n"
	  "&+WR&+Geply [message]&+w....... Reply to a message\n"
	  "&+WN&+cext&+w.................. Go to next unread message.\n"
	  "&+WP&+crevious&+w.............. Go to previous unread msg.\n"
	  "&+WD&+Belete [message]&+w...... Delete a message\n"
	  "&+WU&+Bndelete[messsage]&+w.... Undelete a message\n"
	  "&+WV&+yiew [message]&+w........ View a message\n"
	  "&+WQ&+ruit&+w.................. Quit the mailer\n"
          "&+WT&+moggle&+w................ Toggle auto-list of mail\n"
	  "&+C<number keys>&+w......... View message #\n"
  	  "&+C<return>&+w.............. Show index of mail\n\n"
	  "&+YNote:&+w arguments in square brackets are optional,"
	  "those in <>'s are required.\n"
	  "Your current message is marked with a '>'.  Many commands "
	  "work by default\n on the current message.\n"
	  "Upon exiting the mailer, all messages marked with a D will be "
	  "deleted\n\n");
}

void listmail (void) {
  int i;
  Messageptr message;

  if (cur_player->first_msg != NULL) {
    bprintf("\n     &+G   From      &+R  Subject                                  &+CDate\n");
    bprintf("     &+W   ----------- ---------------------------------------  -----------------\n");
    for (i = 1, message = cur_player->first_msg ;
	 message != NULL ; i++, message = message->next)
      bprintf("%-2s%c %-2d) %-11s %-41.40s%-17s\n",
	      (message == cur_player->cur_msg) ? ">" : "",
	      (message->status == MAIL_STATUS_READ) ? ' ' : message->status,
	      i, message->mailfrom, message->subject, message->date);
    bprintf("\n");
  }
}

/* main routine */
void list(char *inp){
  int i;
  Messageptr message;

  strcpy(cur_player->cprompt, MAILPROMPT);

  if(inp[0] == '*') {
    gamecom(inp+1, True);
    return;
  }
  else if (isdigit(*inp)) {
    i = atoi(inp);
    if ((cur_player->cur_msg = queue_msg(cur_player->first_msg, i)) != NULL)
      viewmsg();
    return;
  }

  switch (toupper(inp[0])) {
  case 'S':
    if (strchr(inp, ' ') != NULL && strlen(inp) > 1)
      new(strchr(inp, ' ') + sizeof(char));
    else
      new(NULL);
    return;
  case 'U':
    if (strchr(inp, ' ') == NULL) { 
      if(cur_player->cur_msg && 
	 cur_player->cur_msg->status == MAIL_STATUS_DELETED) {
	bprintf("Current message no longer marked for deletion.\n");
	cur_player->cur_msg->status = MAIL_STATUS_READ;
	return;
       } 
       bprintf("Usage:  Undelete <message> or U <message>\n");
       break;
    }
    i = atoi(strchr(inp, ' '));
    if ((message = queue_msg(cur_player->first_msg, i)) != NULL)
      message->status = MAIL_STATUS_READ;
    break;
  case 'D':
    if (strchr(inp, ' ') == NULL) {
      if (cur_player->cur_msg && 
	 cur_player->cur_msg->status != MAIL_STATUS_DELETED) {
	bprintf( "Current message marked for deletion.\n");
	cur_player->cur_msg->status = MAIL_STATUS_DELETED;
	break;
      }
      bprintf("Usage: Delete <message> or D <message>\n");
      break;
    }
    i = atoi(strchr(inp, ' '));
    if ((message = queue_msg(cur_player->first_msg, i)) != NULL) {
      message->status = MAIL_STATUS_DELETED;
    }
    break;
  case 'V':
    if (strchr(inp, ' ') == NULL) { 
      if( cur_player->cur_msg ) {
        viewmsg();
	return;
      }
      else
	bprintf("Usage: View <message> or V <message>\n");
    }
    else {
      i = atoi(strchr(inp, ' '));
      if((cur_player->cur_msg = queue_msg(cur_player->first_msg, i)) != NULL) {
	viewmsg();
	return;
      }
    }
    break;
  case 'N':
    if ((message = cur_player->cur_msg) != NULL) {
      do {
      message = message->next;
      } while (message != NULL && message->status == MAIL_STATUS_DELETED);
      if (message == NULL)
	bprintf( "No more un-deleted messages after this one.\n" );
      else
	cur_player->cur_msg = message;
    }
    break;
  case 'T':
    if (ststflg(mynum, SFL_AUTOMAIL)) {
      sclrflg(mynum, SFL_AUTOMAIL); 
      bprintf("Auto mail listing on startup disabled.\n");
    }
    else {
      ssetflg(mynum, SFL_AUTOMAIL);
      bprintf("Auto mail listing on startup enabled.\n");
    }
    break;
  case 'P':
    if ((message = cur_player->cur_msg) != NULL ) {
      do {
      message = message->prev;
      } while (message != NULL && message->status == MAIL_STATUS_DELETED);
    }
    if (message == NULL)
      bprintf( "No more un-deleted messages before this one.\n" );
    else
      cur_player->cur_msg = message;
    break;
  case 'R':
    if (strchr(inp, ' ' ) == NULL) {
      if (cur_player->cur_msg != NULL) {
        replymsg(NULL);
        return;
      }
      else
        bprintf("Usage: Reply <message>\n");
    }
    else {
      i = atoi(strchr(inp, ' '));
      if ((cur_player->cur_msg = queue_msg(cur_player->first_msg, i)) != NULL){
        replymsg(NULL);
        return;
      }
    }
    break;
  case '?': case 'H':
    help_msg();
    break;
  case '\0':
    listmail();
    break;
  case 'Q': case 'X':
    savemail();
    exit_mail();
    return;
  default:
    bprintf("Invalid command, type \"?\" for help with the mailer\n");
    break;
  }
}

/* Caller owns the complete line; mail subjects/replies are not truncated. */
char *mail_read_line(int fd) {
  Text text = {0};
  char c;
  ssize_t n;
  for (;;) {
    n = read(fd, &c, 1);
    if (n < 0 && errno == EINTR) continue;
    if (n <= 0) {
      if (!n && text.len) return text_take(&text);
      free(text.data); return NULL;
    }
    if (c == '\n') return text_take(&text);
    text_char(&text, c);
  }
}

/* copy mail text from the current message to an opened fd */

void write_text(int outfd) {
  char path[256], *line;
  int fd;
  sprintf(path, "%s/%s", MAIL_DIR, pname(mynum));
  if ((fd = OPEN(path, O_RDONLY, 0644)) == -1) {
    bprintf("Unable to open player's mail file.\n"); return;
  }
  lseek(fd, cur_player->cur_msg->text, SEEK_SET);
  while ((line = mail_read_line(fd))) {
    if (strstr(line, DELIM)) { free(line); break; }
    write(outfd, line, strlen(line)); write(outfd, "\n", 1); free(line);
  }
  CLOSE(fd);
}
/* save headers to player's mail file */

void store_header(int fd) {
  char c[2];

  c[0] = cur_player->work_msg->status;
  c[1] = '\0';
  write(fd, "From: ", 6);
  write(fd, cur_player->work_msg->mailfrom,
	strlen(cur_player->work_msg->mailfrom));
  write(fd, "\nSubject: ", 10);
  write(fd, cur_player->work_msg->subject,
	strlen(cur_player->work_msg->subject));
  write(fd, "\nDate: ", 7);
  write(fd, cur_player->work_msg->date,
	strlen(cur_player->work_msg->date));
  write(fd, "\nStatus: ", 9);
  write(fd, c, 1);
  write(fd, "\n", 1);
}

/* save headers for viewing */

void write_header(int fd) {
  write(fd, "\n&+BFrom&+W   : &+w", 19);
  write(fd, cur_player->cur_msg->mailfrom,
        strlen(cur_player->cur_msg->mailfrom));
  write(fd, "\n&+BTo&+W     : &+w", 19);
  write(fd, cur_player->cur_msg->mailto,strlen(cur_player->cur_msg->mailto));
  write(fd, "\n&+BDate&+W   : &+w", 19);
  write(fd, cur_player->cur_msg->date, strlen(cur_player->cur_msg->date));
  write(fd, "\n&+BSubject&+W: &+w", 19);
  write(fd, cur_player->cur_msg->subject,
        strlen(cur_player->cur_msg->subject));
  write(fd, "\n\n", 2);
}

void viewmsg() {
  int tmp;
  char path[256];
  
  sprintf(path, "%s/MESSAGE.%s", TEMP_DIR, pname(mynum));
  if ((tmp = OPEN(path, O_WRONLY | O_TRUNC | O_CREAT, 0660)) == -1) {
    bprintf("Unable to open temporary mail file.  Sorry.\n");
    return;
  }

  write_header(tmp);  /* save header to temp file */
  write_text(tmp);    /* save text to temp file */
    
    if (cur_player->cur_msg->status == MAIL_STATUS_NEW ||
	cur_player->cur_msg->status == MAIL_STATUS_OLD)
      cur_player->cur_msg->status = MAIL_STATUS_READ;

  CLOSE(tmp);
  strcpy(cur_player->cprompt, MAILPROMPT);
  bprintf("\001f%s\003", path);
}

void replymsg(char *input) {
  time_t t;
  struct tm *tm;
  char path[256];
  int tmp;
  Messageptr newmessage;

  if (input == NULL) {
    strcpy(cur_player->cprompt, "Quote text from original message? ");
    replace_input_handler(replymsg);
    return;
  }
  else if (tolower(*input) != 'y' && tolower(*input) != 'n') {
    bprintf("Please answer with Yes or No.\n");
    return;
  }

  time (&t);
  tm = localtime (&t);

  /* Initialize the message */
  newmessage = NEW(Message, 1);
  newmessage->next = NULL;
  newmessage->prev = NULL;

  /* Fill in the data.  */
  strcpy(newmessage->mailfrom, pname(mynum));
  newmessage->status = MAIL_STATUS_NEW;
  strftime(newmessage->date, LINE_LEN, "%a %b %d, %I:%M%P", tm); 
  strcpy(newmessage->mailto, cur_player->cur_msg->mailfrom);
  if(!strncmp(cur_player->cur_msg->subject, "Re:", 3 ))
    newmessage->subject = COPY(cur_player->cur_msg->subject);
  else
    newmessage->subject = text_format("Re: %s", cur_player->cur_msg->subject);

  /* Make it our work message */
  free_work_message();
  cur_player->work_msg = newmessage;

  /* Tell them the info and get the text */
  bprintf("\n&+BTo&+W     : &+w%s\n", newmessage->mailto);
  bprintf("&+BDate&+W   : &+w%s\n", newmessage->date);
  bprintf("&+BSubject&+W: &+w%s\n\n", newmessage->subject);

  if (tolower(*input) == 'y') {
    /* save the msg to a temp file, loaded by the editor */
    sprintf(path, "%s/REPLY.%s", TEMP_DIR, pname(mynum)); 
    if ((tmp = OPEN(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)) != -1) {
      write_text(tmp);
      write_msg(True, path);
    }
    else {
      bprintf("Error opening temporary save file for reply.\n");
      progerror("open");
    }
  }
  else
    write_msg(False, NULL);
  return;
}

Messageptr queue_msg(Messageptr first, int i) {
  Messageptr message = NEW(Message, 1);

  if (first == NULL) {
    bprintf("You don't have any mail, so try sending some!\n");
    return NULL;
  }
  else if (i <= 0) {
    bprintf("No such message.\n");
    return NULL;
  }

  for (message = first; i > 1 && message != NULL; 
       message = message->next, i--);
  if (message == NULL) {
    bprintf("No such message.\n");
    return(NULL);
  }
  return(message);
}

/* cdirt_getline : reads from FD, returns number of characters read, w/o newline */

int cdirt_getline(int fd, char *str, size_t capacity) {
  size_t n = 0;
  char c;
  ssize_t got;
  if (!capacity) return -1;
  str[0] = 0;
  while ((got = read(fd, &c, 1)) > 0) {
    if (c == '\n') return (int)n;
    if (n + 1 >= capacity) { str[n] = 0; return -1; }
    str[n++] = c; str[n] = 0;
  }
  return -1;
}

void loadmail(void) {
  Messageptr message;
  char path[LINE_LEN];
  int j, fd, newmsgs = 0, unreadmsgs = 0;
  char *line = NULL;

  free_mail_list();

  sprintf(path, "%s/%s", MAIL_DIR, pname(mynum));
  if ((fd = OPEN(path, O_RDONLY, 0644)) == -1) { 
      cur_player->cur_msg = NULL;
      bprintf("You have no mail.\n\n");
      return;
  }
  else {
    for (message = cur_player->first_msg, j = 0 ;; j++) {
      Messageptr newmessage = NULL;
      newmessage = NEW(Message, 1);    

      free(line); line = mail_read_line(fd);
      if (line && strlen(line) >= 6 && strlen(line + 6) < sizeof(newmessage->mailfrom))
	strcpy(newmessage->mailfrom, &line[6]);
      else {
        free(newmessage); free(line);
	if (!newmsgs && !unreadmsgs)
	  bprintf("You have no unread mail.\n");
	else if (!newmsgs && unreadmsgs)
          bprintf("You have %d unread %s waiting, %s.\n", unreadmsgs, 
		  (unreadmsgs == 1) ? "message" : "messages", pname(mynum));
	else if (newmsgs && unreadmsgs)
	  bprintf("You have %d unread %s and %d &+Ynew&N, %s.\n", unreadmsgs, 
	   (unreadmsgs == 1) ? "message" : "messages", newmsgs, pname(mynum));
	else if (newmsgs && !unreadmsgs)
	  bprintf("You have %d &+Ynew &N%s, %s.\n", newmsgs,
	       (newmsgs == 1) ? "message" : "messages", pname(mynum));
	CLOSE(fd);
        return;
      }
      free(line); line = mail_read_line(fd);
      if (!line || strlen(line) < 9) { free(line); free(newmessage); CLOSE(fd); return; }
      newmessage->subject = COPY(&line[9]);
      free(line); line = mail_read_line(fd);
      if (!line || strlen(line) < 6 || strlen(line + 6) >= sizeof(newmessage->date)) { free(line); free(newmessage->subject); free(newmessage); CLOSE(fd); return; }
      strcpy(newmessage->date, &line[6]);
      free(line); line = mail_read_line(fd);
      if (!line || strlen(line) < 9) { free(line); free(newmessage->subject); free(newmessage); CLOSE(fd); return; }
      newmessage->status = line[8];
      newmessage->text = lseek(fd, 0L, SEEK_CUR);

      if (newmessage->status == 'N')
	newmsgs++;
      else if (newmessage->status == 'O')
	unreadmsgs++;

      strcpy(newmessage->mailto, pname(mynum));

      do {                                   /* skip to next entry */
        free(line); line = mail_read_line(fd);
        if (!line) {
          free(newmessage->subject); free(newmessage); CLOSE(fd); return;
        }
      } while(!strstr(line, DELIM));

      newmessage->next = NULL;

      if (j == 0) {                           /* first message */
	newmessage->prev = NULL;
	cur_player->cur_msg = newmessage;
	cur_player->first_msg = newmessage;
	message = newmessage;
      }
      else {
	newmessage->prev = message;
	message->next = newmessage;
	message = message->next;
      }
    }
  }
}

/* extracts/saves a message from the players mailbox, should only be used
   when the player exits the mailer, since it changes the current msg */

void save(Messageptr message, int fd) {
  cur_player->cur_msg = message;
  cur_player->work_msg = message;
  store_header(fd);
  write_text(fd);
  write(fd, DELIM, strlen(DELIM));
  write(fd, "\n", 1);
}

void check_for_internet_mail() {
  FILE *fptr;
  char path[80];
  /* Messageptr message = (Messageptr) malloc(sizeof(Message)); */

  return; /* for now.  :) */

  sprintf(path, "%s/%s", MAIL_DIR, cur_player->work_msg->mailto);
  if ((fptr = FOPEN(path, "a")) != NULL) {
  
  }
}

