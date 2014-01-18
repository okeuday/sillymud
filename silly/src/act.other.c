/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
  
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
  */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "protos.h"


/* extern variables */

extern struct str_app_type str_app[];
extern struct descriptor_data *descriptor_list;
extern struct dex_skill_type dex_app_skill[];
extern struct skill_data skill_info[];
extern struct char_data *character_list;
extern struct index_data *obj_index;
extern struct time_info_data time_info;


void do_gain(struct char_data *ch, char *argument, int cmd)
{
  
}

void do_guard(struct char_data *ch, char *argument, int cmd)
{
  char comm[100];
  
  if (!IS_NPC(ch) || IS_SET(ch->specials.act, ACT_POLYSELF)) {
    send_to_char("Sorry. you can't just put your brain on autopilot!\n\r",ch);
    return;
  }
  
  for(;isspace(*argument); argument++);
  
  if (!*argument) {
    if (IS_SET(ch->specials.act, ACT_GUARDIAN)) {
      act("$n relaxes.", FALSE, ch, 0, 0, TO_ROOM);
      send_to_char("You relax.\n\r",ch);
      REMOVE_BIT(ch->specials.act, ACT_GUARDIAN);
    } else {
      SET_BIT(ch->specials.act, ACT_GUARDIAN);
      act("$n alertly watches you.", FALSE, ch, 0, ch->master, TO_VICT);
      act("$n alertly watches $N.", FALSE, ch, 0, ch->master, TO_NOTVICT);
      send_to_char("You snap to attention\n\r", ch);    
    }  
  } else {
    if (!str_cmp(argument,"on")) {
      if (!IS_SET(ch->specials.act, ACT_GUARDIAN)) {
	SET_BIT(ch->specials.act, ACT_GUARDIAN);
	act("$n alertly watches you.", FALSE, ch, 0, ch->master, TO_VICT);
	act("$n alertly watches $N.", FALSE, ch, 0, ch->master, TO_NOTVICT);
	send_to_char("You snap to attention\n\r", ch);
      }
    } else if (!str_cmp(argument,"off")) {
      if (IS_SET(ch->specials.act, ACT_GUARDIAN)) {
	act("$n relaxes.", FALSE, ch, 0, 0, TO_ROOM);
	send_to_char("You relax.\n\r",ch);
	REMOVE_BIT(ch->specials.act, ACT_GUARDIAN);
      }
    }
  }
  
  return;
}


void do_junk(struct char_data *ch, char *argument, int cmd)
{
  char arg[100], buf[100], newarg[100];
  struct obj_data *tmp_object;
  int num, p, count, value=0;
  
  /*
   *   get object name & verify
   */
  
  only_argument(argument, arg);
  if (*arg) {
    if (getall(arg,newarg)!=NULL) {
      num = -1;
      strcpy(arg,newarg);
    } else if ((p = getabunch(arg,newarg))!=NULL) {
      num = p;                     
      strcpy(arg,newarg);
    } else {
      num = 1;  
    }
  } else {
    send_to_char("Junk what?\n\r",ch);
    return;
  }
  count = 0;
  while (num != 0) {
    tmp_object = get_obj_in_list_vis(ch, arg, ch->carrying);
    if (tmp_object) {
      if (IS_OBJ_STAT(tmp_object,ITEM_NODROP)) {
	send_to_char
	  ("You can't let go of it, it must be CURSED!\n\r", ch);
	return;
      }
      value+=(MIN(1000,MAX(tmp_object->obj_flags.cost/4,1)));
      obj_from_char(tmp_object);
      extract_obj(tmp_object);
      if (num > 0) num--;
      count++;
    } else {
      num = 0;
    }
  }
  if (count > 1) {
    sprintf(buf, "You junk %s (%d).\n\r", arg, count);
    act(buf, 1, ch, 0, 0, TO_CHAR);
    sprintf(buf, "$n junks %s.\n\r", arg);
    act(buf, 1, ch, 0, 0, TO_ROOM);
  } else if (count == 1) {
    sprintf(buf, "You junk %s \n\r", arg);
    act(buf, 1, ch, 0, 0, TO_CHAR);
    sprintf(buf, "$n junks %s.\n\r", arg);
    act(buf, 1, ch, 0, 0, TO_ROOM);
  } else {
    send_to_char("You don't have anything like that\n\r", ch);
  }
  
  value /= 2;
  
  if (value) 	{
    act("You are awarded for outstanding performance.", 
	FALSE, ch, 0, 0, TO_CHAR);
    
    if (GetMaxLevel(ch) < 3)
      gain_exp(ch, MIN(100,value));
    else
      GET_GOLD(ch) += value;
  }
  
  return;
}

void do_qui(struct char_data *ch, char *argument, int cmd)
{
  send_to_char("You have to write quit - no less, to quit!\n\r",ch);
  return;
}


void do_title(struct char_data *ch, char *argument, int cmd)
{
  char buf[512];
  
  char *strdup(char *source);
  
  if (IS_NPC(ch) || !ch->desc)
    return;
  
  for(;isspace(*argument); argument++)  ;
  
  if (*argument) {
    
    if (strlen(argument) > 150) {
      send_to_char("Line too long, truncated\n", ch);
      *(argument + 151) = '\0';
    }
    sprintf(buf, "Your title has been set to : <%s>\n\r", argument);
    send_to_char(buf, ch);
    free(ch->player.title);
    ch->player.title = strdup(argument);
  }      
  
}

void do_quit(struct char_data *ch, char *argument, int cmd)
{
  void die(struct char_data *ch);
  char buf[256];
  
  if (IS_NPC(ch) || !ch->desc || IS_AFFECTED(ch, AFF_CHARM))
    return;
  
  if (GET_POS(ch) == POSITION_FIGHTING) {
    send_to_char("No way! You are fighting.\n\r", ch);
    return;
  }
  
  if (GET_POS(ch) < POSITION_STUNNED) {
    send_to_char("You die before your time!\n\r", ch);
    sprintf(buf, "%s dies via quit.", GET_NAME(ch));
    log(buf);
    die(ch);    
    return;
  }
  
  act("Goodbye, friend.. Come back soon!", FALSE, ch, 0, 0, TO_CHAR);
  act("$n has left the game.", TRUE, ch,0,0,TO_ROOM);
  zero_rent(ch);
  extract_char(ch); /* Char is saved in extract char */
}



void do_save(struct char_data *ch, char *argument, int cmd)
{
  struct obj_cost cost;
  struct char_data *tmp;
  struct obj_data *tl, *f;
  struct obj_data *teq[MAX_WEAR], *eq[MAX_WEAR], *o;
  int i;
  struct follow_type *fol;
  
  if (IS_NPC(ch) && !(IS_SET(ch->specials.act, ACT_POLYSELF)))
    return;
  
#if NODUPLICATES
#else
  send_to_char("Saving\n\r", ch);
#endif
  
  if (IS_NPC(ch) && (IS_SET(ch->specials.act, ACT_POLYSELF))) {
    /*  
      swap stuff, and equipment
      */
    if (!ch->desc)  /* fuck you assholes :-) */
      tmp = ch->orig;
    else 
      tmp = ch->desc->original;  /* tmp = the original characer */
    
    if (!tmp) return;
    tl = tmp->carrying;
    /*
      there is a bug with this:  When you save, the alignment thing is checked,
      to see if you are supposed to be wearing what you are.  if your stuff gets
      kicked off your body, it will end up in room #3, on the floor, and in
      the inventory of the polymorphed monster.  This is a "bad" thing.  So,
      to fix it, each item in the inventory is checked.  if it is in a room,
      it is moved from the room, back to the correct inventory slot. 
      */
    tmp->carrying = ch->carrying;
    /* Search ch for followers that may be figurines, if we find one, kick it
       into inventory for save, and kick it out after. */
    f = NULL;
    for(fol = ch->followers; fol; fol = fol->next)
      if(IS_SET(fol->follower->specials.act, ACT_FIGURINE)) {
	f = fol->follower->link;
	break;
      }
    
    if(f)
      obj_to_char(f, tmp);
    
    for (i = 0; i < MAX_WEAR; i++) {  /* move all the mobs eq to the ch */
      teq[i] = tmp->equipment[i];
      tmp->equipment[i] = ch->equipment[i];
    }
    GET_EXP(tmp) = GET_EXP(ch);
    GET_GOLD(tmp) = GET_GOLD(ch);
    GET_ALIGNMENT(tmp) = GET_ALIGNMENT(ch);
    recep_offer(tmp, NULL, &cost);
    save_obj(tmp, &cost, 0);
    save_char(ch, AUTO_RENT);  /* we can't save tmp because they
				  don't have a desc.  */
    write_char_extra(ch);
    tmp->carrying = tl;
    if (f)  /* jerry.. you fucked up :) */
      obj_from_char(f);
    
    for (i = 0; i < MAX_WEAR; i++) {
      tmp->equipment[i] = teq[i];
      if (ch->equipment[i] && ch->equipment[i]->in_room != -1) {
	o = ch->equipment[i];
	ch->equipment[i] = 0;
	obj_from_room(o);
	/*
	  obj_to_char(o, ch);
	  */
	equip_char(ch, o, i);  /* equip the correct slot */
      }
    }
    
    return;
  } else {			/* non polies */
    f = NULL;
    for(fol = ch->followers; fol; fol = fol->next)
      if(IS_SET(fol->follower->specials.act, ACT_FIGURINE)) {
	f = fol->follower->link;
	break;
      }
    
    if(f)
      obj_to_char(f, ch);
    
    recep_offer(ch, NULL, &cost);
    save_obj(ch, &cost, 0);
    save_char(ch, AUTO_RENT);
    if(f)
      obj_from_char(f);
    /* to assert that you never lose your intrinsic abilities. */
    /* But, we need it so that intrinsicly flying people aren't  */
    /* forced to fly all the time. */ 
    if(IS_INTRINSIC(ch, AFF_FLYING) && IS_AFFECTED(ch,AFF_FLYING)) {
      ch->specials.affected_by |= ch->specials.intrinsics;
      REMOVE_BIT(ch->specials.affected_by, AFF_FLYING);
    } else 
      ch->specials.affected_by |= ch->specials.intrinsics;
  }
}


