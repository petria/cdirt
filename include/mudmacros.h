#ifndef _MUDMACROS_H
#define _MUDMACROS_H

#include "macros.h"

#define pzname(C)                   ublock[C].pzname
#define ozname(C)                   objs[C].ozname

#define fpbn(C)                     find_mob(False, C)
#define fmbn(C)                     find_mob(True, C)
#define fobn(obname)                findob(mynum, obname, ALL, -1)

#define objcont(plr, item, obname)  findob(plr, obname, CONT, item)
#define objinv(plr, obname)         findob(plr, obname, INV, -1)
#define objloc(plr, obname)         findob(plr, obname, LOC, -1)

#define is_host_banned(x)           infile(BAN_HOSTS, x)
#define is_host_who_banned(x)       infile(BAN_WHOS, x)
#define is_host_username_banned(x)  infile(BAN_ACCTS, x)
#define is_host_silent(x)           infile(SILENT_LOGIN, x)
#define is_player_banned(x)         infile(BAN_CHARS, x)
#define is_illegal_name(x)          infile(ILLEGAL_FILE, x)
#define privleged_user(x)           infile(PRIVED_FILE, x)
#define is_plr_monitored(x)         infile(MONITOR_CHAR, x)
#define is_host_monitored(x)        infile(MONITOR_HOST, x)

#define is_in_game(C)           ((C) >= 0 && (C) < numchars \
				 && !EMPTY(pname(C)) && ploc(C) > -1 \
				 && ((C) >= max_players || players[C].iamon))

#define is_conn(C)		players[C].is_conn
#define is_ban(C)		players[C].host_ban
#define	is_userban(C)		players[C].user_ban
#define ident(C)                players[C].ident
#define resolved(C)             players[C].resolved
#define his_or_her(C)		(psex(C) ? "her" : "his")
#define him_or_her(C)           (psex(C) ? "her" : "him")
#define he_or_she(C)            (psex(C) ? "She" : "He")

/* Players and Mobiles */

#define pfirst_obj(C)           first_int(pinv(C))
#define lfirst_obj(C)           first_int(linv(C))
#define lfirst_mob(C)           first_int(lmobs(C))
#define ofirst_obj(C)           first_int(oinv(C))
#define zfirst_obj(C)           first_int(zobjs(C))
#define zfirst_mob(C)           first_int(zmobs(C))
#define zfirst_loc(C)           first_int(zlocs(C))
#define first_obj(C)            first_int(C)
#define first_mob(C)            first_int(C)
#define first_loc(C)            first_int(C)

#define ishere(C)               p_ishere(mynum, C)
#define dumpitems()             dumpstuff(mynum, ploc(mynum))
#define create(C)               oclrbit(C, OFL_DESTROYED)
#define is_classname(C)         (C != NULL && findclass(C) != NULL)
#define needsquests(C)          rplrs[C].needsquests
#define iswieldby(C, V)         (ocarrf(C) >= WIELDED_BY && iscarrby(C, V)) 
#define iswornby(C, V)          (isworn(C) && iscarrby(C, V))
#define isavl(C)                (ishere(C) && iscarrby(C, mynum))
#define listobject(C, V)        aobjsat(C, V, 0)
#define mlobjsat(C, V)          aobjsat(C, CARRIED_BY, V)

#define testpeace(C)            (the_world->w_peace || \
                                 ltstflg(ploc(C), LFL_PEACEFUL))

#define is_grouped(C)           ((C < max_players && is_in_game(C)) ? \
                                 pgrouped(C) : -1)

