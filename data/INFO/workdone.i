Ian @ Feb 25
{
  fixed both versions of MOTD, motd.i, and the motd file...
  put in a news.i
}

Guile @ Feb 25
{
  magic.c:
  -fixed invixcom function for 3 new wiz levels.
   You can now go invis to 1 above your level.  Also, Gods can see 
   eachother when max invis, as well as everybody else max invis.
   Max Invis levels are set at 1 wlevel above your level.
  mobile.c:
  -Added in Home, Newstyle commands.
}

Guile @ Feb 27
{
  -Fixed colors in 'iusers' and colors in "shout" and "say" per 
   legolas color requests.
  -Added in IUSERS, MUSERS, ISTARI, NOISTARI commands.
  -Added in (Lev:) and (Vis:) in mud.c in [%s has entered the game] line.
  -Added in NoSlain command.
}


South @ Feb 27
{
  bprintf.c:
  -fixed color parser to deal correctly with both invalid colorcodes and
   bold settings in colorcodes that contain both foreground and background
  -added colortrunc function

  bprintf.h:
  -added colortrunc prototype and visual_length macro

  change.c:
  -fixed check_title function to accept level argument since the format of
   a title change must still be checked even if the target is immortal
   i.e. a title of %n will crash the mud.

  actions:
  -removed evil
  -modified grin, lip, lsd, moan, tongue, unk, urge
  -added button, jbones, khand, lapf, lapm, lips, lkiss, lust, scratch,
         seduce, shy, slurp, snm, squirm, stroke, suck, tkiss, towel,
         vbite, wcream, whop

  SUMMARY:
   Improved colorcode parsing.
   Added color utility to truncate by visual length.
   Fixed %n title bug.
   Added lots of actions.
}

Guile @ Feb 28
{
 - Added HEALALL, fixed HEAL so you can't heal in a fight, or heal players
   who are already at max strength.
 - Added he_or_she macro.
 - Added Away and Busy
}

South @ Feb 28
{
  verbs.src:
  -added verb 'uptime'

  global.h:
  -added startup and resets variables

  extern.h:
  -added externs for startup and resets variables

  main.c:
  -initialized startup and resets variables
  -modified handle_packet() to improve snoop output

  parse.c:
  -added case for 'uptime' define VERB_UPTIME to call uptimecom()

  commands.c:
  -incremented resets variable in reset()
  -added uptimecom() function to implement 'uptime'
  -modified infocom() function to use the file pager

  bprintf.c:
  -modified kiputs() to improve snoop output
  -added show_file(), page_file(), and page_quit() functions for paging

  bprintf.h:
  -added prototypes for show_file() and page_file()

  mobile.c:
  -reduced length of output on all users-style commands by one (for snoop)
  -modified stats command to use partial name matching fpbn() (was fpbns())

  actions:
  -added bleh, buh, cookie, confuse, cwd, doh, grind, honey, oob, pthb,
         slapf, smash, spock, squirt, whisper

  SUMMARY:
   Added uptime command.
   Improved snoop output.
   Added file paging and attached it to info.
   Fixed 'stats' to work for partial names.
   Added a bunch more actions.
}

South @ Mar 1
{
  commands.c:
  -fixed an apparent typo in examcom() which tried to print an unopened
   file pointer rather than the string it should have

  mobile.c:
  -fixed inconsistency with Apprentice level in std_title()
  -added special cases in chkfight() for Thyrannen zone

  questnames.h:
  -added Draknor quest for Thyrannen zone

  quests.h:
  -added #define for Draknor quest

  objsys.c:
  -added special cases in dogive() for Thyrannen zone

  fight.c:
  -added special cases in wound_player() for Draknor quest (Thyrannen zone)

  quests.c:
  -added mudlog for completion of quests

  verbs.src:
  -removed smash verb since it's an action now

  actions:
  -added prod, thank

  SUMMARY:
   Fixed a bug in examine which (potentially) crashed the game if the
    target was a mobile with an examine.
   Fixed a bug in frob which crashed the game when the new level was
    greater than LVL_WIZARD and less than LVL_EMERITI.
   Added special cases for Thyrannen zone and Draknor quest.
   Added logging of quest completion.
   Added a couple more actions.
}

