Following changes made by Mancini:
Wed May 29:  Fixed it so that "where" says it's a corpse if a mob is dead. :)
Wed May 29:  Fixed a few memory over-run problems with the new i/o and "\002"
             escape sequences.
Wed May 29:  Tty now works for players not currently on the game.
Wed May 29:  Made it so that the couples file is paged.
Wed May 29:  Inserted a newline after the quoted text in a reply.
Tue May 28:  Made it so that mobs corpses go to dead1 when they die.
Tue May 28:  Made it so that the runesword quits attacking dead mobs.
Tue May 28:  Fixed it so that you can pass the golem if he is dead.
Fri -- Mon:  Went to PurgCon so didn't do a THING here!  :)
Fri May 24:  Fixed outgoing Internet Email so that it works with the new editor.
Fri May 24:  Made it so that it tells people when away and you change your
             awaymsg.
Sun May 19:  Fixed a bug with rain while declining or inclining not using 
             (double) values while figuring out a percent.
Sun May 19:  Fixed a bug with wind speeds < 0 showing no messages
Sun May 19:  Fixed a bug with your awaymsg not being read from your pfile
             correctly in getuaf.
Sat May 18:  Fixed a bug that woulnd't let players in if their name was short
             for someone else.  Like shi & Shiloh.
Sat May 18:  Added awaymsg, awaybeeps, and seenumbers.
Fri May 17:  Fixed a bug in the wind code so that it didn't go up as fast.
Wed May 15:  Made it so that people lower level than someone else can still
             time them out IF they don't have the NoTimeOut pflag set.
Tue May 14:  Fixed some bugs with weather not being updated.
Tue May 14:  Fixed a bug with forceall forcing players not on game.
Tue May 14:  made it so that Puff only healed people that were sitting down.
Tue May 14:  Re-did all of the weather messages that got lost in the crash
             and added the code for displaying them.
Mon May 13:  Slept all day and didn't do a thing.  *grins*
Sun May 12:  Made Horus hit more often if they don't have the required 
             stones. (request of Shi)
Sun May 12:  Added color to the wear off messages.
Sun May 12:  Raised the mana requirements of blur and damage.
Sat May 11:  Did a LOT of work on our new weather system.  Just need to 
             add in the code for the mesages and for hurricanes to finish 
             the weather patter routines. 
Sat May 11:  Made it so that players can not kill/cast spells/steal from 
             each other.
Fri May 10:  Recovered from a crash and lost all changes dating back to
             Apr 26.  :(
Fri Apr 26:  Made it so that the prompt isn't overwritten in the mailer and
             pager and such.  :)
Fri Apr 26:  Made it so that players could not kill other players.
Thu Apr 25:  Added code to  maintain/update windspeed on the mud.
Wed Apr 24:  Added code for temperature of a specific room to the mud.
Wed Apr 24:  Rewrote the weather command for the new weather system. 
Mon Apr 22:  Removed every remenant of weather from the mud in preparation
             of Arctica's new weather system.  #ifdef'ed it all.
Sun Apr 21:  Did the coding to add rainfall in the zone files. 
Sat Apr 20:  Realized that I've been forgetting to update my Changelog.  :)
Sat Apr 20:  Added a new cfunction bool int_is_on_table() to see if an integer
             is a member of a pointer to a "-1" terminated integer table. 
Sat Apr 20:  Updated and re-wrote the is_armor and is_shield functions.
Sat Apr 20:  Fixed a crash bug in musers.
Sun Apr 14:  Fixed the rest of the stuff for mirage and added a few more
             specials that needed done.  (last minute stuff)
Sun Apr 14:  Fixed Volcano where the exits weren't resetting.
Sat Apr 13:  Fixed the mud where it woulnd't run.
Sat Apr 13:  Made a default wimpy of 25 instead of 0.
Sat Apr 13:  Finished the specials for mirage except for fill.
Fri Apr 12:  Made it so that wizx+ lights burnable objects with rub.
Fri Apr 12:  Coded some more for mirage quest.
Fri Apr 12:  Fixed crash bug with Volcano & killing the queen.
Thu Apr 11:  Fixed the "Non-existant room or exit" bug.
Thu Apr 11:  Fixed crash bug with eflaged mobs.
Thu Apr 11:  Added fillcom() and rubcom().  :)  (for mirage quest)
Thu Apr 11:  Fixed some zone stuff for mirage quest. (in zone file)
Thu Apr 11:  Added an entrance to Rainforest.  What happeend to it?  :)
Wed Apr 10:  Did a *bunch* more work on mirage.
Wed Apr 10:  Made the "ubull" command available to demi+ instead of shalafi+.
Wed Apr  9:  Did some work on the mirage quest.
Mon Apr  8:  Added altitudes into the mud.  :)
Sat Apr  6:  Fixed major crash bug with Shazareth and other spellcasting mobs.
Sat Apr  6:  DId all of the coding for Noxypickle.  'tis a quest.
Sat Apr  6:  Changed the CFLAGS 'cause of bad code generation that
             was causing random crashes from alias.