#define prndsinroom(C)		rplrs[C].rounds_in_room
#define setprndsinroom(C, V)	(prndsinroom(C) = (V))
#define pawaymsg(C)		players[C].awaymsg
#define pcarry(C)               players[C].pcarry
#define setpcarry(C, V)         (pcarry(C) = (V))
#define pangry(C)               ublock[C].angry
#define atarget(C)              rplrs[C].atarget
#define amud(C)                 rplrs[C].amud
#define ftimer(C)               ublock[C].ftimer
#define setpangry(C, V)         (pangry(C) = (V))
#define pcoins(C)               players[C].coins
#define pclass(C)               ublock[C].class
#define pgrouped(C)             rplrs[C].grouped
#define setpclass(C, V)         (pclass(C) = (V))
#define pac(C)                  ublock[C].pac
#define username(C)             rplrs[C].usrname
#define hostname(C)             rplrs[C].hostname
#define snooped(C)              rplrs[C].snooped
#define snooptarget(C)          rplrs[C].snooptarget
#define fildes(C)               rplrs[C].fil_des
#define linkdead(C)             rplrs[C].linkdead
#define ip_addr(C)              rplrs[C].ip_addr
#define port(C)                 rplrs[C].port
#define os(C)                   rplrs[C].os
#define keyboard(C)             rplrs[C].keyb
#define pretend(C)              rplrs[C].pretend
#define isawiz(C)               rplrs[C].isawiz
#define pretend(C)              rplrs[C].pretend
#define polymorphed(C)          rplrs[C].polymorphed
#define aliased(C)              rplrs[C].aliased
#define aliasto(C)              rplrs[C].aliasto
#define ivct(C)                 rplrs[C].me_ivct
#define newplr(C)               rplrs[C].newplr
#define ismonitored(C)          rplrs[C].ismonitored
#define oldscore(C)             rplrs[C].oldscore
#define last_cmd(C)             rplrs[C].last_cmd
#define logged_on(C)            rplrs[C].logged_on
#define rlast_cmd(C)            rplrs[C].rlast_cmd
#define replyplr(C)             rplrs[C].replyplr
#define plastcom(C)             players[C].prev_com
#define wd_them(C)              players[C].wd_them
#define hasquit(C)              players[C].hasquit
#define in_cmd(C)               players[C].in_cmd
#define resfd(C)                players[C].resfd
#define dnsfd(C)                players[C].dnsfd
#define limbo(C)                players[C].limbo
#define wd_him(C)               players[C].wd_him
#define wd_her(C)               players[C].wd_her
#define wd_it(C)                players[C].wd_it
#define newuaf(X)               save_load_plr(X, (X)->ublock.pname, True, True)
#define putuaf(X)               save_load_plr(X, (X)->ublock.pname, True, False)
#define getuaf(X, Y)            save_load_plr(Y, X, False, False)
#define inp_handler(C)          iplrs[C].inp_handler
#define inp_buffer(C)           iplrs[C].inp_buffer
#define output(C)               iplrs[C].output
#define out_size(C)             iplrs[C].out_size
#define inp_ptr(C)              iplrs[C].inp_ptr
#define ignore_input(C)         iplrs[C].ignore_input
#define out_buffer(C)           iplrs[C].out_buffer
#define out_write(C)            iplrs[C].out_write
#define out_read(C)             iplrs[C].out_read

/* these are to help debugging by allowing you to list open fd's in-mud */
#ifndef GENERATE
   #ifdef DEBUG_IO
     #define FOPEN(C, V)             __mfopen(__FILE__, __LINE__, C, V)
     #define OPEN(C, V, Z)           __mopen(__FILE__, __LINE__, C, V, Z)
     #define CLOSE(C)                __mclose(C)
     #define FCLOSE(C)               __mfclose(C)
     extern  FILE *__mfopen(char *, int, char *, char *);
     extern  void __mfclose(FILE *);
     extern  int __mopen(char *, int, char *, int, mode_t);
     extern  void __mclose(int);
   #else
     #define FOPEN(C, V)             fopen(C, V)
     #define OPEN(C, V, Z)           open(C, V, Z)
     #define CLOSE(C)                close(C)
     #define FCLOSE(C)               fclose(C)
   #endif
#endif

/* The main types of bits */

#define obits(O)        objs[O].bits
#define mbits(C)        ublock[C].bits
#define lbits(L)        room_data[L].bits
#define wbits()         the_world->wbits
#define pbits(C)        the_world->pbits[C]

/* These functions are useful for setting bits on UAF files */
#define ptst_flg(P, V, F)   test_bit(P, mindex, V, F)
#define pset_flg(P, V, F)   set_bit(P, mindex, V, True, F)
#define pclr_flg(P, V, F)   set_bit(P, mindex, V, False, F)

#define atstbit(O, V)	test_bit(obits(O), oindex, V, AFLAGS) 
#define asetbit(O, V)	set_bit(obits(O), oindex, V, True, AFLAGS)
#define aclrbit(O, V)	set_bit(obits(O), oindex, V, False, AFLAGS)
#define aresetbits(O)	copy_bits(obits(O), oindex, AFLAGS, AFLAGS_RESET)

