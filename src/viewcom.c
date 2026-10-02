#include "kernel.h"
#include "viewcom.h"
#include "commands.h"
#include "wizard.h"
#include "mud.h"
#include "uaf.h"
#include "change.h"
#include "frob.h"
#include "bprintf.h"
#include "timing.h"
#include "mail.h"

extern void edit1(char *);
extern void configure(char *);
extern void votecom(char *);

void
viewcom (void)
{
  int i;
  char idle[64];

  if (!ptstflg (mynum, PFL_VIEWCOM)) {
    bprintf ("Pardon?\n");
    return;
  }
  bprintf ("&+CUser               Idle  Handler\n");
  bprintf ("&+B-------------------------------------------------------------------------------\n");
  for (i = 0; i < max_players; ++i) {
    if (is_conn(i) && see_player (mynum, i)) {

      if (ptstflg (mynum, PFL_SEEIDLE))
	sprintf (idle, sec_to_hhmmss (global_clock - prlast_cmd (i)));
      else
	sprintf (idle, sec_to_hhmmss (global_clock - plast_cmd (i)));

      bprintf ("&+w%-14.14s %8.8s  ", pname (i), idle);

      if (phandler (i) == get_command) {
	bprintf ("get_command (Last Command: %-.26s)\n", 
          plastcom(i) ? plastcom(i) : "none yet");
      }
      else if (phandler (i) == help2)
	bprintf ("help2 (Help: Page 2)\n");
      else if (phandler (i) == help3)
	bprintf ("help3 (Help: Page 3)\n");
      else if (phandler (i) == help4)
	bprintf ("help4 (Help: Page 4)\n");
      else if (phandler (i) == help5)
	bprintf ("help5 (Help: Page 5)\n");
      else if (phandler (i) == help6)
	bprintf ("help6 (Help: Page 6)\n");
      else if (phandler (i) == get_mailto)
	bprintf("get_mailto (Mail: Addressing message)\n"); 
      else if (phandler (i) == get_subject)
	bprintf("get_subject (Mail: Entering subject)\n");
      else if (phandler (i) == list)
	bprintf("list (Mail: Main menu)\n");
      else if (phandler (i) == configure)
	bprintf("configure (C-Dirt Configuration)\n");
      else if (phandler (i) == edit1)
	bprintf("edit1 (C-Dirt Editor)\n");
      else if (phandler (i) == votecom)
	bprintf("votecom (C-Dirt Voting System)\n");
      else if (phandler (i) == get_pname1)
	bprintf ("get_pname1 (Login: Getting Name)\n");
      else if (phandler (i) == get_pname2)
	bprintf ("get_pname2 (Login: Checking Name)\n");
      else if (phandler (i) == get_new_pass1)
	bprintf ("get_new_pass1 (Login: New Player: Getting Password)\n");
      else if (phandler (i) == get_new_pass2)
	bprintf ("get_new_pass2 (Login: New Player: Confirm Password)\n");
      else if (phandler (i) == get_passwd1)
	bprintf ("get_passwd1 (Login: Getting Password)\n");
      else if (phandler (i) == get_gender)
	bprintf ("get_gender (Login: New Player: Getting Gender)\n");
      else if (phandler (i) == kick_out_yn)
	bprintf ("kick_out_yn (Login: Confirm To Kill Other Session)\n");
      else if (phandler (i) == do_issue)
	bprintf ("do_issue (Login: Display Issue)\n");
      else if (phandler (i) == enter_vis)
	bprintf ("enter_vis (Login: Changing Vis Level)\n");
      else if (phandler (i) == do_motd)
	bprintf ("do_motd (Login: Display MOTD)\n");

      else if (phandler (i) == ask_old_passwd)
	bprintf ("ask_old_passwd (Passwd Change: Ask Old Password)\n");
      else if (phandler (i) == ask_new_passwd)
	bprintf ("ask_new_passwd (Passwd Change: Ask New Password)\n");
      else if (phandler (i) == ask_confirm_passwd)
	bprintf ("ask_confirm_passwd (Passwd Change: Confirm New Password)\n");

      else if (phandler (i) == unveilcom)
	bprintf ("unveilcom (Unveil Command)\n");
      else if (phandler (i) == becom)
	bprintf ("becom (Become Command)\n");
      else if (phandler (i) == frobcom)
	bprintf ("frobcom (Frob Command)\n");

      else if (phandler (i) == pager)
	bprintf ("pager (Using file pager)\n");

      else if (phandler (i) == klockcom)
	bprintf ("klockcom (Keyboard Locker)\n");

      else
	bprintf ("unknown\n");
    }
  }
  bprintf ("&+B-------------------------------------------------------------------------------\n");
}
