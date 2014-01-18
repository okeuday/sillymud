/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <string.h>

#include "protos.h"

/* extern variables */

extern struct descriptor_data *descriptor_list;
extern struct dex_app_type dex_app[];
extern char *att_kick_hit_room[];
extern char *att_kick_hit_victim[];
extern char *att_kick_hit_ch[];
extern char *att_kick_miss_room[];
extern char *att_kick_miss_victim[];
extern char *att_kick_miss_ch[];
extern char *att_kick_kill_room[];
extern char *att_kick_kill_victim[];
extern  char *att_kick_kill_ch[];


void do_hit(struct char_data *ch, char *argument, int cmd)
{
  char arg[80];
  struct char_data *victim;
  
  if (check_peaceful(ch,
		     "You feel too peaceful to contemplate violence.\n\r"))
    return;
  
  only_argument(argument, arg);
  
  if (*arg) {
    victim = get_char_room_vis(ch, arg);
    if (victim) {
      if (victim == ch) {
	send_to_char("You hit yourself..OUCH!.\n\r", ch);
	act("$n hits $mself, and says OUCH!", FALSE, ch, 0, victim, TO_ROOM);
      } else {
	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == victim)) {
	  act("$N is just such a good friend, you simply can't hit $M.",
	      FALSE, ch,0,victim,TO_CHAR);
	  return;
	}
	if ((GET_POS(ch)>=POSITION_STANDING) &&
	    (!ch->specials.fighting)) {
	  hit(ch, victim, TYPE_UNDEFINED);
	  WAIT_STATE(ch, PULSE_VIOLENCE+2);
	  
	} else {
	  if (victim != ch->specials.fighting) {
	    if (ch->skills && ch->skills[SKILL_SWITCH_OPP].learned) {
	      if (number(1,101) < ch->skills[SKILL_SWITCH_OPP].learned) {
	      stop_fighting(ch);
	      if (victim->attackers < 5) 
		set_fighting(ch, victim);
	      else {
		send_to_char("There's no room to switch!\n\r", ch);
	      }
	      send_to_char("You switch opponents\n\r", ch);
	      act("$n switches targets", FALSE, ch, 0, 0, TO_ROOM);
	      WAIT_STATE(ch, PULSE_VIOLENCE+2);
	    } else {
	      send_to_char("You try to switch opponents, but you become confused!\n\r", ch);
	      stop_fighting(ch);
	      LearnFromMistake(ch, SKILL_SWITCH_OPP, 0, 95);
	      WAIT_STATE(ch, PULSE_VIOLENCE*2);
	    }
	    } else {
	      send_to_char("You do the best you can!\n\r",ch);
	    }
	  } else {
	      send_to_char("You do the best you can!\n\r",ch);
	  }
	}
      }
    } else {
      send_to_char("They aren't here.\n\r", ch);
    }
  } else {
    send_to_char("Hit who?\n\r", ch);
  }
}



void do_kill(struct char_data *ch, char *argument, int cmd)
{
  static char arg[MAX_INPUT_LENGTH];
  struct char_data *victim;
  
  if ((GetMaxLevel(ch) < SILLYLORD) || IS_NPC(ch)) {
    do_hit(ch, argument, 0);
    return;
  }
  
  only_argument(argument, arg);
  
  if (!*arg) {
    send_to_char("Kill who?\n\r", ch);
  } else {
    if (!(victim = get_char_room_vis(ch, arg)))
      send_to_char("They aren't here.\n\r", ch);
    else if (ch == victim)
      send_to_char("Your mother would be so sad.. :(\n\r", ch);
    else {
      act("You chop $M to pieces! Ah! The blood!", FALSE, ch, 0, victim, TO_CHAR);
      act("$N chops you to pieces!", FALSE, victim, 0, ch, TO_CHAR);
      act("$n brutally slays $N", FALSE, ch, 0, victim, TO_NOTVICT);
      raw_kill(victim);
    }
  }
}



void do_backstab(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char name[256];
  byte percent, base=0;
  bool penalized=FALSE;
  
  if (check_peaceful(ch, "Naughty, naughty.  None of that here.\n\r"))
    return;
  
  only_argument(argument, name);
  
  if (!(victim = get_char_room_vis(ch, name))) {
    send_to_char("Backstab who?\n\r", ch);
    return;
  }
  
  if (victim == ch) {
    send_to_char("How can you sneak up on yourself?\n\r", ch);
    return;
  }

  if (!HasClass(ch, CLASS_THIEF)) {
    send_to_char("You're no thief!\n\r", ch);
    return;
  }
  
  if (!ch->equipment[WIELD]) {
    send_to_char("You need to wield a weapon, to make it a succes.\n\r",ch);
    return;
  }

  if (MOUNTED(ch)) {
    send_to_char("How can you surprise someone with all the racket that beast makes?\n\r", ch);
    return;
  }

  if (ch->attackers) {
    send_to_char("There's no way to reach that back while you're fighting!\n\r", ch);
    return;
  }
  
  if (victim->attackers >= 3) {
    send_to_char("You can't get close enough to them to backstab!\n\r", ch);
    return;
  }
  
  if (ch->equipment[WIELD]->obj_flags.value[3] != 11 &&
      ch->equipment[WIELD]->obj_flags.value[3] != 1  &&
      ch->equipment[WIELD]->obj_flags.value[3] != 10) {
    send_to_char("Only piercing or stabbing weapons can be used for backstabbing.\n\r",ch);
    return;
  }
  
  if (ch->specials.fighting) {
    send_to_char("You're too busy fighting to backstab\n\r", ch);
    return;
  }
  

  if (victim->specials.fighting) {
    base = 0;
  } else {
    base = 4;
  }

  if (IS_NPC(victim) && IS_SET(victim->specials.act, ACT_HUGE)) {
    if (GET_RACE(ch) != RACE_GIANT && !IS_AFFECTED(ch,AFF_FLYING)) {
      act("$N is MUCH too large to backstab", FALSE, ch, 0, victim, TO_CHAR);
      return;
    }
  }

  percent=number(1,101); /* 101% is a complete failure */

  if (IS_AFFECTED(ch, AFF_SILENCE)) {
    percent -= number(1,15);
  }

  if(IS_AFFECTED(ch, AFF_HIDE) || IS_AFFECTED(ch, AFF_SNEAK)) {
    if(!IS_AFFECTED(ch,AFF_FLYING))
      percent -= number(1,10);
  }

  if(ch->skills && ch->skills[SKILL_SNEAK].learned) {
    if(IS_SET(ch->specials.act,ACT_AGGRESSIVE) ||
       IS_SET(ch->specials.act, ACT_META_AGG)) {
      if(percent > ch->skills[SKILL_SNEAK].learned) { /* failed sneak */
	send_to_char("Damn, you think they heard you.\n\r",ch);
	hit(ch, victim, TYPE_UNDEFINED);
	WAIT_STATE(ch, PULSE_VIOLENCE*2);
	return;
      } else {
	if(IS_AFFECTED(ch,AFF_FLYING))
	  act("You quietly glide up behind $N.",FALSE, ch, 0, victim, TO_CHAR);
	else 
	 act("You quietly approach $N from behind.",FALSE,ch,0,victim,TO_CHAR);
      }
    } else if(number(1,100) < (GetMaxLevel(ch) - GetMaxLevel(victim) + 50)) {
      if(percent > ch->skills[SKILL_SNEAK].learned) { /* failed sneak */
	send_to_char("Damn, you think they almost noticed you.\n\r", ch);
	penalized = TRUE;
      }
    }

    percent = number(1,101);

    if(penalized) {
      base -=2;
      percent += 20;
    }
      
    if (ch->skills && ch->skills[SKILL_BACKSTAB].learned) {
      if (percent > ch->skills[SKILL_BACKSTAB].learned) {
	if(AWAKE(victim))
	  if(!number(0,1))
	    act("Drats, you meant to insert $p a little more to the left!",
		FALSE,ch,ch->equipment[WIELD],0,TO_CHAR);
	  else
	    act("Drats, you meant to insert $p a little more to the right!",
		FALSE,ch,ch->equipment[WIELD],0,TO_CHAR);
	if (AWAKE(victim)) {
	  damage(ch, victim, 0, SKILL_BACKSTAB);
	  AddHated(victim, ch);
	} else {
	  base += 2;
	  GET_HITROLL(ch) += base;
	  hit(ch,victim,SKILL_BACKSTAB);
	  GET_HITROLL(ch) -= base;
	  AddHated(victim, ch);
	}
	LearnFromMistake(ch, SKILL_BACKSTAB, 0, 95);
      } else {
	GET_HITROLL(ch) += base;
	hit(ch,victim,SKILL_BACKSTAB);
	GET_HITROLL(ch) -= base;
	AddHated(victim, ch);
	if (IS_PC(ch) && IS_PC(victim))
	  GET_ALIGNMENT(ch)-=50;
      }
    } else {
      send_to_char("Hey, you don't know how to backstab anyone!\n\r", ch);
      send_to_char("But you sure did piss them off trying.\n\r", ch);
      damage(ch, victim, 0, SKILL_BACKSTAB);
      AddHated(victim, ch);
    }
  } else {
    send_to_char("Hey, you need to be able to SNEAK up on someone first!\n\r",
		 ch);
  }
  WAIT_STATE(ch, PULSE_VIOLENCE*2);
}



