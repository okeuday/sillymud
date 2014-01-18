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
extern struct room_data *world;
extern struct obj_data  *object_list;
extern struct char_data *character_list;


/*
  cleric spells
*/

/*
 **   requires the sacrifice of 150k coins, victim loses a con point, and
 **   caster is knocked down to 1 hp, 1 mp, 1 mana, and sits for a LONG
 **   time (if a pc)
 */

void spell_resurrection(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj)
{
  struct char_file_u st;
  struct affected_type af;
  struct obj_data *obj_object, *next_obj;  
  FILE *fl;  
  int i;
  char t_name[100];
  extern int top_of_p_table;
  extern struct player_index_element *player_table;
  
  
  if (!obj) return;
  
  
  if (IS_CORPSE(obj)) {
    
    if (obj->char_vnum) {  /* corpse is a npc */

      if (GET_GOLD(ch) < 25000) {
	send_to_char("The gods are not happy with your sacrifice.\n\r",ch);
	return;
      } else {
	GET_GOLD(ch) -= 25000;
      }
      
      victim = read_mobile(obj->char_vnum, VIRTUAL);
      char_to_room(victim, ch->in_room);
      GET_GOLD(victim)=0;
      GET_EXP(victim)=0;
      GET_HIT(victim)=1;
      GET_POS(victim)=POSITION_STUNNED;
      
      act("With mystic power, $n resurrects a corpse.", TRUE, ch,
	  0, 0, TO_ROOM);
      act("$N slowly rises from the ground.", FALSE, ch, 0, victim, TO_ROOM);
      
      /*
	should be charmed and follower ch
	*/
      
      if (IsImmune(victim, IMM_CHARM) || IsResist(victim, IMM_CHARM)) {
 	act("$n says 'Thank you'", FALSE, ch, 0, victim, TO_ROOM);
	
      } else if(too_many_followers(ch)){
	act("$N takes one look at the size of your posse and justs says no!",
	    TRUE, ch, 0, victim, TO_CHAR);
	act("You take one look at the size of $n's posse and just say no!",
	    TRUE, ch, 0, victim, TO_ROOM);
      } else {
        af.type      = SPELL_CHARM_PERSON;
        af.duration  = follow_time(ch);
        af.modifier  = 0;
        af.location  = 0;
        af.bitvector = AFF_CHARM;
	
        affect_to_char(victim, &af);
	
       	add_follower(victim, ch);
      }
      
      IS_CARRYING_W(victim) = 0;
      IS_CARRYING_N(victim) = 0;
      
      /*
	take all from corpse, and give to person
	*/
      
      for (obj_object=obj->contains; obj_object; obj_object=next_obj) {
	next_obj = obj_object->next_content;
	obj_from_obj(obj_object);
	obj_to_char(obj_object, victim);
      }
      
      /*
	get rid of corpse
	*/
      extract_obj(obj);
      
      
    } else {          /* corpse is a pc  */
      
      if (GET_GOLD(ch) < 75000) {
	send_to_char("The gods are not happy with your sacrifice.\n\r",ch);
	return;
      } else {
	GET_GOLD(ch) -= 75000;
      }

      fl = fopen(PLAYER_FILE, "r+");
      if (!fl) {
	perror("player file");
	assert(0);
      }
      fseek(fl, obj->char_f_pos * sizeof(struct char_file_u), 0);
      fread(&st, sizeof(struct char_file_u), 1, fl);
      /*
       **   this is a serious kludge, and must be changed before multiple
       **   languages can be implemented
       */	
      if (st.talks[2] && st.abilities.con > 3) {

	/* check for the ONE_LIFE bit.  if it is set, go through the
	   player_table and find the person with the same char_f_pos.
	   change that name to the first name of the corpse, 
	   (making sure to lowercase it).  
	   */

#if 1
	if (IS_SET(st.affected_by2, AFF2_ONE_LIFER)) {
	  char *ptr;
	  for (i = 0; i <= top_of_p_table; i++)	{
	    if ((player_table + i)->nr == obj->char_f_pos) {
	      free((player_table +i)->name);
	      ptr = (char *)strrchr(obj->name, ' ');
	      if (!ptr || *ptr == '\0') {
		send_to_char("This is a very bad thing.\n\r", ch);
		return;
	      }
	      for (;*ptr==' ';ptr++);  /* all the way to last name */
	      (player_table +i)->name = (char *)malloc(strlen(ptr));
	      strcpy((player_table +i)->name, ptr);
	    }
	  }
	}
#endif
	
	st.points.exp *= 2;
	st.talks[2] = TRUE;
	st.abilities.con -= 1;
	act("A clear bell rings throughout the heavens", 
	    TRUE, ch, 0, 0, TO_CHAR);
	act("A ghostly spirit smiles, and says 'Thank you'", 
	    TRUE, ch, 0, 0, TO_CHAR);
	act("A clear bell rings throughout the heavens", 
	    TRUE, ch, 0, 0, TO_ROOM);
	act("A ghostly spirit smiles, and says 'Thank you'", 
	    TRUE, ch, 0, 0, TO_ROOM);
	act("$p dissappears in the blink of an eye.", 
	    TRUE, ch, obj, 0, TO_ROOM);
	act("$p dissappears in the blink of an eye.", 
	    TRUE, ch, obj, 0, TO_ROOM);
	GET_MANA(ch) = 1;
	GET_MOVE(ch) = 1;
	GET_HIT(ch) = 1;
	GET_POS(ch) = POSITION_STUNNED;
	act("$n collapses from the effort!",TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("You collapse from the effort\n\r",ch);
        fseek(fl, obj->char_f_pos * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);
	ObjFromCorpse(obj);	
	
      } else {
	send_to_char
	  ("The body does not have the strength to be recreated.\n\r", ch);
      }
      fclose(fl);
    }
  }  
}

void spell_cause_light(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch && victim);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(1,8);

  if (!HitOrMiss(ch, victim, CalcThaco(ch)))
    dam = 0;

  damage(ch, victim, dam, SPELL_CAUSE_LIGHT);

}

void spell_cause_critical(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch && victim);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(3,8) + 3;

  if (!HitOrMiss(ch, victim, CalcThaco(ch)))
    dam = 0;

  damage(ch, victim, dam, SPELL_CAUSE_CRITICAL);

}

void spell_cause_serious(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch && victim);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(2,8) + 2;

  if (!HitOrMiss(ch, victim, CalcThaco(ch)))
    dam = 0;

  damage(ch, victim, dam, SPELL_CAUSE_SERIOUS);

}

void spell_cure_serious(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch && victim);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(2,8)+2;

  if ( (dam + GET_HIT(victim)) > hit_limit(victim) )
    GET_HIT(victim) = hit_limit(victim);
  else
    GET_HIT(victim) += dam;

  send_to_char("You feel better!\n\r", victim);

  update_pos(victim);
/*
  if (IS_PC(ch) && IS_PC(victim))
      GET_ALIGNMENT(ch)+=2;
*/
}

void spell_mana(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = level*4;

  if (GET_MANA(ch)+dam > GET_MAX_MANA(ch))
    GET_MANA(ch) = GET_MAX_MANA(ch);
  else
    GET_MANA(ch) += dam;

}

void spell_second_wind(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch && victim);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(level,8)+level;

  if ( (dam + GET_MOVE(victim)) > move_limit(victim) )
    GET_MOVE(victim) = move_limit(victim);
  else
    GET_MOVE(victim) += dam;

  send_to_char("You feel less tired\n\r", victim);

}


void spell_flamestrike(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = dice(level,4);

  MissileDamage(ch, victim, dam, SPELL_FLAMESTRIKE);

}



void spell_dispel_good(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && victim);
   assert((level >= 1) && (level<=ABS_MAX_LVL));

  if (IsExtraPlanar(victim)) {
	if (IS_GOOD(ch)) {
       	    victim = ch;
	} else if (IS_EVIL(victim)) {
            act("Evil protects $N.", FALSE, ch, 0, victim, TO_CHAR);
  	    return;
	}
      
        if (!saves_spell(victim, SAVING_SPELL) ) {
	    act("$n forces $N from this plane.",TRUE,ch,0,victim,TO_NOTVICT);
	    act("You force $N from this plane.", TRUE, ch, 0, victim, TO_CHAR);
	    act("$n forces you from this plane.", TRUE, ch, 0, victim,TO_VICT);
	    gain_exp(ch, MIN(GET_EXP(victim)/2, 50000));
	    extract_char(victim);
	}
      } else {
	act("$N laughs at you.", TRUE, ch, 0, victim, TO_CHAR);
	act("$N laughs at $n.", TRUE,ch, 0, victim, TO_NOTVICT);
	act("You laugh at $n.", TRUE,ch,0,victim,TO_VICT);
      }
}

