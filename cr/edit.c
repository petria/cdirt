#include <stdlib.h>

#include "edit.h"
#include "sflags.h"
#include "pflags.h"

/* This is the command to edit files in the data directory */

void editcom() {
  char path[256];

  if (!ptstflg(mynum, PFL_EDITOR))
    bprintf("You can't use the editor!\n");
  else {
    if(brkword() != 1 && wordbuf != NULL && wordbuf[0] != '\0') {
      sprintf(path, "%s/%s", EDIT_PATH, wordbuf);
      start_editor("You may use game commands with *command.",
		   "&+W[&+BC-Edit %.3d&+W] &N",
		   path, path, False, TYPE_WRITE);
    }
    else
      bprintf("Edit what?\n"); 
  }
}

/* This is the command to start the editor.
 * message...... displayed on startup
 * prompt....... a prompt with %d somewhere in it for the line number
 * infile....... file to load, NULL if none
 * outfile...... output file to write to
 * quote........ should a ">" be placed on each line of infile?
 * type......... type of output file, one of:
 * TYPE_MAIL, TYPE_APPEND, TYPE_CREATE, TYPE_DESC, TYPE_WRITE */

void start_editor(char *message,
		  char *prompt, 
		  char *infile,  
		  char *outfile,
		  Boolean quote,
		  int type){

  cur_player->edit_inp_handler = phandler(mynum);
  bprintf(E_TITLE);
  bprintf("%s\n", message);
  strcpy(eprompt, prompt);
  edit_status = NORMAL_MODE;
  cur_player->edit_mode = type;
  cur_player->writer = True;
  strcpy(cur_player->edit_outfile, outfile);
  curr_line = 1;
  if (infile != NULL) {
    load_buff(infile, quote);
    if (type == TYPE_MAIL || type == TYPE_DESC)
      view_text();
  }
  show_prompt();
  replace_input_handler(edit1);
}

void queue_line(int linenum) {
  for (txt_curr = txt_start; linenum > 0 && txt_curr->next != NULL;
       txt_curr = txt_curr->next, linenum--);
}

/* should be switched over to unix IO */
void load_buff(char *infile, Boolean quote) {
  FILE *fptr;
  char buff[MAX_COM_LEN];
 
  numlines = 0;
  txt_curr = txt_start = NULL;

  if ((fptr = FOPEN(infile, "r")) == NULL)
    return;
  else {
    if (quote)
      while (1) {
	fgets(&buff[2], MAX_COM_LEN - 3, fptr);
	if (feof(fptr))
	  break;
	else {
	  buff[0] = '>';
	  buff[1] = ' ';
	  new_line(buff);
	}
      }
    else
      while (1) {
	fgets(buff, MAX_COM_LEN - 1, fptr);
	if (feof(fptr))
	  break;
	else
	  new_line(buff);
      }
    FCLOSE(fptr);
    if (quote)
      bprintf(QUOTE_MSG, numlines);
    else if (cur_player->edit_mode == TYPE_DESC)
      bprintf(DESC_MSG, numlines);
    else if (plev(mynum) < LVL_DEMI) {
      if (numlines)
	bprintf(READ_MSG, numlines, (numlines > 1) ? "s" : "");
      else
	bprintf(READ_MSG2);
    }
    else
      bprintf(GOD_READ_MSG, numlines, (numlines > 1) ? "s" : "", infile);
  }
}

void save_buff(char *outfile, int type) {
  int fd;
  int open_flags = 0;
  Text_Ptr txt_ptr;

  if (numlines)
    bprintf(SAVE_MSG, numlines, (numlines > 1) ? "s" : "" );
  else
    bprintf(SAVE_MSG2);
  
  switch (type) {
  case TYPE_APPEND:
    open_flags = O_WRONLY | O_APPEND | O_CREAT;
    break;
  case TYPE_CREATE: case TYPE_MAIL: case TYPE_DESC: case TYPE_CHDESC:
    open_flags = O_WRONLY | O_CREAT | O_TRUNC;
    break;
  case TYPE_WRITE:
    open_flags = O_WRONLY | O_CREAT;
    break;
  }
  if ((fd = OPEN(outfile, open_flags, 0664)) != -1)
    {
      if (type == TYPE_MAIL)
	store_header(fd);

      for (txt_ptr = txt_start; txt_ptr != NULL; txt_ptr = txt_ptr->next)
	write(fd, txt_ptr->line, strlen(txt_ptr->line));
      if (type == TYPE_MAIL)
	write(fd, "***\n", 4);
      CLOSE(fd);
    }
  else {
    bprintf("Error saving file.\n");
    mudlog("ERROR: save_buff");
  }
}

