/*-*-Mode:C;coding:utf-8;tab-width:8;c-basic-offset:2;indent-tabs-mode:()-*-
 * ex: set ft=cpp fenc=utf-8 sts=2 ts=8 sw=2 et:

  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "protos.h"

struct room_data *real_roomp(int);

extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct title_type titles[MAX_CLASS][ABS_MAX_LVL];
extern struct room_data *world;
extern const char *RaceName[];
extern int RacialMax[][MAX_CLASS];
extern struct wis_app_type wis_app[];
extern struct con_app_type con_app[];
extern struct weather_data weather_info;
extern struct time_info_data time_info;


static char ClassTitlesBuf[256];
char *ClassTitles(struct char_data *ch)
{
  unsigned char i, count=0;
  char * buf = ClassTitlesBuf;
  
    for (i = MAGE_LEVEL_IND; i <= MONK_LEVEL_IND; i++) {
      if (GET_LEVEL(ch, i)) {
	count++;
	if (count > 1) {
	  sprintf(buf + strlen(buf), "/%s",GET_CLASS_TITLE(ch, i,GET_LEVEL(ch,i)));
	} else {
	  sprintf(buf, "%s", GET_CLASS_TITLE(ch, i, GET_LEVEL(ch, i)));
	}
      }
    }
  return(buf);
}


/* When age < 15 return the value p0 */
/* When age in 15..29 calculate the line between p1 & p2 */
/* When age in 30..44 calculate the line between p2 & p3 */
/* When age in 45..59 calculate the line between p3 & p4 */
/* When age in 60..79 calculate the line between p4 & p5 */
/* When age >= 80 return the value p6 */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{
  
  if (age < 15)
    return(p0);                               /* < 15   */
  else if (age <= 29) 
    return (int) (p1+(((age-15)*(p2-p1))/15));  /* 15..29 */
  else if (age <= 44)
    return (int) (p2+(((age-30)*(p3-p2))/15));  /* 30..44 */
  else if (age <= 59)
    return (int) (p3+(((age-45)*(p4-p3))/15));  /* 45..59 */
  else if (age <= 79)
    return (int) (p4+(((age-60)*(p5-p4))/20));  /* 60..79 */
  else
    return(p6);                               /* >= 80 */
}


/* The three MAX functions define a characters Effective maximum */
/* Which is NOT the same as the ch->points.max_xxxx !!!          */
int mana_limit(struct char_data *ch)
{
  int max, tmp;
  
  max = 0;

  if (IS_NPC(ch)) return(100);

  if (HasClass(ch, CLASS_MAGIC_USER)) {
    max += 100;
    max += GET_LEVEL(ch, MAGE_LEVEL_IND) * 5;
  }

  if (HasClass(ch, CLASS_CLERIC)) {
    max += 100;
    max += (GET_LEVEL(ch, CLERIC_LEVEL_IND)/3) * 5;
  }
  if (HasClass(ch, CLASS_DRUID)) {
    max += 100;
    max += (GET_LEVEL(ch, DRUID_LEVEL_IND)/3) * 5;
  }  
  if (HasClass(ch, CLASS_THIEF)) {
    max += 100;
  }
  if (HasClass(ch, CLASS_WARRIOR)) {
    max += 100;
  }
  if (HasClass(ch, CLASS_MONK)) {
    max += 100;
  }

  max /= HowManyClasses(ch);

/*
  new classes should be inserted here.
*/

  tmp = GET_INT(ch)/3;
  tmp += 2;
  tmp = tmp*3;

  max += tmp;
  max += ch->points.max_mana;   /* bonus mana */
  
  return(max);
}


int hit_limit(struct char_data *ch)
{
  int max;
  
  if (!IS_NPC(ch)) {
      struct time_info_data ma;
      age2(ch, &ma);
      max = (ch->points.max_hit);
	(graf(ma.year, 2,4,17,14,8,0,-10));
  } else 
    max = ch->points.max_hit;
  
  /* Class/Level calculations */
  
  /* Skill/Spell calculations */
  
  return (max);
}


