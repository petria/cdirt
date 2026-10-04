#include "vote.h"

void v_clean_up(void) {
  strcpy(cur_player->cprompt, (char *) build_prompt(mynum));
  cur_player->work = 0;
  bzero(cur_player->work2, 64);
  return;
} 

/* load previous votes for a player */
void load_votes(char *file_base) {
  FILE *vfptr;
  char vpath[PATH_LEN];
  char player[BUFF_LEN];

  sprintf(vpath, "%s/results.%s", VOTE_DIR, file_base);

  if ((vfptr = FOPEN(vpath, "r")) == NULL)
    return;
  else
    while(!feof(vfptr)) {
      fscanf(vfptr, "%s %d %d %d\n", player, &v_yes, &v_no, &v_abs);
      if (!strcmp(player, pname(mynum))) {  /* found, use these values */
	FCLOSE(vfptr);
	return;
      }
    }
  /* not found, defaults */
  v_yes = 0;
  v_no = 0;
  v_abs = 0;
  FCLOSE(vfptr);
}

void votecom(char *input) {
  int len;
  static char v_fname[6];

  if (cur_player->writer) {
    bprintf("Finish whatever you're writing first.\n");
    return;
  }

  if (input == NULL) {
                          /* God has access to other files */
    if ((brkword() != -1) && plev(mynum) > LVL_DEMI) {
      if (!strcmp (wordbuf, "uvote") || !strcmp (wordbuf, "ivote") ||
	  !strcmp (wordbuf, "vote")) {
	bprintf("&+C[Using file: %s]\n", wordbuf);
	strcpy(v_fname, wordbuf);
      }
      else {
	bprintf("No such voting file: %s", wordbuf);
	return;
      }
    }
    else {                                 /* use voting file based on rank */
      if (plev(mynum) > LVL_ARCHWIZARD)
	strcpy(v_fname, "uvote");
      else if (plev(mynum) > LVL_WIZARD)
	strcpy(v_fname, "ivote");
      else
	strcpy(v_fname, "vote");
    }
  
    bprintf(V_TITLE);             /* display title, voting list, prompt */
    inp_level = 1;
    cur_player->old_inp_handler = phandler(mynum);
    load_votes(v_fname);
    if (!show_list(v_fname))
      return;
    strcpy(cur_player->cprompt, V_QPROMPT);
   }

  else if (tolower(*input) == 'q')
    inp_level = 3;

  if (input != NULL)
    switch (inp_level) {
    case 1:                       /* asking for number, questions level */
      strcpy(cur_player->cprompt, V_QPROMPT);

      if (*input == '\0') {
	if (!show_list(v_fname))
          return;
	strcpy(cur_player->cprompt, V_QPROMPT);
      }

      else if (!is_num(input)) 
	bprintf("&+YThat's not a number... please try again.\n");

      else if ((number = atoi(input)) < 1 || number >= maxnum)
	bprintf("&+YNumber out of range... please try again.\n");

      else {
	strcpy(cur_player->cprompt, V_PROMPT);
	inp_level++;
      }
      break;
    case 2:
      len = strlen(input);
      inp_level = 1;
      strcpy(cur_player->cprompt, V_QPROMPT);
      if (len == 0) {
	bprintf("&+YType something please.\n");
	strcpy(cur_player->cprompt, V_PROMPT);
	inp_level = 2;
      }
      else if (!strncasecmp(input, "No", len)) {
	xsetbit(v_no, number);
	xclrbit(v_yes, number);
	xclrbit(v_abs, number);
	bprintf("&+B%s\n", nomsg[rand() % 10]);
      }
      else if (!strncasecmp(input, "Yes", len)) {
	xsetbit(v_yes, number);
	xclrbit(v_no, number);
	xclrbit(v_abs, number);
	bprintf("&+G%s\n", yesmsg[rand() % 10]);
      }
      else if (!strncasecmp(input, "Abstain", len)) {
	xsetbit(v_abs, number);
	xclrbit(v_no, number);
	xclrbit(v_yes, number);
	bprintf("&+C%s\n", absmsg[rand() % 10]);
      }
      else {
	bprintf("&+YThat's not an option&+W.\n");
	strcpy(cur_player->cprompt, V_PROMPT);
	inp_level = 2;
      }
      break;
    case 3:
      strcpy(cur_player->cprompt, "Show summary (y/n)? ");
      if (tolower(*input) == 'y') {
	if (plev(mynum) > LVL_DEMI) {
	  inp_level = 4;
	  strcpy(cur_player->cprompt, "Complete summary [god view] (y/n)? ");
	  break;
	}
	else {
	  tally_votes(True, False, v_fname);
	  v_clean_up();
	  return;
	}
      }
      else if (tolower(*input) == 'n') {
	replace_input_handler(cur_player->old_inp_handler);
	tally_votes(False, False, v_fname);
	v_clean_up();
	return;
      }
      break;
    case 4:
      if (tolower(*input) == 'y') {
	tally_votes(True, True, v_fname);
	return;
      }
      else if (tolower(*input) == 'n') {
	tally_votes(True, False, v_fname);
	return;
      }
      else
	bprintf("Please answer with yes or no.\n");
    }
  replace_input_handler(votecom);
}

Boolean valid_line(char *line) {
  int i = strlen(line);

  if (i > 0 && line[0] == '#')
    return False;

  for (; i >= 0 ; i--)
    if (!isspace(line[i]))
      return 1;
  return 0;
}

int is_num(char *line) {
  int i;
  
  if (line[0] == '\0')
    return 0;
  for (i = strlen(line) - 1 ; i > 0 ; i--)
    if (!isdigit(line[i]))
      return 0;
  return 1;
}

