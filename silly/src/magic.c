/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "protos.h"

/* Extern structures */
#if HASH
extern struct hash_header room_db;
#else
extern struct room_data *room_db;
#endif
extern struct obj_data  *object_list;
extern struct char_data *character_list;


/* For future use in blinding those with infravision who are fireballed
   or otherwise subjected to lotsa heat quickly in dark rooms. */

void heat_blind(struct char_data *ch)
{
  struct affected_type af;
  byte tmp;

  tmp=number(1,4);

  if(!ch)		/* Dunno if this does anything */
    return;

  if ( IS_AFFECTED(ch, AFF_BLIND) ) {
    return;			/* no affect */
  }
  else if  ((IS_DARK(ch->in_room)) && (!IS_IMMORTAL(ch)) &&
	    (!IS_AFFECTED(ch, AFF_TRUE_SIGHT)) && 
	    (IS_AFFECTED(ch, AFF_INFRAVISION)))  {
    send_to_char("Aaarrrggghhh!!  The heat blinds you!!\n\r", ch);
    af.type      = SPELL_BLINDNESS;
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;  /* Make hitroll worse */
    af.duration  = tmp;
    af.bitvector = AFF_BLIND;
    affect_to_char(ch, &af);
  }
}
	   

/* Offensive Spells */

void spell_magic_missile(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = dice((int)(level / 2)+1,4)+(level / 2)+1;

  if (affected_by_spell(victim,SPELL_SHIELD))
    dam = 0;

  MissileDamage(ch, victim, dam, SPELL_MAGIC_MISSILE);
}



void spell_chill_touch(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = number(level, 3*level);

  if ( !saves_spell(victim, SAVING_SPELL) )
  {
    af.type      = SPELL_CHILL_TOUCH;
    af.duration  = 6;
    af.modifier  = -1;
    af.location  = APPLY_STR;
    af.bitvector = 0;
    affect_join(victim, &af, TRUE, FALSE);
  } else {
    dam >>= 1;
  }
  damage(ch, victim, dam, SPELL_CHILL_TOUCH);
}

void spell_burning_hands(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  struct char_data *tmp_victim;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(1,4) + level/2 + 1;

  send_to_char("Searing flame fans out in front of you!\n\r", ch);
  act("$n sends a fan of flame shooting from the fingertips!\n\r",
	  FALSE, ch, 0, 0, TO_ROOM);

  for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
       tmp_victim = tmp_victim->next_in_room ) {
    
    if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
      if ((GetMaxLevel(tmp_victim)>LOW_IMMORTAL) && (!IS_NPC(tmp_victim)))
	return;
      if (!in_group(ch, tmp_victim)) {
	act("You are seared by the burning flame!\n\r",
	    FALSE, ch, 0, tmp_victim, TO_VICT);
	heat_blind(tmp_victim);
	if ( saves_spell(tmp_victim, SAVING_SPELL) )
	  dam = 0;
	MissileDamage(ch, tmp_victim, dam, SPELL_BURNING_HANDS);
      } else {
	act("You are able to avoid the flames!\n\r",
	    FALSE, ch, 0, tmp_victim, TO_VICT);
	heat_blind(tmp_victim);
      }
    }
  }
}



void spell_shocking_grasp(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = number(1,8)+level;

  if ((GET_HIT(victim) < -4) && IsHumanoid(victim) &&
      !IsUndead(victim)) {
    act ("$n utters the words 'clear', and touches $N's chest!!", 
	 FALSE,ch, 0, victim, TO_ROOM);
    GET_HIT(victim) += dam;
    update_pos(victim);
    return;
  }

  if (!HitOrMiss(ch, victim, CalcThaco(ch)))
    dam = 0;

  damage(ch, victim, dam, SPELL_SHOCKING_GRASP);
}



void spell_lightning_bolt(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = dice(level,6);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  MissileDamage(ch, victim, dam, SPELL_LIGHTNING_BOLT);
}



void spell_colour_spray(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = 4 * level;

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  MissileDamage(ch, victim, dam, SPELL_COLOUR_SPRAY);

}


/* Drain XP, MANA, HP - caster gains HP and MANA */
void spell_energy_drain(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam, tmp;

  assert(victim && ch);
  assert((level >= 1) && (level <=  ABS_MAX_LVL));

  if ( !saves_spell(victim, SAVING_SPELL) ) {
    GET_ALIGNMENT(ch) = MAX(-1000, GET_ALIGNMENT(ch)-200);
    send_to_char("You feel wicked!\n\r", ch);
    if (GetMaxLevel(victim) <= 1) {
      damage(ch, victim, 100, SPELL_ENERGY_DRAIN); /* Kill the sucker */
    } else if ((!IS_NPC(victim)) && (GetMaxLevel(victim) >= LOW_IMMORTAL)) {
      send_to_char("Some puny mortal just tried to drain you...\n\r",victim);
    } else {

      if (!IS_SET(victim->M_immune, IMM_DRAIN)) {

	send_to_char("Your life energy is drained!\n\r", victim);
	 dam = 1;
	 damage(ch, victim, dam, SPELL_ENERGY_DRAIN);
	 if (!IS_NPC(victim)) {
	    drop_level(victim, BestClass(victim));
	    set_title(victim);
	  } else {
	    tmp = GET_MAX_HIT(victim)/GetMaxLevel(victim);
	    victim->points.max_hit -=tmp;
	    GET_HIT(victim) -= tmp;
	    GET_HIT(ch) += tmp;
	    tmp = GET_EXP(victim)/GetMaxLevel(victim);
	    GET_EXP(ch)+=tmp;
	    GET_EXP(victim)-=tmp;
	    victim->points.hitroll+=1;
	  }
      } else {
	 if (!IS_SET(ch->M_immune, IMM_DRAIN)) {
	    send_to_char("Your spell backfires!\n\r",ch);
	    dam = 1;
	    damage(ch, victim, dam, SPELL_ENERGY_DRAIN);
	    if (!IS_NPC(ch)) {
	       drop_level(ch, BestClass(ch));
               set_title(ch);
	     } else {
	       tmp = GET_MAX_HIT(victim)/GetMaxLevel(victim);
	       victim->points.max_hit -=tmp;
	       GET_HIT(victim) -= tmp;
	       GET_HIT(ch) += tmp;
	       victim->points.hitroll+=1;
	       tmp = GET_EXP(victim)/GetMaxLevel(victim);
	       GET_EXP(ch)+=tmp;
	       GET_EXP(victim)-=tmp;
	     }
         } else {
	   send_to_char("Your spell fails utterly.\n\r",ch);
	 }
       }

     }
  } else {
    damage(ch, victim, 0, SPELL_ENERGY_DRAIN); /* Miss */
  }
}



void spell_fireball(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = dice(level,8);

  AreaDamage(ch, dam, SPELL_FIREBALL, "", 
	     "You dodge the mass of flame!!\n\r", 
	     "You feel a blast of hot air.\n\r", FALSE, TRUE);

  return;

#if 0
   for(tmp_victim = character_list; tmp_victim; tmp_victim = temp) {
      temp = tmp_victim->next;
      if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
         if (!in_group(ch,tmp_victim) && !IS_IMMORTAL(tmp_victim)) {
	   if ( saves_spell(tmp_victim, SAVING_SPELL) )
	     dam >>= 1;
	   heat_blind(tmp_victim);
	   MissileDamage(ch, tmp_victim, dam, SPELL_FIREBALL);
	 } else {
            act("You dodge the mass of flame!!\n\r",
                 FALSE, ch, 0, tmp_victim, TO_VICT);
	    heat_blind(tmp_victim);
	 }
      } else {
	 if (tmp_victim->in_room != NOWHERE) {
            if (real_roomp(ch->in_room)->zone == 
		real_roomp(tmp_victim->in_room)->zone) {
                send_to_char("You feel a blast of hot air.\n\r", tmp_victim);
	    }
	 }
      }
   } 
#endif
}


void spell_earthquake(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  struct char_data *tmp_victim, *temp;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam =  dice(1,4) + level + 1;

  send_to_char("The earth trembles beneath your feet!\n\r", ch);
  act("$n makes the earth tremble and shiver.",
      FALSE, ch, 0, 0, TO_ROOM);

  /* Too special to use AreaDamage() on */
  for(tmp_victim = character_list; tmp_victim; tmp_victim = temp) {
    temp = tmp_victim->next;
    if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
      if (!in_group(ch,tmp_victim) && !IS_IMMORTAL(tmp_victim)) {
	
	if (GetMaxLevel(tmp_victim) > 4) {
	  act("You fall and hurt yourself!!\n\r",
	      FALSE, ch, 0, tmp_victim, TO_VICT);
	  if (number(1,20) > ch->abilities.dex) {
	    GET_POS(tmp_victim) = POSITION_SITTING;
	    send_to_char("You fall on your butt!\n\r",tmp_victim);
	  }
	  MissileDamage(ch, tmp_victim, dam, SPELL_EARTHQUAKE);
	} else {
	  act("You are sucked into a huge hole in the ground!", FALSE,
	      ch, 0, tmp_victim, TO_VICT);
	  act("$N is sucked into a huge hole in the ground!", FALSE,
	      ch, 0, tmp_victim, TO_NOTVICT);
	  MissileDamage(ch, tmp_victim, GET_MAX_HIT(tmp_victim)*12, 
			SPELL_EARTHQUAKE);
	}
      } else {
	act("You almost fall and hurt yourself!!\n\r",
	    FALSE, ch, 0, tmp_victim, TO_VICT);
      }
    } else {
      if (real_roomp(ch->in_room)->zone == 
	  real_roomp(tmp_victim->in_room)->zone)
	send_to_char("The earth trembles...\n\r", tmp_victim);
    }
  } 
}



