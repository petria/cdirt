#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <crypt.h>
#include ".config.sec"     /* For PASSWD_PREFIX */

#define PASSWD_LEN 32      /* Should be the same as in mudtypes.h */

#define OPT_FILE   ".config.opts"
#define IN_FILE    ".config.pri"
#define IN_FILE2   ".config.mkfile"
#define OUT_FILE   ".config.pri.tmp"
#define OUT_FILE2  ".config.mkfile.tmp"
#define BASIC  1
#define CUSTOM 2

#define PASSWD_LEN     32
#define answer_exists(x)   (x && x != '\n' && x != '\r')

#define CUSTOM_STR      "<CUSTOM>"
#define DEFAULT_STR     "<DEFAULT"
#define HEADER 		"<HEADER>"
#define P_HOSTNAME      "<PROMPT_HOSTNAME>"
#define P_INSTALL 	"<PROMPT_INSTALL>"
#define P_BASELOC	"<PROMPT_BASELOC>"
#define P_CFLAGS        "<PROMPT_CFLAGS>"
#define P_LDFLAGS       "<PROMPT_LDFLAGS>"
#define P_LOADDEFTS     "<PROMPT_LOADDEFTS>"
#define P_VERBOSE       "<PROMPT_CONFIG_VERBOSE>"
#define P_UNVEIL        "<PROMPT_UNVEIL>"
#define P_PROMPT 	"<PROMPT>"
#define P_VAL		"<PROMPT_VAL>"
#define P_YN		"<PROMPT_YN>"
#define P_NY		"<PROMPT_NY>"

void get_val(char *, char *, char *, char *);
char *probe_ldflags(void);
char *detect_arch (char *);
char *detect_os (char *);
void detect(char *deft, FILE*);

int ignore = 0;
int load_stored = 0;
int verbose = 1;
FILE *outf, *outf2;


char *replace_deft(char *s, char *deft) {
  static char retbuff[100];
  char *p;

  if ((p = strstr(s, "<deft>"))) {
    *p = 0;
    p += 6;
    sprintf(retbuff, "%s[%s]%s", s, deft, p);
    return(retbuff);
  }
  else
    return(s);
}

char *load_stored_deft(char *s) {
  char buff[100];
  static char val[100];
  char *p, *q;
  FILE *fpr;

  if ((fpr = fopen(IN_FILE2, "r"))) {
    while(1) {
      fgets(buff, 99, fpr);
      if (feof(fpr))
        break;
      if (!*(strchr(buff, '=') + 1))
        continue;
      if ((p = strchr(buff, '\n')))
	*p = 0;
      
      p = strchr(buff, '=');
      *(p-1) = 0;
      p += 2;

      if (!strcmp(s, buff)) {
	fclose(fpr);
	return(strcpy(val, p));
      }
    }
    fclose(fpr);
  }

  if ((fpr = fopen(IN_FILE, "r"))) {
    while(1) {
      fgets(buff, 99, fpr);
      if (feof(fpr))
	break;

      p = strchr(buff, ' ') + 1;
      
      if (!strncmp(buff, "#define ", strlen("#define "))) {
	if (!(q = strchr(p , ' '))) {
	  if (!strncmp(p, s, strlen(s))) {
	    fclose(fpr);
	    return("Y");
	  }
	}
	else {
	  if (!strncmp(p, s, strlen(s))) {
	    fclose(fpr);
	    if ((p = strchr(q+1, '"'))) {
	      *p = 0;
	      q = p;
	    }
            if ((p = strchr(q+1, '"')))
              *p = 0;
	    if ((p = strchr(q+1, '\n')))
	      *p = 0;
	    return(strcpy(val, q+1));
	  }
	}
      }
      else if (!strncmp(buff, "#undef ", strlen("#undef "))) { 
	if (!(q = strchr(p , ' '))) {
	  if (!strncmp(p, s, strlen(s))) {
	    fclose(fpr);
	    return("N");
	  }
	}
	else {
	  if (!strncmp(p, s, strlen(s))) {
	    fclose(fpr);
            if ((p = strchr(q+1, '"'))) {
              *p = 0;
              q = p;
            }
            if ((p = strchr(q+1, '"')))
              *p = 0;
	    if ((p = strchr(q+1, '\n')))
              *p = 0;
            return(strcpy(val, q+1));
	  }
	}
      }
    }
  }
  fclose(fpr);
  return(NULL);
}