void do_order(struct char_data *ch, char *argument, int cmd)
{
  char name[100], message[256];
  char buf[256];
  char tbuf[80];
  bool found = FALSE;
  int org_room;
  struct char_data *victim;
  struct follow_type *k;
  

  if (apply_soundproof(ch))
    return;

  half_chop(argument, name, message);
  
  if (!*name || !*message)
    send_to_char("Order who to do what?\n\r", ch);
  else if (!(victim = get_char_room_vis(ch, name)) &&
	   str_cmp("follower", name) && str_cmp("followers", name))
    send_to_char("That person isn't here.\n\r", ch);
  else if (ch == victim)
    send_to_char("You obviously suffer from Multiple Personality Disorder.\n\r", ch);
  
  else {
    if (IS_AFFECTED(ch, AFF_CHARM)) {
      send_to_char("Your superior would not aprove of you giving orders.\n\r",ch);
      return;
    }
    
    if (victim) {
	if (check_soundproof(victim))
	  return;
      sprintf(buf, "$N orders you to '%s'", message);
      act(buf, FALSE, victim, 0, ch, TO_CHAR);
      act("$n gives $N an order.", FALSE, ch, 0, victim, TO_ROOM);
      
      if ( (victim->master!=ch) || !IS_AFFECTED(victim, AFF_CHARM) )
	if (RIDDEN(victim) == ch) {
	  int check;
	  check = MountEgoCheck(ch, victim);
	  if (check > 5) {
	    if (RideCheck(ch, -5)) {
	      act("$n has an indifferent look.", FALSE, victim, 0, 0, TO_ROOM);
	    } else {
	      Dismount(ch, victim, POSITION_SITTING);
	      act("$n gets pissed and $N falls on $S butt!", 
		  FALSE, victim, 0, ch, TO_NOTVICT);
	      act("$n gets pissed you fall off!", 
		  FALSE, victim, 0, ch, TO_VICT);
	    }
	  } else if (check > 0) {
	    act("$n has an indifferent look.", FALSE, victim, 0, 0, TO_ROOM);
	  } else {
	    send_to_char("Ok.\n\r", ch);
	    command_interpreter(victim, message);
	  }
	} else {
	  act("$n has an indifferent look.", FALSE, victim, 0, 0, TO_ROOM);
	}
      else {
	send_to_char("Ok.\n\r", ch);
	WAIT_STATE(victim, (21-GET_CHR(ch)));
	
	command_interpreter(victim, message);
      }
    } else {  /* This is order "followers" */
      sprintf(buf, "$n issues the order '%s'.", message);
      act(buf, FALSE, ch, 0, victim, TO_ROOM);
      
      org_room = ch->in_room;
      
      for (k = ch->followers; k; k = k->next) {
        if (org_room == k->follower->in_room)
          if (IS_AFFECTED(k->follower, AFF_CHARM)) {
            found = TRUE;
	  }
      }

      if(found) {
	for (k = ch->followers; k; k = k->next) {
	  if (org_room == k->follower->in_room)
	    if (IS_AFFECTED(k->follower, AFF_CHARM)) {
	      command_interpreter(k->follower, message);
	    }
	}
	send_to_char("Ok.\n\r", ch);
      } else {
	send_to_char("Nobody here is a loyal subject of yours!\n\r", ch);
      }
    }
  }
}



