
One file contains all the mobiles, objects, and locations for a zone.
the different categories are separated by a % and the type, such as

% locations

The traditional format is mobiles first, then objects, then locations.
I'll write the format that way, with a breakdown of each description
format in each section.

-----

% mobiles

Name             = <one unique word to identify the mobile for this zone>
PName            = "<the mobile's actual name, as many words as you like>"
Pflags           { <pflags, if any, by name> } 
Mflags           { <mflags, if any, by name> } 
Sflags           { <sflags, if any, by name> } 
Location         = <the name of the room it starts in>
Strength         = <its starting strength>
Damage           = <how much damage it does>
Armor            = <its natural armor class>
Aggression       = <percent chance (out of 100) to attack>
Speed            = <how quickly it wanders around (0 means it doesn't)>
Description      = "<its description>"
End              = <the same name you gave it at the top>

ex:

Name             = bensozia
PName            = "Bensozia"
Sflags           { Female }
Location         = waste70
Strength         = 300
Damage           = 10
Armor            = 0
Aggression       = 20
Speed            = 0
Description      = "The naked devil Bensozia is lying on a red velvet couch, waiting for Asmodeus."
End              = bensozia

Mobiles are fairly straightforward.

-----
% objects

Name             = <one unique word to identify the mobile for this zone>
PName            = <the object's primary name>
AltName          = <the object's secondary name, if it has one>
Oflags           { <oflags, if any, by name> }
Location         = <starting location macro>:<name of starting location>
Linked           = <the name of the object it's linked to, if any>
BValue           = <the base value of the object, if it's not zero>
Size             = <the size of the object, if it's not zero>
Weight           = <the weight of the object, if it's not zero>
Armor            = <the armor value of the object, if it's not zero>
Damage           = <the damage value of the object, if it's not zero>
State            = <the starting state of the object, if it's not zero>
MaxState         = <the max number of states, up to three, if it's not zero>
Desc[0]          = "<room description for state zero>"
Desc[1]          = "<room description for state one, if it can be>"
Desc[2]          = "<room description for state two, if it can be>"
Desc[3]          = "<room description for state three, if it can be>"
Examine          = "<the text seen on an examine of the object, if any>"
End              = <the same name you gave it at the top>

The starting location is tricky.  The macros are:
IN_ROOM - starts in the room given by the name following
IN_CONTAINER - starts out in the object given by the name following
CARRIED_BY - starts carried by the mobile given by the name following
WORN_BY - starts out worn by the mobile given by the name following
WIELDED_BY - starts out wielded by the mobile given by the name following

ex1:

Name             = gold_staff
PName            = staff
AltName          = gold
Oflags           { Lit }
Location         = CARRIED_BY:beast
BValue           = 200
Size             = 10
Weight           = 10
Desc[0]          = "A golden staff shines before you with a hellish red light."
End              = gold_staff

A note about states:
 There are four states, but usually only up to three are ever used.
 State zero indicates the object is 'open'.
 State one indicates the object is 'closed'.
 State two indicates the object is 'locked'.
 If an object is Openable, 'open' will change the object's state to 0 and
  'close' will change the object's state to 1.
 If an object is Lockable, 'unlock' will change the object's state to 1 and
  'lock' will change the object's state to 2.
 If an object is Pushable, 'push' will change the object's state to 0, and
  if it's PushToggle, 'push' will toggle the object's state back and forth
  between 0 and 1.

I'll go over linking after I've detailed locations.

----
% locations

<one unique word> [<direction>:<name of destination>]*
lflags { <lflags on the room (empty if none)> }
<name of room>^
<description of room>
^

The first line needs some explaining.  I'll do it in the example.

ex:

waste1 n:waste6 e:waste2 s:waste7 w:waste7;
lflags { }
Before The Gates^
   You are standing on a vast windswept plain, formed entirely of black
volcanic dust.  The dark gloomy wastes extend as far as your eye can follow,
reflecting strangely against a fiery red sky...
   The only landmark in sight is a huge golden citadel whose walls reflect the
flames and blood-red glow of the hellish skies above.
^

waste1 is the unique name given to this room for this zone.
n:waste6 indicates the north exit from the room leads to waste6.
e:waste2     "      "  east   "     "   "    "    "    " waste2.
etc.

The lflags line must be there, just leave it empty if there aren't any flags.

The same rule above goes for putting a newline before the ^ at the end.

-----
Linking

When two objects are 'linked', their states are maintained to be the same.
That is, when the state of one of the linked objects changes, the state of
the other is changed to match the new state of its partner.  About the only
special cases you can code into zonefiles are those based on manipulating
linked objects.  Suppose you have two rooms, room1 and room2:

%locations

room1  e:room2;
lflags { }
The Blue Room^
   This room is blue.
^
room2  w:room1;
lflags { }
The Green Room^
   This room is green.
^

Suppose you want to have a door between the rooms.  Put a NoGet, Openable,
and Locable object in each room and link them to each other:

%objects

Name             = door1
PName            = door
Location         = IN_ROOM:room1
Oflags           { NoGet Openable Lockable }
Linked           = door2
MaxState         = 2
State            = 2
Desc[0]          = "The door is open."
Desc[1]          = "The door is closed."
Desc[2]          = "The door is locked."
End              = door1

Name             = door2
PName            = door
Location         = IN_ROOM:room2
Oflags           { NoGet Openable Lockable }
Linked           = door1
MaxState         = 2
State            = 2
Desc[0]          = "The door is open."
Desc[1]          = "The door is closed."
Desc[2]          = "The door is locked."
End              = door2

Now rewrite the rooms like this:

%locations

room1  e:^door1;
lflags { }
The Blue Room^
   This room is blue.
^
room2  w:^door2;
lflags { }
The Green Room^
   This room is green.
^

An exit destination preceeded by a '^' means that exit is 'linked' to the
state of the object following the ^ - this is a slightly different use of
the term 'link', but since they're always used in tandem ths difference is
academic.  What this means is the exit will be 'open' if the state of the
objects is zero and 'closed' if the state of the objects is nonzero.  The
exit will show up on 'exits' when it's open and it won't when it's closed.
Furthermore, the way the game determines where you will go when you walk
through an open linked exit is by finding the location of the object linked
to the object linked to that exit.  i.e. since the east exit of room1 is
linked to door1 which is linked to door2, when the state of door1 is 0 and
you walk east in room1, the game will deposit you at the location of door2,
room2 in the initial setup.  This last phrase is necessary since if door2
is somehow moved to a room other than room2, going east in room1 when the
exit is open will take you to that new location instead.

Reread the above paragraph until you understand it.  It's important.  Also
note that the objects don't have to be openable or lockable, they could be
pushable instead, or one could be openable and the other pushable..  They
don't even have to be able to be manipulated with the basic state-changing
commands, you could have a special case where if you examine one of the
objects its state is changed to 0 (and the other as well automagically).
This should go on your list of special cases to be implemented within the
code of whatever mud your zone will be installed on.

-----

That's basically it.  A note, if any quoted (") description contains a quote,
you either need to change it to a ' quote or change the "'s around it to '
or even ^ if the description contains " and '.

ex:

PName        = "John "the Tiger" Smith"
 should be
PName        = 'John "the Tiger" Smith'
 or
PName        = "John 'the Tiger' Smith"
 if you don't mind having 's instead of "s

ex2:

Examine      = "The clock has a 'sign' that says "stop looking at me and get a watch."
"
 should be
Examine      = ^The clock has a 'sign' that says "stop looking at me and get a watch."
^
 or some other sacrifice must be made in the use of quotes.

If you manage to use ", ', and ^ within a quote, it won't work.

-----

Miscellaneous:

All through this we've been assuming all the locations of mobiles and objects
and all the exits for locations are in the zone being detailed.  If you want
to refer to a different zone you can simply attach a '@<zonename> after the
name of whatever you're referencing.  For example, an exit leading to the
temple of paradise (start1) would look like "e:temple@start".  If you don't
know the symbolic name of a room or object you need to reference, either
ask the gods of the mud you're designing it for or include a note somewhere
explaining the mobile, object, or location is in a zone such-and-such.

And speaking of making notes, you can make comments directly in the zonefile
using the C-style comment delimiters "/*" and "*/".  This is ideal for making
notes about how special cases that can't be implemented solely through the
use of states and basic state manipulation commands (open, push, etc) are
implemented.  An example may make this clearer, and I'll also make an example
of the previous note about referencing things in other zones:

%objects

Name             = hairpin
Location         = church@start
Desc[0]          = "A hairpin lies here."
Examine          = "It looks like just the right size to pick a lock."
End              = hairpin
/* This hairpin is used to pick the lock of the safe in the secret room.
   If someone is holding this object and does an 'open safe' in that room
   they should see the text: "You pick the lock and crack the safe." */

-----

Flags:

On mobiles:

  Pflags - NoExorcise, NoSnoop, NoHassle, NoAlias, NoZap, NoSummon,
           NoTrace, NoSteal, NoMagic, NoForce
    (not sure exactly how many of these are actually checked with mobiles)
  Mflags - CanFireball, CanMissile, CanFrost, CanShock, NegFireball,
           NegMissile, NegFrost, NegShock, NoHeat, Thief, StealWeapon,
           StealWorn, PitIt, DrainLev, DrainScr, BarNorth, BarEast,
           BarSouth, BarWest, BarUp, BarDown, QFood, Blind, Deaf, Dumb,
           NoGrab, GrabHostile, PickStuff, NoSteal, Block, Cross, Dead
  Sflags - Female, Possessed, Aloof
    (generally the only trouble a designer goes
     to is to set the sex out of all these flags)

On objects:

  Oflags - Destroyed, NoGet, Openable, Lockable, Pushable, PushToggle,
           Food, Armor, Wearable, Lightable, Extinguish, Key, GetFlips,
           Lit, Container, Weapon

On locations:

  Lflags - Light, Dark, Real, TempOrdinary, Hot, Cold, TempReal, Death,
           CantSummon, NoSummon, NoQuit, NoSnoop, NoMobiles, NoMagic,
           Peaceful, Soundproof, OnePerson, Party, Private, OnWater,
           Underwater, Outdoors, TempExtreme, NegRegen


These are basic flags that come with most dirts, but you should check with
whatever mud you're writing the zone for to get their particular list of
flags, what they do, and how many of them are actually implemented (often
many aren't).


Grant Culbertson         dgray@prism.nmt.edu
AKA South                      - or -
AKA Hastur               grant@sphinx.nmt.edu

(last modified 3/1/94)