IAN: (yes he did something in the CODE!!!)

 added some messages sent to uppers when a player:
	Echoalls
	echo's
	wizecho's
	Exo'
	and I might have forgotten some as well
 and that was in wizard.c
Added the messages to players when specific stuff is done in the Thyrannen zone
well grant made VERY easy so it hardly counts.

Updated MOTD.



3/2/94 (thanks guys, I am learning!)

South @ Mar 5
{
  objsys.c:
  -added newlines in long draknor messages in dogive()
  -added a line to set score of brother2 to mynum for draknor quest in dogive()

  commands.c:
  -rewrote scorecom() to fix several small errors and make it cleaner

  mobile.c:
  -changed level specification requirement in showplayer() to istari+

  pflags:
  -moved chscore from awiz to istari

  SUMMARY:
   Fixed silly error in Draknor quest.
   Revamped score command to be more accurate and prettier.
   Gave immortal level specification - Wizard (500) - to istari+.
   Gave ability to change one's own score to istari+.
}

South @ Mar 7
{
  objsys.c:
  -added newline after elephant trumpeting broadcast for grove zone

  magic.c:
  -added in_file check to healcom() where it checks for fighting status
  -changed some "they"s to "he/she"s

  commands.c:
  -changed some "they"s to "he/she"s

  mudlist.i:
  -moved 'MustangII' to 'Eclipse' and fixed name address
  -added Bladerunner, Elven, Prairie, Rainbow, and Underworld

  SUMMARY:
   Made a few messages prettier.
   Fixed a problem with heal by which you could not heal players not in game
   Updated mudlist.
}

South @ Mar 10
{
  wizard.c:
  -changed setploc() and lookcom() to teletrap() in homecom()

  rooms.c:
  -rearranged parsing order in findloc()

  quests.c:
  -added check for target being mynum in questcom()
  -changed strcasecmp to strncasecmp in qlookup()

  sea.zone:
  -switched Desc[0] and Desc[1] for excalibur, made state and maxstate 2

  actions:
  -added jaw, tag

  SUMMARY:
   Fixed the home command so it announces departure and arrival.
   Fixed location parsing so that the order is player, room, object, mobile.
   Removed logging of changing quests on self.
   Changed quests command to allow partial quest name matching.
   Fixed excalibur so it's not visible at first and later the descs are right.
   Added a couple actions.
}

South @ Mar 11
{
  bin/verbperms.c:
  -imported this file to generate partial verbs for use in the illegal file

  illegal:
  -filled file with general, verb, action, mobile, and local illegal names

  banned_chars:
  -emptied file

  utils.c:
  -imported new match() and infile() functions using better pattern matching

  SUMMARY:
   Fleshed out the illegal file.
   Changed pattern matching functions to handle more complex patterns.
}

South @ Mar 14
{
  flags.c:
  -moved check for MaskEdit pflag further down in maskcom()

  commands.c:
  -moved send_msg() announcing poses inside switch

  codernames.h:
  -added definition of coders[] array of coder names

  wizlist.c:
  -changed return value for player name in parse_wizlist to -2 from -1
  -changed s to wordbuf in error message for wizlistcom()
  -included codernames.h
  -restructured wizlistcom() function to deal with 'coders' separately

  mobile.c:
  -added checks for 'coder' level in iusers/musers/users/ustat/who/stat cmds

  SUMMARY:
   Changed mask command so that you need pflagedit, not maskedit, to see masks.
   Changed pose command so that poses are only announced for the broadcast.
   Fixed 'wizlist <player name>' option on wizlist and (null) error message.
   Added support for 'coder' level outside normal levels setup.  All player
    names in the 'coders' array in codernames.h will have the associated
    levelname 'Coder' in all users-style commands and in stat, and furthermore
    none of the names will show up on wizlist output unless the level 'coder'
    is specified.
}

