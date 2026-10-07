#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#define SECS_A_DAY 60 * 60 * 24

void main(int argc, char *argv[]) {
  DIR *dir_pointer;
  FILE *fp;
  char c;
  char filename[256];
  char dirname[256];
  char buff[256];
  struct dirent *dp;
  time_t cur_time;
  int days, weeks, months, years;
  int option;
  time_t laston;
  time_t delete_t;
  int num_players = 0;
  int num_deleted = 0;
  char *opr, *val;
  int testonly = 0;

  days = weeks = months = years = 0;
  
  for (option = 1 ; option < argc ; option++) {
    if (!strcmp(argv[option], "-d"))
      days = atoi (argv[++option]);
    else if (!strcmp(argv[option], "-w"))
      weeks = atoi (argv[++option]);
    else if (!strcmp(argv[option], "-m"))
      months = atoi(argv[++option]);
    else if (!strcmp(argv[option], "-y"))
      years = atoi(argv[++option]);
    else if (!strcmp(argv[option], "-t"))
      testonly = 1;
    else {
      printf("%s: Illegal option (%s)\n", argv[0], argv[option]);
      exit(0);
    }
  }
  
  time(&cur_time);
  delete_t =  (cur_time -
	       (time_t) (SECS_A_DAY * days) -
	       (time_t) (SECS_A_DAY * 7 * weeks) -
	       (time_t) (SECS_A_DAY * 30 * months) -
	       (time_t) (SECS_A_DAY * 365 * years));

  for (c = 'A'; c <= 'Z'; c++) {
    sprintf(dirname, "%s/%c", TEXT_PFILES_BASE, c);
    dir_pointer = opendir(dirname);
    for (dp = readdir(dir_pointer); dp != NULL; dp = readdir(dir_pointer)) {
      if (dp->d_name[0] != '.') {
	sprintf(filename, "%s/%s", dirname, dp->d_name);
	fp = fopen(filename, "r");
	while(!feof(fp)) {
	  fgets(buff, 256, fp);
	  opr = (char *) strtok(buff, " ");
          val = (char *) strtok(NULL, " ");
	  if (val != NULL)
	    if (!strcmp(opr, "LastOn")) {
	      sscanf(val, "%lx", &laston);
	      break;
	    }
	}
	fclose(fp);
	if (laston < delete_t) {
	  if (!testonly)
	    unlink(filename);
	  num_deleted++;
	}
	num_players++;
      }
    }
    closedir(dir_pointer);
  }
  printf("Summary:\n\tDeleted: %d\n\tSaved: %d\n\tTotal: %d\n", 
	 num_deleted, num_players - num_deleted, num_players);
}