void do_flee(struct char_data *ch, char *argument, int cmd)
{
  int i, attempt, loose, die, percent, charm, nmbr, f, panic, j, tries, badroom;
  bool found;
  void gain_exp(struct char_data *ch, int gain);
  int special(struct char_data *ch, int cmd, char *arg);
  char buf[255];
  char buf2[255];

  argument = one_argument(argument, buf);

  if(IS_PC(ch) || IS_SET(ch->specials.act, ACT_POLYSELF)) {
    if((ch->desc->wait)>1) {      /* Someone is in a wait state */
      send_to_char("Your head is still spinning too much to flee!\n\r", ch);
      sprintf(buf, "You must wait %d pulses\n\r", ch->desc->wait);
      send_to_char(buf, ch);
      return;
    }
    
    if(IS_AFFECTED2(ch, AFF2_BERSERK)) {
      send_to_char("Flee now?  But your opponent aint dead!  Killl, kill, kill.\n\r",ch);
      return;
    }

    if (strlen(buf) != 0) {  /* If flee takes an argument, it is a set command */
       nmbr = atoi(buf); 
       if((nmbr < 1) || (nmbr > 5)) {
         send_to_char("Please choose a flee setting of 1 to 5 rooms.\n\r", ch);
         return;
       }
       sprintf(buf2,"You used to flee %d rooms when you ran away.\n\r",ch->specials.flee);
       send_to_char(buf2,ch);
       ch->specials.flee = nmbr;
       sprintf(buf2,"You will NOW flee %d rooms when you run away.\n\r",ch->specials.flee);
       send_to_char(buf2,ch);
       return;
    }
  }


  if (IS_AFFECTED(ch, AFF_PARALYSIS))
    return;

  if (affected_by_spell(ch, SPELL_WEB)) {
    if (!saves_spell(ch, SAVING_PARA)) {
       WAIT_STATE(ch, PULSE_VIOLENCE);
       send_to_char("You are ensared in webs, you cannot move!\n\r", ch);
       act("$n struggles against the webs that hold $m", FALSE,
	   ch, 0, 0, TO_ROOM);
       return;
    } 
    else {
      send_to_char("You pull free from the sticky webbing!\n\r", ch);
      act("$n manages to pull free from the sticky webbing!", FALSE,
	  ch, 0, 0, TO_ROOM);
      GET_MOVE(ch) -= 50;
    }
  }
  
  if (GET_POS(ch) <= POSITION_SITTING){
    GET_MOVE(ch) -= 10;
    act("$n scrambles madly to $s feet!", TRUE, ch, 0, 0, TO_ROOM);
    act("Panic-stricken, you scramble to your feet.", TRUE, ch, 0, 0,
	TO_CHAR);
    GET_POS(ch) = POSITION_STANDING;
    WAIT_STATE(ch, PULSE_VIOLENCE);
    return;
  }

 if(IS_PC(ch) || IS_SET(ch->specials.act, ACT_POLYSELF)) {  /* This only works on PC types */

  found = FALSE;
  badroom = -1;

  if (!(ch->specials.fighting)) {
    for(f=0; f<(ch->specials.flee); f++) {
      for(i=0; i<6; i++) {
        attempt = number(0, 5);  /* Select a random direction */
        if (CAN_GO(ch, attempt) && !IS_SET(real_roomp(EXIT(ch, attempt)->to_room)->room_flags, DEATH)) {
	  act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);

          found = TRUE;

	  if (RIDDEN(ch)) {
	    if ((die = MoveOne(RIDDEN(ch), attempt, FALSE))== 1) {
	      /* The escape has succeded */
	      send_to_char("You flee head over heels.\n\r", ch);
              break;
	      /* return; */
	    } else {
	      if (!die) act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
              break;
	      /* return; */
	    }
	  } else {
	    if ((die = MoveOne(ch, attempt, FALSE))== 1) {
	      /* The escape has succeded */
	      send_to_char("You flee head over heels.\n\r", ch);
              break;
	      /* return; */
	    } else {
	      if (!die) act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
              break;
	      /* return; */
	    }
	  }
        }
      } /* inner for loop */
      if(!found) {  /* No exits were found */
        send_to_char("PANIC! You couldn't escape!\n\r", ch);
      }
      found = FALSE;
    }
  return;  /* After all has been accounted for.. */
  }

  tries=0;
  do {
    for(i=0; i<3; i++) {
      attempt = number(0, 5);  /* Select a random direction */
      if (CAN_GO(ch, attempt) && !IS_SET(real_roomp(EXIT(ch, attempt)->to_room)->room_flags, DEATH)) {
        
        found = TRUE;
        switch(attempt) {   /* Character will not try to flee back the way he came after first attempt to flee */
          case 0: badroom=2; break;
          case 1: badroom=3; break;
          case 2: badroom=0; break;
          case 3: badroom=1; break;
          case 4: badroom=5; break;
          case 5: badroom=4; break;
        }

        if (!ch->skills || (number(1,101) > ch->skills[SKILL_RETREAT].learned)) {
	  act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);
	  panic = TRUE;
	  LearnFromMistake(ch, SKILL_RETREAT, 0, 90);
        } else {
	    /*
	  find a legal exit
	    */
	  for (j=0; j<6; j++) {
	    if (CAN_GO(ch, j) && !IS_SET(real_roomp(EXIT(ch, j)->to_room)->room_flags, DEATH)) {
	      attempt = j;
              switch(attempt) {   /* Since retreat picks a new exit, we gotta badroom test again */
                case 0: badroom=2; break;
                case 1: badroom=3; break;
                case 2: badroom=0; break;
                case 3: badroom=1; break;
                case 4: badroom=5; break;
                case 5: badroom=4; break;
              }
	      j = 10;
	    }
	  }
	  act("$n skillfully retreats from battle", TRUE, ch, 0, 0, TO_ROOM);
	  panic = FALSE;
        }

        if (IS_AFFECTED(ch, AFF_CHARM)) {
	  charm = TRUE;
	  REMOVE_BIT(ch->specials.affected_by, AFF_CHARM);
        } else 
	  charm = FALSE;

        if (RIDDEN(ch)) {
	  die = MoveOne(RIDDEN(ch), attempt);
        } else {
	  die = MoveOne(ch, attempt);
        }

        if (charm)
	  SET_BIT(ch->specials.affected_by, AFF_CHARM);

        if (die == 1) { 
	  /* The escape has succeded. We'll be nice. */
	  if (GetMaxLevel(ch) > 3) {
	    if (panic || !HasClass(ch, CLASS_WARRIOR)) {
	      loose = GetMaxLevel(ch)+(GetSecMaxLev(ch)/2)+
	        (GetThirdMaxLev(ch)/3);
	      loose -= GetMaxLevel(ch->specials.fighting)+
	        (GetSecMaxLev(ch->specials.fighting)/2)+
		  (GetThirdMaxLev(ch->specials.fighting)/3);
	      loose *= GetMaxLevel(ch);
	    }
	  } else {
	    loose = 0;
	  } 
       
	  if (loose < 0) loose = 1;
          
	  if (IS_NPC(ch) && !(IS_SET(ch->specials.act, ACT_POLYSELF) &&
			    !(IS_SET(ch->specials.act, ACT_AGGRESSIVE)))) {
	    AddFeared(ch, ch->specials.fighting);
	  } else {
	    percent=(int)100 * (float) GET_HIT(ch->specials.fighting) /
	            (float) GET_MAX_HIT(ch->specials.fighting); 
	    if (Hates(ch->specials.fighting, ch)) {   
	       SetHunting(ch->specials.fighting, ch);
	    } else if ((IS_GOOD(ch) && (IS_EVIL(ch->specials.fighting))) ||
	               (IS_EVIL(ch) && (IS_GOOD(ch->specials.fighting)))) {
	        AddHated(ch->specials.fighting, ch);
	        SetHunting(ch->specials.fighting, ch);
	    } else if (number(1,101) < percent) {
	        AddHated(ch->specials.fighting, ch);
	        SetHunting(ch->specials.fighting, ch);
	    }
          }
	  if (IS_PC(ch) && panic) {
	    if (HasClass(ch, CLASS_MONK) || !HasClass(ch, CLASS_WARRIOR))
	      gain_exp(ch, -loose);
	  }
  
	  if (panic) {
	    send_to_char("You flee head over heels.\n\r", ch);
	  } else {
	    send_to_char("You retreat skillfully\n\r", ch);
	  }
         
	  if (ch->specials.fighting->specials.fighting == ch)
	    stop_fighting(ch->specials.fighting);
	  if (ch->specials.fighting)
	    stop_fighting(ch);
          break;
        } else {
	  if (!die) act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
          break;
        }
      }
    } /* inner for */
    if(!found) {    
      /* No exits were found */
      send_to_char("PANIC! You couldn't escape!\n\r", ch);
    }
    tries++;
  } while((!found) && (tries<(ch->specials.flee)));   /* This keeps the sucker fighting if he didn't escape */

  found = FALSE;

  /* This is if he fled, and still had more flees left to burn */

  if (!(ch->specials.fighting)) {
    for(f=0; f<((ch->specials.flee)-tries); f++) {
      for(i=0; i<6; i++) {
        attempt = number(0, 5);  /* Select a random direction */
        if (CAN_GO(ch, attempt) && !IS_SET(real_roomp(EXIT(ch, attempt)->to_room)->room_flags, DEATH) &&
            (attempt != badroom)) {

          badroom = -1;   /* This is so that after you leave this room, you won't know or have prejudices against
                             that particular direction anymore... */

	  act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);

          found = TRUE;
	  if (RIDDEN(ch)) {
	    if ((die = MoveOne(RIDDEN(ch), attempt, FALSE))== 1) {
	      /* The escape has succeded */
	      send_to_char("You flee head over heels.\n\r", ch);
              break;
	      /* return; */
	    } else {
	      if (!die) act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
              break;
	      /* return; */
	    }
	  } else {
	    if ((die = MoveOne(ch, attempt, FALSE))== 1) {
	      /* The escape has succeded */
	      send_to_char("You flee head over heels.\n\r", ch);
              break;
	      /* return; */
	    } else {
	      if (!die) act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
              break;
	      /* return; */
	    }
	  }
        }
        else if(!found && (attempt == badroom)) {
          send_to_char("There is NO WAY IN HELL you'd consider going that way!\n\r", ch);
          found = TRUE;  /* This means you just burnt up one attempt */
          break;
        }
      } /* inner for loop */
      if(!found) {  /* No exits were found */
        send_to_char("PANIC! You couldn't escape!\n\r", ch);
      }
      found = FALSE;
    }
  return;  /* After all has been accounted for.. */
  }
 }
 else {       /* The old flee code will still be used for mobs */
  if (!(ch->specials.fighting)) {
    for(i=0; i<6; i++) {
      attempt = number(0, 5);  /* Select a random direction */
      if (CAN_GO(ch, attempt) &&
	  !IS_SET(real_roomp(EXIT(ch, attempt)->to_room)->room_flags, 
		  DEATH)) {
	act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);

	if (RIDDEN(ch)) {
	  if ((die = MoveOne(RIDDEN(ch), attempt, FALSE))== 1) {
	    /* The escape has succeded */
	    send_to_char("You flee head over heels.\n\r", ch);
	    return;
	  } else {
	    if (!die) act("$n tries to flee, but is too exhausted!", 
			  TRUE, ch, 0, 0, TO_ROOM);
	    return;
	  }
	} else {
	  if ((die = MoveOne(ch, attempt, FALSE))== 1) {
	    /* The escape has succeded */
	    send_to_char("You flee head over heels.\n\r", ch);
	    return;
	  } else {
	    if (!die) act("$n tries to flee, but is too exhausted!", 
			  TRUE, ch, 0, 0, TO_ROOM);
	    return;
	  }
	}
      }
    } /* for */
    /* No exits was found */
    send_to_char("PANIC! You couldn't escape!\n\r", ch);
    return;
  }
  
  for(i=0; i<3; i++) {
    attempt = number(0, 5);  /* Select a random direction */
    if (CAN_GO(ch, attempt) &&
	!IS_SET(real_roomp(EXIT(ch, attempt)->to_room)->room_flags, DEATH)) {
      int panic, j;

      if (!ch->skills || (number(1,101) > ch->skills[SKILL_RETREAT].learned)) {
	act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);
	panic = TRUE;
	LearnFromMistake(ch, SKILL_RETREAT, 0, 90);
      } else {
	/*
	  find a legal exit
	  */
	for (j =0;j<6;j++) {
	  if (CAN_GO(ch, j) &&	
	      !IS_SET(real_roomp(EXIT(ch, j)->to_room)->room_flags, 
		      DEATH)) {
	    attempt = j;
	    j = 10;
	  }
	}
	act("$n skillfully retreats from battle", TRUE, ch, 0, 0, TO_ROOM);
	panic = FALSE;
      }
      

      if (IS_AFFECTED(ch, AFF_CHARM)) {
	charm = TRUE;
	REMOVE_BIT(ch->specials.affected_by, AFF_CHARM);
      } else 
	charm = FALSE;

      if (RIDDEN(ch)) {
	die = MoveOne(RIDDEN(ch), attempt);
      } else {
	die = MoveOne(ch, attempt);
      }

      if (charm)
	SET_BIT(ch->specials.affected_by, AFF_CHARM);

      if (die == 1) { 
	/* The escape has succeded. We'll be nice. */
	if (GetMaxLevel(ch) > 3) {
	  if (panic || !HasClass(ch, CLASS_WARRIOR)) {
	    loose = GetMaxLevel(ch)+(GetSecMaxLev(ch)/2)+
	      (GetThirdMaxLev(ch)/3);
	    loose -= GetMaxLevel(ch->specials.fighting)+
	      (GetSecMaxLev(ch->specials.fighting)/2)+
		(GetThirdMaxLev(ch->specials.fighting)/3);
	    loose *= GetMaxLevel(ch);
	  }
	} else {
	  loose = 0;
	}     
	if (loose < 0) loose = 1;
	if (IS_NPC(ch) && !(IS_SET(ch->specials.act, ACT_POLYSELF) &&
			    !(IS_SET(ch->specials.act, ACT_AGGRESSIVE)))) {
	  AddFeared(ch, ch->specials.fighting);
	} else {
	  percent=(int)100 * (float) GET_HIT(ch->specials.fighting) /
	    (float) GET_MAX_HIT(ch->specials.fighting);
	  if (Hates(ch->specials.fighting, ch)) {	      
	      SetHunting(ch->specials.fighting, ch);
	  } else if ((IS_GOOD(ch) && (IS_EVIL(ch->specials.fighting))) ||
		(IS_EVIL(ch) && (IS_GOOD(ch->specials.fighting)))) {
	      AddHated(ch->specials.fighting, ch);
	      SetHunting(ch->specials.fighting, ch);
	  } else if (number(1,101) < percent) {
	      AddHated(ch->specials.fighting, ch);
	      SetHunting(ch->specials.fighting, ch);
	  }
	}
	if (IS_PC(ch) && panic) {
	  if (HasClass(ch, CLASS_MONK) || !HasClass(ch, CLASS_WARRIOR))
	    gain_exp(ch, -loose);
	}

	if (panic) {
	  send_to_char("You flee head over heels.\n\r", ch);
	} else {
	  send_to_char("You retreat skillfully\n\r", ch);
	}
	if (ch->specials.fighting->specials.fighting == ch)
	  stop_fighting(ch->specials.fighting);
	if (ch->specials.fighting)
	  stop_fighting(ch);
	return;
      } else {
	if (!die) act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
	return;
      }
    }
  } /* for */
  
  /* No exits were found */
  send_to_char("PANIC! You couldn't escape!\n\r", ch);
 }
