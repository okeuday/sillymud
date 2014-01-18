/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement

  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "protos.h"

extern struct time_info_data time_info;  /* In db.c */
extern unsigned char moontype;
extern struct weather_data weather_info;
extern int gSunSet, gSunRise, gMoonRise, gMoonSet; /* weather.c */

struct PolyType DruidList[17] = {
  {"bear",        10, 9024},
  {"spider",     10, 20010},
  {"lamia",      10, 3648},
  {"lizard",     10, 6822},
  {"bear",       12, 9056},	/* 5 */
  {"gator",      12, 9054},
  {"basilisk",   13, 7043},
  {"snog",       14, 27008},
  {"snake",      15, 6517},
  {"spider",     15, 6113},	/* 10 */
  {"lizard",     16, 6505},
  {"allosaurus", 18, 21801},
  {"tiger",      28, 9027},
  {"mulichort",  30, 15830},
  {"tiger",      35, 9055},	/* 15 */
  {"lion",       35, 13718},
  {"salamander", 35, 25506}	/* 17 */
};

/* #define LAST_DRUID_MOB 16, moved to poly.h */

#define VAMPIRE_BAT 3066


void do_changeform(struct char_data *ch, char *argument, int cmd)
{
  char buf[80];
  int mobn, X=LAST_DRUID_MOB, found=FALSE, level;
  struct char_data *mob;
  struct affected_type af;
  
  level = GET_LEVEL(ch, DRUID_LEVEL_IND) ;

  level = (level ? level : GetMaxLevel(ch)); /* for vampires */
  sprintf(buf,"Level %d thing doing a changeform.",level);
  logE(buf);

  one_argument(argument,buf);
  
  if(!*buf) {
    if (HasClass(ch, CLASS_DRUID)) {
      send_to_char("You are able to change into the following forms:\n\r",ch);
      send_to_char("Format: <level> - <name>\n\r",ch);
      send_to_char("     ", ch);
      for(mobn=1;  LAST_DRUID_MOB+1 >= mobn && level >= DruidList[mobn].level;
	  mobn++) {
	sprintf(buf,"%2d - %-15s ",DruidList[mobn].level,DruidList[mobn].name);
	if(!(mobn%3))
	  strcat(buf,"\n\r     ");
	send_to_char(buf, ch);
      }
      send_to_char("\n\rIn order to change into one of these forms enter:\n\r",ch);
      send_to_char("changeform <name>\n\r",ch);
    } else if( GET_RACE(ch) == RACE_VAMPIRE) {
      send_to_char("Vampires may only change into bats.\n\r", ch);
    } else {
      send_to_char("Ack, you can't change into anything!\n\r", ch);
    }
    return;
  }

  if (IS_NPC(ch)) {
    send_to_char("You don't really want to do that.\n\r",ch);
    return;
  }
  
  if (affected_by_spell(ch, SKILL_CHANGE_FORM)) {
    send_to_char("You can only change form once every 8 hours.\n\r", ch);
    return;
  }
  
  if (HasClass(ch, CLASS_DRUID)) {
    if(IS_EVIL(ch) || IS_GOOD(ch)) {
      send_to_char("You unable to change your form.  You have strayed too\n\r",
		   ch);
      send_to_char("far from nature's path to be one with her.\n\r",ch);
      return;
    }
    while (!found) {
      if (DruidList[X].level > level) {
	X--;
      } else {
	if (!str_cmp(DruidList[X].name, buf)) {
	  mobn = DruidList[X].number;
	  found = TRUE;
	} else {
	  X--;
	}
	if (X < 0) {
	  break;
	}
      }
    }
    
    if (!found) {
      send_to_char("You couldn't summon an image of that creature.\n\r", ch);
      return;
    } else {
      mob = read_mobile(mobn, VIRTUAL);
      if (mob) {
	af.type      = SKILL_CHANGE_FORM;
	af.duration  = 8;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = 0;
	
	affect_to_char(ch, &af);
	
	spell_poly_self(level, ch, mob, 0);
      } else {
	send_to_char("You couldn't summon an image of that creature.\n\r", ch);
	return;
      }
    }
  } else if(GET_RACE(ch) == RACE_VAMPIRE) {
    mob = read_mobile(VAMPIRE_BAT, VIRTUAL);
    if(mob) {
   
      mob->points.max_hit += level*2;
      GET_HIT(mob)         = GET_MAX_HIT(mob);
      GET_RACE(ch)         = RACE_VAMPIRE;

      SET_BIT(mob->specials.affected_by, AFF_INFRAVISION);
      
      if(level > 5)
	SET_BIT(mob->specials.affected_by, AFF_SENSE_LIFE);
      if(level > 10)
	SET_BIT(mob->specials.affected_by2, AFF2_ANIMAL_INVIS);
      if(level > 15)
	SET_BIT(mob->specials.affected_by, AFF_TREE_TRAVEL);
      if(level > 20)
	SET_BIT(mob->specials.affected_by, AFF_INVISIBLE);
      if(level > 25)
	SET_BIT(mob->specials.affected_by, AFF_DETECT_INVISIBLE);
      if(level > 30)
	SET_BIT(mob->specials.affected_by, AFF_DETECT_MAGIC);
      if(level > 35)
	SET_BIT(mob->specials.affected_by, AFF_SNEAK);
      if(level > 40)
	SET_BIT(mob->specials.affected_by, AFF_TRUE_SIGHT);
      if(level > 45)
	SET_BIT(mob->specials.affected_by, AFF_GROWTH);

      af.type      = SKILL_CHANGE_FORM;
      af.duration  = 8;
      af.modifier  = 0;
      af.location  = APPLY_NONE;
      af.bitvector = 0;
      
      affect_to_char(ch, &af);
      spell_poly_self(level, ch, mob, 0);
      GET_AC(ch)          -= level*2;
    } else {
      send_to_char("Ack, the bat form is acting screwy, not bats! Bug an imp!\n\r", ch);
      return;
    }
  } else {
    send_to_char("Try as you might, your body does not alter its shape.\n\r",
		 ch);
  }
}

