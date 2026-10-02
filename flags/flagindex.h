/************************************************************************
 * World flags : global flags for the entire game                       * 
 ************************************************************************/

char *Worldflags[] = {"Quests", "QuestsReset", TABLE_END};
int windex[MAX_WORLD_FLAGS+1] = { Q_MAX, Q_MAX };

/************************************************************************
 * Pflag defaults for each level                                        * 
 ************************************************************************/

char *Pflagflags[] = {"Pflags", "Masks", TABLE_END};
int pflindex[MAX_PFL_FLAGS+1] = {PFL_MAX, PFL_MAX };

/************************************************************************
 * Location flags for each room                                         * 
 ************************************************************************/

char *Locflags[] = { "Lflags", "LflagsReset", TABLE_END};
int lindex[MAX_LOC_FLAGS+1] = { LFL_MAX, LFL_MAX };
char **lflagsindex[] = { Lflags, Lflags };

/************************************************************************
 * Object flags for each object                                         * 
 ************************************************************************/

char *Objflags[] = { "Aflags", "AflagsReset", "Oflags", "OflagsReset", 
                     TABLE_END };
int oindex[MAX_OBJ_FLAGS+1] = { AFL_MAX, AFL_MAX, OFL_MAX, OFL_MAX };
char **oflagsindex[] = {  Aflags, Aflags, Oflags, Oflags };

/************************************************************************
 * Mobile flags for each mobile and player                              * 
 ************************************************************************/

char *Mobflags[] = {"Pflags", "PflagsReset", "Mask", "MaskReset",
                    "Mflags", "MflagsReset", "Sflags", "SflagsReset",
                    "Nflags", "NflagsReset", "Eflags", "EflagsReset",
                    "Qflags", TABLE_END};

int mindex[MAX_MOB_FLAGS+1] = { PFL_MAX, PFL_MAX, PFL_MAX, PFL_MAX,
                 MFL_MAX, MFL_MAX, SFL_MAX, SFL_MAX, NFL_MAX, NFL_MAX,
                 EFL_MAX, EFL_MAX, Q_MAX };

char **mobflagsindex[] = { Pflags, Pflags, Pflags, Pflags, Mflags, Mflags,
                           Sflags, Sflags, Nflags, Nflags, Eflags, Eflags };