int show_list (char *file_base) {
  FILE *vfptr;
  char vpath[PATH_LEN];
  char line[LINE_LEN];

  strcpy(vpath, VOTE_DIR);
  strcat(vpath, "/");
  strcat(vpath, file_base);

  if ((vfptr = FOPEN(vpath, "r")) == NULL) {
    bprintf("Sorry, there isn't anything to vote on.\n");
    return(0);
  }
  /* determine number of voting choices */
  if (!maxnum)
    while (!feof(vfptr)) {
      fgets(line, LINE_LEN, vfptr);
      if (valid_line(line))
	maxnum++;
    }
  FCLOSE(vfptr);
  bprintf("\001f%s\003", vpath);
  return(1);
}

Boolean not_voted(int qnum) {
  if (!(xtstbit(v_yes, qnum) || xtstbit(v_no, qnum) || xtstbit(v_abs, qnum)))
    return True;
  else
    return False;
}

void tally_votes(Boolean show_summary, 
		 Boolean complete_summary, 
		 char *file_base) {

  FILE *vfptr, *tmpptr, *qptr;
  char vpath[BUFF_LEN], tmppath[BUFF_LEN], qpath[BUFF_LEN];
  char buff[BUFF_LEN];
  Voteptr votes[MAX_VOTES + 1] = {NULL};
  int i, yes, no, abs, qnum, nv, j;

  i = 0;
  tmpptr = NULL;

  replace_input_handler(cur_player->old_inp_handler);
  sprintf(vpath, "%s/results.%s", VOTE_DIR, file_base);
  if ((vfptr = FOPEN(vpath, "r")) != NULL) {
    p_vote record;
    while (i < MAX_VOTES && fscanf(vfptr, "%79s %d %d %d", record.player,
                                  &record.yes, &record.no, &record.abs) == 4) {
      if (!strcmp(record.player, pname(mynum))) continue;
      votes[i] = NEW(p_vote, 1); *votes[i++] = record;
    }
    FCLOSE(vfptr);
  }
  votes[i] = NEW(p_vote, 1);

  strcpy(votes[i]->player, pname(mynum));
  votes[i]->yes = v_yes;
  votes[i]->no = v_no;
  votes[i]->abs = v_abs;
  
  if (show_summary) {
    sprintf(tmppath, "%s/SUMMARY.%s", TEMP_DIR, pname(mynum));
    if ((tmpptr = FOPEN(tmppath, "w")) == NULL) {
      bprintf(TMP_ERROR);
      goto cleanup;
    }
    sprintf(qpath, "%s/%s", VOTE_DIR, file_base);
    if ((qptr = FOPEN(qpath, "r")) == NULL)
      goto cleanup;

    if (complete_summary)
      fprintf(tmpptr, "\n&+r[&+CGod view, full listing&+r]\n");
    
    for (qnum = 1 ; qnum < maxnum ; qnum++) {
      if (!fgets(buff, BUFF_LEN, qptr)) break;
      if (!valid_line(buff) || (not_voted(qnum) &&
	  !complete_summary))
	continue;
      fprintf(tmpptr, "\n");
      fprintf(tmpptr, "%s", buff);
      
      yes = no = abs = nv = 0;
      for (j = 0 ; j <= i ; j++) {
	if (complete_summary)
	  fprintf(tmpptr, "\n&+CPlayer: &N%-18s&+BVote:&N ", votes[j]->player);
	
	if (xtstbit(votes[j]->yes, qnum)) {
	  if (complete_summary)
	    fprintf(tmpptr, "Yes");
	  yes++;
	}
	
	else if (xtstbit(votes[j]->no, qnum)) {
	  if (complete_summary)
	    fprintf(tmpptr, "No");
	  no++;
	}
	
	else if (xtstbit(votes[j]->abs, qnum)) {
	  if (complete_summary)
	    fprintf(tmpptr, "Abstain");
	  abs++;
	}
	
	else {
	  if (complete_summary)
	    fprintf(tmpptr, "Not voted");
	  nv++;
	}
      }

      if (complete_summary || !not_voted(qnum))
	fprintf(tmpptr,
		"\n&+G%sYes           &+W:&+c%3d&N"
		"&+Y        %sNo            &+W:&+c%3d&N"
		"\n&+R%sAbstain       &+W:&+c%3d&N"
		"&+y        %sNot voted     &+W:&+c%3d\n",
		xtstbit(v_yes, qnum) ? "&=CB" : "", yes, 
		xtstbit(v_no, qnum) ? "&=CB" : "",  no, 
		xtstbit(v_abs, qnum) ? "&=CB" : "", abs, 
		not_voted(qnum) ? "&=CB" : "", nv);
    }
  }

  if (show_summary) FCLOSE(qptr);
  if ((vfptr = FOPEN(vpath, "w")) == NULL) {
    bprintf("Sorry, couldn't open vote file for writing\n");
    mudlog("ERROR: in tally_votes, unable to open vote results file "
	   "for writing.\n");
    goto cleanup;
  }

  for (j = 0 ; j <= i ; j++) {
    fprintf(vfptr, "%s %d %d %d\n", votes[j]->player, votes[j]->yes,
	    votes[j]->no, votes[j]->abs);
    FREE(votes[j]); votes[j] = NULL;
  }

  FCLOSE(vfptr);

  if (show_summary) {
    FCLOSE(tmpptr); tmpptr = NULL;
    v_clean_up();
    bprintf("\001f%s\003", tmppath);
  }
cleanup:
  if (tmpptr) FCLOSE(tmpptr);
  for (j = 0; j <= i; j++) free(votes[j]);
}
