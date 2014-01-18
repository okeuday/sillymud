/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protos.h"

extern char *dirs[];
extern struct char_data *character_list;
extern struct str_app_type str_app[];
extern struct room_data *world;
extern struct dex_app_type dex_app[];
extern struct skill_data skill_info[];
extern struct index_data *obj_index;

struct hunting_data {
  char	*name;
  struct char_data **victim;
};


/*************************************/
/* predicates for find_path function */

int is_target_room_p(int room, void *tgt_room);

int named_object_on_ground(int room, void *c_data);

/* predicates for find_path function */
/*************************************/


/*
**  train:
*/

void do_train(struct char_data *ch, char *argument, int cmd)
{
  /* 
    code to allow high level characters to train low level
    characters.
     */

  
}

void do_inset(struct char_data *ch, char *argument, int cmd)
{
  /* code which allows a character to inset a stone into
     a weapon.  The stone's powers are added to the 
     weapon
     */
  struct obj_data *sword, *gem, *tmp;
  struct char_data *tmp_char;  /* dummy */
  char sp[100], sp2[100], buf[512];
  int ok=FALSE;
  struct extra_descr_data *new_descr;
  int stone_slots, sword_slots, i, j, bits, perc;

  /* get the sword and the gem from the argument */

  if (!ch->skills)
    return;

  argument_interpreter(argument, sp, sp2);

  if (!(*sp) || !(*sp2)) {
    send_to_char("You must supply a weapon and a treasure.\n\r", ch);
    return;
  }

  bits = generic_find(sp, FIND_OBJ_INV,
		      ch, &tmp_char, &sword);

  bits = generic_find(sp2, FIND_OBJ_INV,
		      ch, &tmp_char, &gem);

  /* make sure we have a gem and a sword */

  if (!sword) {
    send_to_char("Inset what?\n\r", ch);
    return;
  }

  if (!gem) {
    send_to_char("Inset what?\n\r", ch);
    return;
  }


  if ((ITEM_TYPE(sword) == ITEM_WEAPON) &&
      (ITEM_TYPE(gem) == ITEM_TREASURE)) {
    ok = TRUE;
  } else if ((ITEM_TYPE(gem) == ITEM_WEAPON) &&
	     (ITEM_TYPE(sword) == ITEM_TREASURE)) {
    tmp = gem;
    gem = sword;
    sword = tmp;
    ok = TRUE;
  }

  if (!ok) {			/* I can't beleive you fucked the act */
				/* parameters. */
    if (GET_ITEM_TYPE(sword) != ITEM_WEAPON) {
      act("$p is not a weapon.", 0, ch, sword, sword, TO_CHAR);
    } 
    if (GET_ITEM_TYPE(gem) != ITEM_TREASURE) {
      act("$p is not a gem.", 0, ch, gem, gem, TO_CHAR);
    }
    return;
  }


  if (sword->obj_flags.weight < gem->obj_flags.weight) {
    send_to_char("That weapon is too small for such a treasure.\n\r",ch);
    /*     act("That weapon is too small for such a treasure",
	   0, ch, 0, 0, TO_CHAR); */
    return;
  }
  
  /* count the effects on the stone */

  for (i=0, stone_slots=0; i < MAX_OBJ_AFFECT; i++) {
    if (gem->affected[i].location != APPLY_NONE) 
      stone_slots++;
  }
  /* count the effects on the sword */
  for (i=0, sword_slots=0; i < MAX_OBJ_AFFECT; i++) {
    if (sword->affected[i].location == APPLY_NONE) 
      sword_slots++;
  }
  /* verify space is available */

  if (stone_slots > sword_slots) {
    act("$p can't hold that many enchantments", 0, ch, sword, sword, TO_CHAR);
    return;
  }
  if (IS_OBJ_STAT(sword, ITEM_INSET)) {
    act("$p has already been inset with a gem", 0, ch, sword, sword, TO_CHAR);
    return;
  }

  /* check skill role.  Failure damages the stone and weapon */
  perc = number(1,101);
  if (perc > ch->skills[SKILL_INSET].learned) {  
    act("$n fumbles with $p and breaks it!", 0, ch, gem, gem, TO_ROOM);
    act("You fumble with $p and break it!", 0, ch, gem, gem, TO_CHAR);
    MakeScrap(ch, gem);
    return;
  }

  /* if success, add stone's effects to sword */


  act("$n insets $p into $P", 0, ch, gem , sword, TO_ROOM);
  act("You inset $p into $P", 0, ch, gem , sword, TO_CHAR);

  SET_BIT(sword->obj_flags.extra_flags, ITEM_INSET);

  /* add an extra description for the stone to the object*/
  CREATE(new_descr, struct extra_descr_data, 1);
  sprintf(buf, "%s hilt",sword->name);
  new_descr->keyword = strdup(buf);
  sprintf(buf, "It is inset with %s", gem->short_description);
  new_descr->description = strdup(buf);
  new_descr->next = sword->ex_description;
  sword->ex_description = new_descr;

  for (i=0; i < MAX_OBJ_AFFECT; i++) {
    if (gem->affected[i].location != APPLY_NONE) {
      j = getFreeAffSlot(sword);
      sword->affected[j].location = gem->affected[i].location;
      sword->affected[j].modifier = gem->affected[i].modifier;
    }
  }

  /* add stone's value to sword's rent (ego)  */
  
  GET_RENT(sword) += GET_VALUE(gem);

  /* delete stone */

  obj_from_char(gem);
  extract_obj(gem);

}

/*
**  Disarm:
*/