void get_val(char *prmpt, char *rslt, char *deft, char *option) {
  char buff[256] = "\0";
  char *ptr, *retloc, *def;

  if (!load_stored || !((def = load_stored_deft(option))))
    def = deft;

  if (!ignore) {
    ptr = strchr(prmpt, '>') + 2;
    
    if ((retloc = strchr(ptr, '\n')))
      *retloc = 0;
    
    printf("%s ", replace_deft(ptr, def));
    fgets(buff, 255, stdin);

    if (!answer_exists(*buff))
      strcpy(rslt, def);
    else {
      if ((retloc = strchr(buff, '\n')))
        *retloc = 0;     
      strcpy(rslt, buff);
    }
  }
  else
    strcpy(rslt, def);
}

/* Should be identical to the one in utils.c */

char *my_crypt (char *buf, char *pw) {
  char *s = crypt (pw, "Mu");
  *(s + PASSWD_LEN - strlen(PASSWD_PREFIX) - 1) = 0;
  sprintf(buf, "%s%s", PASSWD_PREFIX, s);
  return(buf); 
}

int main(void) {
  char line[256];
  char dir[256];
  char deft[256];
  char answer[256];
  char define[256];
  char buff[256];
  char pass[PASSWD_LEN];
  int install;
  FILE *optptr, *f1, *f2;

  if (!(optptr = fopen(OPT_FILE, "r"))) {
    printf("Unable to read options file.\n");
    exit(1);
  }

  if (!(outf = fopen(OUT_FILE, "w"))) {
    printf("Unable to store config file.\n");
    exit(1);
  }

  if (!(outf2 = fopen(OUT_FILE2, "w"))) {
    printf("Unable to store second config file.\n");
    exit(1);
  }

  while(1) {
    fgets(line, 255, optptr);
    if (feof(optptr))
      break;
    if (*line != '<') {
      if (!ignore && verbose)
        printf("%s", line);
    }
    else {
      if (!strcmp(line, HEADER))      /* special types */
        continue;
      else if (!strncmp(line, P_LOADDEFTS, strlen(P_LOADDEFTS))) {
	if ((f1 = fopen(IN_FILE, "r")) && (f2 = fopen(IN_FILE2, "r"))) {
	  printf("\nWould you like to load the previous settings you chose as "
	         "the default\nvalues? You will then be able to hit enter on "
	         "the ones you wish to\nremain the same.\n\n");
	  
	  fclose(f1); fclose(f2);
	  printf("Load previous settings [Y]? ");
	  fgets(answer, 255, stdin);
	  if (tolower(*answer) != 'n')
	    load_stored = 1;
	}
      }
      else if (!strncmp(line, P_VERBOSE, strlen(P_VERBOSE))) {
	get_val(line, answer, "Y", "CONFIG_VERBOSE");
	if (tolower(*answer) == 'n') {
	  verbose = 0;
	  fprintf(outf, "#undef CONFIG_VERBOSE\n");
	}
	else
	  fprintf(outf, "#define CONFIG_VERBOSE\n");
      }
      else if (!strncmp(line, P_HOSTNAME, strlen(P_HOSTNAME))) {
	get_val(line, answer, "localhost", "_HOSTNAME_");
        detect(answer, outf);
      }
      else if (!strncmp(line, P_INSTALL, strlen(P_INSTALL))) {
        get_val(line, answer, "B", "INSTALL");
	fprintf(outf, "#define INSTALL \"%s\"\n", answer);
	if (tolower(*answer) == 'b')
          install = BASIC;
        else
          install = CUSTOM;
      }
      else if (!strncmp(line, P_LDFLAGS, strlen(P_LDFLAGS))) {
        get_val(line, answer, deft, "LDOPT");
        if (!*answer)
          fprintf(outf2, "LDOPT = %s\n", probe_ldflags());
        else
          fprintf(outf2, "LDOPT = %s\n", answer);
      }
      else if (!strncmp(line, P_CFLAGS, strlen(P_CFLAGS))) {
        get_val(line, answer, deft, "COPT");
        fprintf(outf2, "COPT = %s\n", answer);
      }
      else if (!strncmp(line, P_UNVEIL, strlen(P_UNVEIL))) {
        get_val(line, answer, "", "UNVEIL_PASS");
        if (!*answer) {
          printf("Error: you should type in a password. Re-run configure.\n");
          exit(0);
        }
        else if (strlen(answer) > PASSWD_LEN - strlen(PASSWD_PREFIX)) {
          printf("Error: that password is too long. Re-run configure.\n");
          exit(0);
        }
        else {
          if (strncmp(answer, "CDirtPw:", 8))     /* not already encrypted */
            my_crypt(pass, answer);
          else
            strcpy(pass, answer);
          fprintf(outf, "#define UNVEIL_PASS \"%s\"\n", pass); 
        }
      }
      else if (!strncmp(line, P_BASELOC, strlen(P_BASELOC))) {
        getcwd(dir, sizeof(dir));
        sprintf(buff, "<PROMPT_BASELOC> Base location for MUD <deft>:");
	get_val(buff, answer, dir, "ROOTDIR");
        if (*answer != '/') {
          printf("How can that be a directory? It must begin with a slash\n");
          exit(0);
        }
        else {
          if (*(answer + strlen(answer) - 1) == '/')
            *(answer + strlen(answer) - 1) = 0;
        }
        fprintf(outf, "#define BASE_LOC \"%s\"\n", answer);
        fprintf(outf2, "ROOTDIR = %s/\n", answer);
      }
      else if (!strncmp(line, P_PROMPT, strlen(P_PROMPT))) {
        get_val(line, answer, deft, define);
        fprintf(outf, "#define %s \"%s\"\n", define, answer);
      }
      else if (!strncmp(line, P_VAL, strlen(P_VAL))) {
        get_val(line, answer, deft, define);
        fprintf(outf, "#define %s %s\n", define, answer);
      }
      else if (!strncmp(line, P_YN, strlen(P_YN))) {
        get_val(line, answer, "Y", define);
        if (tolower(*answer) == 'y')
          fprintf(outf, "#define %s\n", define);
        else
          fprintf(outf, "#undef %s\n", define);
      }
      else if (!strncmp(line, P_NY, strlen(P_NY))) {
        get_val(line, answer, "N", define);
        if (tolower(*answer) == 'n')
          fprintf(outf, "#undef %s\n", define);
        else
          fprintf(outf, "#define %s\n", define);
      }
      else if (!strncmp(line, CUSTOM_STR, strlen(CUSTOM_STR))) {
        if (install != CUSTOM)
          ignore = 1;
      }
      else if (!strncmp(line, DEFAULT_STR, strlen(DEFAULT_STR))) {
        char *ptr;
        ptr = strchr(line, '=') + 1;
        *(strchr(ptr, '>')) = 0;
        strcpy(deft, ptr);
      }
      else {
        *(strchr(line + 1, '>')) = 0;
        strcpy(define, line + 1);
        *deft = 0;
      }
    }
  }

  fclose(outf);
  fclose(outf2);
  fclose(optptr);
  return(0);
}

