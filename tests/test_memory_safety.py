"""Compile the actual repaired helpers and exercise their allocation boundaries."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from test_c_bounds import function

ROOT = Path(__file__).resolve().parents[1]


class MemorySafetyTests(unittest.TestCase):
    def run_c(self, body):
        if not shutil.which('gcc'):
            self.skipTest('gcc required')
        with tempfile.TemporaryDirectory(prefix='cdirt-memory-') as folder:
            source = Path(folder) / 'memory.c'
            source.write_text('#include "memory.h"\n#include <assert.h>\n#include <ctype.h>\n#include <errno.h>\n#include <unistd.h>\n#include <sys/wait.h>\n' + body)
            binary = source.with_suffix('')
            result = subprocess.run(['gcc', '-g', '-fsanitize=address,undefined',
                                     '-fno-sanitize-recover=all', '-fno-pie', '-no-pie',
                                     '-I', str(ROOT / 'include'), str(source), '-o', str(binary)],
                                    text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(binary)], env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'),
                                    text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_allocation_builder_and_integer_sets(self):
        body = r'''
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define Boolean int
#define False 0
#define True 1
#define min(A,B) ((A)<(B)?(A):(B))
typedef struct { int len,maxlen; int *list; } int_set;
Boolean check_for_possible_resize(int_set *);
'''
        for name in ('void *resize_array (', 'void init_intset (', 'void free_intset (',
                     'Boolean add_int (', 'Boolean remove_int(', 'Boolean check_for_possible_resize ('):
            body += function('src/utils.c', name)
        body += r'''
int main(void) {
  int_set set; int i, status; pid_t child;
  Text text = {0}; char *rendered, *copy;
  int once=0;
  assert(memory_alloc(0,100)==NULL);
  copy=memory_string((once++, "one")); assert(once==1); free(copy);
  for(i=0;i<100000;i++) text_append(&text,"abcd");
  rendered=text_format("%s:%d",text.data,1234);
  assert(strlen(rendered)==400005 && !strcmp(rendered+400000,":1234"));
  free(rendered); free(text.data);
  init_intset(&set,0);
  for(i=0;i<1000;i++) assert(add_int(i,&set));
  assert(!remove_int(2000,&set));
  for(i=0;i<1000;i++) assert(remove_int(i,&set));
  assert(!remove_int(0,&set)); free_intset(&set); free_intset(&set);
  assert(!set.list && !set.maxlen && !set.len);
  child=fork(); assert(child>=0);
  if(!child) { memory_alloc(SIZE_MAX,2); _exit(0); }
  waitpid(child,&status,0); assert(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_FAILURE);
  return 0;
}
'''
        self.run_c(body)

    def test_prompt_travel_title_description_and_editor(self):
        body = r'''
#define DEFAULT_PROMPT "> "
#define SETIN_MAX 80
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define COPY(S) memory_string(S)
#define SETIN_SETIN 1
#define SETIN_SETOUT 2
#define SETIN_SETMIN 3
#define SETIN_SETMOUT 4
#define SETIN_SETVIN 5
#define SETIN_SETVOUT 6
#define SETIN_SETQIN 7
#define SETIN_SETQOUT 8
#define SETIN_SETSIT 9
#define SETIN_SETSTAND 10
#define SETIN_SETSUM 11
#define SETIN_SETSUMIN 12
#define SETIN_SETSUMOUT 13
#define DEFAULT_SETIN "%n arrives."
#define DEFAULT_SETOUT "%n leaves."
#define DEFAULT_SETMIN "in"
#define DEFAULT_SETMOUT "out"
#define DEFAULT_SETVIN "in"
#define DEFAULT_SETVOUT "out"
#define DEFAULT_SETQIN "in"
#define DEFAULT_SETQOUT "out"
#define DEFAULT_SETSIT "sits"
#define DEFAULT_SETSTAND "stands"
#define DEFAULT_SETSUM "sum"
#define DEFAULT_SETSUMIN "in"
#define DEFAULT_SETSUMOUT "out"
#define mynum 0
#define psex(X) 0
static char *xname(char *name) { return name; }
static void mudlog(char *fmt, ...) {}
static void bprintf(char *fmt, ...) {}
struct player { long work2[64]; } player, *cur_player=&player;
#define LOAD_INCR 256
#define FOPEN fopen
#define FCLOSE fclose
'''
        body += function('src/mobile.c', 'char *\nmake_prompt (')
        body += function('src/mobile.c', 'char *make_title(')
        body += function('cr/uaf.c', 'char *build_setin (')
        body += function('src/change.c', 'void load_from_file(')
        body += r'''
int main(void) {
  char prompt[61], expanded[1504], travel[79], path[]="/tmp/cdirt-desc-XXXXXX";
  char *description=COPY("original"), *value; FILE *file; int i,fd=mkstemp(path);
  assert(fd>=0); close(fd);
  for(i=0;i<30;i++) memcpy(prompt+i*2,"%h",2); prompt[60]=0;
  value=make_prompt(expanded,prompt,"-2147483648/-2147483648","0","10","LongTestName","0");
  assert(strlen(value)==690);
  assert(!strcmp(make_prompt(expanded,"",NULL,NULL,NULL,NULL,NULL),""));
  for(i=0;i<39;i++) memcpy(travel+i*2,"%n",2); travel[78]=0;
  value=build_setin(1,NULL,travel,"Rydis",NULL,NULL); assert(strlen(value)==312);
  assert(!strcmp(build_setin(1,NULL,"",NULL,NULL,NULL),""));
  assert(!strcmp(build_setin(1,NULL,"%n%v",NULL,NULL,NULL),""));
  assert(!strcmp(make_title("%s the brave","Rydis"),"Rydis the brave"));
  assert(!strcmp(make_title("bad %n format", "Rydis"),"bad %n format"));
  cur_player->work2[0]=(long)&description;
  strcpy((char*)&cur_player->work2[1],"Description changed.\n");
  file=fopen(path,"w"); for(i=0;i<20000;i++) fputs("abc\n",file); fclose(file);
  load_from_file(path); assert(strlen(description)==79999);
  file=fopen(path,"w"); fputs("invalid ^\n",file); fclose(file);
  load_from_file(path); assert(strlen(description)==79999);
  file=fopen(path,"w"); fclose(file); load_from_file(path); assert(!*description);
  file=fopen(path,"w"); fputs("no newline",file); fclose(file);
  load_from_file(path); assert(!strcmp(description,"no newline"));
  unlink(path); free(description); return 0;
}
'''
        self.run_c(body)

    def test_output_growth_and_recursive_codes(self):
        body = r'''
#define Boolean int
#define False 0
#define True 1
#define M_BUFFLEN 4096
#define HIGH_MARK 1024
#define CRESET "\033[40m\033[0m"
#define ADD_LINE(S) strformat((S),has_color)
#define SHOWNAME(S) ADD_LINE(S)
#define real_mynum 0
#define mynum 0
#define SFL_NOBLINK 0
#define SFL_NOBEEP 0
#define SFL_BLIND 0
#define SFL_DEAF 0
#define ststflg(P,F) 0
#define is_conn(P) 1
#define fmbn(S) 0
#define isdark() 0
#define VERSION "version"
#define EMAIL "email"
#define MUD_NAME "C-Dirt"
#define MASTERUSER "Master"
#define PORT 6715
#define LASTBUILD "build"
#define _HOSTNAME_ "host"
#define _OS_ "os"
#define _ARCH_ "arch"
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
unsigned char *base,*rd,*wr,*dest;
int capacity=4096;
#define out_buffer(P) base
#define out_read(P) rd
#define out_write(P) wr
#define out_size(P) capacity
void strformat(char *,Boolean);
char *do_colorcode(char *,Boolean *,Boolean);
char *do_specialcode(char *,Boolean);
void pfilter(char *s,Boolean b) { strformat(s,b); }
void file_pager(char *s,Boolean b) { strformat(s,b); }
void *resize_array(void *p,int size,int old,int next) {
 void *q=memory_alloc(next,size); memcpy(q,p,old*size); free(p); return q;
}
'''
        source = (ROOT / 'cr/bprintf.c').read_text()
        body += source[source.index('char color_table[]'):source.index('unsigned char *dest;')]
        for name in ('void strip_color (', 'void init_memory(', 'void strformat (',
                     'char * do_colorcode(', 'char *do_specialcode('):
            body += function('cr/bprintf.c', name)
        body += r'''
int main(void) {
 Text text={0}; int i; char *plain;
 base=memory_alloc(capacity,1); rd=wr=base; dest=NULL;
 for(i=0;i<20000;i++) text_append(&text,"&+Rhello\n");
 strformat(text.data,1); assert((size_t)(dest-base)==460000);
 assert(rd==base && dest-base<capacity && *dest==0);
 free(text.data); dest=NULL; wr=base; rd=base;
 text=(Text){0}; text_append(&text,"\001A");
 for(i=0;i<20000;i++) text_append(&text,"x");
 text_char(&text,003); text_append(&text,"end");
 strformat(text.data,1); assert(dest-base==20003 && !strcmp((char*)dest-3,"end"));
 wr=base; dest=NULL; strformat("\001Ax",1); assert(dest-base==1);
 wr=base; dest=NULL; strformat("&=R",1); assert(!strcmp((char*)base,"&=R"));
 plain=memory_string("&=R"); strip_color(plain,plain); assert(!strcmp(plain,"&=R"));
 free(plain); free(text.data); free(base); return 0;
}
'''
        self.run_c(body)

    def test_typed_player_records(self):
        body = r'''
#include <time.h>
#include <inttypes.h>
#define Boolean int
#define NUM_ENT 40
#define LLEN 4096
#define TOKEN_LEN 11
#define INT 0
#define CHAR 1
#define PTR 2
#define UINT 3
#define TIME 4
#define NUM_STORE_SLOTS 1
#define PROMPT_LEN 60
#define SETIN_MAX 80
#define COPY(S) memory_string(S)
#define FOPEN fopen
#define FCLOSE fclose
#define TEXT_PFILES_BASE "."
typedef struct { short type; char *token; void *field; size_t size; } IORec;
typedef struct {
 struct { char ptitle[300],passwd[32];
  char *awaymsg,*prompt,*setin,*setout,*setmin,*setmout,*setvin,*setvout,*setqin,*setqout,*setsit,*setstand,*setsum,*setsumin,*setsumout;
  struct {int len;} pager;
  int pcarry,pmagic,pchannel,pkilled,pdied,coins;
  time_t first_on,time_on,mortal_time,wiz_time,last_on;
 } player;
 struct {char usrname[200],hostname[100]; int storage[1];} rplr;
 struct {char pname[32]; char *phome; unsigned pscore;
  int pstr,pdam,parmor,pvis,plev,pwimpy,class;} ublock;
} PERSONA;
void bprintf(char *fmt,...) {} void mudlog(char *fmt,...) {}
int valid_fname(char *s) {return 1;} char *oname(int i) {return "none";}
int fobn(char *s) {return -1;}
void store_parts(FILE *f,PERSONA *p) {} void store_flags(FILE *f,PERSONA *p) {}
void load_parts(char *s,PERSONA *p) {} void load_flags(char *s,PERSONA *p) {}
'''
        body += function('cr/uaf.c', 'int save_load_plr(')
        body += r'''
int main(void) {
 char dir[]="/tmp/cdirt-uaf-XXXXXX",name[]="Tester";
 PERSONA a={0},b={0}; FILE *file;
 assert(mkdtemp(dir)); assert(!chdir(dir)); assert(!mkdir("T",0700));
 a.rplr.storage[0]=-1;
 a.ublock.pscore=UINT_MAX; a.ublock.pstr=123; a.ublock.pdam=17; a.ublock.parmor=19;
 a.player.coins=INT_MAX; a.player.pmagic=87; a.player.pkilled=91; a.player.pdied=7;
 a.player.first_on=3000000000LL;
 strcpy(a.player.ptitle,"%s the Brave"); a.player.prompt="%h %n";
 assert(save_load_plr(&a,name,1,1)==1);
 assert(save_load_plr(&b,name,0,0)==1);
 assert(b.ublock.pscore==UINT_MAX && b.ublock.pstr==123 && b.ublock.pdam==17 && b.ublock.parmor==19);
 assert(b.player.coins==INT_MAX && b.player.pmagic==87 && b.player.pkilled==91 && b.player.pdied==7);
 assert(b.player.first_on==3000000000LL && !strcmp(b.player.prompt,"%h %n"));
 free(b.player.prompt);
 file=fopen("T/Tester","w"); fprintf(file,"Prompt     > \nStrength   2147483648\n"); fclose(file);
 assert(save_load_plr(&b,name,0,0)==-1 && b.player.prompt==NULL);
 file=fopen("T/Tester","w"); fprintf(file,"Score      -1\n"); fclose(file);
 assert(save_load_plr(&b,name,0,0)==-1);
 file=fopen("T/Tester","w"); fprintf(file,"Username   %0200d\n",0); fclose(file);
 assert(save_load_plr(&b,name,0,0)==-1);
 unlink("T/Tester"); rmdir("T"); chdir("/"); rmdir(dir); return 0;
}
'''
        self.run_c('#include <sys/stat.h>\n' + body)

    def test_editor_and_mail_reader(self):
        body = r'''
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define Boolean int
#define False 0
#define True 1
#define FREE free
struct txt_line {char *line; struct txt_line *next;};
typedef struct txt_line Text_Line; typedef Text_Line *Text_Ptr;
Text_Ptr txt_start,txt_curr; int numlines,curr_line;
'''
        body += function('cr/edit.c', 'void new_line(')
        body += function('cr/edit.c', 'void free_list()')
        body += function('cr/mail.c', 'int cdirt_getline(')
        body += r'''
int main(void) {
 char *line=memory_alloc(10001,1), buff[8],path[]="/tmp/cdirt-mail-XXXXXX";
 int fd=mkstemp(path); assert(fd>=0);
 memset(line,'x',10000); new_line(line);
 assert(strlen(txt_start->line)==10001 && txt_start->line[10000]=='\n');
 free_list(); free_list(); free(line);
 assert(write(fd,"1234567\n",8)==8); lseek(fd,0,SEEK_SET);
 assert(cdirt_getline(fd,buff,sizeof(buff))==7 && !strcmp(buff,"1234567"));
 assert(cdirt_getline(fd,buff,sizeof(buff))==-1 && !*buff);
 lseek(fd,0,SEEK_SET); assert(write(fd,"12345678\n",9)==9); lseek(fd,0,SEEK_SET);
 assert(cdirt_getline(fd,buff,sizeof(buff))==-1 && buff[7]==0);
 close(fd); unlink(path); return 0;
}
'''
        self.run_c(body)

    def test_partial_failed_socket_writes(self):
        body = r'''
#define M_BUFFLEN 4096
#define IO_STATS
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define FREE free
#define Boolean int
#define False 0
char *base,*rd,*wr; int cap=4096,output_flag=1,quit_flag=0,closed=0;
int bytes_sent=0,next_result=0;
#define out_buffer(P) base
#define out_read(P) rd
#define out_write(P) wr
#define out_size(P) cap
#define output(P) output_flag
#define hasquit(P) quit_flag
int find_pl_index(int fd) {return fd==5?0:-1;}
void setup_globals(int p) {} void sock_msg(char *fmt,...) {}
void close_sock(int fd) {closed++;}
int fake_write(int fd,char *text,int count) {return next_result;}
#define write fake_write
'''
        body += function('src/main.c', 'void write_packet(')
        body += r'''
int main(void) {
 base=memory_alloc(cap,1); strcpy(base,"abcdef"); rd=base;wr=base+6;
 errno=EAGAIN; next_result=-1; write_packet(5); assert(rd==base && !closed && bytes_sent==0);
 errno=EINTR; write_packet(5); assert(rd==base && !closed);
 next_result=2; write_packet(5); assert(rd==base+2 && output_flag && bytes_sent==2);
 next_result=4; write_packet(5); assert(rd==base && wr==base && !output_flag && bytes_sent==6);
 errno=EPIPE; next_result=-1; write_packet(5); assert(closed==1);
 free(base); return 0;
}
'''
        self.run_c(body)

    def test_duration_lifecycle_and_splotch_expansion(self):
        body = r'''
#define Boolean int
#define True 1
#define False 0
#define FREE free
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define max_players 1
#define BLANK 040
typedef struct duration {int spell,duration,tmp; struct duration *next;} SPELL_DURATION;
struct player { SPELL_DURATION *duration;} players[1];
int expired;
void duration_end(int p,int spell,int tmp) {expired++;}
char *words;
void shift(int,int);
'''
        for signature in ('void wipe_duration (', 'Boolean\ncheck_duration (',
                          'void handle_duration (', 'void\npush_duration ('):
            body += function('src/spell.c', signature)
        for signature in ('void swap(char **target', 'void shift(int base', 'void gswap(old, new)'):
            body += function('cr/splotch.c', signature)
        body += r'''
int main(void) {
 Text text={0}; char *target; int i;
 push_duration(0,1,2,0); push_duration(0,2,0,0);
 assert(check_duration(0,1) && check_duration(0,2));
 assert(players[0].duration && players[0].duration->next);
 handle_duration(0); assert(expired==1 && check_duration(0,1));
 handle_duration(0); assert(expired==1 && players[0].duration);
 handle_duration(0); assert(expired==2 && !players[0].duration);
 push_duration(0,1,2,0); wipe_duration(0); wipe_duration(0); assert(expired==3 && !players[0].duration);
 for(i=0;i<2000;i++) text_append(&text,"x "); target=text_take(&text);
 swap(&target,"x","lengthy replacement"); assert(strlen(target)==40000);
 swap(&target,"","no"); assert(strlen(target)==40000); free(target);
 words=memory_string("my my"); gswap("my","yourself");
 for(i=0;words[i];i++) words[i]&=0177;
 assert(!strcmp(words,"yourself yourself"));
 gswap("yourself yourself longer than input","x");
 assert(!strcmp(words,"yourself yourself")); free(words); return 0;
}
'''
        self.run_c(body)

    def test_file_tracking_owns_full_strings(self):
        body = r'''
#include <fcntl.h>
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define COPY(S) memory_string(S)
void bprintf(char *f,...) {} void mudlog(char *f,...) {}
'''
        source = (ROOT/'cr/mudfd.c').read_text()
        body += source[source.index('struct __fd_entry {'):source.index('int close1(int fd')]
        for signature in ('int close1(int fd', 'void * open1(char *message'):
            body += function('cr/mudfd.c',signature)
        body += r'''
int main(void) {
 char message[1024], path[]="/tmp/cdirt-track-XXXXXX";
 FILE *file; int fd=mkstemp(path); assert(fd>=0); close(fd);
 memset(message,'x',1023);message[1023]=0;
 file=open1(message,path,0,0,"w+"); assert(file);
 assert(fd_first && strlen(fd_first->msg)==1023 && !strcmp(fd_first->path,path));
 assert(!strcmp(fd_first->stdmode,"w+"));
 assert(close1(fileno(file),file)==0 && !fd_first);
 assert(!open1(message,"/nonexistent/cdirt-missing",0,0,"r"));
 assert(!fd_first); unlink(path); return 0;
}
'''
        self.run_c(body)

    def test_allocation_inventory_matches_source(self):
        import json
        from build_allocation_inventory import inventory
        actual = inventory()
        self.assertEqual(actual['unreviewed'], 0)
        self.assertEqual(actual, json.loads((ROOT/'tests/allocation_inventory.json').read_text()))

    def test_who_padding_and_malformed_codes(self):
        body = r'''
int count_colors(char *s) { return 0; }
'''
        body += function('src/mobile.c', 'char *makepad(char *text')
        body += function('src/mobile.c', 'char *makenewline (char *str')
        body += r'''
int main(void) {
 char a[]="x &=", b[]="x &+", c[]="x &=R", longword[256];
 char *pad=makepad("C-Dirt",33);
 assert(strlen(pad)==34 && !memcmp(pad,"C-Dirt",6)); free(pad);
 assert(makenewline(a,60)==a);
 assert(makenewline(b,60)==b);
 assert(makenewline(c,60)==c);
 memset(longword,'x',255);longword[255]=0;
 assert(makenewline(longword,60)==longword); return 0;
}
'''
        self.run_c(body)

    def test_full_mail_subject_and_body(self):
        body = r'''
#include <fcntl.h>
#define NEW(T,N) ((T *)memory_alloc((N),sizeof(T)))
#define COPY(S) memory_string(S)
#define OPEN open
#define CLOSE close
#define MAIL_DIR "."
#define LINE_LEN 300
#define DELIM "***"
#define mynum 0
char *pname(int i) {return "Tester";}
void bprintf(char *fmt,...) {}
typedef struct message {char *subject; char mailfrom[300],mailto[300],date[300];
 char status; off_t text; struct message *next,*prev;} Message;
typedef Message *Messageptr;
struct player {Messageptr first_msg,cur_msg,work_msg;} player, *cur_player=&player;
'''
        for signature in ('void free_mail_list(', 'char *mail_read_line(', 'void loadmail(void)', 'void write_text('):
            body += function('cr/mail.c',signature)
        body += r'''
int main(void) {
 char dir[]="/tmp/cdirt-fullmail-XXXXXX", *line; FILE *file; int i,fd;
 assert(mkdtemp(dir)); assert(!chdir(dir));
 file=fopen("Tester","w"); fputs("From: Other\nSubject: ",file);
 for(i=0;i<10000;i++) fputc('x',file);
 fputs("\nDate: Today\nStatus: N\n",file);
 for(i=0;i<10000;i++) fputc('y',file);
 fputs("\n***\n",file); fclose(file);
 loadmail(); assert(cur_player->cur_msg && strlen(cur_player->cur_msg->subject)==10000);
 fd=open("copy",O_WRONLY|O_CREAT|O_TRUNC,0600); assert(fd>=0); write_text(fd); close(fd);
 fd=open("copy",O_RDONLY); line=mail_read_line(fd); assert(strlen(line)==10000 && line[0]=='y');
 free(line); assert(!mail_read_line(fd)); close(fd); free_mail_list(); free_mail_list();
 unlink("Tester"); unlink("copy");chdir("/");rmdir(dir); return 0;
}
'''
        self.run_c(body)