void do_disarm(struct char_data *ch, char *argument, int cmd)
{
  char name[30];
  int percent;
  struct char_data *victim;
  struct obj_data *w, *trap;

  if (!ch->skills) return;

  if (check_peaceful(ch,"You feel too peaceful to contemplate violence.\n\r"))
    return;

  if (!IS_PC(ch) && cmd)
    return;
  
  /*
   *   get victim
   */
  only_argument(argument, name);
    if (!(victim = get_char_room_vis(ch, name))) {
    if (ch->specials.fighting) {
      victim = ch->specials.fighting;
    } else {
      if (!ch->skills[SKILL_REMOVE_TRAP].learned) {
	send_to_char("Disarm who?\n\r", ch);
	return;
      } else {

	if (MOUNTED(ch)) {
	  send_to_char("Yeah... right... while mounted\n\r", ch);
	  return;
	}

	if (!(trap = get_obj_in_list_vis(ch, name, 
		    real_roomp(ch->in_room)->contents))) {
	  if (!(trap = get_obj_in_list_vis(ch, name, ch->carrying))) {
	    send_to_char("Disarm what?\n\r", ch);
	    return;
	  }
	}
	if (trap) {
	  remove_trap(ch, trap);
	  return;
	}
      }
    }
  }
  
  
  if (victim == ch) {
    send_to_char("Aren't we funny today...\n\r", ch);
    return;
  }

  if (victim != ch->specials.fighting) {
    send_to_char("but you aren't fighting them!\n\r", ch);
    return;
  }

  if (ch->attackers > 3) {
    send_to_char("There is no room to disarm!\n\r", ch);
    return;
  }

  if (!HasClass(ch, CLASS_WARRIOR) && !HasClass(ch, CLASS_MONK)) {
    send_to_char("You're no warrior!\n\r", ch);
    return;
  }
  
  /*
   *   make roll - modified by dex && level
   */
  percent=number(1,101); /* 101% is a complete failure */
  
  percent -= dex_app[GET_DEX(ch)].reaction*10;
  percent += dex_app[GET_DEX(victim)].reaction*10;
  if (!ch->equipment[WIELD] && !HasClass(ch, CLASS_MONK)) {
    percent += 50;
  }

  percent += GetMaxLevel(victim);
  if (HasClass(victim, CLASS_MONK))
    percent += GetMaxLevel(victim);

  if (HasClass(ch, CLASS_MONK)) {
    percent -= GetMaxLevel(ch);
  } else {
    percent -= GetMaxLevel(ch)>>1;
  }

  if (percent > ch->skills[SKILL_DISARM].learned) {
    /*
     *   failure.
     */
    act("You try to disarm $N, but fail miserably.", 
	TRUE, ch, 0, victim, TO_CHAR);
    act("$n does a nifty fighting move, but then falls on $s butt.",
	TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POSITION_SITTING;
    if ((IS_NPC(victim)) && (GET_POS(victim) > POSITION_SLEEPING) &&
        (!victim->specials.fighting)) {
      set_fighting(victim, ch);
    }
    LearnFromMistake(ch, SKILL_DISARM, 0, 95);
    WAIT_STATE(ch, PULSE_VIOLENCE*3);
  } else {
    /*
     *  success
     */
    if (victim->equipment[WIELD]) {
      w = unequip_char(victim, WIELD);
      act("$n makes an impressive fighting move.", 
	  TRUE, ch, 0, 0, TO_ROOM);
      act("You send $p flying from $N's grasp.", TRUE, ch, w, victim, 
	  TO_CHAR);
      act("$p flies from your grasp.", TRUE, ch, w, victim, TO_VICT);
/*
  send the object to a nearby room, instead
*/
      obj_to_room(w, victim->in_room);
    } else {
      act("You try to disarm $N, but $E doesn't have a weapon.", 
	  TRUE, ch, 0, victim, TO_CHAR);
      act("$n makes an impressive fighting move, but does little more.",
	  TRUE, ch, 0, 0, TO_ROOM);
    }
    if ((IS_NPC(victim)) && (GET_POS(victim) > POSITION_SLEEPING) &&
        (!victim->specials.fighting)) {
      set_fighting(victim, ch);
    }
    WAIT_STATE(victim, PULSE_VIOLENCE*2);
    WAIT_STATE(ch, PULSE_VIOLENCE*2);
  }  
}


/*
**   Track:
*/

int named_mobile_in_room(int room, struct hunting_data *c_data)
{
  struct char_data	*scan;

  for (scan = real_roomp(room)->people; scan; scan = scan->next_in_room)
    if (isname(c_data->name, scan->player.name)) {
         *(c_data->victim) = scan;
         return 1;
  }
  return 0;
}

void do_track(struct char_data *ch, char *argument, int cmd)
{
  char name[256], buf[256], found=FALSE;
  int dist, code;
  struct hunting_data	huntd;
  struct char_data *scan;
  extern struct char_data  *character_list;
  
#if NOTRACK
  send_to_char("Sorry, tracking is disabled. Try again after reboot.\n\r",ch);
  return;
#endif

  only_argument(argument, name);
  
  found = FALSE;
  for (scan = character_list; scan && !found ; scan = scan->next)
    if (isname(name, scan->player.name)) {
      found = TRUE;
    }
  
  if (!found) {
    send_to_char("You are unable to find traces of one.\n\r", ch);
    return;
  }
  
  if (!ch->skills) 
    dist = 10;
  else
    dist = ch->skills[SKILL_HUNT].learned;
  
  if (IS_SET(ch->player.class, CLASS_THIEF)) {
    dist *= 3;
  }
  
  switch(GET_RACE(ch)){
  case RACE_ELVEN:
    dist *= 2;               /* even better */
    break;
  case RACE_DEVIL:
  case RACE_DEMON:
    dist = MAX_ROOMS;   /* as good as can be */
    break;
  default:
    break;
  }
  
  if (GetMaxLevel(ch) >= IMMORTAL)
    dist = MAX_ROOMS;
  
  
  if (affected_by_spell(ch, SPELL_MINOR_TRACK)) {
    dist = GetMaxLevel(ch) * 10;
  } else if (affected_by_spell(ch, SPELL_MAJOR_TRACK)){
    dist = GetMaxLevel(ch) * 20;
  }
  
  if (dist == 0)
    return;
  
  ch->hunt_dist = dist;
  
  ch->specials.hunting = 0;
  huntd.name = name;
  huntd.victim = &ch->specials.hunting;

  if ((GetMaxLevel(ch) < MIN_GLOB_TRACK_LEV) ||
      (affected_by_spell(ch, SPELL_MINOR_TRACK)) || 
      (affected_by_spell(ch, SPELL_MAJOR_TRACK))) {
    code = find_path( ch->in_room, named_mobile_in_room, &huntd, -dist, 1);
  } else {
    code = find_path( ch->in_room, named_mobile_in_room, &huntd, -dist, 0);
  }
  
  WAIT_STATE(ch, PULSE_VIOLENCE*1);
  
  if(ch->specials.hunting)  {

    if(GET_RACE(ch->specials.hunting) == RACE_VEGMAN &&
       real_roomp(ch->specials.hunting->in_room)->sector_type != SECT_FOREST) {
      send_to_char("You are unable to find traces of one.\n\r", ch);
      ch->specials.hunting = 0;
      return;
    }
    
    if(IS_SET(ch->specials.act, ACT_SENTINEL) || 
       IS_AFFECTED(ch->specials.hunting, AFF_FLYING)) {
      send_to_char("You are unable to find traces of one.\n\r", ch);
      ch->specials.hunting = 0;
      return;
    }
  }

  if (code == -1) {
    send_to_char("You are unable to find traces of one.\n\r", ch);
    return;
  } else if (code == -69)  {
    send_to_char("Look in front of your nose, dolt!\n\r",ch);
    return;
  } else {
    if (IS_LIGHT(ch->in_room) || IS_AFFECTED(ch, AFF_TRUE_SIGHT) ) {
      SET_BIT(ch->specials.act, PLR_HUNTING);
      sprintf(buf, "You see traces of your quarry to the %s.\n\r", dirs[code]);
      send_to_char(buf,ch);
    } else {
      ch->specials.hunting = 0;
      send_to_char("It's too dark in here to track...\n\r",ch);
      return;
    }
  }
}

int track( struct char_data *ch, struct char_data *vict)
{

  char buf[256];
  int code;

  if ((!ch) || (!vict))
    return(-1);

  if ((GetMaxLevel(ch) < MIN_GLOB_TRACK_LEV) || 
      (affected_by_spell(ch, SPELL_MINOR_TRACK)) || 
      (affected_by_spell(ch, SPELL_MAJOR_TRACK))) {
    code = choose_exit_in_zone(ch->in_room, vict->in_room, ch->hunt_dist);
  } else {
    code = choose_exit_global(ch->in_room, vict->in_room, ch->hunt_dist);
  }
  if ((!ch) || (!vict))
    return(-1);
  

  if (ch->in_room == vict->in_room) {
    send_to_char("##You have found your target!\n\r",ch);
    return(FALSE);  /* false to continue the hunt */
  }
  if (code == -1) {
    send_to_char("##You have lost the trail.\n\r",ch);
    return(FALSE);
  } else {
    sprintf(buf, "##You see a faint trail to the %s.\n\r", dirs[code]);
    send_to_char(buf, ch);
    return(TRUE);
  }

}

int dir_track( struct char_data *ch, struct char_data *vict)
{

  char buf[256];
  int code;

  if ((!ch) || (!vict))
    return(-1);

  
  if ((GetMaxLevel(ch) >= MIN_GLOB_TRACK_LEV) ||
      (affected_by_spell(ch, SPELL_MINOR_TRACK)) || 
      (affected_by_spell(ch, SPELL_MAJOR_TRACK))) {
    code = choose_exit_global(ch->in_room, vict->in_room, ch->hunt_dist);
  } else {
    code = choose_exit_in_zone(ch->in_room, vict->in_room, ch->hunt_dist);
  }
  
  if ((!ch) || (!vict))
    return(-1);

  if (code == -1) {
    if (ch->in_room == vict->in_room) {
      send_to_char("##You have found your target!\n\r",ch);
    } else {
      send_to_char("##You have lost the trail.\n\r",ch);
    }
    return(-1);  /* false to continue the hunt */
  } else {
    sprintf(buf, "##You see a faint trail to the %s.\n\r", dirs[code]);
    send_to_char(buf, ch);
    return(code);
  }

}




/** Perform breadth first search on rooms from start (in_room) **/
/** until end (tgt_room) is reached. Then return the correct   **/
/** direction to take from start to reach end.                 **/

/* thoth@manatee.cis.ufl.edu
   if dvar<0 then search THROUGH closed but not locked doors,
   for mobiles that know how to open doors.
 */

#define IS_DIR    (real_roomp(q_head->room_nr)->dir_option[i])
#define GO_OK  (!IS_SET(IS_DIR->exit_info,EX_CLOSED)\
		 && (IS_DIR->to_room != NOWHERE))
