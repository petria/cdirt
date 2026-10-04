#include <stdarg.h>
#include <sys/file.h>
#include <stdlib.h>
#include "kernel.h"
#include "sflags.h"
#include "mobile.h"
#include "mud.h"
#include "rooms.h"
#include "bprintf.h"
#include "audit.h"

char color_table[] =
{                               /* Not beautiful, but efficient :) */
  0, 0, '4', '6',
  0, 0, 0, '2',
  0, 0, 0, 0,
  '0', '5', 0, 0,
  0, 0, '1', 0,
  0, 0, 0, '7',
  0, '3', 0, 0,
  0, 0, 0, 0,
  0, 0, '4', '6',
  0, 0, 0, '2',
  0, 0, 0, 0,
  '0', '5', 0, 0,
  0, 0, '1', 0,
  0, 0, 0, '7',
  0, '3', 0, 0,
  0, 0, 0, 0,
};

#define colorcode(x) ((x >= 64 && x <= 127) ? color_table[x-64]: 0)

unsigned char *dest;

void strip_color (char *dests, char *srcs) {
  unsigned char *out = dests;
  unsigned char *src = srcs;

  while (*src != 0) {
    if (*src != '&') {
      *out++ = *src++;
    } 
    else {
      switch (*(src + 1)) {
      case '&':
        src += 2; *out++ = '&';
        continue;
      case '+': case '-':
        if (colorcode (*(src + 2)))
          src += 3;
        else
          *out++ = *src++;
          continue;
      case '=':
        if (src[2] && colorcode(src[2]) && src[3] && colorcode(src[3]))
	  src += 4;
	else                                      
          *out++ = *src++;    
        continue;
      case '*': case 'B': case 'N': case 'n': case '#':
        src += 2;          
        continue;
      default:                                    
        *out++ = *src++;    
        continue;
      }
    }
  }
  *out = 0;
}

int count_colors(char *inp) {
  int nchars = 0;

  unsigned char *src = inp;

  while (*src != 0) {
    if (*src != '&')
      src++;
    else {
      switch (*(src + 1)) {
      case '+': case '-':
        if (colorcode (*(src + 2)))        
          nchars += 3;
        src++;
        continue;
      case '=':
        if (src[2] && colorcode(src[2]) && src[3] && colorcode(src[3]))
          nchars += 4;
        src ++;
        continue;
      case '*': case 'B': case 'N': case '#': case '&': case 'n':
        nchars += 2;
        src++;
        continue;
      default:                                   
        src++;
        continue;
      }
    }
  }
  return nchars;
}

void snoop_txt(int me, char *buffer) {
  Boolean has_color;
  char snoopbuff[50];
  int plx;

  for (plx = 0 ; plx < max_players ; plx++)
    if (snooptarget(plx) == me && is_in_game(plx)) {
      setup_globals(plx);
      has_color = ststflg(real_mynum, SFL_COLOR);
      dest = NULL;
      if (out_write(real_mynum) == out_buffer(real_mynum)) {
        sprintf(snoopbuff, "&+r[&+WSnoop: &+M%s&+r]&N ",
          pname(snooptarget(real_mynum)));
        ADD_LINE(snoopbuff);
      }
      ADD_LINE(buffer);
      output(real_mynum) = True;
      out_write(real_mynum) = dest;
    }
  setup_globals(me);
}

/* main output function, maximum length of all args passed is M_BUFFLEN */

void bprintf (char *format, ...) {
  va_list pvar;
  char *buffer;
  int me = real_mynum;
  Boolean has_color;

  if (me < 0 || me >= max_players || cur_player == NULL || !is_conn(me))
    return;

  has_color = ststflg(me, SFL_COLOR);
  va_start(pvar, format);
  buffer = text_vformat(format, pvar);
  va_end(pvar);

  dest = NULL;
  ADD_LINE(buffer);
  output(me) = True;
  out_write(me) = dest;

  if (islogged(me))
    write_plr_log(buffer);  
  if (snooped(me))
    snoop_txt(me, buffer);
  free(buffer);
}

