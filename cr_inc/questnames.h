/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       DIRT 3.0.x (Version of AberMUD)                     *
 *                                                                           *
 *  Authors of Dirt3.0: Alf Salte (Alf) and Nicknack. See file data/credits  *
 *                                                                           *
 *  File: The old Quests[] has been replaced with a struct with extra in it. *
 *        Must include quests.h BEFORE this file. (Ithor)                    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "quests.h"

/* Make sure that the # define in quests.h, match the entries in here. EG    */
/* EFOREST is defined as 0, then the first entry here is for EFOREST.        */
/*        { POINTS,    CRITICAL?, NAME              }                        */

QUEST Quests[] = {
          {  2,        True,    "ElvenForest"       },
          {  2,        False,   "Shazareth"         },
          {  1,        False,   "Excalibur"         },
          {  2,        False,   "HolyGrail"         },
          {  2,        False,   "FireKings"         },
          {  1,        False,   "Spike"             },
          {  3,        True,    "Painting"          },
          {  4,        False,   "Draknor"           },
	  {  0,	       False,   "UnUsed"	    },
	  {  4,	       True,    "Evolution"	    },
	  {  3,	       True,	"Zodiac"	    },
	  {  3,	       False,	"SunDisc"	    },
	  {  1,        True,    "Volcano"           },
	  {  2,	       True,	"Sabre"		    },
	  {  1,        False,   "Mana"              },
	  {  4,        False,   "Rainforest"	    },
	  {  1,	       True,    "Noxypickle"        },
	  {  5,	       False,	"Mirage"	    },
          {  0,        False,    TABLE_END          }
};