South @ Mar 15
{
  commands.c:
  -added send_msg in aliascom() and in unalias() announcing aliasing/unaliasing

  magic.c:
  -added send_msgs in sumcom() announcing summoning by immortals
  -added checks for summoning things already where they'd be summoned to

  objsys.c:
  -added send_msg in stealcom() announcing stealing by players from mortals

  rooms.c:
  -added send_msgs in gotocom() announcing tiptoeing to next immortal level up+
  -added check for goto'ing the player's current location

  mobile.c:
  -modified check_busy() to include level check

  spec_obj.h:
  -added beach boat to boats
  -added camelot shield to shields
  -added camelot platemail to armors

  zones:
  -Camelot: added Female sflag to Morgana
  -Heaven: changed CARRIED_BY to WIELDED_BY on mjolnir
  -Valley: added NoZap, NoSummon pflags to MooMoo
           added east exit from rocky bank to Hillock@eastcoast
  -Village: added north exit from shore to SandyBeach@eastcoast
  -Eastcoast: changed south exit in SandyBeach from cove@ to shore@village

  actions:
  -modified congrat, smash, whimper (%~ was wrong in others message)
  -added bucket

  SUMMARY:
   Put messages announcing aliasing, unaliasing, summoning by immortals of
    players/mobiles/objects, stealing by players from mortals, and tiptoeing.
   Changed busy flag to make a player only busy to players of lower levels.
   Touched up a few zones (incl. linking eastcoast) and equipment exclusions.
   Touched up on the actions.
}

** CHECKPOINT **  --> /internal/efomalon/heavenly_bodies/backups/Mar15.tar.gz
-rw-r--r--  1 efomalon   917039 Mar 15 04:49 Mar15.tar.gz

South @ Mar 19
{
  verbs.src:
  -added verbs 'medic' and 'soapbox'

  parse.c:
  -added case for 'medic' define VERB_MEDIC to call mediccom()
  -added case for 'soapbox' define VERB_SOAPBOX to call soapboxcom()

  magic.c:
  -added mediccom()

  commands.c:
  -added soapboxcom() and support utility soapbox_handler
  -added utilities for listing files: list_files(), find_file()
  -added qsort utility alpha_cmp()

  mail.c:
  -changed disp_file() to show_file() to use pager

  SUMMARY:
   Added Legolas' medic command (plus some extras to spice it up).
   Added soapbox command and several keen file listing utilities.
   Put paging into viewing mail.
}

ian march 19th or around there.

fixed hosts command.
thats about it.
did some otherstuff too
the thyrannen zone code was extracted, it
is in the file name "thyrannen.code"
suggested to south, that a "soapbox" command would be cool
My idea was to use info files.
But south had a better idea.
he had the idea to code it cool, like mail almost,
so south coded it, it was neat, I watched.
I should have fixed eastcoast zone exit, but I did not *pthb*
well that is about it.

Legolas on March 21st.
added timeout to verbs.src to see if that enables timeout....we'll see.
Not a big thing either way.
Added fuzzball mobile.
added contests.i
Not much for now...
:)
Also added in my cat, Paramour.  *smile*
added in wiz4, wiz5, god4, god5, god6, god7, god8, and god9
added mobiles: Lobob, Paramour, FuzzBall
added items: hit, taco, pen, remote, flower

Guile@March 21st 1994
{
  -fixed "restricted access" function in rooms.c to work with LVL_SHALAFI
  -uncommented "restricted access" so it will work, (Per Legolas request)
}

South @ Mar 20
{
  mudtypes.h:
  -added ONAME_LEN define, made all pronoun storage vars arrays instead of ptrs

  kernel.h:
  -changed definition of EMPTY() define to include check for nonexistent ptr

  global.h:
  -added immort_levels[] to centralize several similar arrays in code

  extern.h:
  -added immort_levels[] external declaration

  mud.c:
  -modified initialization of pronoun storage vars in find_free_player_slot()

  parse.c:
  -modified check/setting of pronoun storage vars in brkword() and parse_1()

  commands.c:
  -modified check of pronoun storage vars in pncom()

  mobile.c:
  -modified iuserscom() to display short level with visibility
  -modified setting of pronoun storage vars in list_people() and setname()

  objsys.c:
  -modified setting of pronoun storage vars in fobnsys() and list_objects()

  wizard.c:
  -added code to cut short execution of timeoutcom() since it's not fully in
  -changed wizlockcom() to use immort_levels[]

  pfilter.c:
  -included global.h and changed ok() to use immort_levels[]

  ZONES:
  -partially finished revamping format of all the zones
   zones touched: xlimbo, wiz, waste, village, valley, treehouse,
                  tower, thyrannen, sherwood, shalafi, sea, quarry

  SUMMARY:
   Changed some variables in the player record and modified code to reflect it.
   Centralized definition of an immortal version of the levels[] array.
   Changed iusers to display visibilities more informatively.
   Reformatted some zones.
}