void spell_dispel_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam=1;
  assert(ch && victim);
  assert((level >= 1) && (level<=ABS_MAX_LVL));
    
  

  if (IsExtraPlanar(victim)) {
    if (IS_EVIL(ch)) {
      victim = ch;
      send_to_char("Eeek, your evilness taints the spell!  It backfires!\n\r",
		   ch);
      act("The spell backfires on $n!",TRUE, ch, 0, victim, TO_ROOM);
      dam=number(2,8);
      damage(ch, victim, dam, SPELL_EARTHQUAKE);
      return;

    } else {
      if (IS_GOOD(victim)) {
	act("Good protects $N.", FALSE, ch, 0, victim, TO_CHAR);
	return;
      }
    }
    if (!saves_spell(victim, SAVING_SPELL) ) {
      act("$n forces $N from this plane.", TRUE, ch, 0, victim, TO_ROOM);
      act("You force $N from this plane.", TRUE, ch, 0, victim, TO_CHAR);
      act("$n forces you from this plane.", TRUE, ch, 0, victim,TO_VICT);
      gain_exp(ch, GET_EXP(victim)/2);
      extract_char(victim);
    } else {
      act("$N resists the attack",TRUE, ch, 0, victim, TO_CHAR);
      act("You resist $n's attack.", TRUE, ch, 0, victim, TO_VICT);
      damage(ch, victim, dam, SPELL_EARTHQUAKE);
    }
  } else {
    act("$N laughs at you for thinking it was extra-planar.", 
	TRUE, ch, 0, victim, TO_CHAR);
    act("$N laughs at $n.", TRUE,ch, 0, victim, TO_NOTVICT);
    act("You laugh at $n.", TRUE,ch,0,victim,TO_VICT);
  }
}


void spell_call_lightning(byte level, struct char_data *ch,
			  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  
  extern struct weather_data weather_info;
  
  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));
  
  dam = dice( level+2, 8);
  
  if (OUTSIDE(ch) && (weather_info.sky>=SKY_RAINING)) {
    
    if ( saves_spell(victim, SAVING_SPELL) )
      dam >>= 1;
    
    MissileDamage(ch, victim, dam, SPELL_CALL_LIGHTNING);
  } else {
    send_to_char("The proper atmospheric conditions are not at hand.\n\r", ch);
    return;
  } 
}



void spell_harm(byte level, struct char_data *ch,
		struct char_data *victim, struct obj_data *obj)
{
  int dam;
  
  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));
  
  dam = GET_HIT(victim) - dice(1,4);
  
  if (dam < 0)
    dam = 100; /* Kill the suffering bastard */
  else {
    if (GET_RACE(ch) == RACE_GOD)
      dam = 0;
    if (!HitOrMiss(ch, victim, CalcThaco(ch)))
      dam = 0;
  }
  dam = MIN(dam, 100);
  
  damage(ch, victim, dam, SPELL_HARM);

  if (IS_PC(ch) && IS_PC(victim))
    GET_ALIGNMENT(ch)-=4;

}



/* spells2.c - Not directly offensive spells */

void spell_armor(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if(IS_NEUTRAL(ch) ||
     (IS_GOOD(ch) && !IS_EVIL(victim)) ||
     (IS_EVIL(ch) && !IS_GOOD(victim)) ) {
    
    if (!affected_by_spell(victim, SPELL_ARMOR)) {
      
      af.type      = SPELL_ARMOR;
      af.duration  = 24;
      af.modifier  = -MAX(10,level);
      af.location  = APPLY_AC;
      af.bitvector = 0;
      
      affect_to_char(victim, &af);
      act("$n is protected.",TRUE,victim,0,ch,TO_VICT);
      send_to_char("You feel someone protecting you.\n\r", victim);
      if(ch != victim)
	act("$n encases $N in a mystical armor.",TRUE,ch,0,victim,TO_NOTVICT);
      else
	act("$n is encased by a mystical force.",TRUE,ch,0,victim,TO_NOTVICT);
    } else {
      send_to_char("Nothing New seems to happen.\n\r", ch);
    }
  } else {
    act("$n calls upon $s god to help $N, but the god refuses!",
	TRUE,ch,0,victim,TO_NOTVICT);
    act("$n's god refuses to assist one such as thee!",TRUE,ch,0,victim,
	TO_VICT);
    act("Your god refuses to assist $n!",TRUE,victim,0,ch,TO_VICT);
  }
}

#define ASTRAL_ENTRANCE   8100	/* to 8224 */

void spell_astral_walk(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
  int entrance;
  struct char_data *tmp, *tmp2;
  struct room_data *rp;

  rp = real_roomp(ch->in_room);
  
  entrance=(number(0,124) + ASTRAL_ENTRANCE);
  
  for (tmp = rp->people;tmp;tmp=tmp2) {
    tmp2 = tmp->next_in_room;
    if (in_group(ch, tmp)) {
      act("$n wavers, fades and dissapears", FALSE, tmp, 0, 0, TO_ROOM);
      char_from_room(tmp);
      char_to_room(tmp, entrance);      
      act("$n appears from a rift in reality.",FALSE, tmp, 0, 0, TO_ROOM);
      do_look(tmp, "\0", 0);
    }
  }  
}

void spell_teleport(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int to_room, try = 0;
  extern int top_of_world;      /* ref to the top element of world */
  struct room_data *room;
  
  assert(ch && victim);
  
  if (victim != ch) {
    if (saves_spell(victim,SAVING_SPELL)) {
      send_to_char("Your spell has no effect.\n\r",ch);
      if (IS_NPC(victim)) {
	if (!victim->specials.fighting)
	  set_fighting(victim, ch);
      } else {
	send_to_char("Your body is momentarily forced somewhere else.\n\r",
		     victim);
      }
      return;
    } else {
      ch = victim;  /* the character (target) is now the victim */
    }
  }
  
  if (!IsOnPmp(victim->in_room)) {
    send_to_char("You're on an extra-dimensional plane!\n\r", ch);
    return;
  }

  
  do {
    to_room = number(0, top_of_world);
    room = real_roomp(to_room);
    if (room) {
      if ((IS_SET(room->room_flags, PRIVATE)) ||
	  (IS_SET(room->room_flags, TUNNEL)) ||
	  (IS_SET(room->room_flags, NO_SUM)) ||
	  (IS_SET(room->room_flags, NO_MAGIC)) ||
	  !IsOnPmp(to_room)) {
	room = 0;
        try++;
	}
    }
    
  } while (!room && try < 10);
  
  if (try >= 5) {
    send_to_char("The magic fails.\n\r", ch);
    return;
  }

  act("$n slowly fade out of existence.", FALSE, ch,0,0,TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, to_room);
  act("$n slowly fade in to existence.", FALSE, ch,0,0,TO_ROOM);
  
  do_look(ch, "", 0);
  
  if (IS_SET(real_roomp(to_room)->room_flags, DEATH) && 
      GetMaxLevel(ch) < LOW_IMMORTAL) {
    NailThisSucker(ch);
    return;
  }

  check_falling(ch);

}



void spell_bless(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(ch && (victim || obj));
  assert((level >= 0) && (level <= ABS_MAX_LVL));
  
  if (obj) {
    if ( (5*GET_LEVEL(ch,CLERIC_LEVEL_IND) > GET_OBJ_WEIGHT(obj)) &&
	(GET_POS(ch) != POSITION_FIGHTING) &&
	!IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_BLESS);
      act("$p briefly glows.",FALSE,ch,obj,0,TO_CHAR);
    }
  } else {
    
    if ((GET_POS(victim) != POSITION_FIGHTING) &&
	(!affected_by_spell(victim, SPELL_BLESS))) {

      if(IS_EVIL(ch)) {
	if(ch != victim)
	  act("$n bestows an unholy blessing upon $N.",
	      TRUE,ch,0,victim,TO_NOTVICT);
	else 
	  act("$n receives an unholy blessing.",TRUE,ch,0,victim,TO_NOTVICT);
	send_to_char("You feel wicked.\n\r",victim);
      } else {
	if(ch != victim)
	  act("$n bestows a holy blessing upon $N.",
	      TRUE,ch,0,victim,TO_NOTVICT);
	else
	  act("$n receives a holy blessing.",TRUE,ch,0,victim,TO_NOTVICT);
	send_to_char("You feel righteous.\n\r", victim);
      }

      af.type      = SPELL_BLESS;
      af.duration  = 6;
      af.modifier  = 1;
      af.location  = APPLY_HITROLL;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -1;                 /* Make better */
      affect_to_char(victim, &af);
    } else if(GET_POS(victim) == POSITION_FIGHTING) {
      send_to_char("You cannot reach someone in battle.\n\r",ch);
    } else {			/* must allready be affected */
      send_to_char("Nothing new seems to happen.\n\r",ch);
    }
  }
}