#define GO_OK_SMARTER  (!IS_SET(IS_DIR->exit_info,EX_LOCKED)\
		 && (IS_DIR->to_room != NOWHERE))

void donothing()
{
  return;
}

int find_path(int in_room, int (*predicate)(), void *c_data, 
	      int depth, int in_zone)
{
   struct room_q *tmp_q, *q_head, *q_tail;
#if 1
  struct hash_header	x_room;
/*  static struct hash_header	x_room; */
#else
  struct nodes x_room[MAX_ROOMS];
#endif
   int i, tmp_room, count=0, thru_doors;
  struct room_data	*herep, *therep;
  struct room_data      *startp;
  struct room_direction_data	*exitp;

	/* If start = destination we are done */
   if ((predicate)(in_room, c_data))
     return(-69);		/* <grin> couldn't return a direction */

#if 0
   if (top_of_world > MAX_ROOMS) {
     log("TRACK Is disabled, too many rooms.\n\rContact Loki soon.\n\r");
    return -1;
   }
#endif

   if (depth<0) {
     thru_doors = TRUE;
     depth = - depth;
   } else {
     thru_doors = FALSE;
   }

  startp = real_roomp(in_room);

  init_hash_table(&x_room, sizeof(int), 2048);
  hash_enter(&x_room, in_room, (void*)-1);

	   /* initialize queue */
   q_head = (struct room_q *) malloc(sizeof(struct room_q));
   q_tail = q_head;
   q_tail->room_nr = in_room;
   q_tail->next_q = 0;

  while(q_head) {
    herep = real_roomp(q_head->room_nr);
		/* for each room test all directions */
    if (herep->zone == startp->zone || !in_zone) {  
                                           /* only look in this zone.. 
					      saves cpu time.  makes world
					      safer for players
					      */
      for(i = 0; i <= 5; i++) {
        exitp = herep->dir_option[i];
        if (exit_ok(exitp, &therep) && (thru_doors ? GO_OK_SMARTER : GO_OK)) {
	  /* next room */
	  tmp_room = herep->dir_option[i]->to_room;
	  if(!((predicate)(tmp_room, c_data))) {
	    /* shall we add room to queue ? */
	    /* count determines total breadth and depth */
	    if(!hash_find(&x_room,tmp_room) && (count < depth)
	       && !IS_SET(RM_FLAGS(tmp_room),DEATH)) {
	      count++;
	      /* mark room as visted and put on queue */
	      
	      tmp_q = (struct room_q *) malloc(sizeof(struct room_q));
	      tmp_q->room_nr = tmp_room;
	      tmp_q->next_q = 0;
	      q_tail->next_q = tmp_q;
	      q_tail = tmp_q;
	      
	      /* ancestor for first layer is the direction */
	      hash_enter(&x_room, tmp_room,
			 ((int)hash_find(&x_room,q_head->room_nr) == -1) ?
			 (void*)(i+1) : hash_find(&x_room,q_head->room_nr));
	    }
	  } else {
	    /* have reached our goal so free queue */
	    tmp_room = q_head->room_nr;
	    for(;q_head;q_head = tmp_q)   {
	      tmp_q = q_head->next_q;
	      free(q_head);
	    }
	    /* return direction if first layer */
	    if ((int)hash_find(&x_room,tmp_room)==-1) {
              if (x_room.buckets) { /* junk left over from a previous track */
		destroy_hash_table(&x_room, donothing);
              }
	      return(i);
	    } else {  /* else return the ancestor */
	      int i;
	      
              i = (int)hash_find(&x_room,tmp_room);
              if (x_room.buckets) { /* junk left over from a previous track */
		destroy_hash_table(&x_room, donothing);
              }
	      return( -1+i);
	    }
	  }
	}
      }
    }
  
      /* free queue head and point to next entry */
      tmp_q = q_head->next_q;
      free(q_head);
      q_head = tmp_q;
   }
   /* couldn't find path */
   if (x_room.buckets) { /* junk left over from a previous track */
      destroy_hash_table(&x_room, donothing);
   } 
   return(-1);

}


int choose_exit_global(int in_room, int tgt_room, int depth)
{
  return find_path(in_room, is_target_room_p, (void*)tgt_room, depth, 0);
}

int choose_exit_in_zone(int in_room, int tgt_room, int depth)
{
  return find_path(in_room, is_target_room_p, (void*)tgt_room, depth, 1);
}

int go_direction(struct char_data *ch, int dir)     
{
  if (ch->specials.fighting)
    return 0;
  
  if (!IS_SET(EXIT(ch,dir)->exit_info, EX_CLOSED)) {
    do_move(ch, "", dir+1);
  } else if ( IsHumanoid(ch) && !IS_SET(EXIT(ch,dir)->exit_info, EX_LOCKED) ) {
    open_door(ch, dir);
    return 0;
  }
}


void slam_into_wall( struct char_data *ch, struct room_direction_data *exitp)
{
  char doorname[128];
  char buf[256];
  
  if (exitp->keyword && *exitp->keyword) {
    if ((strcmp(fname(exitp->keyword), "secret")==0) ||
	(IS_SET(exitp->exit_info, EX_SECRET))) {
      strcpy(doorname, "wall");
    } else {
      strcpy(doorname, fname(exitp->keyword));
    }
  } else {
    strcpy(doorname, "barrier");
  }
  sprintf(buf, "You slam against the %s with no effect.\n\r", doorname);
  send_to_char(buf, ch);
  send_to_char("OUCH!  That REALLY Hurt!\n\r", ch);
  sprintf(buf, "$n crashes against the %s with no effect.\n\r", doorname);
  act(buf, FALSE, ch, 0, 0, TO_ROOM);
  GET_HIT(ch) -= number(1, 10)*2;
  if (GET_HIT(ch) < 0)
    GET_HIT(ch) = 0;
  GET_POS(ch) = POSITION_STUNNED;
  return;
}


