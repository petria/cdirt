#ifndef _EXITNAMES_H
#define _EXITNAMES_H

char *Exits[] = {
  "North", "East",
  "South", "West",
  "Up",    "Down",
  TABLE_END
};

char *exits[] = {
  "north", "east",
  "south", "west",
  "up",    "down",
  NULL
};

char *exittxt[] = {
  "the &+Cnorth&N", "the &+Ceast&N",
  "the &+Csouth&N", "the &+Cwest&N",
  "&+Clands above&N", "&+Clands below&N",
  TABLE_END
};

char *entertxt[] = {
  "the &+Csouth&N", "the &+Cwest&N",
  "the &+Cnorth&N", "the &+Ceast&N",
  "&+Cbelow&N",   "&+Cabove&N",
  TABLE_END
};

char *mobenter[] = { 
  "has arrived from the south", "walks in from the west",
  "enters from the north", "comes in from the east",
  "wanders in from below", "enters from above",
   TABLE_END
};

#endif
