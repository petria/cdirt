#ifndef _ACTIONS_H
#define _ACTIONS_H

Actionptr find_act(char *, Actionptr);
int     do_action(Actionptr);
void    add_action(Actionptr *base, Actionptr newone);
void    dump_act_vb(char *);
void    dump_action(Actionptr);
void    actionscom(void);
void    flowercom(void);
void    ticklecom(void);
void    petcom(void);
void    wavecom(void);
void    praycom(void);
void    meditatecom(void);
void    playcom(void);
void    rosecom(void);
void    wipecom(void);
void    flushcom(void);
void    astrcpy(int, char *, char *);
int     boot_actions(void);

#endif