/* colorsel: pick a color for a string based on a percentage */

void colorsel(int cur, int max, char *str) {
  char mod[4] = "&+";
  float percent;
  char newstr[500];

  percent = ((float) cur / (float) max) * 100;

  if (percent < 11)        mod[2] = 'r';
  else if (percent < 22)   mod[2] = 'R';
  else if (percent < 33)   mod[2] = 'Y';
  else if (percent < 44)   mod[2] = 'b';
  else if (percent < 55)   mod[2] = 'B';
  else if (percent < 66)   mod[2] = 'c';
  else if (percent < 77)   mod[2] = 'C';
  else if (percent < 88)   mod[2] = 'g';
  else                     mod[2] = 'G';
  mod[3] = '\0';
  strcpy(newstr, mod);
  strcat(newstr, str);
  strcat(newstr, "&+w");
  strcpy(str, newstr);
}

void init_memory(void) {
  int readlen = out_read(real_mynum) - out_buffer(real_mynum);
  int writelen;

  if (!dest) {
    writelen = out_write(real_mynum) - out_buffer(real_mynum);
    dest = out_write(real_mynum);
  }
  else writelen = dest - out_buffer(real_mynum);

  if (out_size(real_mynum) - writelen < HIGH_MARK) {
    if (out_size(real_mynum) > INT_MAX - M_BUFFLEN) memory_failure();
    out_buffer(real_mynum) = (unsigned char *)
      resize_array(out_buffer(real_mynum), sizeof(char),
  	 	   out_size(real_mynum), M_BUFFLEN + out_size(real_mynum));

    out_write(real_mynum) = dest = out_buffer(real_mynum) + writelen;
    out_read(real_mynum) = out_buffer(real_mynum) + readlen;
    out_size(real_mynum) += M_BUFFLEN;
  }
}


/* strformat: add/format a string (with color) onto dest */

void strformat (char *srcs, Boolean has_color) {
  register unsigned char *src = srcs;
  Boolean is_color = False;
  
#ifdef CDIRT_AUDIT
  static int audit_format_depth;
  int audit_player = real_mynum;
  size_t audit_offset;
#endif

  init_memory();
#ifdef CDIRT_AUDIT
  audit_offset = dest - out_buffer(audit_player);
  audit_format_depth++;
#endif

  while (*src != 0) {
    init_memory();
    if (*src != '&' && !(iscntrl(*src)))                /* normal case */
      *dest++ = *src++;
    else if (*src == '\n') {                            /* newline needs LF */
      *dest++ = *src++; *dest++ = '\r';
      if (is_color) {
	strcpy ((char *) dest, CRESET);
	is_color = False;
	dest += 9;
      }
    }
    else if (*src == '&')                              /* Color code */
      src = do_colorcode(src, &is_color, has_color);
    else
      src = do_specialcode(src, has_color);            /* \001[X][Arg] */
  }
  init_memory();
  if (is_color) {                                /* reset text after line */
    strcpy ((char *) dest, CRESET);
    is_color = False;
    dest += 9;
  }
  *dest = 0;
#ifdef CDIRT_AUDIT
  if (!--audit_format_depth)
    audit_output(audit_player, out_buffer(audit_player) + audit_offset,
                 dest - out_buffer(audit_player) - audit_offset);
#endif
}

