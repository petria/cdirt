#ifndef _KERNEL_H
#define _KERNEL_H

#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "MACHINE.H"
#include "config.h"
#include "levels.h"
#include "exits.h"
#include "types.h"
#include "utils.h"
#include "flags.h"
#include "mudtypes.h"
#include "mudmacros.h"
#include "extern.h"
#include "files.h"

#define NEW(t, c)               ((t *)calloc(c, sizeof(t)))
#define BCOPY(s, l)		memcpy(calloc(1, l), s, l)
#define COPY(s)			strcpy(NEW(char, strlen(s) + 1), s)
#define EMPTY(p)		(*(p) == '\0')
#define EQ(a, b)		(strcasecmp((a), (b)) == 0)
#define WIZZONE_EXIST_H  72L      /* How many hours will a wizard's zone be
                                     kept in the game without him being on ? */

#endif
