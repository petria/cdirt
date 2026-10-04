#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "kernel.h"
#include "sflags.h"
#include "bprintf.h"

void noswear(const char *);

char *fuck[] = {
  "&+Yf***&N",
  "&+Bprocreate&N",
  "&+W(expletive)&N"
};

char *shit[] = {
  "&+Ys***&N",
  "&+W(curse)&N",
  "&+R(bad word)&N"
};

char *ass[] = {
  "&+Ya**&N",
  "&+Wrear end&N",
  "&+Rbehind&N"
};

char *bitch[] = {
  "&+Yb****&N",
  "&+Rfemale dog in heat&N",
  "&+W(bad word)&N"
};

char *cunt[] = {
  "&+Yc***&N",
  "&+Ypoozle&N",
  "&+Wvaginal area&N",
  "&+W(naughty word)&N"
};

char *pussy[] = {
  "&+Mp****&N",
  "&+Mpussy cat&N",
  "&+Y*swear*&N",
  "&+Yfemale private part&N"
};

char *dick[] = {
  "&+Md***",
  "&+Mpenis",
  "&+Mwillie",
  "&+Msnookie"
};

char *piss[] = {
  "&+Rurinate",
  "&+Wwee-wee",
  "&+Ypee"
};

#define False 0
#define True 1

void wlower(char *word) {
   char *ptr;

   for (ptr = word ; *ptr ; ptr++)
     *ptr = tolower(*ptr);
}

void noswear(const char *srcstr) {
  char *oldword, *begptr, *endptr, *word;
  Text text = {0};
  char *wbuff, *srcbuff = COPY(srcstr);
  int oldlen, oldloc;

  if (!ststflg(mynum, SFL_NEWSTYLE))
    text_char(&text, '\n');

  oldword = strtok(srcbuff, " ");

  while (oldword) {
    wbuff = COPY(oldword);
    oldlen = strlen(oldword);
    wlower(oldword);

    if ((begptr = strstr(oldword, "fuck"))) {
      word = fuck[rand() % 3];
      endptr = begptr + 4;
    }
    else if ((begptr = strstr(oldword, "shit"))) {
      word = shit[rand() % 3];
      endptr = begptr + 4;
    }
    else if ((begptr = strstr(oldword, "ass"))) {
      word = ass[rand() % 3];
      endptr = begptr + 3;
    }
    else if ((begptr = strstr(oldword, "pussy"))) {
      word = pussy[rand() % 4];
      endptr = begptr + 5;
    }
    else if ((begptr = strstr(oldword, "bitch"))) {
      word = bitch[rand() % 3];
      endptr = begptr + 5;
    }
    else if ((begptr = strstr(oldword, "cunt"))) {
      word = cunt[rand() % 4];
      endptr = begptr + 4;
    }
    else if ((begptr = strstr(oldword, "dick"))) {
      word = dick[rand() % 4];
      endptr = begptr + 4;
    }
    else if ((begptr = strstr(oldword, "piss"))) {
      word = piss[rand() % 3];
      endptr = begptr + 4;
    }
    else {
      text_append(&text, wbuff);
      free(wbuff);
      oldword = strtok(NULL, " ");
      if (oldword) text_char(&text, ' ');
      continue;
    }

    oldloc = oldlen - strlen(begptr);

    if (oldloc)
      text_bytes(&text, oldword, oldloc);             /* add on head */

    text_append(&text, word);                           /* add word */
   
    if (endptr)
      text_append(&text, endptr);                       /* add on tail */

    free(wbuff);
    oldword = strtok(NULL, " ");
    if (oldword) text_char(&text, ' ');
  }
  text_reserve(&text, 0);
  bprintf("%s", text.data);
  free(text.data); free(srcbuff);
}