return;
}


void do_bash(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char name[256];
  byte percent;


  if (!ch->skills)
    return;

  if (!IS_PC(ch) && cmd)
    return;

   if (check_peaceful(ch,"You feel too peaceful to contemplate violence.\n\r"))
    return;

  if (!HasClass(ch, CLASS_WARRIOR)) {
    send_to_char("You're no warrior!\n\r", ch);
    return;
  }

  only_argument(argument, name);
  
  if (!(victim = get_char_room_vis(ch, name))) {
    if (ch->specials.fighting) {
      victim = ch->specials.fighting;
    } else {
      send_to_char("Bash who?\n\r", ch);
      return;
    }
  }
  
  
  if (victim == ch) {
    send_to_char("Aren't we funny today...\n\r", ch);
    return;
  }

  if (IS_NPC(victim) && IS_SET(victim->specials.act, ACT_HUGE)) {
    if (GET_RACE(ch) != RACE_GIANT) {
      act("$N is MUCH too large to bash!", FALSE, ch, 0, victim, TO_CHAR);
      return;
    }
  }

  if (MOUNTED(victim)) {
    send_to_char("You can't bash a mounted target!\n\r", ch);
    return;
  }

  if (MOUNTED(ch)) {
    send_to_char("You can't bash while mounted!\n\r", ch);
    return;
  }

  if (ch->attackers > 3) {
    send_to_char("There's no room to bash!\n\r",ch);
    return;
  }

  if (victim->attackers >= 6) {
    send_to_char("You can't get close enough to them to bash!\n\r", ch);
    return;
  }

  if (!ch->skills) {
    if (GET_POS(victim) > POSITION_DEAD) {
      damage(ch, victim, 0, SKILL_BASH);
      GET_POS(ch) = POSITION_SITTING;
    }
  }
  
  percent=number(1,101); /* 101% is a complete failure */
  
  /* some modifications to account for dexterity, and level */
  percent -= dex_app[GET_DEX(ch)].reaction*10;
  percent += dex_app[GET_DEX(victim)].reaction*10;
  if (GetMaxLevel(victim) > 12) {
    percent += ((GetMaxLevel(victim)-10) * 5);
  }

  if (GET_POS(ch)<= POSITION_STUNNED)
    percent = 0;

  if (percent > ch->skills[SKILL_BASH].learned) {
    if (GET_POS(victim) > POSITION_DEAD) {
      damage(ch, victim, 0, SKILL_BASH);
      GET_POS(ch) = POSITION_SITTING;
    }
    LearnFromMistake(ch, SKILL_BASH, 0, 90);
  } else {
    if (GET_POS(victim) > POSITION_DEAD) {
      GET_POS(victim) = POSITION_SITTING;
      damage(ch, victim, 2, SKILL_BASH);
      WAIT_STATE(victim, PULSE_VIOLENCE*2);
      GET_POS(victim) = POSITION_SITTING;
    }
  }
  WAIT_STATE(ch, PULSE_VIOLENCE*2);
}