void do_bite(struct char_data *ch, char *argument, int cmd)
{
  char arg[80];
  struct char_data *victim;

  if(GET_RACE(ch) != RACE_VAMPIRE) {
    do_action(ch, argument, cmd);
    return;
  }

  if (check_peaceful(ch, "You feel too peaceful to contemplate feeding.\n\r"))
    return;
  
  only_argument(argument, arg);
  
  if (*arg) {
    victim = get_char_room_vis(ch, arg);
    if (victim) {
      if(GetMaxLevel(victim) >= LOW_IMMORTAL) {
	send_to_char("Immortals should not be considered food.\n\r", ch);
	send_to_char("You think someone is eyeing you for lunch.\n\r", victim);
	return;
      } else if (!IsHumanoid(victim) || IsUndead(victim)) {
	send_to_char("How on earth could you consider feeding on such a thing?\n\r", ch);
	return;
      } else if (victim == ch) {
	send_to_char("Suck your own blood?   I think not.\n\r", ch);
	act("$n contemplates feeding on $mself but decides against it.", 
	    TRUE, ch, 0, victim, TO_ROOM);
	return;
      } else if(IS_AFFECTED(ch, AFF_CHARM) && (ch->master == victim)) {
	send_to_char("Your master would be very unpleased if you tried that!\n\r", ch);
	return;
      }
      if( (!ch->specials.fighting) ) {
	if( (!victim->specials.fighting) ) {
	  if (GET_POS(ch)>=POSITION_STANDING) {
	    if (!ch->equipment[WIELD]) {
	      SET_BIT(ch->specials.affected_by2, AFF2_FEEDING);
	      hit(ch, victim, TYPE_UNDEFINED);
	    } else 
	      send_to_char("You can't be wielding anything to feed!\n\r", ch);
	  } else
	    send_to_char("Hey, why don't you try doing this when on your feet!\n\r", ch);
	} else
	  send_to_char("You can't do this when they are fighting!\n\r", ch);
      } else 
	send_to_char("How can you think about your stomach at a time like this?!?!\n\r", ch);
    } else
      send_to_char("Sink your fangs into who?\n\r", ch);
  } else 
    send_to_char("Sink your fangs into who?\n\r", ch);
}
	  