int is_ipaddr(char *str) {
  char *p, *q;

  if ((q = strchr(str, '\n')))
    *q = 0;
    
  for (p = str ; *p ; p++)
    if (*p != '.' && !isdigit(*p))
      return(0);

  return(1);
}

void detect (char *hostname, FILE *fil) {
  struct utsname name;
  struct hostent *h;
  struct in_addr in;
  int failed;
  char filename[100];
  char ipname[100];
  FILE *fp;

  sprintf (filename, "include/MACHINE.H");

  if ((fp = fopen (filename, "wt")) == NULL)
    exit(1);

  if (uname(&name) == -1)
    failed = 1;
  else
    failed = 0;

  fprintf (fp, "#ifndef _MACHINE_H\n");
  fprintf (fp, "#define _MACHINE_H\n\n");

  if (!failed) {
    fprintf (fp, "#define %s\n", detect_arch (name.machine));
    fprintf (fp, "#define %s\n\n", detect_os (name.sysname));
  }

  fprintf (fp, "#define _ARCH_\t\"%s\"\n", failed ? 
	   "_UNKNOWN_ARCH_" : name.machine);

  if (is_ipaddr(hostname)) {
    strcpy(ipname, hostname);
    if ((h = gethostbyaddr(hostname, strlen(hostname), AF_INET)))
      strcpy(hostname, h->h_name);
    else {
      printf("\nHostname lookup failed.  Edit MACHINE.H manually.\n");
      strcpy(hostname, "<Your Hostname>");
    }
  }
  else {
    if ((h = gethostbyname(hostname))) {
      bcopy (h->h_addr_list[0], &(in.s_addr), h->h_length);
      strcpy(ipname, inet_ntoa(in));
    }
    else {
      printf("\nIP address lookup failed.  Edit MACHINE.H manually.\n");
      strcpy(ipname, "<Your IP#>");
    } 
  }

  fprintf (fp, "#define _HOSTNAME_\t\"%s\"\n", hostname);
  fprintf (fil, "#define HOSTNAME \"%s\"\n", hostname);
  fprintf (fp, "#define _IPNAME_\t\"%s\"\n", ipname);
  fprintf (fp, "#define _VERSION_\t\"%s\"\n", failed ? 
	   "_UNKNOWN_VERSION_" : name.release);
  fprintf (fp, "#define _OS_\t\"%s\"\n\n", failed ?
	   "_UNKNOWN_OS_" : name.sysname);
  fprintf (fp, "#endif\n");
  fclose (fp);
}