void do_rescue(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim, *tmp_ch;
  int percent;
  char victim_name[240];
  

  if (!ch->skills) {
      send_to_char("You fail the rescue.\n\r", ch);
      return;
  }

  if (!IS_PC(ch) && cmd)
    return;

  if (check_peaceful(ch,"No one should need rescuing here.\n\r"))
    return;

  if (!HasClass(ch, CLASS_WARRIOR)) {
    send_to_char("You're no warrior!\n\r", ch);
    return;
  }

  only_argument(argument, victim_name);
  
  if (!(victim = get_char_room_vis(ch, victim_name))) {
    send_to_char("Who do you want to rescue?\n\r", ch);
    return;
  }
  
  if (victim == ch) {
    send_to_char("What about fleeing instead?\n\r", ch);
    return;
  }

  if (MOUNTED(victim)) {
    send_to_char("You can't rescue a mounted person!\n\r", ch);
    return;
  }
  
  if (ch->specials.fighting == victim) {
    send_to_char("How can you rescue someone you are trying to kill?\n\r",ch);
    return;
  }

  if(IS_AFFECTED2(victim, AFF2_BERSERK)) {
    send_to_char("No way.  They might kill you too!\n\r", ch);
    return;
  }

  if (victim->attackers >= 3) {
    send_to_char("You can't get close enough to them to rescue!\n\r", ch);
    return;
  }
  
  for (tmp_ch=real_roomp(ch->in_room)->people; tmp_ch &&
       (tmp_ch->specials.fighting != victim); tmp_ch=tmp_ch->next_in_room)  ;
  
  if (!tmp_ch) {
    act("But nobody is fighting $M?", FALSE, ch, 0, victim, TO_CHAR);
    return;
  }
  
  
  percent=number(1,101); /* 101% is a complete failure */

  if ((percent > ch->skills[SKILL_RESCUE].learned)) {
    send_to_char("You fail the rescue.\n\r", ch);
    LearnFromMistake(ch, SKILL_RESCUE, 0, 90);
    WAIT_STATE(ch, PULSE_VIOLENCE);
    return;
  }
  
  send_to_char("Banzai! To the rescue...\n\r", ch);
  act("You are rescued by $N, you are confused!", FALSE, victim,0,ch, TO_CHAR);
  act("$n heroically rescues $N.", FALSE, ch, 0, victim, TO_NOTVICT);
  if (IS_PC(ch) && IS_PC(victim))
      GET_ALIGNMENT(ch)+=20;
    
  if (victim->specials.fighting == tmp_ch)
    stop_fighting(victim);
  if (tmp_ch->specials.fighting)
    stop_fighting(tmp_ch);
  if (ch->specials.fighting)
    stop_fighting(ch);
  
  set_fighting(ch, tmp_ch);
  set_fighting(tmp_ch, ch);
  
  WAIT_STATE(victim, 2*PULSE_VIOLENCE);

}



