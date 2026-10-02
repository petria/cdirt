#include <stdio.h>
#include "kernel.h"
#include "commands.h"
#include "wizard.h"
#include "mud.h"
#include "uaf.h"
#include "mail.h"
#include "change.h"
#include "frob.h"
#include "bprintf.h"
#include "timing.h"
#include "pflags.h"
#include "sflags.h"
#include "verbs.h"

extern char *opts[];
extern char *toggles[];
extern char *wizopts[];
extern char *wiztoggles[];
#define NUMTOGGLE 15
#define NUMOPTS 5
#define NUMWIZTOGGLE 6
#define NUMWIZOPTS 14
#define STATUS players[mynum].work 
void configure (char *);
void get_setins (char *);
void show_lists (void);

int ttable[] = {
  VERB_NOORACLE,
  VERB_NOBEEP,
  VERB_NOWET,
  VERB_NOBLINK,
  VERB_NEWSTYLE,
  VERB_AUTOEXIT,
  VERB_HEARBACK,
  VERB_NOINV,
  VERB_NOFIGHT,
  VERB_AWAY,
  VERB_BUSY,
  VERB_BRIEF,
  VERB_NOGOSSIP,
  VERB_NOANON,
  VERB_COLOR,
  VERB_NOPUFF
};

int wizttable[] = {
  VERB_NOWISH,
  VERB_SEEEXT,
  VERB_NOSHOUT,
  VERB_ALOOF,
  VERB_NOWIZ,
  VERB_NOSLAIN,
  VERB_IMINVIS,
};

char *wizoptwords[] = {
  "prompt",
  "setin",
  "setout",
  "setmin",
  "setmout",
  "setvin",
  "setvout",
  "setqin",
  "setqout",
  "setsit",
  "setstnd",
  "setsum",
  "setsin"
};

int sfltable[] = {
  SFL_NOORACLE,
  SFL_NOBEEP,
  SFL_NOWET,
  SFL_NOBLINK,
  SFL_NEWSTYLE,
  SFL_AUTOEXIT,
  SFL_HEARBACK,
  SFL_NOINV,
  SFL_NOFIGHT,
  SFL_AWAY,
  SFL_BUSY,
  SFL_BRIEF,
  SFL_NOCHAT,
  SFL_NOANON,
  SFL_COLOR,
  SFL_NOPUFF
};


int wizsfltable[] = {
  SFL_NOWISH,
  SFL_SEEEXT,
  SFL_NOSHOUT,
  SFL_ALOOF,
  SFL_NOWIZ,
  SFL_NOSLAIN,
};

void show_menu (char *msg, char *type[], int max, int mode) {
  int i;
  char buff[80];

  bprintf("&+M%s\n\n", msg);
  for (i = 0 ; i < max ; i++) {
    strcpy(buff, type[i]);
    if (mode) {
      if ((mode == 1 && ststflg(mynum, sfltable[i])) ||
	  (mode == 2 && ststflg(mynum, wizsfltable[i])))
	strcat(buff, " &+W[&+Gon&+W]");
      else
	strcat(buff, " &+W[&+Roff&+W]");
    }
    if (i % 2)
      bprintf("&+R%c&+W) &+Y%-36s\n", 'a' + i, buff);
    else
      bprintf("&+R%c&+W) &+Y%-37s", 'a' + i, buff);
  }
  bprintf("\n");
}


void configurecom () {
  if (cur_player->writer) {
    bprintf("Finish whatever you're writing first.\n");
    return;
  }
  bprintf("Welcome to the CDirt Configuration Editor!\n[return] ");

  cur_player->old_inp_handler = phandler(mynum);
  STATUS = 0;
  push_input_handler(configure);
  STATUS = 0;
}

void exit_config() {
  replace_input_handler ((void *)cur_player->old_inp_handler);
  pop_input_handler();
  strcpy( cur_player->cprompt, (char *) build_prompt(mynum) );
  return;
}

void configure (char *input) {

  /* handle possible inputs, player typed return */
  if (*input == '\0') {
    if ((STATUS > 1 && plev(mynum) < LVL_WIZARD) ||
        (STATUS > 4)) {
      exit_config();
      return;
    }
    else {
      STATUS++;
      replace_input_handler(configure);
    }
  }
  else {
    if (!isalpha(*input)) {
      bprintf("Please type a letter.\n");
      replace_input_handler(configure);
    }
    else if (*input == 'q') {
      replace_input_handler ((void *)cur_player->old_inp_handler);
      pop_input_handler();
      strcpy( cur_player->cprompt, (char *) build_prompt(mynum) );
      return;
    }
    else if ((STATUS == 1 && ((*input - 'a') >= NUMTOGGLE)) || 
	     (STATUS == 2 && ((*input - 'a') >= NUMOPTS)) ||
	     (STATUS == 3 && ((*input - 'a') >= NUMWIZTOGGLE)) ||
	     (STATUS == 4 && ((*input - 'a') >= NUMWIZOPTS))) {
      bprintf("Invalid letter.\n");
      replace_input_handler(configure);
    }
    else {
      switch (players[mynum].work) {
      case 1:
	doverb(ttable[*input - 'a']);
	break;
      case 2:
	switch(*input) {
	case 'a':
	  reviewcom();
	  break;
	case 'b':
	  /* change_desc(); */
	  break;
	case 'c':
	  /* change_sex(); */
	  break;
	case 'd':
	  /* change_title(); */
	  break;
	case 'e':
	  /* change_passwd(); */
	  break;
	}
	break;
      case 3:
	doverb(wizttable[*input - 'a']);
	break;
      case 4:
        cur_player->work2[0] = *input - 'a';
	get_setins(NULL);
	return;
      }
    }
  }
  show_lists();
}

void show_lists(void) {
  switch (STATUS) {
  case 1:
    show_menu("Mortal toggle commands:", toggles, NUMTOGGLE, 1);
    bprintf("Enter a letter or [return] for more options: ");
    strcpy(cur_player->cprompt, 
	   "Enter a letter or [return] for more options: ");
    break;
  case 2:
    show_menu("Mortal options:", opts, NUMOPTS, 0);
    if (plev(mynum) > LVL_WIZARD) {
      bprintf("Enter a letter or [return] for wizard toggles: ");
      strcpy(cur_player->cprompt,
	     "Enter a letter or [return] for wizard toggles: ");
    }
    else {
      bprintf("Enter a letter or [return] to exit: ");
      strcpy(cur_player->cprompt,
             "Enter a letter or [return] to exit: ");
    }
    break;
  case 3:
    show_menu("Wizard-only toggles:", wiztoggles, NUMWIZTOGGLE, 2);
    bprintf("Enter a letter or [return] for wizard options: ");
    strcpy(cur_player->cprompt,
	   "Enter a letter or [return] for wizard options: ");
    break;
  case 4:
    show_menu("Wizard-only options:", wizopts, NUMWIZOPTS, 0);
    bprintf("Enter a letter or [return] to exit: ");
    strcpy(cur_player->cprompt,
	   "Enter a letter or [return] to exit: ");
    break;
  default:
    exit_config();
    return;
  }
  replace_input_handler(configure);
}

void get_setins (char *input) {
  char buff[MAX_COM_LEN + 10];
  int num;

  if (input == NULL) {
    strcpy( cur_player->cprompt, "New message: " );
    replace_input_handler(get_setins);
  }
  else {
    num = cur_player->work2[0];
    sprintf(buff, "%s %s", wizoptwords[num], input);
    gamecom(buff, True);
    replace_input_handler(configure);
    return;
  }
}