South @ Mar 21
{
  main.c:
  -added check for loss of input handler at start of handle_packet()

  mobile.c:
  -set exec.c variable doing_table to False in xcrapup()

  commands.c:
  -added closedir calls in find_file() that were forgotten initially

  SUMMARY:
   Fixed three bugs:
    1) Sometimes even after a player is disconnected, data will still be
       received through the socket and sent to handle_packet, but the
       player's inp_handler variable is NULL and so it crashes. (proper fix?)
    2) If while executing a case in the event array a player is exited by
       longjmp to JMP_QUITTING in main.c, the doing_table variable used in
       determining whether exec.c is still looking through one of its tables
       is not cleared.  This leads to a lockout of commands like say, look,
       and all other commands handled solely by the arrays in database.c.
    3) Forgot closedir() calls in find_file(), over time causing the number
       of open files to exceed the maximum allowed (RLIMIT_NOFILE = 64).
}

Guile @ Mar 22
{
  rooms.c - commented out the "restricted access" stuff, (per Legolas request)
}

Ian march 22

{
Played with myself mostly, but while I was at it, I added send_messages
for snoop, and took the mudlog of failed crashes out.
BFG
BIG FUCKIN DEAL
}

South @ Mar 22
{
  *deep breath*...

  mudtypes.h:
  -changed PERSONA structure:
   increased title storage size
   added hostname storage
   added first_on and wiz_time markers
   added time_on and mortal_time cumulative trackers
   added lfl_homes array
   added wimpy and lines variables
  -changed UBLOCK_REC structure:
   added plflhomes array
   added pwimpy and plines variables
  -changed PLAYER_REC structure:
   added connect_at, first_on, time_on, mortal_time, wiz_time

  global.h:
  -added game_is_open flag

  extern.h:
  -added game_is_open external declaration

  quests.h:
  -removed Q_GUXX and Q_ORCKING

  questnames.h:
  -removed Guxx and OrcKing

  files.h:
  -added define for 'crashed' file

  group.c:
  -added check for group leader being self in check_group_leader()

  main.c:
  -commented out report of exceptions pending on file decriptors
  -added command line option '-s' for starting up game in shutdown mode
  -added opengame or shutdown for open_on_startup flag in main_loop()
  -signal_handler() prints 'crashed' file, awards points for inventory,
   and saves all players on fatal signal

  uaf.c:
  -modified pers2player() and player2pers() to reflect PERSONA changes
  -modified get_gender() to store connect time on a new player

  readolduaf.c:

  change.c:
  -modified restriction on title length to be visual_length

  commands.c:
  -added info in globalcom() about open/closed status
  -removed check_nologin() function

  condition.c:
  -removed check_nologin() case

  database.c:
  -removed check_nologin() case

  parse.c:
  -added case for 'timestats'

  mud.c:
  -added initialization of player's connect_at marker
  -changed privileged_user() to be non-static

  wizard.c:
  -modified opengame(), added do_opengame()
  -modified shutdown() - shutdown(1) is crash, added do_shutdown()
  -crash/shutdown now awards points for inventory and saves kicked off players

  fight.c:
  -commented out questset for Guxx

  objsys.c:
  -commented out questset for OrcKing

  conv.c:
  -added to handle uaf_rand conversion

  readuaf.c:
  -added as a utility in reading uaf_rand

  mobile.c:
  -added timestatscom() to implement 'timestats'
  -modified hostscom() to display connect time, on for, and idle time as well

  SUMMARY:
   Modified uaf_rand file to increase title length, add hostname records,
    add records on first connect, time spent on the game, etc, and add
    lflaghomes, wimpy, and lines availability for future expansion.
   Also modified uaf_rand to remove Guxx and OrcKing quests.
   Fixed shutdown.
   Fixed group follow bug that was hanging the game.
   Added timestats command and support code.
}