void free_list() {
  Text_Ptr txt_ptr;
  Text_Ptr tmpptr = NULL;
  
  for (txt_ptr = txt_start; txt_ptr != NULL; txt_ptr = txt_ptr->next) {
    if (tmpptr) free(tmpptr->line);
    FREE(tmpptr);
    tmpptr = txt_ptr;
  }
  if (tmpptr) free(tmpptr->line);
  FREE(tmpptr);
  txt_curr = txt_start = NULL;
  numlines = 0;
  curr_line = 1;
}

void new_line(char *input) {
  Text_Ptr newline;
  int j;
  Boolean newline_found = False;

  newline = NEW(Text_Line, 1);

  { Text text = {0};
    text_append(&text, input);
    if (!strchr(input, '\n')) text_char(&text, '\n');
    newline->line = text_take(&text);
  }

  newline->next = NULL;
  if (numlines == 0)                           /* first line of text */
    txt_start = txt_curr = newline;
  else {
    txt_curr->next = newline;
    txt_curr = txt_curr->next;
  }
  numlines++;
  curr_line++;
}

void add_lines(int linenum) {
  if (linenum > 0) {
    bprintf(ADD_MSG, linenum);
    linenum--;
  }
  else
    bprintf(ADD_MSG2);

  queue_line(linenum);
  if (txt_curr != NULL)
    cur_player->tmp_ptr = txt_curr->next;
  else
    cur_player->tmp_ptr = NULL;
  edit_status = ADD_MODE;
  edit1(NULL);
}

void delete_line(int linenum) {
  Text_Ptr tmpptr;

  if ((linenum - 1) == 0) {
    tmpptr = txt_start;
    txt_start = txt_start->next;
    if (tmpptr) free(tmpptr->line);
    FREE(tmpptr);
  }
  else {
    queue_line(linenum - 2);
    tmpptr = txt_curr->next;
    txt_curr->next = txt_curr->next->next;
    if (tmpptr) free(tmpptr->line);
    FREE(tmpptr);
  }
  numlines--;
}

void view_text (void) {
  int j = 1;
  Text_Ptr txt_ptr;

  bprintf( "\n" );
  for (txt_ptr = txt_start ; txt_ptr != NULL ; txt_ptr = txt_ptr->next)
    bprintf("&+B[&+M%.3d&+B]&+W: %s", j++, txt_ptr->line);
  bprintf("\n");
}

/* return difference of 2 input numbers */
int getnum (char *str, int *num) {
  char *ptr;
  int num1, num2;

    for (ptr = str ; !isdigit(*ptr) ; ptr++) {
      if (*ptr == '\0') {
	bprintf("This command must be followed by a number.\n");
        return -1;
      }
    }
    num1 = atoi(ptr); 
    if ((ptr = strchr(str, '-')) != NULL) {
      if (num1 == 0 || (num2 = atoi(ptr + 1)) == 0) {
	bprintf("To use this feature, type a range.  Ex: D 1-5\n");
	return -1;
      }
      else {
	if (num1 > numlines || num2 > numlines || 
	    num1 < 1 || num2 < 1 ||
	    ((num2 - num1) < 0)) {
	  bprintf("Number out of range.\n");
	  return -1;
	}
	else {
	*num = num1;
	return(num2 - num1 + 1);
	}
      }
    }
    else {
    *num = num1;
    if (*num > numlines || *num < 1) {
      bprintf("Number out of range.\n");
      return -1;
    }
    else
      return 1;
  }
}

void show_prompt() {
  static char edit_prompt[80];
  sprintf(edit_prompt, eprompt, curr_line);
  strcpy(cur_player->cprompt, edit_prompt);
}