int move_limit(struct char_data *ch)
{
  int max;
  
  if (!IS_NPC(ch)) {
    max = 100 + GET_CON(ch);
  } else {
    max = ch->points.max_move;
  }

  if (IsRideable(ch))
    max *= 2;

  
  if (GET_RACE(ch) == RACE_DWARF || GET_RACE(ch) == RACE_GNOME)
    max -= 20;
  else if (GET_RACE(ch) == RACE_ELVEN)
    max += 20;
  else if (GET_RACE(ch) == RACE_OGRE)
    max += 35;
  else if (GET_RACE(ch) == RACE_HALFLING)
    max -= 25;
  else if (GET_RACE(ch) == RACE_VEGMAN)
    max -= 30;
  
  max += ch->points.max_move;  /* move bonus */
  
  return (max);
}


/* manapoint gain pr. game hour */
int mana_gain(struct char_data *ch)
{
  int gain;
  
  if((IS_NPC(ch)) && (!IS_SET(ch->specials.act, ACT_POLYSELF))) {
    /* Neat and fast */
    gain = 8;
  } else {
    struct time_info_data ma;
    age2(ch, &ma);
    gain = graf(ma.year, 2,4,6,8,10,16,2);
  }    
    /* Class calculations */
    
    /* Skill/Spell calculations */
    
    /* Position calculations    */
    switch (GET_POS(ch)) {
    case POSITION_SLEEPING:
      gain += gain;
      break;
    case POSITION_RESTING:
      gain+= (gain>>1);  /* Divide by 2 */
      break;
    case POSITION_SITTING:
      gain += (gain>>2); /* Divide by 4 */
      break;
    }

    gain += gain;

  if(GET_RACE(ch) == RACE_VEGMAN) {
    gain += veggie_gain_mods(ch, gain, 0);
  }
    
  gain += wis_app[GET_WIS(ch)].bonus*2;

  gain += ch->points.mana_gain;
  
  if (IS_AFFECTED(ch,AFF_POISON))
    gain >>= 2;
  
  if((GET_COND(ch,FULL)==0)||(GET_COND(ch,THIRST)==0))
    gain >>= 2;

  if (GET_RACE(ch) == RACE_ELVEN || GET_RACE(ch) == RACE_GNOME ||
      GET_RACE(ch) == RACE_FAERIE)
    gain+=2;
  
  else if (GET_RACE(ch) == RACE_DRAAGDIM) 
    gain -= 5;

  if (GET_COND(ch, DRUNK)>10)
    gain += (gain >> 1);
  else if (GET_COND(ch, DRUNK)>0)
    gain += (gain >> 2);

  if (IS_AFFECTED2(ch, AFF2_ONE_LIFER))
    gain += number(3,7);

  if(GET_RACE(ch) == RACE_VAMPIRE) { /* this comes last! */
    gain += vamp_gain_mods(ch, gain, 0);
  }

  return ( (gain > 0) ? gain : 1); /* do we really want - mana? */
}


