#include "log.h"
#define ADD_LINE(x) strformat(x, has_color)
#define SHOWNAME(arg) ADD_LINE(fmbn(arg) != -1 ? arg : "Someone")
#define CRESET "\033[40m\033[0m"

void colorsel(int, int, char *);
void bprintf (char *, ...);
void pfilter (char *, Boolean);
void pfile (char *, Boolean);
void file_pager (char *, Boolean);
void strformat (char *, Boolean);
int count_colors(char *);
char * do_colorcode(char *, Boolean *, Boolean);
char *do_specialcode(char *, Boolean);
void apply_filecodes(char *);
void strip_color (char *dests, char *srcs);
void pager (char *);


