#include <stdio.h>
#include <stdlib.h>
#include "kernel.h"
#include "malloc.h"
#include "parse.h"
#include "log.h"
#include "bprintf.h"
#include "mobile.h"
#include "mud.h"

#define inp_level players[mynum].work
#define v_yes     players[mynum].work2[0]
#define v_no      players[mynum].work2[1]
#define v_abs     players[mynum].work2[2]
#define number    players[mynum].work2[4]
#define maxnum    players[mynum].work2[5]
#define VOTE_DIR "XRVOTE"
#define PATH_LEN 256
#define BUFF_LEN 256
#define PLAYER_LEN 80
#define MAX_VOTES 1000
#define TMP_ERROR "&+YSorry, couldn't create your temporary file.  Odd.\n"
#define V_PROMPT "&+W(&+BC-Vote&+W)&N &+MVote Yes, No, or Abstain &+W:&N "
#define V_QPROMPT "&+W(&+BC-Vote&+W)&N &+MVote on which topic (q=quit)&+W?&N "
#define V_TITLE "&+BCrossroads&N voting forum, &+R(C)&N 1996, Prometheus.\n\n"

struct __P_vote {
  char player[PLAYER_LEN];
  int yes;
  int no;
  int abs;
};
typedef struct __P_vote p_vote;
typedef p_vote *Voteptr;

void tally_votes(Boolean, Boolean, char *);
int show_list (char *);
int is_num (char *);

char *nomsg[] = {
  "Really?  Why not?",
  "Gee.. No fooling!",
  "Ok.",
  "Good enough.",
  "Nononono!",
  "I didn't think so either.",
  "Who could possibly agree to that?",
  "Please reconsider *smile*",
  "Why so negative?",
  "...SLAM"
};

char *yesmsg[] = {
  "Another yes-man, eh?",
  "Excellent!",
  "I agree whole-heartedly.",
  "Go for it d00d!",
  "It's groovy.",
  "Way-cool.",
  "Good choice.",
  "Yeah, that's a pretty cool idea.",
  "I don't agree with you...  but who listens to me anyways?",
  "Are you sure? Really really sure?"
};

char *absmsg[] = {
  "Splundge for me too, sir!",
  "Can't make up your mind?",
  "You really don't care, do you?",
  "I may just add some ambiguity to your bits... see how YOU like it.",
  "Wiffle-waffle??!?",
  "\"Do or do not, there is no try.\" -Yoda",
  "Umm... I dunno.",
  "Ok.",
  "Uhh... too many questions!",
  "Cool."
};

