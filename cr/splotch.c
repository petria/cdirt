/*******************************************************
   splotch.c a robot type thing Version 1.0

   7/17/1992 Duane Fields (dkf8117@tamsun.tamu.edu)
********************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "kernel.h"

#define tolow(A) (isupper(A)?(tolower(A)):(A))

#define NAME      "Bob"        /* name of robot */
#define HISTORY   4            /* number of slots in old key queue */
#define TEMPLSIZ  2000         /* maximum number of templates */
#define DICTFILE  DATA_DIR "/main.dict"  /* name of dictionary file */
#define DEBUG 0                /* debug flag */
#define VERBOSE 0              /* verbose errors */
#define BLANK 040

struct template {
  long    toffset;           /* start of responses in file */
  char    tplate[400];       /* the template itself */
  int     priority;          /* how important key is 1=worst, 9=most */
  int     talts;             /* number of alternate replies (>0) */
  int     tnext;             /* next reply (1 <= tnext <= talts) */
}         templ[TEMPLSIZ];

char   *response;        /* response to be returned */
char   my_nick[20];
char   *words;           /* a template has been matched, this is % */
FILE   *dfile;               /* file pointer to main dictionary file */
int    maxtempl;             /* templ[maxtempl] is last entry */
int    oldkeywd[HISTORY];    /* queue of indices of most recent keys */

char *strcasestr();
char *phrasefind();
void ask(char *, char *);
void init(void);
void buildtempl(void);
void usetempl(int);
int grline(char *, char *);
char *expand(char *);
void swap(char **, char *, char *);
void strlower(char *);
char *lower(char *);
void fixfile(char *);
extern char *strcasestr(char *, char *);
int trytempl(char *);
char *phrasefind(char *, char *);
void fix(void);
void gswap(char *, char *);
void shift(int, int);

/***************************************************************************
  ask(person, question) takes two null terminated strings, one is the
  question and the other is the person who asked the question.  The function
  returns the int value which represents the template used (0 or greater).
  if no match was found, a -1 is returned.  ask always sets the global
  variable "response" to contain an appropriate response, or if no match was
  found, an reply from the last entry in the dictionary. (a default reply)
 ***************************************************************************/

static void ask_expanded(char *, char *);
void ask(char *person, char *question) {
  char *expanded = expand(question);
  if (!words) words = COPY("");
  if (!response) response = COPY("");
  ask_expanded(person, expanded);
  free(expanded);
}

static void ask_expanded(person, question)
     char    person[20];            /* person who asked the question */
     char    question[400];         /* input line from user */

{
  int     i,j;
                                     /* expand also strlowers() */
  /* The public wrapper owns the expanded question. */

  if (DEBUG)
   fprintf(stderr, "<<expanded>>\n");

  i = trytempl(question);            /* look for a matching template */

  if (DEBUG)
    fprintf(stderr, "<<trytempl done, i=%i>>\n",i);


  if (i >=0)   {                     /* found a match, index=i */
    if (templ[i].priority == 6)  {    /* key words 6 */
      for (j=0; j < HISTORY-1; j++)  /* update history que */
        oldkeywd[j] = oldkeywd[j+1];
      oldkeywd[HISTORY-1]=i;
      }
    usetempl(i);                      /* build response */
    return;
  }
  else {                              /* no match found */

    if ((random() % 100)+1)         /* HISTORY NOT IMPEMENTED! */
       usetempl(maxtempl);          /* use a neutral response */
    else {                            /* use an old key */
      i = (random() % HISTORY);
      i = oldkeywd[i];
      if (DEBUG)
	fprintf(stderr, "<<History used, i=%i>>\n",i);


      if (i < 0)                      /* no history yet */
	usetempl(maxtempl);
      else usetempl(i);
    }
  }
}



/***************************************************************************
  init() sets up the template, zeros variables. Must be called by main prg
 ***************************************************************************/

void init()
{
  int    i;

  /* initialization section, zero array, open files */

  for (i=0; i < HISTORY; i++)
    oldkeywd[i] = -1;                /* no previous keywords */

  dfile = FOPEN(DICTFILE, "r");
  if (dfile == NULL) {
    fprintf(stderr, "ERROR: unable to read %s\n",DICTFILE);
    exit(1);
  }

  if (DEBUG)
    fprintf(stderr, "<<using dict file %s>>>\n", DICTFILE);

  srandom(getpid());   /* randomize seed */
  buildtempl();    /* read the templates, make 1 entry per template */

  if (DEBUG)
    fprintf(stderr, "<<templates built, seed chosen>>\n");
}