int hit_gain(struct char_data *ch)
     /* Hitpoint gain pr. game hour */
{

  int gain, dam, i;

 
  if(IS_NPC(ch)) {
    gain = 8;
    /* Neat and fast */
  } else {
    
    if (GET_POS(ch) == POSITION_FIGHTING ||
	ch->specials.fighting) {
      gain = 0;
    } else {
      struct time_info_data ma;
      age2(ch, &ma);
      gain = graf(ma.year, 2,4,6,8,6,3,1);
    }
  }    
    /* Class/Level calculations */
    
    /* Skill/Spell calculations */
    
    /* Position calculations    */
    
  switch (GET_POS(ch)) {
  case POSITION_SLEEPING:
    gain += gain>>2;
    break;
  case POSITION_RESTING:
    gain+= gain>>3;
    break;
  case POSITION_SITTING:
    gain += gain>>4;
    break;
  }
  
  if(GET_RACE(ch) == RACE_VEGMAN) {
    gain += veggie_gain_mods(ch, gain, 0);
  }
  
  if (GET_RACE(ch) == RACE_DWARF || GET_RACE(ch) == RACE_HALFORC)
    gain += 2;
  
  if (GET_RACE(ch) == RACE_OGRE)
    gain += 3;

  if (GET_RACE(ch) == RACE_HALFLING)
    gain += 1;

  gain += con_app[GET_CON(ch)].hitp/2;

  if (IS_AFFECTED(ch,AFF_POISON))  {
    gain = 0;
    dam = 32;
    if (GET_RACE(ch) == RACE_HALFLING)
      dam = 16;
    if (affected_by_spell(ch, SPELL_SLOW_POISON))
      dam /= 4;
    damage(ch, ch, dam, SPELL_POISON);
  }

  if (IS_AFFECTED2(ch, AFF2_HEAT_STUFF)) {
    dam = 0;
    /*
      count items in eq
      */
    for (i=0;i<=HOLD;i++) {
      if (ch->equipment[i]) {
	dam += 2;
      }
    }

    damage(ch, ch, dam, SPELL_HEAT_STUFF);
  }
  
  gain += ch->points.hit_gain;
  
  if((GET_COND(ch,FULL)==0)||(GET_COND(ch,THIRST)==0))
    gain >>= 4;
  
  if (GET_COND(ch, DRUNK)>10)
    gain += (gain >> 1);
  else if (GET_COND(ch, DRUNK)>0)
    gain += (gain >> 2);
  
  if (IS_AFFECTED2(ch, AFF2_ONE_LIFER))
    gain += number(1,5);

  if(GET_RACE(ch) == RACE_VAMPIRE) {
    gain += vamp_gain_mods(ch, gain, 0);
  }

  
  return (gain);
}



int move_gain(struct char_data *ch)
     /* move gain pr. game hour */
{
  int gain;
  
  if(IS_NPC(ch)) {
    gain = 22;
    if (IsRideable(ch))
      gain += gain/2;

    /* Neat and fast */
  } else {
      struct time_info_data ma;
      age2(ch, &ma);
    if (GET_POS(ch) != POSITION_FIGHTING)
      gain = graf(ma.year, 10,15,20,22,15,7,1);

    else
      gain = 0;
  }
    
    /* Position calculations    */
    switch (GET_POS(ch)) {
    case POSITION_SLEEPING:
      gain += (gain>>2); /* Divide by 4 */
      break;
    case POSITION_RESTING:
      gain+= (gain>>3);  /* Divide by 8 */
      break;
    case POSITION_SITTING:
      gain += (gain>>4); /* Divide by 16 */
      break;
    }
  
  
  if (GET_RACE(ch) == RACE_DWARF || GET_RACE(ch)==RACE_HALFLING || 
      GET_RACE(ch) == RACE_HALFORC)
    gain += 15;

  if(GET_RACE(ch) == RACE_VEGMAN) {
    gain += veggie_gain_mods(ch, gain, 1);
  }

  gain += ch->points.move_gain;
    
  if (IS_AFFECTED(ch,AFF_POISON))
    gain >>= 5;
  
  if((GET_COND(ch,FULL)==0)||(GET_COND(ch,THIRST)==0))
    gain >>= 3;

  if (IS_AFFECTED2(ch, AFF2_ONE_LIFER))
    gain += number(4,8);

  if(GET_RACE(ch) == RACE_VAMPIRE) {
    gain += vamp_gain_mods(ch, gain, 1);
  }
  
  return (gain);
}