void do_not_here(struct char_data *ch, char *argument, int cmd)
{
  send_to_char("Sorry, but you cannot do that here!\n\r",ch);
}



void do_sneak(struct char_data *ch, char *argument, int cmd)
{
  struct affected_type af;
  byte percent;
  
  if (IS_AFFECTED(ch, AFF_SNEAK)) {
    affect_from_char(ch, SKILL_SNEAK);
    if (IS_AFFECTED(ch, AFF_HIDE))
      REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);
    send_to_char("You are no longer sneaky.\n\r",ch);
    return;
  }
  if (!HasClass(ch, CLASS_THIEF) && !(HasClass(ch, CLASS_MONK))) {
    send_to_char("You're no thief!\n\r", ch);
    return;
  }
  
  if (MOUNTED(ch)) {
    send_to_char("Yeah... right... while mounted\n\r", ch);
    return;
  }
  
  if (!IS_AFFECTED(ch, AFF_SILENCE)) {
    if (EqWBits(ch, ITEM_ANTI_THIEF)) {
      send_to_char("Gonna be hard to sneak around in that!\n\r", ch);
      return;
    }
    if (HasWBits(ch, ITEM_HUM)) {
      send_to_char("Gonna be hard to sneak around with that thing humming\n\r", 
		   ch);
      return;
    }
  }
  
  send_to_char("Ok, you'll try to move silently for a while.\n\r", ch);
  
  percent=number(1,101); /* 101% is a complete failure */
  
  if (!ch->skills)
    return;
  
  if (IS_AFFECTED(ch, AFF_SILENCE))
    percent = MIN(1, percent-35);  /* much easier when silenced */
  
  if (percent > ch->skills[SKILL_SNEAK].learned +
      dex_app_skill[GET_DEX(ch)].sneak) {
    LearnFromMistake(ch, SKILL_SNEAK, 1, 90);
    WAIT_STATE(ch, PULSE_VIOLENCE/2);
    return;
  }
  
  af.type = SKILL_SNEAK;
  af.duration = GET_LEVEL(ch, BestThiefClass(ch));
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = AFF_SNEAK;
  affect_to_char(ch, &af);
  WAIT_STATE(ch, PULSE_VIOLENCE/2);
  
  
}



void do_hide(struct char_data *ch, char *argument, int cmd)
{
  byte percent;
  
  send_to_char("you attempt to hide yourself.\n\r", ch);
  
  if (IS_AFFECTED(ch, AFF_HIDE))
    REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);
  
  if (!HasClass(ch, CLASS_THIEF) && !(HasClass(ch, CLASS_MONK))) {
    send_to_char("You're no thief!\n\r", ch);
    return;
  }
  
  if (MOUNTED(ch)) {
    send_to_char("Yeah... right... while mounted\n\r", ch);
    return;
  }
  
  percent=number(1,101); /* 101% is a complete failure */
  
  if (!ch->skills)
    return;
  
  if (percent > ch->skills[SKILL_HIDE].learned +
      dex_app_skill[GET_DEX(ch)].hide) {
    LearnFromMistake(ch, SKILL_HIDE, 1, 90);
    WAIT_STATE(ch, PULSE_VIOLENCE*1);
    return;
  }
  
  SET_BIT(ch->specials.affected_by, AFF_HIDE);
  WAIT_STATE(ch, PULSE_VIOLENCE*1);
  
}