void MindflayerAttack(struct char_data *ch, struct char_data *v)
{
  int loss;

  if(ch->specials.bitten)
    return;

  if(number(1,100) > (GetMaxLevel(ch)) + GET_DEX(ch))
    return;

  if (v->equipment[WEAR_HEAD]) { /* hey, this bozo has headgear! */

    if(number(1,10) >= (v->equipment[WEAR_HEAD]->obj_flags.value[0])) {

      /* I figure steel (plate) helm = 30% chance of hitting head */

      act("$n almost sucked you with $s tentacles, your headgear blocked it!",
          FALSE, ch, 0, v, TO_VICT);
      act("Your tentacles are blocked by $N's headgear!", FALSE, ch, 0,
          v, TO_CHAR);
      act("$N's tentacles are blocked by $n's headgear!", FALSE, ch, 0,
          v, TO_NOTVICT);
      return;
    }
  }

  if(HitOrMiss(ch, v, CalcThaco(ch)) && GetMaxLevel(v) < LOW_IMMORTAL) {
    /* ha ha, we suck them */
    act("$N screams out in pain as $n's tentacles suck $S energies!", FALSE,
        ch, 0, v, TO_NOTVICT);
    act("$n's tentacles momentarily wrap themselves around your head!", FALSE,
        ch, 0, v, TO_VICT);
    send_to_char("Blinding pain fills your head, and flashes of red dot your vision.\n\r",v);

    if(IS_SET(v->player.class, CLASS_MAGIC_USER | CLASS_CLERIC | CLASS_DRUID))
      send_to_char("You feel weaker.\n\r", v);

    act("You wrap your tentacles around $N's head.", FALSE, ch, 0, v,
        TO_CHAR);

    loss = number(1,GetMaxLevel(ch));

    if(GET_MANA(v) > 0) {

      GET_MANA(ch) += loss;
      GET_MANA(v)  -= loss;

      act("You suck away some of $S magical energies!", FALSE, ch, 0, v,
          TO_CHAR);
    } else {
      act("Blech, $E has no more magical energies for you to drain.", FALSE,
          ch, 0, v, TO_CHAR);
    }

    GET_HIT(v)   -= loss/2;

    update_pos(v);
    if(GET_POS(v) == POSITION_DEAD)
      DamageEpilog(ch,v);
  }
}

void VampireBite(struct char_data *ch, struct char_data *v)
{
  bool to_bite = FALSE;
  float chance;

  /* when a vampire attempts to feed I think that they should not */
  /* go about pummeling their victims with their fists.  Nor should */
  /* they kill someone and eat their corpse.  They should not be armed, */
  /* again, they are trying to subdue and bite.  So, here's the biting */
  /* part :)  */

  if(v->specials.bitten) {	/* vampire's victim */
    chance = GET_STR(v) - GET_STR(ch);
    chance *= 3.4;		/* chance from +51 to -51 */
    chance += GetMaxLevel(v) - GetMaxLevel(ch);
    /* chance is now some # from +100 to -100 */

    /* we bite better at night :) */
    if(time_info.hours < gSunRise && time_info.hours > gSunSet -1)
      chance += number(1,50);	/* chance from +150 to -50 */
   
    /* for avg str diff of 5, 17%, if same level, chance = 17% + [1->50] */
    /* or, about 42% */
    /* for str diff of 8, 27%, if 10 level dif., 37%, chance averages 62% */
    /* NOTE: This is the chance for the vampire to keep ahold of v */

    if(GetMaxLevel(ch) < GetMaxLevel(v)-5) /* life seemed too easy */
      chance+=25;

    if(chance > number(1,100)) {
      act("$N struggles violently and breaks free of $n's grasp!", FALSE,
	  ch, 0, v, TO_NOTVICT);
      act("$N struggles violently and breaks free of your grasp!", FALSE,
	  ch, 0, v, TO_CHAR);
      act("You struggle violently and break free of $n's grasp!", FALSE, 
	  ch, 0, v, TO_VICT);
      v->specials.bitten = FALSE;
      return;
    } else {
      act("$N limply struggles in your grasp.", FALSE, ch, 0, v, TO_CHAR);
      act("$N limply struggles in $n's clutches.",FALSE, ch, 0, v, TO_NOTVICT);
      act("You struggle to free yourself of $n's grasp, but can't.", FALSE,
	  ch, 0, v, TO_VICT);
      to_bite = TRUE;
    }
  } else {			/* attemp to bite */
    if(HitOrMiss(ch, v, CalcThaco(ch))) {
      to_bite = TRUE;
    } else {
      to_bite = FALSE;
      act("$n bares $s long, sharp fangs and almost bites you!", FALSE, ch,
	  0, v, TO_VICT);
      act("$N avoids your fangs!", FALSE, ch, 0, v, TO_CHAR);
      act("$N narrowly avoids $n's bared fangs!", FALSE, ch, 0, v, TO_NOTVICT);
    }
  }

  
  if(to_bite) {
    int loss, ammt;
   
    v->specials.bitten = TRUE;

    loss = number(1,MAX(10, GetMaxLevel(ch)));

    if(GET_HIT(v) > 0) {
      act("You savor the salty taste of $N's blood as you drain it.", FALSE,
	  ch, 0, v, TO_CHAR);
      act("$n sinks $s fangs into $N's neck and drinks deeply.", FALSE, ch, 0,
	  v, TO_NOTVICT);
      act("$n sinks $s fangs into your neck drains away your essence.", FALSE,
	  ch, 0, v, TO_VICT);
      
      gain_condition(ch,FULL, MAX(loss/10,3));

      ammt = number(1,loss);
      
      if(GET_HIT(v) - ammt <= 0)
	ammt = ammt = GET_HIT(v);
      (ammt > 0) ? ammt : 0;
      GET_HIT(v) -= ammt;
      GET_HIT(ch) += ammt;
      
      loss -= ammt;
      
      if(loss > 0) {
	ammt = number(1, loss);
	if(GET_MOVE(v) - ammt <= 0)
	  ammt = GET_MOVE(v);
	ammt = (ammt > 0) ? ammt : 0;
	GET_MOVE(ch) += ammt;
	GET_MOVE(v) -= ammt;
	loss -= ammt;
      }
      
      if(loss > 0) {
	if(GET_MANA(v) - loss <= 0)
	  loss = GET_MANA(v);
	loss = (loss > 0) ? loss : 0;
	GET_MANA(v) -= loss;
	GET_MANA(ch) += loss;
      }
    } else {
      send_to_char("You cannot drain anything more from this lifeless husk.\n\r", ch);
      act("You put $N out of $S misery.", FALSE, ch, 0, v, TO_CHAR);
      act("$n rips out your throat.", FALSE, ch, 0, v, TO_VICT);
      act("$n rips out $N's throat and drops the lifeless body to the ground.",
	  TRUE, ch, 0, v, TO_NOTVICT);
      GET_HIT(v) -= 20;
      update_pos(v);
      if(GET_POS(v) == POSITION_DEAD) /* and it damn well better be */
	DamageEpilog(ch,v);
    }
  }
}