void spell_turn(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int diff,i, chance;
  bool turned;
  struct affected_type af;
  char buf[80];

  assert(ch && victim);
  assert((level >= 1) && (level<=ABS_MAX_LVL));

  turned = FALSE;

  if (IsUndead(victim)) {
    diff = level - GetMaxLevel(victim);
    if (diff <= -4) {  /* The puny girly girly clerics just don't cut it */
      act("You are powerless to affect $N", TRUE, ch, 0, victim, TO_CHAR);
      return;
    } else {
      chance = (50 + 15*diff);  /* % of success */

      /* This part is to see how many shots at the save we get
         Since anyone 4 levels or above will not fail the incompetance (chance)
         test, we now need a new way of distinguishing how powerful the
         cast was.  So.. for every 6-7 additional levels you're up on the
         victim, you get one more shot at the save.  This amounts to a level 50
        getting 8 shots at the save when turning a level 1 (50 + 49*15 + 1 = 8)
      */

      diff = (int) chance/100 + 1;
      if(diff < 1) diff=1; 

      if((number(1,100)) <= chance) {
        for(i=0; i<diff; i++) {
          if ((!saves_spell(victim, SAVING_SPELL)) && 
	      ((diff >= 25) && (GetTotLevel(victim) <= 10))) {
	    act("$n turns $N into a smoldering pile of dust!",
		TRUE,ch,0,victim,TO_NOTVICT);
	    act("You disintegrate $N!!!", TRUE, ch, 0, victim, TO_CHAR);
	    act("$n has just undone your life force!", 
		TRUE, ch, 0, victim,TO_VICT);
	    GET_POS(victim) = POSITION_DEAD;
	    DamageEpilog(ch, victim);
	    turned = TRUE;
	    break;
	  } else if (!saves_spell(victim, SAVING_SPELL)) {
	    if(IS_EVIL(ch) && 
	       !(IsImmune(victim, IMM_CHARM) || WeaponImmune(victim))) {
	      act("$n has taken control of $N.",TRUE,ch,0,victim,TO_NOTVICT);
	      act("You take over as master of $N.",
		  TRUE, ch, 0, victim, TO_CHAR);
	      act("$n is your new master.", TRUE, ch, 0, victim,TO_VICT);
	      if (victim->master)
		stop_follower(victim);
	      
	      add_follower(victim, ch);
	      
	      af.type      = SPELL_CHARM_PERSON;
	      
	      if (GET_CHR(ch))
		af.duration  = follow_time(ch);
	      else
		af.duration  = 24*18;   
	      af.modifier  = 0;
	      af.location  = 0;
	      af.bitvector = AFF_CHARM;
	      affect_to_char(victim, &af);
	      
	      if (!IS_PC(victim)) {
		REMOVE_BIT(victim->specials.act, ACT_AGGRESSIVE);
		SET_BIT(victim->specials.act, ACT_SENTINEL);
	      }
	      turned = TRUE;
	      break;
	    } else {
	      act("$n forces $N from this room.",
		  TRUE, ch, 0, victim, TO_NOTVICT);
	      act("You force $N from this room.", 
		  TRUE, ch, 0, victim, TO_CHAR);
	      act("$n forces you from this room.", 
		  TRUE, ch, 0, victim, TO_VICT);
	      AddFeared(victim, ch);
	      do_flee(victim,"",0);
	      turned = TRUE;
	      break;
	    }
	  }
        }
        if(!turned) {
          act("You laugh defiantly at $n.", TRUE, ch, 0, victim, TO_VICT);
	  act("$N laughs defiantly at $n.", TRUE, ch, 0, victim, TO_NOTVICT);
	  act("$N laughs defiantly at you.", TRUE, ch, 0, victim, TO_CHAR);
        }
      }
      else {
	act("You laugh at $n's incompetance.", TRUE, ch, 0, victim, TO_VICT);
	act("$N laughs at $n's incompetance.", TRUE, ch, 0, victim, TO_NOTVICT);
	act("$N laughs at your incompetance.", TRUE, ch, 0, victim, TO_CHAR);
      }
    }
  } 
  else {
    act("$n just tried to turn you, what a moron!", TRUE, ch, 0, victim, TO_VICT);
    act("$N thinks $n is really strange.", TRUE, ch, 0, victim, TO_NOTVICT);
    act("Um... $N isn't undead...", TRUE, ch, 0, victim, TO_CHAR);
  }
}

void spell_remove_paralysis(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && victim);

  if (IS_AFFECTED(victim, AFF_PARALYSIS))
    REMOVE_BIT(victim->specials.affected_by, AFF_PARALYSIS);

  if (affected_by_spell(victim,SPELL_PARALYSIS)) {
      affect_from_char(victim,SPELL_PARALYSIS);
      act("A warm feeling runs through your body.",FALSE,victim,0,0,TO_CHAR);
      act("$N looks better.",FALSE,ch,0,victim,TO_ROOM);
  } 

}

/* revamped by Hachiman, 10-25-93, repaired by Ripper */
/* If you ever get any code by Hachiman, debug it b4 you use it! */
void spell_holy_word(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct char_data *t, *next;
  
  assert((level >= 1) && (level<=ABS_MAX_LVL));

  if (IS_GOOD(ch)) {          /* If the caster is GOOD */
    send_to_char("You feel righteous fury fill your body as you declare your holy word!\n\r",ch);
    act("$n bellows a holy word!  You stagger from the blast and shield your eyes!",TRUE,ch,0,victim,TO_ROOM);
  }

  if (IS_EVIL(ch)) {          /* If the caster is EVIL */
    send_to_char("You feel demonic power fill your soul as you declare your unholy word!\n\r",ch);
      act("$n screams an unholy word!  Waves of nausea make you weary and sick!",TRUE,ch,0,victim,TO_ROOM);
  }

  if ( !(IS_GOOD(ch)) && !(IS_EVIL(ch)) ) {  /* Caster must be neutral! */
    send_to_char("You feel ecstasy in becoming as one with absolute emptiness.\n\r",ch);
    act("$n declares a holy word!  An unearthly cold grips your very soul.",
	TRUE,ch,0,victim,TO_ROOM);
  }
  
  for (t = real_roomp(ch->in_room)->people; t; t=next) {
    next = t->next_in_room;
    if (!in_group(ch, t) && !IS_IMMORTAL(t)) {
      if ( ( (IS_GOOD(ch)) && (!IS_EVIL(t)) ) ||
	  ( (IS_EVIL(ch)) && (!IS_GOOD(t)) ) ||
	  ( (!IS_EVIL(ch)) && (!IS_GOOD(ch)) &&
	   (!IS_EVIL(t)) && (!IS_GOOD(t)) ) ) {
	act("Your holy word does not affect $N.", TRUE, ch, 0, t, TO_CHAR);
	continue;
      } else if(!affected_by_spell(ch, SPELL_HOLY_WORD)) {
	if (IsExtraPlanar(t)) {
	  holy_banish(level,ch,t,obj);
	  continue;
	} else if ( level - GetMaxLevel(t) > 30 ) {
	  holy_destroy(level,ch,t,obj);
	  continue;
	} else if (level - GetMaxLevel(t) > 20 ) {
	  holy_bigdif(level,ch,t,obj);
	  continue;
	} else  if ( level - GetMaxLevel(t) > 10 ) {
	  holy_meddif(level,ch,t,obj);
	  continue;
	} else if ( level - GetMaxLevel(t) > 0 ) {
	  holy_smldif(level,ch,t,obj);
	}
	continue;
      } else {
	act("$N is allready in awe of your holy wrath!", 
	    TRUE, ch, 0, t, TO_CHAR);
      }
    }
  }  
}

