/* newid.c: program to re-number the player ID's */

#include <stdio.h>
#include <dirent.h>
#include "config.h"
#define ID_COUNTER 	BASE_LOC "/data/WIZ_ZONES/ID_COUNTER"
#define False 0
#define True 1

int main() {
  DIR *dir_pointer;
  FILE *fp;
  char dirname[256];
  char filename[256];
  int num_read;
  char text[100][256];
  int id_counter;
  struct dirent *dp;
  int i;
  char c;
  FILE *idptr;
  int id_found;

  if ((idptr = fopen(ID_COUNTER, "r")) == NULL) {
    printf("ID_COUNTER could not be opened");
    exit(1);
  }

  fscanf(idptr, "%ld", &id_counter);
  fclose(idptr);
  id_counter++;   /* just to be safe */

  printf("Numbering player ID's, starting with %d...\n", id_counter);

  for (c = 'A'; c <= 'Z'; c++) {
    sprintf(dirname, "%s/%c", TEXT_PFILES_BASE, c);
    dir_pointer = opendir(dirname);
    for (dp = readdir(dir_pointer); dp != NULL; dp = readdir(dir_pointer))
      if (dp->d_name[0] != '.') {
	sprintf(filename, "%s/%s", dirname, dp->d_name);
	if ((fp = fopen(filename, "r")) != NULL) {        /* read file */
	  for (i = 0 ; !feof(fp) ; i++) {
	    fgets(text[i], 256, fp);
	    if (!strncmp(text[i], "ID", 2)) {
	      sprintf(text[i], "ID         %ld\n", id_counter++);
	      id_found = True;
	    }
	  }
	  num_read = i;
	  fclose(fp);

	  if ((fp = fopen(filename, "w")) != NULL) {      /* write file */
	    for (i = 0 ; i < num_read ; i++)
 	      fputs(text[i], fp);
	    if (!id_found)
	      fprintf(fp, "ID         %ld\n", id_counter++);
	    fclose(fp);
	  }
	  else
	    printf("Couldn't save %s\n", dp->d_name);
	}
	else
	  printf("Couldn't read %s\n", dp->d_name);
      }
  }
  if ((idptr = fopen(ID_COUNTER, "w")) == NULL)
    exit(1);

  fprintf(idptr, "%ld\n", id_counter);
  fclose(idptr);
  printf("Stored ID_COUNTER = %d\n", id_counter);
  return(0);
}