Guile @ 3/23/94
{
  -added in timeoutcom from old Mirage code into wizard.c.
  -commented out timeoutcom that was already in wizard.c.
  -added in "is_higher_lev" function in wizard.c (not sure if
   something similar already exists, if so, delete it)
  -added "beepcom" function in wizard.c
  -increased max_players to default to 50 instead of 32 in main.c.
   and in /include/globals.h
   This was done because max_players was reached today.
  -fixed declaration of *a and *b in cmp_player function in mobile.c 
   so the stupid "arg 4 qsort" warnings would go away.
  -fixed "alpha_cmp" in commands.c to skip the same qsort warning.
  -added in SFL_HEARBACK in /include/sflags.h
  -added "HearBack" to /src/sflagnames.h
  -added checks for SFL_HEARBACK in commands.c in saycom, shoutcom, and 
   tellcom.
  -added check for SFL_HEARBACK in echotocom in wizard.c
  -added colorcodes to "list_people" function in mobile.c so "(Providing
   Light)", etc wouldn't be in the same color as last title color was.
}

South @ Mar 24
{
  verbs.src:
  -added verb 'wimpy'

  files.h:
  -added define for 'registered' file

  mud.c:
  -added is_registered_name()
  -added check for name in registered file when host is banned in login_ok()

  ban.c:
  -switched order of args in match() call in is_host/player_banned()

  mobile.c:
  -changed hostscom() to display "(unavailable)" on a God for a non-God

  parse.c:
  -added case for 'wimpy'

  change.c:
  -added change_wimpy() and associated structure modifications

  fight.c:
  -added check for do_flee() if wimpy value < strength in wound_player()

  move.c:
  -changed do_flee() to choose a random exit on flee with no args
   (also accomodates use of wimpy flee)
  -added support function can_go()

  rooms.c:
  -added check for exists() after getroomnum() call in homecom()

  flags.c:
  -added check for exists() after getroomnum() call in lflagscom()

  mail.c:
  -commented out del_mail() call for mortals after displaying mail file

  magic.c:
  -added check for linked object for plev(mynum) < LVL_ISTARI in sumcom()

  wizardc:
  -added check for OFL_NOGET for plev(mynum) < LVL_ISTARI in setcom()

  fullhelp:
  -added help for 'wimpy'
  -modified help for 'mail' and 'nomail'

  general:
  -removed ending newline in several calls to mudlog()

  SUMMARY:
   Added 'registered' file to allow certain names in regardless of ban on host.
   Changed hosts command to not show host info on Gods to non-Gods to remain
    consistent with tty command.
   Added wimpy command and check for wimpy flee whenever a player is hit.
   Fixed crashbug in lflags command for no given offset in location arg.
   Changed mailer to not delete autodelete mail for mortals, either.
   Made summon not work for non-uppers on linked objects.
   Made setting and unsetting of NoGet oflag not work for non-uppers.
   Updated help file.
   Took out newlines in syslogging by removing ending newlines in any
    mudlog() call that had it throughout the code.
}

South @ Mar 25
{
  actions:
  -fixed shmoo
  -made snowb and thank 'afar'
  -added rofld and prozac (Suraklin)

  fullhelp:
  -fixed colorcode on help for 'mail'

  SUMMARY:
   Messed a bit with the actions.
   Messed a bit with the help file.
}