/*
  skill to allow fighters to break down doors
*/
void do_doorbash( struct char_data *ch, char *arg, int cmd)
{
  extern char *dirs[];
  int dir;
  int ok;
  struct room_direction_data *exitp;
  int was_in, roll;

  char buf[256], type[128], direction[128];

  if (GET_MOVE(ch) < 10) {
    send_to_char("You're too tired to do that\n\r", ch);
    return;
  }

  if (MOUNTED(ch)) {
    send_to_char("Yeah... right... while mounted\n\r", ch);
    return;
  }

  /*
    make sure that the argument is a direction, or a keyword.
  */

  for (;*arg == ' '; arg++);

  argument_interpreter(arg, type, direction);

  if ((dir = find_door(ch, type, direction)) >= 0) {
    ok = TRUE;
  } else {
    act("$n looks around, bewildered.", FALSE, ch, 0, 0, TO_ROOM);
    return;
  }

  if (!ok) {
    send_to_char("Hmm, you shouldn't have gotten this far\n\r", ch);
    return;
  }

  exitp = EXIT(ch, dir);
  if (!exitp) {
    send_to_char("you shouldn't have gotten here.\n\r", ch);
    return;
  }

  if (dir == UP) {
    if (real_roomp(exitp->to_room)->sector_type == SECT_AIR &&
	!IS_AFFECTED(ch, AFF_FLYING)) {
      send_to_char("You have no way of getting there!\n\r", ch);
      return;
    }
  }
  
  sprintf(buf, "$n charges %swards", dirs[dir]);
  act(buf, FALSE, ch, 0, 0, TO_ROOM);
  sprintf(buf, "You charge %swards\n\r", dirs[dir]);
  send_to_char(buf, ch);

  if (!IS_SET(exitp->exit_info, EX_CLOSED)) {
    was_in = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, exitp->to_room);
    do_look(ch, "", 0);

    DisplayMove(ch, dir, was_in, 1);
    if (!check_falling(ch)) {
      if (IS_SET(RM_FLAGS(ch->in_room), DEATH) && 
	  GetMaxLevel(ch) < LOW_IMMORTAL) {
	NailThisSucker(ch);
	return;
      } else {
	WAIT_STATE(ch, PULSE_VIOLENCE*3);
	GET_MOVE(ch) -= 10;
      }
    } else {
      return;
    }
    WAIT_STATE(ch, PULSE_VIOLENCE*3);
    GET_MOVE(ch) -= 10;
    return;
  }

  GET_MOVE(ch) -= 10;

  if (IS_SET(exitp->exit_info, EX_LOCKED) &&
      IS_SET(exitp->exit_info, EX_PICKPROOF)) {
    slam_into_wall(ch, exitp);
    return;
  }

  /*
    now we've checked for failures, time to check for success;
    */
  if (ch->skills) {
    if (ch->skills[SKILL_DOORBASH].learned) {
      roll = number(1, 100);
      if (roll > ch->skills[SKILL_DOORBASH].learned) {
	slam_into_wall(ch, exitp);
	LearnFromMistake(ch, SKILL_DOORBASH, 0, 95);
      } else {
	/*
	  unlock and open the door
	  */
	sprintf(buf, "$n slams into the %s, and it bursts open!", 
		fname(exitp->keyword));
	act(buf, FALSE, ch, 0, 0, TO_ROOM);
	sprintf(buf, "You slam into the %s, and it bursts open!\n\r", 
		fname(exitp->keyword));
	send_to_char(buf, ch);
	raw_unlock_door(ch, exitp, dir);
	raw_open_door(ch, dir);
	GET_HIT(ch) -= number(1,5);
	/*
	  Now a dex check to keep from flying into the next room
	  */
	roll = number(1, 20);
	if (roll > GET_DEX(ch)) {
	  was_in = ch->in_room;

	  char_from_room(ch);
	  char_to_room(ch, exitp->to_room);
	  do_look(ch, "", 0);
	  DisplayMove(ch, dir, was_in, 1);
	  if (!check_falling(ch)) {
	    if (IS_SET(RM_FLAGS(ch->in_room), DEATH) && 
		GetMaxLevel(ch) < LOW_IMMORTAL) {
	      NailThisSucker(ch);
	      return;
	    }
	  } else {
	    return;
	  }
	  WAIT_STATE(ch, PULSE_VIOLENCE*3);
	  GET_MOVE(ch) -= 10;
	  return;	  
	} else {
	  WAIT_STATE(ch, PULSE_VIOLENCE*1);
	  GET_MOVE(ch) -= 5;
	  return;
	}
      }
    } else {
      send_to_char("You just don't know the nuances of door-bashing.\n\r", ch);
      slam_into_wall(ch, exitp);
      return;
    }
  } else {
    send_to_char("You're just a goofy mob.\n\r", ch);
    return;
  }
}

/*
  skill to allow anyone to move through rivers and underwater
*/

void do_swim( struct char_data *ch, char *arg, int cmd)
{

  struct affected_type af;
  byte percent;
  

  send_to_char("Ok, you'll try to swim for a while.\n\r", ch);

  if (IS_AFFECTED(ch, AFF_WATERBREATH)) {
    /* kinda pointless if they don't need to...*/
    return;
  }
  
  if (affected_by_spell(ch, SKILL_SWIM)) {
    send_to_char("You're too exhausted to swim right now\n", ch);
    return;
  }

  percent=number(1,101); /* 101% is a complete failure */

  if (!ch->skills)
    return;
  
  if (percent > ch->skills[SKILL_SWIM].learned) {
    send_to_char("You're too afraid to enter the water.\n\r",ch);
    if (ch->skills[SKILL_SWIM].learned < 95 &&
	ch->skills[SKILL_SWIM].learned > 0) {
      if (number(1,101) > ch->skills[SKILL_SWIM].learned) {
	send_to_char("You feel a bit braver, though.\n\r", ch);
	ch->skills[SKILL_SWIM].learned++;
      }
    }
    return;
  }
 
  af.type = SKILL_SWIM;
  af.duration = (ch->skills[SKILL_SWIM].learned/10)+1;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = AFF_WATERBREATH;
  affect_to_char(ch, &af);

  af.type = SKILL_SWIM;
  af.duration = 13;
  af.modifier = -10;
  af.location = APPLY_MOVE;
  af.bitvector = 0;
  affect_to_char(ch, &af);

}


int SpyCheck(struct char_data *ch)
{

  if (!ch->skills) return(FALSE);

  if (number(1,101) > ch->skills[SKILL_SPY].learned)
    return(FALSE);

  return(TRUE);

}

void do_spy( struct char_data *ch, char *arg, int cmd)
{

  struct affected_type af;

  send_to_char("Ok, you'll try to act like a secret agent.\n\r", ch);

  if (IS_AFFECTED(ch, AFF_SCRYING)) {
    return;
  }
  
  if (affected_by_spell(ch, SKILL_SPY)) {
    send_to_char("You're already acting like a secret agent.\n", ch);
    return;
  }

  if (!ch->skills)
    return;

  if (number(1,101) > ch->skills[SKILL_SPY].learned) {
    LearnFromMistake(ch, SKILL_SPY, 0, 95);

    af.type = SKILL_SPY;
    af.duration = (ch->skills[SKILL_SPY].learned/10)+1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char(ch, &af);    return;
  }
 
  af.type = SKILL_SPY;
  af.duration = (ch->skills[SKILL_SPY].learned/10)+1;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = AFF_SCRYING;
  affect_to_char(ch, &af);    return;
}

int remove_trap( struct char_data *ch, struct obj_data *trap)
{
  int num;

  if (ITEM_TYPE(trap) != ITEM_TRAP) {
    send_to_char("That's no trap!\n\r", ch);
    return(FALSE);
  }
  if (GET_TRAP_CHARGES(trap) <= 0) {
    send_to_char("That trap is already sprung!\n\r", ch);
    return(FALSE);
  }
  num = number(1,101);
  if (num < ch->skills[SKILL_REMOVE_TRAP].learned) {
    send_to_char("<Click>\n\r", ch);
    act("$n disarms $p", FALSE, ch, trap, 0, TO_ROOM);
    GET_TRAP_CHARGES(trap) = 0;
    return(TRUE);
  } else {
    send_to_char("<Click>\n\r(uh oh)\n\r", ch);
    act("$n attempts to disarm $p, ack!", FALSE, ch, trap, 0, TO_ROOM);
    TriggerTrap(ch, trap);
    return(TRUE);
  }
}

void do_feign_death( struct char_data *ch, char *arg, int cmd)
{
  struct room_data *rp;
  struct char_data *t;

  if (!ch->skills)
    return;

  if (!ch->specials.fighting) {
    send_to_char("But you are not fighting anything...\n\r", ch);
    return;
  }
  
  if (!HasClass(ch, CLASS_MONK)) {
    send_to_char("You're no monk!\n\r", ch);
    return;
  }

  if (MOUNTED(ch)) {
    send_to_char("Yeah... right... while mounted\n\r", ch);
    return;
  }

  rp = real_roomp(ch->in_room);
  if (!rp)
    return;

  send_to_char("You try to fake your own demise\n\r", ch);

  death_cry(ch);
  act("$n is dead! R.I.P.", FALSE, ch, 0, 0, TO_ROOM);

  if (number(1,101) < ch->skills[SKILL_FEIGN_DEATH].learned) {
    stop_fighting(ch);
    for (t = rp->people;t;t=t->next_in_room) {
      if (t->specials.fighting == ch) {
	stop_fighting(t);
	if (number(1,101) < ch->skills[SKILL_FEIGN_DEATH].learned/2)
	  SET_BIT(ch->specials.affected_by, AFF_HIDE);
	GET_POS(ch) = POSITION_SLEEPING;
      }
    }
    WAIT_STATE(ch, PULSE_VIOLENCE*2);
    return;
  } else {
    GET_POS(ch) = POSITION_SLEEPING;
    WAIT_STATE(ch, PULSE_VIOLENCE*3);
    LearnFromMistake(ch, SKILL_FEIGN_DEATH, 0, 95);
  }
}


