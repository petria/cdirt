/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * *
 *                       DIRT 3.0.x (Version of AberMUD)                     *
 *                                                                           *
 *                   Utility program for AberMUD [Dirt 3.0]                  *
 *                                                                           *
 *  Dumps the user-activity-file (uaf) given as argument in human-readable   *
 *  form to the standard output. If no argument is given, the file UAF_RAND  *
 *  is assumed.                                                              *
 *                                                                           *
 *  Options:  -l [level]  -Level. Only show players >= the specified level.  *
 *                         The default is 1, but ignore level if -a is on.   *
 *                         This option is really unneccesary on UNIX systems,*
 *                         I'd like to remove it, but it's there...          *
 *                                                                           *
 *            -f          -Flags. Show full names of flags. Default is hex.  *
 *                         (Not imlemented yet.)                             *
 *                                                                           *
 *            -h          -Header. Include a header that explains the        *
 *                         columns. Useful if the standard output is a tty.  *
 *                                                                           *
 *            -d [days]   -Show only players who have been on at least once  *
 *                         the last specified number of days.                *
 *                                                                           *
 *            -a          -All. Do not skip entries with empty names. For    *
 *                         debugging purposes where we want the whole file.  *
 *                                                                           *
 *  Gjermund S. (Nicknack), March 1991                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include "kernel.h"
#include "macros.h"
#include "sflags.h"
#include "pflags.h"
#include "pflagnames.h" 

extern int getopt(int argc, char **argv, char *optstr);
extern int optind;
extern char *optarg;

void usage(char *progname);

int main( int argc, char *argv[] )
{
  FILE *       fl;
  int          option;
  time_t       earliest;
  struct dirent *dp;
  DIR           *dir_pointer;
  char c;
  char dirname[200];
  char filename[200];
  char line[MAX_COM_LEN];
  char *opr, *val;
  int level, score, strength;
  long int pflags_l, pflags_h, sflags_l;
  const int    secs_in_a_day = 86400; 
  int          incl = 0, flags = 0, debug = 0, proglevel = 1, days = 0;


  while ((option = getopt(argc, argv, "ad:l:fh")) != -1) 
  {
    switch (option) 
    {
      case 'a':
        debug++;
        break;
      case 'h':
        incl++;
        break;
      case 'f':
        flags++;
        break;
      case 'l':
        proglevel = atoi(optarg);
        break;
      case 'd':
        days = atoi(optarg);
        break;
      case '?':
        usage(*argv);
        exit(1);
    }
  }

  if (days > 0)
    earliest = time((time_t *)NULL) - days * secs_in_a_day;

  if (incl) 
  {
    printf("Name        Sex      Level        Score      Strength");
    printf("           Pflags.n------------------------");
    printf("----------------------------------------------------\n\n");
  }

  for (c = 'A'; c <= 'Z'; c++) {
    sprintf(dirname, "%s/%c", TEXT_PFILES_BASE, c);
    dir_pointer = opendir(dirname);
    for (dp = readdir(dir_pointer); dp != NULL; dp = readdir(dir_pointer)) {
      if (dp->d_name[0] == '.')
	continue;

      sprintf(filename, "%s/%s", dirname, dp->d_name);
      fl = fopen(filename, "r");
      
      while(!feof(fl)) {
	fgets(line, MAX_COM_LEN, fl);
	opr = strtok(line, " ");
	val = strtok(NULL, " ");
	if (val != NULL) {
	  if(!strcmp(opr, "Score"))
	    sscanf(val, "%d", &score);
	  if(!strcmp(opr, "Level"))
	    sscanf(val, "%d", &level);
	  if(!strcmp(opr, "Strength"))
	    sscanf(val, "%d", &strength);
	  if(!strcmp(opr, "SFlags") ||  !strcmp(opr, "SFlagsL"))
	    sscanf(val, "%lx", &sflags_l);
	  if(!strcmp(opr, "Pflags."))
	    sscanf(val, "%lx", &pflags_h);
	    if(!strcmp(opr, "Pflags."))
	      sscanf(val, "%lx", &pflags_l);
	}
      }
	fclose(fl);

      printf( "%-13.12s%c   %9d   %10d     %9d     0x%08lx%08lx\n",
	     dp->d_name,
	     xtstbit(sflags_l, SFL_FEMALE) ? 'f' : 'm',
	     level,
	     score,
	     strength,
	     pflags_h,
	     pflags_l);
    }
    closedir(dir_pointer);
  }
  return(0);
}

void usage(char *progname)
{
  fprintf(stderr, "Usage: %s <options>\n\n", progname);
  fprintf(stderr, "Options: -a        All entries (including empty ones)\n");
  fprintf(stderr, "         -d <#>    Only players on the last # days.\n");
  fprintf(stderr, "         -l <level>Only players > level. Default = 1.\n");
  fprintf(stderr, "         -h        Include Header explaning output\n");
}