/**************************************************************************
  buildtempl() reads the MAINDICT file and fills in the template table.
  Each entry in the template table refers to a single template and all of
  its replies.
 ***************************************************************************/

void buildtempl()
{
  char    line[400];
  char    temp[400];
  int     i;


  i=0;   /* first template starts at zero */

  /* loop, one pass per template (including all replies) */
  while (i < TEMPLSIZ && !feof(dfile)) {
    fgets(line, sizeof(line), dfile);

    while (((line[0] == '#') || isspace(line[0])) && (!feof(dfile)))
      fgets(line, sizeof(line), dfile);

    if (feof(dfile))
      break;

    /* read in template */
    strcpy(templ[i].tplate, line);

    /* read priority */
    fgets(line, sizeof(line), dfile);

    if (atoi(line) == 0)  /* no entry */
      templ[i].priority = 9;                /* default priority */
    else
      templ[i].priority = atoi(line);

    /* set number of alternate replies to 0 */
    templ[i].talts = 0;

    /* count number or responses, start with asterisk */
    if (line[0] != '*')
      fgets(line, sizeof(line), dfile);

    /* where to find first response withing dictionary */
    templ[i].toffset = ftell(dfile) - strlen(line);

    while ((line[0] == '*') && !feof(dfile)) {
      templ[i].talts++;
      fgets(line, sizeof(line), dfile);
    }

    /* pick a random starting point for the responses */
    templ[i].tnext = templ[i].talts ? 1 + random() % templ[i].talts : 1;
    if (templ[i].tnext > templ[i].talts)
      templ[i].tnext = 1;

    strcpy(temp, templ[i].tplate);
    temp[strlen(temp)-1]='\0';

    if (VERBOSE)
      fprintf(stderr, "<<Template[%i]=:%s:>>\n",i,temp);


    i++;   /* next template */

    if (i >= TEMPLSIZ) {
      fprintf(stderr, "ERROR: template array too small\n");
      exit(1);
    }

  }

  /* all templates have been read and stored */
  maxtempl = i -1;
}




/**************************************************************************
 usetempl() a template has been sucessfully matched.  generate output
**************************************************************************/
void usetempl(i)
     int     i;
{
  int     k, n, p1,p2;
  char    text[400];
  char    text2[400];
  char    add[400];
  char    filename[400];
  char    new[400];
  char    *p;
  char    *pp;

  if (DEBUG)
    fprintf(stderr, "<<entering usetempl>>\n");

  fseek(dfile, templ[i].toffset, 0);	/* seek the template */
  fgets(text, sizeof(text), dfile);

  if (templ[i].tnext > templ[i].talts)
    templ[i].tnext = 1;
  n = templ[i].tnext;
  templ[i].tnext++;

  /* skip to the proper alternative */
  for (k = 1; k < n; k++)
    fgets(text, sizeof(text), dfile);

  /* make reply using template */
  free(response); response = COPY(text+1);
  if (*response && response[strlen(response)-1] == '\n')
    response[strlen(response)-1]='\0';

  if (words[0] != '\0') {

    p=strstr(words, ",");          /* chop off trailing cluases */
    if (p != NULL)
       p[0]='\0';

    p=strstr(words, ". ");
    if (p != NULL)
      p[0]='\0';

    p=strstr(words, "...");
    if (p != NULL)
      p[0]='\0';

    p=strstr(words, "?");
    if (p != NULL)
      p[0]='\0';


    p=strstr(words, ";");
    if (p != NULL)
       p[0]='\0';

    p=strstr(words, "!");
    if (p != NULL)
       p[0]='\0';

    p=strstr(words, ":");
    if (p != NULL)
      p[0]='\0';

    sprintf(text, " %s", lower(my_nick));
    sprintf(text2,"than %s", lower(my_nick));
    pp=strstr(words, text2);
    p=strstr(words, text);
    if ((p != NULL) && (pp == NULL))
       p[0]='\0';

    p=strstr(words, " please");
    if(p != NULL)
      p[0]='\0';


    if (*words && ispunct((unsigned char)words[strlen(words)-1]))
      words[strlen(words)-1]='\0';

    fix();                           /* fix grammer */

    swap(&response, "%", words);
  }

  swap(&response, lower(my_nick), "I");          /* should use NAME */
  swap(&response, "%", " ");
  for (i=0; words[i] != '\0'; i++)
    words[i]=words[i] & 0177;

/* fix problems with grammer */

  gswap("i are", "I am");
  gswap("me have", "I have");
  gswap("me don't", "I don't");
  gswap("you am", "you are");
  gswap("me am", "I am");
  gswap("i", "I");
  gswap("did not be", "weren't");

  for (i=0; words[i] != '\0'; i++)
    words[i]=words[i] & 0177;


  /* do file insertians */

  while ((p = strchr(response, '@'))) {
    char *dot = strchr(p + 1, '.');
    size_t length;
    Text result = {0};
    if (!dot || !dot[1]) break;
    length = (size_t)(dot - p) + 1;
    if (length >= sizeof(filename)) break;
    memcpy(filename, p + 1, length); filename[length] = 0;
    if (!grline(filename, add)) { free(response); response = COPY("hmmm"); return; }
    text_bytes(&result, response, p - response);
    text_append(&result, add); text_append(&result, dot + 2);
    free(response); response = text_take(&result);
  }

}