#define otstbit(O, V)	test_bit(obits(O), oindex, V, OFLAGS) 
#define osetbit(O, V)	set_bit(obits(O), oindex, V, True, OFLAGS)
#define oclrbit(O, V)	set_bit(obits(O), oindex, V, False, OFLAGS)
#define oresetbits(O)	copy_bits(obits(O), oindex, OFLAGS, OFLAGS_RESET)

#define ptstflg(C, V)	test_bit(mbits(C), mindex, V, PFLAGS) 
#define psetflg(C, V)	set_bit(mbits(C), mindex, V, True, PFLAGS)
#define pclrflg(C, V)	set_bit(mbits(C), mindex, V, False, PFLAGS)
#define presetflgs(C)	copy_bits(mbits(C), mindex, PFLAGS, PFLAGS_RESET)

#define ptstmsk(C, V)	test_bit(mbits(C), mindex, V, PMASK) 
#define psetmsk(C, V)	set_bit(mbits(C), mindex, V, True, PMASK)
#define pclrmsk(C, V)	set_bit(mbits(C), mindex, V, False, PMASK)
#define presetmsk(C)    copy_bits(mbits(C), mindex, PMASK, PMASK_RESET)

#define mtstflg(C, V)	test_bit(mbits(C), mindex, V, MFLAGS) 
#define msetflg(C, V)	set_bit(mbits(C), mindex, V, True, MFLAGS)
#define mclrflg(C, V)	set_bit(mbits(C), mindex, V, False, MFLAGS)
#define mresetflgs(C)	copy_bits(mbits(C), mindex, MFLAGS, MFLAGS_RESET)

#define ststflg(C, V)	test_bit(mbits(C), mindex, V, SFLAGS) 
#define ssetflg(C, V)	set_bit(mbits(C), mindex, V, True, SFLAGS)
#define sclrflg(C, V)	set_bit(mbits(C), mindex, V, False, SFLAGS)
#define sresetflgs(C)	copy_bits(mbits(C), mindex, SFLAGS, SFLAGS_RESET)

#define ntstflg(C, V)	test_bit(mbits(C), mindex, V, NFLAGS) 
#define nsetflg(C, V)	set_bit(mbits(C), mindex, V, True, NFLAGS)
#define nclrflg(C, V)	set_bit(mbits(C), mindex, V, False, NFLAGS)
#define nresetflgs(C)	copy_bits(mbits(C), mindex, NFLAGS, NFLAGS_RESET)

#define etstflg(C, V)	test_bit(mbits(C), mindex, V, EFLAGS) 
#define esetflg(C, V)	set_bit(mbits(C), mindex, V, True, EFLAGS)
#define eclrflg(C, V)	set_bit(mbits(C), mindex, V, False, EFLAGS)
#define eresetflgs(C)	copy_bits(mbits(C), mindex, EFLAGS, EFLAGS_RESET)

#define qtstflg(C, V)	test_bit(mbits(C), mindex, V, QFLAGS) 
#define qsetflg(C, V)	set_bit(mbits(C), mindex, V, True, QFLAGS)
#define qclrflg(C, V)	set_bit(mbits(C), mindex, V, False, QFLAGS)
#define qresetflgs(C)	copy_bits(mbits(C), mindex, QFLAGS, QFLAGS_RESET)

#define qdclrflg(C)     set_bit(wbits(), windex, C, False, QDFLAGS)
#define qdsetflg(C)     set_bit(wbits(), windex, C, True, QDFLAGS)
#define qdtstflg(C)     test_bit(wbits(), windex, C, QDFLAGS)
#define qdresetflgs()   copy_bits(wbits(), windex, QDFLAGS, QDFLAGS_RESET)

#define ltstflg(C, V)	test_bit(lbits(C), lindex, V, LFLAGS) 
#define lsetflg(C, V)	set_bit(lbits(C), lindex, V, True, LFLAGS)
#define lclrflg(C, V)	set_bit(lbits(C), lindex, V, False, LFLAGS)
#define xlresetflgs(C)	copy_bits(lbits(C), lindex, LFLAGS, LFLAGS_RESET)

#define is_head(C)              atstbit(C, AFL_HEAD)
#define is_foot(C)              atstbit(C, AFL_FOOT)
#define is_feet(C)              atstbit(C, AFL_FEET)
#define is_hand(C)              atstbit(C, AFL_HAND)
#define is_hands(C)             atstbit(C, AFL_HANDS)
#define is_face(C)              atstbit(C, AFL_FACE)
#define is_back(C)              atstbit(C, AFL_BACK)
#define is_arm(C)               atstbit(C, AFL_ARM)
#define is_arms(C)              atstbit(C, AFL_ARMS)
#define is_leg(C)               atstbit(C, AFL_LEG)
#define is_legs(C)              atstbit(C, AFL_LEGS)
#define is_chest(C)             atstbit(C, AFL_CHEST)
#define is_neck(C)              atstbit(C, AFL_NECK)

