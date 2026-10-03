/****************************************************************
 * C-Dirt 3.0beta3						*
 * (C) 1999 G. Castrataro					*
 ****************************************************************/

#include "main.h"
#include "audit.h"

void save_pid(void) {
  char pidfile[50];
  FILE *pidptr;

  sprintf (pidfile, "%s/%s.%d", data_dir, PID_FILE, mud_port);
  if ((pidptr = FOPEN(pidfile, "w"))) {
    fprintf(pidptr, "%d\n", getpid());
    FCLOSE(pidptr);
  }
}

void rm_pid(void) {
  char pidfile[50];
 
  sprintf(pidfile, "%s/%s.%d", data_dir, PID_FILE, mud_port);
  unlink(pidfile);
}

int main (int argc, char **argv, char **ep) {
  int x;
  envp = ep;
  progname = argv[0];

  srand48(time(NULL));          /* init random numbers */
  srand((int) time(NULL));

  get_options (argc, argv);     /* Parse command line */

  if (!old_proc_num)
    fprintf(stderr, "%s Daemon Loading", VERSION);

  if (!quiet) {
    if (data_dir == NULL) {
      fprintf (stderr, "  C-Dirt Daemon Error: data_dir is a NULL value.\n");
      fprintf (stderr, "  Halting Daemon!\n");
      exit (1);
    }
    fprintf (stderr, "\n  Data Directory:   %s\n", data_dir);
    fprintf (stderr, "  Maximum Players:  %d\n", max_players);
    fprintf (stderr, "  Port Selected:    %d\n", mud_port);
    fprintf (stderr, "  Clear System Log: %s\n", clear_syslog_file ?"Yes":"No");
    fprintf (stderr, "  Kill Other MUD:   %s\n", kill_other_mud ?"Yes":"Ask User");
  }

  chdir (data_dir);

  for (x = 0 ; x < MAX_FDS ; x++)
    sock_fds[x] = -1;

  if (!old_proc_num) {
    check_pid();
    x = xmain ();
  }
  else
    x = xmain_reboot ();

  rm_pid();

  if (x < 0)
    mudlog ("BOOTUP: Abnormal termination of mud");
  else
    mudlog ("BOOTUP: Normal termination of mud");

  return(0);
}

int msock(int port) {
  int s;
  struct sockaddr_in server;
#ifndef CDIRT_DOCKER
  struct hostent *h;
#endif
  int opt;

  s = socket(AF_INET, SOCK_STREAM, 0);

  if (s == -1) {
    fprintf(stderr, "(Bootup): Error creating stream socket\n");
    exit(1);
  }

  opt = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == -1)
  {
    fprintf(stderr, "(Bootup): Error in setsockopt\n");
    exit(2);
  }

#ifdef CDIRT_DOCKER
  /* Docker publishes ports to the container interface, not its hostname. */
  memset(&server, 0, sizeof(server));
  server.sin_addr.s_addr = htonl(INADDR_ANY);
#else
  if (!(h = gethostbyname(_HOSTNAME_))) {
    fprintf(stderr, "Unable to bind to hostname you chose. Run configure.\n");
    exit(3);
  }
  bcopy (h->h_addr_list[0], &(server.sin_addr), h->h_length);
#endif

  server.sin_family = AF_INET;
  server.sin_port = htons (port);

  if (bind (s, (struct sockaddr *) & server, sizeof (server))) {
    fprintf(stderr, "(Bootup): Error binding stream socket\n");
    close(s);
    exit(4);
  }
  listen (s, 5);
  return s;
}

int xmain () {
  if (open_logfile(clear_syslog_file) || bootstrap() || go_background())
    return(-1);

  main_socket = msock(mud_port);

  numresets = 0;
  numreboots = 0;
  numcrashes = 0;

  /* Initialize MUD Daemon Startup Time */
  time (&last_startup);

  /* Main program loop */
  main_loop(main_socket);
          
  mudlog ("&+WSYSTEM:&N Closing listening socket");
  close(main_socket);
  return(0);
}

/* BSD style daemonizing */

