#include <stdlib.h>
#include "kernel.h"
#include "sendsys.h"
#include "mobiles.h"
#include "puff.h"
#include "mobile.h"

static char *puff_speech[] =
{
  "I &+R*love*&N The Crossroads!",
  "I wonder what Prometheus messed up this time...",
  "Need a &+Ylight&N?  Come a little closer, baby!",
  "*Sniff* I haven't had a hug today...",
  "Lookie at the pretty &+Gc&+Bo&+Wl&+Co&+Rr&+Ms&+w!",
  "Puff on this, *toke*",
  "McCloud is a nice guy!",
  "I'm afraid of bugs.",
  "Humans are overrated.",
  "&#I LOVE BEEPS!&#",
  "I'm tired of wasting all of my precious time. -J.Hendrix",
  "Now I'm Nothing.",
  "Freak OUT!!!",
  "I don't know just where I'm going.  But I'm gonna try for the kingdom if I can!",
  "In a world of fools and liars, Puff is your only true friend!",
  "Pass this around...",
  "Je t'adore!  Viens-ici pour que je puisse te donner une baise!",
  "Listen to the Velvet Underground!!!",
  "I'm really hot!",
  "I actually saw Moses running! Almost had a heart attack!",
  "Beavis kissed me once... EWW!!",
  "I love spam!",
  "I hate this mud.",
  "I think I'm manic depressive.",
  "I heard that Prometheus is a slacker.",
  "Eddie just goosed me! Come back here and do it again, big boy!",
  "The Juice is Loose!",
  "I'm selling a pound of Dutch hashish.  Any takers?",
  "I want to be like all of the different people.",
  "I'm hurt, I'm never invited to weddings.",
  "Hi! I'm Puff! *wiggle*",
  "I had the hiccups the other day! Burned the end of my tail! *growl*",
  "Hey! Go North! Trust me!",
  "Type NOPUFF to turn me off!",
  "Check out HELP MAGIC, there are some neat things in there!",
  "I heard that the Hermit can help you with your problem with Shazareth.",
  "It helps to look around when you are looking for the Pendant of Mana."
};

/* Pick a random player from the MUD */
int
randplr (void)
{
  int r, i, *a = NEW(int, max_players), a_len = 0;

  for (i = 0; i < max_players; i++)
    if (is_in_game (i) && (pvis (i) <= 0)) {
      a[a_len++] = i;
    }
  r = a_len ? a[rand() % a_len] : -1;
  free(a);
  return r;
}

