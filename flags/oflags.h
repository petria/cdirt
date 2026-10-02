#define OFL_DESTROYED	0		/* destroyed */
#define OFL_NOGET	1		/* Flannel */
#define OFL_OPENABLE	2		/* can be opened (1=closed, 0=open */
#define OFL_LOCKABLE	3		/* can be locked (state 2 = locked) */
#define OFL_PUSHABLE	4		/* set state to 0 when pushed */
#define OFL_PUSHTOGGLE	5		/* toggle state when pushed */
#define OFL_FOOD	6		/* can be eaten */
#define OFL_ARMOR	7		/* is armor */
#define OFL_WEARABLE	8		/* can be worn */
#define OFL_LIGHTABLE	9		/* can light */
#define OFL_EXTINGUISH	10		/* can extinguish */
#define OFL_KEY		11		/* is a key */
#define OFL_GETFLIPS	12		/* change to state 0 when taken */
#define OFL_LIT		13		/* is lit */
#define OFL_CONTAINER	14		/* is a container */
#define OFL_WEAPON	15		/* is a weapon */
#define OFL_REGENHEALTH 16              /* Regenerates Health Faster */
#define OFL_REGENMANA   17              /* Regenerates Mana Faster */
#define OFL_FIREBALL    18             /* Helps with the fireball spell */
#define OFL_MISSILE     19             /* Helps with the missile spell  */
#define OFL_FROST       20             /* Helps with the frost spell  */
#define OFL_SHOCK       21             /* Helps with the shock spell  */
#define OFL_AID         22             /* Helps with the aid spell */
#define OFL_VTOUCH      23             /* Helps with the vtouch spell */
#define OFL_LIGHT       24             /* Helps with the light spell */
#define OFL_DAMAGE      25             /* Helps with the damage spell */
#define OFL_ARMORSPELL  26             /* Helps with the armor spell  */
#define OFL_BHANDS      27             /* Helps with the bhands spell */
#define OFL_BLUR        28             /* Helps with the blur spell */
#define OFL_ICESTORM    29             /* Helps with the icestorm spell */
#define OFL_ENCHANTED   30             /* Exact effect of enchanted/cursed  */
#define OFL_CURSED      31             /* depends on other flags.           */
#define OFL_UNLOCATABLE 32             /* The object CAN'T be found by where*/
#define OFL_ALCOHOL     33             /* Makes you drunk. should be FOOD 2 */
#define OFL_NEWBIE	34	       /* Newbie object.  level 3 or under */
#define OFL_POTION      35             /* Object is a potion */
#define OFL_SHIELD      36             /* Shields are a special object type */ 
#define OFL_BOAT        37             /* Can use on water (BOAT type) */
#define OFL_MAX         38