void do_steal(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  struct obj_data *obj;
  char victim_name[240];
  char obj_name[240];
  char buf[240];
  int percent;
  int gold, eq_pos;
  bool ohoh = FALSE;
  
  if (!ch->skills)
    return;
  
  if (check_peaceful(ch, "What if he caught you?\n\r"))
    return;
  
  argument = one_argument(argument, obj_name);
  only_argument(argument, victim_name);
  
  if (!HasClass(ch, CLASS_THIEF)) {
    send_to_char("You're no thief!\n\r", ch);
    return;
  }
  
  if (MOUNTED(ch)) {
    send_to_char("Yeah... right... while mounted\n\r", ch);
    return;
  }
  
  if (!(victim = get_char_room_vis(ch, victim_name))) {
    send_to_char("Steal what from who?\n\r", ch);
    return;
  } else if (victim == ch) {
    send_to_char("Come on now, that's rather stupid!\n\r", ch);
    return;
  }
  
  if(GetMaxLevel(victim) > 50) {
    send_to_char("Steal from a God?!?  Oh the thought!\n\r", ch);
    sprintf(buf, "BUG NOTE: %s tried to steal from GOD %s", GET_NAME(ch), GET_NAME(victim));
    log(buf);
    return;
  }
  
  WAIT_STATE(ch, PULSE_VIOLENCE*2);  /* they're gonna have to wait. */
  
  if ((GetMaxLevel(ch) < 2) && (!IS_NPC(victim))) {
    send_to_char("Due to misuse of steal, you can't steal from other players\n\r", ch);
    send_to_char("unless you are at least 2cnd level. \n\r", ch);
    return;
  }
  
  if ((!victim->desc) && (!IS_NPC(victim)))
    return;
  
  /* 101% is a complete failure */
  percent=number(1,101) - dex_app_skill[GET_DEX(ch)].p_pocket;
  
  if (GET_POS(victim) < POSITION_SLEEPING)
    percent = -1; /* ALWAYS SUCCESS */
  
  percent += GET_AVE_LEVEL(victim);
  
  if (GetMaxLevel(victim)>MAX_MORT)
    percent = 101; /* Failure */
  
  if (str_cmp(obj_name, "coins") && str_cmp(obj_name,"gold")) {
    
    if (!(obj = get_obj_in_list_vis(victim, obj_name, victim->carrying))) {
      
      for (eq_pos = 0; (eq_pos < MAX_WEAR); eq_pos++)
	if (victim->equipment[eq_pos] &&
	    (isname(obj_name, victim->equipment[eq_pos]->name)) &&
	    CAN_SEE_OBJ(ch,victim->equipment[eq_pos])) {
	  obj = victim->equipment[eq_pos];
	  break;
	}
      
      if (!obj) {
	act("$E has not got that item.",FALSE,ch,0,victim,TO_CHAR);
	return;
      } else { /* It is equipment */
	if ((GET_POS(victim) > POSITION_STUNNED)) {
	  send_to_char("Steal the equipment now? Impossible!\n\r", ch);
	  return;
	} else {
	  act("You unequip $p and steal it.",FALSE, ch, obj ,0, TO_CHAR);
	  act("$n steals $p from $N.",FALSE,ch,obj,victim,TO_NOTVICT);
	  obj_to_char(unequip_char(victim, eq_pos), ch);
#if NODUPLICATES
	  do_save(ch, "", 0);
	  do_save(victim, "", 0);
#endif
	  if (IS_PC(ch) && IS_PC(victim))
	    GET_ALIGNMENT(ch)-=20;
	  
	}
      }
    } else {  /* obj found in inventory */
      
      if (IS_OBJ_STAT(obj,ITEM_NODROP)) {
	send_to_char
	  ("You can't steal it, it must be CURSED!\n\r", ch);
	return;
      }
      
      percent += GET_OBJ_WEIGHT(obj); /* Make heavy harder */
      
      if (AWAKE(victim) && (percent > ch->skills[SKILL_STEAL].learned)) {
	ohoh = TRUE;
	act("Oops..", FALSE, ch,0,0,TO_CHAR);
	LearnFromMistake(ch, SKILL_STEAL, 0, 90);
	act("$n tried to steal something from you!",FALSE,ch,0,victim,TO_VICT);
	act("$n tries to steal something from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
      } else { /* Steal the item */
	if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
	  if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) < CAN_CARRY_W(ch)) {
	    obj_from_char(obj);
	    obj_to_char(obj, ch);
	    send_to_char("Got it!\n\r", ch);
#if NODUPLICATES
	    do_save(ch, "", 0);
	    do_save(victim, "", 0);
#endif
	    if (IS_PC(ch) && IS_PC(victim))
	      GET_ALIGNMENT(ch)-=20;
	    
	  } else {
	    send_to_char("You cannot carry that much.\n\r", ch);
	  }
	} else
	  send_to_char("You cannot carry that much.\n\r", ch);
      }
    }
  } else { /* Steal some coins */
    if (AWAKE(victim) && (percent > ch->skills[SKILL_STEAL].learned)) {
      ohoh = TRUE;
      act("Oops..", FALSE, ch,0,0,TO_CHAR);
      if (ch->skills[SKILL_STEAL].learned < 90) {
        act("Even though you were caught, you realize your mistake and promise to remember.",FALSE, ch, 0, 0, TO_CHAR);
        ch->skills[SKILL_STEAL].learned++;
	if (ch->skills[SKILL_STEAL].learned >= 90)
	  send_to_char("You are now learned in this skill!\n\r", ch);
      }
      act("You discover that $n has $s hands in your wallet.",FALSE,ch,0,victim,TO_VICT);
      act("$n tries to steal gold from $N.",TRUE, ch, 0, victim, TO_NOTVICT);
    } else {
      /* Steal some gold coins */
      gold = (int) ((GET_GOLD(victim)*number(1,10))/100);
      gold = MIN(number(1000,2000), gold);
      if (gold > 0) {
	GET_GOLD(ch) += gold;
	GET_GOLD(victim) -= gold;
	sprintf(buf, "Bingo! You got %d gold coins.\n\r", gold);
	send_to_char(buf, ch);
	if (IS_PC(ch) && IS_PC(victim))
	  GET_ALIGNMENT(ch)-=20;
      } else {
	send_to_char("You couldn't get any gold...\n\r", ch);
      }
    }
  }
  
  if (ohoh && IS_NPC(victim) && AWAKE(victim))
    if (IS_SET(victim->specials.act, ACT_NICE_THIEF)) {
      sprintf(buf, "%s is a bloody thief.", GET_NAME(ch));
      do_shout(victim, buf, 0);
      do_say(victim, "Don't you ever do that again!", 0);
    } else {
      if (CAN_SEE(victim, ch))
	hit(victim, ch, TYPE_UNDEFINED);
      else if (number(0,1))
	hit(victim, ch, TYPE_UNDEFINED);
    }
  
}

void do_practice(struct char_data *ch, char *arg, int cmd) 
{
  char buf[MAX_STRING_LENGTH*2], buffer[MAX_STRING_LENGTH*2];
  int i;
  
  extern char *spells[];
  extern struct skill_data skill_info[MAX_SPL_LIST];
  
  buffer[0] = '\0';
  
  if ((cmd != 164) && (cmd != 170)) return;
  
  if (!ch->skills)
    return;
  
  for (; isspace(*arg); arg++);
  
  if (!arg || !*arg) {
    send_to_char("You need to supply a class.",ch);
    return;
  }
  
  switch(*arg) {
  case 's':
  case 'S':
    arg++;
    if(LOWER(*arg) == 'p') {
      if(!HasClass(ch, CLASS_MAGIC_USER) && !HasClass(ch, CLASS_CLERIC) &&
	 !HasClass(ch, CLASS_DRUID)) {
	send_to_char("You have the following intrinsic spell abilities:\n\r",ch);
	for(i=0; *spells[i] != '\n' && i < (MAX_EXIST_SPELL+1); i++)
	  if(IsIntrinsic(ch,i+1)) {
	    sprintf(buf,"%-30s %s \n\r",spells[i],
		    how_good(ch->skills[i+1].learned));
	    if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	      break;
	    strcat(buffer, buf);
	    strcat(buffer, "\r");
	  }
	page_string(ch->desc, buffer, 1);
	return;
      } else {
	send_to_char("You know of these spells:\n\r", ch);
	for(i=0; *spells[i] != '\n'; i++)
	  if (skill_info[i+1].spell_pointer && (ch->skills[i+1].learned ||
						IsIntrinsic(ch, i+1))) {
	    sprintf(buf,"[%-3d] %-25s %s",
		    (skill_info[i+1].min_level[MIN_LEVEL_MAGIC] < 
		     skill_info[i+1].min_level[MIN_LEVEL_CLERIC] ? 
		     skill_info[i+1].min_level[MIN_LEVEL_MAGIC] : 
		     skill_info[i+1].min_level[MIN_LEVEL_CLERIC]),
		    spells[i],how_good(ch->skills[i+1].learned));
	    if(IsIntrinsic(ch, i+1))
	      strcat(buf," (intrinsic)\n\r");
	    else 
	      strcat(buf,"\n\r");
	    if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	      break;
	    strcat(buffer, buf);
	    strcat(buffer, "\r");
	  }
      }
      page_string(ch->desc, buffer, 1);
      return;
      break;
    } else if (LOWER(*arg) == 'k') {
      send_to_char("You have knowledge of these skills:\n\r", ch);
      for(i=0; *spells[i] != '\n' && i < (MAX_EXIST_SPELL+1); i++)
        if (!skill_info[i+1].spell_pointer && 
	    ( IS_SET(ch->player.class, skill_info[i+1].class_use) ||
	     IsIntrinsic(ch,i+1))) {
          sprintf(buf,"%-30s %s \n\r",spells[i],
                  how_good(ch->skills[i+1].learned));
          if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
            break;
          strcat(buffer, buf);
          strcat(buffer, "\r");
        }
      page_string(ch->desc, buffer, 1);
      return;
    }
    break;
  case 'w':
  case 'W':
  case 'f':
  case 'F': 
    {
      if (!HasClass(ch, CLASS_WARRIOR)) {
	send_to_char("I bet you think you're a warrior.\n\r", ch);
	return;
      }
      send_to_char("You have knowledge of these skills:\n\r", ch);
      for(i=0; *spells[i] != '\n' && i < (MAX_EXIST_SPELL+1); i++)
	if (!skill_info[i+1].spell_pointer && ch->skills[i+1].learned) {
	  sprintf(buf,"%-30s %s \n\r",spells[i],
		  how_good(ch->skills[i+1].learned));
	  if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	    break;
	  strcat(buffer, buf);
	  strcat(buffer, "\r");
	}
      page_string(ch->desc, buffer, 1);
      return;
    } break;
  case 't':
  case 'T':
    {
      
      if (!HasClass(ch, CLASS_THIEF)) {
	send_to_char("I bet you think you're a thief.\n\r", ch);
	return;
      }
      send_to_char("You have knowledge of these skills:\n\r", ch);
      for(i=0; *spells[i] != '\n' && i < (MAX_EXIST_SPELL+1); i++)
	if (!skill_info[i+1].spell_pointer && ch->skills[i+1].learned) {
	  sprintf(buf,"%-30s %s \n\r",spells[i],
		  how_good(ch->skills[i+1].learned));
	  if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	    break;
	  strcat(buffer, buf);
	  strcat(buffer, "\r");
	}
      page_string(ch->desc, buffer, 1);
      return;
    } break;
  case 'M':
  case 'm':
    {
      if (!HasClass(ch, CLASS_MAGIC_USER)) {
	send_to_char("I bet you think you're a magic-user.\n\r", ch);
	return;
      }
      send_to_char("Your spellbook holds these spells:\n\r", ch);
      for(i=0; *spells[i] != '\n'; i++)
	if (skill_info[i+1].spell_pointer &&
	    (skill_info[i+1].min_level[MIN_LEVEL_MAGIC] <=
	     GET_LEVEL(ch,MAGE_LEVEL_IND))) {
	  sprintf(buf,"[%d] %s %s \n\r",
                  skill_info[i+1].min_level[MIN_LEVEL_MAGIC],
		  spells[i],how_good(ch->skills[i+1].learned));
	  if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	    break;
	  strcat(buffer, buf);
	  strcat(buffer, "\r");
	}
      page_string(ch->desc, buffer, 1);
      return;			  
    } 
    break;
  case 'C':
  case 'c':
  case 'P':
  case 'p':
    {
      if (!HasClass(ch, CLASS_CLERIC)) {
	send_to_char("I bet you think you're a cleric.\n\r", ch);
	return;
      }
      send_to_char("You can attempt any of these spells:\n\r", ch);
      for(i=0; *spells[i] != '\n'; i++)
	if (skill_info[i+1].spell_pointer &&
	    (skill_info[i+1].min_level[MIN_LEVEL_CLERIC] <=
	     GET_LEVEL(ch,CLERIC_LEVEL_IND))){
	  sprintf(buf,"[%d] %s %s \n\r",
                  skill_info[i+1].min_level[MIN_LEVEL_CLERIC],
		  spells[i],how_good(ch->skills[i+1].learned));
	  if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	    break;
	  strcat(buffer, buf);
	  strcat(buffer, "\r");
	}
      page_string(ch->desc, buffer, 1);
      return;
    } 
    break;
    
  case 'D':
  case 'd':
    {
      if (!HasClass(ch, CLASS_DRUID)) {
	send_to_char("I bet you think you're a druid.\n\r", ch);
	return;
      }
      send_to_char("You can attempt any of these spells:\n\r", ch);
      for(i=0; *spells[i] != '\n'; i++)
	if (skill_info[i+1].spell_pointer &&
	    (skill_info[i+1].min_level[MIN_LEVEL_DRUID] <=
	     GET_LEVEL(ch, DRUID_LEVEL_IND))){
	  sprintf(buf,"[%d] %s %s \n\r",
		  skill_info[i+1].min_level[MIN_LEVEL_DRUID],
		  spells[i],how_good(ch->skills[i+1].learned));
	  if (strlen(buf)+strlen(buffer) > MAX_STRING_LENGTH-2)
	    break;
	  strcat(buffer, buf);
	  strcat(buffer, "\r");
	}
      page_string(ch->desc, buffer, 1);
      return;
    } 
    break;
  case 'K':
  case 'k': {
    if (!HasClass(ch, CLASS_MONK)) {
      send_to_char("I bet you think you're a monk.\n\r", ch);
      return;
    }
    
    send_to_char("You have knowledge of these skills:\n\r", ch);
    for(i=0; *spells[i] != '\n' && i < (MAX_EXIST_SPELL+1); i++)
      if (!skill_info[i+1].spell_pointer && ch->skills[i+1].learned) {
	sprintf(buf,"%-30s %s \n\r",spells[i],
		how_good(ch->skills[i+1].learned));
	if (strlen(buf)+strlen(buffer) > (MAX_STRING_LENGTH*2)-2)
	  break;
	strcat(buffer, buf);
	strcat(buffer, "\r");
      }
    page_string(ch->desc, buffer, 1);
    return;
  }
    break;
    
  default:
    send_to_char("Which class???\n\r", ch);
  }
  
  send_to_char("Go to your guildmaster to see the spells you don't have\n\r", ch);
  
}

