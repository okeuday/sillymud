
/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "protos.h"

/*   external vars  */

extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *obj_index;
extern struct time_info_data time_info;
extern struct index_data *mob_index;
extern struct weather_data weather_info;
	extern int top_of_world;
	extern struct int_app_type int_app[26];

extern struct title_type titles[4][ABS_MAX_LVL];
extern char *dirs[]; 

extern int gSeason;  /* what season is it ? */

/* chess_game() stuff starts here */
/* Inspiration and original idea by Feith */
/* Implementation by Gecko */

#define WHITE 0
#define BLACK 1

int side = WHITE;  /* to avoid having to pass side with each function call */

#define IS_BLACK(piece) (((piece) >= 1400) && ((piece) <= 1415))
#define IS_WHITE(piece) (((piece) >= 1448) && ((piece) <= 1463))
#define IS_PIECE(piece) ((IS_WHITE(piece)) || (IS_BLACK(piece)))
#define IS_ENEMY(piece) (side?IS_WHITE(piece):IS_BLACK(piece))
#define IS_FRIEND(piece) (side?IS_BLACK(piece):IS_WHITE(piece))
#define ON_BOARD(room) (((room) >= 1400) && ((room) <= 1463))
#define FORWARD (side?2:0)
#define BACK    (side?0:2)
#define LEFT    (side?1:3)
#define RIGHT   (side?3:1)

#define EXIT_ROOM(roomp,dir) ((roomp)?((roomp)->dir_option[dir]):NULL)
#define CAN_GO_ROOM(roomp,dir) (EXIT_ROOM(roomp,dir) && \
                               real_roomp(EXIT_ROOM(roomp,dir)->to_room))
                                
/* get pointer to room in the given direction */                               
#define ROOMP(roomp,dir) ((CAN_GO_ROOM(roomp,dir)) ? \
                          real_roomp(EXIT_ROOM(roomp,dir)->to_room) : NULL)
                       
struct room_data *forward_square(struct room_data *room)
{
  return ROOMP(room, FORWARD);
}

struct room_data *back_square(struct room_data *room)
{
  return ROOMP(room, BACK);
}

struct room_data *left_square(struct room_data *room)
{
  return ROOMP(room, LEFT);
}

struct room_data *right_square(struct room_data *room)
{
  return ROOMP(room, RIGHT);
}

struct room_data *forward_left_square(struct room_data *room)
{
  return ROOMP(ROOMP(room, FORWARD), LEFT);
}

struct room_data *forward_right_square(struct room_data *room)
{
  return ROOMP(ROOMP(room, FORWARD), RIGHT);
}

struct room_data *back_right_square(struct room_data *room)
{
  return ROOMP(ROOMP(room, BACK), RIGHT);
}

struct room_data *back_left_square(struct room_data *room)
{
  return ROOMP(ROOMP(room, BACK), LEFT);
}

struct char_data *square_contains_enemy(struct room_data *square)
{
  struct char_data *i;
  
  for (i = square->people; i; i = i->next_in_room)
    if (IS_ENEMY(mob_index[i->nr].virtual))
      return i;

  return NULL;
}

int square_contains_friend(struct room_data *square)
{
  struct char_data *i;

  for (i = square->people; i; i = i->next_in_room)
    if (IS_FRIEND(mob_index[i->nr].virtual))
      return TRUE;

  return FALSE;
}

int square_empty(struct room_data *square)
{
  struct char_data *i;
  
  for (i = square->people; i; i = i->next_in_room)
    if (IS_PIECE(mob_index[i->nr].virtual))
      return FALSE;

  return TRUE;
}
  