/* Gain maximum in various points */
void advance_level(struct char_data *ch, int class)
{
  int add_hp, i;


  if (class > MAX_CLASS) {
    logE("Bad advance class.. no such class");
    return;
  }
  
  if (GET_LEVEL(ch, class) > 0 && 
      GET_EXP(ch) < titles[class][GET_LEVEL(ch, class)+1].exp) {
    /*  they can't advance here */
    logE("Bad advance_level");
    return;
  }

  GET_LEVEL(ch, class) += 1;



  if (class == WARRIOR_LEVEL_IND)  
     add_hp = con_app[GET_RCON(ch)].hitp;
  else 
     add_hp = MIN(con_app[GET_RCON(ch)].hitp,2);
    

  switch(class) {
    
  case MAGE_LEVEL_IND : {
    if (GET_LEVEL(ch, MAGE_LEVEL_IND) < 12)
      add_hp += number(1, 4);
    else
      add_hp += 1;
  } break;
    
  case CLERIC_LEVEL_IND : {
    if (GET_LEVEL(ch, CLERIC_LEVEL_IND) < 12)
      add_hp += number(1, 8);
    else
      add_hp += 3;
  } break;
    
  case THIEF_LEVEL_IND : {
    if (GET_LEVEL(ch, THIEF_LEVEL_IND) < 12)
      add_hp += number(1,6);
    else
      add_hp += 2;
  } break;
    
  case WARRIOR_LEVEL_IND : {
    if (GET_LEVEL(ch, WARRIOR_LEVEL_IND) < 10)
      add_hp += number(1,10);
    else
      add_hp += 4;

  } break;
  case DRUID_LEVEL_IND:
    if (GET_LEVEL(ch,  DRUID_LEVEL_IND) < 15)
      add_hp += number(1,8);
    else
      add_hp += 3;
    break;

  case MONK_LEVEL_IND:
    if (GET_LEVEL(ch, MONK_LEVEL_IND) < 17)
      add_hp += number(1,6);
    else
      add_hp += 2;
    break;

  }

  add_hp /= HowManyClasses(ch);

  ch->points.max_hit += MAX(1, add_hp);

  if (ch->specials.spells_to_learn < 50) {
    if(GET_RACE(ch) == RACE_OGRE) 
      ch->specials.spells_to_learn += 
	MIN(3,MAX(1, MAX(2, wis_app[GET_RWIS(ch)].bonus)/HowManyClasses(ch)));
    else
      ch->specials.spells_to_learn += 
	MAX(1, MAX(2, wis_app[GET_RWIS(ch)].bonus)/HowManyClasses(ch));
  } else {
    send_to_char("Practices: Use them or lose them (you just did).\n\r", ch);
  }

  ClassSpecificStuff(ch);

  if (GetMaxLevel(ch) >= LOW_IMMORTAL)
    for (i = 0; i < 3; i++)
      ch->specials.conditions[i] = -1;

}	



/* Lose in various points */
/*
** Damn tricky for multi-class...
*/