void editor_help(void) {
  bprintf("\nHelp for the &+BCDirt editor commands:\n\n"
          "&+RE&+WX&+Rit&N................... Save the buffer, then exit\n"
          "&+WS&+Mave&N................... Save the buffer\n"
          "&+WC&+Yancel&N................. Stop editing this buffer\n"
	  "&+WNew&N.................... Start editing from srcatch\n"
          "&+WD&+Celete [Line#]&N.. ...... *Delete a line of text\n"
          "&+WD&+Belete [Line# - Line#].&N *Delete multiple lines of text\n"
          "&+WI&+Gnsert [Line#]&N......... *Insert text after [line #]\n"
	  "&+W*&+y[command]&N............. Run a mud command in editor\n"
	  "&+W=&+y[command&N.............. Run a menu command from edit mode\n"
          "&+y[return]&N............... View current buffer\n\n"
          "&+Y*Note: Commands marked with an asterisk will cause lines to be "
	  "re-numbered\n&+YAs this may cause some confusion, it is probably "
	  "best to issue the command,\nthen type &+y[RETURN]&+Y to view the "
	  "buffer.\n");
}

void clean_up() {
  if (cur_player->inmailer) {
    strcpy(cur_player->cprompt, MAILPROMPT);
    bprintf(RETURN_MSG);
  }
  else
    strcpy(cur_player->cprompt, (char *) build_prompt(mynum));
  
  cur_player->writer = False;
  replace_input_handler(cur_player->edit_inp_handler);
}

/* not working yet */
int cancel_buff(char *input) {
  if (input == NULL) {
    bprintf(CANCEL_MSG);
    replace_input_handler( (void *) cancel_buff);
  }
  else if (*input == 'Y' || *input == 'y') {
    bprintf(CANCELED);
  }
  else if (*input == 'N' || *input == 'n') {
    bprintf("Cancel aborted.\n");
    edit1(NULL);
    return 0;
  }
  else
    replace_input_handler((void *) cancel_buff);
  return 0;
}

void send_mail() {
  int sendto, tmp_mynum, fd1, fd2;
  char tmp_path[256];
  char mail_path[256];
  char buff[MAX_COM_LEN];
  PERSONA p;
  int numread;

    /* copy tmp file to player's mailbox */

    /* Internet email */
#ifdef INTERNET_EMAIL
    if( is_internet_email( cur_player->work_msg->mailto ) ) {
      char *sendmail;
      char *title;
      const char *type = "w";
      FILE *fptr;

      title = COPY(make_title(ptitle(mynum), pname(mynum)));
      strip_color(title, title);

      sendmail = text_format("%s -f%s -F \"%s\" -odb %s", SENDMAIL,
                         pname(mynum),
			 title,
                         cur_player->work_msg->mailto);
      fptr = popen(sendmail, type);
      free(sendmail); free(title);

      if( !fptr ) {
        bprintf( "Odd, couldn't send the mail.  Sorry.\n" );
        return;
      }

      fprintf( fptr, 
	   "Organization: &$ (&H: &P)\n"
           "X-Mailer: CDirt Mud Mailer (C) Weinerdog and Mancini.\n");

      strcpy(tmp_path, cur_player->edit_outfile);

      if((fd1 = OPEN(tmp_path, O_RDONLY, 0644)) != -1) {
        while ((numread = read(fd1, buff, MAX_COM_LEN)) > 0)
          fwrite( buff, numread, 1, fptr );
        CLOSE(fd1);
      }
      else {
        bprintf( "Couldn't open temp file\n" );
        return;
      }

      fputs( ".", fptr );
      pclose( fptr );
      bprintf( "Internet e-mail sent!\n" ); 
      return;
    }
#endif /* INTERNET_EMAIL */

  strcpy(tmp_path, cur_player->edit_outfile);
  sprintf(mail_path, "%s/%s", MAILDIR, cur_player->work_msg->mailto);

  if (((fd1 = OPEN(tmp_path, O_RDONLY, 0644)) != -1) &&
      ((fd2 = OPEN(mail_path, O_WRONLY | O_APPEND | O_CREAT, 0664)) != -1)) {
    while ((numread = read(fd1, buff, MAX_COM_LEN)) > 0)
      write (fd2, buff, numread);
    CLOSE(fd1);
    CLOSE(fd2);

  /* send received-mail message & update list */

    sendto = fpbn(cur_player->work_msg->mailto);
    if (sendto != -1) {
      if(players[sendto].inmailer) {
	tmp_mynum = real_mynum;
	setup_globals(sendto);
	loadmail();
	listmail();
	setup_globals(tmp_mynum);
      }
      ssetflg(sendto, SFL_MAIL);
      sendf(sendto,"&+Y***&+W\a You have received new mail from %s &+Y***\n",
	    pname(mynum));
    }
    else {
      if (getuaf(cur_player->work_msg->mailto, &p))
	pset_flg(p.ublock.bits, SFL_MAIL, SFLAGS);
      putuaf(&p);
    }
  }
  else { 
    bprintf("Error saving file.\n");
    mudlog("ERROR: send_mail\n");
  }
}