void holy_banish (byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{
  act("$n forces you from this plane.",0, ch, 0, victim, TO_VICT);
  act("$N screams as $n banishes $M from this plane!",
      TRUE,ch,0,victim,TO_ROOM);
  act("Your holy word banishes $N to its home plane.",0, ch, 0, victim,
      TO_CHAR);
  if(!IS_PC(victim))
    extract_char(victim);
  else				/* in theory this should never happen */
				/* but in reality people do wierd things */
    holy_destroy(level,ch,victim,obj);
}

void holy_destroy(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{
  act("$N is destroyed in the searing blast!!",
      TRUE,ch,0,victim,TO_NOTVICT);
  act("$N is utterly destroyed by the power of your word!",0,ch, 0, victim,
      TO_CHAR);
  act("You are ripped to shreds by $n's power!", 0, ch, 0, victim, TO_VICT);
  GET_POS(victim) = POSITION_DEAD;
  DamageEpilog(ch, victim);
}

void holy_bigdif(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  int damg = 1;

  damg = (level-GetMaxLevel(victim))*3;
  
  act("$N screams in agony and terror at $n's furious blast!",
      TRUE,ch,0,victim,TO_NOTVICT);
  act("Your word blasts $N, $E screams in terror and pain!",
      0,ch, 0, victim, TO_CHAR);
  act("You scream out as $n blasts you. You are paralyzed with fear!",
      0, ch, 0, victim, TO_VICT);

  MissileDamage(ch,victim,damg,SPELL_EARTHQUAKE);

  af.type      = SPELL_HOLY_WORD;
  af.bitvector = AFF_PARALYSIS;
  af.duration  = (int)(level/10);
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  affect_join(victim, &af, FALSE, FALSE);
  GET_POS(victim)=POSITION_STUNNED;

  /* This next part blinds the opponent. Short duration  */

  send_to_char("You have been blinded!\n\r", victim);
  af.location  = APPLY_HITROLL;
  af.modifier  = -4;
  af.duration  = (int)(level/10);
  af.bitvector = AFF_BLIND;
  affect_to_char(victim, &af);
  af.location = APPLY_AC;
  af.modifier = +20;
  affect_to_char(victim, &af);

}

void holy_meddif(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  int damg = 1;

  damg = 2 * (level-GetMaxLevel(victim));

  act("$N is blasted by $n's mighty word, bringing $N to $S knees.",
      TRUE,ch,0,victim,TO_NOTVICT);
  act("You blast $N with your power, making $M writhe in silent agony.",0,
      ch, 0, victim, TO_CHAR);
  act("You are knocked to your knees as $n's word blasts your spirit",0,
      ch, 0, victim, TO_VICT);

  MissileDamage(ch,victim,damg,SPELL_EARTHQUAKE);
  GET_POS(victim)=POSITION_SITTING;
  send_to_char("You have been blinded!\n\r", victim);

  af.type      = SPELL_HOLY_WORD;
  af.location  = APPLY_HITROLL;
  af.modifier  = -4;
  af.duration  = (int)(level/10);
  af.bitvector = AFF_BLIND;
  affect_to_char(victim, &af);
  af.location = APPLY_AC;
  af.modifier = +20;
  affect_to_char(victim, &af);

  send_to_char("You clutch your throat, but cannot scream!\n\r",victim);
  af.duration  = (int)(level/10);
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = AFF_SILENCE;
  affect_to_char(victim, &af);
}

void holy_smldif(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{

  struct affected_type af;

  send_to_char("You stumble and fall from the blast!\n\r",victim);
  GET_POS(victim)=POSITION_SITTING;

  send_to_char("You have been blinded!\n\r", victim);
  af.type      = SPELL_HOLY_WORD;
  af.location  = APPLY_HITROLL;
  af.modifier  = -4;
  af.duration  = (int)(level/10);
  af.bitvector = AFF_BLIND;
  affect_to_char(victim, &af);
  af.location = APPLY_AC;
  af.modifier = +20;
  affect_to_char(victim, &af);
}

void spell_succor(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *o;

  o = read_object(3052,VIRTUAL);
  obj_to_char(o,ch);

  o->obj_flags.cost = 0;
  o->obj_flags.cost_per_day = -1;

  act("$n waves $s hand, and creates $p", TRUE, ch, o, 0, TO_ROOM);
  act("You wave your hand and create $p.", TRUE, ch, o, 0, TO_CHAR);

}

void spell_detect_charm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{

}

void spell_true_seeing(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim && ch);

  if (!IS_AFFECTED(victim, AFF_TRUE_SIGHT)) {    
    if (ch != victim) {
       send_to_char("Your eyes glow silver for a moment.\n\r", victim);
       act("$n's eyes take on a silvery hue.\n\r", FALSE, victim, 0, 0, TO_ROOM);
    } else {
       send_to_char("Your eyes glow silver.\n\r", ch);
       act("$n's eyes glow silver.\n\r", FALSE, ch, 0, 0, TO_ROOM);
    }

    af.type      = SPELL_TRUE_SIGHT;
    af.duration  = 2*level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_TRUE_SIGHT;
    affect_to_char(victim, &af);
  } else {
    send_to_char("Nothing seems to happen\n\r", ch);
  }
}




/*
   magic user spells
*/

void spell_track(byte level, struct char_data *ch,
   struct char_data *targ, struct obj_data *obj)
{
  char name[256], buf[256];
  struct affected_type af;

  extern struct char_data *character_list;

  if (ch != targ) {
    send_to_char("You feel your awareness grow!\n\r", targ);
  } else {
    send_to_char("You feel your awareness grow!\n\r", ch);
  }

  act("$N's eyes take on an emerald hue for just a moment.", 0,  ch, 0, targ, TO_ROOM);

  if (!obj) {
     af.type      = SPELL_MINOR_TRACK;
     af.duration  = level;
  } else {
     af.type      = SPELL_MAJOR_TRACK;
     af.duration  = level*2;
  }

  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char(targ, &af);


}

void spell_poly_self(byte level, struct char_data *ch,
   struct char_data *mob, struct obj_data *obj)
{

  char *buf;

  void do_snoop(struct char_data *ch, char *argument, int cmd);

  /*
   *  Check to make sure that there is no snooping going on.
   */
   if (!ch->desc || ch->desc->snoop.snooping) {
      send_to_char("Godly interference prevents the spell from working.\n\r",
		   ch);
      extract_char(mob);
      return;
   }

  if (ch->desc->snoop.snoop_by) {  /* force the snooper to stop */
      do_snoop(ch->desc->snoop.snoop_by, 
	       GET_NAME(ch->desc->snoop.snoop_by), 0);
  }


  /*
   * Put mobile in correct room
   */
  
  char_to_room(mob, ch->in_room);

  SwitchStuff(ch, mob);

  /*
   *  move char to storage
   */

  act("$n's flesh melts and flows into the shape of $N.", 
      TRUE, ch, 0, mob, TO_ROOM);
 
  act("Your flesh melts and flows into the shape of $N.", 
      TRUE, ch, 0, mob, TO_CHAR);

  char_from_room(ch);
  char_to_room(ch, 3); 
 
  /*
    stop following whoever you are following..
   */
   if (ch->master)
     stop_follower(ch);

  
  /*
   *  switch caster into mobile
   */
  
  ch->desc->character = mob;
  ch->desc->original = ch;
  
  mob->desc = ch->desc;
  ch->desc = 0;
 
  mob->specials.pct = ch->specials.pct;     /* Copy wimpy percentage so that it isn't 0 */
  mob->specials.flee = ch->specials.flee;   /* Also copy flee number */
  mob->specials.loot = ch->specials.loot;   /* Copy the auto stuff */
  mob->specials.split = ch->specials.split;

  SET_BIT(mob->specials.act, ACT_POLYSELF);
  SET_BIT(mob->specials.act, ACT_NICE_THIEF);
  SET_BIT(mob->specials.act, ACT_SENTINEL);
  REMOVE_BIT(mob->specials.act, ACT_AGGRESSIVE);
  REMOVE_BIT(mob->specials.act, ACT_SCAVENGER);

  GET_MANA(mob) = MIN((GET_MANA(mob)-15), 85);
  WAIT_STATE(mob, PULSE_VIOLENCE*2);


  /* do some fiddling with the strings */
  buf = (char *)malloc(strlen(GET_NAME(mob)) + strlen(GET_NAME(ch)) + 2);
  sprintf(buf, "%s %s", GET_NAME(ch), GET_NAME(mob));

#if TITAN
#else
  /* this code crashes ardent titans */
  if (GET_NAME(mob))
    free(GET_NAME(mob));
#endif

  GET_NAME(mob) = buf;
  buf = (char *)malloc(strlen(mob->player.short_descr) 
		       + strlen(GET_NAME(ch)) + 2);
  sprintf(buf, "%s %s", GET_NAME(ch), mob->player.short_descr);

#if TITAN
  if (mob->player.short_descr)
    free(mob->player.short_descr);
#endif
  mob->player.short_descr = buf;

  buf = (char *)malloc(strlen(mob->player.short_descr)+12);
  sprintf(buf, "%s is here\n\r", mob->player.short_descr);

#if TITAN
#else
  if (mob->player.long_descr)
    free(mob->player.long_descr);
#endif

  mob->player.long_descr = buf;

}

void spell_minor_create(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{

  assert(ch && obj);
  
  act("$n claps $s hands together.", TRUE, ch, 0, 0, TO_ROOM);
  act("You clap your hands together.", TRUE, ch, 0, 0, TO_CHAR);
  act("In a flash of light, $p appears.", TRUE, ch, obj, 0, TO_ROOM);
  act("In a flash of light, $p appears.", TRUE, ch, obj, 0, TO_CHAR);

  obj_to_room(obj, ch->in_room);

}


void spell_stone_skin(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if (!affected_by_spell(ch, SPELL_STONE_SKIN)) {
    act("$n's skin turns grey and granite-like.", TRUE, ch, 0, 0, TO_ROOM);
    act("Your skin turns to a stone-like substance.", TRUE, ch, 0, 0, TO_CHAR);

    af.type      = SPELL_STONE_SKIN;
    af.duration  = level;
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(ch, &af);

    /* resistance to piercing weapons */

    af.type      = SPELL_STONE_SKIN;
    af.duration  = level;
    af.modifier  = 32;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 0;
    affect_to_char(ch, &af);
  } 
}



void spell_infravision(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim && ch);

  if (!IS_AFFECTED(victim, AFF_INFRAVISION)) {
    if (ch != victim) {
       send_to_char("Your eyes glow red.\n\r", victim);
       act("$n's eyes glow red.\n\r", FALSE, victim, 0, 0, TO_ROOM);
    } else {
       send_to_char("Your eyes glow red.\n\r", ch);
       act("$n's eyes glow red.\n\r", FALSE, ch, 0, 0, TO_ROOM);
    }

    af.type      = SPELL_INFRAVISION;
    af.duration  = 4*level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_INFRAVISION;
    affect_to_char(victim, &af);

  } 
}

void spell_shield(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim && ch);

  if (!affected_by_spell(victim, SPELL_SHIELD)) {
    act("$N is surrounded by a strong force shield.", 
	TRUE, ch, 0, victim, TO_NOTVICT);
    if (ch != victim) {
       act("$N is surrounded by a strong force shield.", 
	   TRUE, ch, 0, victim, TO_CHAR);
       act("You are surrounded by a strong force shield.", 
	   TRUE, ch, 0, victim, TO_VICT);
    } else {
       act("You are surrounded by a strong force shield.", 
	   TRUE, ch, 0, 0, TO_CHAR);
     }

    af.type      = SPELL_SHIELD;
    af.duration  = 8+level;
    af.modifier  = -10;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(victim, &af);
  } 
}

void spell_weakness(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  float modifier;

  assert(ch && victim);

  if (!affected_by_spell(victim,SPELL_WEAKNESS)) {
    if (!saves_spell(victim, SAVING_SPELL)) {
      modifier = level/200.0;
      act("You feel weaker.", FALSE, victim,0,0,TO_VICT);
      act("$n seems weaker.", FALSE, victim, 0, 0, TO_ROOM);
      
      af.type      = SPELL_WEAKNESS;
      af.duration  = (int) level/2;
      af.modifier  = (int) 0 - (victim->abilities.str * modifier);
      if (victim->abilities.str_add) 
	af.modifier -= 2;
      af.location  = APPLY_STR;
      af.bitvector = 0;
      
      affect_to_char(victim, &af);
    } else {
      send_to_char("They resist the attempt to weaken them!\n\r", ch);
    }
  } else {
    send_to_char("They are allready weak enough!\n\r", ch);
  }
}

void spell_invis_group(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct char_data *tmp_victim;
  struct affected_type af;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

   for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
	tmp_victim = tmp_victim->next_in_room) {
      if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim))
         if (in_group(ch,tmp_victim)) {
       	    if (!affected_by_spell(tmp_victim, SPELL_INVISIBLE)) {

	       act("$n slowly fades out of existence.", TRUE, tmp_victim,0,0,TO_ROOM);
  	       send_to_char("You vanish.\n\r", tmp_victim);

	       af.type      = SPELL_INVISIBLE;
    	       af.duration  = 24;
    	       af.modifier  = -40;
  	       af.location  = APPLY_AC;
	       af.bitvector = AFF_INVISIBLE;
  	       affect_to_char(tmp_victim, &af);
	     }
	 }         
    }
}