/*****************************/
/* gets random line from file*/
/*****************************/
int grline(char *infile, char *line) {
  FILE *fp;
  char *path = text_format("words/%s", infile);
  int count, chosen, i;
  fp = FOPEN(path, "r");
  if (!fp) { fprintf(stderr, "\n ERROR! could not open :%s: \n", infile); free(path); return 0; }
  do { if (!fgets(line, 256, fp)) { FCLOSE(fp); free(path); return 0; } }
  while (line[0] == '#' || isspace((unsigned char)line[0]));
  if (atoi(line) == 0) {
    FCLOSE(fp); fixfile(path); fp = FOPEN(path, "r");
    if (!fp) { free(path); return 0; }
    do { if (!fgets(line, 256, fp)) { FCLOSE(fp); free(path); return 0; } }
    while (line[0] == '#' || isspace((unsigned char)line[0]));
  }
  count = atoi(line);
  if (count <= 0) { FCLOSE(fp); free(path); return 0; }
  chosen = random() % count + 1;
  for (i = 0; i < chosen; i++) if (!fgets(line, 256, fp)) { FCLOSE(fp); free(path); return 0; }
  if (*line && line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = 0;
  FCLOSE(fp); free(path); return 1;
}







/***************************************************************************
   expand() takes a string pointer.  Using they syn.dict file (format is
   given in the syn.dict file itself) it expands synonyms.
****************************************************************************/
char *expand(char *input) {
  char *s = COPY(input), *old, *replacement;
  char line[255];
  FILE *fp;
  strlower(s);
  fp = FOPEN("syn.dict", "r");
  if (!fp) { fprintf(stderr, "ERROR: Could not open the file syn.dict\n"); return s; }
  while (fgets(line, sizeof(line), fp)) {
    if (line[0] == '#' || isspace((unsigned char)line[0])) continue;
    strlower(line);
    replacement = strtok(line, ":\n");
    if (!replacement) continue;
    while ((old = strtok(NULL, ":\n"))) swap(&s, old, replacement);
  }
  FCLOSE(fp); return s;
}





/*********************************************************************
   swap() takes a string pointer and a two words.  All occurances of
   the first word are replaced by the second word.
 *********************************************************************/

void swap(char **target, char *old, char *replacement) {
  Text text = {0};
  const char *start = *target, *s = start, *hit;
  size_t n = strlen(old);
  if (!n) return;
  while ((hit = strstr(s, old))) {
    text_bytes(&text, s, hit - s);
    if ((isspace((unsigned char)hit[n]) || ispunct((unsigned char)hit[n]) || !hit[n]) &&
        (hit == start || isspace((unsigned char)hit[-1]) || ispunct((unsigned char)hit[-1]))) {
      text_append(&text, replacement); s = hit + n;
    } else { text_char(&text, *hit); s = hit + 1; }
  }
  text_append(&text, s);
  free(*target); *target = text_take(&text);
}




/*********************************************************************
   strlower() takes a string pointer and makes that string lowercase
 *********************************************************************/

void strlower(s)
char *s;
{
   register int i;

   for (i=0; s[i]; ++i)
       s[i] = tolower (s[i]);

}




char *lower(char *s) {
  static char *scratch;
  char *p;
  free(scratch); scratch = COPY(s);
  for (p = scratch; *p; p++) *p = tolower((unsigned char)*p);
  return scratch;
}


/***************************************************************************
fixfile will count the non-blank lines in a file and put the count as the
first line of the file.  Uses a tmp file called "tmp"
***************************************************************************/

void fixfile(fname)
char fname[50];

{
  int count;
  FILE *tmpfp;
  FILE *fp;
  char line[255];
  char tline[255];

  count=0;

  fp=FOPEN(fname, "r");
  if (fp  == NULL)
    {
      fprintf(stderr, "ERROR: Could not open %s file\n",fname);
      return;
    }
  fgets(line,255,fp);
  while (!feof(fp))
    {
      if (!((line[0] == '#') || (isspace(line[0]))))
	count++;
      fgets(line,255,fp);
    }
  rewind(fp);
  tmpfp=FOPEN("tmp","w");
  if (tmpfp == NULL)
    {
      fprintf(stderr, "ERROR: Could not create tmp file\n");
      return;
    }
  fgets(line, 255, fp);

  while ((line[0] == '#') || (isspace(line[0])))
    {
      fputs(line,tmpfp);
      fgets(line,255,fp);
    }
  if (atoi(line)==0)
    {
      count=count-1;
      sprintf(tline, "%i\n",count);
      fputs(tline, tmpfp);
    }
  else
    {
      sprintf(tline, "%i\n",count);
      fputs(tline, tmpfp);
      fputs(line, tmpfp);
    }
  fgets(line,255,fp);
  while (!feof(fp))
    {
      fputs(line,tmpfp);
      fgets(line,255,fp);
    }
  FCLOSE(tmpfp);
  FCLOSE(fp);

  fp=FOPEN(fname, "w");
  tmpfp=FOPEN("tmp", "r");
  fgets(line,255,tmpfp);
  while (!feof(tmpfp))
    {
      fputs(line,fp);
      fgets(line,255,tmpfp);
    }
  FCLOSE(tmpfp);
  FCLOSE(fp);
}






/***************************************************************************
 trytempl() will try to match the current line to a template.  In turn, try
 all templates to see if if they find a match.
***************************************************************************/

int trytempl(question)
     char question[];
{
  int     i;
  char    t[400];
  int     found,done;
  char    *key;
  char    key1[400];                 /* first half of a % template */
  char    key2[400];                 /* second half of a % template */
  char    *winwords = COPY("");
  int     firstime;
  int     p,j;
  char    *p1,*p2;
  int     score;                     /* current highest priority */
  int     winner;                    /* current high scorer */

  winner=-1;
  found=0;
  done=0;
  score=0;
  key=0;
  words[0] = '\0';                    /* the % words */
  firstime=1;

  for (i=0; i <= maxtempl; i++) {     /* loop through all templates */
    done=0;
    strcpy(t, templ[i].tplate);
    firstime=1;

    while (done == 0) {
      if (firstime) {
	key = strtok(t, ":\n");
	firstime=0;
      }
      else
	 key=strtok(NULL, ":\n");
      if (key == NULL) {
	done=1;
	break;
      }
      switch (key[0]) {
      case '!': if (phrasefind(question, key+1) != NULL) {
	           found=0;
		   done=1;
		 }
	        break;
      case '&': if (phrasefind(question, key+1) == NULL) {
	           found=0;
		   done=1;
		 }
	        break;
      case '+': if (phrasefind(question, my_nick) == NULL) {  /* name */
                   found=0;
                   done=1;
                 }
                break;
      default:  if (!strstr(key, " %")) {    /* regular template, no % */
	           if (phrasefind(question, key))
		     found=1;
		   break;
		 }
	        else {     		  /* is a % in template */
		  key1[0]=key[0];
		  p=0;
		  j=0;
		  while ((key1[p++]=key[j++])!='\%');
		  key1[p-2]='\0';

		  if ((key[p] != '\0') && (key[p] != '\n') )  { /* xxx % xxx */
		    strcpy(key2, &key[p+1]);
		    j=0;
		    p1=phrasefind(question, key1);
		    if (p1 != NULL)
		      if (!ispunct(p1[strlen(p1)-1])) {
			p2=phrasefind(p1+1, key2);
			if (p1 != NULL && p2 != NULL && p2 - p1 >= strlen(key1) + 2) { /* both keys found */
			  found=1;
			  free(words); words = COPY(p1+strlen(key1)+1);
			  words[p2-p1-strlen(key1)-2]='\0'; /* -2 for spaces */
			}
		      }
		  }
		  else {		    /* xxxx % */
		    p1=phrasefind(question, key1);
		    if ((p1 != NULL)&& p1[strlen(key1)] != '\0')
		      {
		      /* if (!ispunct(p1[strlen(p1)-1])) { */
			found=1;
			free(words); words = COPY(p1+strlen(key1)+1);
		      }
		  }
		}
      }
    }

    if (found && done)
      {
        if (templ[i].priority >score) {
          winner=i;
	  score=templ[i].priority;
	  free(winwords); winwords = COPY(words);
/*	  printf ("the new template is %s\n",templ[i].tplate); */
	}

	if (templ[i].priority == 9) {
	  free(words); words = COPY(winwords);
	  free(winwords);
  return(winner);
	}
      }
    found=0;
    done=0;
  }
  free(words); words = COPY(winwords);
  free(winwords);
  return(winner);
}








/**************************************************************************
 phrasefind()
**************************************************************************/
char *phrasefind(string, searchstring)
char *string;
char *searchstring;
{

        while (!isalnum(*string) && (*string != '\0')) string++;
        while (isalnum(*string))
        {
                if (!strncasecmp(string, searchstring, strlen(searchstring)) &&
                        !isalnum(*(string + strlen(searchstring))))
                        return string;
                while (isalnum(*string)) string++;
                while ((*string != '\0') && !isalnum(*string)) string++;
        }
        return NULL;
}

#if 0
int strncasecmp (a, b, n)
char *a;
char *b;
int   n;
{

        for (; (*a != '\0') && (*b != '\0') &&
                (tolow (*a) == tolow (*b)) && (n  > 0); a++, b++, n--);
        if (n == 0)
                return 0;
        return (tolow (*a) - tolow (*b));
}
#endif


void fix()
{
  int i;

  gswap("that you", "that I");
  gswap("my", "your");
  gswap("you", "me");
  gswap("your", "my");
  gswap("me", "you");
  gswap("mine", "yours");
  gswap("am", "are");
  gswap("yours", "mine");
  gswap("yourself", "myself");
  gswap("myself", "yourself");
  gswap("are", "am");
  gswap("i", "you");

  for (i=0; words[i] != '\0'; i++)
    words[i] = (words[i] & 0177);              /* readjust parity */

}



/*
scan the array "words" for the string OLD.  if it occurs,
replace it by NEW.  also set the parity bits on in the
replacement text to  mark them as already modified
*/
void gswap(old, new)
	char    old[], new[];
{
	int     i, nlen, olen, flag, base, delim;
	olen = 0;
	while (old[olen] != 0)
		olen++;
	nlen = 0;
	while (new[nlen] != 0)
		nlen++;

	for (base = 0; words[base] != 0; base++) {
                if ((size_t)olen > strlen(words + base)) break;
		flag = 1;
		for (i = 0; i < olen; i++)
			if (old[i] != words[base + i]) {
				flag = 0;
				break;
			}
		delim = words[base + olen];
		if (flag == 1 && (base == 0 || words[base - 1] == BLANK)
		    && (delim == BLANK || delim == '\n' || delim == 0)) {
			shift(base, nlen - olen);
			for (i = 0; i < nlen; i++)
				words[base + i] = new[i] + 128;
		}
	}
}



void shift(int base, int delta) {
  size_t len = strlen(words);
  char *next;
  if (!delta) return;
  if (base < 0 || (size_t)base > len || (delta < 0 && (size_t)(-delta) > len - base)) return;
  if (delta > 0) {
    next = memory_alloc(memory_add(memory_add(len, (size_t)delta), 1), 1);
    memcpy(next, words, base);
    memcpy(next + base + delta, words + base, len - base + 1);
    free(words); words = next;
  } else memmove(words + base, words + base - delta, len - base + delta + 1);
}