void edit1(char *input) {
  int num, count;

  if (input == NULL)
    show_prompt();
  else if (edit_status != MENU_MODE && input[0] != '=') { 
    if (input[0] == '*') {
      if (input[1] == '*') {                                 /* "**" == exit */
	if (edit_status == ADD_MODE) {    /* adding something in? ifso, link */
	  if (cur_player->tmp_ptr != NULL)        /* have something typed in */
	    txt_curr->next = cur_player->tmp_ptr;
	  else
	    txt_curr = NULL;
	}
	if (cur_player->edit_mode == TYPE_MAIL)
	  strcpy(cur_player->cprompt, MAIL_PROMPT);
	else if (cur_player->edit_mode == TYPE_DESC)
	  strcpy(cur_player->cprompt, DESC_PROMPT);
	else
	  strcpy(cur_player->cprompt, EDIT_PROMPT);
	edit_status = MENU_MODE;
      }
      else {                             /* game command */
	replace_input_handler(cur_player->edit_inp_handler);
	gamecom(input+1, True);
	edit1(NULL);
      }
    }
    else {                             /* normal case: add this line of text */
      new_line(input);
      show_prompt();
    }
  }
  else if (edit_status == MENU_MODE || input[0] == '=') {  
                                      /* MENU MODE or =command */
    switch ((input[0] == '=') ? tolower(input[1]) : tolower(input[0])) {
    case 'c':                         /* cancel */
      if (cur_player->edit_mode == TYPE_MAIL)
	bprintf("Message cancelled.\n");
      else
	bprintf("Buffer &+Ynot &Nsaved.\n");
      free_list();
      clean_up();
      return;
    case 'e': case 'x': case 'q':                            /* save/exit */
      save_buff(cur_player->edit_outfile, cur_player->edit_mode);
      if (cur_player->edit_mode == TYPE_MAIL)
	send_mail();
      if (cur_player->edit_mode == TYPE_CHDESC)
        load_from_file(cur_player->edit_outfile);
      free_list();
      clean_up();
      return;
    case 'd':                         /* delete a line */
      if ((count = getnum(input, &num)) != -1) {
	if (count == 1)
	  bprintf("[Line %d deleted]\n", num);
	else
	  bprintf("[Lines %d through %d deleted]\n", num, num + count - 1);
	while (count-- > 0)
	  delete_line(num);
      }
      break;
    case 'i':                         /* add a line */
      if ((getnum(input, &num)) != -1) {
	curr_line = num + 1;
	add_lines(num);
      }
      break;
    case '?': case 'h':               /* help        */
      editor_help();
      break;
    case 'n':                         /* new buffer  */
      free_list();
      add_lines(0);
      break;
    case 's':                         /* save buffer */
      save_buff(cur_player->edit_outfile, cur_player->edit_mode);
      break;
    case '\0': case 'v':              /* view buffer */
      view_text();
      break;
    default:
      bprintf("Command Invalid; Type ? for help.\n");
      break;
    }
    if (edit_status == MENU_MODE)
      bprintf(EDIT_PROMPT);
    else if (input[0] == '=' && input[1] != 'n' && input[1] != 'i')
      show_prompt();
  }
  replace_input_handler(edit1);
}