int chess_game(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  struct room_data *rp = NULL, *crp = real_roomp(ch->in_room);
  struct char_data *ep = NULL;
  int move_dir = 0, move_amount = 0, move_found = FALSE;
  int c = 0;

  if (cmd || !AWAKE(ch))
    return FALSE;

  /* keep original fighter() spec_proc for kings and knights */    
  if (ch->specials.fighting)
    switch (mob_index[ch->nr].virtual) {
      case 1401: case 1404: case 1406: case 1457: case 1460: case 1462:
        return fighter(ch, cmd, arg, mob, type);
      default:
        return FALSE;
    }

  if (!crp || !ON_BOARD(crp->number))
    return FALSE;

  if (side == WHITE && IS_BLACK(mob_index[ch->nr].virtual))
    return FALSE;

  if (side == BLACK && IS_WHITE(mob_index[ch->nr].virtual))
    return FALSE;

  if (number(0,15))
    return FALSE;

  switch (mob_index[ch->nr].virtual) {
    case 1408: case 1409: case 1410: case 1411:  /* black pawns */
    case 1412: case 1413: case 1414: case 1415:
    case 1448: case 1449: case 1450: case 1451:  /* white pawns */
    case 1452: case 1453: case 1454: case 1455:
      move_dir = number(0,3);
      switch (move_dir) {
        case 0: rp = forward_left_square(crp);  break;
        case 1: rp = forward_right_square(crp); break;
        case 2: rp = forward_square(crp);       break;
        case 3: 
          if (real_roomp(ch->in_room) &&
              (real_roomp(ch->in_room)->number == mob_index[ch->nr].virtual)) {
            rp = forward_square(crp); 
            if (rp && square_empty(rp) && ON_BOARD(rp->number)) {
              crp = rp;
              rp = forward_square(crp);
            }
          }
      }
      if (rp && (!square_contains_friend(rp)) && ON_BOARD(rp->number)) {
        ep = square_contains_enemy(rp);
        if (((move_dir <= 1) && ep) || ((move_dir > 1) && !ep))
          move_found = TRUE;
      }
      break;

    case 1400:  /* black rooks */
    case 1407:
    case 1456:  /* white rooks */
    case 1463:
      move_dir = number(0,3);
      move_amount = number(1,7);
      for (c = 0; c < move_amount; c++) {
        switch(move_dir) {
          case 0: rp = forward_square(crp);  break;
          case 1: rp = back_square(crp);     break;
          case 2: rp = right_square(crp);    break;
          case 3: rp = left_square(crp);
        }
        if (rp && !square_contains_friend(rp) && ON_BOARD(rp->number)) {
          move_found = TRUE;
          if ((ep = square_contains_enemy(rp)))
            c = move_amount;
          else
            crp = rp;
        }
        else {
          c = move_amount;
          rp = crp;
        }
      }
      break;
      
    case 1401:  /* black knights */
    case 1406:
    case 1457:  /* white knights */
    case 1462:
      move_dir = number(0,7);
      switch(move_dir) {
        case 0: rp = forward_left_square(forward_square(crp));  break;
        case 1: rp = forward_right_square(forward_square(crp)); break;
        case 2: rp = forward_right_square(right_square(crp));   break;
        case 3: rp = back_right_square(right_square(crp));  break;
        case 4: rp = back_right_square(back_square(crp));   break;
        case 5: rp = back_left_square(back_square(crp));    break;
        case 6: rp = back_left_square(left_square(crp));    break;
        case 7: rp = forward_left_square(left_square(crp));
      }
      if (rp && !square_contains_friend(rp) && ON_BOARD(rp->number)) {
        move_found = TRUE;
        ep = square_contains_enemy(rp);
      }
      break;
      
    case 1402:  /* black bishops */
    case 1405:
    case 1458:  /* white bishops */
    case 1461:
      move_dir = number(0,3);
      move_amount = number(1,7);
      for (c = 0; c < move_amount; c++) {
        switch(move_dir) {
          case 0: rp = forward_left_square(crp);  break;
          case 1: rp = forward_right_square(crp); break;
          case 2: rp = back_right_square(crp);    break;
          case 3: rp = back_left_square(crp);
        }
        if (rp && !square_contains_friend(rp) && ON_BOARD(rp->number)) {
          move_found = TRUE;
          if ((ep = square_contains_enemy(rp)))
            c = move_amount;
          else
            crp = rp;
        }
        else {
          c = move_amount;
          rp = crp;
        }
      }
      break;
      
    case 1403:  /* black queen */
    case 1459:  /* white queen */
      move_dir = number(0,7);
      move_amount = number(1,7);
      for (c = 0; c < move_amount; c++) {
        switch(move_dir) {
          case 0: rp = forward_left_square(crp);  break;
          case 1: rp = forward_square(crp);       break;
          case 2: rp = forward_right_square(crp); break;
          case 3: rp = right_square(crp);         break;
          case 4: rp = back_right_square(crp);    break;
          case 5: rp = back_square(crp);          break;
          case 6: rp = back_left_square(crp);     break;
          case 7: rp = left_square(crp);
        }
        if (rp && !square_contains_friend(rp) && ON_BOARD(rp->number)) {
          move_found = TRUE;
          if ((ep = square_contains_enemy(rp)))
            c = move_amount;
          else
            crp = rp;
        }
        else {
          c = move_amount;
          rp = crp;
        }
      }
      break;
            
    case 1404:  /* black king */
    case 1460:  /* white king */ 
      move_dir = number(0,7);
      switch (move_dir) {
        case 0: rp = forward_left_square(crp);  break;
        case 1: rp = forward_square(crp);       break;
        case 2: rp = forward_right_square(crp); break;
        case 3: rp = right_square(crp);         break;
        case 4: rp = back_right_square(crp);    break;
        case 5: rp = back_square(crp);          break;
        case 6: rp = back_left_square(crp);     break;
        case 7: rp = left_square(crp);
      }
      if (rp && !square_contains_friend(rp) && ON_BOARD(rp->number)) {
        move_found = TRUE;
        ep = square_contains_enemy(rp);
      }
      break;  
  }

  if (move_found && rp) {
    do_emote(ch, "leaves the room.", 0);    
    char_from_room(ch);
    char_to_room(ch, rp->number);
    do_emote(ch, "has arrived.", 0);
    
    if (ep) {
      if (side)
        switch(number(0,3)) {
          case 0: 
            do_emote(ch, "grins evilly and says, 'ONLY EVIL shall rule!'", 0);  
            break;
          case 1: 
            do_emote(ch, "leers cruelly and says, 'You will die now!'", 0);
            break;
          case 2: 
            do_emote(ch, "issues a bloodcurdling scream.", 0);
            break;
          case 3: 
            do_emote(ch, "glares with black anger.", 0);
        }
      else
        switch(number(0,3)) {
          case 0: 
            do_emote(ch, "glows an even brighter pristine white.", 0);
            break;
          case 1: 
            do_emote(ch, "chants a prayer and begins battle.", 0);
            break;
          case 2: 
            do_emote(ch, "says, 'Black shall lose!", 0);
            break;
          case 3: 
            do_emote(ch, "shouts, 'For the Flame! The Flame!'", 0);
        }
      hit(ch, ep, TYPE_UNDEFINED);
    }
    side = (side + 1) % 2;
    return TRUE;
  }
  return FALSE;
}