#define head(C)                 ublock[C].body.head
#define r_arm(C)                ublock[C].body.r_arm
#define l_arm(C)                ublock[C].body.l_arm
#define r_leg(C)                ublock[C].body.r_leg
#define l_leg(C)                ublock[C].body.l_leg
#define r_foot(C)               ublock[C].body.r_foot
#define l_foot(C)               ublock[C].body.l_foot
#define r_hand(C)               ublock[C].body.r_hand
#define l_hand(C)               ublock[C].body.l_hand
#define chest(C)                ublock[C].body.chest
#define back(C)                 ublock[C].body.back
#define face(C)                 ublock[C].body.face
#define neck(C)                 ublock[C].body.neck

#define armor(D)                (D).armor
#define bodyname(D)             (D).name
#define vital(D)                (D).vital
#define loc(D)                  (D).loc
#define attached(D)             (D).attach
#define hasarmr(D)              ((D)->armor != -1)

#define wears_head(C)           (armor(head(C)) != -1)
#define wears_chest(C)          (armor(chest(C)) != -1)
#define wears_back(C)           (armor(back(C)) != -1)
#define wears_face(C)           (armor(face(C)) != -1)
#define wears_neck(C)           (armor(neck(C)) != -1)
 
#define wears_rfoot(C)          (armor(r_foot(C)) != -1)
#define wears_lfoot(C)          (armor(l_foot(C)) != -1)
#define wears_feet(C)           (wears_rfoot(C) && wears_lfoot(C))

#define wears_rhand(C)          (armor(r_hand(C)) != -1)
#define wears_lhand(C)          (armor(l_hand(C)) != -1)
#define wears_hands(C)          (wears_rhand(C) && wears_lhand(C))

#define wears_rleg(C)           (armor(r_leg(C)) != -1)
#define wears_lleg(C)           (armor(l_leg(C)) != -1)
#define wears_legs(C)           (wears_rleg(C) && wears_lleg(C))

#define wears_rarm(C)           (armor(r_arm(C)) != -1)
#define wears_larm(C)           (armor(l_arm(C)) != -1)
#define wears_arms(C)           (wears_rarm(C) && wears_larm(C))

#define pagg(C)                 ublock[C].pagg
#define setpagg(C, V)           (pagg(C) = (V))
#define pagg_reset(C)           ublock[C].pagg_reset

#define pspeed(C)               ublock[C].pspeed
#define setpspeed(C, V)         (pspeed(C) = (V))
#define pspeed_reset(C)         ublock[C].pspeed_reset

#define pdam(C)                 ublock[C].pdam
#define setpdam(C, V)           (pdam(C) = (V))
#define pdam_reset(C)           ublock[C].pdam_reset

#define parmor(C)               ublock[C].parmor
#define setparmor(C, V)         (parmor(C) = (V))
#define parmor_reset(C)         ublock[C].parmor_reset

#define pscore(C)		(ublock[C].pscore)
#define setpscore(C, V)		(pscore(C) = (V))

#define pfighting(C)		(ublock[C].pfighting)

#define phelping(C)		(ublock[C].phelping)
#define setphelping(C, V)	(phelping(C) = (V))

#define psitting(C)		(ublock[C].psitting)
#define setpsitting(C, V)	(psitting(C) = (V))

#define pwpn(C)			(ublock[C].pweapon)
#define setpwpn(C, V)		(pwpn(C) = (V))

#define plev(C)			(ublock[C].plev)
#define setplev(C, V)		(plev(C) = (V))
#define plev_reset(C)		(ublock[C].plev_reset)

#define pmobile(C)              ((C) >= max_players)

#define pftxt(C)                (ublock[C].pftxt)
#define pexam(C)                (ublock[C].p_exam)

#define plang(C)		ublock[C].planguage 
#define setplang(C, V)		(plang(C) = (V))
#define is_eng(C)		(plang(C) == NFL_ENGLISH)

#define psex(C)			ststflg(C, SFL_FEMALE)
#define player_level(C)         levnam(plev(C), pclass(C), psex(C))