void do_assist(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim, *tmp_ch;
  char victim_name[240];
  
  if (check_peaceful(ch,"Noone should need assistance here.\n\r"))
    return;
  
  only_argument(argument, victim_name);
  
  if (!(victim = get_char_room_vis(ch, victim_name))) {
    send_to_char("Who do you want to assist?\n\r", ch);
    return;
  }
  
  if (victim == ch) {
    send_to_char("Oh, by all means, help yourself...\n\r", ch);
    return;
  }
  
  if (ch->specials.fighting == victim) {
    send_to_char("That would be counterproductive?\n\r",ch);
    return;
  }
  
  if (ch->specials.fighting) {
    send_to_char("You have your hands full right now\n\r",ch);
    return;
  }

  if (victim->attackers >= 6) {
    send_to_char("You can't get close enough to them to assist!\n\r", ch);
    return;
  }

  
  tmp_ch = victim->specials.fighting;
  /*	for (tmp_ch=real_roomp(ch->in_room)->people; tmp_ch &&
	(tmp_ch->specials.fighting != victim); tmp_ch=tmp_ch->next_in_room)  ;
	*/
  if (!tmp_ch) {
    act("But $E's not fighting anyone.", FALSE, ch, 0, victim, TO_CHAR);
    return;
  }
  
  hit(ch, tmp_ch, TYPE_UNDEFINED);
  
  WAIT_STATE(victim, PULSE_VIOLENCE+2); /* same as hit */
}



void do_kick(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char name[80];
  int dam;
  byte percent;

  if (!ch->skills)
    return;
  
  if (check_peaceful(ch,
         "You feel too peaceful to contemplate violence.\n\r"))
    return;

  if (!HasClass(ch, CLASS_WARRIOR) && !HasClass(ch, CLASS_MONK)) {
    send_to_char("You're no warrior!\n\r", ch);    
    return;
  }
  

  if (!IS_PC(ch) && cmd)
    return;

  only_argument(argument, name);
  
  if (!(victim = get_char_room_vis(ch, name))) {
    if (ch->specials.fighting) {
      victim = ch->specials.fighting;
    } else {
      send_to_char("Kick who?\n\r", ch);
      return;
    }
  }
  
  if (victim == ch) {
    send_to_char("Aren't we funny today...\n\r", ch);
    return;
  }


  if (MOUNTED(victim)) {
    send_to_char("You can't kick while mounted!\n\r", ch);
    return;
  }

  if (ch->attackers > 3) {
    send_to_char("There's no room to kick!\n\r",ch);
    return;
  }

  if (victim->attackers >= 4) {
    send_to_char("You can't get close enough to them to kick!\n\r", ch);
    return;
  }
  
  percent=((10-(GET_AC(victim)/10))) + number(1,101);
  /* 101% is a complete failure */

  if (GET_POS(victim)<=POSITION_STUNNED) {
    percent = 1;
  }
  
  if(GET_RACE(victim)==RACE_GHOST) {
    kick_messages(ch,victim,0);
    SetVictFighting(ch,victim);
    return;
  } else if (!IS_NPC(victim) && (GetMaxLevel(victim) > MAX_MORT)) {
    kick_messages(ch,victim,0);
    SetVictFighting(ch,victim);
    SetCharFighting(ch,victim);
    return;
  }

  if (GET_POS(victim) <= POSITION_SLEEPING)
    percent = 1;

  if (percent > ch->skills[SKILL_KICK].learned) {
    if (GET_POS(victim) > POSITION_DEAD) {
      damage(ch, victim, 0, SKILL_KICK);
      kick_messages(ch,victim,0);
    }
    LearnFromMistake(ch, SKILL_KICK, 0, 90);
  } else {
    if (GET_POS(victim) > POSITION_DEAD) {
      dam = GET_LEVEL(ch, BestFightingClass(ch));
      if (!HasClass(ch, CLASS_MONK))
	dam=dam>>1;
	/* damage(ch, victim, dam, SKILL_KICK); 
	   else
	   damage(ch, victim, dam>>1, SKILL_KICK);
	   */
      kick_messages(ch,victim,dam);
      damage(ch, victim, dam, SKILL_KICK);
    }
    WAIT_STATE(victim, PULSE_VIOLENCE);
  }
  WAIT_STATE(ch, PULSE_VIOLENCE*3);
}

