/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

/*
**  just for polymorph spell(s)
*/

struct PolyType {
  char name[20];
  int  level;
  int  number;
};

#define LAST_DRUID_MOB 16
