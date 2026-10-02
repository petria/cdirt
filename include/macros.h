#ifndef _MACROS_H
#define _MACROS_H

#define min(x,y)                ((x)>(y)?(y):(x))
#define max(x,y)                ((x)<(y)?(y):(x))
#define arraysize(a)            (sizeof(a)/sizeof(a[0]))
#define EVEN(N)                 (((N)&1) == 0)
#define ODD(N)                  (((N)&1) != 0)

#define setbits(W,F)            ((W) |= (F))
#define clrbits(W,F)            ((W) &= ~(F))
#define bits(W,M)               ((W) & (M))
#define xsetbit(W,V)            setbits((W),(1UL << (V)))
#define xclrbit(W,V)            clrbits((W),(1UL << (V)))
#define xtstbit(W,V)            bits(W, (1UL << (V)))

#define my_random()             ((unsigned int)(rand() >> 8))
#define rnd(X) 			(X > 0 ? (rand() % X +1) : 0)
#endif