char * do_colorcode(char *srcs, Boolean *is_color, Boolean has_color) 
{
  unsigned char *src;
  src = srcs;

  switch (*(src + 1)) {
  case '&':
    src += 2; *dest++ = '&';                        
    break;
  case '+':
    if (colorcode (*(src + 2))) {
      if (has_color) {
	strcpy ((char *) dest, "\033[1;30m");
	dest[5] = colorcode (*(src + 2));
	if (*(src + 2) >= 96)  dest[2] = '0';
	dest += 7;
	*is_color = True;
      }
      src += 3;
    } else
      *dest++ = *src++;                                  
    break;
  case '-':
    if (colorcode (*(src + 2))) {
      if (has_color) {
	strcpy ((char *) dest, "\033[1;40m");
	dest[5] = colorcode (*(src + 2));
	if (*(src + 2) >= 96)  dest[2] = '0';
	dest += 7;
	*is_color = True;
      }
      src += 3;
    }
    else *dest++ = *src++;
    break;
  case '=':
    if (src[2] && colorcode(src[2]) && src[3] && colorcode(src[3])) {
      if (has_color) {
	strcpy ((char *) dest, "\033[1;40;30m");
	dest[5] = colorcode (*(src + 3));
	dest[8] = colorcode (*(src + 2));
	if (*(src + 2) >= 96)
	  dest[2] = '0';
	dest += 10;
	*is_color = True;
      }
      src += 4;
    }
    else *dest++ = *src++;
    break;
  case '*': case 'N': case 'n':
    if (has_color) {
      strcpy ((char *) dest, CRESET);
      dest += 9;
      *is_color = False;
    }
    src += 2;
    break;
  case 'B':
    if (has_color && !ststflg (real_mynum, SFL_NOBLINK)) {
      strcpy ((char *) dest, "\033[5m");
      dest += 4;
      *is_color = True;
    }
    src += 2;
    break;
    /* Carriage return */
  case '/':
    strcpy ((char *) dest, "\r\n");
    dest += 2; src += 2;
    break;
  case '#':
    if (!ststflg (real_mynum, SFL_NOBEEP)) {
      strcpy ((char *) dest, "\a");
      dest++;
    }
    src += 2;
    break;
  default: 
    *dest++ = *src++;
    break;
  }
  return src;
}

char *do_specialcode(char *srcs, Boolean has_color) {
  char *delim, *src;
  char *arg = NULL;
  int i;

  src = srcs;

  if (*src == 001) {                               /* special code */
    delim = ++src;
    if (!*src) return src;
    src++;
    { char *start = src;
      while (*src && *src != 003) src++;
      arg = NEW(char, (size_t)(src - start) + 1);
      memcpy(arg, start, (size_t)(src - start));
    }
    if (*src == 003) src++;

    switch(*delim) {
    case 'A':
      if (is_conn(real_mynum) && has_color)                 
	ADD_LINE(arg);
      break;
    case 'C':
      ADD_LINE("\033[2J\033[H");
      break;
    case 'D':
      if (!ststflg(mynum, SFL_BLIND))                    
	SHOWNAME(arg);
      break;
    case 'E':                             /* IAC Echo */
      strcpy(dest, "\377\374\001");
      dest += 4;
      break;
    case 'F':  
      pfilter(arg, has_color);
      break;
    case 'N':
      ADD_LINE((fmbn (arg) == mynum) ? "you" : arg);    
      break;
    case 'P':
      if (!ststflg(mynum, SFL_DEAF)) 
	SHOWNAME(arg);
      break;
    case 'R':                              /* IAC NO Echo */
      strcpy(dest, "\377\373\001");
      dest += 4;
      break;
    case 'V':
      ADD_LINE (VERSION);
      break;
    case '@':
      ADD_LINE (EMAIL);
      break;
    case '$':
      ADD_LINE (MUD_NAME);
      break;
    case 'U':
      ADD_LINE (MASTERUSER);
      break;
    case '#':
      { char *port_text = text_format("%d", PORT);
        ADD_LINE(port_text); free(port_text); }
      break;
    case 'T':
      ADD_LINE (LASTBUILD);
      break;
    case 'H':
      ADD_LINE (_HOSTNAME_);
      break;
    case 'O':
      ADD_LINE (_OS_);
      break;
    case '~':
      ADD_LINE (_ARCH_);
      break;
    case 'c':
      if ((!isdark ()) && (!ststflg (mynum, SFL_BLIND)))  
	ADD_LINE (arg);
      break;
    case 'd':
      if (!ststflg(mynum, SFL_DEAF))                     
	ADD_LINE(arg);
      break;
    case 'f':
      file_pager(arg, has_color);
      break;
    case 'n':
      if (fmbn (arg) == mynum)            
	ADD_LINE ("your");
      else {                   
	ADD_LINE (arg); 
	ADD_LINE ("'s");
      }
      break;
    case 'p':
      SHOWNAME(arg);
      break;
    }
  }
  else
    *dest++ = *src++;                       /* something unknown */
  free(arg);
  return src;
}