#define pvis(C)			(ublock[C].pvis)
#define setpvis(C, V)		(pvis(C) = (V))
#define pvis_reset(C)		(ublock[C].pvis_reset)

#define pstr(C)			(ublock[C].pstr)
#define pmaxstrength(L)		(67 + 8 * (L))
#define maxstrength(P)          pmaxstrength(plev(P))
#define setpstr(C, V)		(pstr(C) = (V))
#define pstr_reset(C)		(ublock[C].pstr_reset)

#define ploc(C)			(ublock[C].ploc)
#define ploc_reset(C)           (ublock[C].ploc_reset)
#define sendloc(C)              (int2idx(ploc(C), LOC))

#define phome(C)                (ublock[C].phome)     

#define pname(C)                (ublock[C].pname)
#define setpname(C, V)          (strcpy(pname(C), (V)))
#define pname_reset(C)          (ublock[C].pname_reset)

#define ptitle(C)               players[C].ptitle
#define setptitle(C, V)         (strcpy(ptitle(C), (V)))

#define pwimpy(C)               ublock[C].pwimpy
#define pwimpy_reset(C)         ublock[C].pwimpy_reset
#define setpwimpy(C, V)         (pwimpy(C) = (V))

#define pnum(C)                 (C - max_players)
#define pzone(C)                ublock[C].zone
#define ppermanent(C)           ((C) < num_const_chars)
#define plast_cmd(C)            (rplrs[C].last_cmd)
#define prlast_cmd(C)           (rplrs[C].rlast_cmd)
#define plogged_on(C)           (rplrs[C].logged_on)
#define powner(C)               zname(pzone(C))

#define pinv(C)                 (&ublock[C].objects)
#define pnumobs(C)              set_size(pinv(C))
#define pobj_nr(N, C)           int_number((N), pinv(C))

#define pkilled(C)              (players[C].pkilled)
#define setpkilled(C, V)        (pkilled(C) = (V))

#define pdied(C)                (players[C].pdied)
#define setpdied(C, V)          (pdied(C) = (V))

#define pchannel(C)             (players[C].pchannel)
#define setpchannel(C, V)       (pchannel(C) = (V))

#define pmagic(C)               (players[C].pmagic)
#define pmaxmagic(L)            (12 + 3*(L))
#define maxmagic(P)             (((pclass(P) == MAGE) || (pclass(P) ==PRIEST)) \
                                  ? pmaxmagic(plev(P)) : 0)
#define setpmagic(C, V)         (pmagic(C) = (V))

#define pspeech(C, V)           ublock[C].pspeech[V]
#define setpspeech(C, V, L)     (strcpy(pspeech(C, V), (L)))

#define pchance(C)              ublock[C].pchance
#define setpchance(C, V)        (pchance(C) = (V))

#define phandler(C)             iplrs[C].inp_handler->inp_handler

#define pconv(C)                rplrs[C].pconverse
#define setpconv(C, V)          (pconv(C) = (V))

#define ppager(C)               (players[C].pager.len)
#define setppager(C, V)         (ppager(C) = (V))

#define islogged(C)             rplrs[C].logged
#define pfollow(C)		rplrs[C].i_follow

/* Locations */
#define exists(L)               (L < numloc && L > -1)
#define lpermanent(L)           ((L) < num_const_locs)
#define laltitude(L)		room_data[L].r_altitude
#define laltitude_reset(L)	room_data[L].r_altitude_reset
#define lexit(L, E)             room_data[L].r_exit[E]
#define lexit_reset(L, E)       room_data[L].r_exit_reset[E]
#define lshort(L)               room_data[L].r_short
#define llong(L)                room_data[L].r_long
#define lname(L)                room_data[L].r_name
#define lowner(L)               zname(lzone(L))

#define lexits_to_me(L)         (&room_data[L].exits_to_me)
#define lzone(L)                (room_data[L].zone)

#define linv(L)                 (&room_data[L].objects)
#define lnumobs(C)              set_size(linv(C))
#define lobj_nr(N, L)           int_number((N), linv(L))

#define lmobs(L)                (&room_data[L].mobiles)
#define lnumchars(C)            set_size(lmobs(C))
#define lmob_nr(N, L)           int_number((N), lmobs(L))

/* Objects */

