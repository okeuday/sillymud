/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/


/*
**  0 = Mage, 1 = cleric, 3 = thief, 2 = fighter
*/

/*
**  
*/

#include <stdio.h>
#include <string.h>

#include "protos.h"


/* extern variables */

extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct room_data *world;
extern struct dex_app_type dex_app[];



int GetClassLevel(struct char_data *ch, int class)
{

  if (IS_SET(ch->player.class, class)) {
    return(GET_LEVEL(ch, CountBits(class)-1));
  }
  return(0);
}

int CountBits(int class)
{

  if (class == 1) return(1);
  if (class == 2) return(2);
  if (class == 4) return(3);
  if (class == 8) return(4);
  if (class == 16) return(5);
  if (class == 32) return(6);

}

int OnlyClass( struct char_data *ch, int class)
{
  int i;

  for (i=1;i<=32; i*=2) {
    if (GetClassLevel(ch, i) != 0)
      if (i != class)
	return(FALSE);
  }
  return(TRUE);

}


int HasClass(struct char_data *ch, int class)
{

  if (!IS_PC(ch)) {
    if (!IS_SET(class, CLASS_MONK)) {
      return(TRUE);
    }
  }

  if (IS_SET(ch->player.class, class))
     return(TRUE);

  return FALSE;
}

int HowManyClasses(struct char_data *ch)
{
  short i, tot=0;

  for (i=0;i<MAX_CLASS;i++) {
    if (GET_LEVEL(ch, i)) {
      tot++;
    }
  }
  if (tot) 
    return(tot);
  else {
    if (IS_SET(ch->player.class, CLASS_MAGIC_USER)) 
      tot++;

    if (IS_SET(ch->player.class, CLASS_WARRIOR)) 
      tot++;
      
    if (IS_SET(ch->player.class, CLASS_THIEF))
      tot++;

    if (IS_SET(ch->player.class, CLASS_CLERIC))
      tot++;

    if (IS_SET(ch->player.class, CLASS_DRUID))
      tot++;

    if (IS_SET(ch->player.class, CLASS_MONK))
      tot++;

  }
  return(tot);
}


int BestFightingClass(struct char_data *ch)
{

 if (GET_LEVEL(ch, WARRIOR_LEVEL_IND)) 
   return(WARRIOR_LEVEL_IND);
 if (GET_LEVEL(ch, CLERIC_LEVEL_IND))
   return(CLERIC_LEVEL_IND);
 if (GET_LEVEL(ch, DRUID_LEVEL_IND))
   return(DRUID_LEVEL_IND);
 if (GET_LEVEL(ch, MONK_LEVEL_IND)) 
   return(MONK_LEVEL_IND);
 if (GET_LEVEL(ch, THIEF_LEVEL_IND)) 
   return(THIEF_LEVEL_IND);
 if (GET_LEVEL(ch, MAGE_LEVEL_IND)) 
   return(MAGE_LEVEL_IND);
 
  logE("Massive error.. character has no recognized class.");
  logE(GET_NAME(ch));
  assert(0);

  return(1);
}

int BestThiefClass(struct char_data *ch)
{

 if (GET_LEVEL(ch, THIEF_LEVEL_IND)) 
   return(THIEF_LEVEL_IND);
 if (GET_LEVEL(ch, MONK_LEVEL_IND)) 
   return(MONK_LEVEL_IND);
 if (GET_LEVEL(ch, MAGE_LEVEL_IND)) 
   return(MAGE_LEVEL_IND);
 if (GET_LEVEL(ch, WARRIOR_LEVEL_IND)) 
   return(WARRIOR_LEVEL_IND);
 if (GET_LEVEL(ch, DRUID_LEVEL_IND)) 
   return(DRUID_LEVEL_IND);
 if (GET_LEVEL(ch, CLERIC_LEVEL_IND)) 
   return(CLERIC_LEVEL_IND);
 
  logE("Massive error.. character has no recognized class.");
  logE(GET_NAME(ch));
  assert(0);

  return(1);
}