void spell_acid_blast(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{

  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = dice(level,6);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  MissileDamage(ch, victim, dam, SPELL_ACID_BLAST);

}

void spell_cone_of_cold(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{

  int dam;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(level,5);

  send_to_char("A cone of freezing air fans out before you.\n\r", ch);
  act("$n sends a cone of ice shooting from the fingertips!\n\r",
	  FALSE, ch, 0, 0, TO_ROOM);

  AreaDamage(ch, dam, SPELL_CONE_OF_COLD, "You are chilled to the bone!\n\r",
	     "You are able to avoid the cone!\n\r", "", FALSE, FALSE);
#if 0
   for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
	tmp_victim = temp ) {
      temp = tmp_victim->next_in_room;
      if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
         if ((GetMaxLevel(tmp_victim)>LOW_IMMORTAL) && (!IS_NPC(tmp_victim)))
            return;
         if (!in_group(ch, tmp_victim)) {
            act("You are chilled to the bone!\n\r",
                 FALSE, ch, 0, tmp_victim, TO_VICT);
            if ( saves_spell(tmp_victim, SAVING_SPELL) )
                dam >>= 1;
   	    MissileDamage(ch, tmp_victim, dam, SPELL_CONE_OF_COLD);
	 } else {
            act("You are able to avoid the cone!\n\r",
                 FALSE, ch, 0, tmp_victim, TO_VICT);
	  }
       }
    }
#endif
}

void spell_ice_storm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(level,4);

  send_to_char("You conjure a storm of ice.\n\r", ch);
  act("$n conjures an ice storm!\n\r",
	  FALSE, ch, 0, 0, TO_ROOM);

  AreaDamage(ch, dam, SPELL_ICE_STORM, "You are blasted by the storm!\n\r",
	     "You are able to dodge the storm.\n\r", "", FALSE, FALSE);
#if 0
   for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
	tmp_victim = temp) {
      temp = tmp_victim->next_in_room;
      if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
         if ((GetMaxLevel(tmp_victim)>LOW_IMMORTAL) && (!IS_NPC(tmp_victim)))
            return;
         if (!in_group(ch, tmp_victim)) {
            act("You are blasted by the storm\n\r",
                 FALSE, ch, 0, tmp_victim, TO_VICT);
            if ( saves_spell(tmp_victim, SAVING_SPELL) )
                dam >>= 1;
   	    MissileDamage(ch, tmp_victim, dam, SPELL_ICE_STORM);
	 } else {
            act("You are able to dodge the storm!\n\r",
                 FALSE, ch, 0, tmp_victim, TO_VICT);
	  }
       }
    }
#endif
}


void spell_poison_cloud(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{

  /* hmm, I like this one. */

}

void spell_major_create(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
}


void spell_sending(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
}

void spell_meteor_swarm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 

  dam = dice(level,12);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  MissileDamage(ch, victim, dam, SPELL_METEOR_SWARM);

}

void spell_Create_Monster(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  struct char_data *mob;
  int rnum;
  struct room_data *rp;

   /* load in a monster of the correct type, determined by
      level of the spell */

/* really simple to start out with */

   if ((rp = real_roomp(ch->in_room)) == NULL)
     return;

   if (IS_SET(rp->room_flags, TUNNEL)) {
     send_to_char("There isn't enough room in here to summon that.\n\r", ch);
     return;
   }

   if (level <= 5) {
      rnum = number(1,10)+200;      
      mob = read_mobile(rnum, VIRTUAL);
   } else if (level <= 7) {
      rnum = number(1,10)+210;
      mob = read_mobile(rnum, VIRTUAL);
   } else if (level <= 9) {
      rnum = number(1,10)+220;
      mob = read_mobile(rnum, VIRTUAL);
   } else if (level <= 11) {
      rnum = number(1,10)+230;
      mob = read_mobile(rnum, VIRTUAL);
   } else if (level <= 13) {
      rnum = number(1,10)+240;
      mob = read_mobile(rnum, VIRTUAL);
   } else if (level <= 15) {
      rnum = 250+number(1,10);
      mob = read_mobile(rnum, VIRTUAL);
   } else {
      rnum = 260+number(1,10);
      mob = read_mobile(rnum, VIRTUAL);
   }

    if (!mob) {
     send_to_char("The summons failed\n\r", ch);
     return;
    }

    char_to_room(mob, ch->in_room);

    act("$n waves $s hand, and $N appears!", TRUE, ch, 0, mob, TO_ROOM);
    act("You wave your hand, and $N appears!", TRUE, ch, 0, mob, TO_CHAR);

    if (too_many_followers(ch)) {
      act("$N says 'No way I'm hanging with that crowd!!'",
          TRUE, ch, 0, mob, TO_ROOM);
      act("$N refuses to hang out with crowd of your size!!", TRUE, ch, 0,
	  mob, TO_CHAR);
    } else {


      /* charm them for a while */
      if (mob->master)
	stop_follower(mob);
      
      add_follower(mob, ch);
      
      af.type      = SPELL_CHARM_PERSON;
      
      if (GET_CHR(ch)) {
	if (IS_PC(ch) || ch->master) {
	  af.duration  = follow_time(ch);
	  af.modifier  = 0;
	  af.location  = 0;
	  af.bitvector = AFF_CHARM;
	  affect_to_char(mob, &af);
	  
	} else {
	  SET_BIT(mob->specials.affected_by, AFF_CHARM);
	}
      } else {

	af.duration  = 24*18;
	
	af.modifier  = 0;
	af.location  = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char(mob, &af);
      }
    }

/*
  adjust the bits...
*/

/*
 get rid of aggressive, add sentinel
*/

  if (IS_SET(mob->specials.act, ACT_AGGRESSIVE)) {
    REMOVE_BIT(mob->specials.act, ACT_AGGRESSIVE);
  }
  if (!IS_SET(mob->specials.act, ACT_SENTINEL)) {
    SET_BIT(mob->specials.act, ACT_SENTINEL);
  }


}