void drop_level(struct char_data *ch, int class)
{
  int add_hp, lin_class;
  
  extern struct wis_app_type wis_app[];
  extern struct con_app_type con_app[];
  
  
  if (GetMaxLevel(ch) >= LOW_IMMORTAL)
    return;
  if (GetMaxLevel(ch) == 1)
    return;
  
  add_hp = con_app[GET_RCON(ch)].hitp;
  
  switch(class) {
    
  case CLASS_MAGIC_USER : {
    lin_class = MAGE_LEVEL_IND;
    if (GET_LEVEL(ch, MAGE_LEVEL_IND) < 12)
      add_hp += number(2, 8);
    else
      add_hp += 2;
  } break;
    
  case CLASS_CLERIC : {
    lin_class = CLERIC_LEVEL_IND;
    if (GET_LEVEL(ch, CLERIC_LEVEL_IND) < 12)
      add_hp += number(2, 12);
    else
      add_hp += 5;
  } break;
    
  case CLASS_THIEF : {
    lin_class = THIEF_LEVEL_IND;
    if (GET_LEVEL(ch, THIEF_LEVEL_IND) < 12)
      add_hp += number(2,10);
    else
      add_hp += 4;
  } break;
    
  case CLASS_WARRIOR : {
    lin_class = WARRIOR_LEVEL_IND;
    if (GET_LEVEL(ch, WARRIOR_LEVEL_IND) < 10)
      add_hp += number(2,14);
    else
      add_hp += 6;
  } break;

  case CLASS_DRUID:
    lin_class = DRUID_LEVEL_IND;
    if (GET_LEVEL(ch,  DRUID_LEVEL_IND) < 15)
      add_hp += number(2,12);
    else
      add_hp += 5;
    break;

  case CLASS_MONK:
    lin_class = MONK_LEVEL_IND;
    if (GET_LEVEL(ch, MONK_LEVEL_IND) < 17)
      add_hp += number(2,10);
    else
      add_hp += 4;
    break;

  }


  GET_LEVEL(ch, lin_class) -= 1;
  
  if (GET_LEVEL(ch, lin_class) < 1) {
    GET_LEVEL(ch, lin_class) = 1;
    if (ch->points.max_hit > 15)
      ch->points.max_hit = 15;
  }

  if (lin_class == WARRIOR_LEVEL_IND)
    add_hp = MAX(add_hp, 6);
  if (lin_class == CLERIC_LEVEL_IND)
    add_hp = MAX(add_hp, 5);
  if (lin_class == THIEF_LEVEL_IND)
    add_hp = MAX(add_hp, 4);
  if (lin_class == MAGE_LEVEL_IND)
    add_hp = MAX(add_hp, 3);
  if (lin_class == MONK_LEVEL_IND)
    add_hp = MAX(add_hp, 4);
  if (lin_class == DRUID_LEVEL_IND)
    add_hp = MAX(add_hp, 5);

  add_hp /= HowManyClasses(ch);

  if (add_hp <=2) add_hp=3;

  ch->points.max_hit -= MAX(1,add_hp);
  if (ch->points.max_hit < 1)
    ch->points.max_hit = 1;
  
    ch->specials.spells_to_learn -= MAX(1, MAX(2, wis_app[GET_RWIS(ch)].bonus)/HowManyClasses(ch));

  ch->points.exp = 
    MIN(titles[lin_class][GET_LEVEL(ch, lin_class)].exp, GET_EXP(ch));
  
  if (ch->points.exp < 0)
    ch->points.exp = 0;

  send_to_char("You lose a level\n\r", ch);
}	



void set_title(struct char_data *ch)
{
  
  char buf[256];
  
  sprintf(buf, 
     "the %s %s", RaceName[ch->race], ClassTitles(ch));
  
  if (GET_TITLE(ch)) {
    free(GET_TITLE(ch));
    CREATE(GET_TITLE(ch),char,strlen(buf)+1);    
  } else {
    CREATE(GET_TITLE(ch),char,strlen(buf)+1);
  }

  strcpy(GET_TITLE(ch), buf);

}