int go_background () {
  int pid;

  if (!stay_foreground) {
    if ((pid = fork())) {
      fflush (stdout);
      exit(3);
    }
    fclose (stdin);
    fclose (stdout);

    #ifdef _LINUX_
      setpgrp ();
    #else
      setpgrp (pid, pid);
    #endif
  }
  save_pid();

  setsignals();
  return(0);
}
 
/* check if the pid exists, if so, ask to kill it */

void check_pid(void) {
  FILE *pidptr;
  char c;
  int oldpid;
  char pidfile[50];

  sprintf (pidfile, "%s/%s.%d", data_dir, PID_FILE, mud_port);
  if (!(pidptr = FOPEN(pidfile, "r")))
    return;

  fscanf(pidptr, "%d", &oldpid);
  FCLOSE(pidptr);

  if (kill(oldpid, SIGCONT) == -1) {
    if (errno == EPERM) {
      fprintf(stderr, "...\nAberd already running under a different user.\n");
      exit(4);
    }
  }
  else {
    fprintf(stderr, "...\nAberd is already running, kill it? ");
    if (tolower(c = getchar()) == 'y') {
      kill(oldpid, SIGTERM);
        while(!access(pidfile, F_OK))    /* sleep until other mud dies */
          sleep(1);
      fprintf(stderr, "  Other MUD killed, loading");
    }
    else {
      fprintf(stderr, "Aborted aberd loading.\n");
      exit(5);
    }
  }
}

void get_options (int argc, char **argv) {
  char *s;
  int x;

  if (argc == 1) {
    stay_foreground = False;
    clear_syslog_file = False;
    mud_port = PORT;
    max_players = 32;
    return;
  }
  while (--argc > 0) {
    s = *++argv;
    if (*s++ != '-') {
      usage ();
      exit (0);
    }
    x = *s++;
    switch (x) {
    case 'a':
      break;
    case 'h':
      usage ();
      exit(0);
    case 'H':
      fullusage ();
      exit(0);
    case 'p':
      if (*s != '\0' || (--argc > 0 && *(s = *++argv) != '\0')) {
	if ((mud_port = atoi (s)) < 1000 || mud_port > 65535) {
	  mud_port = PORT + 5;
	}
      }
      break;
    case 'f':
      stay_foreground = True;
      break;
    case 'k':
      kill_other_mud = True;
      break;
    case 'c':
      clear_syslog_file = True;
      break;
    case 'v':
      quiet = False;
      break;
    case 'V':
      printf ("\nC-Dirt Version Information\n");
      printf ("-------------------------\n");
      printf ("%s (%s/%s)\n", VERSION, _ARCH_, _OS_);
      printf ("1999, G. Castrataro\n\n");
      printf ("See credits for contributions\n");
      exit (0);
      break;
    case 'u':
      update = 1;
    case 'r':
      if (*s != '\0' || (--argc > 0 && *(s = *++argv) != '\0'))
	old_proc_num = atoi (s);
      break;
    case 'n':
      if (*s != '\0' || (--argc > 0 && *(s = *++argv) != '\0')) {
	if ((max_players = atoi (s)) < 1 || max_players > 1000) {
	  max_players = 32;
	}
      }
      break;
    case 'd':
      if (*s != '\0' || (--argc > 0 && *(s = *++argv) != '\0')) {
	data_dir = s;
      }
      break;
    default:
      usage ();
      exit (6);
    }
  }
  if (argc > 0) {
    usage ();
    exit (7);
  }
}

void usage (void) {
  fprintf (stderr, "usage: %s [-p port] [-d path] [-n #] [-f] [-k] [-c] [-v] [-V] [-o] [-H]\n", progname);
  fprintf (stderr, "(%s -H for detailed help)\n", progname);
}

void fullusage (void) {
  fprintf (stderr, "usage: %s [-p port] [-d path] [-n #] [-f] [-k] [-c] [-v] [-V] [-o] [-H]\n", progname);
  fprintf (stderr, "  -p port     : Alternate port to attach C-Dirt to.\n");
  fprintf (stderr, "  -d path     : Path to data files.\n");
  fprintf (stderr, "  -n #        : Maximum number of players (Default is 32).\n");
  fprintf (stderr, "  -f          : Run C-Dirt in the foreground.\n");
  fprintf (stderr, "  -k          : Automatically kill any other C-Dirt Daemons that are running.\n");
  fprintf (stderr, "  -c          : Automatically clear system log.\n");
  fprintf (stderr, "  -v          : Run in verbose startup mode.\n");
  fprintf (stderr, "  -V          : Display version information.\n");
  fprintf (stderr, "  -o          : Automatically open MUD.\n\n");
}