/*
   either
*/

void spell_light(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
   
/*
   creates a ball of light in the hands.
*/
  struct obj_data *tmp_obj;

  assert(ch);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

#if 0
  CREATE(tmp_obj, struct obj_data, 1);
  clear_object(tmp_obj);

  tmp_obj->name = strdup("ball light");
  tmp_obj->short_description = strdup("A ball of light");
  tmp_obj->description = strdup("There is a ball of light on the ground here.");

  tmp_obj->obj_flags.type_flag = ITEM_LIGHT;
  tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_HOLD;
  tmp_obj->obj_flags.value[2] = 24+level;
  tmp_obj->obj_flags.weight = 1;
  tmp_obj->obj_flags.cost = 10;
  tmp_obj->obj_flags.cost_per_day = 1;

  tmp_obj->next = object_list;
  object_list = tmp_obj;

  obj_to_char(tmp_obj,ch);

  tmp_obj->item_number = -1;
#else
  tmp_obj = read_object(20, VIRTUAL);  /* this is all you have to do */
  if (tmp_obj) {
      tmp_obj->obj_flags.value[2] = 24+level;
      obj_to_char(tmp_obj,ch);
  } else {
    send_to_char("Sorry, I can't create the ball of light\n\r", ch);
    return;
  }


#endif

  act("$n twiddles $s thumbs and $p suddenly appears.",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("You twiddle your thumbs and $p suddenly appears.",TRUE,ch,tmp_obj,0,TO_CHAR);

}

void spell_fly(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim);
  
  act("You feel lighter than air!", TRUE, ch, 0, victim, TO_VICT);
  if (victim != ch) {
     act("$N's feet rise off the ground.", TRUE, ch, 0, victim, TO_CHAR);
   } else {
     send_to_char("Your feet rise up off the ground.", ch);
   }
  act("$N's feet rise off the ground.", TRUE, ch, 0, victim, TO_NOTVICT);
  
    af.type      = SPELL_FLY;
    af.duration  = GET_LEVEL(ch, BestMagicClass(ch))+3;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char(victim, &af);
}

void spell_fly_group(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  struct char_data *tch;
  
  assert(ch);

  if (real_roomp(ch->in_room) == NULL)  {
    return;
  }
  
  for (tch=real_roomp(ch->in_room)->people; tch; tch=tch->next_in_room) {
    if (in_group(ch, tch)) {
      act("You feel lighter than air!", TRUE, ch, 0, tch, TO_VICT);
      if (tch != ch) {
	act("$N's feet rise off the ground.", TRUE, ch, 0, tch, TO_CHAR);
      } else {
	send_to_char("Your feet rise up off the ground.", ch);
      }
      act("$N's feet rise off the ground.", TRUE, ch, 0, tch, TO_NOTVICT);
      
      af.type      = SPELL_FLY;
      af.duration  = GET_LEVEL(ch, BestMagicClass(ch))+3;
      af.modifier  = 0;
      af.location  = 0;
      af.bitvector = AFF_FLYING;
      affect_to_char(tch, &af);
    }
  }
}

void spell_refresh(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(ch && victim);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(level,4)+level;
  dam = MAX(dam,20);

  if ( (dam + GET_MOVE(victim)) > move_limit(victim) )
    GET_MOVE(victim) = move_limit(victim);
  else
    GET_MOVE(victim) += dam;

  send_to_char("You feel less tired\n\r", victim);

}


void spell_water_breath(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{

  struct affected_type af;

  assert(ch && victim);
  
  act("You feel fishy!", TRUE, ch, 0, victim, TO_VICT);
  if (victim != ch) {
     act("$N makes a face like a fish.", TRUE, ch, 0, victim, TO_CHAR);
   }
  act("$N makes a face like a fish.", TRUE, ch, 0, victim, TO_NOTVICT);
  
    af.type      = SPELL_WATER_BREATH;
    af.duration  = GET_LEVEL(ch, BestMagicClass(ch))+3;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_WATERBREATH;
    affect_to_char(victim, &af);
  

}



void spell_cont_light(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
/*
   creates a ball of light in the hands.
*/
  struct obj_data *tmp_obj;

  assert(ch);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

#if 0
  CREATE(tmp_obj, struct obj_data, 1);
  clear_object(tmp_obj);

  tmp_obj->name = strdup("ball light");
  tmp_obj->short_description = strdup("A bright ball of light");
  tmp_obj->description = strdup("There is a bright ball of light on the ground here.");

  tmp_obj->obj_flags.type_flag = ITEM_LIGHT;
  tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_HOLD;
  tmp_obj->obj_flags.value[2] = -1;
  tmp_obj->obj_flags.weight = 1;
  tmp_obj->obj_flags.cost = 40;
  tmp_obj->obj_flags.cost_per_day = 1;

  tmp_obj->next = object_list;
  object_list = tmp_obj;

  obj_to_char(tmp_obj,ch);

  tmp_obj->item_number = -1;
#else
  tmp_obj = read_object(20, VIRTUAL);  /* this is all you have to do */
  if (tmp_obj)
     obj_to_char(tmp_obj,ch);
  else {
    send_to_char("Sorry, I can't create the ball of light\n\r", ch);
    return;
  }    
#endif

  act("$n twiddles $s thumbs and $p suddenly appears.",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("You twiddle your thumbs and $p suddenly appears.",TRUE,ch,tmp_obj,0,TO_CHAR);

}

void spell_animate_dead(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *corpse)
{
  struct char_data *mob;
  struct obj_data *obj_object,*next_obj;
  char buf[MAX_STRING_LENGTH];
  int r_num=100; /* virtual # for zombie */

  /*
    some sort of check for corpse hood
    */
  if ((GET_ITEM_TYPE(corpse)!=ITEM_CONTAINER)||
      (!corpse->obj_flags.value[3])) {
    send_to_char("The magic fails abruptly!\n\r",ch);
    return;
  }
  
  mob = read_mobile(r_num, VIRTUAL);
  char_to_room(mob, ch->in_room);
  
  act("With mystic power, $n animates a corpse.", TRUE, ch,
      0, 0, TO_ROOM);
  act("$N slowly rises from the ground.", FALSE, ch, 0, mob, TO_ROOM);
  
  /*
    zombie should be charmed and follower ch
    */
  
  SET_BIT(mob->specials.affected_by, AFF_CHARM);
  GET_EXP(mob) = 0;
  add_follower(mob, ch);
  IS_CARRYING_W(mob) = 0;
  IS_CARRYING_N(mob) = 0;
  
  /*
    take all from corpse, and give to zombie 
    */
  
  for (obj_object=corpse->contains; obj_object; obj_object=next_obj) {
    next_obj = obj_object->next_content;
    obj_from_obj(obj_object);
    obj_to_char(obj_object, mob);
  }
  
  /*
    set up descriptions and such
    */ 
  sprintf(buf,"%s is here, slowly animating\n\r",corpse->short_description);
  mob->player.long_descr = strdup(buf);
  
  /*
  set up hitpoints
  */
  mob->points.max_hit = dice( MAX(level/2,5),8);
  mob->points.hit = mob->points.max_hit/2;
  mob->player.sex = 0;
  
  GET_RACE(mob) = RACE_UNDEAD;
  mob->player.class = ch->player.class;
  
  /*
    get rid of corpse
    */
  extract_obj(corpse);
  
}

void spell_know_alignment(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
   int ap;
   char buf[200], name[100];

   assert(victim && ch);

   if (IS_NPC(victim))
      strcpy(name,victim->player.short_descr);
   else
      strcpy(name,GET_NAME(victim));
   
   ap = GET_ALIGNMENT(victim);
   
   if (ap > 700) 
      sprintf(buf,"%s has an aura as white as the driven snow.\n\r",name);
   else if (ap > 350)
      sprintf(buf, "%s is of excellent moral character.\n\r",name);
   else if (ap > 100)
      sprintf(buf, "%s is often kind and thoughtful.\n\r",name);
   else if (ap > 25)
      sprintf(buf, "%s isn't a bad sort...\n\r",name);
   else if (ap > -25)
      sprintf(buf, "%s doesn't seem to have a firm moral commitment\n\r",name);
   else if (ap > -100)
    sprintf(buf, "%s isn't the worst you've come across\n\r",name);
   else if (ap > -350)
    sprintf(buf, "%s could be a little nicer, but who couldn't?\n\r",name);
   else if (ap > -700)
    sprintf(buf, "%s probably just had a bad childhood\n\r",name);
   else 
     sprintf(buf,"I'd rather just not say anything at all about %s\n\r",name);

   send_to_char(buf,ch);
   
}

void spell_dispel_magic(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
   int yes=0;  
   int i;

   int check_falling( struct char_data *ch);

   assert(ch && (victim || obj));

   if (obj) {
     if ( IS_SET(obj->obj_flags.extra_flags, ITEM_INVISIBLE) )
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);

     if ( IS_SET(obj->obj_flags.extra_flags, ITEM_BLESS))
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_BLESS);

     if ( IS_SET(obj->obj_flags.extra_flags, ITEM_MAGIC))
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_MAGIC);

     if (level > 50) {
       /* strip off everything */
       for (i=0;i<MAX_OBJ_AFFECT;i++) {
	 obj->affected[i].location = 0;	
       }
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_GLOW);
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_HUM);
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD);
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_EVIL);
       REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_NEUTRAL);
     }
     return;
   }

