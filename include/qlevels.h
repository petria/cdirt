/* qlevels.h: set the quest-requirements for each level here */

/* for the first qtype, this is the number of quests needed at each lev */

#if QTYPE == 1
int qlevels[LVL_WIZARD+1] = {0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
7, 8, 9, 10, 11, 12, 13};
#endif

/* each element corresponds to the number of qpoints needed at each lev */

#if QTYPE == 2
int qlevels[LVL_WIZARD+1] = { 0, 2, 2, 2, 4, 4, 4, 6, 6, 6, 6, 8, 8, 10,
  10, 12, 12, 14, 14, 16, 16, 18};
#endif