Boolean is_identing(int fd) {
  int i;
 
  for (i = 0 ; i < max_players ; i++)
   if (resfd(i) == fd)
     return(True);
  return(False);
}

void set_fd(int fd, Boolean output) {
  if (fd == -1)
    return;
  if (output)
    FD_SET(fd, &output_set);
  if (fd >= width)
    width = fd + 1;
  FD_SET(fd, &input_set);
}

extern void dnsboot(void);

void main_loop (int listen_socket) {
  int i, fd, plx, fds;
  struct timeval timeout, slice;

  time (&global_clock);
  time (&last_autosave);
  time (&last_healall);
  breset = False;

  if (!old_proc_num)
    mudlog ("&+WSYSTEM:&N C-Dirt %s Booted (PID: %d)", VERSION, getpid());
  else
    mudlog ("&+WSYSTEM:&N %s Successful, C-Dirt Daemon Restarted",
	    update ? "Update" : "Reboot");

  if (clear_syslog_file)
    mudlog ("&+WSYSTEM:&N System Log Cleared");

  if (!update) {
    last_reset = global_clock;
    qdresetflgs();

    for (i = 0; i < numzon; i++) {
      move_pouncie ();
      reset_zone (i);
    }
    scatter_potions();
  }

  setup_globals(-1);
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;

#ifdef ABERCHAT
  if (mud_port == PORT)                          /* do not run on test mud */
    aberchat_boot();
#endif

  dnsboot();
  audit_boot();

  while (1) {
    FD_ZERO(&input_set);                           /* zero out all fd_sets */
    FD_ZERO(&output_set);
    FD_ZERO(&exception_set);
     
    set_fd(listen_socket, False);
    set_fd(aberfd, aber_output);
    set_fd(dnsfd, dns_output);
    set_fd(audit_descriptor(), False);

    for (plx = 0 ; plx < max_players ; plx++)      /* add player fds */
      if (is_conn(plx) && !linkdead(plx)) {
        if (resolved(plx) && ident(plx) && limbo(plx))
          enter_game(plx);
        set_fd(fildes(plx), output(plx));
        if (!ident(plx))
          set_fd(players[plx].resfd, players[plx].respos != -1);
      }

    if (crashing)
      flush_mud(False);

    fds = select(width, &input_set, &output_set, &exception_set, &timeout);
    if (fds == -1) {
      if (errno == EINTR)
        continue;   
      else if (errno == EBADF) {
	progerror ("select");
        rm_pid();
	exit(8);
      }
    } 


    gettimeofday(&slice, (struct timezone *) NULL);
    timeout.tv_usec = 1000000 - slice.tv_usec;
    timeout.tv_sec = (slice.tv_sec + 1) % 2;

    if (!fds) {
      if (!audit_manual_timer()) on_timer();
      continue;
    }
      
    for (fd = 0 ; fds > 0; fd++) {
      if (FD_ISSET (fd, &exception_set)) {
	mudlog ("SOCKET: Exception pending with fd = %d", fd);
	fds--;
      }
      if (FD_ISSET (fd, &input_set)) {
        if (fd == audit_descriptor())
          audit_request();
        else if (fd == listen_socket)
          new_connection(fd);
        else if (fd == aberfd)
          aberchat_readpacket(fd);
        else if (fd == dnsfd)
          read_dns(fd);
        else if (is_identing(fd))
          read_ident(fd);
        else
          read_packet(fd);
        fds--;
      }
      if (FD_ISSET (fd, &output_set)) {
        if (fd == aberfd) 
          aberchat_writepacket(fd); 
        else if (fd == dnsfd)
          send_dns(fd);
        else if (is_identing(fd))
          send_ident(fd);
        else
	  write_packet(fd);
	fds--;
      }
    }
  }
}