South @ Mar 26
{
  mudtypes.h:
  -added page_lines and eof_marker vars in PLAYER_REC
  -removed isawiz var in PAYER_REC
  -removed plines from UBLOCK_REC

  mudmacros.h:
  -changed definition of plines() to reflect move from ublock to players

  utils.c:
  -added fp_close() to close a file pointer which may or may not be a pipe

  utils.h:
  -added function prototype for fp_close()

  bprintf.c:
  -added page_setup() and changed paging functions to use page_lines variable

  bprintf.h:
  -added function prototype for page_setup()

  verbs.src:
  -added 'plines'

  parse.c:
  -added case for 'plines'

  change.c:
  -changed change_wimpy() slightly and added change_plines() to handle 'plines'

  magic.c:
  -made it impossible to force someone who is currently in a pager

  mud.c:
  -removed use of isawiz - privileged_user() handles that now

  commands.c:
  -removed use of isawiz in unveilcom() and replaced with privileged_user()

  mobile.c:
  -fixed some formatting errors in timestatscom()

  wizard.c:
  -fixed colorcodes in opengame/shutdown

  main.c:
  -took out do_opengame() call in the signal handler (don't really know why
   I had it there in the first place)

  adjust.c:
  -added to initialize p_lines to -1 (default) and reinitialize timestats info

  fullhelp:
  -added entries for timestats and plines
  -reformatted all recent additions to put 'usage' in the right place

  help1:
  -added timestats, wimpy, and plines on mortal help screen

  SUMMARY:
   Put paging into help.
   Added 'plines' command to modify lines/page for each player, and support
    code in the bprintf.c pager code.
   Removed isawiz variable and replaced use with privileged_user().
   Initialized all players' plines, and reinitialized all players' total
    time on (and mortal time on) vars since several random errors have been
    fixed since it was installed.
   Minor fixup work on timestats, opengame/shutdown, signal handler, and
    help files.
}

Guile @ Mar 28
{
  mobile.c
  -modified "ustatcom" so it qsorts and put in colorcodes.
   Also added "Sex" and "Status" fields.
  -added in # of players at bottom of "hosts" command.
  mudlist.i
  -updated and added "Cond" field.
  wizlist.c
  -modified table for levelnames to have the ==='s be same length
   as level name, instead of ======== for all levels.
}

South @ Mar 28
{
  mobile.c:
  -modified cmp_player() to order the same way on every comparison
   (it was returning zeros on equal levels, leading to randomness)
  -renamed usercom() to userscom() and ustatcom() to ustatscom()
  -modified userscom(), iuserscom(), muserscom(), ustatscom(), whocom(), and
   hostscom() to use item1 for name matching
  -added case in vic2f() for talonshield (defendershield quality)
  -added case in jumpcom() for talon25->talon4

  mobile.h:
  -renamed usercom() to userscom() and ustatcom() to ustatscom()

  parse.c:
  -changed usercom() call to userscom() and ustatcom() call to ustatscom()

  condition.c:
  -modified USERS to call userscom() and PLAYERS to call hostscom()

  spec_obj.h:
  -added talonshield to list of shields

  magic.c:
  -added case in sumcom() for talonshield (defendershield quality)

  sendsys.h:
  -removed MODE_NOWIZ
  -added MODE_NOBLIND

  sendsys.c:
  -removed code to implement MODE_NOWIZ
  -added code to implement MODE_NOBLIND

  wizard.c:
  -changed instances of MODE_NOWIZ to MODE_NSFLAG|MS(SFL_NOWIZ)

  objsys.c:
  -added case in putcom() for putting ruby in staff to make firestaff
  -added cases in eatcom() for healing potion and transporting spring
  -replaced several osetbit()/setoloc() calls with eat() or destroy()

  commands.c:
  -added case in examcom() for finding ruby in desk
  -added case in wavecom() for waving firestaff to open door in talon25
  -added case in pushcom() for pushing china figuring to open/close passage
  -fixed toggling of destroyed bit on ropes at cliff steps and moor pit
   (throwcom(), tiecom(), untiecom())

  fight.c:
  -added case in hit_player() for talonshield (defendershield quality)
  -added code to reduce Talon's damage from 50 to 5 against an opponent
   wearing talonshield

  valley.zone:
  -changed RopeEast to not start destroyed
  -added exit link in valley42 east to ledge1 via RopeEast

  ledge.zone:
  -changed RopeBotPit and RopeWest to not start destroyed

  moor.zone:
  -changed RopeTopPit to not start destroyed

  shalafi.zone:
  -changed door in quarry7 to be invis when closed and have a 'door' altname

  talon.zone:
  -added to implement Iguana's Talon zone

  files:
  -added entry for talon.zone

  mkdata.c:
  -changed forced case rules to capitalize the first letter of every word in
   a mobile's name

  zones.c:
  -rewrote zones command

  SUMMARY:
   Adjusted all users-style commands to allow specific record selection via
    partial name matching on the argument (e.g. 'users sou')
   Added Iguana's Talon zone and associated support cases.
   Took out a useless sendsys mode flag and added a missing one.
   Fixed rope manipulation around the rope bridge and moor pit.
   Fixed Thumper's secret door to be secret.
   Fixed capitalization of mobile names of more than one word.
   Put in a new zones command with nice colors and formatting.
}