Sat Apr  6:  Changed the colors of the istari line.
Fri Apr  5:  Raised quest points requred to wiz from 29 to 33.
Fri Apr  5:  Fixed Rainforest so that rafts work.  :)
Fri Apr  5:  Fixed deluaf so it worked.  :)
Fri Apr  5:  Made Rainforest a quest and Evolution critical.
Fri Apr  5:  Made it so that quests are being checked to wiz!
Thu Apr  4:  Fixed a crash bug with mail being sent to mobiles. :)
Thu Apr  4:  Made it so that you have to use a mobs full name instead of
             abbreviations.
Wed Apr  3:  Made it so that we use TSCALE (for obj value) again.  :)
Wed Apr  3:  Made it so that people can log in if they have a shortened form
             of a mob name.  Ie Vamp couldn't log on because of Vampire.
Wed Apr  3:  Made it so that Demi+ can change lflags on others rooms.
Wed Apr  3:  Made it so that we don't see Inego's echoto's.  :)
Wed Apr  3:  Snoops are now logged.
Wed Apr  3:  Added on-line logging for summons.  Still needs a bit of work
             for objects.
Tue Apr  2:  Added on-line logging for flees.
Tue Apr  2:  Messed up objects and fixed them later that day.  :)
Wed Mar 27:  Fixed Zodiac so that it gives you the quest.
Wed Mar 27:  Fixed it so that you can put the sharktooth in the sharkjaw.
Sat Mar 23:  made it so that you could see the room@zone name from the zone 
             files in looks for demi+.
Sat Mar 23:  Changed it so that you can wave at someone.
Sat Mar 23:  Fixed it so that you level while fighting instead of just when
             you pit objects.  :)
Sat Mar 23:  Made it so that cloned zones are loaded on reboots/updates.
Tue Mar 19:  Fixed it so that you get credit for Evolution.
Tue Mar 19:  Made it so that wiz+ can talk to people in Soundproofed rooms.
Tue Mar 19:  Made a lot of minor changes to the mud including adding people
             to "info email" and the mudlist.  Also some minor changes to
             Volcano that Arctica sent me.
Tue Mar 19:  Raised the healing rates so they were more like the old XR.
Tue Mar 19:  Added a broad() message for Rainforest.
Tue Mar 19:  Made it so that we can do "*" game commands from the main mail
             prompt.
Tue Mar 19:  Disabled "alias" because it is causing crashes. (updates)

Weinerdog' Changes
02/30/96
 
