/* Strength Messages for char's and mobs. */

/* For yourself. */

char *smess[] = {
    "You are dead.",
    "***You are near death***",
    "***You are near death***",
    "***You are mortally wounded***",
    "***You are seriously wounded***",
    "You have some wounds, but are still fairly strong.",
    "You have minor cuts and abrasions.",
    "You have minor cuts and abrasions.",
    "You feel a bit dazed.",
    "You are in better than average condition.",
    "You are in exceptional health.",
    "You are in exceptional health.",
    "&+GYou are invincible."
};

char *sarmors[] = {
  "You are unarmored.",
  "Your armor is little more than a fashion statement.",
  "You are moderately well armored.",
  "You are well protected.",
  "You are heavily armored.",
  "You are so well equipped, any more armor would slow you down.",
  "&+GYour skin is made of steel."
};

char *sweapon[] = {
  "You are bare-fisted.",
  "Your weapon intimidates small children and rodents.",
  "You are well armed.",
  "You wield a mighty weapon.",
  "Your offensive power frightens immortals.",
  "&+GYou have infinite power at your fingertips."
};

char *scorpse[] = {
  "You touch the newly-slain body of %s.  It still feels a little warm.",
  "The skin and muscle of %s have become hard to the touch.",
  "The smell of %s is so pungent, you dare not approach.",
  "&+YMaggots from the corpse crawl all over your hands!!!",
  "You find nothing of interest.",
  "You examine the bones of %s, finding nothing."
};


/* Other's messages. */

char *omess[] = {
    "%s is dead.",
    "%s is near death",
    "%s is near death",
    "%s is mortally wounded",
    "%s is seriously wounded",
    "%s has some wounds, but is still fairly strong",
    "%s has minor cuts and abrasions",
    "%s has minor cuts and abrasions",
    "%s seems a bit dazed",
    "%s is in better than average condition",
    "%s is in exceptional health",
    "%s is in exceptional health",
    "&+G%s is invincible"
};

char *oarmors[] = {
  "%s is unarmored",
  "%s's armor is little more than a fashion statement",
  "%s is moderately well armored",
  "%s is well protected",
  "%s is heavily armored",
  "%s is so well equipped, immortals would be jealous",
  "&+G%s's skin is made of steel"
};

char *oweapon[] = {
  "%s is bare-fisted",
  "%s's weapon intimidates small children and rodents",
  "%s is well armed",
  "%s wields a mighty weapon",
  "%s's offensive power frightens immortals",
  "&+G%s has infinite power"
};

char *ocorpse[] = {
  "The newly-slain body of %s is lying here, still warm to the touch.",
  "The pallid corpse of %s is lying here, stiff with rigor mortis.",
  "The decaying corpse of %s is here, and it is beginning to reek.",
  "The carcass of %s is lying here, infested with wriggling maggots.",
  "The skeletal remains of %s lie here, hung with tatters of dry flesh.",
  "The bones of %s are lying here, picked clean by scavengers."
};

char *toggles[] = {                 /* configure options */
  "disable oracle",
  "disable beeps",
  "disable weather",
  "disable blinking",
  "new display style",
  "show exits",
  "quote written text",
  "hide inventories",
  "ignore fights",
  "away from keyboard",
  "i'm busy",
  "brief mode",
  "disable chat",
  "disable anon",
  "color mode",
  "ignore puff"
};

char *opts[] = {
  "see your current travel messages",
  "modify your description",
  "alter your sex (ouch)",
  "change your title",
  "change your password"
};

char *wiztoggles[] = {
  "ignore wishes",
  "see extended cmds",
  "block actions on you",
  "ignore shouts",
  "ignore wiz channel",
  "ignore slain messages",
  "go invis. to intermud",
  "disable intermud chat"
};

char *wizopts[] = {
  "your prompt",
  "your arriving message",
  "your leaving message",
  "your teleport-arrive message",
  "your teleport-leave message",
  "your turn-visible message",
  "your turn-invisible message",
  "your game-entry message",
  "your game-leaving message",
  "your sitting message",
  "your standing message",
  "your summoning message",
  "your summon-person-arrive msg",
  "your summon-person-leave msg"
};

/* not in yet, some ideas for a new combat system */

/* %p is player, %b is body part & %w is weapon %g = gender.. */
/* %p: 50% chance of saying Him/Her */

#ifdef NEW_BATTLE
char *missmsgs[] = {
  "%p swings %g %w at your %b, missing entirely.",
  "%p lunges at your %b but you sidestep the blow!",
  "%p tries to headbutt you, but instead trips and falls on the ground.",
  "You ward off a fierce attack by %p!",
  "You narrowly evade a vicious attack by %p.",
  "You parry a bungled blow by %p! %g looks enraged!",
  "You ward off a mighty lunge by %p.",
  "%p lunges at you with %g %w, but you parry the blow!",
  "%p runs at you, %g eyes a bright &+Rred&N, but you evade %g.",
  "%p swings at you with %g %w, but you sidestep %g!"
};
#endif