void do_first_aid( struct char_data *ch, char *arg, int cmd)
{
  struct affected_type af;
    
  send_to_char("You attempt to render first aid unto yourself.\n\r", ch);

  if (affected_by_spell(ch, SKILL_FIRST_AID)) {
    send_to_char("You can only do this once per day.\n\r", ch);
    return;
  }

  if (number(1,101) < ch->skills[SKILL_FIRST_AID].learned) {
    GET_HIT(ch)+= number(1,20) + (GetMaxLevel(ch)/2);
    if(GET_HIT(ch) > GET_MAX_HIT(ch))
       GET_HIT(ch) = GET_MAX_HIT(ch);

    af.duration = 24;
  } else {
    af.duration = 6;
    LearnFromMistake(ch, SKILL_FEIGN_DEATH, TRUE, 95);
  }

  af.type = SKILL_FIRST_AID;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char(ch, &af);    
  return;  
}


void do_disguise(struct char_data *ch, char *argument, int cmd)
{
  struct affected_type af;
    
  send_to_char("You attempt to disguise yourself\n\r", ch);

  if (affected_by_spell(ch, SKILL_DISGUISE)) {
    send_to_char("You can only do this once per day\n\r", ch);
    return;
  }

  if (number(1,101) < ch->skills[SKILL_DISGUISE].learned) {
    struct char_data *k;

    for (k=character_list; k; k=k->next) {
      if (k->specials.hunting == ch) {
	k->specials.hunting = 0;
      }
      if (number(1,101) < ch->skills[SKILL_DISGUISE].learned) {
	if (Hates(k, ch)) {
	  ZeroHatred(k, ch);
	}
	if (Fears(k, ch)) {
	  ZeroFeared(k, ch);
	}
      }
    }
  } else {
    LearnFromMistake(ch, SKILL_DISGUISE, 0, 95);
  }

  af.type = SKILL_DISGUISE;
  af.duration = 24;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char(ch, &af);    
  return;  
}

/* Skill for climbing walls and the like -DM */
void do_climb( struct char_data *ch, char *arg, int cmd)
{
  extern char *dirs[];
  int dir;
  struct room_direction_data *exitp;
  int was_in, roll;
  extern char *dirs[];

  char buf[256], type[128], direction[128];

  if (GET_MOVE(ch) < 10) {
    send_to_char("You're too tired to do that\n\r", ch);
    return;
  }

  if (MOUNTED(ch)) {
    send_to_char("Yeah... right... while mounted\n\r", ch);
    return;
  }

  /*
    make sure that the argument is a direction, or a keyword.
  */

  for (;*arg == ' '; arg++);

  only_argument(arg,direction);

  if ((dir = search_block(direction, dirs, FALSE)) < 0) {
    send_to_char("You can't climb that way.\n\r", ch);
    return;
  }

  exitp = EXIT(ch, dir);
  if (!exitp) {
    send_to_char("You can't climb that way.\n\r", ch);
    return;
  }

  if(!IS_SET(exitp->exit_info, EX_CLIMB)) {
    send_to_char("You can't climb that way.\n\r", ch);
    return;
  }

  if (dir == UP) {
    if (real_roomp(exitp->to_room)->sector_type == SECT_AIR &&
	!IS_AFFECTED(ch, AFF_FLYING)) {
      send_to_char("You have no way of getting there!\n\r", ch);
      return;
    }
  }

  if (IS_SET(exitp->exit_info, EX_ISDOOR) &&
      IS_SET(exitp->exit_info, EX_CLOSED)) {
    send_to_char("You can't climb that way.\n\r", ch);
    return;
  }

  sprintf(buf, "$n attempts to climb %swards", dirs[dir]);
  act(buf, FALSE, ch, 0, 0, TO_ROOM);
  sprintf(buf, "You attempt to climb %swards\n\r", dirs[dir]);
  send_to_char(buf, ch);

  GET_MOVE(ch) -= 10;

  /*
    now we've checked for failures, time to check for success;
    */
  if (ch->skills) {
    if (ch->skills[SKILL_CLIMB].learned) {
      roll = number(1, 100);
      if (roll > ch->skills[SKILL_CLIMB].learned) {
	slip_in_climb(ch, dir, exitp->to_room);
	LearnFromMistake(ch, SKILL_CLIMB, 0, 95);
      } else {

	  was_in = ch->in_room;

	  char_from_room(ch);
	  char_to_room(ch, exitp->to_room);
	  do_look(ch, "", 0);
	  DisplayMove(ch, dir, was_in, 1);
	  if (!check_falling(ch)) {
	    if (IS_SET(RM_FLAGS(ch->in_room), DEATH) && 
		GetMaxLevel(ch) < LOW_IMMORTAL) {
	      NailThisSucker(ch);
	      return;
	    }

	  }
	  WAIT_STATE(ch, PULSE_VIOLENCE*3);
	  GET_MOVE(ch) -= 10;
	  return;	  
	}
      }
     else {
      send_to_char("You just don't know the nuances of climbing.\n\r", ch);
      slip_in_climb(ch, dir, exitp->to_room);
      return;
    }
  } else {
    send_to_char("You're just a goofy mob.\n\r", ch);
    return;
  }
}


void slip_in_climb(struct char_data *ch, int dir, int room)
{
 int i;

 i = number(1, 6);

 if(dir != DOWN) {
   act("$n falls down and goes splut.", FALSE, ch, 0, 0, TO_ROOM);
   send_to_char("You fall.\n\r", ch);
 }

 else {
   act("$n loses $s grip and falls further down.", FALSE, ch, 0, 0, TO_ROOM);
   send_to_char("You slip and start to fall.\n\r", ch);
   i += number(1, 6);
   char_from_room(ch);
   char_to_room(ch, room);
   do_look(ch, "", 0);
 }

 GET_POS(ch) = POSITION_SITTING;
 if(i > GET_HIT(ch))
   GET_HIT(ch) = 1;
 else
   GET_HIT(ch) -= i;
}