#define oarmor(O)	        objs[O].oarmor
#define oarmor_reset(O)	        objs[O].oarmor_reset
#define odamage(O)		objs[O].odamage
#define odamage_reset(O)	objs[O].odamage_reset
#define osetarmor(O,V)		(oarmor(O) = (V))
#define osetdamage(O,V)		(odamage(O) = (V))

#define ovis(O)                 (objs[O].ovis)
#define osetvis(O, V)           (ovis(O) = (V))
#define ovis_reset(O)           (objs[O].ovis_reset)

#define oloc(O)			objs[O].oloc
#define oloc_reset(O)		objs[O].oloc_reset

#define state(O)		objs[O].ostate
#define state_reset(O)		objs[O].ostate_reset
#define ocarrf(O)		objs[O].ocarrf
#define ocarrf_reset(O)		objs[O].ocarrf_reset
#define setcarrf(O,C)           (ocarrf(O) = (C))
#define olongt(O, V)		(objs[O].odesc[V])
#define omaxstate(O)		(objs[O].omaxstate)
#define oaltname(O)		(objs[O].oaltname)

#define osize(O)		(objs[O].osize)
#define osize_reset(O)		(objs[O].osize_reset)

#define oweight(O)              (objs[O].oweight)
#define oweight_reset(O)        (objs[O].oweight_reset)
#define osetweight(O, V)        (oweight(O) = (V))

#define osetsize(O, V)          (osize(O) = (V))
#define oflannel(O)		otstbit(O,OFL_NOGET)

#define obaseval(O)		(objs[O].ovalue)
#define ovalue_reset(O) 	(objs[O].ovalue_reset)
#define osetbaseval(O, V)       (obaseval(O) = (V))

#define oname(O)                objs[O].oname
#define ospare(O)               (otstbit(O, OFL_DESTROYED) ? -1 : 0)
#define oexam_text(O)           objs[O].oexam_text

#define opermanent(O)           ((O) < num_const_obs)
#define olinked(O)              objs[O].linked
#define ozone(O)                objs[O].zone
#define oowner(O)               zname(ozone(O))

#define oinv(C)                 (&objs[C].objects)
#define onumobs(C)              set_size(oinv(C))
#define oobj_nr(N, C)           int_number((N), oinv(C))

/* Zones */

#define zname(Z)                zones[Z].z_name

#define zlatitude(Z)		zones[Z].latitude 
#define zrainfall(Z)		zones[Z].rainfall

#define zmaxlocs(Z)             zones[Z].maxlocs
#define zmaxmobs(Z)             zones[Z].maxmobs
#define zmaxobjs(Z)             zones[Z].maxobjs

#define zlocs(Z)                (&zones[Z].locations)
#define zmobs(Z)                (&zones[Z].mobiles)
#define zobjs(Z)                (&zones[Z].objects)

#define zpermanent(Z)           ((Z) >= 0 && (Z) < num_const_zon)

#define zadd_obj(O, Z)          add_int((O), zobjs(Z))
#define zadd_mob(O, Z)          add_int((O), zmobs(Z))
#define zadd_loc(O, Z)          add_int((O), zlocs(Z))

#define zremove_obj(O, Z)       remove_int((O), zobjs(Z))
#define zremove_mob(O, Z)       remove_int((O), zmobs(Z))
#define zremove_loc(O, Z)       remove_int((O), zlocs(Z))

#define znumobs(O)              set_size(zobjs(O))
#define znumloc(O)              set_size(zlocs(O))
#define znumchars(O)            set_size(zmobs(O))

#define zobj_nr(N, Z)           int_number((N), zobjs(Z))
#define zmob_nr(N, Z)           int_number((N), zmobs(Z))
#define zloc_nr(N, Z)           int_number((N), zlocs(Z))

#define broad(T)                    send_msg(DEST_ALL, 0, LVL_MIN, \
                                            LVL_MAX, NOBODY, NOBODY, T)

/* General */


#define find_int_number(C, V)   ((C < 0 || C >= V->len) ? SET_END : V->list[C])
#define randperc()              rand() % 100
#define tlookup(C, V)           glookup(C, strlen(C), V, strncasecmp)
#define get_set_mem_usage(C)    (C->len * sizeof(int))
#define hash(C, V)              (C % V)
#define get_table_mem_usage(C)  (C->len * sizeof(table_entry *))

#ifdef NO_BCOPY
#define bcopy(A, B, C)		memcpy(B, A, C)
#define bzero(A, B)		memset(A, 0, B)
#endif

#endif