void do_idea(struct char_data *ch, char *argument, int cmd)
{
  FILE *fl;
  char str[MAX_INPUT_LENGTH+20];
  
  if (IS_NPC(ch))	{
    send_to_char("Monsters can't have ideas - Go away.\n\r", ch);
    return;
  }
  
  /* skip whites */
  for (; isspace(*argument); argument++);
  
  if (!*argument)	{
    send_to_char
      ("That doesn't sound like a good idea to me.. Sorry.\n\r",ch);
    return;
  }
  if (!(fl = fopen(IDEA_FILE, "a")))	{
    perror ("do_idea");
    send_to_char("Could not open the idea-file.\n\r", ch);
    return;
  }
  
  sprintf(str, "**%s: %s\n", GET_NAME(ch), argument);
  
  fputs(str, fl);
  fclose(fl);
  send_to_char("Ok. Thanks.\n\r", ch);
}







void do_typo(struct char_data *ch, char *argument, int cmd)
{
  FILE *fl;
  char str[MAX_INPUT_LENGTH+20];
  
  if (IS_NPC(ch))	{
    send_to_char("Monsters can't spell - leave me alone.\n\r", ch);
    return;
  }
  
  /* skip whites */
  for (; isspace(*argument); argument++);
  
  if (!*argument)	{
    send_to_char("I beg your pardon?\n\r", 	ch);
    return;
  }
  if (!(fl = fopen(TYPO_FILE, "a")))	{
    perror ("do_typo");
    send_to_char("Could not open the typo-file.\n\r", ch);
    return;
  }
  
  sprintf(str, "**%s[%d]: %s\n",
	  GET_NAME(ch), ch->in_room, argument);
  fputs(str, fl);
  fclose(fl);
  send_to_char("Ok. thanks.\n\r", ch);
  
}





void do_bug(struct char_data *ch, char *argument, int cmd)
{
  FILE *fl;
  char str[MAX_INPUT_LENGTH+20];
  
  if (IS_NPC(ch))	{
    send_to_char("You are a monster! Bug off!\n\r", ch);
    return;
  }
  
  /* skip whites */
  for (; isspace(*argument); argument++);
  
  if (!*argument)	{
    send_to_char("Pardon?\n\r",ch);
    return;
  }
  if (!(fl = fopen(BUG_FILE, "a")))	{
    perror ("do_bug");
    send_to_char("Could not open the bug-file.\n\r", ch);
    return;
  }
  
  sprintf(str, "**%s[%d]: %s\n",
	  GET_NAME(ch), ch->in_room, argument);
  fputs(str, fl);
  fclose(fl);
  send_to_char("Ok.\n\r", ch);
}



void do_brief(struct char_data *ch, char *argument, int cmd)
{
  if (IS_NPC(ch)) {
    if (IS_SET(ch->specials.act, ACT_BRIEF))	{
      send_to_char("Brief mode off.\n\r", ch);
      REMOVE_BIT(ch->specials.act, ACT_BRIEF);
    }	
    else {
      send_to_char("Brief mode on.\n\r", ch);
      SET_BIT(ch->specials.act, ACT_BRIEF);
    }
  }
  else {
    if (IS_SET(ch->specials.act, PLR_BRIEF))	{
      send_to_char("Brief mode off.\n\r", ch);
      REMOVE_BIT(ch->specials.act, PLR_BRIEF);
    }	
    else {
      send_to_char("Brief mode on.\n\r", ch);
      SET_BIT(ch->specials.act, PLR_BRIEF);
    }
  }
  return;
}


void do_compact(struct char_data *ch, char *argument, int cmd)
{
  if (IS_NPC(ch))
    return;
  
  if (IS_SET(ch->specials.act, PLR_COMPACT))	{
    send_to_char("You are now in the uncompacted mode.\n\r", ch);
    REMOVE_BIT(ch->specials.act, PLR_COMPACT);
  }	else	{
    send_to_char("You are now in compact mode.\n\r", ch);
    SET_BIT(ch->specials.act, PLR_COMPACT);
  }
}

char *Condition(struct char_data *ch)
{
  float a, b, t;
  int   c;
  static char buf[100];
  static char *p;
  
  a = (float)GET_HIT(ch);
  b = (float)GET_MAX_HIT(ch);
  
  t = a / b;
  c = (int)100.0*t;
  
  strcpy(buf, how_good(c));
  p = buf;
  return(p);
  
}

char *Tiredness(struct char_data *ch)
{
  float a, b, t;
  int   c;
  static char buf[100];
  static char *p;
  
  a = (float)GET_MOVE(ch);
  b = (float)GET_MAX_MOVE(ch);
  
  t = a / b;
  c = (int)100.0*t;
  
  strcpy(buf, how_good(c));
  p = buf;
  return(p);
  
}