void do_palm( struct char_data *ch, char *arg, int cmd)
{
  char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH], 
  buffer[MAX_STRING_LENGTH];
  struct obj_data *sub_object;
  struct obj_data *obj_object;
  bool has=FALSE;

  if(!ch->desc || !ch->skills) {
    send_to_char("You are unable to use this command right now.\n\r", ch);
    return;
  }

  if(number(1,101) > ch->skills[SKILL_PALM].learned || 
     !(HasClass(ch, CLASS_THIEF) && !IsIntrinsic(ch, SKILL_SPY))) {

    do_get(ch, arg, cmd);
    return;
  }
  
  argument_interpreter(arg, arg1, arg2);
  
  if(!*arg1) {
    send_to_char("Palm what doofus?\n\r",ch);
  } else if(*arg1 && !*arg2) {
    
    if(!str_cmp(arg1,"all")) {
      send_to_char("Palm everything?  Are you mad?!?!\n\r", ch);
      return;
    }
    
    obj_object = get_obj_in_list_vis(ch, arg1,
				     real_roomp(ch->in_room)->contents);

    if(obj_object) {
      if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
	if ((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) <
	    CAN_CARRY_W(ch)) {
	  if (CAN_WEAR(obj_object,ITEM_TAKE)) {
	    if (obj_object->in_room == NOWHERE) {
	      obj_object->in_room = ch->in_room;
	    }
	    obj_from_room(obj_object);
	    obj_to_char(obj_object, ch);
	    act("You get $p.", 0, ch, obj_object, 0, TO_CHAR);
	    if((obj_object->obj_flags.type_flag == ITEM_MONEY)) {
	      if (obj_object->obj_flags.value[0]<1)
		obj_object->obj_flags.value[0] = 1;
	      obj_from_char(obj_object);
	      sprintf(buffer,"There %s %d coins.\n\r",
		      obj_object->obj_flags.value[0] > 1 ? "were" : "was",
		      obj_object->obj_flags.value[0]);
	      send_to_char(buffer,ch);
	      GET_GOLD(ch) += obj_object->obj_flags.value[0];
	      if (GET_GOLD(ch) > 100000 && 
		  obj_object->obj_flags.value[0] > 10000) {
		char buf[MAX_INPUT_LENGTH];
		sprintf(buf,"%s just got %d coins!",
			GET_NAME(ch),obj_object->obj_flags.value[0]);
		logE(buf);
	      }
	      extract_obj(obj_object);
	    }
	  } else {
	    send_to_char("You can't take that.\n\r", ch);
	    return;
	  }
	} else {
	  sprintf(buffer,"%s : You can't carry that much weight.\n\r",
		  obj_object->short_description);
	  send_to_char(buffer, ch);
	  return;
	}
      } else {
	sprintf(buffer,"%s : You can't carry that many items.\n\r",
		obj_object->short_description);
	send_to_char(buffer, ch);
	return;
      }
    } else {
      sprintf(buffer,"You do not see a %s here.\n\r", arg1);
      send_to_char(buffer, ch);
      return;
    }
  } else {			/* arg1 && arg2 */
    if(!str_cmp(arg1,"all")) {
      send_to_char("Palm everything?  Are you mad?!?!\n\r", ch);
      return;
    }
    sub_object = (struct obj_data *)get_obj_vis_accessible(ch, arg2);
    if (sub_object) {
      if(get_obj_in_list_vis(ch,arg2, ch->carrying)) has=TRUE;
      if (GET_ITEM_TYPE(sub_object) == ITEM_CONTAINER) {
	obj_object = get_obj_in_list_vis(ch, arg1,
					 sub_object->contains);
	if (obj_object) {
	  if (CheckForInsideTrap(ch, sub_object))
	    return;
	  if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
	    if (has || (IS_CARRYING_W(ch) + obj_object->obj_flags.weight) <
		CAN_CARRY_W(ch)) {
	      if (CAN_WEAR(obj_object,ITEM_TAKE)) {
		if (!IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED)) {
		  obj_from_obj(obj_object);
		  obj_to_char(obj_object, ch);
	  act("You get $p from $P.",0,ch,obj_object,sub_object,TO_CHAR);
		  if((obj_object->obj_flags.type_flag == ITEM_MONEY)) {
		    if (obj_object->obj_flags.value[0]<1)
		      obj_object->obj_flags.value[0] = 1;
		    obj_from_char(obj_object);
		    sprintf(buffer,"There %s %d coins.\n\r",
			   obj_object->obj_flags.value[0] > 1 ? "were" : "was",
			    obj_object->obj_flags.value[0]);
		    send_to_char(buffer,ch);
		    GET_GOLD(ch) += obj_object->obj_flags.value[0];
		    if (GET_GOLD(ch) > 100000 &&
			obj_object->obj_flags.value[0] > 10000) {
		      char buf[MAX_INPUT_LENGTH];
		      sprintf(buf,"%s just got %d coins!",
			      GET_NAME(ch),obj_object->obj_flags.value[0]);
		      logE(buf);
		    }
		    extract_obj(obj_object);
		  }
		} else {
		  act("$P must be opened first.",1,ch,0,sub_object,TO_CHAR);
		}
	      } else {
		send_to_char("You can't take that.\n\r", ch);
	      }
	    } else {
	      sprintf(buffer,"%s : You can't carry that much weight.\n\r",
		      obj_object->short_description);
	      send_to_char(buffer, ch);
	    }
	  } else {
	    sprintf(buffer,"%s : You can't carry that many items.\n\r",
		    obj_object->short_description);
	    send_to_char(buffer, ch);
	  }
	} else {
	  sprintf(buffer,"%s does not contain the %s.\n\r",
		  sub_object->short_description, arg1);
	  send_to_char(buffer, ch);
	}
      } else {
	sprintf(buffer,"%s is not a container.\n\r", 
		sub_object->short_description);
      }
    } else {
      sprintf(buffer,"You do not see or have the %s.\n\r", arg2);
      send_to_char(buffer, ch);
    }
  }
}


void do_peek( struct char_data *ch, char *arg, int cmd)
{
  char *argument;
  struct char_data *peeked;
  struct  obj_data *dummy;

  if(!ch->desc || !ch->skills) {
    send_to_char("You are unable to use this command right now.\n\r", ch);
    return;
  }

  if ( IS_AFFECTED(ch, AFF_BLIND) ) {
    send_to_char("You can't see a damn thing, you're blinded!\n\r", ch);
    return;
  }

  if ( IS_DARK(ch->in_room) ) {
    if(!SUNPROBLEM(ch) && IS_AFFECTED(ch, AFF_INFRAVISION) && 
       !IS_AFFECTED(ch, AFF_TRUE_SIGHT)) {
      send_to_char("Your infravision can't discern the items!\n\r",ch);
      return;
    }
  }

  if(!IS_IMMORTAL(ch)) {
    if( number(1,101) > ch->skills[SKILL_PEEK].learned || 
       !(HasClass(ch, CLASS_THIEF) && !IsIntrinsic(ch, SKILL_SPY))) {
      do_look(ch, arg, cmd);
      LearnFromMistake(ch, SKILL_PEEK, 0, 95);
      return;
    }
  }

  argument = arg;

  if(!strn_cmp(arg,"at",2))
    argument = arg+3;

  if(generic_find(argument, FIND_CHAR_ROOM, ch, &peeked, &dummy)) {
    show_char_to_char(peeked, ch, 1);
  } else {
    send_to_char("Heh heh, peek at whom?\n\r", ch);
  }
}

void do_berserk( struct char_data *ch, char *arg, int cmd)
{

  struct affected_type af;
  
  if(!ch->skills)
    return;

  if (IS_NPC(ch)) {
    send_to_char("Funny, you don't feel like a real player ogre.\n\r",ch);
    return;
  }

  if (affected_by_spell(ch, SKILL_BERSERK)) {
    send_to_char("You have not recovered completely from the last time!\n\r",
		 ch);
    return;
  }

  if(IS_AFFECTED2(ch, AFF2_BERSERK)) {
    send_to_char("But you allready are!\n\r",ch);
    return;
  }

  if (!ch->specials.fighting) {
    act("$n gets worked up into a lather and runs madly about!",TRUE, ch, 0, 
	0, TO_ROOM);
    send_to_char("You get all worked up over nothing and run madly about.\n\r",
		 ch);
    send_to_char("You get the feeling that this works better in combat.\n\r",
		 ch);
    return;
  }

  if(GET_RACE(ch) != RACE_OGRE) {
    send_to_char("Hey, you aint no ogre, go home wuss.\n\r",ch);
    return;
  }

  /* ok, we assume we have a fighting ogre */

  if(number(1,101) > ch->skills[SKILL_BERSERK].learned) {
    send_to_char("You fail to go totally ape and kill everyone!\n\r",ch);
    act("$n starts snorting and huffing, but stops.",TRUE,ch,0,0,TO_ROOM);
    WAIT_STATE(ch, PULSE_VIOLENCE);
  } else {
    if (GET_POS(ch) >= POSITION_FIGHTING) {
      SET_BIT(ch->specials.affected_by2, AFF2_BERSERK);
      af.type = SKILL_BERSERK;
      af.duration = 24;
      af.modifier = 30;
      af.location = APPLY_AC;
      af.bitvector =0;

      affect_to_char(ch, &af);

      af.modifier = 1;
      af.location = APPLY_HASTE;
      affect_to_char(ch, &af);

      af.modifier = 3;
      af.location = APPLY_DAMROLL;
      affect_to_char(ch, &af);
      
      send_to_char("You fly into a furious rage!\n\r",ch);
      act("$n roars furiously and starts squashing $s opponents!\n\r",TRUE,
	  ch,0, 0, TO_ROOM);
      
    } else {
      send_to_char("Maybe you should get on your feet first?\n\r",ch);
    }
  }
}