int AcidBlob(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  struct obj_data *i;
  
  if (cmd || !AWAKE(ch))
    return(FALSE);
  
  for (i = real_roomp(ch->in_room)->contents; i; i = i->next_content) {
    if (IS_SET(i->obj_flags.wear_flags, ITEM_TAKE) && !strncmp(i->name, "corpse", 6)) {
      act("$n destroys some trash.", FALSE, ch, 0, 0, TO_ROOM);
      
      obj_from_room(i);
      extract_obj(i);
      return(TRUE);
    }
  }
  return(FALSE);
}
 
int death_knight(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{

  if (cmd) return(FALSE);
  if (!AWAKE(mob)) return(FALSE);

  if (number(0,1)) {
    return(fighter(mob, cmd, arg, mob, type));
  } else {
    return(magic_user(mob, cmd, arg, mob, type));
  }
}

int baby_bear(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  struct char_data *t;
  struct room_data *rp;

  if(cmd)
    return(FALSE);

  if(ch->specials.fighting)  {
    command_interpreter(ch,"scream");
    rp = real_roomp(ch->in_room);
    if (!rp) return(FALSE);

    for(t=rp->people; t ; t=t->next_in_room) {
      if(GET_POS(t) == POSITION_SLEEPING)
        if(number(0,1))
          do_wake(t,"",0);
    }
  }
  return(FALSE);
}

#define TimnusRoom 22602
#define TIMNUSNORTHLIMIT 30
#define TIMNUSWESTLIMIT 12

int timnus(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  /* north = 1 */
  /* west  = 4 */

  struct char_data *vict;
  byte lspell;

/* This will stop people from moving Timnus :) */

  if(!(str_cmp("Timnus", (ch)->player.short_descr))) {
    if((ch->in_room) != TimnusRoom) {
      act("$n says 'Hey!  I'm not supposed to be here!'",1,ch,0,0,TO_ROOM);
      act("$n glares at you, and then suddenly, is gone!",1,ch,0,0,TO_ROOM);
      if (ch->specials.fighting)
        stop_fighting(ch);
      char_from_room(ch);
      char_to_room(ch, TimnusRoom);
    }
  }

  if(cmd) {
    if(cmd == 1 && ch->in_room == TimnusRoom) {
      if ((TIMNUSNORTHLIMIT < GetMaxLevel(ch)) &&
	  (GetMaxLevel(ch) < LOW_IMMORTAL))       {
	if (!check_soundproof(ch)) {
	  act("$n tells you 'Thou art not pure enough of heart.'", TRUE,
	      mob, 0, ch, TO_VICT);
	}
	act("$n grins evilly.", TRUE, mob, 0, ch, TO_VICT);
	return(TRUE);
      }
      return(FALSE);
    } else if(cmd == 4 && ch->in_room == TimnusRoom) {
      if ((TIMNUSWESTLIMIT < GetMaxLevel(ch)) &&
	  (GetMaxLevel(ch) < LOW_IMMORTAL))       {
	if (!check_soundproof(ch)) {
	  act("$n tells you 'Thou art not pure enough of heart.'", TRUE,
	      mob, 0, ch, TO_VICT);
	}
	act("$n grins evilly.", TRUE, mob, 0, ch, TO_VICT);
	return(TRUE);
      }
    }
    return(FALSE);
  } else {			/* end of if(cmd) */
    if(!IS_PC(ch)) {
      if (ch->specials.fighting) {
	if ((GET_POS(ch) < POSITION_FIGHTING) &&
	    (GET_POS(ch) > POSITION_STUNNED)){
	  StandUp(ch);
	} else {
	  if(check_soundproof(ch) || check_nomagic(ch,0,0))
	    MakeNiftyAttack(ch);
	  else {
	    vict = FindVictim(ch);
	    if(!vict)
	      vict = ch->specials.fighting;
	    if(!vict)
	      return(FALSE);
	    lspell = number(0,GetMaxLevel(ch));
	    if (!IS_PC(ch)) {
	      lspell+= GetMaxLevel(ch)/5;
	    }
	    lspell = MIN(GetMaxLevel(ch), lspell);
	    
	    if(lspell < 1)
	      lspell=1;
	    
	    if(IS_AFFECTED(ch, AFF_BLIND) && (lspell > 15)) {
	      act("$n utters the words 'Let me see the light!'",
		  TRUE, ch, 0, 0, TO_ROOM);
	      cast_cure_blind(GetMaxLevel(ch),ch,"",SPELL_TYPE_SPELL,ch,0);
	      return(FALSE);
	    }
	    
	    if(GET_MOVE(ch) < 0) {
	      act("$n pulls a glass of lemonade out of thin air.  How refreshing.",
		  1,ch,0,0,TO_ROOM);
	      cast_refresh(GetMaxLevel(ch),ch,"",SPELL_TYPE_SPELL,ch,0);
	      return(FALSE);
	    }

	    if(!(IS_AFFECTED(ch,AFF_FIRESHIELD)) && (lspell > 39)) {
	      act("$n utters the words 'FLAME ON!!!'",1,ch,0,0,TO_ROOM);
	      SET_BIT(ch->specials.affected_by, AFF_FIRESHIELD);
              act("$n is surrounded by a glowing red aura.",TRUE,ch,0,0,TO_ROOM);
              act("You start glowing red.",TRUE,ch,0,0,TO_CHAR);
	      return(FALSE);
	    }
	    
	    if(!(IS_AFFECTED(ch,AFF_SANCTUARY)) && (lspell > 25)) {
              act("$n utters the words 'Don't you just hate it when I do this?'",1,ch,0,0,TO_ROOM);
              SET_BIT(ch->specials.affected_by, AFF_SANCTUARY);
              act("$n is surrounded by a white aura.",TRUE,ch,0,0,TO_ROOM);
              act("You start glowing.",TRUE,ch,0,0,TO_CHAR);
              return(FALSE);
            }

            if(((IS_AFFECTED(vict, AFF_SANCTUARY)) && (lspell > 25)) && (GetMaxLevel(ch) >= GetMaxLevel(vict))) {
              act("$n utters the words 'Do unto others as you'd have them do unto you...'", 
	      1, ch, 0, 0, TO_ROOM);
              cast_dispel_magic(GetMaxLevel(ch),ch,"",SPELL_TYPE_SPELL,vict,0);
              return(FALSE);
            }
  
            if(((IS_AFFECTED(vict, AFF_FIRESHIELD)) && (lspell > 25)) && (GetMaxLevel(ch) >= GetMaxLevel(vict))) {
              act("$n utters the words 'Do unto others as you'd have them do unto you...'", 
	      1, ch, 0, 0, TO_ROOM);
              cast_dispel_magic(GetMaxLevel(ch),ch,"",SPELL_TYPE_SPELL,vict,0);
              return(FALSE);
            }

            if(lspell > 40) {
               act("$n utters the words 'Here's a penny, go buy a brain, and give me the change'",
               1, ch, 0, 0, TO_ROOM);
               cast_feeblemind(GetMaxLevel(ch),ch,"",SPELL_TYPE_SPELL,vict,0);
               return(FALSE);
            }
	    /* well, spells failed or not needed, let's kick someone :) */

	    FighterMove(ch);
	    return(FALSE);
	  }
	}
      }
    }
  }
}


int winger(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  struct char_data *vict;

  if(cmd)
    return(FALSE);

  if (check_soundproof(ch)) return(FALSE);

  if (check_nomagic(ch, 0, 0))  return(FALSE);

  if (IS_AFFECTED(ch, AFF_PARALYSIS)) return(FALSE);

  if (ch->specials.fighting) {

    vict = FindVictim(ch);

    if(!vict)
      vict = ch->specials.fighting;

    if(!vict)
      return(FALSE);

    if(IS_AFFECTED(vict,AFF_FLYING)) {
      act("$n utters the words 'Instant Wing Remover!'.",1, ch, 0, 0, TO_ROOM);
      cast_dispel_magic(GetMaxLevel(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      return(TRUE);
    } else {
      /* As per request of Alfred, if we are fighting and they aren't  */
      /* affected by fly, then eek, we flee! */
      do_flee(ch,"",0);
      return(TRUE);
    }
  }
  return(FALSE);
}

int DeathRoom(int dt_room)
{
  struct obj_data *k;
  struct room_data *room;
  char buf[100];
  int whee, to_room;
  extern int top_of_world;

  PulseMobiles(EVENT_DEATHROOM);

  for(k = real_roomp(dt_room)->contents; k ; k = real_roomp(dt_room)->contents)
    {
      whee = number(1,9);  /* Yes, I don't trust randomness :) */
      if((whee == 1) || (whee == 4) || (whee == 7)) {
        obj_from_room(k);
        obj_to_room(k,99);
      }
      else if((whee == 2) || (whee == 5) || (whee == 8))
        extract_obj(k);
      else if((whee == 3) || (whee == 6) || (whee == 9)) {
        do {
	  to_room = number(0, top_of_world);
	  room = real_roomp(to_room);
	  if (room) {
	    if((IS_SET(room->room_flags, PRIVATE)) || 
	       (IS_SET(room->room_flags, TUNNEL)) ||
	       (IS_SET(room->room_flags, NO_SUM)) || 
	       (IS_SET(room->room_flags, NO_MAGIC)) || 
	       !IsOnPmp(to_room))
	      room = 0;
	  }
	} while (!room);
	
        send_to_all(buf);
        obj_from_room(k);
        obj_to_room(k,to_room);
      }
    }
  return(FALSE);
}

int YouthPotion( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  char obj_name[80], vict_name[80], buf[MAX_INPUT_LENGTH];
  /*   struct char_data *vict; */
  struct obj_data *obj;
  int agepoints, negativeage, modifiedage;
  bool equipped;

  if (cmd == 206) {          /* quaff */
    if (!AWAKE(ch))
      return(FALSE);
    
    one_argument(arg,buf);
    
    if (*buf) {
      if(!(str_cmp("potion",buf)) || !(str_cmp("youth",buf)) || 
	 !(str_cmp("shiny",buf)) || !(str_cmp("bright",buf)) || 
	 !(str_cmp("white",buf))) {

	/* First drink the sucker.. */

	equipped = FALSE;
	if (!(obj = get_obj_in_list_vis(ch,buf,ch->carrying))) {
	  obj = ch->equipment[HOLD];
	  equipped = TRUE;
	}
	if(equipped)
	  obj = unequip_char(ch, HOLD);
	extract_obj(obj);
	
	send_to_char("You chug down the potion...\n\r",ch);
	
	agepoints = number(1,6);
	
	if((GET_AGE(ch) - agepoints) < 1) {
	  negativeage = (GET_AGE(ch) - agepoints);
	  modifiedage = (1 - negativeage);
	  ch->player.time.birth += (agepoints*SECS_PER_MUD_YEAR);
	  ch->player.time.birth -= (modifiedage*SECS_PER_MUD_YEAR);
	}
	else {
	  ch->player.time.birth += (agepoints*SECS_PER_MUD_YEAR);
	}
	send_to_char("You feel young again!\n\r", ch);
	return(TRUE);
      }
      send_to_char("I think you spelled it wrong...\n\r", ch);
      return(TRUE);
    }
    send_to_char("Did you have something particular in mind?\n\r", ch);
    return(TRUE);
  }
  return(FALSE);
}

#define WARPSTONE 29

int warpstone( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{

  char buf[100];
  struct obj_data *temp;

  if(cmd != 12 || !AWAKE(ch))			/* eat */
    return(FALSE);
  
  if(GET_RACE(ch) != RACE_DRAAGDIM)
    return(FALSE);


  one_argument(arg,buf);

  if(!(temp = get_obj_in_list_vis(ch,buf,ch->carrying)))  {
    act("You can't find it!",FALSE,ch,0,0,TO_CHAR);
    return(TRUE);
  }

  if(temp->obj_flags.type_flag != ITEM_WARPSTONE) 
    return(FALSE);
      
  if(GET_COND(ch,FULL)>20) /* Stomach full */     {
    send_to_char("You are too full to eat any more!\n\r", ch);
    return(TRUE);
  }

  act("$n munches on $p.",TRUE,ch,temp,0,TO_ROOM);
  act("You eat the $o.",FALSE,ch,temp,0,TO_CHAR);
  
  gain_condition(ch,FULL,temp->obj_flags.value[0]);
  
  send_to_char("My, that was very invigorating.\n\r", ch);
  
  GET_MANA(ch) += number(30,60); /* 45 average */

  if(GET_COND(ch,FULL)>20)
    act("You are full.",FALSE,ch,0,0,TO_CHAR);
  
  extract_obj(temp);

  return(TRUE);
}

struct turbolift_keywords {
  int dest;
  char *trigger;
};

const struct turbolift_keywords TurboLiftList[5] = {
  { 3030, "dump"},
  { 3033, "zifnab"},
  { 2,    "hell"},
  { 2500, "draagdim"},
  { 0,    "\n"}
};

#define TURBO_LIFT 2639

int entering_turbo_lift(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type)
{
  int i;

  if( cmd != 1 && cmd != 2 && cmd != 3 && cmd != 4 && cmd != 5 && cmd != 6)
    return(FALSE);
  if(rp->dir_option[cmd-1] && rp->dir_option[cmd-1]->to_room == TURBO_LIFT) {
    char_from_room(ch);
    char_to_room(ch, TURBO_LIFT);
    
    rp = real_roomp(ch->in_room);
    send_to_char("The doors open as you approach.\n\r",ch);
    send_to_char("\n\rThey then close quietly behind you.\n\r", ch);
    do_look(ch,"",0);
    for(i=0;i<6;i++) {
      if (rp->dir_option[i]) {
	rp->dir_option[i] = 0;
	free(rp->dir_option[i]);
      }
    }
  return(TRUE);
  } else 
    return(FALSE);
}
  
int turbo_lift(struct char_data *ch, int cmd, char *arg, struct room_data *rp,
int type)
{

  int i, dest;
  char buf[80];

  if(cmd > 0 && cmd < 7) {	/* movement */
    if(rp->dir_option[cmd-1]) {
      char_from_room(ch);
      char_to_room(ch, rp->dir_option[cmd-1]->to_room);
      act("$n has entered the room.", TRUE, ch, 0, 0, TO_ROOM);
      do_look(ch,"",0);
      send_to_char("\n\rThe doors close quietly behind you.\n\r", ch);
    } else {
      send_to_char("\nYou walk right into the side of the wall.\n\r",ch);
      act("$n walks right into the wall.",TRUE,ch,0, 0, TO_ROOM);
    }
    return(TRUE);
  }


  if(cmd != 169 && cmd != 17 )
    return(FALSE);
  
  do_say(ch, arg, cmd);
  
  for(i=dest=0;TurboLiftList[i].trigger[0] != '\n'; i++)
    if(!str_cmp(arg,TurboLiftList[i].trigger)) {
      dest = TurboLiftList[i].dest;
      break;
    }
  for(i=0;i<6;i++) {
    if (rp->dir_option[i]) {	
      rp->dir_option[i] = 0;
      free(rp->dir_option[i]);
    }
  }

  if(!dest) 
    return;
  
  i=number(0,3);
  
  
  CREATE(rp->dir_option[i], struct room_direction_data, 1);
  
  rp->dir_option[i]->exit_info = 0;
  rp->dir_option[i]->key = 0;
  rp->dir_option[i]->to_room = dest;

  send_to_room("There is a small sensation of motion.\n\r",ch->in_room);
  send_to_room("The lights flicker for a moment.\n\r", ch->in_room);
  sprintf(buf,"\n\rThe doors open to the %s.\n\r",dirs[i]);
  send_to_room(buf,ch->in_room);

  return(TRUE);

}
 
 
/* Scrolls, can be anything really, \n's even */
char *scroll_text[] = {
  "They say a dandelion is a blessing indeed.\n",
  "For minor aches and pains hospitals use Mistletoe the most.\n",
  "They say a toad can see the evil in men's hearts.\n",
  "Said the Spider on her web, 'tis magic I feel about me.'\n",
  "With Eye of Toad and Mistletoe, the unseen shall be seen!\n",
  "Crimson and Clo*@#, Athlete's Foot and Mis*##@eo, plus a dandelion\n are said to cure one's serious wounds.\n",
  "Out of breath? Try melting some fool's gold, holy symbol and webs!\n",
  "9 out of 10 neurosurgeons use a poultice of\n(1) holy symbol\n(1) stick of holly\n(1) some sassafras\n",
  "The beast with the eyes of a toad, fangs of a snake, toe of a mistle, a\nsilver collar and an anchor is see to be able to see all!\nMy, they were really grasping here.\n",
  "'Up, Up, away.', said the ball to the web.",
  "A chip of stone, Collar of Silver, Some Gold's Fool, and a dandelion \nwith mistletoe are said to be the cure-all of everything.",
  "White stone in one hand, anchor in the other, holy symbol raised high,\nthe bark clad druid mumbled something about needing a silver collar.\n",
  "Bark makes a good shield.\n",
  "Hercules was known to drink a concoction of tops and oak leaves.\n",
  "Flotsam and wet wool are needed to turn lungs to gills.\n",
  "Snake fangs and oak leaves will help a fool find his gold.\n",
  "Snake fangs and oak leaves and some quartz will help gold find a fool.\n",
  "\0" };

#define MAX_SCROLL     18    /* Max of scrolls */
#define SCROLL_OBJ     87    /* Scroll to load */
 
int OldHag(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  /* Mob is always passed as the mobile, no matter what */
  /* So we'll intercept the first call here, no matter  */
  /* What it is, that way we can make sure something's  */
  /* Loaded, and set it's long description to a random  */
  /* Scroll type.      */
 
  int rand_num;
  struct obj_data *obj;
 
  rand_num = rand()%MAX_SCROLL;
  if(mob->generic == 1)
    return(FALSE);
  
  obj = read_object(SCROLL_OBJ, VIRTUAL);
  obj->obj_flags.type_flag = ITEM_NOTE;
  if(obj->action_description || *(obj->action_description))
    free(obj->action_description);
  
  obj->action_description = strdup(scroll_text[rand_num]);
  obj_to_char(obj, mob);
  mob->generic = 1;
  REMOVE_BIT(mob->specials.act, ACT_SPEC);
  return(FALSE);
}

