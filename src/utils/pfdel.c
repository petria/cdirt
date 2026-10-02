/* pfdel.c: remove a string of text from all player files */

#include <stdio.h>
#include <sys/file.h>
#include <dirent.h>

#define TEXT_PFILES_BASE "/home/xroads/idirt/data/players"

int main(int argc, char *argv[]) {
  char c;
  char pfdirname[200], pfile1[200], pfile2[200], buff[200];
  struct dirent *dp;
  DIR           *dir_pointer;

  if (argc != 2) {
    printf("Usage: %s <string>\n", argv[0]);
    return;
  }

  for (c = 'A'; c <= 'Z'; c++) {
    sprintf(pfdirname, "%s/%c", TEXT_PFILES_BASE, c);
    dir_pointer = opendir(pfdirname);
    for (dp = readdir(dir_pointer); dp != NULL; dp = readdir(dir_pointer)) {
      if (dp->d_name[0] == '.')
	continue;
      else {
	sprintf(pfile1, "%s/%s", pfdirname, dp->d_name);
	sprintf(pfile2, "%s.new", pfile1);
	sprintf(buff, "fgrep -v -w %s %s > %s", argv[1], pfile1, pfile2);
	system(buff);
	sprintf(buff, "mv -f %s %s", pfile2, pfile1);
	system(buff);
      }
    }
    closedir(dir_pointer);
  }
  return(0);
}
