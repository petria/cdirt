#ifndef _PFLAGS_H
#define _PFLAGS_H

#define	PFL_NOEXOR	0	/* Can not be exo'd by same level plr   */	
#define	PFL_NOSNOOP	1	/* Can not be snooped by same level plr	*/
#define	PFL_NOHASSLE	2	/* Can not be attacked or magic'd	*/
#define	PFL_NOALIAS	3	/* Can not be aliased by same lev	*/
#define PFL_NOZAP	4	/* Can not be zapped by same lev 	*/
#define PFL_NOSUMMON	5	/* Can not be summoned by same lev	*/
#define PFL_NOTRACE	6	/* Can not be traced			*/
#define PFL_NOSTEAL	7	/* You cannot steal from him		*/
#define PFL_NOFORCE	8	/* Can not be forced by same lev	*/
#define PFL_NOPUNT	9	/* Can not be punted			*/
#define PFL_NOFORGET	10	/* Can not be forgotten			*/
#define	PFL_NOBURN	11	/* Can not be burned by same lev	*/
#define PFL_NOSIC	12	/* Can not be sic'd by same lev	        */
#define PFL_EDITOR	13	/* Can use the editor ("edit" command)	*/
#define PFL_NOPROBATION 14      /* No probation by same level           */
#define PFL_FROB	15	/* Can frob and 17-18 works on others	*/
#define PFL_CH_SCORE	16	/* May change score			*/
#define PFL_CH_LEVEL	17	/* May change level			*/
#define PFL_CH_MDATA	18	/* May change mobile data		*/
#define PFL_CLONE	19	/* May use the CLONE command		*/
#define PFL_LD_STORE	20	/* May use the LOAD and STORE commands	*/
#define	PFL_PFLAGS	21	/* May view privs of others		*/
#define PFL_MASK	22	/* May set priveliges in MASK		*/
#define PFL_ROOM	23	/* May set location bits and exits	*/
#define PFL_MFLAGEDIT	24	/* May set mobile bits			*/
#define PFL_OBJECT	25	/* May set object properties with SET	*/
#define PFL_TITLES	26	/* May set titles/setins permanently	*/
#define PFL_UAF		27	/* May get/set data for players not on	*/
#define PFL_EXOR	28	/* May exorcise players			*/
#define PFL_RESET	29	/* May reset the game			*/
#define PFL_CRASH	30	/* May crash the game (nasty!!)		*/
#define PFL_SNOOP	31	/* May snoop or trace other players	*/
#define PFL_HEAL	32	/* May use Heal				*/
#define PFL_ALIAS	33	/* May alias mobiles			*/
#define PFL_ALIASP	34	/* May alias other players		*/
#define PFL_RAW		35	/* May send Raw messages		*/
#define PFL_EMOTE	36	/* May use Emote			*/
#define PFL_ECHO	37	/* May use Echo and Echoall		*/
#define PFL_ZAP		38	/* May use Zap				*/
#define PFL_RES		39	/* May use Resurrect			*/
#define PFL_SEEHOST	40	/* Can see other players hosts		*/
#define PFL_STATS	41	/* May use STAT and SHOW and PRIVS	*/
#define PFL_GOTO	42	/* May use GOTO and IN/AT		*/
#define PFL_SUMOBJ	43	/* May summon objects			*/
#define PFL_WEATHER	44	/* May control the weather		*/
#define PFL_LOCK	45	/* May lock the game			*/
#define PFL_WRECK	46	/* May wreck doors (don't need key)	*/
#define PFL_PEACE	47	/* May declare peace or war		*/
#define PFL_SYSLOG	48	/* May look at the system log-file	*/
#define PFL_STARTINVIS	49	/* May start as invisible		*/
#define PFL_TRACE	50	/* May trace players/objects		*/
#define PFL_PUNT	51	/* May punt players 			*/
#define PFL_EFLAGEDIT	52	/* Can use eflag editor			*/
#define PFL_IGNORENOAT	53	/* Can override 'NoAt' lflag		*/
#define PFL_TIMEOUT	54	/* Can timeout users			*/
#define PFL_REBOOT	55	/* Can reboot the MUD			*/
#define PFL_TEXTRAW	56	/* Can use TextRaw			*/
#define PFL_PZAP	57	/* Can PZap (pretend Zap)		*/
#define PFL_SIGNAL	58	/* Can call SIGNAL			*/
#define PFL_BURN	59	/* Can burn other players		*/
#define PFL_SEEUSER	60	/* Can see username from Ident		*/
#define PFL_CANLOG	61	/* Can write to the Mudlog		*/
#define PFL_SIC		62	/* Can sic mobiles on players		*/
#define PFL_FLIST	63	/* Can use the ForgetList command	*/
#define	PFL_NFLAGEDIT	64	/* Can edit Nflags			*/
#define	PFL_SETTIME	65	/* Can set the climate time		*/
#define PFL_CHATMOD	66	/* Chat line moderator			*/
#define PFL_SOCKET	67	/* Can view/edit socket information	*/
#define PFL_IDLE	68	/* Pretending to be idle		*/
#define PFL_SEEANON	69	/* Can see people on the anon channel	*/
#define PFL_NOTIMEOUT	70	/* Cannot be tout'd			*/
#define PFL_NOECHO	71	/* Will not see echo's, echoall's, etc	*/
#define PFL_SEEIDLE	72	/* Can see real idle times		*/
#define PFL_VIEWCOM	73	/* Can see player input handlers	*/
#define PFL_SEESOCKET	74	/* Can see socket activity		*/
#define PFL_BAN		75	/* Can use the BAN command		*/
#define	PFL_PUNTALL	76	/* Can use the PUNTALL command		*/
#define PFL_ABERCHAT    77      /* Can use AberChat			*/
#define PFL_PROBATION   78      /* Can use the probation command        */
#define PFL_SFLAGEDIT   79      /* Can unconditionally edit sflags      */
#define PFL_CODER       80      /* MUD Coder                            */
#define PFL_AFLAGS      81      /* Can view armor flags                 */
#define PFL_AFLAGEDIT   82      /* Can edit armor flags                 */
#define PFL_OFLAGS      83      /* Can see object flags                 */
#define PFL_OFLAGEDIT   84      /* Can edit object flags                */
#define PFL_EFLAGS      85      /* Can view spell flags                 */
#define PFL_NFLAGS      86      /* Can view language flags              */
#define PFL_SFLAGS      87      /* Can edit set flags (sflags)          */
#define PFL_MASKEDIT    88      /* Can edit mask bits for pflags        */
#define PFL_LFLAGS      89      /* Can view location flags              */
#define PFL_LFLAGEDIT   90      /* Can edit location flags              */
#define PFL_MFLAGS      91      /* Can edit mobile flags                */
#define PFL_PFLAGEDIT   92      /* Can edit pflags                      */
#define PFL_CH_COINS    93      /* Can change players coins             */
#define PFL_MAX		94	/* Maximum Pflag			*/

#endif