void spell_blindness(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));


  if (saves_spell(victim, SAVING_SPELL) ) {

    /* Blindness is only hostile if it fails. */

    if(ch != victim) {
      act("$N blinks twice, and ignores $n's attempt to blind $M.",
	  TRUE,ch,0,victim,TO_ROOM);
      send_to_char("You attempt to blind fails!\n\r",ch);
      act("$N tried to blind you!!",TRUE,victim,0,ch,TO_CHAR);

      if ((!victim->specials.fighting)&&(victim!=ch))
	set_fighting(victim,ch);
      return;
    } else {
      act("$N is almost blinded!",TRUE,ch,0,victim,TO_ROOM);
      send_to_char("You were almost blinded!",ch);

      if ((!victim->specials.fighting)&&(victim!=ch))
	set_fighting(victim,ch);
      return;
    }
  }

  
  if ( affected_by_spell(victim, SPELL_BLINDNESS)) {
    act("But $N is allready blinded!", FALSE, ch, 0, victim, TO_CHAR);
    return;
  }
  
  act("$n seems to be blinded!", TRUE, victim, 0, 0, TO_ROOM);
  send_to_char("You have been blinded!\n\r", victim);
  
  af.type      = SPELL_BLINDNESS;
  af.location  = APPLY_HITROLL;
  af.modifier  = -4;  /* Make hitroll worse */
  af.duration  = level / 2;
  af.bitvector = AFF_BLIND;
  affect_to_char(victim, &af);
  
  
  af.location = APPLY_AC;
  af.modifier = +20; /* Make AC Worse! */
  affect_to_char(victim, &af);
  
  return;
}



void spell_clone(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && (victim || obj));
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  send_to_char("Clone is not ready yet.\n\r", ch);

  if (obj) {

  } else {
    /* clone_char(victim); */
  }
}



void spell_control_weather(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
   /* Control Weather is not possible here!!! */
   /* Better/Worse can not be transferred     */
}



void spell_create_food(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *tmp_obj;

  assert(ch);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  CREATE(tmp_obj, struct obj_data, 1);
  clear_object(tmp_obj);

  tmp_obj->name = strdup("mushroom");
  tmp_obj->short_description = strdup("A Magic Mushroom");
  tmp_obj->description = strdup("A really delicious looking magic mushroom lies here.");

  tmp_obj->obj_flags.type_flag = ITEM_FOOD;
  tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_HOLD;
  tmp_obj->obj_flags.value[0] = (int) 5+level/2;
  tmp_obj->obj_flags.weight = 1;
  tmp_obj->obj_flags.cost = 10;
  tmp_obj->obj_flags.cost_per_day = 1;

  tmp_obj->next = object_list;
  object_list = tmp_obj;

  obj_to_room(tmp_obj,ch->in_room);

  tmp_obj->item_number = -1;

  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_CHAR);
}



void spell_create_water(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int water;

  extern struct weather_data weather_info;
	void name_to_drinkcon(struct obj_data *obj,int type);
	void name_from_drinkcon(struct obj_data *obj);

  assert(ch && obj);

  if (GET_ITEM_TYPE(obj) == ITEM_DRINKCON) {
    if ((obj->obj_flags.value[2] != LIQ_WATER)
	&& (obj->obj_flags.value[1] != 0)) {
      
      name_from_drinkcon(obj);
      obj->obj_flags.value[2] = LIQ_SLIME;
      name_to_drinkcon(obj, LIQ_SLIME);
      
    } else {
      
      water = 2*level * ((weather_info.sky >= SKY_RAINING) ? 2 : 1);

      /* Calculate water it can contain, or water created */
      water = MIN(obj->obj_flags.value[0]-obj->obj_flags.value[1], water);

      if (water > 0) {
	obj->obj_flags.value[2] = LIQ_WATER;
	obj->obj_flags.value[1] += water;
	
	weight_change_object(obj, water);
	
	name_from_drinkcon(obj);
	name_to_drinkcon(obj, LIQ_WATER);
	act("$p is partially filled.", FALSE, ch,obj,0,TO_CHAR);
      }
    }
  }
}



void spell_cure_blind(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  /* never make this to remove SUN_BLIND!! */
  if (IS_AFFECTED(victim, AFF_BLIND)) {
    REMOVE_BIT(victim->specials.affected_by, AFF_BLIND);
    send_to_char("Your vision returns!\n\r", victim);
  }

  if (affected_by_spell(victim, SPELL_BLINDNESS)) {
    affect_from_char(victim, SPELL_BLINDNESS);    
    send_to_char("Your vision returns!\n\r", victim);
    act("$n restores $N's vision!",TRUE,ch,0,victim,TO_ROOM);
    if (IS_PC(ch) && IS_PC(victim))
      GET_ALIGNMENT(ch)+=1;
  }
}



void spell_cure_critic(byte level, struct char_data *ch,
       struct char_data *victim, struct obj_data *obj)
{
  int healpoints;

  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));
  
  healpoints = dice(3,8)+3;
  
  if ( (healpoints + GET_HIT(victim)) > hit_limit(victim) )
    GET_HIT(victim) = hit_limit(victim);
  else
    GET_HIT(victim) += healpoints;

  send_to_char("You feel much better!\n\r", victim);

  if(ch != victim) {
    act("You make $N feel much better.",TRUE,ch,0,victim,TO_CHAR);
    if(IS_EVIL(ch)) 
      act("$n's hands glow red as $e lays hands on $N",
	  TRUE,ch,0,victim,TO_NOTVICT);
    else if(IS_GOOD(ch))
      act("$n's hands glow white as $e lays hands on $N",
	  TRUE,ch,0,victim,TO_NOTVICT);
    else				/* must be neutral */
      act("$n's hands glow blue as $e lays hands on $N",
	  TRUE,ch,0,victim,TO_NOTVICT);
  } else {
    if(IS_EVIL(ch))
      act("$n's wounds strongly glow red and are healed!",
          TRUE,ch,0,victim,TO_NOTVICT);
    else if(IS_GOOD(ch))
      act("$n's wounds strongly glow white and are healed!",
          TRUE,ch,0,victim,TO_NOTVICT);
    else                                /* must be neutral */
      act("$n's wounds strongly glow blue and are healed!",
          TRUE,ch,0,victim,TO_NOTVICT);
  }
    
  update_pos(victim);

  /* pc's won't know this, but I will :) */
  
  if(!number(0,2)) {
    if(IS_EVIL(victim) && IS_GOOD(ch))
      GET_ALIGNMENT(ch) -=1;
    else if(IS_GOOD(victim) && IS_EVIL(ch))
      GET_ALIGNMENT(ch) +=1;
  }
}


void spell_cure_light(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int healpoints;

  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  healpoints = dice(1,8);

  if ( (healpoints + GET_HIT(victim)) > hit_limit(victim) )
    GET_HIT(victim) = hit_limit(victim);
  else
    GET_HIT(victim) += healpoints;

  send_to_char("You feel a little better!\n\r", victim);

  if(ch != victim) {
    act("You make $N feel much better.",TRUE,ch,0,victim,TO_CHAR);
    act("$n recites a soothing prayer for $N",
	TRUE,ch,0,victim,TO_NOTVICT);
  } else {
    if(IS_EVIL(ch))
      act("$n's wounds faintly glow red and are healed!",
          TRUE,ch,0,victim,TO_NOTVICT);
    else if(IS_GOOD(ch))
      act("$n's wounds faintly glow white and are healed!",
          TRUE,ch,0,victim,TO_NOTVICT);
    else                                /* must be neutral */
      act("$n's wounds faintly glow blue and are healed!",
          TRUE,ch,0,victim,TO_NOTVICT);
  }
  update_pos(victim);
  /* trivial spell, never should affect alignments */
}