void file_pager (char filename[512], Boolean has_color) {
  FILE *file;
  if (!ppager (mynum)) {
    pfile (filename, has_color);
    return;
  }
  if ((file = FOPEN(filename, "r")) == NULL) {
    mudlog ("ERROR: Cannot read file: %s", filename);
    return;
  }
  cur_player->pager.old_handler = (INP_HANDLER *) phandler(real_mynum);
  strcpy(cur_player->pager.prompt, cur_player->cprompt); /* store old prompt */
  strcpy(cur_player->cprompt, qwait);
  cur_player->inpager = True;
  cur_player->pager.file = file;
  replace_input_handler (pager);
  pager (NULL);
}

void quit_pager (void) {
  cur_player->inpager = False;
  replace_input_handler ((void *) cur_player->pager.old_handler);
  FCLOSE(cur_player->pager.file);
  strcpy(cur_player->cprompt, cur_player->pager.prompt);
}

char *apply_filecodes(const char *inputstr) {
  Text text = {0};
  const char *p;
  for (p = inputstr; *p; p++) {
    if (*p != '&') text_char(&text, *p);
    else {
      switch (*(p+1)) {
        case 'V':
          text_append(&text, VERSION);
          break;
        case '@':
          text_append(&text, EMAIL);
          break;
        case '$':
          text_append(&text, MUD_NAME);
          break;
        case 'U':
          text_append(&text, MASTERUSER);
          break;
        case '?':
          text_append(&text, HOMEPAGE);
          break;
        case '#':
          { char *port_text = text_format("%d", PORT); text_append(&text, port_text); free(port_text); }
          break;
        case 'T':
          text_append(&text, LASTBUILD);
          break;
        case 'H':
          text_append(&text, _HOSTNAME_);
          break;
        case 'O':
          text_append(&text, _OS_);
          break;
        case '~':
          text_append(&text, _ARCH_);
          break;
        default:
          text_char(&text, *p);
          continue;
      }
      p++;
    }
  }
  return text_take(&text);
}

void pager (char *c) {
  Boolean has_color = ststflg(real_mynum, SFL_COLOR);
  char ch[256];
  int ct;

  if (c == NULL || tolower(*c) != 'q') {
    for (ct = 1 ; ct < ppager(real_mynum) ; ct++) {
      if (fgets(ch, sizeof(ch), cur_player->pager.file)) {
        if (c == NULL) {
          char *expanded = apply_filecodes(ch);
          ADD_LINE(expanded); free(expanded);
        }
        else
          bprintf("%s", ch);
      }
      else {
        quit_pager();
        return;
      }
    }
  }
  else
    quit_pager();
}

void pfile (char *filename, Boolean has_color) {
  FILE *a;
  char x[BUFSIZ];

  if ((a = FOPEN(filename, "r")) == NULL)
    mudlog ("ERROR: Cannot read file: %s", filename);
  else {
    while (fgets (x, sizeof (x), a)) {
      if (!feof(a)) {
        char *expanded = apply_filecodes(x);
        ADD_LINE(expanded); free(expanded);
      }
      else break;
    }
    FCLOSE(a);
  }
}

void pfilter (char *arg, Boolean has_color) {
  FILE *a;
  char x[BUFSIZ];

  if ((a = popen (arg, "r")) == NULL)
    fprintf (stderr, "[Cannot find filter ->%s]\n", arg);
  else {
    while (fgets (x, sizeof x, a))
      ADD_LINE (x);
    pclose (a);
    *dest = 0;
  }
}