South @ Mar 31
{
  parse.c:
  -removed resetting of player's idle time in doaction()
  -changed 'credits' to use the pager (\001f\003 -> show_file)

  main.c:
  -added resetting of player's idle time in handle_packet()

  mud.c:
  -added check in talker() for non-gods seeing (unavailable) on entering gods

  mobile.c:
  -fixed hosts command to not show idle time in both On Time and Idle Time

  dgod.zone:
  -added Rachael's room and two objects

  credits:
  -changed 'Gods of ElvenMUD' to read 'Founders of ElvenMUD'
  -added some zone designer credits

  SUMMARY:
   Moved idle time resetting to where newline-terminated input is actually
    received from each player's socket connection, and added &-prefix for
    dgod+ and Idle sflag handling code for when that flag is added.
   Fixed enter message so non-gods see '(unavailable)' on god connects.
   Fixed dumb error in hostscom() which made it show idle time twice.
   Added some things to the dgod zone for Rachael.
   Flushed out 'credits' some and put it on the pager.
}

South @ Apr 1
{
  verbs.src:
  -added 'idle'

  parse.c:
  -added case for 'idle'

  sflags.h:
  -added SFL_IDLE define

  sflagnames.h:
  -added "Idle" sflag

  mobile.c:
  -fixed timestats wiz-time code in calib_player() (added setup_global() calls)
  -changed showplayer() to not display Idle sflag in Various to lower levels
  -fixed minor typos in consid_move() and added a new puff message

  wizard.c:
  -added send_msg() calls in rawcom(), warcom(), and peacecom()
  -added idlecom() to handle 'idle'

  main.c:
  -added check for SFL_IDLE in handle_packet() to not reset idle time

  zones.c:
  -small fix in formatting (messed up column split when zonnum%4 == 0)

  objsys.c:
  -fixed dryad to go to LOC_DEAD_DEAD when given the garland

  group.h:
  -changed REPORT_WHO to -1 so only group members see a report

  group.c:
  -fixed typo on reportcom() ("You reports ...")

  commands.c:
  -added case for nibelung fire in examcom() to set arrow color
  -added case for nibelung arrows in throwcom() to open tower base for potion

  talon.zone:
  -rewrote the desk's examine so you don't 'find a ruby' every time

  sherwood:
  -minor spelling errors (this zone stinks, btw)

  camelot:
  -minor spelling errors

  nibelung:
  -adjusted a few examines, changed maxstate of fire to 3

  actions:
  -swapped text on target vs all message sets on 'sing'

  SUMMARY:
   Fixed crashbug in setting wiztime for timestats.
   Added reports for raw, war, and peace.
   Small format fix in zones.
   Small fix for oaktree zone so Dryad won't still be seen in the room after
    someone gives her the garland.
   Changed report so only group members see it, and fixed a minor typo.
   Added Idle sflag and idle command to toggle it.  Usable by Dgod+, if Idle
    is set on a player his/her idle time will not be reset.  Furthermore
    nobody below that player's immortal level will be able to see the Idle
    flag set on that player with stat.
   Added missing cases to finish implementing nibelung Faffner area.  Nothing
    about the firestone, fire, arrows, etc. was implemented.
   Minor corrections on some zonefiles: talon, sherwood, camelot, nibelung
   Fixed the sing action (again).
}

** CHECKPOINT **  --> /internal/efomalon/heavenly_bodies/backups/Apr1.tar.gz
-rw-r--r--  1 efomalon  1439750 Apr  2 10:13 Apr1.tar.gz