void spell_curse(byte level, struct char_data *ch,
	 struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim || obj);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if (obj) {
    SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD);
    SET_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);

    /* LOWER ATTACK DICE BY -1 */
    if(obj->obj_flags.type_flag == ITEM_WEAPON)
      obj->obj_flags.value[2]--;
    act("$p glows polkadotted.  (very bad indeed)",
	FALSE, ch, obj, 0, TO_CHAR);
  } else {			/* must be a victim */
    if ( saves_spell(victim, SAVING_SPELL)) {
      act("$N resists your attempt to curse $M.",
	  FALSE, ch, 0, victim, TO_CHAR);
      return;
    }
    
    if (affected_by_spell(victim, SPELL_CURSE)) {
      act("$N curses you, but nothing new seems to happen.",TRUE, ch, 0,
	  victim,TO_CHAR);
      send_to_char("Nothing new seems to happen.\n\r",ch);
      return;
    }
    
    af.type      = SPELL_CURSE;
    af.duration  = 24*7;       /* 7 Days */
    af.modifier  = (int) -1 * (level / 10);
    af.location  = APPLY_HITROLL;
    af.bitvector = AFF_CURSE;
    affect_to_char(victim, &af);

    af.location = APPLY_DAMROLL; /* hee hee */
    affect_to_char(victim, &af);

    af.location = APPLY_SAVING_PARA;
    af.modifier = 1; /* Make worse */
    affect_to_char(victim, &af);

    if(ch != victim) {

    act("$n curses $N, $E is surrounded by a malignant aura!",
	TRUE, ch, 0, victim, TO_NOTVICT);
    act("$n curses you.  You are enveloped in a malignant aura.",
	TRUE, ch, 0, victim, TO_VICT);
    act("You curse $N!",FALSE, ch, 0, victim, TO_CHAR);
  } else {
    act("$n is sorrounded in a malignant aura!",
	TRUE,ch,0,0,TO_NOTVICT);
    send_to_char("You are surrounded in a malignant aura!",ch);
  }
    
    if (IS_NPC(victim) && !victim->specials.fighting)
      set_fighting(victim,ch);
    
    /* again, cursing evil in the name of a holy god is good */
    
    if (IS_PC(ch) && IS_PC(victim)) {
      if(IS_EVIL(ch) && IS_GOOD(victim))
	GET_ALIGNMENT(ch)-=2;
      else if(IS_GOOD(ch) && IS_EVIL(victim))
	GET_ALIGNMENT(ch)+=2;
    }
  }
}



void spell_detect_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));
  
  if ( affected_by_spell(victim, SPELL_DETECT_EVIL) ) {
    send_to_char("Nothing new seems to happen.\n\r",ch);
    return;
  }
  
  af.type      = SPELL_DETECT_EVIL;
  af.duration  = level*5;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_DETECT_EVIL;
  
  affect_to_char(victim, &af);

  act("$n's eyes briefly glow.", FALSE, victim, 0, 0, TO_NOTVICT);
  send_to_char("Your eyes tingle.\n\r", victim);
  
}



void spell_detect_invisibility(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if ( affected_by_spell(victim, SPELL_DETECT_INVISIBLE) ) {
    send_to_char("Nothing new seems to happen.\n\r",ch);
    return;
  }

  af.type      = SPELL_DETECT_INVISIBLE;
  af.duration  = level*5;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_DETECT_INVISIBLE;

  affect_to_char(victim, &af);
  act("$n's eyes briefly glow yellow.", FALSE, victim, 0, 0, TO_ROOM);
  send_to_char("Your eyes tingle.\n\r", victim);
}



void spell_detect_magic(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if ( affected_by_spell(victim, SPELL_DETECT_MAGIC) ) {
    send_to_char("Nothing new seems to happen.\n\r",ch);
    return;
  }

  af.type      = SPELL_DETECT_MAGIC;
  af.duration  = level*5;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_DETECT_MAGIC;

  affect_to_char(victim, &af);

  act("$n's eyes briefly glow.",TRUE, victim, 0, 0, TO_ROOM);
  send_to_char("Your eyes tingle.\n\r", victim);
}



void spell_detect_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
	assert(ch && (victim || obj));

  if (victim) {
    if (victim == ch)
      if (IS_AFFECTED(victim, AFF_POISON))
        send_to_char("You can sense poison in your blood.\n\r", ch);
      else
        send_to_char("You feel healthy.\n\r", ch);
    else
      if (IS_AFFECTED(victim, AFF_POISON)) {
        act("You sense that $E is poisoned.",FALSE,ch,0,victim,TO_CHAR);
      } else {
        act("You sense that $E is poisoned",FALSE,ch,0,victim,TO_CHAR);
      }
  } else { /* It's an object */
    if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
        (obj->obj_flags.type_flag == ITEM_FOOD)) {
      if (obj->obj_flags.value[3])
        act("Poisonous fumes are revealed.",FALSE, ch, 0, 0, TO_CHAR);
      else
        send_to_char("It looks very delicious.\n\r", ch);
    }
  }
}



void spell_enchant_weapon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int i;
  int count=0;
  
  assert(ch && obj);
  assert(MAX_OBJ_AFFECT >= 2);
  
  if ((GET_ITEM_TYPE(obj) == ITEM_WEAPON) &&
      !IS_SET(obj->obj_flags.extra_flags, ITEM_MAGIC)) {
    
    for (i=0; i < MAX_OBJ_AFFECT; i++) {
      if (obj->affected[i].location == APPLY_NONE) 
	count++;
      if (obj->affected[i].location == APPLY_HITNDAM ||
          obj->affected[i].location == APPLY_HITROLL ||
          obj->affected[i].location == APPLY_DAMROLL) {
	send_to_char("This weapon can hold no further enchantments.",ch);
	return;
      }
    }
    if (count < 2) {
      send_to_char("This weapon can hold no further enchantments.",ch);
      return;
    }
      /*  find the slots */
    i = getFreeAffSlot(obj);
    
    SET_BIT(obj->obj_flags.extra_flags, ITEM_MAGIC);
    
    obj->affected[i].location = APPLY_HITROLL;
    obj->affected[i].modifier = 1;
    if (level > 20)
      obj->affected[i].modifier += 1;
    if (level > 40)
      obj->affected[i].modifier += 1;
    if (level > MAX_MORT)
      obj->affected[i].modifier += 1;
    if (level >=  ADMIN )	/* :P */
      obj->affected[i].modifier += 1;
    
    i = getFreeAffSlot(obj);
    
    obj->affected[i].location = APPLY_DAMROLL;		
    obj->affected[i].modifier = 1;
    if (level > 15)
      obj->affected[i].modifier += 1;
    if (level > 30)
      obj->affected[i].modifier += 1;
    if (level > MAX_MORT)
      obj->affected[i].modifier += 1;
    if (level >= ADMIN)
      obj->affected[i].modifier += 1;
    
    if (IS_GOOD(ch)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_EVIL|ITEM_ANTI_NEUTRAL);
      act("$p glows blue.",FALSE,ch,obj,0,TO_CHAR);
    } else if (IS_EVIL(ch)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD|ITEM_ANTI_NEUTRAL);
      act("$p glows red.",FALSE,ch,obj,0,TO_CHAR);
    } else {
      act("$p glows yellow.",FALSE,ch,obj,0,TO_CHAR);
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD|ITEM_ANTI_EVIL);
    }
  }
}

void spell_enchant_armor(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int i;
  int count=0;
  
  assert(ch && obj);
  assert(MAX_OBJ_AFFECT >= 2);
  
  if ((GET_ITEM_TYPE(obj) == ITEM_ARMOR) &&
      !IS_SET(obj->obj_flags.extra_flags, ITEM_MAGIC)) {
    
    for (i=0; i < MAX_OBJ_AFFECT; i++) {
      if (obj->affected[i].location == APPLY_NONE) 
	count++;
      if (obj->affected[i].location == APPLY_ARMOR ||
          obj->affected[i].location == APPLY_SAVE_ALL ||
          obj->affected[i].location == APPLY_SAVING_PARA ||
          obj->affected[i].location == APPLY_SAVING_ROD ||
          obj->affected[i].location == APPLY_SAVING_PETRI ||
          obj->affected[i].location == APPLY_SAVING_BREATH ||
          obj->affected[i].location == APPLY_SAVING_SPELL ||
          obj->affected[i].location == APPLY_SAVE_ALL) {
	send_to_char("This item may not hold further enchantments.\n\r", ch);
	return;
      }
    }

    if (count < 2) {
      send_to_char("This item may not be enchanted.\n\r", ch);
      return;
    }
    /*  find the slots */
    i = getFreeAffSlot(obj);
    
    SET_BIT(obj->obj_flags.extra_flags, ITEM_MAGIC);
    
    obj->affected[i].location = APPLY_ARMOR;
    obj->affected[i].modifier = -1;
    if (level > 20)
      obj->affected[i].modifier -= 1;
    if (level > 40)
      obj->affected[i].modifier -= 1;
    if (level > MAX_MORT)
      obj->affected[i].modifier -= 1;
    if (level >= ADMIN)
      obj->affected[i].modifier -= 1;
    
    i = getFreeAffSlot(obj);
    
    obj->affected[i].location = APPLY_SAVE_ALL;		
    obj->affected[i].modifier = 0;
    if (level > 30)
      obj->affected[i].modifier -= 1;
    if (level > MAX_MORT)
      obj->affected[i].modifier -= 1;
    if (level >= ADMIN)
      obj->affected[i].modifier -= 1;
    
    if (IS_GOOD(ch)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_EVIL|ITEM_ANTI_NEUTRAL);
      act("$p glows blue.",FALSE,ch,obj,0,TO_CHAR);
    } else if (IS_EVIL(ch)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD|ITEM_ANTI_NEUTRAL);
      act("$p glows red.",FALSE,ch,obj,0,TO_CHAR);
    } else {
      act("$p glows yellow.",FALSE,ch,obj,0,TO_CHAR);
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD|ITEM_ANTI_EVIL);
    }
  }
}