int veggie_gain_mods(struct char_data *ch, int gain, bool msgs)
{
  int bonus=0;

  if(time_info.hours > gSunRise && time_info.hours < gSunSet) { /* daylight */
    if(weather_info.sky < 2) {  /* partly cloudy & sunny */
      if(!IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
        if(!IS_SET(real_roomp(ch->in_room)->room_flags, DARK)) {
          if(real_roomp(ch->in_room)->sector_type != SECT_INSIDE &&
             real_roomp(ch->in_room)->sector_type != SECT_CITY   &&
             real_roomp(ch->in_room)->sector_type != SECT_DESERT) {
            /* we are now outdoors in good weather with sun! */
            bonus += gain>>1;
            if(msgs)
              send_to_char("Ahh, this weather is most invigorating.\n\r",ch);
          } else if( real_roomp(ch->in_room)->sector_type == SECT_DESERT) {
            bonus -= gain>>2;
            if(msgs)
              send_to_char("The heat and dry wind parch your skin.\n\r", ch);
          }
        } else {
          if(msgs)
            send_to_char("You wish the sun were caressing your limbs once again.\n\r", ch);
        }
      } else {                  /* we are indoors */
        if(msgs)
          send_to_char("You yearn to return outside once again.\n\r", ch);
      }
    }   /* is raining or worse */
  } else if(time_info.hours < gSunRise || time_info.hours > gSunSet) {
    if(msgs)
      send_to_char("Your metabolism slows from lack of sunlight.\n\r", ch);
    bonus -= gain>>2;
  }
  return(bonus);
}

int vamp_gain_mods(struct char_data *ch, int gain, bool msgs) 
{
  /* hierarchy of vampire problems in the world: */
  /* 1st: gain in sunlight is -1. */
  /* 2nd: gain elsewhere is normal */
  /* gain during nightimes at 3/4ths or better moons better regen */

  if(time_info.hours > gSunRise && time_info.hours < gSunSet - 1) {
    if(OUTSIDE(ch) && IS_LIGHT(ch->in_room)) {
      /* we are outside in daylight!  nuke the gain! */
      if(msgs)
        send_to_char("You cower in the sunlight.\n\r", ch);
      return(-gain);
    }
  } else if(time_info.hours < gSunRise || time_info.hours > gSunSet) {
    if(moontype > 20 && moontype < 28 ) { /* 3/4ths or better moon */
      if(msgs)
	send_to_char("The energy of the moon fills your veins.\n\r", ch);
      return(10);
    }
  }
  
  return(0);
  
}
