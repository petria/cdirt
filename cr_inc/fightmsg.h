#define CODES(x, y, z) ((x) = __add_codes(attacker, victim, area, y, z))
#define FMSG(x, y)      __fightmsg(attacker, victim, area, x, y)
#define GMSG(x)         __generalmsg(attacker, victim, area, x)

/* default type ofr a slain message */

#define SLAIN       DEST_ALL, MODE_NSFLAG | MS(SFL_NOSLAIN), pvis (victim), \
                    LVL_MAX, attacker, victim

#define WIZSLAIN    DEST_ALL, MODE_NSFLAG | MS(SFL_NOSLAIN), \
                    pvis(attacker) > LVL_WIZARD ? pvis(attacker):LVL_WIZARD, \
		    LVL_MAX, attacker, victim

#define ATTACKER 0
#define VICTIM 1
#define OTHER 2

/* Temporary solution: multiply all Armor values by this number */
#define ARMOR_MULT 4

/* Time it takes to crawl around; 10 = 1 sec */
#define CRAWL_SECS 10 

/* this is for a standard fist-hit: */
#define BASE_DAM 8

/* the maximum damage of any weapon: */
#define MAX_DAM 50

#define POTION_VAL 2000

/* chance to hit various parts */

#define HEAD_CHANCE  10
#define RARM_CHANCE  10
#define LARM_CHANCE  10
#define RLEG_CHANCE  10
#define LLEG_CHANCE  10
#define RFOOT_CHANCE 5
#define LFOOT_CHANCE 5
#define RHAND_CHANCE 5
#define LHAND_CHANCE 5
#define CHEST_CHANCE 10
#define BACK_CHANCE  10
#define FACE_CHANCE  5
#define NECK_CHANCE  5

#define NUM_PARTS 13

/* a hit is absorbed by a piece of armor if: */
/* %Effective / %Rolled > Val */

#define ABSORB_VAL 2

/* a hit is also absorbed if the %Rolled is less than: */
/* PLAYERS AVG AC / AC_ABSORB_VAL */

#define AC_ABSORB_VAL 1

/* base chance to hit */

#define CTH_BASE   30

/* The starting number for vitality of players */
#define MAX_PLAYER_VITAL 100

/* Divide the mobile's max strength by this number to see what the */
/* vitality is for each part on the mobiles' body */

#define MOB_VITAL_DIVISOR 4

/* This is the minimum damage a weapon can do to still be able to cause */
/* a mortal hit.. ie, gives the death message */

#define MIN_FATAL_DAMAGE 20

#define MAX_FREE_LEVEL            5
#define COINS_PER_SEVERED_LIMB   20
#define SCORE_PER_SEVERED_LIMB 2000
#define IMM_DAMAGE               25

/* These are defaults for a human character, others may be set */

char *plr_head = "&+Rhead&N";
char *plr_right_arm = "&+mright arm&N";
char *plr_left_arm = "&+mleft arm&N";
char *plr_right_leg = "&+Gright leg&N";
char *plr_left_leg = "&+Gleft leg&N";
char *plr_right_foot = "&+yright foot&N";
char *plr_left_foot = "&+yleft foot&N";
char *plr_right_hand = "&+Bright hand&N";
char *plr_left_hand = "&+Bleft hand&N";
char *plr_chest = "&+Cchest&N";
char *plr_back = "&+gback&N";
char *plr_face = "&+rface&N";
char *plr_neck = "&+Yneck&N";

/* 
   %b ... body part
   %w ... weapon
   %a ... attacker's name
   %e ... attacker's name : posessive
   %g ... gender of attacker : his/her
   %q ... gender of defender : he/she
   %d ... defender's name
   %p ... defender's name : posessive
   %r ... armor or body part
   %s ... add an 's' if attacker
   %S ... same but capital
   %t ... add an 's' if victim
   %T ... same but capital
*/