void spell_heal(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  assert(victim);

  spell_cure_blind(level, ch, victim, obj);

  GET_HIT(victim) += 100;

  if (GET_HIT(victim) >= hit_limit(victim))
    GET_HIT(victim) = hit_limit(victim)-dice(1,4);

  update_pos( victim );

  send_to_char("A warm feeling fills your body.\n\r", victim);

  /* if (IS_PC(ch) && IS_PC(victim)) BAH, healing evil people is bad.*/ 
  
  if(IS_GOOD(victim))
    GET_ALIGNMENT(ch)+=5;
  else if(IS_EVIL(victim))
    GET_ALIGNMENT(ch)-=5;

  /* healing neutral people does what? bring you closer to neutrality? */

}


void spell_invisibility(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert((ch && obj) || victim);

  if (obj) {
    if ( !IS_SET(obj->obj_flags.extra_flags, ITEM_INVISIBLE) ) {
      act("$p turns invisible.",FALSE,ch,obj,0,TO_CHAR);
      act("$p turns invisible.",TRUE,ch,obj,0,TO_ROOM);
      SET_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);
		}
  } else {              /* Then it is a PC | NPC */
    if (!affected_by_spell(victim, SPELL_INVISIBLE)) {

      act("$n slowly fades out of existence.", TRUE, victim,0,0,TO_ROOM);
      send_to_char("You vanish.\n\r", victim);

      af.type      = SPELL_INVISIBLE;
      af.duration  = 24;
      af.modifier  = -40;
      af.location  = APPLY_AC;
      af.bitvector = AFF_INVISIBLE;
      affect_to_char(victim, &af);
    }
  }
}


void spell_locate_object(byte level, struct char_data *ch,
			 struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *i;
  char name[256];
  char buf2[256];
  char buf[MAX_STRING_LENGTH];
  int j;

  assert(ch);

  if (!obj) {
    send_to_char("locate what??\n\r",ch);
    return;
  }

  if (!obj->name || !(*obj->name)) {
    send_to_char("which object?\n\r", ch);
    return;
  }
    

  strcpy(name, obj->name);

  j=level>>1;
  buf[0] = '\0';
  for (i = object_list; i && (j>0); i = i->next)
    if (isname(name, i->name)) {
      if(i->carried_by) {
	if (strlen(PERS(i->carried_by, ch))>0) {
          sprintf(buf2,"%s carried by %s.\n\r",
		  i->short_description,PERS(i->carried_by,ch));
          strcat(buf, buf2);
	}
      } else if(i->equipped_by) {
	if (strlen(PERS(i->equipped_by, ch))>0) {
          sprintf(buf2,"%s equipped by %s.\n\r",
		  i->short_description,PERS(i->equipped_by,ch));
          strcat(buf, buf2);
	}
      } else if (i->in_obj) {
	sprintf(buf2,"%s in %s.\n\r",i->short_description,
		i->in_obj->short_description);
	strcat(buf, buf2);
      } else {
	sprintf(buf2,"%s in %s.\n\r",i->short_description,
		(i->in_room == NOWHERE ? "use but uncertain." : real_roomp(i->in_room)->name));
	strcat(buf, buf2);
	j--;
      }      
    }

  page_string(ch->desc, buf, 0);

  if(j==0)
    send_to_char("You are very confused.\n\r",ch);
  if(j==level>>1)
    send_to_char("No such object.\n\r",ch);
}


void spell_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim || obj);

  /* why on earth did deadly poison only work on npc's? */
  /* where were the checks for resistances and immunities? */
  
  if (victim) {
    
    if(IsImmune(victim,IMM_POISON) || (!IS_SET(ch->specials.act, ACT_DEADLY)
				       && GET_RACE(ch) == RACE_VEGMAN)) {
      act("$N tried to poison you, but you ignore the poison!",
	  TRUE,victim,0,ch,TO_CHAR);
      act("$n tried to poison $N, but $E ignores it!",
	  TRUE,ch,0,victim,TO_NOTVICT);
      act("$n ignores your poison!",
	  TRUE,victim,0,ch,TO_VICT);
      if (!victim->specials.fighting)
	set_fighting(victim, ch);
      return;
    }

    
    if (IS_SET(ch->specials.act, ACT_DEADLY)  && ch != victim) {
      
      /* resistances: fail save = die, make save = normal poison */
      if(IsResist(victim,IMM_POISON)) {	
	
	if(!ImpSaveSpell(victim, SAVING_PARA, 0))    {
	  af.type = SPELL_POISON;
	  af.duration = level*2;
	  af.modifier = -2;
	  af.location = APPLY_STR;
	  af.bitvector = AFF_POISON;
	  
	  affect_join(victim, &af, FALSE, FALSE);
	  
	  send_to_char("You feel very sick.\n\r", victim);
	  if (!victim->specials.fighting)
	    set_fighting(victim, ch);
	  return;
	} else {		/* made save */
	  act("$N tried to poison you with deadly poison, but you resist it!",
	      TRUE,victim,0,ch,TO_CHAR);
	  act("$n tries to poison $N, but $E resists it!",
	      TRUE,ch,0,victim,TO_NOTVICT);
	  act("$n resists your deadly poison!",
	      TRUE,victim,0,ch,TO_VICT);
	  if (!victim->specials.fighting)
	    set_fighting(victim, ch);
	  return;
	}
      }
      /* not resistant, not immune, toast? */

      if(!ImpSaveSpell(victim, SAVING_PARA, 0))    {
	act("Deadly poison fills your veins.",TRUE, victim, 0, 0, TO_CHAR);
	act("$n fills $N's veins with deadly poison!",
	    TRUE,ch,0,victim,TO_NOTVICT);
	act("You kill $N with your deadly poison!",TRUE,ch,0,victim,TO_CHAR);
	damage(victim, victim, MAX(10, GET_HIT(victim)*2), SPELL_POISON);
	
      } else {			/* failed save, affect them w/normal */
	
	af.type = SPELL_POISON;
	af.duration = level*2;
	af.modifier = -2;
	af.location = APPLY_STR;
	af.bitvector = AFF_POISON;
	
	affect_join(victim, &af, FALSE, FALSE);

	act("Deadly poison fills your veins, luckily you resist.",
	    TRUE, victim, 0, 0, TO_CHAR);
        act("$n fills $N's veins with deadly poison, but $E luckily resists!!",
            TRUE,ch,0,victim,TO_NOTVICT);
        act("$N luckily resists your deadly poison!",TRUE,ch,0,victim,TO_CHAR);

	if (!victim->specials.fighting)
	  set_fighting(victim, ch);
      }
    } else {			/* normal poison */
      
      if(IsResist(victim,IMM_POISON)) {
	if(ImpSaveSpell(victim, SAVING_PARA, 0)) {
	  act("$N tried to poison you, but you resist it!",
              TRUE,victim,0,ch,TO_CHAR);
          act("$n tries to poison $N, but $E resists it!",
              TRUE,ch,0,victim,TO_NOTVICT);
          act("$n resists your poison!",
              TRUE,victim,0,ch,TO_VICT);
          if (!victim->specials.fighting)
            set_fighting(victim, ch);
          return;
	}
      }
      
      if(!ImpSaveSpell(victim, SAVING_PARA, 0))    {
	af.type = SPELL_POISON;
	af.duration = level*2;
	af.modifier = -2;
	af.location = APPLY_STR;
	af.bitvector = AFF_POISON;
	
	affect_join(victim, &af, FALSE, FALSE);
	
        act("Poison fills your veins.", TRUE, victim, 0, 0, TO_CHAR);
        act("$n fills $N's veins with poison.",TRUE,ch,0,victim,TO_NOTVICT);
        act("$N succumbs to your poison!",TRUE,ch,0,victim,TO_CHAR);
	
	if (!victim->specials.fighting)
          set_fighting(victim, ch);
      } else {
	
	act("$N tried to poison you.",
	    TRUE,victim,0,ch,TO_CHAR);
	act("$n tries to poison $N, but it fails!",
	    TRUE,ch,0,victim,TO_NOTVICT);
	act("$n avoids your poison!",
	    TRUE,victim,0,ch,TO_VICT);
	if (!victim->specials.fighting)
	  set_fighting(victim, ch);
	return;
      } 
    }
  } else { /* Object poison */
    if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
        (obj->obj_flags.type_flag == ITEM_FOOD)) {
      obj->obj_flags.value[3] = 1;
    }
  }
}


void spell_protection_from_evil(byte level, struct char_data *ch,
	 struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

	assert(victim);

  if (!affected_by_spell(victim, SPELL_PROTECT_FROM_EVIL) ) {
    af.type      = SPELL_PROTECT_FROM_EVIL;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You have a righteous feeling!\n\r", victim);
    if(ch != victim) {
      act("$n encases $N in a faint golden glow.",TRUE,ch,0,victim,TO_NOTVICT);
      act("You encase $N in a faint golden glow.",TRUE,ch,0,victim,TO_CHAR);
    } else {
      act("$n is encased in a faint golden glow.",TRUE,ch,0,0,TO_CHAR);
    }
  }
}


