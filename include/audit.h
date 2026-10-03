#ifndef CDIRT_AUDIT_H
#define CDIRT_AUDIT_H

#ifdef CDIRT_AUDIT
void audit_boot(void);
int audit_descriptor(void);
int audit_manual_timer(void);
void audit_request(void);
void audit_begin(const char *);
void audit_end(void);
void audit_dispatch(int);
void audit_action(const char *);
void audit_output(int, const unsigned char *, unsigned long);
#else
#define audit_boot() ((void)0)
#define audit_descriptor() (-1)
#define audit_manual_timer() 0
#define audit_request() ((void)0)
#define audit_begin(s) ((void)0)
#define audit_end() ((void)0)
#define audit_dispatch(v) ((void)0)
#define audit_action(s) ((void)0)
#define audit_output(p, s, n) ((void)0)
#endif

#endif