void gain_exp(struct char_data *ch, int gain)
{
  int i;
  char buf[256];
  short chrace;
  save_char(ch,AUTO_RENT);

  if (!IS_PC(ch) && ch->master && IS_AFFECTED(ch, AFF_CHARM)) {
    if (ch->master->in_room == ch->in_room) {
      if (gain > 1) {
	gain/=2;
	sprintf(buf, "you gain $N's share of %d exp", gain);
	act(buf, 0, ch->master, 0, ch, TO_CHAR);
	gain_exp(ch->master, gain);
      }
    }
    return;
  }
  

  
  if (!IS_IMMORTAL(ch)) {
    if (gain > 0) {
      gain /= HowManyClasses(ch);

      if (GetMaxLevel(ch) == 1 && IS_PC(ch)) {
	gain *= 2;
      }

      if (IS_AFFECTED2(ch, AFF2_ONE_LIFER))
	gain *= 2;

      if (IS_PC(ch)) {
	if (ch->desc && ch->desc->original)
	  chrace = ch->desc->original->race;
	else
	  chrace = GET_RACE(ch);
	for (i = MAGE_LEVEL_IND; i < MAX_CLASS; i++) {
	  
	  if (GET_LEVEL(ch,i)&&(GET_LEVEL(ch,i))<RacialMax[chrace][i]) {
   	    if (GET_EXP(ch) >= titles[i][GET_LEVEL(ch,i)+2].exp) {
	      send_to_char("You must practice at a guild before you can gain any more experience\n\r", ch);
	      GET_EXP(ch) = titles[i][GET_LEVEL(ch,i)+2].exp - 1;
	      return;	      
	    } else if (GET_EXP(ch) >= titles[i][GET_LEVEL(ch,i)+1].exp) {
	      /* do nothing..this is cool */
	    } else if (GET_EXP(ch)+gain >= titles[i][GET_LEVEL(ch,i)+1].exp) {
	      sprintf(buf, "You have gained enough to be a(n) %s\n\r", GET_CLASS_TITLE(ch, i, GET_LEVEL(ch, i)+1));
	      send_to_char(buf, ch);
	      send_to_char("You must return to a guild to earn the level\n\r",ch);
	      if (GET_EXP(ch)+gain >= titles[i][GET_LEVEL(ch,i)+2].exp) {
		GET_EXP(ch) = titles[i][GET_LEVEL(ch,i)+2].exp - 1;
		return;
	      }
	    }
	  }
	}
      }

      GET_EXP(ch)+=gain;
      if (IS_PC(ch)) {
         for (i=MAGE_LEVEL_IND; i< MAX_CLASS; i++) {
           if (GET_LEVEL(ch,i)&&GET_LEVEL(ch, i)<RacialMax[chrace][i]) {
             if (GET_EXP(ch) > titles[i][GET_LEVEL(ch,i)+2].exp) {
               GET_EXP(ch) = titles[i][GET_LEVEL(ch,i)+2].exp - 1;
             }
          }
        }
      }
    }

    if (gain < 0) {
      GET_EXP(ch) += gain;
      if (GET_EXP(ch) < 0)
	GET_EXP(ch) = 0;
    }
  }
}


void gain_exp_regardless(struct char_data *ch, int gain, int class)
{
  int i;
  bool is_altered = FALSE;

  save_char(ch,AUTO_RENT);
  if (!IS_NPC(ch)) {
    if (gain > 0) {
      GET_EXP(ch) += gain;

      for (i=0;(i<ABS_MAX_LVL) &&(titles[class][i].exp <= GET_EXP(ch)); i++) {
	if (i > GET_LEVEL(ch,class)) {
	  send_to_char("You raise a level\n\r", ch);
	  GET_LEVEL(ch,class) = i;
	  advance_level(ch,class);
	  is_altered = TRUE;
	}
      }
    }
    if (gain < 0) 
      GET_EXP(ch) += gain;
    if (GET_EXP(ch) < 0)
      GET_EXP(ch) = 0;
  }
  if (is_altered)
    set_title(ch);
}

void gain_condition(struct char_data *ch,int condition,int value)
{
  bool intoxicated;
  
  if(GET_COND(ch, condition)==-1) /* No change */
    return;
  
  if(GET_RACE(ch) == RACE_VAMPIRE && condition == THIRST)
    return;
  
  intoxicated=(GET_COND(ch, DRUNK) > 0);

  if(condition == FULL && GET_RACE(ch) == RACE_VEGMAN)
    return;
  
  if(condition == DRUNK && GET_RACE(ch) == RACE_VEGMAN && value > 0 ) {
    send_to_char("You feel the alcohol run through your phloem.\n\r",ch);
    GET_HIT(ch) -= value;
  }

  GET_COND(ch, condition)  += value;
  
  GET_COND(ch,condition) = MAX(0,GET_COND(ch,condition));
  GET_COND(ch,condition) = MIN(24,GET_COND(ch,condition));
  
  if(GET_COND(ch,condition))
    return;
  
  switch(condition){
  case FULL :
    {
      if(GET_RACE(ch) == RACE_VAMPIRE)
	send_to_char("You hunger for blood.\n\r", ch);
      else
	send_to_char("You are hungry.\n\r",ch);
      return;
    }
  case THIRST :
    {
      send_to_char("You are thirsty.\n\r",ch);
      return;
    }
  case DRUNK :
    {
      if(intoxicated)
	send_to_char("You are now sober.\n\r",ch);
      return;
    }
    default : break;
  }
  
}