void
puffcom (int mon)
{
  int check, plr;
  char msg1[100], msg2[100];

  check = randperc ();

  /* Heal */
  if (check < 5) {
    if ((plr = randplr ()) == -1)
      return;

    if (pfighting (plr) < 0 && plev (plr) < LVL_WIZARD && psitting(plr) &&
	((pstr (plr) < maxstrength (plr) || pmagic (plr) < maxmagic (plr)) ||
       (pstr (plr) < maxstrength (plr) && pmagic (plr) < maxmagic (plr)))) {

      setpstr (plr, maxstrength (plr));
      setpmagic (plr, maxmagic (plr));

      sendf (plr, "Puff heals all of your wounds.\n");
      send_msg (DEST_ALL, MODE_QUIET, LVL_WIZARD, LVL_MAX, NOBODY, NOBODY,
	   "&+B[&+WPuff &*has &+Chealed &+W\001p%s\003&+B]\n", pname (plr));
    }
  }

  /* Say */
  if (check >= 5 && check < 20) {
    sendl(ploc (mon), "&+GPuff &+wsays, &+W'&+w%s&+W'\n",
	   puff_speech[my_random () % arraysize (puff_speech)]);
    return;
  }

  /* Action */
  if (check >= 80) {

    if ((plr = randplr ()) == -1)
      return;

    if (ststflg (plr, SFL_NOPUFF))
      return;
    if (ploc(plr) != ploc(MOB_START_PUFF + max_players))
      return;

    msg2[0] = '\0';

    switch (rand () % 45) {
    case 0:
      sprintf (msg1, "Puff gives you a round of applause!");
      sprintf (msg2, "Puff applauds %s on %s great performance.",
	       pname (plr), his_or_her (plr));
      break;

    case 1:
      sprintf (msg1, "Puff looks embarrassed, and then turns bright red.");
      break;

    case 2:
      sprintf (msg1, "Puff grins evilly and then bounces on you!");
      sprintf (msg2, "Puff grins evilly and then bounces on %s.",
	       pname (plr));
      break;

    case 3:
      sprintf (msg1, "Puff fetches a bucket of ice water and pours it all over you!");
      sprintf (msg2, "Puff fetches a bucket of ice water and pours it all over %s!",
	       pname (plr));
      break;

    case 4:
      sprintf (msg1, "Puff grabs a chainsaw and looks at you menacingly!");
      sprintf (msg2, "Puff grabs a chainsaw and glares at %s menacingly!",
	       pname (plr));
      break;

    case 5:
      sprintf (msg1, "Puff snaps her fingers and a lighted disco floor appears!");
      break;

    case 6:
      sprintf (msg1, "Puff slips you a hit of acid.");
      sprintf (msg2, "Puff gives something to %s.", pname (plr));
      break;

    case 7:
      sprintf (msg1, "Puff leaps onto a table and screams 'Eeeeeeeek!' at the sight of you.");
      sprintf (msg2, "Puff leaps onto a table and screams 'Eeeeeeek!' at the sight of %s.",
	       pname (plr));
      break;

    case 8:
      sprintf (msg1, "Puff giggles evilly at you.");
      sprintf (msg2, "Puff giggles evilly at %s.", pname (plr));
      break;

    case 9:
      sprintf (msg1, "Puff flexes her muscles, trying to look strong.");
      break;

    case 10:
      sprintf (msg1, "Puff uses the international sign language, showing you her middle finger.");
      sprintf (msg2, "Puff shows %s her middle finger.", pname (plr));
      break;

    case 11:
      sprintf (msg1, "Puff forgives you.");
      sprintf (msg2, "Puff forgives %s.", pname (plr));
      break;

    case 12:
      sprintf (msg1, "Puff kisses you deeply and runs her tongue along the back of your teeth.");
      sprintf (msg2, "Puff kisses %s, running her tongue along the back of %s teeth.",
	       pname (plr), his_or_her (plr));
      break;

    case 13:
      sprintf (msg1, "Puff giggles at your manners.");
      sprintf (msg2, "Puff giggles at %s.", pname (plr));
      break;

    case 14:
      sprintf (msg1, "Puff grins at you evilly!");
      sprintf (msg2, "Puff grins at %s.", pname (plr));
      break;

    case 15:
      sprintf (msg1, "Puff reaches out and begins to grope you.");
      sprintf (msg2, "Puff reaches out towards %s and starts to grope %s.",
	       pname (plr), him_or_her (plr));
      break;

    case 16:
      sprintf (msg1, "Puff has a nasty hangover, please be quiet!");
      break;

    case 17:
      sprintf (msg1, "Puff hugs you close.");
      sprintf (msg2, "Puff hugs %s.", pname (plr));
      break;

    case 18:
      sprintf (msg1, "Puff jumps your bones!");
      sprintf (msg2, "I don't think you want to know what puff did to %s!",
	       pname (plr));
      break;

    case 19:
      sprintf (msg1, "Puff climbs into your lap and smiles at you.");
      sprintf (msg2, "Puff climbs into %s's lap and smiles at %s.",
	       pname (plr), him_or_her (plr));
      break;

    case 20:
      sprintf (msg1, "Puff laughs at you!");
      sprintf (msg2, "Puff laughs at %s.", pname (plr));
      break;

    case 21:
      sprintf (msg1, "Puff licks you.");
      sprintf (msg2, "Puff licks %s.", pname (plr));
      break;

    case 22:
      sprintf (msg1, "Puff looks deeply into your eyes.");
      sprintf (msg2, "Puff looks deeply into %s's eyes.", pname (plr));
      break;

    case 23:
      sprintf (msg1, "Puff points at you and yells 'LOSER!'");
      sprintf (msg2, "Puff points at %s and yells 'LOSER!'", pname (plr));
      break;

    case 24:
      sprintf (msg1, "Puff opens her mouth and moans at you.");
      sprintf (msg2, "Puff opens her mouth and moans at %s.", pname (plr));
      break;

    case 25:
      sprintf (msg1, "Puff says 'brb... nature run'");
      break;

    case 26:
      sprintf (msg1, "Puff nuzzles your neck softly.");
      sprintf (msg2, "Puff nuzzles %s's neck softly.", pname (plr));
      break;

    case 27:
      sprintf (msg1, "Puff pounces on you merrily and knocks you to the floor.");
      sprintf (msg2, "Puff pounces on %s merrily and knocks %s to the floor.",
	       pname (plr), him_or_her (plr));
      break;

    case 28:
      sprintf (msg1, "Puff quacks at you playfully.");
      sprintf (msg2, "Puff quacks at %s playfully.", pname (plr));
      break;

    case 29:
      sprintf (msg1, "Puff gives you the ultimite in snuggles, cuddles, and wuv.");
      sprintf (msg2, "Puff gives %s the ultimite in snuggles, cuddles, and wuv.",
	       pname (plr));

    case 30:
      sprintf (msg1, "Puff snickers at you.");
      sprintf (msg2, "Puff snickers at %s.", pname (plr));
      break;

    case 31:
      sprintf (msg1, "Puff pulls out her Super-Soaker XP150 and then sprays you!");
      sprintf (msg2, "Puff just hosed down %s with her SuperSoaker!",
	       pname (plr));
      break;

    case 32:
      sprintf (msg1, "Puff sobs quietly.");
      break;

    case 33:
      sprintf (msg1, "Puff puts you over her knee and gives you a sound spanking!");
      sprintf (msg2, "Puff gives %s sound spanking!", pname (plr));
      break;

    case 34:
      sprintf (msg1, "Puff tickles you.");
      sprintf (msg2, "Puff tickles %s.", pname (plr));
      break;

    case 35:
      sprintf (msg1, "Puff thinks that you need a valium.");
      sprintf (msg2, "Puff thinks that %s should take a valium.", pname (plr));
      break;

    case 36:
      sprintf (msg1, "Puff bites the soft part of your neck!");
      sprintf (msg2, "Puff bites the soft part of %s's neck!", pname (plr));
      break;

    case 37:
      sprintf (msg1, "Puff pours you a double shot of vodka.");
      sprintf (msg2, "Puff pours %s a double shot of vodka.", pname (plr));
      break;

    case 38:
      sprintf (msg1, "Puff takes a magic marker and writes 'Wanker' on your forehead.");
      sprintf (msg2, "Puff takes a magic marker and writes 'Wanker' on the forehead of %s",
	       pname (plr));
      break;

    case 39:
      sprintf (msg1, "Puff sprays whipcream on you, then licks it off slowly.");
      sprintf (msg2, "Puff sprays whipcream on %s, then licks it off slowly.",
	       pname (plr));
      break;

    case 40:
      sprintf (msg1, "Puff points at you and screams 'WEENIE!' at the top of her lungs.");
      sprintf (msg2, "You watch as Puff points at %s and screams 'WEENIE!'.",
	       pname (plr));
      break;

    case 41:
      sprintf (msg1, "Puff looking confused holds out her hands, looks up, and says, 'Why me?'");
      break;

    case 42:
      sprintf (msg1, "Puff wiggles her bottom.");
      break;

    case 43:
      sprintf (msg1, "Puff winks at you playfully.");
      sprintf (msg2, "Puff winks at %s.", pname (plr));
      break;

    case 44:
      sprintf (msg1, "Puff gives you a playful zerbert, right on the tummy!");
      sprintf (msg2, "Puff gives %s a playful zerbert, right on the tummy!",
	       pname (plr));
      break;
    }

    sendf (plr, "%s\n", msg1);
    if (msg2[0] != '\0')
      send_msg (DEST_ALL, MODE_NSFLAG | MS (SFL_NOPUFF), LVL_MIN, LVL_MAX,
		plr, NOBODY, "%s\n", msg2);
    return;
  }
}