/* gets rid of infravision, invisibility, detect, etc */

   if (GetMaxLevel(victim)<=GetMaxLevel(ch))
      yes = TRUE;
   else 
     yes = FALSE;

    if (affected_by_spell(victim,SPELL_INVISIBLE)) 
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
         affect_from_char(victim,SPELL_INVISIBLE);
         send_to_char("You feel exposed.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_DETECT_INVISIBLE))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
         affect_from_char(victim,SPELL_DETECT_INVISIBLE);
         send_to_char("You feel less perceptive.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_DETECT_EVIL))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
         affect_from_char(victim,SPELL_DETECT_EVIL);
         send_to_char("You feel less morally alert.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_DETECT_MAGIC))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_DETECT_MAGIC);
        send_to_char("You stop noticing the magic in your life.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_SENSE_LIFE))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_SENSE_LIFE);
        send_to_char("You feel less in touch with living things.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_SANCTUARY)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_SANCTUARY);
        send_to_char("You don't feel so invulnerable anymore.\n\r",victim);
        act("The white glow around $n's body fades.",FALSE,victim,0,0,TO_ROOM);
      }
      /*
       *  aggressive Act.
       */
      if ((victim->attackers < 6) && (!victim->specials.fighting) &&
	    (IS_NPC(victim))) {
	  set_fighting(victim, ch);
	}
    }
    if (IS_AFFECTED(victim, AFF_SANCTUARY)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
	REMOVE_BIT(victim->specials.affected_by, AFF_SANCTUARY);
	send_to_char("You don't feel so invulnerable anymore.\n\r",victim);
	act("The white glow around $n's body fades.",FALSE,victim,0,0,TO_ROOM);      }
      /*
       *  aggressive Act.
       */
      if ((victim->attackers < 6) && (!victim->specials.fighting) &&
	  (IS_NPC(victim))) {
	set_fighting(victim, ch);
      }
    }
    if (affected_by_spell(victim,SPELL_PROTECT_FROM_EVIL))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_PROTECT_FROM_EVIL);
        send_to_char("You feel less morally protected.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_INFRAVISION))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_INFRAVISION);
        send_to_char("Your sight grows dimmer.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_SLEEP))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_SLEEP);
        send_to_char("You don't feel so tired.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_CHARM_PERSON))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_CHARM_PERSON);
        send_to_char("You feel less enthused about your master.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_WEAKNESS))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_WEAKNESS);
        send_to_char("You don't feel so weak.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_STRENGTH))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_STRENGTH);
        send_to_char("You don't feel so strong.\n\r",victim);
    }
    
    if (affected_by_spell(victim,SPELL_ARMOR))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_ARMOR);
        send_to_char("You don't feel so well protected.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_DETECT_POISON))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_DETECT_POISON);
        send_to_char("You don't feel so sensitive to fumes.\n\r",victim);
    }
    
    if (affected_by_spell(victim,SPELL_BLESS))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_BLESS);
        send_to_char("You don't feel so blessed.\n\r",victim);
    }

    if (affected_by_spell(victim,SPELL_FLY))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_FLY);
        send_to_char("You don't feel lighter than air anymore.\n\r",victim);
	check_falling(victim);
    }

    if (affected_by_spell(victim,SPELL_WATER_BREATH))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_WATER_BREATH);
        send_to_char("You don't feel so fishy anymore.\n\r",victim);
    }

    if (affected_by_spell(victim,SPELL_FIRE_BREATH))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_FIRE_BREATH);
        send_to_char("You don't feel so fiery anymore.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_LIGHTNING_BREATH))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_LIGHTNING_BREATH);
        send_to_char("You don't feel so electric anymore.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_GAS_BREATH))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_GAS_BREATH);
        send_to_char("You don't have gas anymore.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_FROST_BREATH))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_FROST_BREATH);
        send_to_char("You don't feel so frosty anymore.\n\r",victim);
    }
    if (affected_by_spell(victim,SPELL_FIRESHIELD)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_FIRESHIELD);
        send_to_char("You don't feel so fiery anymore.\n\r",victim);
        act("The fiery aura around $n's body fades.",FALSE,victim,0,0,TO_ROOM);
      }
      /*
       *  aggressive Act.
       */
      if ((victim->attackers < 6) && (!victim->specials.fighting) &&
            (IS_NPC(victim))) {
          set_fighting(victim, ch);
        }
    }
    if (IS_AFFECTED(victim, AFF_FIRESHIELD)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        REMOVE_BIT(victim->specials.affected_by, AFF_FIRESHIELD);
        send_to_char("You don't fiery so invulnerable anymore.\n\r",victim);
        act("The fiery aura around $n's body fades.",FALSE,victim,0,0,TO_ROOM);
	}
      /*
       *  aggressive Act.
       */
      if ((victim->attackers < 6) && (!victim->specials.fighting) &&
          (IS_NPC(victim))) {
        set_fighting(victim, ch);
      }
    }

    if (affected_by_spell(victim,SPELL_FAERIE_FIRE))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_FAERIE_FIRE);
        send_to_char("You don't feel so pink anymore.\n\r",victim);
	act("The pink glow around $n's body fades.", TRUE, ch, 0, 0, TO_ROOM);
    }

    if (affected_by_spell(victim,SPELL_MINOR_TRACK))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_MINOR_TRACK);
        send_to_char("You lose the trail.\n\r",victim);
      
      }

    if (affected_by_spell(victim,SPELL_MAJOR_TRACK))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_MAJOR_TRACK);
        send_to_char("You lose the trail.\n\r",victim);
      }

    if (affected_by_spell(victim,SPELL_WEB)) {
        affect_from_char(victim,SPELL_WEB);
        send_to_char("You don't feel so sticky anymore.\n\r",victim);
    }

   if (affected_by_spell(victim, SPELL_SILENCE)) {
        affect_from_char(victim,SPELL_SILENCE);
        send_to_char("You don't feel so quiet anymore.\n\r",victim);
   }
   if (affected_by_spell(victim, SPELL_TREE_TRAVEL)) {
        affect_from_char(victim,SPELL_TREE_TRAVEL);
        send_to_char("You don't feel so in touch with trees anymore.\n\r",
		     victim);     
   }
   if (affected_by_spell(victim, SPELL_HEAT_STUFF)) {
        affect_from_char(victim,SPELL_HEAT_STUFF);
        send_to_char("You don't feel so hot anymore\n\r",
		     victim);     
   }
   if (affected_by_spell(victim, SPELL_HASTE)) {
        affect_from_char(victim,SPELL_HASTE);
        send_to_char("You don't feel so fast anymore\n\r",
		     victim);     
   }
   if (affected_by_spell(victim, SPELL_SLOW)) {
        affect_from_char(victim,SPELL_SLOW);
        send_to_char("You don't feel so slow anymore\n\r",
		     victim);     
   }
   if (affected_by_spell(victim, SPELL_BARKSKIN)) {
        affect_from_char(victim,SPELL_BARKSKIN);
        send_to_char("You don't feel so barky anymore\n\r",
		     victim);     
   }
   if (affected_by_spell(victim,SPELL_AID))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
	affect_from_char(victim,SPELL_AID);
        send_to_char("You feel less aided.\n\r",victim);
      }
   if (affected_by_spell(victim,SPELL_SHIELD))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_SHIELD);
        send_to_char("You feel less shielded.\n\r",victim);
      }
      if (affected_by_spell(victim,SPELL_TRUE_SIGHT))
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        REMOVE_BIT(victim->specials.affected_by, AFF_TRUE_SIGHT);
        affect_from_char(victim,SPELL_TRUE_SIGHT);
        send_to_char("You feel less keen.\n\r",victim);
      }


   if (affected_by_spell(victim, SPELL_INVIS_TO_ANIMALS)) {
        affect_from_char(victim,SPELL_INVIS_TO_ANIMALS);
   }

   if (affected_by_spell(victim, SPELL_DRAGON_RIDE)) {
        affect_from_char(victim,SPELL_DRAGON_RIDE);
   }


   if (level == IMPLEMENTOR)  {
    if (affected_by_spell(victim,SPELL_BLINDNESS)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_BLINDNESS);
        send_to_char("Your vision returns.\n\r",victim);
      }
    }
    if (affected_by_spell(victim,SPELL_PARALYSIS)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_PARALYSIS);
        send_to_char("You feel freedom of movement.\n\r",victim);
      }
    }
    if (affected_by_spell(victim,SPELL_POISON)) {
      if (yes || !saves_spell(victim, SAVING_SPELL) ) {
        affect_from_char(victim,SPELL_POISON);
      }
    }
   }
}