void do_wimp(struct char_data *ch, char *argument, int cmd)
{
  char buf[255];
  char buf2[255];
  int pct;
  
  if(GET_RACE(ch) == RACE_OGRE) {
    send_to_char("Ha, a wimpy ogre?  Surely you jest.\n\r", ch);
    REMOVE_BIT(ch->specials.act, PLR_WIMPY); /* just in case */
    return;
  }

  argument = one_argument(argument, buf);
  
  /* sets the character in wimpy mode.  */
  
  if(((!IS_NPC(ch)) && (IS_SET(ch->specials.act, PLR_WIMPY))) || ((IS_NPC(ch)) && (IS_SET(ch->specials.act, ACT_WIMPY))))  {
    if (strlen(buf) != 0) {
      pct = atoi(buf);
      if(pct < 1) {
        send_to_char("Please choose a value between 1 and 50.\n\r",ch);
        return;
      }
      else if(pct > 50) {
        send_to_char("Please choose a value between 1 and 50.\n\r",ch);
        return;
      }
      if(IS_NPC(ch))
        REMOVE_BIT(ch->specials.act, ACT_WIMPY);  /* eld - For polys... I'm surprised it wasn't in here originally */
      else
        REMOVE_BIT(ch->specials.act, PLR_WIMPY);
      send_to_char("Ok, you are no longer a wimp...\n\r",ch);
      ch->specials.pct = pct;
      sprintf(buf2,"And you will now get your BLEEDING message at %d%% of max hitpoints.\n\r",ch->specials.pct);
      send_to_char(buf2,ch);
    }
    else {
      if(IS_NPC(ch))
        REMOVE_BIT(ch->specials.act, ACT_WIMPY);
      else
        REMOVE_BIT(ch->specials.act, PLR_WIMPY);
      send_to_char("Ok, you are no longer a wimp...\n\r",ch);
      sprintf(buf2,"However, you will still get your BLEEDING message at %d%% of max hitpoints.\n\r",ch->specials.pct);
      send_to_char(buf2,ch);
    }
  } else {
    if (strlen(buf) != 0) {
      pct = atoi(buf);
      if(pct < 1) {
        send_to_char("Please choose a value between 1 and 50.\n\r",ch);
        return;
      }
      else if(pct > 50) {
        send_to_char("Please choose a value between 1 and 50.\n\r",ch);
        return;
      }
      sprintf(buf2,"Your wimpy percentage WAS %d%%.\n\r",ch->specials.pct);
      send_to_char(buf2,ch);
      ch->specials.pct = pct;
      sprintf(buf2,"Your wimpy percentage has now been set to %d%%.\n\r",
	      ch->specials.pct);
      send_to_char(buf2,ch);
    }
    else {
      sprintf(buf2,"Your wimpy percentage remains at %d%%.\n\r",
	      ch->specials.pct);
      send_to_char(buf2,ch);
    }
    if(IS_NPC(ch))
      SET_BIT(ch->specials.act, ACT_WIMPY);
    else
      SET_BIT(ch->specials.act, PLR_WIMPY);
    send_to_char("Ok, you are now in wimpy mode.\n\r", ch);
  }
  
}


extern struct breather breath_monsters[];
extern struct index_data *mob_index;
void (*bweapons[])() = {
  cast_geyser,
  cast_fire_breath, cast_gas_breath, cast_frost_breath, cast_acid_breath,
  cast_lightning_breath};

void do_breath(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char	buf[MAX_STRING_LENGTH], name[MAX_STRING_LENGTH];
  int	count, manacost;
  void	(*weapon)();
  
  if (check_peaceful(ch,"That wouldn't be nice at all.\n\r"))
    return;
  
  only_argument(argument, name);
  
  for (count = FIRST_BREATH_WEAPON;
       count <= LAST_BREATH_WEAPON && !affected_by_spell(ch, count);
       count++)
    ;
  
  if (count>LAST_BREATH_WEAPON) {
    struct breather *scan;
    
    for (scan = breath_monsters;
	 scan->vnum >= 0 && scan->vnum != mob_index[ch->nr].virtual;
	 scan++)
      ;
    
    if (scan->vnum < 0) {
      send_to_char("You don't have a breath weapon, potatohead.\n\r", ch);
      return;
    }
    
    for (count=0; scan->breaths[count]; count++)
      ;
    
    if (count<1) {
      sprintf(buf, "monster %s has no breath weapons",
	      ch->player.short_descr);
      log(buf);
      send_to_char("Hey, why don't you have any breath weapons!?\n\r",ch);
      return;
    }
    
    weapon = scan->breaths[dice(1,count)-1];
    manacost = scan->cost;
    if (GET_MANA(ch) <= -3*manacost) {
      weapon = NULL;
    }
  } else {
    manacost = 0;
    weapon = bweapons[count-FIRST_BREATH_WEAPON];
    affect_from_char(ch, count);
  }
  
  if (!(victim = get_char_room_vis(ch, name))) {
    if (ch->specials.fighting) {
      victim = ch->specials.fighting;
    } else {
      send_to_char("Breath on who?\n\r", ch);
      return;
    }
  }
  
  breath_weapon(ch, victim, manacost, weapon);
  
  WAIT_STATE(ch, PULSE_VIOLENCE*2);
}

void do_shoot(struct char_data *ch, char *argument, int cmd)
{
  char arg[80];
  struct char_data *victim;
  
  if (check_peaceful(ch,"You feel too peaceful to contemplate violence.\n\r"))
    return;

  only_argument(argument, arg);
  
  if (*arg) {
    victim = get_char_room_vis(ch, arg);
    if (victim) {
      if (victim == ch) {
	send_to_char("You can't shoot things at yourself!", ch);
	return;
      } else {
	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == victim)) {
	  act("$N is just such a good friend, you simply can't shoot at $M.",
	      FALSE, ch,0,victim,TO_CHAR);
	  return;
	}
	if (ch->specials.fighting) {
	  send_to_char("You're at too close range to fire a weapon!\n\r", ch);
	  return;
	}
	shoot(ch, victim);
	WAIT_STATE(ch, PULSE_VIOLENCE);
      }
    } else {
      send_to_char("They aren't here.\n\r", ch);
    }
  } else {
    send_to_char("Shoot who?\n\r", ch);
  }
}


void do_springleap(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char name[256];
  byte percent;

  if (!ch->skills)
    return;
  
  if (check_peaceful(ch,
         "You feel too peaceful to contemplate violence.\n\r"))
    return;

  if (!HasClass(ch, CLASS_MONK)) {
    send_to_char("You're no monk!\n\r", ch);    
    return;
  }
  
  only_argument(argument, name);
  
  if (!(victim = get_char_room_vis(ch, name))) {
    if (ch->specials.fighting) {
      victim = ch->specials.fighting;
    } else {
      send_to_char("Spring-leap at who?\n\r", ch);
      return;
    }
  }

  if (GET_POS(ch) > POSITION_SITTING || !ch->specials.fighting) {
    send_to_char("You're not in position for that!\n\r", ch);
    return;
  }
  
  if (victim == ch) {
    send_to_char("Aren't we funny today...\n\r", ch);
    return;
  }

  if (ch->attackers > 3) {
    send_to_char("There's no room to spring-leap!\n\r",ch);
    return;
  }

  if (victim->attackers >= 3) {
    send_to_char("You can't get close enough\n\r", ch);
    return;
  }
  
  percent=number(1,101);
  
  act("$n does a really nifty move, and aims a leg towards $N.", FALSE,
      ch, 0, victim, TO_ROOM);
  act("You leap off the ground at $N.", FALSE,
      ch, 0, victim, TO_CHAR);
  act("$n leaps off the ground at you.", FALSE,
      ch, 0, victim, TO_VICT);


  if (percent > ch->skills[SKILL_SPRING_LEAP].learned) {
    if (GET_POS(victim) > POSITION_DEAD) {
      damage(ch, victim, 0, SKILL_KICK);
      LearnFromMistake(ch, SKILL_SPRING_LEAP, 0, 90);
      send_to_char("You fall on your butt\n\r", ch);
      act("$n falls on $s butt", FALSE, ch, 0, 0, TO_ROOM);
    }
    WAIT_STATE(ch, PULSE_VIOLENCE*3);
    return;
    
  } else {
    if (HitOrMiss(ch, victim, CalcThaco(ch))) {
      if (GET_POS(victim) > POSITION_DEAD)
	damage(ch, victim, GET_LEVEL(ch, BestFightingClass(ch))>>1, 
	       SKILL_KICK);
      kick_messages(ch,victim, GET_LEVEL(ch, BestFightingClass(ch))>>1);
    } else {
	damage(ch, victim, 0, SKILL_KICK);
	kick_messages(ch, victim, 0);
    }
    WAIT_STATE(victim, PULSE_VIOLENCE);
  }
  WAIT_STATE(ch, PULSE_VIOLENCE*1);
  GET_POS(ch)=POSITION_STANDING;
  update_pos(ch);
  
}