void write_packet(int fd) {
  int len, n_wrote, me;
  
  me = find_pl_index(fd);          

  if (me < 0) return;
  len = out_write(me) - out_read(me);

  if (len > M_BUFFLEN)
    len = M_BUFFLEN;

  n_wrote = write(fd, out_read(me), len);

#ifdef IO_STATS
  bytes_sent += n_wrote;
#endif

  if (*(out_read(me) + n_wrote))          /* more bytes to send */
    out_read(me) += n_wrote;
  else {
    if (out_size(me) > M_BUFFLEN) {       /* shrink buffer */
      FREE(out_buffer(me));
      out_size(me) = M_BUFFLEN;
      out_buffer(me) = NEW(char, M_BUFFLEN);
    }
    out_write(me) = out_buffer(me);
    out_read(me) = out_buffer(me);
    output(me) = False;

    if (hasquit(me)) {
      setup_globals(me);
      sock_msg("&+C%H &+Wclosed");
      close_sock(fd);
    }
  }
}

void close_sock(int fd) {
  int i, new_width;

  setup_globals(find_pl_index(fd));

  shutdown(fd, 2);
  if (cur_player->iamon)
    remove_from_game();

  free_player();
  cur_player->iamon = False;
  cur_player->is_conn = False;
  sock_fds[fd] = -1;

  for (i = 0, new_width = 0 ; i < width ; i++)
    if (sock_fds[i] != -1)
      new_width = i;

  width = new_width; 
  close(fd);
}

char *test_multi_connects (struct in_addr *in) {
  static char inet_str[16];
  int host_connects;
  int i;

  strcpy(inet_str, inet_ntoa(*in));
  host_connects = 0;

  for (i = 0, host_connects = 0 ; i < max_players ; i++)
    if (is_conn(i) && *(ip_addr(i)) &&
	!strcmp(strchr(ip_addr(i), '.'), strchr(inet_str, '.')))
      host_connects++;
	
  if (host_connects > MAX_CONNECTS || !strcmp(inet_str, "0.0.0.0"))
    return NULL;
  else
    return inet_str;
}

void closemsg(int fd, char *msg) {
  write(fd, msg, strlen(msg));
  shutdown(fd, 2);
  close(fd);
}

void new_connection (int listen_socket) {
  int plx;
  int fd;
  int sin_len;
  struct sockaddr_in sin;
  char *ip_addr;

  bzero ((char *) &sin, sizeof (struct sockaddr_in));
  sin_len = sizeof (struct sockaddr_in);

  if ((fd = accept (listen_socket, (struct sockaddr *) &sin, &sin_len)) < 0)
    return;
  if (fcntl (fd, F_SETFL, FNDELAY) == -1)
    return;
  if ((ip_addr = test_multi_connects(&sin.sin_addr)) == NULL) {
    closemsg(fd, "Too many connects from your domain.\n");
    return;
  }
  if ((plx = find_free_player_slot ()) < 0) {
    sock_msg("Connection refused (MUD full): %s", ip_addr);
    closemsg(fd, "\nSorry, but this mud is full, please come back later.\n");
    return;
  }
  setup_io (plx, fd);
  setup_globals (plx);

#ifdef IO_STATS
  sock_conns++;
#endif

  strcpy(ip_addr(plx), ip_addr);
  strcpy(hostname(plx), ip_addr);
  strcpy(username(plx), ip_addr);
  port(plx) = (int) ntohs(sin.sin_port);

  setpname (mynum, "<Logging In>");
  dns_output = True;
  ident_player();
  setup_globals(-1);
}

void setup_io(int plx, int fd) {
  sock_fds[fd] = plx;
  rplrs[plx].fil_des = fd;
  players[plx].resfd = -1;
  out_buffer(plx) = NEW (char, M_BUFFLEN);
  out_size(plx) = M_BUFFLEN;
  out_read(plx) = out_buffer(plx);
  out_write(plx) = out_buffer(plx);
  inp_ptr(plx) = inp_buffer(plx);
  *inp_buffer(plx) = *out_buffer(plx) = 0;
  ignore_input(plx) = False;
  is_conn(plx) = True;
  limbo(plx) = True;
}