void spell_remove_curse(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && (victim || obj));

  if (obj) {
    if (IS_SET(obj->obj_flags.extra_flags, ITEM_NODROP)) {
      act("$p briefly glows blue.", TRUE, ch, obj, 0, TO_CHAR);
      REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
    }
  } else {      /* Then it is a PC | NPC */
    if (affected_by_spell(victim, SPELL_CURSE) ) {
      act("$n briefly glows red, then blue.",FALSE,victim,0,0,TO_ROOM);
      act("You feel better.",FALSE,victim,0,0,TO_CHAR);
      affect_from_char(victim, SPELL_CURSE);
      if (IS_PC(ch) && IS_PC(victim))
	GET_ALIGNMENT(ch)+=2;
    }
  }
}


void spell_remove_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && (victim || obj));

  if (victim) {
    if(affected_by_spell(victim,SPELL_POISON)) {
      affect_from_char(victim,SPELL_POISON);
      act("You remove the poison from $N's veins.",TRUE,ch,0,victim,TO_CHAR);
      act("A warm feeling runs through your body.",FALSE,victim,0,0,TO_CHAR);
      act("$N looks better.",FALSE,ch,0,victim,TO_ROOM);
    }
  } else {
    if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
        (obj->obj_flags.type_flag == ITEM_FOOD)) {
      obj->obj_flags.value[3] = 0;
      act("The $p steams briefly.",FALSE,ch,obj,0,TO_CHAR);
    }
  }
}



void spell_fireshield(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (!affected_by_spell(victim, SPELL_FIRESHIELD) ) {

    act("$n is surrounded by a glowing red aura.",TRUE,victim,0,0,TO_ROOM);
    act("You start glowing red.",TRUE,victim,0,0,TO_CHAR);
    
    af.type      = SPELL_FIRESHIELD;
    af.duration  = (level<LOW_IMMORTAL) ? 3 : level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_FIRESHIELD;
    affect_to_char(victim, &af);
  }
}

void spell_sanctuary(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  if ((!affected_by_spell(victim, SPELL_SANCTUARY)) && 
      (!IS_AFFECTED(victim, AFF_SANCTUARY))) {

    act("$n is surrounded by a white aura.",TRUE,victim,0,0,TO_ROOM);
    act("You start glowing.",TRUE,victim,0,0,TO_CHAR);
    
    af.type      = SPELL_SANCTUARY;
    af.duration  = (level<LOW_IMMORTAL) ? 3 : level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char(victim, &af);
  }
}



void spell_sleep(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
	assert(victim);
  
  if (IsImmune(victim, IMM_SLEEP)) {
    FailSleep(victim, ch);
    return;
  }
  if (IsResist(victim, IMM_SLEEP)) {
    if (saves_spell(victim, SAVING_SPELL)) {
      FailSleep(victim, ch);
      return;
    }
    if (saves_spell(victim, SAVING_SPELL)) {
      FailSleep(victim, ch);
      return;
    }
  } else if (!IsSusc(victim, IMM_SLEEP)) {
    if (saves_spell(victim, SAVING_SPELL)) {
      FailSleep(victim, ch);
      return;
    }    
  }

  af.type      = SPELL_SLEEP;
  af.duration  = 4+level;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_SLEEP;
  affect_join(victim, &af, FALSE, FALSE);
  
  if (GET_POS(victim)>POSITION_SLEEPING)    {
    act("You feel very sleepy ..... zzzzzz",FALSE,victim,0,0,TO_CHAR);
    act("$N puts $n to sleep.",TRUE,victim,0,ch,TO_ROOM);
    GET_POS(victim)=POSITION_SLEEPING;
  }
}



void spell_strength(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  

  if (!affected_by_spell(victim,SPELL_STRENGTH)) {
     act("Your muscles bulge.", FALSE, victim,0,0,TO_CHAR);
     act("$n's muscles bulge!!\n\r",
	 FALSE, victim, 0, 0, TO_ROOM);
     af.type      = SPELL_STRENGTH;
     af.duration  = 2*level;
     if (IS_NPC(victim))
       if (level >= CREATOR) {
	 af.modifier = 25 - GET_STR(victim);
       } else
	 af.modifier = number(1,6);
     else {
       
       if (HasClass(ch, CLASS_WARRIOR)) 
	 af.modifier = number(1,8);
       else if (HasClass(ch, CLASS_CLERIC) ||
		HasClass(ch, CLASS_THIEF))
	 af.modifier = number(1,6);
       else 
	 af.modifier = number(1,4);
     }
     af.location  = APPLY_STR;
     af.bitvector = 0;
     affect_to_char(victim, &af);
   } else {
     
     act("Nothing seems to happen.", FALSE, ch,0,0,TO_CHAR);
     
   }
}



void spell_ventriloquate(byte level, struct char_data *ch,
 struct char_data *victim, struct obj_data *obj)
{
  /* Not possible!! No argument! */
}



void spell_word_of_recall(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  extern int top_of_world;
  int location;

  void do_look(struct char_data *ch, char *argument, int cmd);

  assert(victim);

  if (IS_NPC(victim))
     return;

  /*  loc_nr = GET_HOME(ch); */

  if (victim->player.hometown) {
    location = victim->player.hometown;
  } else {
    location = 3001;
  }

  if (!real_roomp(location))    {
    send_to_char("You are completely lost.\n\r", victim);
    location = 0;
    return;
  }

  if (!IsOnPmp(victim->in_room)) {
    send_to_char("You can't recall!, you're on a different plane!\n\r", 
		 victim);
    return;
  }

	/* a location has been found. */

  act("$n disappears.", TRUE, victim, 0, 0, TO_ROOM);
  char_from_room(victim);
  char_to_room(victim, location);
  act("$n appears in the middle of the room.", TRUE, victim, 0, 0, TO_ROOM);
  do_look(victim, "",15);

}


void spell_summon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct char_data *tmp;
  struct room_data *rp;
  int count;

  assert(ch && victim);

  if (victim->in_room <= NOWHERE) {
    send_to_char("Couldn't find any of those.\n\r", ch);
    return;
  }

  if ((rp = real_roomp(ch->in_room)) == NULL)
    return;

  if (IS_SET(rp->room_flags, NO_SUM) || IS_SET(rp->room_flags, NO_MAGIC)) {
    send_to_char("Eldritch wizardry obstructs thee.\n\r", ch);
    return;
  }

  if (IS_SET(rp->room_flags, TUNNEL)) {
    send_to_char("There is no room in here to summon!\n\r", ch);
    return;
  }
  
  if ((rp->sector_type == SECT_AIR) || rp->sector_type == SECT_UNDERWATER) {
    send_to_char("Strange powers block your summons\n", ch);
    return;
  }

  if (check_peaceful(ch, "Ancient powers obstruct thy magik\n"))
    return;

  if (check_peaceful(victim, "")) {
    send_to_char("You cannot get past the magical defenses.\n\r", ch);
    return;
  }

  if (IS_SET(real_roomp(victim->in_room)->room_flags, NO_SUM)) {
    send_to_char("Ancient Magiks bar your path.\n\r", ch);
    return;
  }
  
  if (victim->specials.fighting) {
    send_to_char("You can't get a clear fix on them\n", ch);
    return;
  }

  if(GetPlane(ch) != GetPlane(victim)) {
    send_to_char("Your magic is not powerful enough to reach that far.\n\r",
		 ch);
    return;
  }

  if(!IsInSameZone(ch,victim)) {
    send_to_char("Phew, that was tough.  You didn't realize how far away they were.\n\r",ch);
    GET_MANA(ch) -= 10;
    if(GET_MANA(ch) < 0)
      GET_MANA(ch) = 0;
  }
  
  if (!IS_PC(victim)) {
    count = 0;
    for (tmp=real_roomp(victim->in_room)->people; 
	 tmp; tmp = tmp->next_in_room) {
      count++;
    }
    
    if (count==0) {
      send_to_char("You failed.\n\r", ch);
      return;
    } else {
      count = number(0,count);
      
      for (tmp=real_roomp(victim->in_room)->people; 
	   count && tmp;
	   tmp = tmp->next_in_room, count--)
	;
      
      if (tmp) {
	RawSummon(tmp, ch);
	AddHated(tmp,ch);
      } else {
	send_to_char("You failed.\n\r", ch);
	return;
      }
    }
  } else {
    RawSummon(victim, ch);
  }

}