void check_idling(struct char_data *ch)
{
  void do_save(struct char_data *ch, char *argument, int cmd);


  if (++(ch->specials.timer) == 8) {
       do_save(ch, "", 0);
       
  } else if (ch->specials.timer == 72) {

    if (ch->in_room != NOWHERE && ch->in_room != 0) {
      ch->specials.was_in_room = ch->in_room;
      if (ch->specials.fighting)     	{
	stop_fighting(ch->specials.fighting);
	stop_fighting(ch);
      }
      act("$n disappears into the void.", TRUE, ch, 0, 0, TO_ROOM);
      send_to_char("You have been idle, and are pulled into a void.\n\r", ch);
      char_from_room(ch);
      char_to_room(ch, 0);  /* Into room number 0 */
    }

  } else if (ch->specials.timer == 120)  {
      struct obj_cost cost;
      if (ch->in_room != NOWHERE)
	char_from_room(ch);
      
      char_to_room(ch, 4);
      
      if (ch->desc) 
	close_socket(ch->desc);

      ch->desc = 0;

#if defined(NO_RENT)
      recep_offer(ch, NULL, &cost);
      save_obj(ch, &cost, 1);
      extract_char(ch);
      save_char(ch, AUTO_RENT);
#else
      if (recep_offer(ch, NULL, &cost)) {
	cost.total_cost = 100;
	save_obj(ch, &cost, 1);
       }
      
      extract_char(ch);
#endif
    }
}



int ObjFromCorpse( struct obj_data *c)
{
  struct obj_data *jj, *next_thing;

  for(jj = c->contains; jj; jj = next_thing) {
    next_thing = jj->next_content; /* Next in inventory */
    if (jj->in_obj) {
      obj_from_obj(jj);
      if (c->in_obj)
	obj_to_obj(jj,c->in_obj);
      else if (c->carried_by) {
	obj_to_room(jj,c->carried_by->in_room);
	check_falling_obj(jj, c->carried_by->in_room);
      } else if (c->in_room != NOWHERE) {
	obj_to_room(jj,c->in_room);
	check_falling_obj(jj, c->in_room);
      } else
	assert(FALSE);
    } else {
      /*
       **  hmm..  it isn't in the object it says it is in.
       **  don't extract it.
       */
      c->contains = 0;
      logE("Memory lost in ObjFromCorpse.");
      return(TRUE);
    }
  }
  extract_obj(c);
}