int BestMagicClass(struct char_data *ch)
{

 if (GET_LEVEL(ch, MAGE_LEVEL_IND)) 
   return(MAGE_LEVEL_IND);
 if (GET_LEVEL(ch, DRUID_LEVEL_IND)) 
   return(DRUID_LEVEL_IND);
 if (GET_LEVEL(ch, CLERIC_LEVEL_IND)) 
   return(CLERIC_LEVEL_IND);
 if (GET_LEVEL(ch, THIEF_LEVEL_IND)) 
   return(THIEF_LEVEL_IND);
 if (GET_LEVEL(ch, WARRIOR_LEVEL_IND)) 
   return(WARRIOR_LEVEL_IND);
 if (GET_LEVEL(ch, MONK_LEVEL_IND)) 
   return(MONK_LEVEL_IND);
 
  logE("Massive error.. character has no recognized class.");
  logE(GET_NAME(ch));
  ch->player.class = 4;

  return(1);
}

int GetSecMaxLev(struct char_data *ch)
{
   return(GetALevel(ch, 2));
}

int GetALevel(struct char_data *ch, int which)
{
  byte ind[MAX_CLASS],j,k,i;

  for (i=0; i< MAX_CLASS; i++) {
    ind[i] = GET_LEVEL(ch,i);
  }

/*
 *  chintzy sort. (just to prove that I did learn something in college)
 */

  for (i = 0; i< MAX_CLASS-1; i++) {
    for (j=i+1;j< MAX_CLASS;j++) {
      if (ind[j] > ind[i]) {
	k = ind[i];
        ind[i] = ind[j];
        ind[j] = k;
      }
    }
  }

  if (which > -1 && which < 4) {
    return(ind[which]);
  }
}

int GetThirdMaxLev(struct char_data *ch)
{
   return(GetALevel(ch, 3));
}

int GetMaxLevel(struct char_data *ch)
{
  register int max=0, i;

  for (i=0; i<= MAX_CLASS; i++) {
    if (GET_LEVEL(ch, i) > max)
      max = GET_LEVEL(ch,i);
  }

  return(max);
}

int GetTotLevel(struct char_data *ch)
{
  int max=0, i;

  for (i=0; i< MAX_CLASS; i++)
      max += GET_LEVEL(ch,i);

  return(max);

}

void StartLevels(struct char_data *ch)
{

  if (IS_SET(ch->player.class, CLASS_MAGIC_USER)) {
    advance_level(ch, MAGE_LEVEL_IND);
  }
  if (IS_SET(ch->player.class, CLASS_CLERIC)) {
    advance_level(ch, CLERIC_LEVEL_IND);
  }
  if (IS_SET(ch->player.class, CLASS_WARRIOR)) {
    advance_level(ch, WARRIOR_LEVEL_IND);
  }
  if (IS_SET(ch->player.class, CLASS_THIEF)) {
    advance_level(ch, THIEF_LEVEL_IND);
  }
  if (IS_SET(ch->player.class, CLASS_DRUID)) {
    advance_level(ch,DRUID_LEVEL_IND);
  }
  if (IS_SET(ch->player.class, CLASS_MONK)) {
    advance_level(ch, MONK_LEVEL_IND);
  }
  
  if(GET_RACE(ch) == RACE_OGRE)
    ch->specials.spells_to_learn = 2;
  else
    ch->specials.spells_to_learn = 4; /* we are generous. */

}


int BestClass(struct char_data *ch)
{
  int max=0, class=0, i;

  for (i=0; i< MAX_CLASS; i++)
    if (max < GET_LEVEL(ch,i)) {
      max = GET_LEVEL(ch, i);
      class = i;
    }

  class = 1 << class;  /* perhaps this will work properly */

  assert(max > 0);
  return(class);

}