char *detect_arch (char arch[65]) {
  if ((strstr (arch, "i386")) != NULL)
    return "_I386_";

  if ((strstr (arch, "i486")) != NULL)
    return "_I486_";

  if ((strstr (arch, "i586")) != NULL)
    return "_I586_";

  if ((strstr (arch, "i686")) != NULL)
    return "_I686_";

  if ((strstr (arch, "sun")) != NULL)
    return "_SUN_";

  if ((strstr (arch, "mac68k")) != NULL)
    return "_MAC68K_";

  return "_UNKNOWN_ARCH_";
}

char *detect_os(char os[65]) {
  if ((strstr (os, "Linux")) != NULL)
    return "_LINUX_";

  if ((strstr (os, "SunOS")) != NULL)
    return "_SUNOS_";

  if ((strstr (os, "NetBSD")) != NULL)
    return "_NETBSD_";

  if ((strstr (os, "BSD")) != NULL)
    return "_BSD_";

  return "_UNKNOWN_OS_";
}


int file_exists(char *fname) {
  FILE *p;
  
  if ((p = fopen(fname, "r"))) {
    fclose(p);
    return(1);
  }
  else
    return(0);
}

char *probe_ldflags(void) {
  static char buff[256];

  *buff = 0;

  if (file_exists("/usr/lib/libm.a"))
    strcat(buff, "-L/usr/lib -lm ");
  else if (file_exists("/usr/local/lib/libm.a"))
    strcat(buff, "-L/usr/local/lib -lm ");
  else if (file_exists("/lib/libm.a"))
    strcat(buff, "-L/lib/libm.a -lm ");

  if (file_exists("/usr/lib/libcrypt.a"))
    strcat(buff, "-L/usr/lib -lcrypt");
  else if (file_exists("/usr/local/lib/libcrypt.a"))
    strcat(buff, "-L/usr/local/lib -lcrypt");
  else if (file_exists("/lib/libcrypt.a"))
    strcat(buff, "-L/lib/libm.a -lcrypt");

  return(buff);    
}
