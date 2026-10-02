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
  char str[300], wbuff[300], buff[1024], srcbuff[1024];
  int oldlen, oldloc;

  *buff = *str = 0;
  if (!ststflg(mynum, SFL_NEWSTYLE))
    strcpy(buff, "\n");

  strcpy(srcbuff, srcstr);
  oldword = strtok(srcbuff, " ");

  do {
    strcpy(wbuff, oldword);
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
      strcat(buff, wbuff);
      continue;
    }

    oldloc = oldlen - strlen(begptr);

    if (oldloc)
      strncat(buff, oldword, oldloc);             /* add on head */

    strcat(buff, word);                           /* add word */
   
    if (endptr)
      strcat(buff, endptr);                       /* add on tail */

  } while((oldword = strtok(NULL, " ")) && strcat(buff, " "));
 
  bprintf("%s", buff);
}