void read_packet (int fd) {
  char *ptr, *buffptr, *buff_start;
  int num_read, me;

  me = find_pl_index(fd);                       /* real player giving input */
  if (me < 0) return;

  setup_globals(me);

  buff_start = inp_buffer(me);
  buffptr = inp_ptr(me);

  if ((num_read = read (fd, buffptr, CUTOFF_LEN - (buffptr - buff_start))) < 1)
    {
      if (!num_read)                      /* 0-byte packets = connection cut */
	quit_player(-1);
      else
	quit_player(errno);
      return;
    }

#ifdef IO_STATS
  bytes_read += num_read;
#endif  

  *(buffptr + num_read) = 0;

  if (!isascii(*buffptr)) {
/*  if (!isprint(*buffptr) && !isspace(*buffer)) {         telnetresponse */
    inp_ptr(me) = inp_buffer(me);
    return;
  }

  /* lines below are for the end of a spam of text */
  
  else if (num_read + (buffptr - buff_start) < CUTOFF_LEN) {
    if (ignore_input(me) && strchr(buffptr, '\n')) {
      inp_ptr(me) = inp_buffer(me);
      ignore_input(me) = False;
      return;
    }
  }

  else {                                                   /* packet too big */
    ptr = buff_start + CUTOFF_LEN;
    if (!ignore_input(me)) {                        /* first oversize packet */
      *ptr = 0;
      bprintf ("&+B(&*Maximum Input Exceeded&+B)\n"
               "&+B(&*Input Cut Off After: &+W%s&+B)\n", ptr - 16);
      strcat(buff_start, "\r\n"); 
      ignore_input(me) = True;
    }
    else {
      inp_ptr(me) = inp_buffer(me);               /* don't add to buff */
      return;
    }
  }

  if (!strchr(buff_start, '\n') && !strchr(buff_start, '\r'))
    inp_ptr(me) += num_read;
  else  if (limbo(me))                     /* player is resolvin, susp */
    inp_ptr(me) += num_read;
  else if ((!inp_handler(me) || !phandler(me))) {
    bprintf("\tInternal Error Has Occurred: Lost Your Input Handler");
    quit_player(-3);
  }
  else
    do_packet(me, buff_start);
  setup_globals(-1);
}

/* this function deals with a complete & valid packet which may
   have multiple lines of text separated by \n, \r, \r\n or \n\r */

void do_packet(int me, char *packet) {
  static char buff[MAX_COM_LEN];
  static char *bptr, *tptr;                 /* buff ptr, text ptr */
  static int plx;

  if (snooped(me)) {
    for (plx = 0 ; plx < max_players ; plx++)
      if (snooptarget(plx) == me) {
	setup_globals(plx);
	bprintf("\n&+r[&+WSnoop: %s&+r]&N %s", pname(me), packet);
      }
    setup_globals(me);
  }

  if (islogged(me))
    write_plr_log(packet);

  /* copy command to buffer, exec command, repeat */

  for (tptr = packet, bptr = buff ; *tptr ; tptr++) {
    if (!iscntrl(*tptr))
      *bptr++ = *tptr;
    else if (*tptr == '\n' || *tptr == '\r' ) {
      if (*(tptr + 1) == '\n' || *(tptr + 1) == '\r')
        tptr++;
      *bptr = 0;
      exec_cmd(me, buff);
      bptr = buff;
    } 
    else if (*tptr == '\b' && bptr > buff)
      bptr--;
  }
  inp_ptr(me) = inp_buffer(me);
}

/* actually execute the mud command */

void exec_cmd(int me, char *text) {

#ifdef LOG_INPUT
  if (plogptr) {
    fprintf(plogptr, "(%s) %s:%s\n", 
      time2ascii(global_clock) + 11, pname(mynum), text);
    fflush(plogptr);
  }
#endif

  if (!inp_handler(me))
    return;
  
  if (!ptstflg (me, PFL_IDLE))
    plast_cmd (me) = global_clock;
  prlast_cmd (me) = global_clock;
  
  in_cmd(me) = True;

  (phandler(me))(text);

  if (me != real_mynum)
    setup_globals(me);

  in_cmd(me) = False;

  if (!hasquit(me))
    bprintf("%s", cur_player->cprompt);
}