void do_quivering_palm( struct char_data *ch, char *arg, int cmd)
{
  struct char_data *victim;
  struct affected_type af;
  byte percent;
  char name[256];

  if (!ch->skills)
    return;
  
  if (check_peaceful(ch,
         "You feel too peaceful to contemplate violence.\n\r"))
    return;

  if (!HasClass(ch, CLASS_MONK)) {
    send_to_char("You're no monk!\n\r", ch);    
    return;
  }
  
  only_argument(arg, name);
  
  if (!(victim = get_char_room_vis(ch, name))) {
    if (ch->specials.fighting) {
      victim = ch->specials.fighting;
    } else {
      send_to_char("Use the fabled quivering palm on who?\n\r", ch);
      return;
    }
  }
  
  if (!ch->skills)
    return;
  
  if (ch->attackers > 3) {
    send_to_char("There's no room to use that skill!\n\r",ch);
    return;
  }

  if (victim->attackers >= 3) {
    send_to_char("You can't get close enough\n\r", ch);
    return;
  }

  if (!IsHumanoid(victim) ) {
    send_to_char("You can only do this to humanoid opponents\n\r", ch);
    return;
  }

  send_to_char("You begin to work on the vibrations\n\r", ch);

  if (affected_by_spell(ch, SKILL_QUIV_PALM)) {
    send_to_char("You can only do this once per week\n\r", ch);
    return;
  }
    
  percent=number(1,101);
  
  if (percent > ch->skills[SKILL_QUIV_PALM].learned) {
    send_to_char("The vibrations fade ineffectively\n\r", ch);
    if (GET_POS(victim) > POSITION_DEAD) {
      LearnFromMistake(ch, SKILL_QUIV_PALM, 0, 95);
    }
    WAIT_STATE(ch, PULSE_VIOLENCE*3);
    return;
    
  } else {
    if (GET_MAX_HIT(victim) > GET_MAX_HIT(ch)*2 || GetMaxLevel(victim) > GetMaxLevel(ch)) {
	damage(ch, victim, 0,SKILL_QUIV_PALM);
      return;
    }
    if (HitOrMiss(ch, victim, CalcThaco(ch))) {
      if (GET_POS(victim) > POSITION_DEAD)
	damage(ch, victim, GET_MAX_HIT(victim)*20,SKILL_QUIV_PALM);
    }
  }

  WAIT_STATE(ch, PULSE_VIOLENCE*1);
 
  af.type = SKILL_QUIV_PALM;
  af.duration = 168;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char(ch, &af);
}


void kick_messages(struct char_data *ch, struct char_data *victim, int damage)
{
  int i;
 
  switch(GET_RACE(victim)) {
  case RACE_HUMAN:
  case RACE_ELVEN:
  case RACE_DWARF:
  case RACE_DROW:
  case RACE_ORC:
  case RACE_LYCANTH:
  case RACE_TROLL:
  case RACE_DEMON:
  case RACE_DEVIL:
  case RACE_MFLAYER:
  case RACE_ASTRAL:
  case RACE_PATRYN:
  case RACE_SARTAN:
  case RACE_DRAAGDIM:
  case RACE_GOLEM:
  case RACE_TROGMAN:
    i=number(0,3);
    break;
  case RACE_PREDATOR:
  case RACE_HERBIV:
  case RACE_LABRAT:
    i=number(4,6);
    break;
  case RACE_REPTILE:
  case RACE_DRAGON:
    i=number(4,7);
    break;
  case RACE_TREE:
    i=8;
    break;
  case RACE_PARASITE:
  case RACE_SLIME:
  case RACE_VEGGIE:
  case RACE_VEGMAN:
    i=9;
    break;
  case RACE_ROO:
  case RACE_GNOME:
  case RACE_HALFLING:
  case RACE_GOBLIN:
  case RACE_SMURF:
  case RACE_ENFAN:
    i=10;
    break;
  case RACE_GIANT:
  case RACE_TYTAN:
  case RACE_GOD:
    if(GET_RACE(ch) == RACE_OGRE) /* hee hee */
      i = number(0,3);
    else
      i=11;
    break;
  case RACE_GHOST:
    i=12;
    break;
  case RACE_BIRD:
  case RACE_SKEXIE:
    i=13;
    break;
  case RACE_UNDEAD:
    i=14;
    break;
  case RACE_DINOSAUR:
    i=15;
    break;
  case RACE_INSECT:
  case RACE_ARACHNID:
    i=16;
    break;
  case RACE_FISH:
    i=17;
    break;
 default:
    i=18;
  };
  if(!damage) {
    act(att_kick_miss_ch[i], FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
    act(att_kick_miss_victim[i],FALSE, ch, ch->equipment[WIELD],victim, 
	TO_VICT);
    act(att_kick_miss_room[i],FALSE, ch, ch->equipment[WIELD], victim,
	TO_NOTVICT);
  } else if( GET_HIT(victim)-DamageTrivia(ch,victim,damage,SKILL_KICK) < -10 ) {
    act(att_kick_kill_ch[i], FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
    act(att_kick_kill_victim[i],FALSE, ch, ch->equipment[WIELD],victim,
        TO_VICT);
    act(att_kick_kill_room[i],FALSE, ch, ch->equipment[WIELD], victim,
        TO_NOTVICT);
  }
  else {
    act(att_kick_hit_ch[i], FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
    act(att_kick_hit_victim[i],FALSE, ch, ch->equipment[WIELD],victim,
        TO_VICT);
    act(att_kick_hit_room[i],FALSE, ch, ch->equipment[WIELD], victim,
        TO_NOTVICT);
  }
 
}