void do_makepotion(struct char_data *ch, char *argument, int cmd)
{
  int i, ingredients=0 ,which_potion=0, match=0, j, max;
  bool object[5];
  struct obj_data *o, *in_o, *next, *potion;
  struct room_data *rp;
  char buf[80];

  extern struct index_data *obj_index;
  extern struct BrewMeister BrewList[MAX_POTIONS];

  for(i=0;i<5;i++)		/* very important indeed */
    object[i] = 0;
  
  for(i=0; i<MAX_POTIONS; i++) {
    if(!strcmp(argument, BrewList[i].keyword)) {
      which_potion = i;
      for(j=0;j<5;j++)
	if(BrewList[which_potion].object[j] > 0) {
	  ingredients++;	/* number of valid ingredients */
	}
      break;
    }
  }
 
  /* well, these (and only these) ingredients must be found in a */
  /* cauldron that is sitting in the room.  One communal one in the */
  /* druid's tree, another sold by elvira in her shop :) and one in the*/
  /* mages tower. */

  if(!ingredients) {
    send_to_char("Eh, what?  Have you got a gummi bear up your nose?\n\r", ch);
    send_to_char("You've never heard of such a potion!\n\r", ch);
    return;
  }


#define CAULDRON 1531

  rp = real_roomp(ch->in_room);
  if (!rp) return;
  for (o = rp->contents; o; o = o->next_content) {
    if( obj_index[o->item_number].virtual == CAULDRON ) {
      match = TRUE;		/* o now points at our cauldron */
      break;		
    }
  }
  
  if(!match) {
    send_to_char("Egads, go find Sherlock, your cauldron is gone!\n\r", ch);
    return;
  }
  potion = read_object(BrewList[which_potion].object[5], VIRTUAL); 

  /* is the caster high enough level to cast this? */

  for(max=0,j=1;j<4;j++) {
    if(potion->obj_flags.value[j] >= 1) {
      for(i=0;i<MIN_LEVEL_NUM;i++)
	if(skill_info[potion->obj_flags.value[j]].min_level[i] < LOW_IMMORTAL)
	  max=MAX(max,
		  skill_info[potion->obj_flags.value[j]].min_level[i]);
    }
  }

  {
    char buf[80];
    sprintf(buf,"Min brew level is: %d", max);
    logE(buf);
  }

  extract_obj(potion);

  if(which_potion) 
    if(!max || GetMaxLevel(ch) < max) {
      send_to_char("This brew is beyond your powers.\n\r", ch);
      return;
    }
  else {
    if(GetMaxLevel(ch) < 40) {
      send_to_char("This brew is beyond your powers.\n\r", ch);
      return;
    }
  }
  
  if(GET_ITEM_TYPE(o) == ITEM_CONTAINER) { /* it better be */
    /* our cauldron must contain ONLY the right ingredients */
    for(in_o=o->contains,i=0,match=ingredients; in_o; in_o = next) {
      next = in_o->next_content;
      i++;
      for(j=0;j<5;j++) {
	if (obj_index[in_o->item_number].virtual == 
	    BrewList[which_potion].object[j]) {
	  if(!object[j]) {
	    object[j] = TRUE;
	    match--;
	  }
	}
      }
    }
  }

  if(!i) {
    send_to_char("But the pot is empty?\n\r", ch);
    return;
  }

  if(ingredients != i || match != 0) {
    send_to_char("You don't have all the correct ingredients!\n\r", ch);
    send_to_char("Damn, you lost this batch.\n\r", ch);
  } else if (number(1,101) > ch->skills[SKILL_BREWING].learned) {  
    act("$n accidentally lets a drop of sweat fall into the brew...",
	FALSE, ch, 0, 0, TO_ROOM);
    act("You stood too close to the cauldron.  A drop of your sweat just fell in...", FALSE, ch, 0, 0, TO_CHAR);
    act("The cauldron overflows!  Everything is ruined!", 
	FALSE, ch, 0, 0, TO_ROOM);
    act("Next time pay more attention to your master... You just ruined this batch!", FALSE, ch, 0, 0, TO_CHAR);
  } else {
    potion = read_object(BrewList[which_potion].object[5], VIRTUAL);
    obj_to_room(potion, ch->in_room);
    
    act("$n carefully mixes some secret ingredients in a cauldron.",
	FALSE, ch, 0, 0, TO_ROOM);
    act("You carefully mix the ingredients of an age-old recipe.", 
	FALSE, ch, 0, 0, TO_CHAR);
    act("Suddenly... Rainbows shoot out of the pot, and sparks fly!",
	FALSE, ch, 0, 0, TO_ROOM);
    act("Then in a burst of light, nothing remains but a bright flask!", 
	FALSE, ch, 0, 0, TO_ROOM);
    act("Suddenly... Rainbows shoot out of the pot, and sparks fly!",
	FALSE, ch, 0, 0, TO_CHAR);
    act("Then in a burst of light, nothing remains but a bright flask!",
	  FALSE, ch, 0, 0, TO_CHAR);
  }

  /* we always clean the pot, brew or fail. */

  for(in_o = o->contains; in_o; in_o = next) {
    next = in_o->next_content;
    obj_from_obj(in_o);
    act("$p has vanished in a flash of bright light.", 
	FALSE, ch, in_o, 0, TO_CHAR);
    act("$p has vanished in a flash of bright light.", 
	FALSE, ch, in_o, 0, TO_ROOM);
    extract_obj(in_o);
  }
}

/* skill code pieces contributed by Gecko */

void add_skill(int nr, int taught_by, int class_use, int percent)
{
  int i;

  skill_info[nr].spell_pointer = NULL;
  skill_info[nr].minimum_position = POSITION_STANDING;
  skill_info[nr].min_usesmana = 200;
  skill_info[nr].beats = 0;
  skill_info[nr].min_level[MIN_LEVEL_CLERIC] = LOKI+1; /* changed by Kiku */
  skill_info[nr].min_level[MIN_LEVEL_MAGIC] = LOKI+1;  /* New data structure */
  skill_info[nr].min_level[MIN_LEVEL_DRUID] = LOKI+1;  /* implementation. */
  skill_info[nr].targets = TAR_IGNORE;
  skill_info[nr].spellfail = 0;
  skill_info[nr].percent = percent;                    /* added by Kiku   */

  for (i = 0; i < MAX_RACE_DENY; i++)
    skill_info[nr].race_deny[i] = 0;
  
  for (i = 0; i < MAX_RACE_INTRINSIC; i++)
    skill_info[nr].race_intrinsic[i] = 0;

  skill_info[nr].taught_by = taught_by;
  skill_info[nr].class_use = class_use;
}