void RawSummon( struct char_data *v, struct char_data *c)
{
  sh_int target;
  struct char_data *tmp;
  struct obj_data *o, *n;
  int    j, i;
  extern char EasySummon;
  char buf[400];  

  if (IS_NPC(v) && (!IS_SET(v->specials.act, ACT_POLYSELF)) &&
      (GetMaxLevel(v) > GetMaxLevel(c)+3)) { 
    act("$N struggles, and all of $S items are destroyed!", TRUE, c, 0, v, TO_CHAR);
    /* remove objects from victim */
    for (j = 0; j < MAX_WEAR; j++) {
      if (v->equipment[j]) {
	o = unequip_char(v, j);
	extract_obj(o);
      }
    }
    for (o = v->carrying; o; o = n) {
      n = o->next_content;
      obj_from_char(o);
      extract_obj(o);
    }
    AddHated(v, c);
  } else {
    if (!EasySummon) {
      send_to_char("A wave of nausea overcomes you.  You collapse!\n\r",c);
      WAIT_STATE(c, PULSE_VIOLENCE*6);
      GET_POS(c) = POSITION_STUNNED;
    }
  }
  
  act("$n disappears suddenly.",TRUE,v,0,0,TO_ROOM);
  target = c->in_room;
  char_from_room(v);
  char_to_room(v,target);
  
  act("$n arrives suddenly.",TRUE,v,0,0,TO_ROOM);
  
  sprintf(buf, "%s has summoned you!\n\r", (IS_NPC(c)?c->player.short_descr:GET_NAME(c)));
  send_to_char(buf, v);
  do_look(v,"",15);
  
  for (tmp = real_roomp(v->in_room)->people; tmp; tmp = tmp->next_in_room) {
    if (IS_NPC(tmp) && !(IS_SET(tmp->specials.act,ACT_POLYSELF)) &&
	((IS_SET(tmp->specials.act, ACT_AGGRESSIVE) ||
	  (IS_SET(tmp->specials.act, ACT_META_AGG))))) {
      act("$n growls at you.", 1, tmp, 0, c, TO_VICT);
      act("$n growls at $N.", 1, tmp, 0, c, TO_NOTVICT);
      i = number(0,6);
      if (i==0) {
	if (can_see(tmp, c)) {
	  hit(tmp, c, TYPE_UNDEFINED);
	}
      }
    }
  } 
}


void spell_charm_person(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  void add_follower(struct char_data *ch, struct char_data *leader);
  bool circle_follow(struct char_data *ch, struct char_data *victim);
  void stop_follower(struct char_data *ch);

  assert(ch && victim);

  if (victim == ch) {
    send_to_char("You like yourself even better!\n\r", ch);
    return;
  }
  
  if (!IS_AFFECTED(victim, AFF_CHARM) && !IS_AFFECTED(ch, AFF_CHARM)) {
    if (circle_follow(victim, ch)) {
      send_to_char("Sorry, following in circles can not be allowed.\n\r", ch);
      return;
    }
    
    if (!IsPerson(victim)) {
      send_to_char("Umm,  that's not a person....\n\r",ch);
      return;
    }
    
    
    if (GetMaxLevel(victim) > GetMaxLevel(ch)+3) {
      FailCharm(victim, ch);
      return;
    }
    
    if (too_many_followers(ch)) {
      act("$N takes one look at the size of your posse and justs says no!",
	  TRUE, ch, ch->equipment[WIELD], victim, TO_CHAR);
      act("$N takes one look at the size of $n's posse and just says no!",
	  TRUE, ch, ch->equipment[WIELD], victim, TO_ROOM);
      return;
    }

    if (IsImmune(victim, IMM_CHARM) || (WeaponImmune(victim))) {
      FailCharm(victim,ch);
      return;
    }

    if (IsResist(victim, IMM_CHARM)) {
      if (saves_spell(victim, SAVING_PARA)) {
	FailCharm(victim,ch);
	return;
      }
      
      if (saves_spell(victim, SAVING_PARA)) {
	FailCharm(victim,ch);
	return;
      }
    } else {
      if (!IsSusc(victim, IMM_CHARM)) {
	if (saves_spell(victim, SAVING_PARA)) {
	  FailCharm(victim,ch);
	  return;
	}
      }
    }

    if (victim->master)
      stop_follower(victim);
    
    add_follower(victim, ch);
    
    af.type      = SPELL_CHARM_PERSON;
    
    if (GET_CHR(ch))
      af.duration  = follow_time(ch);
    else
      af.duration  = 24*18;
    
    if (IS_GOOD(victim) && IS_GOOD(ch))
      af.duration *= 2;
    if (IS_EVIL(victim) && IS_EVIL(ch))
      af.duration  += af.duration >> 1;

    
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(victim, &af);
    
    act("Isn't $n just such a nice fellow?",FALSE,ch,0,victim,TO_VICT);

    if (!IS_PC(victim)) {
      REMOVE_BIT(victim->specials.act, ACT_AGGRESSIVE);
      SET_BIT(victim->specials.act, ACT_SENTINEL);
    }
  }
}



void spell_charm_monster(byte level, struct char_data *ch,
			 struct char_data *victim, struct obj_data *obj)
{
  char buf[MAX_INPUT_LENGTH];
  struct affected_type af;
  
  void add_follower(struct char_data *ch, struct char_data *leader);
  bool circle_follow(struct char_data *ch, struct char_data *victim);
  void stop_follower(struct char_data *ch);
  
  assert(ch && victim);

  if (victim == ch) {
    send_to_char("You like yourself even better!\n\r", ch);
    return;
  }

  if (IsVeggie(victim)) {
    send_to_char("You can't charm a plant-creature!\n\r", ch);
    return;
  }

  if (GetMaxLevel(victim) > GetMaxLevel(ch)+3) {
    FailCharm(victim, ch);
    return;
  }

  if (too_many_followers(ch)) {
    act("$N takes one look at the size of your posse and justs says no!",
	TRUE, ch, 0, victim, TO_CHAR);
    act("$N takes one look at the size of $n's posse and just says no!",
	TRUE, ch, 0, victim, TO_ROOM);
    return;
  }
  
  if (!IS_AFFECTED(victim, AFF_CHARM) && !IS_AFFECTED(ch, AFF_CHARM)) {
    if (circle_follow(victim, ch)) {
      send_to_char("Sorry, following in circles can not be allowed.\n\r", ch);
      return;
    }
      if (IsImmune(victim, IMM_CHARM) || (WeaponImmune(victim))) {
          FailCharm(victim,ch);
       	  return;
      }
      if (IsResist(victim, IMM_CHARM)) {
         if (saves_spell(victim, SAVING_PARA)) {
          FailCharm(victim,ch);
       	  return;
	 }

         if (saves_spell(victim, SAVING_PARA)) {
          FailCharm(victim,ch);
       	  return;
	 }
       } else {
          if (!IsSusc(victim, IMM_CHARM)) {
	     if (saves_spell(victim, SAVING_PARA)) {
	        FailCharm(victim,ch);
		return;
	     }
	  }
       }
    
    if (victim->master)
      stop_follower(victim);
    
    add_follower(victim, ch);
    
    af.type      = SPELL_CHARM_PERSON;
    
    if (GET_CHR(ch))
      af.duration  = follow_time(ch);
    else
      af.duration  = 24*18;

    if (IS_GOOD(victim) && IS_GOOD(ch))
      af.duration *= 2;
    if (IS_EVIL(victim) && IS_EVIL(ch))
      af.duration  += af.duration >> 1;

    
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(victim, &af);
    
    act("Isn't $n just such a nice fellow?",FALSE,ch,0,victim,TO_VICT);

    if (!IS_PC(victim)) {
      REMOVE_BIT(victim->specials.act, ACT_AGGRESSIVE);
      SET_BIT(victim->specials.act, ACT_SENTINEL);
    }

  }
}


void spell_sense_life(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

	assert(victim);

  if (!affected_by_spell(victim, SPELL_SENSE_LIFE)) {
    send_to_char("Your feel your awareness improve.\n\r", ch);

    af.type      = SPELL_SENSE_LIFE;
    af.duration  = 5*level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_SENSE_LIFE;
    affect_to_char(victim, &af);
  }

}

/* ***************************************************************************
 *                     Not cast-able spells                                  *
 * ************************************************************************* */


void sprintbit(unsigned long, char *[], char *);