int ClassSpecificStuff( struct char_data *ch)
{

  if (HasClass(ch, CLASS_WARRIOR) || HasClass(ch, CLASS_MONK)) {

    ch->mult_att = 1.0;
    
    if (HasClass(ch, CLASS_WARRIOR)) {
      
      ch->mult_att += (GET_LEVEL(ch, WARRIOR_LEVEL_IND)*.05);
      /* ch->mult_att+=(MIN(50,(GET_LEVEL(ch, WARRIOR_LEVEL_IND)))*.05); */
    } else {
      if (HasClass(ch, CLASS_MONK)) {
	ch->mult_att+= (GET_LEVEL(ch, MONK_LEVEL_IND)/16.0);
      }
      /* fix up damage stuff */
      switch(GET_LEVEL(ch, MONK_LEVEL_IND)) {
      case 1:
      case 2:
      case 3:
	ch->specials.damnodice = 1;
	ch->specials.damsizedice = 3;
	break;
      case 4:
      case 5:
	ch->specials.damnodice = 1;
	ch->specials.damsizedice = 4;
	break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
	ch->specials.damnodice = 1;
	ch->specials.damsizedice = 6;
	break;
      case 12:
      case 13:
      case 14:
	ch->specials.damnodice = 2;
	ch->specials.damsizedice = 3;
	break;
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
	ch->specials.damnodice = 2;
	ch->specials.damsizedice = 4;
	break;
      case 20:
      case 21:
	ch->specials.damnodice = 3;
	ch->specials.damsizedice = 3;
	break;
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
	ch->specials.damnodice = 3;
	ch->specials.damsizedice = 4;
	break;
      case 27:
      case 28:
      case 29:
	ch->specials.damnodice = 4;
	ch->specials.damsizedice = 3;
	break;
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
	ch->specials.damnodice = 4;
	ch->specials.damsizedice = 4;
	break;
      case 35:
      case 36:
	ch->specials.damnodice = 4;
	ch->specials.damsizedice = 5;
	break;
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
	ch->specials.damnodice = 5;
	ch->specials.damsizedice = 4;
	break;
      case 42:
      case 43:
      case 44:
	ch->specials.damnodice = 6;
	ch->specials.damsizedice = 4;
	break;
      case 45:
      case 46:
      case 47:
      case 48:
      case 49:
	ch->specials.damnodice = 6;
	ch->specials.damsizedice = 5;
	break;
      case 50:
	ch->specials.damnodice = 8;
	ch->specials.damsizedice = 4;
	break;
      default:
	ch->specials.damnodice=1;
	ch->specials.damsizedice = 2;
	break;
      }
    }
  }

      /* other stuff.. immunities, etc, are set here */

  if (HasClass(ch, CLASS_MONK)) {
    if (GET_LEVEL(ch, MONK_LEVEL_IND) > 10)
      SET_BIT(ch->M_immune, IMM_HOLD);
    
    if (GET_LEVEL(ch, MONK_LEVEL_IND) > 31)
      SET_BIT(ch->M_immune, IMM_POISON);
    else if (GET_LEVEL(ch, MONK_LEVEL_IND) > 14)
      SET_BIT(ch->immune, IMM_POISON);
    
    if (GET_LEVEL(ch, MONK_LEVEL_IND) > 35)
      SET_BIT(ch->M_immune, IMM_CHARM);
    else if (GET_LEVEL(ch, MONK_LEVEL_IND) > 17)
      SET_BIT(ch->immune, IMM_CHARM);
  } else {
    if (HasClass(ch, CLASS_DRUID)) {
      if (GET_LEVEL(ch, DRUID_LEVEL_IND) >= 14) {
	SET_BIT(ch->immune, IMM_CHARM);
      }
      if (GET_LEVEL(ch, DRUID_LEVEL_IND) >= 32) {
	SET_BIT(ch->M_immune, IMM_POISON);
      } else if (GET_LEVEL(ch, DRUID_LEVEL_IND) >= 15) {
	SET_BIT(ch->immune, IMM_POISON);
      }
      if (GET_LEVEL(ch, DRUID_LEVEL_IND >= 20)) {
	SET_BIT(ch->immune, IMM_ELEC);
      }
      if (GET_LEVEL(ch, DRUID_LEVEL_IND >= 30)) {
	SET_BIT(ch->immune, IMM_COLD);
      }
      if (GET_LEVEL(ch, DRUID_LEVEL_IND >= 40)) {
	SET_BIT(ch->immune, IMM_FIRE);
      }
    }

    if (HasClass(ch, CLASS_THIEF)) {

      if (OnlyClass(ch, CLASS_THIEF))
	GET_CHR(ch)+=1;
      
      GET_CHR(ch) += GET_LEVEL(ch, THIEF_LEVEL_IND)/10;
    }

    if(!HasClass(ch,CLASS_WARRIOR)) {
      ch->mult_att = 1.0;
      if(HasClass(ch,CLASS_CLERIC)) {
	ch->mult_att += (GET_LEVEL(ch, CLERIC_LEVEL_IND) * .03);
      } else if (HasClass(ch,CLASS_DRUID)) {
	ch->mult_att += (GET_LEVEL(ch, DRUID_LEVEL_IND) * .03);
      } else if (HasClass(ch,CLASS_THIEF)) {
	ch->mult_att += (GET_LEVEL(ch, THIEF_LEVEL_IND) / 100.0);
      }
    }
  }
}