got the zones to compile: note any objects which have spaces in their 
Pnames will cause the bootstrap to crash (took me forever to figure out what 
was wrong; gdb wouldn't give any useful info)

02/29/96

fixed a few warnings
found a bug in the mailer: mail <user> works, but after when you type
   mail, things get hosed... FIXED
dumped all of the zone files in
made some links to the data files in xroads/
began work on taking ZONE parts from commands.c --> put in xroads/ dir
finished with commands.c, probably mistakes everywhere
finished zones/quests, lots of errors
fixed errors, need testing

02/30/96

Problems:                    (* indicates not fixed )
talon quest not awarded
dinthiar not giving cloak
spike: dryad not opening portal
Draknor: Haggardly doesn't water pot and give it back
Volcano: Mayor doesn't give gold coin
* Sabre: can't enter lagoon
* painting: In the Vault (frobozz41) connected directly to  start1 instead
   of Outside the Vault (frobozz44)
excalibur desc same on land as sea
draknor,sabre,zodiac and volcano not part of world yet
* the mobs in zodiac have neg<spell> for all spells
crevace is a death room, amulet should be there instead
(question of installing x-roads zones over the ones supplied in idirt)
getting gives twice

put the xr score code in for the most part. pmess is still a little hosed
fixed the give problem
fixed the talon quest problem
put in the &N colorcode
put in draknor
fixed up scorecom, should work now
fixed excalibur problem
put in make clean, make depend and make reg

loads of stuff from objsys.c should be put in database.c or written as
functions... mancini? any ideas on which is better?

changed levels, made them more like xroads
necromancer: level inbetween istari and wizard (honorary position)
fixed problem with crevace
fixed quest code to report acquired quests
fixed quests: dinthiar, spike, draknor, volcano

* seg faults in evolution, rainforest, fairybook, fairytale

put in wallcom [not tested]

not in yet: corpses

linked up draknor, sabre, zodiac & volcano
put in XR's ban code, musers, who, users & hosts [none tested]

* fix includes for stuff in xroads/
* timestats code needed
new quest code needed

put in new player files

* hosts messed up
sflags load once ok, but don't save/load properly afterwords  -- FIXED

defaults for sflags, pflags, kills, mana should be set now
put in new quest code
put in database-replacement funcs
reworked info

quests that work:
ElvenForest     Shazareth       Excalibur       HolyGrail       FireKings
Painting        Talon           Zodiac          SunDisc		Mana
Spike

quests that don't work yet:
Draknor	        Evolution

fixed some warnings & quests
put in an old_inp_h for our mailer

problems with brother1 and brother2 in draknor
put in a few default sflags
fixed a minor bug in scorecom
added some color to scorecom
added more color to chat, tell & say
put in corpses
wrote a configuration program, not done yet
fixed a minor bug in mail
upgraded to idirt 1.80, fixed gen bug
put in evolution
made changes to newuaf.c, removed forget
put in fairybook, fairytale & rainforest [special code needed for these]
made some changes to puff
messed  around more with colors in chat, say & tell
added some color to mortal-help
added some color to the wiz lines
fixed a bug in clonecom
made it so that puff's messages only appear to people in the room she is
wrote a cache of mobiles who move
made a change to mobiles.c in movemob: only send exit/entry messages to
rooms with 1 or more players in them (cut running time in half)
fixed a crash bug in the mailer
fixed puff to not say/emote to everywhere
fixed freaq to give the correct title
fixed a crash bug in clone
changed the help files for CHAT/NOCHAT
cut the cpu-load about 65% by adding a check to send exit/entry msgs to a room 
which has fewer than 2 players/mobiles in it
fixed a problem with quests not resetting
made setin/out messages save automatically
seemed to have fixed the alias problem, tried it on about 6 mobiles
fixed the problem with mail & nopuff - had the same SFL number (DOH!)
fixed a bug in pflagscom which was allowing mortals to set their own pflags
fixed problems with syslog, actions & wizlist
fixed problems with mail & the mail sflag
unlinked pirate (not working) & tower (crashing) zones
switched to the old ban code
fixed draknor
fixed a really bad bug causing crashes with old players that had wimpy > 0
fixed Dump
wrote a little utility to expire player files: expire
added: aberd_stable makefile option
installed linux 1.3.76 (has new network drivers for my card)
fixed musers to report the correct STR
fixed it so that update works
fixed it so that you can run the mud & restart from any directory
put in some debug code in main.c: accept()
made it so that select() will only block for .1 sec, should fix lag
hopefully fixed the seemingly random crashes & inability to profile/debug
(made database.c an include file, as it should be, and moved event[] & status[]
to exec.c & timing.c)
made changes to main.c & INTERMUD

***
started work on a new output scheme, the mud currently only checks for avail
FD's with reads... new scheme is to group writes into a queue, ala tinyfugue.
***

changed main.c around a little, keeping input file descriptors seperate from
output & exception... made the player output stream buffered, the routines
could be made simpler (Note: this is part of the real mud)

removed code: socket messages (causing crashes)
made it so that intermud messages are sent through a stream
(should fix the cpu-eating bug :)
made it so that intermud properly shuts down
put in a fflush so that each player receives the first update message
made it so that intermud messages aren't buffered anymore [causing problems]
removed intermud fd's from the select() statement
made it so that intermud will shutdown properly on a crash
added a log file for bob
made it so that bob will talk back if you're the only one in the room with
him and/or you say something with the word "bob" in it.
bob will show up on the who list (not hosts/users/etc YET) even though he's
a normal mobile...
(changes in commands.c & mobile.c)
BUG FIX: the mailer died on messages without the delim character.
BUG FIX: made it so that the mud won't flush output to a lost connection...
when the mud is shutdown, the nologin file now has a port so that when the
experimental mud is shutdown, it won't mess up the official mud.
BUG FIX: now the pid files should be deleted properly...
BUG FIX: the mailer was trying to send mail to mobiles who have the same 
         names as players, only happened when the players were off-line.
wrote an editor, not tested yet.
tested editor a little bit
put in option: delete [num-num]
put in option: quote text
BUG FIX: affecting forceall, problem in objsys.c, fobnh()
made it so that all mortals have the correct starting locations
added N [new buffer], S [save buffer] & in-editor commands to xredit
now cloned zones are stored before an update
made it so that cloned objects aren't stored in the objects file
it will now save cloned objects on quit (may or may not be good)

made changes to the editor to make it work smoothly with the mailer &
put the editor into the mailer, a big part from get_notes is missing, also:
1) save will cause multiple messages to be sent :(
probably more...

Found a bug in spells.c, hopefully fixed it but if there's a problem, it should
give us some info...
made it so that the mud won't hang when a player file has 0 bytes
now the mailer will page output after saving it to a temporary file
moved some things around in xredit.c: now it informs you of new mail when
it actually arrives...
VERB_GCHAT, VERB_CHANNEL, VERB_INFO, VERB_WIZLIST, VERB_ACTIONS, VERB_POLICY,
VERB_CONVERSE, VERB_OPTIONS, VERB_KLOCK & VERB_SAVE all crashed when a player 
was aliased.
fixed a few warnings
put the editor in change desc
did a temporary fix for the spell problem
fixed a bug in change desc again
fixed a bug in the editor again
made it so that the editor won't kill the mailer
worked on the configuration editor a little
removed saveset entirely since it saves them automatically now
fixed the kick out Y/N bug
fixed a bug in the pager - not updating the prompt when done...
wrote a voting booth thing
now syslog DEBUGs are saved in a seperate file for extra privacy
fixed a bug in the voting thing
fixed 2 bugs in newuaf.c
fixed a bug in wizard.c, an opened file not being closed
made the verbs a binary tree
worked a little on the new io routines
fixed the multiple-kill msg bug

[GAVE UP COMPLETELY ON CHANGELOG]