Guile @ Apr1
{
  mobile.c: in ustatscom fixed Vis so it shows all 5 digits, not Inv.
            in muserscom fixed it so the Fields won't be printed if no
                         mortals are on. It qsorts before the field printfs.
            in iuserscom same thing done.
  mud.c:    in do_motd added the ability to say "i" to go invis when you 
	    enter the game. 
}

South @ Apr 3
{
  objsys.c:
  -added ruby destruction in talon firestaff code in putcom()

  SUMMARY:
   Minor fix to Talon cases
}

Guile @ Apr 4
{
  group.c:
  -fixed crashbug in "reportcom" function. Now you can't do it while
   aliasing.
}

South @ Apr 4
{
  commands.c:
  -added line to set OBJ_ARENA_CRACK's state to 1 upon examining it

  arena.zone:
  -added line to set crack's maxstate to 1

  SUMMARY:
   Minor fix to arena zone so that examining the crack multiple times will
    only produce one carrion crawler.
}

South @ Apr 5
{
  wizard.c:
  -removed NoWiz sflag check in send_msg for wizecho

  SUMMARY:
   Changed wizecho so even those who are NoWiz see it.
    (was annoying to see soandso has wizecho'd without seeing the wizecho)
}

South @ Apr 6
{
  commands.c:
  -added pl1 checks and send_msg() calls in petcom(), ticklecom(),
    wavecom(), and blowcom()

  parse.c:
  -changed 'couples' to use the pager (\001f\003 -> show_file)

  actions:
  -minor spelling fix in drool

  couples:
  -minor colorcode tweaking

  general:
  -removed &N's immediately before newlines in all strings since the color
    parser automatically puts that code before newlines anyway
   * files modified: bprintf.c, commands.c, fight.c, magic.c,
                     mobile.c, mud.c, wizard.c, wizlist.c

  SUMMARY:
   Changed pet, tickle, wave, and blow to be able to be used as actions.
   Fixed a few colorcode errors in 'couples' and put it on the pager.
   Minor action fix.
   General code cleanup of extra &N's at the ends of message lines.
    (also fixes the odd behavior of 'wiz &' -> South: &N)
}

South @ Apr 7
{
  bprintf.c:
  -added check for end marker in eof test in page_file()

  magic.c:
  -commented out all of mediccom() that Legolas wrote by his request

  verbs.src:
  -added 'fungame', 'banhost', and 'banuser'

  parse.c:
  -added cases for 'fungame', 'banhost', and 'banuser'

  commands.c:
  -added fungamecom() to handle 'fungame'

  wizard.c:
  -added bancom() to handle 'banhost' and 'banuser'

  mud.h:
  -added prototype for is_hostname_banned() and is_player_banned()

  mud.c:
  -uncommented is_hostname_banned() and re-added is_player_banned()
  -moved connect_at initialization to find_free_player_slot() in the hopes
    that this will fix some slight inaccuracies.

  Makefile:
  -removed anything having to do with ban.c or ban.h

  actions:
  -modified neck (%~ was wrong in others message)

  help1:
  -reformatted a bit, created 'Just For Fun' header, moved medic there,
    and added 'fungame' under that header

  help3:
  -reformatted a bit, added banuser/banhost, and removed old ban commands

  fullhelp:
  -modified help for wimpy with a warning note
  -modified help for medic reflecting removal of Legolas' healing code
  -added help for banhost, banuser, and wildcards
  -modified help for timestats with a note mentioning restrictions

  info:
  -added qinfo listing

  quests.i:
  -fixed INFO QLIST to read INFO QINFO

  policy.i:
  -rewrote section on lagdeath refrobs

  SUMMARY:
   Minor pager fix.
   Removed all of Legolas' code, consisting of the healing part of medic.
   Added silly new command 'fungame' and code in commands.c to implement it.
   Added 'banhost' and 'banuser', and detached the old ban code in ban.c.
    Also set the banned checks back up in mud.c and their prototypes in mud.h.
   Minor action fix.
   Minor help modifications.
   Minor info corrections.
   Policy change about lagdeath refrobs reflecting wimpy feature.
}

}
 Ian someday in feb.
	Moved WORKDONE to ../data/INFO so it is no possible to view the file
from the game. woo hoo, arent I special?
}
