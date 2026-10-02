#ifndef _EFLAGNAMES_H
#define _EFLAGNAMES_H

#define EFL_MUTE        34
#define EFL_CRIPPLE     35
#define EFL_BLIND       36
#define EFL_DEAF        37
#define EFL_WHERE       38


char *Eflags[] = {
	"Fireball",	"FearFireball",	"ImmFireball",	"Missile",
	"FearMissile",	"ImmMissile",	"Frost",	"FearFrost",
	"ImmFrost",	"Shock",	"FearShock",	"ImmShock",
	"Aid",		"VTouch",	"ImmVTouch",	"Light",
	"Damage",	"Protect",	"BHands",	"FearBHands",
	"ImmBHands",	"Blur",		"IceStorm",	"FearIceStorm",
	"ImmIceStorm",	"NegFireball",  "NegMissile",   "NegFrost",
        "NegShock",     "NegBHands",    "NegVTouch", 	"NegIceStorm",
	"Babel",	"MakePig",	"Mute",		"Cripple",
	"Blind",	"Deaf",		"Where",	"ImmMute",
	"ImmCripple",	"ImmBlind",	"ImmDeaf",
	TABLE_END
};

#endif
