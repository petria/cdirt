/*
    Defining this will cause some variable values to be displayed when
    functions like glist() are executed. It is not pretty, or particularly
    useful to most people. Useful when looking for bugs, sometimes. 
*/
#undef SHOW_RAW_GROUP_DATA

/*
    REPORT command. 
    REPORT_WHO > 1 Reports to just those in the room.
    REPORT_WHO = 1 Reports to everyone in the group and in same room as mynum.
    REPORT_WHO = 0 Reports only to those in same group and in same location.
    REPORT_WHO < 0 Reports to just the group. 
*/
#define REPORT_WHO  -1

/* 
    BONUS_FACTOR_DAM	: The amount which we multiply pdam(victim) by to get 
			  score to be gained on death blow. Dirt3.0 it is 10
    BONUS_FACTOR_ARMOR	: Same as above, except for armor. Additional bonus
			  not in dirt3.0, but since armor makes mobs harder to 
			  kill. :) Set to 0 to leave it out.
    BONUS_PER_MEMBER	: The % bonus added to the score gained for the death
			  blow, since it is divided up by the group members,
			  they still get less than if they killed alone.
    MAX_BONUS		: The maximum bonus allowed to be added.
*/
#define BONUS_FACTOR_DAM	10
#define BONUS_FACTOR_ARMOR	5
#define BONUS_PER_MEMBER	25 /* % */
#define MAX_BONUS		75 /* No more than 3 members affect it */

/* Prototypes for group.c */

void group(void);
void ungroup(int pl);
void reportcom(void);
void init_group(void);
void glist(void);
void kill_bonus(int a, int v);
void gtell(void);
void check_group_leader(int lloc, int dir);

void promote();