void spell_identify(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  char buf[256+18];
  char buf2[256];
  int i;
  bool found;
  
  struct time_info_data age(struct char_data *ch);
  
  /* Spell Names */
  extern char *spells[];
  
  /* For Objects */
  extern char *item_types[];
  extern char *extra_bits[];
  extern char *apply_types[];
  extern char *affected_bits[];
  extern char *affected_bits2[];
  extern char *immunity_names[];
  
  
  assert(ch && (obj || victim));
  
  if (obj) {
    send_to_char("You feel informed:\n\r", ch);
    
    sprintf(buf, "Object '%s', Item type: ", obj->name);
    sprinttype(GET_ITEM_TYPE(obj),item_types,buf2);
    strcat(buf,buf2); strcat(buf,"\n\r");
    send_to_char(buf, ch);
    
    sprintf(buf, "This item\'s ego is of %s proportions.\n\r",
	    EgoDesc(GET_OBJ_EGO(obj)));
    send_to_char(buf, ch);

    if (obj->obj_flags.bitvector) {
      send_to_char("Item will give you following abilities:  ", ch);
      sprintbit((unsigned)obj->obj_flags.bitvector,affected_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf, ch);
    }
    
    send_to_char("Item is: ", ch);
    sprintbit( (unsigned)obj->obj_flags.extra_flags,extra_bits,buf);
    strcat(buf,"\n\r");
    send_to_char(buf,ch);
    
    sprintf(buf,"Weight: %d\n\r",
	    obj->obj_flags.weight); 
    send_to_char(buf, ch);

    
    switch (GET_ITEM_TYPE(obj)) {
      
    case ITEM_SCROLL : 
    case ITEM_POTION :
      sprintf(buf, "Level %d spells of:\n\r",	obj->obj_flags.value[0]);
      send_to_char(buf, ch);
      if (obj->obj_flags.value[1] >= 1) {
	sprinttype(obj->obj_flags.value[1]-1,spells,buf);
	strcat(buf,"\n\r");
	send_to_char(buf, ch);
      }
      if (obj->obj_flags.value[2] >= 1) {
	sprinttype(obj->obj_flags.value[2]-1,spells,buf);
	strcat(buf,"\n\r");
	send_to_char(buf, ch);
      }
      if (obj->obj_flags.value[3] >= 1) {
	sprinttype(obj->obj_flags.value[3]-1,spells,buf);
	strcat(buf,"\n\r");
	send_to_char(buf, ch);
      }
      break;
      
    case ITEM_WAND : 
    case ITEM_STAFF : 
      sprintf(buf, "Has %d chages, with %d charges left.\n\r",
	      obj->obj_flags.value[1],
	      obj->obj_flags.value[2]);
      send_to_char(buf, ch);
      
      sprintf(buf, "Level %d spell of:\n\r",	obj->obj_flags.value[0]);
      send_to_char(buf, ch);
      
      if (obj->obj_flags.value[3] >= 1) {
	sprinttype(obj->obj_flags.value[3]-1,spells,buf);
	strcat(buf,"\n\r");
	send_to_char(buf, ch);
      }
      break;
      
    case ITEM_WEAPON :
      sprintf(buf, "Damage Dice is '%dD%d'\n\r",
	      obj->obj_flags.value[1],
	      obj->obj_flags.value[2]);
      send_to_char(buf, ch);
      break;
      
    case ITEM_ARMOR :
      sprintf(buf, "AC-apply is %d\n\r",
	      obj->obj_flags.value[0]);
      send_to_char(buf, ch);
      break;
      
    }
    
    found = FALSE;
    
    for (i=0;i<MAX_OBJ_AFFECT;i++) {
      if ((obj->affected[i].location != APPLY_NONE) &&
	  (obj->affected[i].modifier != 0)) {
	if (!found) {
	  send_to_char("Can affect you as :\n\r", ch);
	  found = TRUE;
	}
	
	sprinttype(obj->affected[i].location,apply_types,buf2);
	sprintf(buf,"    Affects : %s By ", buf2);
	send_to_char(buf,ch);
	switch(obj->affected[i].location) {
	case APPLY_M_IMMUNE:
	case APPLY_IMMUNE:
	case APPLY_SUSC:
	   sprintbit(obj->affected[i].modifier,immunity_names,buf2);
	   strcat(buf2,"\n\r");
	   break;
	case APPLY_ATTACKS:
	   sprintf(buf2,"%ld\n\r", obj->affected[i].modifier/10);
	   break;
        case APPLY_WEAPON_SPELL:
	case APPLY_EAT_SPELL:
	   sprintf(buf2,"%s\n\r", spells[obj->affected[i].modifier-1]);
	   break;
	case APPLY_SPELL:
	   sprintbit(obj->affected[i].modifier,affected_bits, buf2);
	   strcat(buf2,"\n\r");
	   break;

	 default:
	   sprintf(buf2,"%ld\n\r", obj->affected[i].modifier);
	   break;
	}
	send_to_char(buf2,ch);

      }
    }
    
  } else {       /* victim */
    
    if (!IS_NPC(victim)) {
      struct time_info_data ma;

      age2(victim, &ma);
      sprintf(buf,"%d Years,  %d Months,  %d Days,  %d Hours old.\n\r",
	      ma.year, ma.month,
	      ma.day, ma.hours);
      send_to_char(buf,ch);
      
      sprintf(buf,"Height %dcm  Weight %dpounds \n\r",
	      GET_HEIGHT(victim), GET_WEIGHT(victim));
      send_to_char(buf,ch);
      
      sprintf(buf,"Armor Class %d\n\r",victim->points.armor);
      send_to_char(buf,ch);

      if (level > 30) {

	sprintf(buf,"Str %d/%d, Int %d, Wis %d, Dex %d, Con %d, Ch %d\n\r",
	GET_STR(victim), GET_ADD(victim),
	GET_INT(victim),
	GET_WIS(victim),
	GET_DEX(victim),
	GET_CON(victim),
	GET_CHR(victim));
	send_to_char(buf,ch);
      }
      
    } else {
      send_to_char("You learn nothing new.\n\r", ch);
    }
  }
  
}


/* ***************************************************************************
 *                     NPC spells..                                          *
 * ************************************************************************* */

void spell_fire_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
	int dam;
	int hpch;
	struct obj_data *burn;

	assert(victim && ch);
	assert((level >= 1) && (level <= ABS_MAX_LVL)); 

	hpch = GET_MAX_HIT(ch);
	hpch *= level;
	hpch /= GetMaxLevel(ch);
	if(hpch<10) hpch=10;

	dam = hpch;

	if ( saves_spell(victim, SAVING_BREATH) )
		dam >>= 1;

	MissileDamage(ch, victim, dam, SPELL_FIRE_BREATH);

	/* And now for the damage on inventory */

/*
  DamageStuff(victim, FIRE_DAMAGE);
*/

       	for (burn=victim->carrying ; 
	     burn && (burn->obj_flags.type_flag!=ITEM_SCROLL) && 
	    (burn->obj_flags.type_flag!=ITEM_WAND) &&
	    (burn->obj_flags.type_flag!=ITEM_STAFF) &&
	    (burn->obj_flags.type_flag!=ITEM_BOAT);
	     burn=burn->next_content) {
	     if (!saves_spell(victim, SAVING_BREATH) ) 	{
       		if (burn)  {
       			act("$o burns",0,victim,burn,0,TO_CHAR);
       			extract_obj(burn);
       		}
	     }
	}
}


void spell_frost_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
	int dam;
	int hpch;
	struct obj_data *frozen;

	assert(victim && ch);
	assert((level >= 1) && (level <= ABS_MAX_LVL)); 

	hpch = GET_MAX_HIT(ch);
	hpch *= level;
	hpch /= GetMaxLevel(ch);
	if(hpch<10) hpch=10;

	dam = hpch;

	if ( saves_spell(victim, SAVING_BREATH) )
		dam >>= 1;

	MissileDamage(ch, victim, dam, SPELL_FROST_BREATH);

	/* And now for the damage on inventory */

       	for (frozen=victim->carrying ; 
       	    frozen && (frozen->obj_flags.type_flag!=ITEM_DRINKCON) && 
	    (frozen->obj_flags.type_flag!=ITEM_POTION);
	    frozen=frozen->next_content) {

       	    if (!saves_spell(victim, SAVING_BREATH) ) {
       	      if (frozen) {
		    act("$o shatters.",0,victim,frozen,0,TO_CHAR);
		    extract_obj(frozen);
	      }
	    }
	}
}


void spell_acid_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  int hpch;
  
  int apply_ac(struct char_data *ch, int eq_pos);
  
  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 
  
  hpch = GET_MAX_HIT(ch);
  hpch *= level;
  hpch /= GetMaxLevel(ch);
  if(hpch<10) hpch=10;
  
  dam = hpch;
  
  if ( saves_spell(victim, SAVING_BREATH) )
    dam >>= 1;
  
  MissileDamage(ch, victim, dam, SPELL_ACID_BREATH);

}


void spell_gas_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
	int dam;
	int hpch;

	assert(victim && ch);
	assert((level >= 1) && (level <= ABS_MAX_LVL)); 

	hpch = GET_MAX_HIT(ch);
	hpch *= level;
	hpch /= GetMaxLevel(ch);
	if(hpch<10) hpch=10;

	dam = hpch;

	if ( saves_spell(victim, SAVING_BREATH) )
		dam >>= 1;

	MissileDamage(ch, victim, dam, SPELL_GAS_BREATH);


}


void spell_lightning_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
	int dam;
	int hpch;

	assert(victim && ch);
	assert((level >= 1) && (level <= ABS_MAX_LVL)); 

	hpch = GET_MAX_HIT(ch);
	hpch *= level;
	hpch /= GetMaxLevel(ch);
	if(hpch<10) hpch=10;

	dam = hpch;

	if ( saves_spell(victim, SAVING_BREATH) )
		dam >>= 1;

	MissileDamage(ch, victim, dam, SPELL_LIGHTNING_BREATH);


}