void assign_skills()
{
  add_skill(SKILL_SNEAK,       TAUGHT_BY_THIEF | TAUGHT_BY_MONK,
                               CLASS_THIEF     | CLASS_MONK, 45);
  add_skill(SKILL_HIDE,        TAUGHT_BY_THIEF | TAUGHT_BY_MONK,
                               CLASS_THIEF     | CLASS_MONK, 45);
  add_skill(SKILL_STEAL,       TAUGHT_BY_THIEF, CLASS_THIEF, 45);
  add_skill(SKILL_BACKSTAB,    TAUGHT_BY_THIEF, CLASS_THIEF, 45);
  add_skill(SKILL_PICK_LOCK,   TAUGHT_BY_THIEF | TAUGHT_BY_MONK,
                               CLASS_THIEF     | CLASS_MONK, 45);

  add_skill(SKILL_KICK,        TAUGHT_BY_WARRIOR | TAUGHT_BY_MONK,
                               CLASS_WARRIOR     | CLASS_MONK, 45);
  add_skill(SKILL_BASH,        TAUGHT_BY_WARRIOR, CLASS_WARRIOR, 45);
  add_skill(SKILL_RESCUE,      TAUGHT_BY_WARRIOR, CLASS_WARRIOR, 45);

  add_skill(SKILL_DUAL_WIELD,  TAUGHT_BY_NINJA,
                               CLASS_THIEF | CLASS_MONK | CLASS_WARRIOR, 95);
  add_skill(SKILL_FIRST_AID,   TAUGHT_BY_HUNTER, CLASS_ALL, 95);

  add_skill(SKILL_SIGN,        TAUGHT_BY_LORE,   CLASS_ALL, 95);
  add_skill(SKILL_RIDE,        TAUGHT_BY_NINJA,  CLASS_ALL, 95);
  add_skill(SKILL_SWITCH_OPP,  TAUGHT_BY_MONK  | TAUGHT_BY_NINJA,
                               CLASS_MONK      | CLASS_WARRIOR, 45);
  add_skill(SKILL_DODGE,       TAUGHT_BY_MONK,   CLASS_WARRIOR | CLASS_MONK, 95);
  add_skill(SKILL_REMOVE_TRAP, TAUGHT_BY_HUNTER, CLASS_THIEF, 95);

  add_skill(SKILL_RETREAT,     TAUGHT_BY_NINJA | TAUGHT_BY_MONK,
                               CLASS_THIEF | CLASS_WARRIOR | CLASS_MONK, 45);
  add_skill(SKILL_QUIV_PALM,   TAUGHT_BY_MONK,   CLASS_MONK, 45);
  add_skill(SKILL_SAFE_FALL,   TAUGHT_BY_MONK,   CLASS_MONK, 95);
  add_skill(SKILL_FEIGN_DEATH, TAUGHT_BY_MONK,   CLASS_MONK, 45);
  add_skill(SKILL_HUNT,        TAUGHT_BY_HUNTER, CLASS_THIEF, 95);

  add_skill(SKILL_LOCATE_TRAP,   TAUGHT_BY_HUNTER, CLASS_THIEF, 95);
  add_skill(SKILL_SPRING_LEAP, TAUGHT_BY_MONK,   CLASS_MONK, 45);
  add_skill(SKILL_DISARM,      TAUGHT_BY_NINJA | TAUGHT_BY_MONK,
	                       CLASS_WARRIOR   | CLASS_MONK, 45);
  add_skill(SKILL_READ_MAGIC,  TAUGHT_BY_LORE,   CLASS_ALL, 95);
  add_skill(SKILL_EVALUATE,    TAUGHT_BY_HUNTER, CLASS_THIEF, 95);
  
  add_skill(SKILL_SPY,         TAUGHT_BY_NINJA,  CLASS_THIEF, 45);
  add_skill(SKILL_DOORBASH,    TAUGHT_BY_NINJA,  CLASS_WARRIOR, 45);
  add_skill(SKILL_SWIM,        TAUGHT_BY_SAILOR, CLASS_ALL, 60);
  add_skill(SKILL_CONS_UNDEAD, TAUGHT_BY_LORE,   CLASS_ALL, 95);
  add_skill(SKILL_CONS_VEGGIE, TAUGHT_BY_LORE,   CLASS_ALL, 95);
  
  add_skill(SKILL_CONS_DEMON,   TAUGHT_BY_LORE, CLASS_ALL, 95);
  add_skill(SKILL_CONS_ANIMAL,  TAUGHT_BY_LORE, CLASS_ALL, 95);
  add_skill(SKILL_CONS_REPTILE, TAUGHT_BY_LORE, CLASS_ALL, 95);
  add_skill(SKILL_CONS_PEOPLE,  TAUGHT_BY_LORE, CLASS_ALL, 95);
  add_skill(SKILL_CONS_GIANT,   TAUGHT_BY_LORE, CLASS_ALL, 95);
  
  add_skill(SKILL_CONS_OTHER, TAUGHT_BY_LORE,  CLASS_ALL, 95);
  add_skill(SKILL_DISGUISE,   TAUGHT_BY_NINJA, CLASS_THIEF, 45);
  add_skill(SKILL_CLIMB,      TAUGHT_BY_NINJA, CLASS_THIEF, 45);
  add_skill(SKILL_INSET,      TAUGHT_BY_LORE,  CLASS_ALL, 95);
  add_skill(SKILL_BREWING,    TAUGHT_BY_LORE,  CLASS_DRUID, 95);
  add_skill(SKILL_BERSERK,     TAUGHT_BY_ETTIN, 0, 80);
  add_skill(SKILL_PALM,       TAUGHT_BY_THIEF, CLASS_THIEF, 45);
  add_skill(SKILL_PEEK,       TAUGHT_BY_THIEF, CLASS_THIEF, 45);
  add_skill(SKILL_CONS_AVIAN, TAUGHT_BY_LORE, CLASS_ALL, 95);
  add_skill(SKILL_CONS_INSECT, TAUGHT_BY_LORE, CLASS_ALL, 95);
  
  /* Racial access and forbiddance goes below.  Must ensure that */
  /* skills (spells will) allow these via IsIntrinsic() checks.  */

  skill_info[SKILL_SWIM].race_deny[0] = RACE_DWARF;

  skill_info[SPELL_FAERIE_FIRE].race_intrinsic[0] = RACE_ELVEN;
  skill_info[SPELL_FAERIE_FIRE].race_intrinsic[1] = RACE_FAERIE;
  skill_info[SPELL_FAERIE_FIRE].race_intrinsic[2] = RACE_DROW;

  skill_info[SPELL_FAERIE_FOG].race_intrinsic[0] = RACE_ELVEN;
  skill_info[SPELL_FAERIE_FOG].race_intrinsic[1] = RACE_DROW;

  skill_info[SPELL_ENCHANT_WEAPON].race_intrinsic[0] = RACE_DWARF;
  skill_info[SPELL_ENCHANT_ARMOR].race_intrinsic[0] = RACE_DWARF;

  skill_info[SPELL_CURE_LIGHT].race_intrinsic[0] = RACE_FAERIE;
  skill_info[SPELL_WEB].race_intrinsic[0] = RACE_DROW;

  skill_info[SPELL_CHARM_PERSON].race_intrinsic[0] = RACE_VAMPIRE;
  skill_info[SPELL_FEAR].race_intrinsic[0] = RACE_VAMPIRE;

  skill_info[SKILL_BERSERK].race_intrinsic[0] = RACE_OGRE;
  skill_info[SKILL_HUNT].race_intrinsic[0] = RACE_ELVEN;

  skill_info[SKILL_PEEK].race_intrinsic[0] = RACE_FAERIE;

  skill_info[SKILL_RETREAT].race_intrinsic[0] = RACE_HALFLING;
  skill_info[SKILL_RETREAT].race_intrinsic[1] = RACE_DRAAGDIM;

  skill_info[SKILL_SPY].race_intrinsic[0] = RACE_MFLAYER;

  skill_info[SKILL_PALM].race_intrinsic[0] = RACE_HALFLING;
  skill_info[SKILL_PALM].race_intrinsic[1] = RACE_DRAAGDIM;

  skill_info[SKILL_EVALUATE].race_intrinsic[1] = RACE_DWARF;
  skill_info[SKILL_EVALUATE].race_intrinsic[0] = RACE_HALFLING;

  skill_info[SKILL_SAFE_FALL].race_intrinsic[0] = RACE_VAMPIRE;

  skill_info[SKILL_PICK_LOCK].race_intrinsic[0] = RACE_GNOME;
  skill_info[SKILL_REMOVE_TRAP].race_intrinsic[0] = RACE_GNOME;

}
