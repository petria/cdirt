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
	  {  4,	       True,    "Evolution"	    },
	  {  3,	       True,	"Zodiac"	    },
	  {  3,	       False,	"SunDisc"	    },
	  {  1,        True,    "Volcano"           },
	  {  2,	       True,	"Sabre"		    },
	  {  4,        False,   "Rainforest"	    },
	  {  1,	       True,    "Noxypickle"        },
          {  4,        False,   "Talon"             },
          {  2,        False,   "Abyss"             },
          {  1,        False,   "Mithdan"           },
          {  1,        False,   "Orchold"           },
          {  1,        False,   "Ramses"            },
          {  1,        False,   "Faffner"           },   
          {  1,        False,   "Chlyon"            },
          {  1,        False,   "Mithril"           },
          {  1,        False,   "PinkElephant"      },
          {  0,        False,    TABLE_END          }
};