void do_group(struct char_data *ch, char *argument, int cmd)
{
  char name[256];
  struct char_data *victim, *k;
  struct follow_type *f;
  char buf[1000];
  bool found;
  
  only_argument(argument, name);
  
  if (!*name) {
    if (!IS_AFFECTED(ch, AFF_GROUP)) {
      send_to_char("But you are a member of no group?!\n\r", ch);
    } else {
      send_to_char("Your group consists of:\n\r", ch);
      if (ch->master)
	k = ch->master;
      else
	k = ch;
      
      if (IS_AFFECTED(k, AFF_GROUP)) {
	sprintf(buf, "     $N (Head of group) HP:%s MV:%s",
		Condition(k), Tiredness(k));
	act(buf,FALSE,ch, 0, k, TO_CHAR);
	
      }
      
      for(f=k->followers; f; f=f->next)
	if (IS_AFFECTED(f->follower, AFF_GROUP)) {
	  sprintf(buf, "     $N  \tHP:%s MV:%s",
		  Condition(f->follower), Tiredness(f->follower));
	  act(buf,FALSE,ch, 0, f->follower, TO_CHAR);
	}
    }
    
    return;
  }
  
  if (!(victim = get_char_room_vis(ch, name))) {
    send_to_char("No one here by that name.\n\r", ch);
  } else {
    
    if (ch->master) {
      act("You can not enroll group members without being head of a group.",
	  FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    
    found = FALSE;
    
    if (victim == ch)
      found = TRUE;
    else {
      for(f=ch->followers; f; f=f->next) {
	if (f->follower == victim) {
	  found = TRUE;
	  break;
	}
      }
    }
    
    if (found) {
      if (IS_AFFECTED(victim, AFF_GROUP)) {
	act("$n has been kicked out of $N's group!", FALSE, victim, 0, ch, TO_ROOM);
	act("You are no longer a member of $N's group!", FALSE, victim, 0, ch, TO_CHAR);
	REMOVE_BIT(victim->specials.affected_by, AFF_GROUP);
      } else {
	if (GetMaxLevel(victim)>=LOW_IMMORTAL) {
	  act("You really don't want $n in your group.", FALSE, ch, 0, 0, TO_CHAR);
	  return;
	}
	if (GetMaxLevel(ch)>=LOW_IMMORTAL) {
	  act("Now now.  That would be CHEATING!",FALSE,ch,0,0,TO_CHAR);
	  return;  
	  
	}
	act("$n is now a member of $N's group.", 
	    FALSE, victim, 0, ch, TO_ROOM);
	act("You are now a member of $N's group.", 
	    FALSE, victim, 0, ch, TO_CHAR);
	SET_BIT(victim->specials.affected_by, AFF_GROUP);
      }
    } else {
      act("$N must follow you, to enter the group", 
	  FALSE, ch, 0, victim, TO_CHAR);
    }
  }
}


void do_quaff(struct char_data *ch, char *argument, int cmd)
{
  char buf[100];
  struct obj_data *temp;
  int i;
  bool equipped;
  
  equipped = FALSE;
  
  only_argument(argument,buf);
  
  if (!(temp = get_obj_in_list_vis(ch,buf,ch->carrying))) {
    temp = ch->equipment[HOLD];
    equipped = TRUE;
    if ((temp==0) || !isname(buf, temp->name)) {
      act("You do not have that item.",FALSE,ch,0,0,TO_CHAR);
      return;
    }
  }
  
  if (!IS_IMMORTAL(ch)) {
    if (GET_COND(ch,FULL)>23) {
      act("Your stomach can't contain anymore!",FALSE,ch,0,0,TO_CHAR);
      return;
    } else {
      GET_COND(ch, FULL)+=1;
    }
  }
  
  if (temp->obj_flags.type_flag!=ITEM_POTION) {
    act("You can only quaff potions.",FALSE,ch,0,0,TO_CHAR);
    return;
  }
  
  act("$n quaffs $p.", TRUE, ch, temp, 0, TO_ROOM);
  act("You quaff $p which dissolves.",FALSE, ch, temp,0, TO_CHAR);
  
  /*  my stuff */
  if (ch->specials.fighting) {
    if (equipped) {
      if (number(1,20) > ch->abilities.dex) {
	act("$n is jolted and drops $p!  It shatters!", 
	    TRUE, ch, temp, 0, TO_ROOM);
	act("You arm is jolted and $p flies from your hand, *SMASH*",
	    TRUE, ch, temp, 0, TO_CHAR);
	if (equipped)
	  temp = unequip_char(ch, HOLD);
	extract_obj(temp);
	return;
      }
    } else {
      if (number(1,20) > ch->abilities.dex - 4) {
	act("$n is jolted and drops $p!  It shatters!", 
	    TRUE, ch, temp, 0, TO_ROOM);
	act("You arm is jolted and $p flies from your hand, *SMASH*",
	    TRUE, ch, temp, 0, TO_CHAR);
	extract_obj(temp);
	return;
      }
    }
  }
  
  for (i=1; i<4; i++)
    if (temp->obj_flags.value[i] >= 1) 
      if (!ItemMagicFailure(ch, temp->obj_flags.value[i])) /* dwarf & gnome */
	((*skill_info[temp->obj_flags.value[i]].spell_pointer)
	 ((byte) temp->obj_flags.value[0], ch, "", SPELL_TYPE_POTION, ch, 
	  temp));
  
  if (equipped)
    temp = unequip_char(ch, HOLD);
  
  extract_obj(temp);
  
  WAIT_STATE(ch, PULSE_VIOLENCE);
  
}


void do_recite(struct char_data *ch, char *argument, int cmd)
{
  char buf[100];
  struct obj_data *scroll, *obj;
  struct char_data *victim;
  int i, bits;
  bool equipped;
  
  equipped = FALSE;
  obj = 0;
  victim = 0;
  
  if (!ch->skills)
    return;
  
  argument = one_argument(argument,buf);
  
  if (!(scroll = get_obj_in_list_vis(ch,buf,ch->carrying))) {
    scroll = ch->equipment[HOLD];
    equipped = TRUE;
    if ((scroll==0) || !isname(buf, scroll->name)) {
      act("You do not have that item.",FALSE,ch,0,0,TO_CHAR);
      return;
    }
  }
  
  if (scroll->obj_flags.type_flag!=ITEM_SCROLL)  {
    act("Recite is normally used for scrolls.",FALSE,ch,0,0,TO_CHAR);
    return;
  }
  
  if (*argument) {
    bits = generic_find(argument, FIND_OBJ_INV | FIND_OBJ_ROOM |
			FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &victim, &obj);
    if (bits == 0) {
      send_to_char("No such thing around to recite the scroll on.\n\r", ch);
      return;
    }
  } else {
    victim = ch;
  }
  
  if (!HasClass(ch, CLASS_MAGIC_USER) && 
      !HasClass(ch, CLASS_CLERIC)) {
    if (ch->skills[SKILL_READ_MAGIC].learned < number(1,95)) {
      send_to_char("after several seconds of study, you realize that you can't understand this\n\r",ch);
      WAIT_STATE(ch, PULSE_VIOLENCE*3);
      return;
    }
  }
  
  act("$n recites $p.", TRUE, ch, scroll, 0, TO_ROOM);
  act("You recite $p which bursts into butterflies.",FALSE,ch,scroll,0,TO_CHAR);
  
  for (i=1; i<4; i++) {
    if (scroll->obj_flags.value[0] > 0) {  /* spells for casting */
      if (scroll->obj_flags.value[i] >= 1) {
	if (IS_SET(skill_info[scroll->obj_flags.value[i]].targets, 
		   TAR_VIOLENT) && check_peaceful(ch, 
						  "Impolite magic is banned here."))
	  continue;
	
	if (check_nomagic(ch,"The magic is blocked by unknown forces.\n\r", 
			  "The magic dissolves powerlessly"))
	  continue;
	if(!ItemMagicFailure(ch, scroll->obj_flags.value[i]))
	  ((*skill_info[scroll->obj_flags.value[i]].spell_pointer)
	   ((byte) scroll->obj_flags.value[0], ch, "", SPELL_TYPE_SCROLL, 
	    victim, obj));
      }
    } else {
      /* this is a learning scroll */
      if (scroll->obj_flags.value[0] < -30)  /* max learning is 30% */
	scroll->obj_flags.value[0] = -30;
      
      if (scroll->obj_flags.value[i] > 0) {  /* positive learning */
	if (ch->skills) {
	  if (ch->skills[scroll->obj_flags.value[i]].learned < 45)
	    ch->skills[scroll->obj_flags.value[i]].learned +=
	      (-scroll->obj_flags.value[0]);
	}
      } else {  /* negative learning (cursed */
	if (scroll->obj_flags.value[i] < 0) {  /* 0 = blank */
	  if (ch->skills) {
	    if (ch->skills[-scroll->obj_flags.value[i]].learned > 0)
	      ch->skills[-scroll->obj_flags.value[i]].learned +=
		scroll->obj_flags.value[0];
	    ch->skills[-scroll->obj_flags.value[i]].learned =
	      MAX(0, ch->skills[scroll->obj_flags.value[i]].learned);
	  }
	}
      }
    }
  }
  if (equipped)
    scroll = unequip_char(ch, HOLD);
  
  extract_obj(scroll);
  
}



void do_use(struct char_data *ch, char *argument, int cmd)
{
  char buf[100];
  struct char_data *tmp_char;
  struct obj_data *tmp_object, *stick;
  struct char_data *f;
  int bits, found = 0;
  struct follow_type *fol;
  struct skill_data    *spellp;
  
  argument = one_argument(argument,buf);
  for(fol = ch->followers; fol; fol = fol->next)
    if(IS_SET(fol->follower->specials.act, ACT_FIGURINE)) {
      f = fol->follower;
      if(isname(buf, GET_NAME(f))) {
	found = 1;
	break;
      }
    }
  
  if(!found)  
    if ((ch->equipment[HOLD] == 0 ||
	 !isname(buf, ch->equipment[HOLD]->name))) {
      act("You do not hold that item in your hand.",FALSE,ch,0,0,TO_CHAR);
      return;
    }
  
  if (!IS_PC(ch) && ch->master) {
    act("$n looks confused, and shrugs helplessly.", FALSE, ch, 0, 0, TO_ROOM);
    return;
  }
  
  if (RIDDEN(ch)) {
    return;
  }
  if(found) {
    char buf[255];
    
    act("$n shakes and begins to shrink.", FALSE, f, 0, 0, TO_ROOM);
    REMOVE_BIT(f->specials.affected_by, AFF_CHARM);
    stop_follower(f);
    char_from_room(f);
    char_to_room(f, 3);
    obj_to_char(f->link, ch);
    sprintf(buf, "You now have %s.\n", f->link->short_description);
    send_to_char(buf, ch);
    return;
  }
  
  
  stick = ch->equipment[HOLD];
  
  /* if they have no more charges, they go poof. */
  
  if(stick->obj_flags.type_flag == ITEM_STAFF ||
     stick->obj_flags.type_flag == ITEM_WAND) {
    if(stick->obj_flags.value[2] < 1) {
      act("$p disintegrates in $n's hands.",TRUE, ch, stick, 0, TO_ROOM);
      act("$p disintegrates in your hands!", FALSE, ch, stick, 0, TO_CHAR);
      stick = unequip_char(ch, HOLD);
      extract_obj(stick);
      return;
    }
  }
  
  if (stick->obj_flags.type_flag == ITEM_STAFF)  {
    
    spellp = skill_info + (stick->obj_flags.value[3]);
    if (IS_SET(spellp->targets, TAR_VIOLENT) &&
	check_peaceful(ch, "Impolite magic is banned here."))
      return;
    
    
    act("$n taps $p three times on the ground.",TRUE, ch, stick, 0,TO_ROOM);
    act("You tap $p three times on the ground.",FALSE,ch, stick, 0,TO_CHAR);
    if (stick->obj_flags.value[2] > 0) {  /* Is there any charges left? */
      stick->obj_flags.value[2]--;
      
      if (check_nomagic(ch,"The magic is blocked by unknown forces.", 
			"The magic is blocked by unknown forces."))
	return;
      if(!ItemMagicFailure(ch, stick->obj_flags.value[3]))
	((*skill_info[stick->obj_flags.value[3]].spell_pointer)
	 ((byte) stick->obj_flags.value[0], ch, "", SPELL_TYPE_STAFF, 0, 0));
      WAIT_STATE(ch, PULSE_VIOLENCE);
    } else {
      send_to_char("The staff seems powerless.\n\r", ch);
    }
  } else if (stick->obj_flags.type_flag == ITEM_WAND) {
    bits = generic_find(argument, FIND_CHAR_ROOM | FIND_OBJ_INV | 
			FIND_OBJ_ROOM | FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);
    if (bits) {
      struct skill_data	*spellp;
      
      spellp = skill_info + (stick->obj_flags.value[3]);
      
      if (bits == FIND_CHAR_ROOM) {
	act("$n points $p at $N.", TRUE, ch, stick, tmp_char, TO_ROOM);
	act("You point $p at $N.",FALSE,ch, stick, tmp_char, TO_CHAR);
      } else {
	act("$n points $p at $P.", TRUE, ch, stick, tmp_object, TO_ROOM);
	act("You point $p at $P.",FALSE,ch, stick, tmp_object, TO_CHAR);
      }
      
      if (IS_SET(spellp->targets, TAR_VIOLENT) &&
	  check_peaceful(ch, "Impolite magic is banned here."))
	return;
      
      if (stick->obj_flags.value[2] > 0) { /* Is there any charges left? */
	stick->obj_flags.value[2]--;
	
	if (check_nomagic(ch,"The magic is blocked by unknown forces.", 
			  "The magic is blocked by unknown forces.")) 
	  return;
	if(!ItemMagicFailure(ch, stick->obj_flags.value[0]))
	  ((*spellp->spell_pointer)
	   ((byte) stick->obj_flags.value[0], ch, "", SPELL_TYPE_WAND, 
	    tmp_char, tmp_object));
	WAIT_STATE(ch, PULSE_VIOLENCE);
	
      } else {
	send_to_char("The wand seems powerless.\n\r", ch);
      }
    } else {
      send_to_char("What should the wand be pointed at?\n\r", ch);
    }
  } else if(IS_SET(stick->obj_flags.extra_flags, ITEM_FIGURINE)) {
    char buf[255];
    for(fol = ch->followers; fol; fol = fol->next)
      if(IS_SET(fol->follower->specials.act, ACT_FIGURINE)) {
	send_to_char("The beast you attempted to summon will not come to this plane of existance.\n", ch);
	return;
      }
    
    
    if(number(1,26) == 20) { /* One in 20 chance of it destroying itself */
      sprintf(buf, "%s shudders and shakes, finally cracking into a million pieces.\n", stick->short_description);
      send_to_char(buf, ch);
      stick = unequip_char(ch, HOLD);
      char_from_room(stick->link);
      extract_char(stick->link);
      return;
    }
    if(GET_HIT(stick->link) < GET_MAX_HIT(stick->link)/7) {
      send_to_char("The beast refuses to step foot into this plane of existance until it has healed.\n\r", ch);
      return;
    }
    stick = unequip_char(ch, HOLD);
    act("$p starts to shudder an shake in $n's hands.", FALSE, ch, stick, 0, TO_ROOM);
    sprintf(buf, "%s shakes in your hands and grows into %s.\n", stick->short_description, stick->link->player.short_descr);
    send_to_char(buf, ch);
    SET_BIT(stick->link->specials.affected_by, AFF_CHARM);
    char_from_room(stick->link);
    char_to_room(stick->link, ch->in_room);
    add_follower(stick->link, ch);
  }
  
  
  else {
    send_to_char("Use is normally only for wand's and staff's.\n\r", ch);
  }
}

void do_plr_noshout(struct char_data *ch, char *argument, int cmd)
{
  char buf[128];
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, buf);
  
  if (!*buf) {
    if (IS_SET(ch->specials.act, PLR_DEAF)) {
      send_to_char("You can now hear shouts again.\n\r", ch);
      REMOVE_BIT(ch->specials.act, PLR_DEAF);
    } else {
      send_to_char("From now on, you won't hear shouts.\n\r", ch);
      SET_BIT(ch->specials.act, PLR_DEAF);
    }
  } else {
    send_to_char("Only the gods can shut up someone else. \n\r",ch);
  }
  
}

void do_show_exits(struct char_data *ch, char *argument, int cmd)
{
  char buf[128];
  
  only_argument(argument, buf);
  
  if (IS_NPC(ch)) {
    if (!*buf) {
      if (IS_SET(ch->specials.act, ACT_SHOWEXITS)) {
        REMOVE_BIT(ch->specials.act, ACT_SHOWEXITS);
        send_to_char("You will no longer see exits in room displays\n\r",ch);
      } 
      else {
        SET_BIT(ch->specials.act, ACT_SHOWEXITS);
        send_to_char("You will now see exits in room displays\n\r",ch);
      }
    }
  }
  else {
    if (!*buf) {
      if (IS_SET(ch->specials.act, PLR_SHOWEXITS)) {
        REMOVE_BIT(ch->specials.act, PLR_SHOWEXITS);
        send_to_char("You will no longer see exits in room displays\n\r",ch);
      } 
      else {
        SET_BIT(ch->specials.act, PLR_SHOWEXITS);
        send_to_char("You will now see exits in room displays\n\r",ch);
      }
    }
  }  
}

void do_alias(struct char_data *ch, char *arg, int cmd)
{
  char buf[512], buf2[512];
  char *p, *p2;
  int i, num;
  
  if (cmd == 260) {
    for (;*arg==' ';arg++);
    if (!*arg) {  /* print list of current aliases */
      if (ch->specials.A_list) {
	for(i=0;i<10;i++) {
	  if (ch->specials.A_list->com[i]) {
	    sprintf(buf,"[%d] %s\n\r",i, ch->specials.A_list->com[i]);
	    send_to_char(buf,ch);
	  }
	}
      } else {
	send_to_char("You have no aliases defined!\n\r", ch);
	return;
      }
    } else {  /* assign a particular alias */
      if (!ch->specials.A_list) {
	ch->specials.A_list = (Alias *)malloc(sizeof(Alias));
	for (i=0;i<10;i++)
	  ch->specials.A_list->com[i] = (char *)0;
      }
      strcpy(buf, arg);
      p = strtok(buf," ");
      num = atoi(p);
      if (num < 0 || num > 9) {
	send_to_char("numbers between 0 and 9, please \n\r", ch);
	return;
      }
      if (GET_ALIAS(ch,num)) {
	free(GET_ALIAS(ch, num));
	GET_ALIAS(ch, num) = 0;
      }
      /*
	testing
	*/
      p = strtok(0," ");  /* get the command string */
      if (!p) {
	send_to_char("Need to supply a command to alias bu--------dee\n\r",ch);
	return;
      }
      p2 = strtok(p," ");  /* check the command, make sure its not an alias
			    */
      if (!p2) {
	send_to_char("Hmmmmm\n\r", ch);
	return;
      }
      if (*p2 >= '0' && *p2 <= '9') {
	send_to_char("Can't use an alias inside an alias\n\r", ch);
	return;
      }
      if (strncmp(p2,"alias",strlen(p2))==0) {
	send_to_char("Can't use the word 'alias' in an alias\n\r", ch);
	return;
      }
      /*
	verified.. now the copy.
	*/
      if (strlen(p) <= 80) {
        strcpy(buf2, arg);  /* have to rebuild, because buf is full of
			       nulls */
	p = strchr(buf2,' ');
	p++;
	ch->specials.A_list->com[num] = (char *)malloc(strlen(p)+1);
	strcpy(ch->specials.A_list->com[num], p);
      } else {
	send_to_char("alias must be less than 80 chars, lost\n\r", ch);
	return;
      }
    }
  } else {   /* execute this alias */
    num = cmd - 260;  /* 260 = alias */
    if (num == 10)
      num = 0;
    if (ch->specials.A_list) {
      if (GET_ALIAS(ch, num)) {
	strcpy(buf, GET_ALIAS(ch, num));
	if (*arg) {
	  sprintf(buf2,"%s%s",buf,arg);
	  command_interpreter(ch, buf2);
	} else {
	  command_interpreter(ch, buf);
	}
      }
    }
  }
}

int Dismount(struct char_data *ch, struct char_data *h, int pos)
{
  
  MOUNTED(ch) = 0;
  RIDDEN(h) = 0;
  GET_POS(ch) = pos;
  
  check_falling(ch);
  
}

void do_mount(struct char_data *ch, char *arg, int cmd)
{
  char buf[256];
  char name[112];
  int check;
  struct char_data *horse;
  
  
  if (cmd == 276 || cmd == 278) {
    only_argument(arg, name);
    
    if (!(horse = get_char_room_vis(ch, name))) {
      send_to_char("Mount what?\n\r", ch);
      return;
    }
    
    if (!IsHumanoid(ch)) {
      send_to_char("You can't ride things!\n\r", ch);
      return;
    }
    
    if (IsRideable(horse)) {
      
      if (GET_POS(horse) < POSITION_STANDING) {
	send_to_char("Your mount must be standing\n\r", ch);
	return;
      }
      
      if (RIDDEN(horse)) {
	send_to_char("Already ridden\n\r", ch);
	return;
      } else if (MOUNTED(ch)) {
	send_to_char("Already riding\n\r", ch);
	return;
      }
      
      check = MountEgoCheck(ch, horse);
      if (check > 5) {
	act("$N snarls and attacks!", 
	    FALSE, ch, 0, horse, TO_CHAR);
	act("as $n tries to mount $N, $N attacks $n!",
	    FALSE, ch, 0, horse, TO_NOTVICT);
	WAIT_STATE(ch, PULSE_VIOLENCE);
	hit(horse, ch, TYPE_UNDEFINED);
	return;
      } else if (check > -1) {
	act("$N moves out of the way, you fall on your butt", 
	    FALSE, ch, 0, horse, TO_CHAR);
	act("as $n tries to mount $N, $N moves out of the way",
	    FALSE, ch, 0, horse, TO_NOTVICT);
	WAIT_STATE(ch, PULSE_VIOLENCE);
	GET_POS(ch) = POSITION_SITTING;
	return;
      }
      
      
      if (RideCheck(ch, 50)) {
	act("You hop on $N's back", FALSE, ch, 0, horse, TO_CHAR);
	act("$n hops on $N's back", FALSE, ch, 0, horse, TO_NOTVICT);
	act("$n hops on your back!", FALSE, ch, 0, horse, TO_VICT);
	MOUNTED(ch) = horse;
	RIDDEN(horse) = ch;
	GET_POS(ch) = POSITION_MOUNTED;
	REMOVE_BIT(ch->specials.affected_by, AFF_SNEAK);
      } else {
	act("You try to ride $N, but falls on $s butt", 
	    FALSE, ch, 0, horse, TO_CHAR);
	act("$n tries to ride $N, but falls on $s butt", 
	    FALSE, ch, 0, horse, TO_NOTVICT);
	act("$n tries to ride you, but falls on $s butt", 
	    FALSE, ch, 0, horse, TO_VICT);
	GET_POS(ch) = POSITION_SITTING;
	WAIT_STATE(ch, PULSE_VIOLENCE*2);
      }
    } else {
      send_to_char("You can't ride that!\n\r", ch);
      return;
    }
  } else if (cmd == 277) {
    horse = MOUNTED(ch);
    
    act("You dismount from $N", FALSE, ch, 0, horse, TO_CHAR);
    act("$n dismounts from $N", FALSE, ch, 0, horse, TO_NOTVICT);
    act("$n dismounts from you", FALSE, ch, 0, horse, TO_VICT);
    Dismount(ch, MOUNTED(ch), POSITION_STANDING);
    return;
  }
  
}

void do_split(struct char_data *ch, char *arg, int cmd)
{
  int num, share, count=0;
  struct follow_type *f;
  char amnt[100];
  struct char_data *k;
  char buf[200];
  
  if (!IS_PC(ch))
    return;
  
  for (;*arg==' ';arg++);
  
  arg=one_argument(arg, amnt);	
  
  
  if (!IS_AFFECTED(ch, AFF_GROUP)) {
    send_to_char("But you are a member of no group?!\n\r", ch);
    return;
  }
  
  if (is_number(amnt)) {
    num = atoi(amnt);
    
    if (num < 1) {
      return;      
    }
    
    
    if (num > GET_GOLD(ch)) {
      send_to_char("You don't have that much gold\n\r", ch);
      return;
    }
    
    GET_GOLD(ch) -= num;
    
    /* count people in group in room */
    count = 0;
    if (ch->master) {
      k = ch->master;
      if (k->in_room == ch->in_room)
	count++;
    } else {
      k = ch;
      count++;
    }
    
    for(f=k->followers; f; f=f->next)
      if (IS_AFFECTED(f->follower, AFF_GROUP)) {
	if (f->follower->in_room == ch->in_room)
	  count++;
      }
    
    if (count == 0)
      return;
    
    /* divide into equal shares */
    share = num / count;
    /* give 1 share person */
    if (ch->master) {
      k = ch->master;
      if (k->in_room == ch->in_room) {
	GET_GOLD(k) += share;
	num -= share;
	sprintf(buf, "$N gives you %d gold", share);
	act(buf, 0, k, 0, ch, TO_CHAR);
      }
    } else {
      k = ch;
      GET_GOLD(k) += share;
      num -= share;
      sprintf(buf, "You keep %d gold\n\r", share);
      send_to_char(buf, ch);
    }
    
    for(f=k->followers; f; f=f->next)
      if (IS_AFFECTED(f->follower, AFF_GROUP)) {
	if (f->follower->in_room == ch->in_room) {
	  if (f->follower != ch) {
	    sprintf(buf, "$N gives you %d gold", share);
	    act(buf, 0, f->follower, 0, ch, TO_CHAR);
	    sprintf(buf, "You give $N %d gold", share);
	    act(buf, 0, ch, 0, f->follower, TO_CHAR);
	  } else {
	    sprintf(buf, "You keep %d gold\n\r", share);
	    send_to_char(buf, ch);	  
	  }
	  GET_GOLD(f->follower) += share;
	  num -= share;
	}
      }
    
    /* give rest to ch */
    GET_GOLD(ch) += num;
    sprintf(buf, "And you keep the %d gold left over.\n\r", num);
    send_to_char(buf, ch);	  
    
    
  } else {
    send_to_char("Argument must be a number.\n\r", ch);
  }
  
}

void do_gname(struct char_data *ch, char *arg, int cmd)
{
  int count;
  struct follow_type *f;
  
  /* check to see if this person is the master */
  if (ch->master || !IS_AFFECTED(ch, AFF_GROUP)) {
    send_to_char("You aren't the master of a group.\n\r", ch);
    return;
  }
  /* check to see at least 2 pcs in group      */
  for(count=0,f=ch->followers;f;f=f->next) {
    if (IS_AFFECTED(f->follower, AFF_GROUP) && IS_PC(f->follower)) {
      count++;
    }
  }
  if (count < 1) {
    send_to_char("You can't have a group with just one player!\n\r", ch);
    return;
  }
  /* free the old ch->specials.gname           */
  if (ch->specials.gname) free(ch->specials.gname);
  /* set ch->specials.gname to the argument    */
  for (;*arg==' ';arg++);
  send_to_char("Setting your group name to :", ch);
  send_to_char(arg, ch);
  ch->specials.gname = strdup(arg);
  
}

void do_donate(struct char_data *ch, char *argument, int cmd)
{
  char arg[100], buf[100], newarg[100];
  struct obj_data *tmp_object;
  int num, p, count;
  
  /*
   *   get object name & verify
   */
  
  only_argument(argument, arg);
  if (*arg) {
    if (getall(arg,newarg)!=NULL) {
      num = -1;
      strcpy(arg,newarg);
    } else if ((p = getabunch(arg,newarg))!=NULL) {
      num = p;
      strcpy(arg,newarg);
    } else {
      num = 1;
    }
  } else {
    send_to_char("Donate what?\n\r",ch);
    return;
  }
  count = 0;
  while (num != 0) {
    tmp_object = get_obj_in_list_vis(ch, arg, ch->carrying);
    if (tmp_object) {
      if (IS_OBJ_STAT(tmp_object,ITEM_NODROP)) {
        send_to_char
          ("You can't let go of it, it must be CURSED!\n\r", ch);
        return;
      }
      obj_from_char(tmp_object);
      obj_to_room(tmp_object,99);
      if (num > 0) num--;
      count++;
    } else {
      num = 0;
    }
  }
  if (count > 1) {
    sprintf(buf, "You donate %s (%d).\n\r", arg, count);
    act(buf, 1, ch, 0, 0, TO_CHAR);
    sprintf(buf, "$n donates %s.\n\r", arg);
    act(buf, 1, ch, 0, 0, TO_ROOM);
    sprintf(buf, "%s has just donated %s.\n\r", GET_NAME(ch), arg);
    send_to_room(buf, 99);
    act("Thank you for your most generous donation.",
        FALSE, ch, 0, 0, TO_CHAR);
  } else if (count == 1) {
    sprintf(buf, "You donate %s \n\r", arg);
    act(buf, 1, ch, 0, 0, TO_CHAR);
    sprintf(buf, "$n donates %s.\n\r", arg);
    act(buf, 1, ch, 0, 0, TO_ROOM);
    sprintf(buf, "%s has just donated %s.\n\r", GET_NAME(ch), arg);
    send_to_room(buf, 99);
    act("Thank you for your most generous donation.",
        FALSE, ch, 0, 0, TO_CHAR);
  } else {
    send_to_char("You don't have anything like that\n\r", ch);
  }
  
  return;
}

void do_auto(struct char_data *ch, char *argument, int cmd)
{
  
  char arg[MAX_INPUT_LENGTH];
  int keyword_no;
  static char *keywords[]= { 
    "loot",
    "split",
    "",
    "\n" };
  
  only_argument(argument, arg);
  
  keyword_no = search_block(arg, keywords, FALSE);
  
  switch(keyword_no) {
    
  case 0: {
    if(ch->specials.loot) {   /* Remove auto if already set */
      ch->specials.loot = FALSE;
      send_to_char("Auto Looting is OFF\n\r",ch);
    }
    else {
      ch->specials.loot = TRUE;
      send_to_char("Auto Looting is ON\n\r",ch);
    }
  }
    break;
  case 1: {
    if(ch->specials.split) {   /* Remove auto if already set */
      ch->specials.split = FALSE;
      send_to_char("Auto Splitting is OFF\n\r",ch);
    }
    else {
      ch->specials.split = TRUE;
      send_to_char("Auto Splitting is ON\n\r",ch);
    }
  }
    break;
  case 2: {
    send_to_char("Yes, and WHAT exactly would you like set to auto?\n\r",ch);
  }
    break;
    
  default: {
    send_to_char("Auto WHAT???  I don't think I understood that one.\n\r",ch);
  }
    break;
    
  }
  
  return;
}

void do_prompt(struct char_data *ch, char *argument, int cmd) 
{
  char *arg;
  int prompt=0;
  
  for(;*argument == ' '; argument++);
  arg = lower(argument);
  
  if(arg[0] == '-') {
    if (GetMaxLevel(ch) >= LOW_IMMORTAL) { 
      if (index(arg,'r') != NULL) {
	SET_BIT(prompt, PROMPT_R);
      }
      if (index(arg,'f') != NULL) {
	SET_BIT(prompt, PROMPT_F);
      }
      if (index(arg,'s') != NULL) {
	SET_BIT(prompt, PROMPT_S);
      }
    } else {
      if (index(arg,'h') != NULL) {
	SET_BIT(prompt, PROMPT_H);
      }
      if (index(arg,'m') != NULL) {
	SET_BIT(prompt, PROMPT_M);
      }
      if (index(arg,'v') != NULL) {
	SET_BIT(prompt, PROMPT_V);
      }
    }
    if(!prompt) {
      send_to_char("Hey doofus, let us see some valid statistic options!\n\r",
		   ch);
    } else {
      send_to_char("Prompt set.\n\r", ch);
      ch->specials.prompt=prompt;
    }
    return;
  } else {
    send_to_char("format: prompt -<statistics>\n\r", ch);
    if(GetMaxLevel(ch) >= LOW_IMMORTAL) {
      send_to_char("Immortals may specify the following statistics:\n\r", ch);
      send_to_char("(R)oom Number, Room (F)lags, (S)ector Type\n\r", ch);
      send_to_char("Immortals may not specify Mortal statistics.\n\r", ch);
      send_to_char("for example: prompt -RSF\n\r", ch);
    } else {
      send_to_char("Where <statistics> is one or more of the following:\n\r",
		   ch);
      send_to_char("(H)ealth, (M)ana (V)itality\n\r", ch);
      send_to_char("for example: prompt -HMV\n\r", ch);
    }
  }
}