char *miss[] = {
  "%a swing%s %g %w at %p %b, missing entirely.",
  "%a lunge%s at %p %b but %q sidestep%t the blow.",
  "%d ward%t off a fierce attack!",
  "%d narrowly evade%t a vicous attack!",
  "%d parry%t %e bungled blow.",
  "%d dodge%t a mighty lunge!",
  "%a lunge%s at %d with %g %w, but %q parry%t the blow!",
  "%a run%s at %d with bright &+Rred&N eyes, but %q evade%t the attack.",
  "%a &+Yswing%s&N at %d with %g &+W%w&N, but %q sidestep%t %a!",
  "%a come%s charging at %d with the %w, but fail%s.",
  "%a come%s very close to hitting %d with %g %w.",
  "%a aim%s for %p %b but the attack fails.",
  "%a run%s at %p %b, but the attempt misses.",
  "%a swing%s %g %w at %p %b, but it it misses by an inch!",
  "%a ward%s off %p fierce attack to %r!",
  "%a lunge%s at %d with %g %w but does not contact!",
  "%a ferociously aim%s for %p %b but %g dodges the blow!",
  "%a &+Rgrowl%s&N, and run%s at %p %b, but the shot is deflected!"
};

char *handmiss[] = {
  "%d evade%t %e weak punch.",
  "%a curl%s %g fist, charge%s at %p %b, and trip%s.",
  "%a attempt%s to headbutt %d but fall%s.",
  "%d easily parry%t a pathetic punch.",
  "%d narrowly dodge%t a blow to %p %b.",
  "%d luckily evade%t %e fierce hit to %r!"
};

char *death[] = {
  "%a gutt%s %d with the %w!",
  "%a impale%s %d with %g %w!",
  "%a slice%s %d to bits with the %w! Oh, the humanity!!",
  "%a flay%s, tar%s and feather%s %d!",
  "%a hit%s %d with a fatal blow!"
};

char *hit3[] = {
  "%a &+CTOTALLY MASSACRE%S&N %d with the %w!!!",
  "%a &+CUTTERLY DESTROY%S&N %d with the %w!!!",
  "%a &+CANNIALATE%S&N %d with %g %w!!!",
  "%a &+CBEAT%S&N %d with %g %w!!!",
  "%a &+CCUT%S&N %d up with %g %w!!!",
  "%a &+CTEAR%S&N %d up with a forceful hit!!!"
};

char *hit2[] = {
  "%a deliver%s a good hit to %p %b.",
  "%a hit%s %p %b firmly with the &+Y%w.",
  "%a pierce%s %p %b with an average hit from the &+Y%w.",
  "%a hit%s %p %b hard with %e &+Y%w."
};

char *hit1[] = {
  "%a hit%s %r weakly with the %w.",
  "%a barely hit%s %r with the %w.",
  "%a braise%s %p %b with the %w.",
  "%a hit%s %r very weakly with the %w.",
  "%a run%s at %r with the %w, but barely make%s contact."
};

char *hit0[] = {
   "%a hit%s %d with %g %w, but it is absorbed by %r.",
   "%r absorbs %e blow from the %w.",
   "%a hit%s %d, but it is absorbed by %r."
};

char *handhit3[] = {
  "%a hit%s %d with a powerful uppercut!",
  "%a drive%s forward, hitting %d forcefully with %g hand!",
  "%a deliver%s a mighty blow to %p %b!",
  "%d reel%t as %a jab%s %p %b!",
  "%d stagger%t as %a give%s a brutal punch!"
};

char *handhit2[] = {
  "%a hit%s %d with %g hand.",
  "%a attack%s %p %b with %g hand.",
  "%a give%s %d a mediocre punch to the %b.",
  "%a throw%s an average punch to %p %b.",
  "%a connect%s with a decent cut to %p %b."
};

char *handhit1[] = {
  "%a deliver%s a weak punch to %p %b.",
  "%a barely hit%s %p %b with %g bare hands.",
  "A bungled uppercut by %a barely braises %p %b.",
  "A weak jab by %a barely touches %p %b."
};

char *handhit0[] = {
  "%a hit%s %d, but it's absorbed by %r.",
  "%a punch%s %p %b, but %r absorbs the blow.",
  "%a weakly hit%s %p %b, but there is no effect."
};

extern char *exittxt[];
extern int exitnum[];