void spell_paralyze(byte level, struct char_data *ch,
		    struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(victim);
  
  
  if (!IS_AFFECTED(victim, AFF_PARALYSIS)) {
    if (IsImmune(victim, IMM_HOLD)) {
      FailPara(victim, ch);
      return;
    }
    if (IsResist(victim, IMM_HOLD)) {
      if (saves_spell(victim, SAVING_PARA)) {
	FailPara(victim, ch);
	return;
      }
      if (saves_spell(victim, SAVING_PARA)) {
	FailPara(victim, ch);
	return;
      }
    } else if (!IsSusc(victim, IMM_HOLD)) {
      if (saves_spell(victim, SAVING_PARA)) {
	FailPara(victim, ch);
	return;
      }    
    }
    
    af.type      = SPELL_PARALYSIS;
    af.duration  = 4+level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_PARALYSIS;
    affect_join(victim, &af, FALSE, FALSE);
    
    act("Your limbs freeze in place.",FALSE,victim,0,0,TO_CHAR);
    act("$n is paralyzed!",TRUE,victim,0,0,TO_ROOM);
    GET_POS(victim)=POSITION_STUNNED;
    
  } else {
    send_to_char("Someone tries to paralyze you AGAIN!\n\r",victim);
  }
}

void spell_fear(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(victim && ch);
  
  if (GetMaxLevel(ch) >= GetMaxLevel(victim)-2) {
     if ( !saves_spell(victim, SAVING_SPELL))  {
	do_flee(victim, "", 0);
	if (!IS_PC(victim))
	  AddFeared(victim, ch);
      } else {
	send_to_char("You feel afraid, but the effect fades.\n\r",victim);
	return;
      }
   }
}

void spell_calm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{  
  assert(ch && victim);
/* 
   removes aggressive bit from monsters 
*/
  if (IS_NPC(victim)) {
     if (IS_SET(victim->specials.act, ACT_AGGRESSIVE)) {
       if (HitOrMiss(ch, victim, CalcThaco(ch))) {
	 REMOVE_BIT(victim->specials.act, ACT_AGGRESSIVE);
	 send_to_char("You sense peace.\n\r", ch);
       }
     } else {
       send_to_char("You feel calm\n\r", victim);
     }
  } else {
    send_to_char("You feel calm.\n\r", victim);
  }
}

void spell_web(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{  
  struct affected_type af;
  bool fail,pissed,big,no_effect;
  
  assert(ch && victim);

  big=pissed=fail=no_effect=FALSE;

  switch (GET_RACE(victim)) {
  case RACE_ARACHNID:
  case RACE_SLIME:
  case RACE_GHOST:
    act("$N laughs at the webs!",FALSE, ch, 0, victim, TO_CHAR);
    act("Hah, $n casted web on you, what a flake.",FALSE,ch,0,victim,TO_VICT);
    act("$N laughs at $n's webs!",FALSE, ch, 0, victim, TO_NOTVICT);
    return;
    break;
  }

  if(!saves_spell(victim, SAVING_PARA)) fail=TRUE;
  
  if (IS_NPC(victim) && IS_SET(victim->specials.act, ACT_HUGE)) big=TRUE;

    if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
      if (IsSmall(victim) && !fail && !number(0,3)) pissed=TRUE;   /* 25% */
      else if(big) {
	if(fail) {
	  if(number(0,4) < 2) pissed=TRUE;                         /* 40% */
	}	else {
	  if(number(0,4) < 3) pissed=TRUE;                         /* 60% */
	}
      } else {
	if(fail) {
	  if(!number(0,4)) pissed=TRUE;                            /* 20% */
	} else {
	  if(!number(0,2)) pissed=TRUE;                            /* 33% */
	}
      }
    } else {			/* assume if not indoors, outdoors and */
                                /* web is less affective at blocking the */
				/* victim from the caster. */
      if(IsSmall(victim) && !fail && !number(0,2)) pissed=TRUE;    /* 33% */
      else if(big) {
	if(fail) {
	  if(number(0,4) < 3) pissed=TRUE;                         /* 60% */
	} else pissed=TRUE;	                                 /* 100% */
      } else {
	if(fail) {
	  if(number(0,4) < 2) pissed=TRUE;                         /* 40% */
	} else {
	  if(number(0,4) < 3) pissed=TRUE;                         /* 60% */
	}
      }
    }

  if (fail) {
    af.type      = SPELL_WEB;
    af.duration  = level;
    af.modifier  = -50;
    af.location  = APPLY_MOVE;
    af.bitvector = 0;

    affect_to_char(victim, &af);
    if(!pissed) {
      act("You are stuck in a sticky webbing!", FALSE, ch, 0, victim, TO_VICT);
      act("$N is stuck in a sticky webbing!", FALSE, ch, 0, victim,TO_NOTVICT);
      act("You wrap $N in a sticky webbing!",FALSE, ch, 0, victim, TO_CHAR);
    } else {
      act("You are wrapped in webs, but they don't stop you!", FALSE, ch, 
	  0, victim, TO_VICT);
      act("$N attacks, paying little heed to the webs that slow it.", FALSE,
	  ch, 0, victim, TO_NOTVICT);
      act("You only manage to piss off $N with your webs, ack!", FALSE, ch,
	  0, victim, TO_CHAR);
    }
  }else {
    if(pissed) {
      act("You are almost caught in a sticky webbing, GRRRR!",
	  FALSE, ch, 0, victim, TO_VICT);
      act("$N growls and dodges $n's sticky webbing!", 
	  FALSE, ch, 0, victim,TO_NOTVICT);
      act("You miss $N with your sticky webbing!  Uh oh, it's mad.",
	  FALSE, ch, 0, victim, TO_CHAR);
    } else {
      act("You watch with amusement as $n casts web about the room.",
	  FALSE, ch, 0, victim, TO_VICT);
      act("$n misses $N with the webs!", FALSE, ch, 0, victim, TO_NOTVICT);
      act("You miss with your webs, but $N doesn't seem to notice.",
	  FALSE, ch, 0, victim, TO_CHAR);
    }
  }
  if(pissed)
    if (IS_NPC(victim) && !victim->specials.fighting)
      set_fighting(victim,ch);
}

void spell_heroes_feast(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct char_data *tch;

  if (real_roomp(ch->in_room) == NULL)  {
    return;
  }

  for (tch=real_roomp(ch->in_room)->people; tch; tch=tch->next_in_room) {
     if ((in_group(tch, ch)) && (GET_POS(tch) > POSITION_SLEEPING)) {
        send_to_char("You partake of a magnificent feast!\n\r", tch);	
	gain_condition(tch,FULL,24);
	gain_condition(tch,THIRST,24);
	GET_HIT(tch)+= level/2;
	if (GET_HIT(tch) < GET_MAX_HIT(tch)) {
	  GET_HIT(tch)+=1;
	}
	GET_MOVE(tch) = GET_MAX_MOVE(tch);	
     }
  }
}




