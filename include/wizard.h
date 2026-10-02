#ifndef _WIZARD_H
#define _WIZARD_H

void    snoopcom(void);
void	rawcom(void);
void	systemcom(void);
void	textrawcom(void);
void	deletecom(void);
void	opengamecom(void);
void    snoop_off(int);
void    auto_tout(void);
void	tournamentcom(void);
void	syslogcom(void);
void	levechocom(void);
void	echocom(void);
void	echoallcom(void);
void	echotocom(void);
void	emotecom(void);
void	emotetocom(void);
void	setcom(void);
void	exorcom(void);
void	setstart(void);
void	noshoutcom(void);
void	showlocation(int o);
void	showitem(void);
void	wizlock(void);
void	warcom(void);
void	peacecom(void);

void	zapcom(void);
void	pzapcom(void);
void	nowizcom(void);
void	showtty(void);
void	wiz_handler(int lvl, int flg);
void	nolinecom(int lvl, int flg, char txt[20]);
void	ttycom(void);
void    snoop_off(int);

void	nowishcom(void);
void	noslaincom(void);
void	idlecom(void);
void	puntcom(void);
void	puntallcom(void);
void	freaqcom(void);
void	ploccom(void);
void	cfrcom(void);
void	burncom(void);
void	silentcom(void);
void	wloadcom(void);
void	writelog(void);
void	atviscom(void);
void	siccom(void);
void	fakequitcom(void);
void	nopuntcom(void);
void	findcom(void);
void	follist(void);
void	toggleseeext(void);
void	togglecoding(void);
void	socketcom(void);
void	toggleseesocket(void);
void	litcom(void);
void	seeidlecom(void);
void	bresetcom(void);
void	toutcom(void);
void	debugcom(void);

extern void end_connection(int);

#endif
