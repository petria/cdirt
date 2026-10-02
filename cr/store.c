#include "kernel.h"
#include "locations.h"
#include "store.h"
#include "objects.h"
#include "cflags.h"
#include "objsys.h"
#include "bprintf.h"
#include "parse.h"
#include "clone.h"
#include <string.h>

#define citem(X) clone_object(X, -1, NULL, mynum, CARRIED_BY)

#ifdef LOCMIN_SYSTEM
void buyfood(void) {
  if (pcoins(mynum) < FOOD_COST) {
    bprintf("Sorry, you don't have enough money.  Food costs %d coin%s\n",
    FOOD_COST, (FOOD_COST > 1) ? "s" : "");
    return;
  }
  else {
    citem(OBJ_SYSTEM_MUSH);
    bprintf("You now have a pint of fine grade mush.\n");
    pcoins(mynum) -= FOOD_COST;
  }
}

void buytorch(void) {
  if (pcoins(mynum) < TORCH_COST) {
    bprintf("Sorry, you don't have enough money.  Torches costs %d coin%s\n",
       TORCH_COST, (TORCH_COST > 1) ? "s" : "");
    return;
  }
  else {
    citem(OBJ_SYSTEM_TORCH);
    bprintf("You now have a torch.\n");
    pcoins(mynum) -= TORCH_COST;
  }
}

void buykey(void) {
  if (pcoins(mynum) < KEY_COST) {
    bprintf("Sorry, you don't have enough money.  Keys costs %d coin%s\n",
            KEY_COST, (KEY_COST > 1) ? "s" : "");
    return;
  }
  else {
    citem(OBJ_SYSTEM_KEY);
    bprintf("You now have a key.\n");
    pcoins(mynum) -= KEY_COST;
  }
}

void buystorage(void) {
  int a, cost, i;

  if (!*txt2) {
    bprintf("Store what exactly?\n");
    return;
  }
  else if ((a = ob1) == -1) {
    bprintf("What's that?\n");
    return;
  }
  else if (oloc(a) != mynum) {
    bprintf("You wish you had a %s to store!\n", oname(a));
    return;
  }
  else {
    cost = oarmor(a);
    if (!cost)
     cost = odamage(a);
    if (!cost) {
      bprintf("Why would you want to store the %s?", oname(a));
      return;
    }
    if (cost > pcoins(mynum)) {
      bprintf("It costs %d coins to store the %s.\n", cost, oname(a));
      return;
    }
    else {
      for (i = 0 ; i < NUM_STORE_SLOTS ; i++)
	if (rplrs[mynum].storage[i] == -1) {
	  rplrs[mynum].storage[i] = a;
	  break;
	}
      if (i == NUM_STORE_SLOTS) {
	bprintf("You may only store %d items.\n", NUM_STORE_SLOTS);
	return;
      }
      bprintf("You hand the %s to the store owner.\n", oname(a));
      destroy(a);
    }
  }
}
#endif

void buycom(void) {

#ifndef LOCMIN_SYSTEM
  bprintf("The store isn't implemented on this game, sorry\n");
  return;
#else
  if (ploc(mynum) != LOC_BLIZZARD_STORE) {
    bprintf("You aren't in a store, silly.\n");
    return;
  }

  if (!*item1) {
    bprintf("Buy what exactly?\n");
    return;
  }
  else if (EQ(item1, "food"))
    buyfood();
  else if (EQ(item1, "torch"))
    buytorch();
  else if (EQ(item1, "key"))
    buykey();
  else if (EQ(item1, "storage"))
    buystorage();
  else
    bprintf("You can't buy that.\n");
#endif
}