void spell_conjure_elemental(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  /*
   *   victim, in this case, is the elemental
   *   object could be the sacrificial object
   */

   assert(ch && victim && obj);

   /*
   ** objects:
   **     fire  : red stone
   **     water : pale blue stone
   **     earth : grey stone
   **     air   : clear stone
   */

   act("$n gestures, and a cloud of smoke appears", TRUE, ch, 0, 0, TO_ROOM);
   act("$n gestures, and a cloud of smoke appears", TRUE, ch, 0, 0, TO_CHAR);
   act("$p explodes with a loud BANG!", TRUE, ch, obj, 0, TO_ROOM);
   act("$p explodes with a loud BANG!", TRUE, ch, obj, 0, TO_CHAR);
   obj_from_char(obj);
   extract_obj(obj);
   char_to_room(victim, ch->in_room);
   act("Out of the smoke, $N emerges", TRUE, ch, 0, victim, TO_NOTVICT);

   /* charm them for a while */

  if (too_many_followers(ch)) {
    act("$N says 'No way I'm hanging with that crowd, but thanks for the lift'",TRUE, ch, 0, victim, TO_ROOM);
    act("$N refuses to hang out with crowd of your size, but thanks you for the free ride.", TRUE, ch, 0, victim, TO_CHAR);
  } else {
    

    if (victim->master)
      stop_follower(victim);

    add_follower(victim, ch);

    af.type      = SPELL_CHARM_PERSON;
    af.duration  = 40;
    af.modifier  = follow_time(ch);
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    
    affect_to_char(victim, &af);
  }
}

void spell_faerie_fire (byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim);

  if (affected_by_spell(victim, SPELL_FAERIE_FIRE)) {
    send_to_char("Nothing new seems to happen",ch);
    return;
  }

  act("$n points at $N.", TRUE, ch, 0, victim, TO_ROOM);
  act("You point at $N.", TRUE, ch, 0, victim, TO_CHAR);
  act("$N is surrounded by a pink outline", TRUE, ch, 0, victim, TO_ROOM);
  act("$N is surrounded by a pink outline", TRUE, ch, 0, victim, TO_CHAR);
  
  af.type      = SPELL_FAERIE_FIRE;
  af.duration  = level;
  af.modifier  = 20;
  af.location  = APPLY_ARMOR;
  af.bitvector = 0;
  
  affect_to_char(victim, &af);

}

void spell_faerie_fog (byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct char_data *tmp_victim;

  assert(ch);

  act("$n snaps $s fingers, and a cloud of purple smoke billows forth",
      TRUE, ch, 0, 0, TO_ROOM);
  act("You snap your fingers, and a cloud of purple smoke billows forth",
      TRUE, ch, 0, 0, TO_CHAR);

   for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
	tmp_victim = tmp_victim->next_in_room) {
      if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
         if (IS_IMMORTAL(tmp_victim))
            break;
         if (!in_group(ch, tmp_victim)) {
	   if (IS_AFFECTED(tmp_victim, AFF_INVISIBLE)) {
            if ( saves_spell(tmp_victim, SAVING_SPELL) ) {
	      REMOVE_BIT(tmp_victim->specials.affected_by, AFF_INVISIBLE);
	      act("$n is briefly revealed, but dissapears again.",
		  TRUE, tmp_victim, 0, 0, TO_ROOM);
	      act("You are briefly revealed, but dissapear again.",
		  TRUE, tmp_victim, 0, 0, TO_CHAR);
	      SET_BIT(tmp_victim->specials.affected_by, AFF_INVISIBLE);
	    } else {
	      struct affected_type af;

	      REMOVE_BIT(tmp_victim->specials.affected_by, AFF_INVISIBLE);

	      act("$n is revealed!",
		  TRUE, tmp_victim, 0, 0, TO_ROOM);
	      act("You are revealed!",
		  TRUE, tmp_victim, 0, 0, TO_CHAR);
	      af.type      = SPELL_FAERIE_FIRE;
	      af.duration  = level;
	      af.modifier  = 20;
	      af.location  = APPLY_ARMOR;
	      af.bitvector = 0;
	      
	      affect_to_char(tmp_victim, &af);
	    }
	   }
	 }
       }
    }
}



void spell_cacaodemon(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim && obj);

   act("$n gestures, and a black cloud of smoke appears", TRUE, ch, 0, 0, TO_ROOM);
   act("$n gestures, and a black cloud of smoke appears", TRUE, ch, 0, 0, TO_CHAR);
   if (GET_LEVEL(ch, CLERIC_LEVEL_IND) > 40 && IS_EVIL(ch)) {
     act("$p smokes briefly", TRUE, ch, obj, 0, TO_ROOM);
     act("$p smokes briefly", TRUE, ch, obj, 0, TO_CHAR);
     obj->obj_flags.cost /= 2;
     if (obj->obj_flags.cost < 100) {
       act("$p bursts into flame and disintegrates!", 
	   TRUE, ch, obj, 0, TO_ROOM);
       act("$p bursts into flame and disintegrates!", 
	   TRUE, ch, obj, 0, TO_CHAR);
       obj_from_char(obj);
       extract_obj(obj);
     }
   } else {
     act("$p bursts into flame and disintegrates!", TRUE, ch, obj, 0, TO_ROOM);
     act("$p bursts into flame and disintegrates!", TRUE, ch, obj, 0, TO_CHAR);
     obj_from_char(obj);
     extract_obj(obj);
     GET_ALIGNMENT(ch)-=5;
   }
   char_to_room(victim, ch->in_room);

   act("With an evil laugh, $N emerges from the smoke", TRUE, ch, 0, victim, TO_NOTVICT);

  if (too_many_followers(ch)) {
    act("$N says 'No way I'm hanging with that crowd!!'",
	TRUE, ch, 0, victim, TO_ROOM);
    act("$N refuses to hang out with crowd of your size!!", TRUE, ch, 0,
	victim, TO_CHAR);
  } else {

    /* charm them for a while */
    if (victim->master)
      stop_follower(victim);
    
    add_follower(victim, ch);
    
    af.type      = SPELL_CHARM_PERSON;
    af.duration  = follow_time(ch);
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;

    affect_to_char(victim, &af);

  }
  if (IS_SET(victim->specials.act, ACT_AGGRESSIVE))
    REMOVE_BIT(victim->specials.act, ACT_AGGRESSIVE);
  
  if (!IS_SET(victim->specials.act, ACT_SENTINEL))
    SET_BIT(victim->specials.act, ACT_SENTINEL);
}

/*
 neither
*/

void spell_improved_identify(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
}



void spell_geyser(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  if (ch->in_room<0)
    return;
	dam =  dice(level,3);

  act("The Geyser erupts in a huge column of steam!\n\r",
	  FALSE, ch, 0, 0, TO_ROOM);

  AreaDamage(ch, dam, SPELL_GEYSER, 
	     "You are seared by the boiling water!!\n\r",
	     "You are almost seared by the boiling water!!\n\r",
	     "You hear Old Faithful off in the distance.\n\r",
	     FALSE, FALSE);
#if 0
   for(tmp_victim = real_roomp(ch->in_room)->people; 
       tmp_victim; tmp_victim = temp) {
      temp = tmp_victim->next_in_room;
      if ((ch != tmp_victim) && (ch->in_room == tmp_victim->in_room)) {
            if ((GetMaxLevel(tmp_victim)<LOW_IMMORTAL)||(IS_NPC(tmp_victim))) {
      	       MissileDamage(ch, tmp_victim, dam, SPELL_GEYSER);
               act("You are seared by the boiling water!!\n\r",
                   FALSE, ch, 0, tmp_victim, TO_VICT);
	    } else {
               act("You are almost seared by the boiling water!!\n\r",
                 FALSE, ch, 0, tmp_victim, TO_VICT);
	    }
      }
    }
#endif
}

void spell_green_slime(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  int hpch;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL)); 
  
  hpch = GET_MAX_HIT(ch);
  if(hpch<10) hpch=10;
  
  dam = (int)(hpch/10);
  
  if ( saves_spell(victim, SAVING_BREATH) )
    dam >>= 1;
  
  send_to_char("You are attacked by green slime!\n\r",victim);
  
  damage(ch, victim, dam, SPELL_GREEN_SLIME);

}
