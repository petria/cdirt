#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "sflags.h"
#include "sendsys.h"
#include "mobile.h"
#include "bprintf.h"
#include "pig.h"

/* xrpig.c: piglatin for abermud, recursive version */

Boolean is_special(char c) {
  if (!isalpha(c) && !isspace(c))
    return True;
  else
    return False;
}

void eng2pig (char *deststr, char *srcstr) {
  char *endptr;
  char *dest;
  char word[300];
  char *ptr;
  int i, j;

  ptr = NULL;
  endptr = strchr(srcstr, ' ');
  dest = strlen(deststr) + deststr;

  /* copy all normal characters to a word, have a ptr to special chars */

  for (i = 0, j = 0 ; !isspace(srcstr[i]) && srcstr[i] != '\0' ; i++) {
    if (is_special(srcstr[i])) {
      if (!ptr)
	ptr = &srcstr[i];
    }
    else {
      word[j] = srcstr[i];
      j++;
    }
  }
  word[j] = '\0';

  if (is_vowel(tolower(word[0]))) {
    strcpy(dest, word);
    dest += strlen(word);
    strcpy(dest, "-way");
    dest += 4;
  }
  else if (*word != '\0') {
    if (strlen(word) > 1) {
      for (i = 1 ; word[i] != '\0' ; i++)
	if (is_vowel(word[i]))
	  break;

      strcpy(dest, &word[i]);
      dest += strlen(&word[i]);
      *dest++ = '-';
      *dest = '\0';
      word[i] = '\0';
      strcpy(dest, word);
      dest += strlen(word);
      strcpy(dest, "ay"); 
      dest += 2;
    }
    else {              /* len = 1 */
      *dest++ = word[0];
      *dest = '\0';
      strcpy(dest, "-ay");
      dest += 3;
    }
  }
  
  /* add on the tail */
  if (ptr)
    for (; *ptr != 0 && is_special(*ptr) ; ptr++)
      *dest++ = *ptr;

  if (endptr != NULL) {
    *dest++ = ' ';
    *dest = '\0';
    eng2pig(dest, endptr + 1);
  }
  else
    *dest = '\0';
}


void pigcom(void) {
  char buff[MAX_COM_LEN * 3] = {'\0'};

  if (ststflg (mynum, SFL_NOPIG)) {
    bprintf ("You must be listening to the piglatin channel to talk.\n");
    return;
  }
  if (EMPTY (txt1)) {
    bprintf ("Oink!\n");
    return;
  }

  eng2pig(buff, txt1);

  send_msg (DEST_ALL, MODE_NSFLAG | MS (SFL_NOPIG), LVL_MIN, LVL_MAX,
            NOBODY, NOBODY, "&+M\001p%s\003&N pig-chats, &+W\"&+m%s&+W\"\n",
            pname (mynum), buff);
}

void makepigcom(void) {
  int plr;

  if (plev(mynum) < LVL_WIZARD) {
    bprintf("Try cooking some bacon instead.\n");
    return;
  }
  else if ((plr = fpbn(txt1)) == -1) {
    bprintf("You want to turn WHO into a pig?\n");
    return;
  }
  else if (ptstflg(plr, SFL_NOPIG)) {
    bprintf("%s doesn't want to be turned into a pig.\n", pname(plr));
    return;
  }
  else {
    if (!ststflg(plr, SFL_ISPIG)) {
      sendf(plr, "&+M%s turned you into a pig!\n", pname(mynum));
      bprintf("&+MYou turn %s into a pig!\n", txt1);
      ssetflg(plr, SFL_ISPIG);
    }
    else 
      bprintf("&+M%s is already a pig! Oink!\n", pname(plr));
  }
}

Boolean is_vowel(char c) {
  switch (tolower(c)) {
  case 'a': case 'e': case 'i': case 'o': case 'u': case 'y':
    return True;
  default:
    return False;
  }
}
