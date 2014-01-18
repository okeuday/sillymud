/* ************************************************************************
 *  file: act.informative.c , Implementation of commands.  Part of DIKUMUD *
 *  Usage : Informative commands.                                          *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 ************************************************************************* */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>

#include "protos.h"

/* extern variables */
#if HASH
extern struct hash_header room_db;
#else
extern struct room_data *room_db;
#endif
extern struct descriptor_data *descriptor_list;
extern struct char_data *character_list;
extern struct obj_data *object_list;

extern int  top_of_world;
extern int  top_of_zone_table;
extern int  top_of_mobt;
extern int  top_of_objt;
extern int  top_of_p_table;

extern char credits[MAX_STRING_LENGTH];
extern char news[MAX_STRING_LENGTH];
extern char info[MAX_STRING_LENGTH];
extern char wizlist[MAX_STRING_LENGTH];
extern char *dirs[]; 
extern char *where[];
extern char *color_liquid[];
extern char *fullness[];
extern const char *RaceName[];
extern const int RacialMax[][6];
extern char *spell_desc[];
extern char *spells[];
/* extern functions */

struct time_info_data age(struct char_data *ch);
void page_string(struct descriptor_data *d, char *str, int keep_internal);
int track( struct char_data *ch, struct char_data *vict);
int GetApprox(int num, int perc);
int SpyCheck(struct char_data *ch);
int remove_trap( struct char_data *ch, struct obj_data *trap);

struct obj_data *get_obj_in_list_vis(struct char_data *ch, char *name, 
				     struct obj_data *list);

void list_groups(struct char_data *ch);

/* intern functions */

void list_obj_to_char(struct obj_data *list,struct char_data *ch, int mode,
		      bool show);
char *DescDamage(float dam);
char *DescRatio(float f);  /* theirs / yours */
char *DamRollDesc(int a);
char *HitRollDesc(int a);
char *ArmorDesc(int a);
char *AlignDesc(int a);
char *DescAttacks(float a);
char *EgoDesc(int a);

int singular( struct obj_data *o)
{

  if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_HANDS) ||
      IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_FEET) ||
      IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_LEGS) ||
      IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_ARMS))
    return(FALSE);
  return(TRUE);
}

/* Procedures related to 'look' */

void argument_split_2(char *argument, char *first_arg, char *second_arg) {
  int look_at, begin;
  begin = 0;
  
  /* Find first non blank */
  for ( ;*(argument + begin ) == ' ' ; begin++);
  
  /* Find length of first word */
  for (look_at=0; *(argument+begin+look_at) > ' ' ; look_at++)
    
    /* Make all letters lower case, AND copy them to first_arg */
    *(first_arg + look_at) = LOWER(*(argument + begin + look_at));
  *(first_arg + look_at) = '\0';
  begin += look_at;
  
  /* Find first non blank */
  for ( ;*(argument + begin ) == ' ' ; begin++);
  
  /* Find length of second word */
  for ( look_at=0; *(argument+begin+look_at)> ' ' ; look_at++)
    
    /* Make all letters lower case, AND copy them to second_arg */
    *(second_arg + look_at) = LOWER(*(argument + begin + look_at));
  *(second_arg + look_at)='\0';
  begin += look_at;
}

struct obj_data *get_object_in_equip_vis(struct char_data *ch,
       		 char *arg, struct obj_data *equipment[], int *j) {
  
  for ((*j) = 0; (*j) < MAX_WEAR ; (*j)++)
    if (equipment[(*j)])
      if (CAN_SEE_OBJ(ch,equipment[(*j)])) {
	if (isname(arg, equipment[(*j)]->name)) {
	  return(equipment[(*j)]);
	}
      } else if( IS_AFFECTED2(ch, AFF2_SUN_BLIND)) { /* sun blind can see */
	if (isname(arg, equipment[(*j)]->name)) {    /* their own stuff   */
          return(equipment[(*j)]);
	}
      }

  return (0);
}

char *find_ex_description(char *word, struct extra_descr_data *list)
{
  struct extra_descr_data *i;
  
  for (i = list; i; i = i->next)
    if (isname(word,i->keyword))
      return(i->description);
  
  return(0);
}


void show_obj_to_char(struct obj_data *object, struct char_data *ch, int mode)
{
  char buffer[MAX_STRING_LENGTH];
  
  buffer[0] = 0;
  if ((mode == 0) && object->description)
    strcpy(buffer, object->description);	    
  else if (object->short_description && ((mode == 1) ||
					 (mode == 2) || (mode==3) || (mode == 4))) 
    strcpy(buffer,object->short_description);
  else if (mode == 5) {
    if (object->obj_flags.type_flag == ITEM_NOTE)  	{
      if (object->action_description)	 {
	strcpy(buffer, "There is something written upon it:\n\r\n\r");
	strcat(buffer, object->action_description);
	page_string(ch->desc, buffer, 1);
      }  else {
	act("It's blank.", FALSE, ch,0,0,TO_CHAR);
      }
    } else if((object->obj_flags.type_flag != ITEM_DRINKCON)) {
      strcpy(buffer,"You see nothing special..");
    }  else  { /* ITEM_TYPE == ITEM_DRINKCON */
      strcpy(buffer, "It looks like a drink container.");
    }
  }
  
  if (mode != 3) { 
    if (IS_OBJ_STAT(object,ITEM_INVISIBLE)) {
      strcat(buffer,"(invisible)");
    }
    if (IS_OBJ_STAT(object,ITEM_ANTI_GOOD) && 
	IS_AFFECTED(ch,AFF_DETECT_EVIL)) {
      if (singular(object))
	strcat(buffer,"..It glows red");
      else 
	strcat(buffer,"..They glow red");
    }
    if (IS_OBJ_STAT(object,ITEM_MAGIC) && IS_AFFECTED(ch,AFF_DETECT_MAGIC)) {
      if (singular(object))
	strcat(buffer,"..It glows blue");
      else 
	strcat(buffer,"..They glow blue");
    }
    if (IS_OBJ_STAT(object,ITEM_GLOW)) {
      if (singular(object))
	strcat(buffer,"..It glows softly");
      else 
	strcat(buffer,"..They glow softly");
    }
    if (IS_OBJ_STAT(object,ITEM_HUM)) {
      if (singular(object))
      strcat(buffer,"..It hums powerfully");
      else 
	strcat(buffer,"..They hum with power");
    }
    if (object->obj_flags.type_flag == ITEM_ARMOR) {
      if (object->obj_flags.value[0] <
	  (object->obj_flags.value[1] / 4)) {
	if (singular(object))
	  strcat(buffer, "..It is falling apart");
	else 
	  strcat(buffer,"..They are falling apart");
      } else if (object->obj_flags.value[0] < 
		 (object->obj_flags.value[1] / 3)) {
	if (singular(object))
	  strcat(buffer, "..It is need of much repair.");
	else 
	  strcat(buffer,"..They are in need of much repair");
      } else if (object->obj_flags.value[0] < 
		 (object->obj_flags.value[1] / 2)) {
	if (singular(object))
	  strcat(buffer, "..It is in fair condition");
	else 
	  strcat(buffer,"..They are in fair condition");
      } else if  (object->obj_flags.value[0] < 
		  object->obj_flags.value[1]) {
	if (singular(object))
	  strcat(buffer, "..It is in good condition");
	else 
	  strcat(buffer,"..They are in good condition");
      } else {
	if (singular(object))
	  strcat(buffer, "..It is in excellent condition");
	else 
	  strcat(buffer,"..They are in excellent condition");
      }
    }
  }
  
  
  strcat(buffer, "\n\r");
  page_string(ch->desc, buffer, 1);
  
}

void show_mult_obj_to_char(struct obj_data *object, struct char_data *ch, int mode, int num)
{
  char buffer[MAX_STRING_LENGTH];
  char tmp[10];

  buffer[0] = 0;
  tmp[0] = 0;
  
  if ((mode == 0) && object->description)
    strcpy(buffer,object->description);
  else 	if (object->short_description && ((mode == 1) ||
					  (mode == 2) || (mode==3) || (mode == 4))) 
    strcpy(buffer,object->short_description);
  else if (mode == 5) {
    if (object->obj_flags.type_flag == ITEM_NOTE)  	{
      if (object->action_description)	 {
	strcpy(buffer, "There is something written upon it:\n\r\n\r");
	strcat(buffer, object->action_description);
	page_string(ch->desc, buffer, 1);
      }  else
	act("It's blank.", FALSE, ch,0,0,TO_CHAR);
      return;
    } else if((object->obj_flags.type_flag != ITEM_DRINKCON)) {
      strcpy(buffer,"You see nothing special..");
    }  else  { /* ITEM_TYPE == ITEM_DRINKCON */
      strcpy(buffer, "It looks like a drink container.");
    }
  }
  
  if (mode != 3) { 
    if (IS_OBJ_STAT(object,ITEM_INVISIBLE)) {
      strcat(buffer,"(invisible)");
    }
    if (IS_OBJ_STAT(object,ITEM_ANTI_GOOD) && 
	IS_AFFECTED(ch,AFF_DETECT_EVIL)) {
      strcat(buffer,"..It glows red!");
    }
    if (IS_OBJ_STAT(object,ITEM_MAGIC) && IS_AFFECTED(ch,AFF_DETECT_MAGIC)) {
      strcat(buffer,"..It glows blue!");
    }
    if (IS_OBJ_STAT(object,ITEM_GLOW)) {
      strcat(buffer,"..It has a soft glowing aura!");
    }
    if (IS_OBJ_STAT(object,ITEM_HUM)) {
      strcat(buffer,"..It emits a faint humming sound!");
    }
  }
  
  if (num>1) {
    sprintf(tmp,"[%d]", num);
    strcat(buffer, tmp);
  }
  strcat(buffer, "\n\r");
  page_string(ch->desc, buffer, 1);
}

void list_obj_in_room(struct obj_data *list, struct char_data *ch)
{
  struct obj_data *i, *cond_ptr[50];
  int Inventory_Num = 1, num;
  int k, cond_top, cond_tot[50], found=FALSE;
  char buf[MAX_STRING_LENGTH];
  
  cond_top = 0; 
  
  for (i=list; i; i = i->next_content) {
    if (CAN_SEE_OBJ(ch, i)) {
      if (cond_top< 50) {
	found = FALSE;
	for (k=0;(k<cond_top&& !found);k++) {
	  if (cond_top>0) {
	    if ((i->item_number == cond_ptr[k]->item_number) &&
		(i->description && cond_ptr[k]->description &&
		 !strcmp(i->description,cond_ptr[k]->description))){
	      cond_tot[k] += 1;
	      found=TRUE;
	    }
	  }          
	}
	if (!found) {
	  cond_ptr[cond_top] = i;
	  cond_tot[cond_top] = 1;
	  cond_top+=1;
	}
      } else {
	if ((ITEM_TYPE(i) == ITEM_TRAP) || (GET_TRAP_CHARGES(i) > 0)) {
       	  num = number(1,100);
       	  if (ch->skills && num < (ch->skills[SKILL_LOCATE_TRAP].learned/2)) {
	    show_obj_to_char(i,ch,0);
	  }
        } else {
	  show_obj_to_char(i,ch,0);	  
	}
      }
    }
  }
  
  if (cond_top) {
    for (k=0; k<cond_top; k++) {
      if ((ITEM_TYPE(cond_ptr[k]) == ITEM_TRAP) && 
	  (GET_TRAP_CHARGES(cond_ptr[k]) > 0)) {
	num = number(1,101);
	if (ch->skills && (num < (ch->skills[SKILL_LOCATE_TRAP].learned/2)))
	  if (cond_tot[k] > 1) {
	    sprintf(buf,"[%2d] ",Inventory_Num++);
	    send_to_char(buf,ch);
	    show_mult_obj_to_char(cond_ptr[k],ch,0,cond_tot[k]);
	  } else {
	    show_obj_to_char(cond_ptr[k],ch,0);
	  }
      } else {
	if (cond_tot[k] > 1) {
	  sprintf(buf,"[%2d] ",Inventory_Num++);
	  send_to_char(buf,ch);
	  show_mult_obj_to_char(cond_ptr[k],ch,0,cond_tot[k]);
	} else {
	  show_obj_to_char(cond_ptr[k],ch,0);
	}
      }
    }
  }
}

void list_obj_in_heap(struct obj_data *list, struct char_data *ch)
{
  struct obj_data *i, *cond_ptr[255];
  int k, cond_top, cond_tot[255], found=FALSE;  
  char buf[MAX_STRING_LENGTH];
  
  int Num_Inventory = 1;
  cond_top = 0; 
  
  for (i=list; i; i = i->next_content) {
    if (CAN_SEE_OBJ(ch, i)) {
      if (cond_top< 50) {
	found = FALSE;
        for (k=0;(k<cond_top&& !found);k++) {
          if (cond_top>0) {
            if ((i->item_number == cond_ptr[k]->item_number) &&
		(i->short_description && cond_ptr[k]->short_description &&
		 (!strcmp(i->short_description,cond_ptr[k]->short_description)))){
	      cond_tot[k] += 1;
	      found=TRUE;
	    }
	  }        
	}
	if (!found) {
	  cond_ptr[cond_top] = i;
	  cond_tot[cond_top] = 1;
	  cond_top+=1;
	}
      } else {
	show_obj_to_char(i,ch,2);
      }
    }
  }
  
  if (cond_top) {
    for (k=0; k<cond_top; k++) {
      sprintf(buf,"[%2d] ",Num_Inventory++);
      send_to_char(buf,ch);
      if (cond_tot[k] > 1) {
	Num_Inventory += cond_tot[k] - 1;
	show_mult_obj_to_char(cond_ptr[k],ch,2,cond_tot[k]);
      } else {
	show_obj_to_char(cond_ptr[k],ch,2);
      }	
    }
  }
}

void list_obj_to_char(struct obj_data *list,struct char_data *ch, int mode, 
		      bool show) {
  char buf[MAX_STRING_LENGTH];
  int Num_In_Bag = 1;
  struct obj_data *i;
  bool found;
  
  found = FALSE;
  for ( i = list ; i ; i = i->next_content ) { 
    if (CAN_SEE_OBJ(ch,i)) {
      sprintf(buf,"[%2d] ",Num_In_Bag++);
      send_to_char(buf,ch);
      show_obj_to_char(i, ch, mode);
      found = TRUE;
    }    
  }  
  if ((! found) && (show)) send_to_char("Nothing\n\r", ch);
}



void show_char_to_char(struct char_data *i, struct char_data *ch, int mode)
{
  char buffer[MAX_STRING_LENGTH];
  int j, found, percent, otype;
  struct obj_data *tmp_obj;
  struct affected_type *aff;
  
  
  if (mode == 0) {
    
    if (IS_AFFECTED(i, AFF_HIDE) || !CAN_SEE(ch,i)) {
      if (IS_AFFECTED(ch, AFF_SENSE_LIFE))
	send_to_char("You sense a hidden life form in the room.\n\r", ch);
      return;
    }
    
    if (!(i->player.long_descr)||(GET_POS(i) != i->specials.default_pos)){
      /* A player char or a mobile without long descr, or not in default pos.*/
      if (!IS_NPC(i)) {	
	strcpy(buffer,GET_NAME(i));
	strcat(buffer," ");
	if (GET_TITLE(i))
	  strcat(buffer,GET_TITLE(i));
      } else {
	strcpy(buffer, i->player.short_descr);
	CAP(buffer);
      }
      
      if ( IS_AFFECTED(i,AFF_INVISIBLE) || i->invis_level == LOW_IMMORTAL)
	strcat(buffer," (invisible)");
      if ( IS_AFFECTED(i,AFF_CHARM))
	strcat(buffer," (pet)");
      
      switch(GET_POS(i)) {
      case POSITION_STUNNED  : 
	strcat(buffer," is lying here, stunned."); break;
      case POSITION_INCAP    : 
	strcat(buffer," is lying here, incapacitated."); break;
      case POSITION_MORTALLYW: 
	strcat(buffer," is lying here, mortally wounded."); break;
      case POSITION_DEAD     : 
	strcat(buffer," is lying here, dead."); break;
      case POSITION_MOUNTED:
	if (MOUNTED(i)) {
	  strcat(buffer, " is here, riding ");
	  strcat(buffer, MOUNTED(i)->player.short_descr);
	} else {
	  strcat(buffer, " is standing here.");
	}
	break;
      case POSITION_STANDING : 
	if (!IS_AFFECTED(i, AFF_FLYING)) {
	  if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	    strcat(buffer, "is floating here.");
	  else
	    strcat(buffer," is standing here."); 
	} else {
	  strcat(buffer," is flying about.");
	}
	break;
      case POSITION_SITTING  : 
	if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	  strcat(buffer, "is floating here.");
	else
	  strcat(buffer," is sitting here.");  break;
      case POSITION_RESTING  : 
	if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	  strcat(buffer, "is resting here in the water.");
	else
	strcat(buffer," is resting here.");  break;
      case POSITION_SLEEPING : 
	if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	  strcat(buffer, "is sleeping here in the water.");
	else
	strcat(buffer," is sleeping here."); break;
      case POSITION_FIGHTING :
	if (i->specials.fighting) {
	  
	  strcat(buffer," is here, fighting ");
	  if (i->specials.fighting == ch)
	    strcat(buffer," YOU!");
	  else {
	    if (i->in_room == i->specials.fighting->in_room)
	      if (IS_NPC(i->specials.fighting))
		strcat(buffer, i->specials.fighting->player.short_descr);
	      else
		strcat(buffer, GET_NAME(i->specials.fighting));
	    else
	      strcat(buffer, "someone who has already left.");
	  }
	} else /* NIL fighting pointer */
	  strcat(buffer," is here struggling with thin air.");
	break;
	default : strcat(buffer," is floating here."); break;
      }
      if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	if (IS_EVIL(i))
	  strcat(buffer, " (Red Aura)");
      }
      
      strcat(buffer,"\n\r");
      send_to_char(buffer, ch);
    } else {  /* npc with long */
      
      if (IS_AFFECTED(i,AFF_INVISIBLE))
	strcpy(buffer,"*");
      else
	*buffer = '\0';
      
      if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	if (IS_EVIL(i))
	  strcat(buffer, " (Red Aura)");
      }
      
      strcat(buffer, i->player.long_descr);
      
      send_to_char(buffer, ch);
    }
    
    if (IS_AFFECTED(i,AFF_SANCTUARY))
      act("$n glows with a bright light!", FALSE, i, 0, ch, TO_VICT);
    if (IS_AFFECTED(i,AFF_GROWTH))
      act("$n is extremely large!", FALSE, i, 0, ch, TO_VICT);
    if (IS_AFFECTED(i, AFF_FIRESHIELD)) 
      act("$n is surrounded by burning flames!", FALSE, i, 0, ch, TO_VICT);
    
  } else if (mode == 1) {
    
    if (i->player.description)
      send_to_char(i->player.description, ch);
    else {
      act("You see nothing special about $m.", FALSE, i, 0, ch, TO_VICT);
    }

/*
  personal descriptions.
*/

    if (IS_PC(i)) {
	sprintf(buffer, "$n is %s", RaceName[GET_RACE(i)]);
	act(buffer, FALSE, i, 0, ch, TO_VICT);	
    }

    if (MOUNTED(i)) {
      sprintf(buffer,"$n is mounted on %s", MOUNTED(i)->player.short_descr);
      act(buffer, FALSE, i, 0, ch, TO_VICT);
    }
    
    if (RIDDEN(i)) {
      sprintf(buffer,"$n is ridden by %s", IS_NPC(RIDDEN(i))?RIDDEN(i)->player.short_descr:GET_NAME(RIDDEN(i)));
      act(buffer, FALSE, i, 0, ch, TO_VICT);
    }

    /* Show a character to another */
    
    if (GET_MAX_HIT(i) > 0)
      percent = (100*GET_HIT(i))/GET_MAX_HIT(i);
    else
      percent = -1; /* How could MAX_HIT be < 1?? */
    
    if (IS_NPC(i))
      strcpy(buffer, i->player.short_descr);
    else
      strcpy(buffer, GET_NAME(i));
    
    if (percent >= 100)
      strcat(buffer, " is in an excellent condition.\n\r");
    else if (percent >= 90)
      strcat(buffer, " has a few scratches.\n\r");
    else if (percent >= 75)
      strcat(buffer, " has some small wounds and many bruises.\n\r");
    else if (percent >= 50)
      strcat(buffer, " is wounded, and bleeding.\n\r");
    else if (percent >= 30)
      strcat(buffer, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
      strcat(buffer, " is badly wounded\n\r");
    else if (percent >= 0)
      strcat(buffer, " is in an awful condition.\n\r");
    else
      strcat(buffer, " is bleeding badly from large, gaping wounds.\n\r");
    
    send_to_char(buffer, ch);


/*
  spell_descriptions, etc.
*/
    for (aff = i->affected; aff; aff = aff->next) {
      if (aff->type < 170) {
	otype = -1;
	if (spell_desc[aff->type] && *spell_desc[aff->type] && 
	    aff->location != APPLY_INTRINSIC)
	  if (aff->type != otype) {
	    act(spell_desc[aff->type], FALSE, i, 0, ch, TO_VICT);
	    otype = aff->type;
	  }
      }
    }

    
    found = FALSE;
    for (j=0; j< MAX_WEAR; j++) {
      if (i->equipment[j]) {
	if (CAN_SEE_OBJ(ch,i->equipment[j])) {
	  found = TRUE;
	}
      }
    }
    if (found) {
      act("\n\r$n is using:", FALSE, i, 0, ch, TO_VICT);
      for (j=0; j< MAX_WEAR; j++) {
	if (i->equipment[j]) {
	  if (CAN_SEE_OBJ(ch,i->equipment[j])) {
	    send_to_char(where[j],ch);
	    show_obj_to_char(i->equipment[j],ch,1);
	  }
	}
      }
    }
    if (HasClass(ch, CLASS_THIEF) && (ch != i) &&
	(!IS_IMMORTAL(ch))){
      found = FALSE;
      send_to_char
	("\n\rYou attempt to peek at the inventory:\n\r", ch);
      for(tmp_obj = i->carrying; tmp_obj; 
	  tmp_obj = tmp_obj->next_content) {
	if (CAN_SEE_OBJ(ch, tmp_obj) && 
	    (number(0,MAX_MORT) < GetMaxLevel(ch))) {
	  show_obj_to_char(tmp_obj, ch, 1);
	  found = TRUE;
	}
      }
      if (!found)
	send_to_char("You can't see anything.\n\r", ch);
    } else if (IS_IMMORTAL(ch)) {
      send_to_char("Inventory:\n\r",ch);
      for(tmp_obj = i->carrying; tmp_obj; 
	  tmp_obj = tmp_obj->next_content) {
	show_obj_to_char(tmp_obj, ch, 1);
	found = TRUE;
      }
      if (!found) {
	send_to_char("Nothing\n\r",ch);
      }
    }
    
  } else if (mode == 2) {
    
    /* Lists inventory */
    act("$n is carrying:", FALSE, i, 0, ch, TO_VICT);
    list_obj_in_heap(i->carrying,ch);
  }
}


void show_mult_char_to_char(struct char_data *i, struct char_data *ch, int mode, int num)
{
  char buffer[MAX_STRING_LENGTH];
  char tmp[10];
  int j, found, percent;
  struct obj_data *tmp_obj;
  
  if (mode == 0) {
    if (IS_AFFECTED(i, AFF_HIDE) || !CAN_SEE(ch,i)) {
      if (IS_AFFECTED(ch, AFF_SENSE_LIFE))
	if (num==1)
	  send_to_char("You sense a hidden life form in the room.\n\r", ch);
	else 
	  send_to_char("You sense hidden life forma in the room.\n\r", ch);		
      return;
    }
    
    if (!(i->player.long_descr)||(GET_POS(i) != i->specials.default_pos)){
      /* A player char or a mobile without long descr, or not in default pos. */
      if (!IS_NPC(i)) {	
	strcpy(buffer,GET_NAME(i));
	strcat(buffer," ");
	if (GET_TITLE(i))
	  strcat(buffer,GET_TITLE(i));
      } else {
	strcpy(buffer, i->player.short_descr);
	CAP(buffer);
      }
      
      if ( IS_AFFECTED(i,AFF_INVISIBLE))
	strcat(buffer," (invisible)");
      if ( IS_AFFECTED(i,AFF_CHARM))
	strcat(buffer," (pet)");
      
      switch(GET_POS(i)) {
      case POSITION_STUNNED  : 
	strcat(buffer," is lying here, stunned."); break;
      case POSITION_INCAP    : 
	strcat(buffer," is lying here, incapacitated."); break;
      case POSITION_MORTALLYW: 
	strcat(buffer," is lying here, mortally wounded."); break;
      case POSITION_DEAD     : 
	strcat(buffer," is lying here, dead."); break;
      case POSITION_STANDING : 
	if (!IS_AFFECTED(i, AFF_FLYING)) {
	  if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	    strcat(buffer, "is floating here.");
	  else
	    strcat(buffer," is standing here."); 
	} else {
	  strcat(buffer," is flying about.");
	}
	break;
      case POSITION_SITTING  : 
	if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	  strcat(buffer, "is floating here.");
	else
	  strcat(buffer," is sitting here.");  break;
      case POSITION_RESTING  : 
	if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	  strcat(buffer, "is resting here in the water");
	else
	  strcat(buffer," is resting here.");  break;
      case POSITION_SLEEPING : 
	if (real_roomp(i->in_room)->sector_type == SECT_WATER_NOSWIM)
	  strcat(buffer, "is sleeping here in the water");
	else
	  strcat(buffer," is sleeping here."); break;
      case POSITION_FIGHTING :
	if (i->specials.fighting) {
	  
	  strcat(buffer," is here, fighting ");
	  if (i->specials.fighting == ch)
	    strcat(buffer," YOU!");
	  else {
	    if (i->in_room == i->specials.fighting->in_room)
	      if (IS_NPC(i->specials.fighting))
		strcat(buffer, i->specials.fighting->player.short_descr);
	      else
		strcat(buffer, GET_NAME(i->specials.fighting));
	    else
	      strcat(buffer, "someone who has already left.");
	  }
	} else /* NIL fighting pointer */
	  strcat(buffer," is here struggling with thin air.");
	break;
	default : strcat(buffer," is floating here."); break;
      }
      if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	if (IS_EVIL(i))
	  strcat(buffer, " (Red Aura)");
      }
      
      if (num > 1) {
	sprintf(tmp," [%d]", num);
	strcat(buffer, tmp);
      }
      strcat(buffer,"\n\r");
      send_to_char(buffer, ch);
    } else {  /* npc with long */
      
      if (IS_AFFECTED(i,AFF_INVISIBLE))
	strcpy(buffer,"*");
      else
	*buffer = '\0';
      
      if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	if (IS_EVIL(i))
	  strcat(buffer, " (Red Aura)");
      }
      
      strcat(buffer, i->player.long_descr);
      
      /* this gets a little annoying */
      
      if (num > 1) {
	while ((buffer[strlen(buffer)-1]=='\r') ||
	       (buffer[strlen(buffer)-1]=='\n') ||
	       (buffer[strlen(buffer)-1]==' ')) {
	  buffer[strlen(buffer)-1] = '\0';
	}
	sprintf(tmp," [%d]\n\r", num);
	strcat(buffer, tmp);
      }
      
      send_to_char(buffer, ch);
    }
    
    if (IS_AFFECTED(i,AFF_SANCTUARY))
      act("$n glows with a bright light!", FALSE, i, 0, ch, TO_VICT);
    if (IS_AFFECTED(i,AFF_GROWTH))
      act("$n is twice $s normal size!", FALSE, i, 0, ch, TO_VICT);
    if (IS_AFFECTED(i, AFF_FIRESHIELD)) 
      act("$n is surrounded by burning flames", FALSE, i, 0, ch, TO_VICT);
    
  } else if (mode == 1) {
    
    if (i->player.description)
      send_to_char(i->player.description, ch);
    else {
      act("You see nothing special about $m.", FALSE, i, 0, ch, TO_VICT);
    }
    
    /* Show a character to another */
    
    if (GET_MAX_HIT(i) > 0)
      percent = (100*GET_HIT(i))/GET_MAX_HIT(i);
    else
      percent = -1; /* How could MAX_HIT be < 1?? */
    
    if (IS_NPC(i))
      strcpy(buffer, i->player.short_descr);
    else
      strcpy(buffer, GET_NAME(i));
    
    if (percent >= 100)
      strcat(buffer, " is in an excellent condition.\n\r");
    else if (percent >= 90)
      strcat(buffer, " has a few scratches.\n\r");
    else if (percent >= 75)
      strcat(buffer, " has some small wounds and bruises.\n\r");
    else if (percent >= 50)
      strcat(buffer, " has quite a few wounds.\n\r");
    else if (percent >= 30)
      strcat(buffer, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
      strcat(buffer, " looks pretty hurt.\n\r");
    else if (percent >= 0)
      strcat(buffer, " is in an awful condition.\n\r");
    else
      strcat(buffer, " is bleeding awfully from big wounds.\n\r");
    
    send_to_char(buffer, ch);
    
    found = FALSE;
    for (j=0; j< MAX_WEAR; j++) {
      if (i->equipment[j]) {
	if (CAN_SEE_OBJ(ch,i->equipment[j])) {
	  found = TRUE;
	}
      }
    }
    if (found) {
      act("\n\r$n is using:", FALSE, i, 0, ch, TO_VICT);
      for (j=0; j< MAX_WEAR; j++) {
	if (i->equipment[j]) {
	  if (CAN_SEE_OBJ(ch,i->equipment[j])) {
	    send_to_char(where[j],ch);
	    show_obj_to_char(i->equipment[j],ch,1);
	  }
	}
      }
    }
    if ((HasClass(ch, CLASS_THIEF)) && (ch != i)) {
      found = FALSE;
      send_to_char("\n\rYou attempt to peek at the inventory:\n\r", ch);
      for(tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
	if (CAN_SEE_OBJ(ch,tmp_obj)&&(number(0,MAX_MORT) < GetMaxLevel(ch))) {
	  show_obj_to_char(tmp_obj, ch, 1);
	  found = TRUE;
	}
      }
      if (!found)
	send_to_char("You can't see anything.\n\r", ch);
    }
    
  } else if (mode == 2) {
    
    /* Lists inventory */
    act("$n is carrying:", FALSE, i, 0, ch, TO_VICT);
    list_obj_in_heap(i->carrying,ch);
  }
}


void list_char_in_room(struct char_data *list, struct char_data *ch)
{
  struct char_data *i, *cond_ptr[50];
  int k, cond_top, cond_tot[50], found=FALSE;  
  
  cond_top = 0; 
  
  for (i=list; i; i = i->next_in_room) {
    if ( (ch!=i) && (!RIDDEN(i)) && (IS_AFFECTED(ch, AFF_SENSE_LIFE) ||
		     (CAN_SEE(ch,i) && !IS_AFFECTED(i, AFF_HIDE))) ) {
      if ((cond_top< 50) && !MOUNTED(i)) {
	found = FALSE;
	if (IS_NPC(i)) {
	  for (k=0;(k<cond_top&& !found);k++) {
	    if (cond_top>0) {
	      if (i->nr == cond_ptr[k]->nr &&
		  (GET_POS(i) == GET_POS(cond_ptr[k])) &&
		  (i->specials.affected_by==cond_ptr[k]->specials.affected_by) &&  
		  (i->specials.fighting == cond_ptr[k]->specials.fighting) &&
		  (i->player.short_descr && cond_ptr[k]->player.short_descr &&
		   0==strcmp(i->player.short_descr,cond_ptr[k]->player.short_descr))) {
		cond_tot[k] += 1;
		found=TRUE;
	      }
	    }
	  }
	}
	if (!found) {
	  cond_ptr[cond_top] = i;
	  cond_tot[cond_top] = 1;
	  cond_top+=1;
	}
      } else {
	show_char_to_char(i,ch,0);
      }
    }
  }
  
  if (cond_top) {
    for (k=0; k<cond_top; k++) {
      if (cond_tot[k] > 1) {
	show_mult_char_to_char(cond_ptr[k],ch,0,cond_tot[k]);
      } else {
	show_char_to_char(cond_ptr[k],ch,0);
      }
    }
  }
}


void list_char_to_char(struct char_data *list, struct char_data *ch, 
		       int mode) {
  struct char_data *i;
  
  for (i = list; i ; i = i->next_in_room) {
    if ( (ch!=i) && (IS_AFFECTED(ch, AFF_SENSE_LIFE) ||
		     (CAN_SEE(ch,i) && !IS_AFFECTED(i, AFF_HIDE))) )
      show_char_to_char(i,ch,0); 
  } 
}



void do_look(struct char_data *ch, char *argument, int cmd)
{
  char buffer[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int keyword_no, res;
  int j, bits, temp;
  bool found;
  struct obj_data *tmp_object, *found_object;
  struct char_data *tmp_char;
  char *tmp_desc;
  static char *keywords[]= { 
    "north",
    "east",
    "south",
    "west",
    "up",
    "down",
    "in",
    "at",
    "",  /* Look at '' case */
    "room",
    "\n" };
  
  if (!ch->desc)
    return;
  
  if ( IS_AFFECTED(ch, AFF_BLIND) )
    send_to_char("You can't see a damn thing, you're blinded!\n\r", ch);
  else if (IS_DARK(ch->in_room) && !IS_IMMORTAL(ch) && !SUNPROBLEM(ch) && 
	   !IS_AFFECTED(ch, AFF_TRUE_SIGHT)) {
    if(!IS_AFFECTED(ch, AFF_TRUE_SIGHT)) {
      send_to_char("It is very dark in here...\n\r", ch);
    }
    if (IS_AFFECTED(ch, AFF_INFRAVISION)) {
      list_char_in_room(real_roomp(ch->in_room)->people, ch);
    }
  } else {

    only_argument(argument, arg1);

    if (0==strn_cmp(arg1,"at",2) && isspace(arg1[2])) {
      only_argument(argument+3, arg2);
      keyword_no = 7;
    } else if (0==strn_cmp(arg1,"in",2) && isspace(arg1[2])) {
      only_argument(argument+3, arg2);
      keyword_no = 6;
    } else {
      keyword_no = search_block(arg1, keywords, FALSE);
    }

    if ((keyword_no == -1) && *arg1) {
      keyword_no = 7;
      only_argument(argument, arg2);
    }
    

    found = FALSE;
    tmp_object = 0;
    tmp_char	 = 0;
    tmp_desc	 = 0;

    if(keyword_no != 8 && keyword_no != 9 && IS_AFFECTED2(ch, AFF2_SUN_BLIND)){
      send_to_char("The daylight is too bright, you can't see a damn thing!\n\r", ch);
    } else 
      switch(keyword_no) {
	/* look <dir> */
      case 0 :
      case 1 :
      case 2 : 
      case 3 : 
      case 4 :
      case 5 : 
	{
       struct room_direction_data	*exitp;
       exitp = EXIT(ch, keyword_no);
       if (exitp) {
	 if (exitp->general_description) {
	   send_to_char(exitp-> general_description, ch);
	 } else {
	   send_to_char("You see nothing special.\n\r", ch);
	 }
	 
	 if (IS_SET(exitp->exit_info, EX_CLOSED) && 
	     (exitp->keyword)) {
	   if ((strcmp(fname(exitp->keyword), "secret")) &&
	       (!IS_SET(exitp->exit_info, EX_SECRET))) {
	     sprintf(buffer, "The %s is closed.\n\r", 
		     fname(exitp->keyword));
	     send_to_char(buffer, ch);
	   } 
	 } else {
	   if (IS_SET(exitp->exit_info, EX_ISDOOR) &&
	       exitp->keyword) {
	     sprintf(buffer, "The %s is open.\n\r",
		     fname(exitp->keyword));
	     send_to_char(buffer, ch);
	   }
	 }
       } else {
	 send_to_char("You see nothing special.\n\r", ch);
       }
       if (exitp && exitp->to_room && (!IS_SET(exitp->exit_info, EX_ISDOOR) ||
				       (!IS_SET(exitp->exit_info, EX_CLOSED)))) {
	 if (IS_AFFECTED(ch, AFF_SCRYING) || IS_IMMORTAL(ch)) {
	   struct room_data	*rp;
	   sprintf(buffer,"You look %swards.\n\r", dirs[keyword_no]);
	   send_to_char(buffer, ch);
	   
	   sprintf(buffer,"$n looks %swards.", dirs[keyword_no]);
	   act(buffer, FALSE, ch, 0, 0, TO_ROOM);
	   
	   rp = real_roomp(exitp->to_room);
	   if (!rp) {
	     send_to_char("You see swirling chaos.\n\r", ch);
	   } else if(exitp) {
	     sprintf(buffer, "%d look", exitp->to_room);
	     do_at(ch, buffer, 0);
	   } else {
	     send_to_char("You see nothing special.\n\r", ch);
	   }
	 }
       }
     }
     break;
      
      /* look 'in'	*/
    case 6: {
      if (*arg2) {
	/* Item carried */
	bits = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM |
			    FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);
	
	if (bits) { /* Found something */
	  if (GET_ITEM_TYPE(tmp_object)== ITEM_DRINKCON) 	{
	    if (tmp_object->obj_flags.value[1] <= 0) {
	      act("It is empty.", FALSE, ch, 0, 0, TO_CHAR);
	    } else {
	      temp=((tmp_object->obj_flags.value[1]*3)/tmp_object->obj_flags.value[0]);
	      sprintf(buffer,"It's %sfull of a %s liquid.\n\r",
		      fullness[temp],color_liquid[tmp_object->obj_flags.value[2]]);
	      send_to_char(buffer, ch);
	    }
	  } else if (GET_ITEM_TYPE(tmp_object) == ITEM_CONTAINER) {
	    if (!IS_SET(tmp_object->obj_flags.value[1],CONT_CLOSED)) {
	      send_to_char(fname(tmp_object->name), ch);
	      switch (bits) {
	      case FIND_OBJ_INV :
		send_to_char(" (carried) : \n\r", ch);
		break;
	      case FIND_OBJ_ROOM :
		send_to_char(" (here) : \n\r", ch);
		break;
	      case FIND_OBJ_EQUIP :
		send_to_char(" (used) : \n\r", ch);
		break;
	      }
	      list_obj_in_heap(tmp_object->contains, ch);
	    } else
	      send_to_char("It is closed.\n\r", ch);
	  } else {
	    send_to_char("That is not a container.\n\r", ch);
	  }
	} else { /* wrong argument */
	  send_to_char("You do not see that item here.\n\r", ch);
	}
      } else { /* no argument */
	send_to_char("Look in what?!\n\r", ch);
      }
    }
      break;
      
      /* look 'at'	*/
    case 7 : {
      if (*arg2) {
	bits = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM |
			    FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &tmp_char, &found_object);
	if (tmp_char) {
	  show_char_to_char(tmp_char, ch, 1);
	  if (ch != tmp_char) {
	    act("$n looks at you.", TRUE, ch, 0, tmp_char, TO_VICT);
	    act("$n looks at $N.", TRUE, ch, 0, tmp_char, TO_NOTVICT);
	  }
	  return;
	}
	/* 
	  Search for Extra Descriptions in room and items 
	  */
	
	/* Extra description in room?? */
	if (!found) {
	  tmp_desc = find_ex_description(arg2, 
					 real_roomp(ch->in_room)->ex_description);
	  if (tmp_desc) {
	    page_string(ch->desc, tmp_desc, 0);
	    return; 
	  }
	}
	
	/* extra descriptions in items */
	
	/* Equipment Used */
	if (!found) {
	  for (j = 0; j< MAX_WEAR && !found; j++) {
	    if (ch->equipment[j]) {
	      if (CAN_SEE_OBJ(ch,ch->equipment[j])) {
		tmp_desc = find_ex_description(arg2, 
					       ch->equipment[j]->ex_description);
		if (tmp_desc) {
		  page_string(ch->desc, tmp_desc, 1);
		  found = TRUE;
		}
	      }
	    }
	  }
	}
	/* In inventory */
	if (!found) {
	  for(tmp_object = ch->carrying; 
	      tmp_object && !found; 
	      tmp_object = tmp_object->next_content) {
	    if (CAN_SEE_OBJ(ch, tmp_object)) {
	      tmp_desc = find_ex_description(arg2, 
					     tmp_object->ex_description);
	      if (tmp_desc) {
		page_string(ch->desc, tmp_desc, 1);
		found = TRUE;
	      }
	    }
	  }
	}
	/* Object In room */
	
	if (!found) {
	  for(tmp_object = real_roomp(ch->in_room)->contents; 
	      tmp_object && !found; 
	      tmp_object = tmp_object->next_content) {
	    if (CAN_SEE_OBJ(ch, tmp_object)) {
	      tmp_desc = find_ex_description(arg2, 
					     tmp_object->ex_description);
	      if (tmp_desc) {
		page_string(ch->desc, tmp_desc, 1);
		found = TRUE;
	      }
	    }
	  }
	}
	/* wrong argument */
	if (bits) { /* If an object was found */
	  if (!found)
	    show_obj_to_char(found_object, ch, 5); 
	  /* Show no-description */
	  else
	    show_obj_to_char(found_object, ch, 6); 
	  /* Find hum, glow etc */
	} else if (!found) {
	  send_to_char("You do not see that here.\n\r", ch);
	}
      } else {
	/* no argument */	
	send_to_char("Look at what?\n\r", ch);
      }
    }
      break;
      
      /* look ''		*/ 
    case 8 : 
      if ( IS_AFFECTED2(ch, AFF2_SUN_BLIND)) {
	send_to_char(real_roomp(ch->in_room)->name, ch);
	send_to_char("\n\r", ch);
	send_to_char("Your eyes can't see much in this light!\n\r",
		     ch);
      } else {

	send_to_char(real_roomp(ch->in_room)->name, ch);
	send_to_char("\n\r", ch);
	if (!IS_SET(ch->specials.act, PLR_BRIEF))
	  send_to_char(real_roomp(ch->in_room)->description, ch);
	
	if (IS_SET(ch->specials.act, PLR_SHOWEXITS))
	  show_exits(ch);
	
	
	if (!IS_NPC(ch)) {
	  if (IS_SET(ch->specials.act, PLR_HUNTING)) {
	    if (ch->specials.hunting) {
	      res = track(ch, ch->specials.hunting);
	      if (!res) {
		ch->specials.hunting = 0;
		ch->hunt_dist = 0;
		REMOVE_BIT(ch->specials.act, PLR_HUNTING);
	      }
	    } else {
	      ch->hunt_dist = 0;
	      REMOVE_BIT(ch->specials.act, PLR_HUNTING);
	    }
	  }
	} else {
	  if (IS_SET(ch->specials.act, ACT_HUNTING) || IS_SET(ch->specials.act,
							      PLR_HUNTING)) {
	    if (ch->specials.hunting) {
	      res = track(ch, ch->specials.hunting);
	      if (!res) {
		ch->specials.hunting = 0;
		ch->hunt_dist = 0;
		REMOVE_BIT(ch->specials.act, ACT_HUNTING);
		REMOVE_BIT(ch->specials.act, PLR_HUNTING);
	      }  
	    } else {
	      ch->hunt_dist = 0;
	      REMOVE_BIT(ch->specials.act, ACT_HUNTING);
	      REMOVE_BIT(ch->specials.act, PLR_HUNTING);
	    }
	  }
	}
	
	list_obj_in_room(real_roomp(ch->in_room)->contents, ch);
	list_char_in_room(real_roomp(ch->in_room)->people, ch);
	KillTheOrcs(ch);
      }
      break;
      
      /* wrong arg	*/
    case -1 : 
      send_to_char("Sorry, I didn't understand that!\n\r", ch);
      break;
      
      /* look 'room' */
    case 9 : 
      if ( IS_AFFECTED2(ch, AFF2_SUN_BLIND)) {
        send_to_char(real_roomp(ch->in_room)->name, ch);
        send_to_char("\n\r", ch);
        send_to_char("Your eyes can't see much in this light!\n\r",
                     ch);
      } else {
      
	send_to_char(real_roomp(ch->in_room)->name, ch);
	send_to_char("\n\r", ch);
	send_to_char(real_roomp(ch->in_room)->description, ch);
	
	if (IS_SET(ch->specials.act, PLR_SHOWEXITS))
	  show_exits(ch);
	
	if (!IS_NPC(ch)) {
	  if (IS_SET(ch->specials.act, PLR_HUNTING)) {
	    if (ch->specials.hunting) {
	      res = track(ch, ch->specials.hunting);
	      if (!res) {
		ch->specials.hunting = 0;
		ch->hunt_dist = 0;
		REMOVE_BIT(ch->specials.act, PLR_HUNTING);
	      }
	    } else {
	      ch->hunt_dist = 0;
	      REMOVE_BIT(ch->specials.act, PLR_HUNTING);
	    }
	  }
	} else {
	  if (IS_SET(ch->specials.act, ACT_HUNTING)) {
	    if (ch->specials.hunting) {
	      res = track(ch, ch->specials.hunting);
	      if (!res) {
		ch->specials.hunting = 0;
		ch->hunt_dist = 0;
		REMOVE_BIT(ch->specials.act, ACT_HUNTING);
	      }  
	    } else {
	      ch->hunt_dist = 0;
	      REMOVE_BIT(ch->specials.act, ACT_HUNTING);
	    }
	  }
	}
	
	list_obj_in_room(real_roomp(ch->in_room)->contents, ch);
	list_char_in_room(real_roomp(ch->in_room)->people, ch);
	
      }
      break;
    }
  }
}

/* end of look */




void do_read(struct char_data *ch, char *argument, int cmd)
{
  char buf[100];
  
  /* This is just for now - To be changed later.! */
  sprintf(buf,"at %s",argument);
  do_look(ch,buf,15);
}



void do_examine(struct char_data *ch, char *argument, int cmd)
{
  char name[100], buf[100];
  struct char_data *tmp_char;
  struct obj_data *tmp_object;
  
  sprintf(buf,"at %s",argument);
  do_look(ch,buf,15);
  
  one_argument(argument, name);
  
  if (!*name) {
    send_to_char("Examine what?\n\r", ch);
    return;
  }
  
  generic_find(name, FIND_OBJ_INV | FIND_OBJ_ROOM |
		      FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);
  
  if (tmp_object) {
    if ((GET_ITEM_TYPE(tmp_object)==ITEM_DRINKCON) ||
	(GET_ITEM_TYPE(tmp_object)==ITEM_CONTAINER)) {
      send_to_char("When you look inside, you see:\n\r", ch);
      sprintf(buf,"in %s",argument);
      do_look(ch,buf,15);
    }
  }
}

void show_exits(struct char_data *ch)
{
  int door;
  char buf[1000];
  struct room_direction_data	*exitdata;
  extern char *exits[];
  
  *buf = '\0';
  
  for (door = 0; door <= 5; door++) {
    exitdata = EXIT(ch,door);
    if (exitdata) {
      if (!real_roomp(exitdata->to_room)) {
      } else if (exitdata->to_room != NOWHERE &&
		 (!IS_SET(exitdata->exit_info, EX_CLOSED) ||
		  IS_IMMORTAL(ch))) {
	  sprintf(buf + strlen(buf), "%s ", exits[door]);
      }
    }
  }

  strcat(buf, "\n\r");
  send_to_char("exits: ", ch);
  
  if (*buf)
    send_to_char(buf, ch);
  else
    send_to_char("None.\n\r", ch);
}

#if 1

/*  Gecko's spiffy enhancement to do_exits() from act.info.c */

void do_exits(struct char_data *ch, char *argument, int cmd)
{
  /* NOTE: Input var 'cmd' is not used. */
  int door;
  char buf[1000];
  struct room_direction_data *exitdata;
  extern char *exits[];
  
  *buf = '\0';
  
  for (door = 0; door <= 5; door++) {
    exitdata = EXIT(ch,door);
    if (exitdata) {
      if (!real_roomp(exitdata->to_room)) {
	/* don't print unless immortal */
	if (IS_IMMORTAL(ch)) {
	  sprintf(buf + strlen(buf), "%s - swirling chaos of #%d\n\r",
	    	  exits[door], exitdata->to_room);
        }
      }
      else if (exitdata->to_room != NOWHERE) {
        if (IS_IMMORTAL(ch)){
          sprintf(buf + strlen(buf), "%s - %s", exits[door],
                  real_roomp(exitdata->to_room)->name);
          if(IS_SET(exitdata->exit_info, EX_CLOSED))
            strcat(buf, " (closed)");
          if(IS_DARK(exitdata->to_room))
            strcat(buf, " (dark)");
          sprintf(buf + strlen(buf), " #%d\n\r", exitdata->to_room);
        }
        else if (!IS_SET(exitdata->exit_info, EX_CLOSED)) {
          if (IS_DARK(exitdata->to_room) && !IS_AFFECTED(ch, AFF_TRUE_SIGHT)) {
	    if(IS_AFFECTED(ch, AFF_INFRAVISION) || IS_LIGHT(ch->in_room))
	      sprintf(buf + strlen(buf), "%s - Too dark to tell\n\r", 
		      exits[door]);
	  } else
            sprintf(buf + strlen(buf), "%s - %s\n\r", exits[door],
		    real_roomp(exitdata->to_room)->name);
        }
      }
    }
  }
  
  send_to_char("Obvious exits:\n\r", ch);
  
  if (*buf)
    send_to_char(buf, ch);
  else
    send_to_char("None.\n\r", ch);
}

#endif

void do_score(struct char_data *ch, char *argument, int cmd)
{
  struct time_info_data playing_time;
  static char buf[100], buf2[100];
  struct time_info_data my_age;
  extern const struct title_type titles[MAX_CLASS][ABS_MAX_LVL];
  extern char *RaceNames[];
  
  struct time_info_data real_time_passed(time_t t2, time_t t1);

  age2(ch, &my_age);
  sprintf(buf, "You are %d years old.", my_age.year);
  
  
  if ((my_age.month == 0) && (my_age.year == 0))
    strcat(buf," It's your birthday today.\n\r");
  else
    strcat(buf,"\n\r");
  send_to_char(buf, ch);

  sprintf(buf, "You belong to the %s race\n\r", RaceName[GET_RACE(ch)]);
  send_to_char(buf, ch);
  
  if (!IS_IMMORTAL(ch) && (!IS_NPC(ch))) {
    if (GET_COND(ch,DRUNK)>10)
      send_to_char("You are intoxicated.\n\r", ch);
    if (GET_COND(ch,FULL)<2)
      send_to_char("You are hungry...\n\r", ch);
    if (GET_COND(ch,THIRST)<2)
      send_to_char("You are thirsty...\n\r", ch);
  }
  
  sprintf(buf, 
	  "You have %d(%d) hit, %d(%d) mana and %d(%d) movement points.\n\r",
	  GET_HIT(ch),GET_MAX_HIT(ch),
	  GET_MANA(ch),GET_MAX_MANA(ch),
	  GET_MOVE(ch),GET_MAX_MOVE(ch));
  send_to_char(buf,ch);

  if(HasClass(ch, CLASS_DRUID)) 
    sprintf(buf, "Your alignment is: %s (%d).\n\r", 
	    AlignDesc(GET_ALIGNMENT(ch)), GET_ALIGNMENT(ch));
  else 
    sprintf(buf, "Your alignment is: %s.\n\r", 
	    AlignDesc(GET_ALIGNMENT(ch)));

  send_to_char(buf,ch);
  
  sprintf(buf, "Your ego is of %s proportions.\n\r", EgoDesc(GET_EGO(ch)));
  send_to_char(buf,ch);

  sprintf(buf,"You have scored %d exp, and have %d gold coins.\n\r",
	  GET_EXP(ch),GET_GOLD(ch));
  send_to_char(buf,ch);

  buf[0] = '\0';
  sprintf(buf, "Your levels:");
  if (HasClass(ch, CLASS_MAGIC_USER)) {
    sprintf(buf2, " M:%d", GET_LEVEL(ch, MAGE_LEVEL_IND));
    strcat(buf, buf2);
  }
  if (HasClass(ch, CLASS_CLERIC)) {
    sprintf(buf2, " C:%d", GET_LEVEL(ch, CLERIC_LEVEL_IND));
    strcat(buf, buf2);
  }
  if (HasClass(ch, CLASS_WARRIOR)) {
    sprintf(buf2, " W:%d", GET_LEVEL(ch, WARRIOR_LEVEL_IND));
    strcat(buf, buf2);
  }
  if (HasClass(ch, CLASS_THIEF)) {
    sprintf(buf2, " T:%d", GET_LEVEL(ch, THIEF_LEVEL_IND));
    strcat(buf, buf2);
  }
  if (HasClass(ch, CLASS_DRUID)) {
    sprintf(buf2, " D:%d", GET_LEVEL(ch, DRUID_LEVEL_IND));
    strcat(buf, buf2);
  }
  if (HasClass(ch, CLASS_MONK)) {
    sprintf(buf2, " K:%d", GET_LEVEL(ch, MONK_LEVEL_IND));
    strcat(buf, buf2);
  }

  strcat(buf, "\n\r");
  send_to_char(buf,ch);

  if (GET_TITLE(ch)) {
    sprintf(buf,"This ranks you as %s %s.\n\r", GET_NAME(ch), GET_TITLE(ch));
    send_to_char(buf,ch);
  }
  
  playing_time = real_time_passed((time(0)-ch->player.time.logon) +
				  ch->player.time.played, 0);
  sprintf(buf,"You have been playing for %d days and %d hours.\n\r",
	  playing_time.day,
	  playing_time.hours);		
  send_to_char(buf, ch);		
  
  switch(GET_POS(ch)) {
  case POSITION_DEAD : 
    send_to_char("You are DEAD!\n\r", ch); break;
  case POSITION_MORTALLYW :
    send_to_char("You are mortally wounded!, you should seek help!\n\r", ch); break;
  case POSITION_INCAP : 
    send_to_char("You are incapacitated, slowly fading away\n\r", ch); break;
  case POSITION_STUNNED : 
    send_to_char("You are stunned! You can't move\n\r", ch); break;
  case POSITION_SLEEPING : 
    send_to_char("You are sleeping.\n\r",ch); break;
  case POSITION_RESTING  : 
    send_to_char("You are resting.\n\r",ch); break;
  case POSITION_SITTING  : 
    send_to_char("You are sitting.\n\r",ch); break;
  case POSITION_FIGHTING :
    if (ch->specials.fighting)
      act("You are fighting $N.\n\r", FALSE, ch, 0,
	  ch->specials.fighting, TO_CHAR);
    else
      send_to_char("You are fighting thin air.\n\r", ch);
    break;
  case POSITION_STANDING : 
    send_to_char("You are standing.\n\r",ch); break;
  case POSITION_MOUNTED:
    if (MOUNTED(ch)) {
      send_to_char("You are riding on ",ch); 
      send_to_char(MOUNTED(ch)->player.short_descr, ch);
      send_to_char("\n\r", ch);
    } else {
      send_to_char("You are standing.\n\r",ch); break;
    }
    break;
    default :
      send_to_char("You are floating.\n\r",ch); break;
  }
  
}


void do_time(struct char_data *ch, char *argument, int cmd)
{
  char buf[100], *suf;
  int weekday, day;
  extern struct time_info_data time_info;
  extern const char *weekdays[];
  extern const char *month_name[];
  
  sprintf(buf, "It is %d o'clock %s, on ",
	  ((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
	  ((time_info.hours >= 12) ? "pm" : "am") );
  
  weekday = ((35*time_info.month)+time_info.day+1) % 7;/* 35 days in a month */
  
  strcat(buf,weekdays[weekday]);
  strcat(buf,"\n\r");
  send_to_char(buf,ch);
  
  day = time_info.day + 1;   /* day in [1..35] */
  
  if (day == 1)
    suf = "st";
  else if (day == 2)
    suf = "nd";
  else if (day == 3)
    suf = "rd";
  else if (day < 20)
    suf = "th";
  else if ((day % 10) == 1)
    suf = "st";
  else if ((day % 10) == 2)
    suf = "nd";
  else if ((day % 10) == 3)
    suf = "rd";
  else
    suf = "th";
  
  sprintf(buf, "The %d%s Day of the %s, Year %d.\n\r",
	  day,
	  suf,
	  month_name[time_info.month],
	  time_info.year);
  
  send_to_char(buf,ch);
}


void do_weather(struct char_data *ch, char *argument, int cmd)
{
  extern struct weather_data weather_info;
  static char buf[100];
  char static *sky_look[4]= {
    "cloudless",
    "cloudy",
    "rainy",
    "lit by flashes of lightning"};
  
  if (OUTSIDE(ch)) {
    sprintf(buf, 
	    "The sky is %s and %s.\n\r",
	    sky_look[weather_info.sky],
	    (weather_info.change >=0 ? "you feel a warm wind from south" :
	     "your foot tells you bad weather is due"));
    send_to_char(buf,ch);
  } else
    send_to_char("You have no feeling about the weather at all.\n\r", ch);
}


void do_help(struct char_data *ch, char *argument, int cmd)
{

  extern int top_of_helpt;
  extern struct help_index_element *help_index;
  extern FILE *help_fl;
  extern char help[MAX_STRING_LENGTH];
  
  int chk, bot, top, mid, minlen;
  char buf[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];
  
  
  if (!ch->desc)
    return;
  
  for(;isspace(*argument); argument++)  ;
  
  
  if (*argument)
    {
      if (!help_index)	{
	  send_to_char("No help available.\n\r", ch);
	  return;
	}
      bot = 0;
      top = top_of_helpt;
      
      for (;;)
	{
	  mid = (bot + top) / 2;
	  minlen = strlen(argument);
	  
	  if (!(chk = strn_cmp(argument, help_index[mid].keyword, minlen)))
	    {
	      fseek(help_fl, help_index[mid].pos, 0);
	      *buffer = '\0';
	      for (;;)	{
		  fgets(buf, 80, help_fl);
		  if (*buf == '#')
		    break;
		  if (strlen(buf)+strlen(buffer) > MAX_STRING_LENGTH-2)
		    break;
		  strcat(buffer, buf);
		  strcat(buffer, "\r");
		}
	      page_string(ch->desc, buffer, 1);
	      return;
	    }	  else if (bot >= top)	    {
	      send_to_char("There is no help on that word.\n\r", ch);
	      return;
	    }
	  else if (chk > 0)
	    bot = ++mid;
	  else
	    top = --mid;
	}
      return;
    }
  
  
  send_to_char(help, ch);
  
}


void do_wizhelp(struct char_data *ch, char *arg, int cmd)
{
 char buf[1000];
 int i, j = 1;
 NODE *n;
 extern struct radix_list radix_head[];
 
 if(IS_NPC(ch))
    return;
 
 sprintf(buf, "Wizard Commands Available To You:\n\r\n\r");
 
 for(i = 0; i < 26; i++) {
    n = radix_head[i].next;
    while(n) {
        if(n->min_level <= GetMaxLevel(ch) && n->min_level >= LOW_IMMORTAL) {
           sprintf((buf + strlen(buf)), "%-10s", n->name);
           if(!(j % 7))
              sprintf((buf + strlen(buf)), "\n\r");
           j++;
	 }
        n = n->next;
      }
  }
 
 strcat(buf, "\n\r");
 
 page_string(ch->desc, buf, 1);
}
 

void do_who(struct char_data *ch, char *argument, int cmd)
{
  struct descriptor_data *d;
  char buf[256];
  int count, gods=FALSE, group=FALSE;
  struct char_data	*person;

  /*  check for an arg */
  only_argument(argument, buf);  
  if (*buf) {
    if (!strncmp(buf, "group", 5))
      group = TRUE;
    else
      gods = TRUE;
  }  else {
    gods = FALSE;
  }

  if (group) {
    send_to_char("These are the groups:\n\r", ch);
    list_groups(ch);
    return;
  }

  if (!IS_IMMORTAL(ch) || cmd == 234) {

    send_to_char("Players\n\r-------\n\r", ch);
    count=0;
    for (d = descriptor_list; d; d = d->next) {
      if (!d->connected && CAN_SEE(ch, d->character) &&
	  ( real_roomp((person = (d->original ? d->original:d->character)
			)->in_room)) &&
	  ( real_roomp((person = (d->original ? d->original:d->character)
			)->in_room)->zone == real_roomp(ch->in_room)->zone || 
	   cmd!=234 ) && (!gods || IS_IMMORTAL(d->character))) {
	count++;
	sprintf(buf, "%s %s", 
		GET_NAME(person),
		(person->player.title?person->player.title:"(Null)"));
	
	if (cmd==234) { /* it's a whozone command */
	  if ((!IS_AFFECTED(person, AFF_HIDE)) || (IS_IMMORTAL(ch))) {
	    sprintf(buf,"%-25s - %s ", GET_NAME(person),
		    real_roomp(person->in_room)->name);
	    if (GetMaxLevel(ch) >= LOW_IMMORTAL)
	      sprintf(buf+strlen(buf),"[%d]", person->in_room);
	  }
	} else {
	  sprintf(buf, "%s %s", 
		  GET_NAME(person),
		  (person->player.title?person->player.title:"(Null)"));
	}
	strcat(buf, "\n\r");
	
	send_to_char(buf, ch);
      }
    }
    sprintf(buf, "\n\rTotal visible players: %d\n\r", count);
    send_to_char(buf, ch);
    
  } else {
    int listed = 0, count, lcount, l, skip = FALSE;
    char arg[256], tempbuf[256];

    send_to_char("Players [God Version -? for Help]\n\r",ch);
    send_to_char("--------\n\r", ch);
    count=0;
    lcount=0;
    if (strlen(argument) == 0) {
      for (person = character_list; person; person = person->next) {
	if ((!IS_NPC(person)) || (IS_SET(person->specials.act, ACT_POLYSELF))) {
	  count++;
	  if (person->desc == NULL) {
	    lcount++;
	  } else {
	    sprintf(buf, "%s %s\n\r", 
		    GET_NAME(person),
		    (person->player.title?person->player.title:"(Null)"));
	    send_to_char(buf,ch);
	  }
	}
      }
    } else {
      argument = one_argument(argument,arg);
      if (arg[0] == '-') {
	if (index(arg,'?') != NULL) {
	  send_to_char("[-]i=idle l=levels t=title h=hit/mana/move s=stats\n\r",ch);
	  send_to_char("[-]d=linkdead g=God o=Mort [1]Mage[2]Cleric[3]War[4]Thief[5]Druid[6]Monk\n\r", ch);
    send_to_char("--------\n\r", ch);  
	}
	for (person = character_list; person; person = person->next) {
	  if (!IS_NPC(person)) {
	    count++;
	    if (person->desc == NULL) lcount ++;
	    skip = FALSE;
	    if (index(arg,'g') != NULL) {
	      if (!IS_IMMORTAL(person)) skip = TRUE;
	    }
	    if (index(arg,'o') != NULL) {
	      if (IS_IMMORTAL(person)) skip = TRUE;
	    }
	    if (index(arg,'1') != NULL) {
	      if (!HasClass(person,CLASS_MAGIC_USER)) skip = TRUE;
	    }
	    if (index(arg,'2') != NULL) {
	      if (!HasClass(person,CLASS_CLERIC)) skip = TRUE;
	    }
	    if (index(arg,'3') != NULL) {
	      if (!HasClass(person,CLASS_WARRIOR)) skip = TRUE;
	    }
	    if (index(arg,'4') != NULL) {
	      if (!HasClass(person,CLASS_THIEF)) skip = TRUE;
	    }
	    if (index(arg,'5') != NULL) {
	      if (!HasClass(person,CLASS_DRUID)) skip = TRUE;
	    }
	    if (index(arg,'6') != NULL) {
	      if (!HasClass(person,CLASS_MONK)) skip = TRUE;
	    }
	    if (!skip) {
	      if (person->desc == NULL) {
		if (index(arg,'d') != NULL) {
		  sprintf(buf, "[%-12s] ", GET_NAME(person));
		  listed++;
		}
	      } else {
		if (IS_NPC(person) && 
		    IS_SET(person->specials.act, ACT_POLYSELF)) {
		  sprintf(buf, "(%-14s) ", GET_NAME(person));
		  listed++;
		} else {
		  sprintf(buf, "%-14s ", GET_NAME(person));
		  listed++;
		}
	      }
	      if ((person->desc != NULL) || (index(arg,'d') != NULL)) {
		for (l = 1; l <= strlen(arg) ; l++) {
		  switch (arg[l]) {
		  case 'i': {
		    sprintf(tempbuf,"Idle:[%-3d] ",person->specials.timer);
		    strcat(buf,tempbuf);
		    break;
		  }
		  case 'l': {
		    sprintf(tempbuf,"Level:[%-2d/%-2d/%-2d/%-2d/%-2d/%-2d] ",
			    person->player.level[0],person->player.level[1],
			    person->player.level[2],person->player.level[3],
			    person->player.level[4],person->player.level[5]);
		    strcat(buf,tempbuf);
		    break;
		  }
		  case 'h': {
		    sprintf(tempbuf,"Hit:[%-3d] Mana:[%-3d] Move:[%-3d] ",GET_HIT(person),GET_MANA(person),GET_MOVE(person));
		    strcat(buf,tempbuf);
		    break;
		  }		 
		  case 's': {
		    sprintf(tempbuf,"[S:%-2d I:%-2d W:%-2d C:%-2d D:%-2d] ",GET_STR(person),GET_INT(person),GET_WIS(person),GET_CON(person),GET_DEX(person));
		    strcat(buf,tempbuf);
		    break;
		  }		 
		  case 't': {
		    sprintf(tempbuf," %-16s ",(person->player.title?person->player.title:"(null)"));
		    strcat(buf,tempbuf);
		    break;
		  }		 
		  default: {
		    break;
		  }
		  }
		}
	      }
	      if ((person->desc != NULL) || (index(arg,'d') != NULL)) {
		strcat(buf,"\n\r");
		send_to_char(buf,ch);
	      }
	    }
	  }
	}
      } else {
	/* list gods */
	for (person = character_list; person; person = person->next) {
	  if (!IS_NPC(person)) {
	    count++;
	    if (IS_IMMORTAL(person)) {
	      if (person->desc == NULL) {
		lcount++;
	      } else {
		sprintf(buf, "%s %s\n\r", GET_NAME(person), (person->player.title?person->player.title:"(null)"));
		send_to_char(buf,ch);
	      }
	    }
	  }
	}
      }
    }
    if (listed == 0) {
      sprintf(buf, "\n\rTotal players / Link dead [%d/%d] (%2.0f%%)\n\r",
	      count,lcount,((float)lcount / (int)count) * 100);
      send_to_char(buf, ch);
    } else {
      sprintf(buf, "\n\rTotal players / Link dead [%d/%d] (%2.0f%%) Number Listed: %d\n\r",
	      count,lcount,((float)lcount / (int)count) * 100,listed);
      send_to_char(buf, ch);
    }
  }
}

void do_users(struct char_data *ch, char *argument, int cmd)
{
  char buf[MAX_STRING_LENGTH], line[200], buf2[255];
  extern const char *connected_types[];

  struct descriptor_data *d;
  
  strcpy(buf, "Connections:\n\r------------\n\r");
  
  for (d = descriptor_list; d; d = d->next){
      if (d->character && d->character->player.name){
	if(d->original)
	  sprintf(line, "%-16s: ", d->original->player.name);
	else
	  sprintf(line, "%-16s: ", d->character->player.name);
      } else
	sprintf(line, "UNDEFINED       : ");
      if (d->host && *d->host) {
	sprintf(buf2, "%-22s [%s]\n\r", connected_types[d->connected],d->host);
      } else if(d) {
	sprintf(buf2, "%-22s [%s]\n\r", connected_types[d->connected],"????");
      } else {
	sprintf(buf2, "%-22s [%s]\n\r", "ACK, FIDO!", "booga");
      }
      strcat(line, buf2);
      strcat(buf, line);
    }
  send_to_char(buf, ch);
}



void do_inventory(struct char_data *ch, char *argument, int cmd) {
  
  send_to_char("You are carrying:\n\r", ch);
  list_obj_in_heap(ch->carrying, ch);
}


void do_equipment(struct char_data *ch, char *argument, int cmd) {
  int j,Worn_Index;
  bool found;
  char String[256];
  
  send_to_char("You are using:\n\r", ch);
  found = FALSE;
  for (Worn_Index = j=0; j< MAX_WEAR; j++) {
    if (ch->equipment[j]) {
      Worn_Index++;
      sprintf(String,"[%d] %s",Worn_Index,where[j]);
      send_to_char(String,ch);
      if (CAN_SEE_OBJ(ch,ch->equipment[j])) {
	show_obj_to_char(ch->equipment[j],ch,1);
	found = TRUE;
      } else {
	send_to_char("Something.\n\r",ch);
	found = TRUE;
      }
    }
  }
  if(!found) {
    send_to_char(" Nothing.\n\r", ch);
  }
}


void do_credits(struct char_data *ch, char *argument, int cmd) {
  
  page_string(ch->desc, credits, 0);
}


void do_news(struct char_data *ch, char *argument, int cmd) {
  
  page_string(ch->desc, news, 0);
}


void do_info(struct char_data *ch, char *argument, int cmd) {
  
  page_string(ch->desc, info, 0);
}


void do_wizlist(struct char_data *ch, char *argument, int cmd) {
  
  page_string(ch->desc, wizlist, 0);
}

int which_number_mobile(struct char_data *ch, struct char_data *mob)
{
  struct char_data	*i;
  char	*name;
  int	number;
  
  name = fname(mob->player.name);
  for (i=character_list, number=0; i; i=i->next) {
    if (isname(name, i->player.name) && i->in_room != NOWHERE) {
      number++;
      if (i==mob)
	return number;
    }
  }
  return 0;
}

char *numbered_person(struct char_data *ch, struct char_data *person)
{
  static char buf[MAX_STRING_LENGTH];
  if (IS_NPC(person) && IS_IMMORTAL(ch)) {
    sprintf(buf, "%d.%s", which_number_mobile(ch, person),
	    fname(person->player.name));
  } else {
    strcpy(buf, PERS(person, ch));
  }
  return buf;
}

void do_where_person(struct char_data *ch, struct char_data *person, struct string_block *sb)
{
  char buf[MAX_STRING_LENGTH];
  
  sprintf(buf, "%-30s- %s ", PERS(person, ch),
	  (person->in_room > -1 ? real_roomp(person->in_room)->name : "Nowhere"));
  
  if (GetMaxLevel(ch) >= LOW_IMMORTAL)
    sprintf(buf+strlen(buf),"[%d]", person->in_room);
  
  strcpy(buf+strlen(buf), "\n\r");
  
  append_to_string_block(sb, buf);
}

void do_where_object(struct char_data *ch, struct obj_data *obj,
			    int recurse, struct string_block *sb)
{
  char buf[MAX_STRING_LENGTH];
  if (obj->in_room != NOWHERE) { /* object in a room */
    sprintf(buf, "%-30s- %s [%d]\n\r",
	    obj->short_description,
	    real_roomp(obj->in_room)->name,
	    obj->in_room);
  } else if (obj->carried_by != NULL) { /* object carried by monster */
    sprintf(buf, "%-30s- carried by %s\n\r",
	    obj->short_description,
	    numbered_person(ch, obj->carried_by));
  } else if (obj->equipped_by != NULL) { /* object equipped by monster */
    sprintf(buf, "%-30s- equipped by %s\n\r",
	    obj->short_description,
	    numbered_person(ch, obj->equipped_by));
  } else if (obj->in_obj) { /* object in object */
    sprintf(buf, "%-30s- in %s\n\r",
	    obj->short_description,
	    obj->in_obj->short_description);
  } else {
    sprintf(buf, "%-30s- god doesn't even know where...\n\r",
	    obj->short_description);
  }
  if (*buf)
    append_to_string_block(sb, buf);
  
  if (recurse) {
    if (obj->in_room != NOWHERE)
      return;
    else if (obj->carried_by != NULL)
      do_where_person(ch, obj->carried_by, sb);
    else if (obj->equipped_by != NULL)
      do_where_person(ch, obj->equipped_by, sb);
    else if (obj->in_obj != NULL)
      do_where_object(ch, obj->in_obj, TRUE, sb);
  }
}

void do_where(struct char_data *ch, char *argument, int cmd)
{
  char name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  char	*nameonly;
  register struct char_data *i;
  register struct obj_data *k;
  struct descriptor_data *d;
  int	number, count;
  struct string_block	sb;
  
  only_argument(argument, name);
  
  if (!*name) {
    if (GetMaxLevel(ch) < LOW_IMMORTAL)      	{
      send_to_char("What are you looking for?\n\r", ch);
      return;
    } else 	{
      init_string_block(&sb);
      append_to_string_block(&sb, "Players:\n\r--------\n\r");
      
      for (d = descriptor_list; d; d = d->next) {
	if (d->character && (d->connected == CON_PLYNG) && (d->character->in_room != NOWHERE)) {
	  if (d->original)   /* If switched */
	    sprintf(buf, "%-20s - %s [%d] In body of %s\n\r",
		    d->original->player.name,
		    real_roomp(d->character->in_room)->name,
		    d->character->in_room,
		    fname(d->character->player.name));
	  else
	    sprintf(buf, "%-20s - %s [%d]\n\r",
		    d->character->player.name,
		    real_roomp(d->character->in_room)->name,
		    d->character->in_room);
	  
	  append_to_string_block(&sb, buf);
	}
      }
      page_string_block(&sb,ch);
      destroy_string_block(&sb);
      return;
    }
  }
  
  if (isdigit(*name)) {
    nameonly = name;
    count = number = get_number(&nameonly);
  } else {
    count = number = 0;
  }
  
  *buf = '\0';
  
  init_string_block(&sb);
  
  for (i = character_list; i; i = i->next)
    if (isname(name, i->player.name) && CAN_SEE(ch, i) )   	{
      if ((i->in_room != NOWHERE) &&
	  ((GetMaxLevel(ch)>=LOW_IMMORTAL) || (real_roomp(i->in_room)->zone ==
					     real_roomp(ch->in_room)->zone))) {
	if (number==0 || (--count) == 0) {
	  if (number==0) {
	    sprintf(buf, "[%2d] ", ++count); /* I love short circuiting :) */
	    append_to_string_block(&sb, buf);
	  }
	  do_where_person(ch, i, &sb);
	  *buf = 1;
	  if (number!=0)
	    break;
	}
       	if (GetMaxLevel(ch) < LOW_IMMORTAL)
	  break;
      }
    }
  
  /*  count = number;*/
  
  if (GetMaxLevel(ch) >= LOW_IMMORTAL ) {
    for (k = object_list; k; k = k->next)
      if (isname(name, k->name) && CAN_SEE_OBJ(ch, k)) {
	if (number==0 || (--count)==0) {
	  if (number==0) {
	    sprintf(buf, "[%2d] ", ++count);
	    append_to_string_block(&sb, buf);
	  }
	  do_where_object(ch, k, number!=0, &sb);
	  *buf = 1;
	  if (number!=0)
	    break;
	}
      }
  }
  
  if (!*sb.data)
    send_to_char("Couldn't find any such thing.\n\r", ch);
  else
    page_string_block(&sb, ch);
  destroy_string_block(&sb);
}




void do_levels(struct char_data *ch, char *argument, int cmd)
{
  int i, RaceMax, class;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  
  extern const struct title_type titles[MAX_CLASS][ABS_MAX_LVL];
  
  if (IS_NPC(ch))	{
    send_to_char("You ain't nothin' but a hound-dog.\n\r", ch);
    return;
  }
  
  *buf = '\0';
/*
**  get the class
*/

  for (;isspace(*argument);argument++);

  if (!*argument) {
    char buf[100];
    int exp;
	
    if(GetMaxLevel(ch) >= LOW_IMMORTAL) {
      send_to_char("No ammount of experience will ever get you a level!\n\r",
                   ch);
      send_to_char("However, have you considered brown nosing?\n\r",ch);
      return;
    }
    
    sprintf(buf,"You have scored %d experience points.\n\r",GET_EXP(ch));
    send_to_char(buf,ch);
    if (HasClass(ch, CLASS_MAGIC_USER)) {
      exp = (titles[MAGE_LEVEL_IND][GET_LEVEL(ch,MAGE_LEVEL_IND) + 1].exp);
      sprintf(buf,
              "You need %d experience points to become a level %d mage.\n\r",
              exp - GET_EXP(ch),
	      GET_LEVEL(ch,MAGE_LEVEL_IND) +1);
      send_to_char(buf,ch);
    }
    
    if (HasClass(ch, CLASS_CLERIC)) {
      exp = (titles[CLERIC_LEVEL_IND][GET_LEVEL(ch,CLERIC_LEVEL_IND)+1].exp);
      sprintf(buf,
              "You need %d experience points to become a level %d cleric.\n\r",
	      exp - GET_EXP(ch),
	      GET_LEVEL(ch,CLERIC_LEVEL_IND) +1);
      send_to_char(buf,ch);
    }
    
    if (HasClass(ch, CLASS_WARRIOR)) {
      exp = titles[WARRIOR_LEVEL_IND][GET_LEVEL(ch,WARRIOR_LEVEL_IND)+1].exp;
      sprintf(buf,
	      "You need %d experience points to become a level %d warrior.\n\r",
	      exp - GET_EXP(ch),
	      GET_LEVEL(ch,WARRIOR_LEVEL_IND) +1);
      send_to_char(buf,ch);
    }
    if (HasClass(ch, CLASS_THIEF)) {
      exp = titles[THIEF_LEVEL_IND][GET_LEVEL(ch,THIEF_LEVEL_IND) + 1].exp;
      sprintf(buf,
              "You need %d experience points to become a level %d thief.\n\r",
              exp - GET_EXP(ch),
              GET_LEVEL(ch,THIEF_LEVEL_IND) +1);
      send_to_char(buf,ch);
    }
    if (HasClass(ch, CLASS_DRUID)) {
      exp = titles[DRUID_LEVEL_IND][GET_LEVEL(ch,DRUID_LEVEL_IND) + 1].exp;
      sprintf(buf,
              "You need %d experience points to become a level %d druid.\n\r",
	      exp - GET_EXP(ch),
              GET_LEVEL(ch,DRUID_LEVEL_IND) +1);
      send_to_char(buf,ch);
    }
    if (HasClass(ch, CLASS_MONK)) {
      exp = titles[MONK_LEVEL_IND][GET_LEVEL(ch,MONK_LEVEL_IND) + 1].exp;
      sprintf(buf,
              "You need %d experience points to become a level %d monk.\n\r",
              exp - GET_EXP(ch),
              GET_LEVEL(ch,MONK_LEVEL_IND) +1);
      send_to_char(buf,ch);
    }
    return;
  }
  
  switch(*argument) {
  case 'C':
  case 'c':
  case 'P':
  case 'p':
    class = CLERIC_LEVEL_IND;
    break;
  case 'F':
  case 'f':
  case 'W':
  case 'w':
    class = WARRIOR_LEVEL_IND;
    break;
  case 'M':
  case 'm':
    class = MAGE_LEVEL_IND;
    break;
  case 'T':
  case 't':
    class = THIEF_LEVEL_IND;
    break;
  case 'D':
  case 'd':
    class = DRUID_LEVEL_IND;
    break;
  case 'K':
  case 'k':
    class = MONK_LEVEL_IND;
    break;

  default:
    sprintf(buf, "I don't recognize %s.\n\r", argument);
    send_to_char(buf,ch);
    return;
    break;
  }

  RaceMax = RacialMax[GET_RACE(ch)][class];
  
  for (i = 1; i <= RaceMax; i++) {
    
    sprintf(buf, "[%2d] %9d-%-9d : %s\n\r", i,
	    titles[class][i].exp,
	    titles[class][i + 1].exp, (GET_SEX(ch)==SEX_FEMALE?titles[class][i].title_f:titles[class][i].title_m));
    
    send_to_char(buf, ch);
  }
}



void do_consider(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char name[256], buf[256];
  int diff;
  
  only_argument(argument, name);
  
  if (!(victim = get_char_room_vis(ch, name))) {
    send_to_char("Consider killing who?\n\r", ch);
    return;
  }
  
  if (victim == ch) {
    send_to_char("Easy! Very easy indeed!\n\r", ch);
    return;
  }
  
  if (!IS_NPC(victim)) {
    send_to_char("Would you like to borrow a cross and a shovel?\n\r", ch);
    return;
  }

  act("$n looks at $N", FALSE, ch, 0, victim, TO_NOTVICT);
  act("$n looks at you", FALSE, ch, 0, victim, TO_VICT);


  diff =  GET_AVE_LEVEL(victim) - GET_AVE_LEVEL(ch);

  diff += MobLevBonus(victim);

  if (diff <= -10)
    send_to_char("Too easy to be believed.\n\r", ch);
  else if (diff <= -5)
    send_to_char("Not a problem.\n\r", ch);
  else if (diff <= -3)
    send_to_char("Rather easy.\n\r",ch);
  else if (diff <= -2)
    send_to_char("Easy.\n\r", ch);
  else if (diff <= -1)
    send_to_char("Fairly easy.\n\r", ch);
  else if (diff == 0)
    send_to_char("The perfect match!\n\r", ch);
  else if (diff <= 1)
    send_to_char("You would need some luck!\n\r", ch);
  else if (diff <= 2)
    send_to_char("You would need a lot of luck!\n\r", ch);
  else if (diff <= 3)
    send_to_char("You would need a lot of luck and great equipment!\n\r", ch);
  else if (diff <= 5)
    send_to_char("Do you feel lucky, punk?\n\r", ch);
  else if (diff <= 10)
    send_to_char("Are you crazy?  Is that your problem?\n\r", ch);
  else if (diff <= 30)
    send_to_char("You ARE mad!\n\r", ch);
  else
    send_to_char("Why don't I just kill you right now and save you the trouble?\n\r", ch);
  
  if (ch->skills) {
    int skill=0;
    int learn=0;
    int num, num2;
    float fnum;

    if (IsAnimal(victim) && ch->skills[SKILL_CONS_ANIMAL].learned) {
      skill = SKILL_CONS_ANIMAL;
      learn = ch->skills[skill].learned;
      act("$N seems to be an animal", FALSE, ch, 0, victim, TO_CHAR);
    }
    if (IsVeggie(victim) && ch->skills[SKILL_CONS_VEGGIE].learned) {
      if (!skill)
	skill = SKILL_CONS_VEGGIE;
      learn = MAX(learn, ch->skills[SKILL_CONS_VEGGIE].learned);
      act("$N seems to be an ambulatory vegetable", 
	  FALSE, ch, 0, victim, TO_CHAR);
    }
    if (IsDiabolic(victim) && ch->skills[SKILL_CONS_DEMON].learned) {
      if (!skill)
	skill = SKILL_CONS_DEMON;
      learn = MAX(learn, ch->skills[SKILL_CONS_DEMON].learned);
      act("$N seems to be a demon!", FALSE, ch, 0, victim, TO_CHAR);
    }
    if (IsReptile(victim) && ch->skills[SKILL_CONS_REPTILE].learned) {
      if (!skill)
	skill = SKILL_CONS_REPTILE;
      learn = MAX(learn, ch->skills[SKILL_CONS_REPTILE].learned);
      act("$N seems to be a reptilian creature", 
	  FALSE, ch, 0, victim, TO_CHAR);
    }
    if (IsUndead(victim) && ch->skills[SKILL_CONS_UNDEAD].learned) {
      if (!skill)
	skill = SKILL_CONS_UNDEAD;
      learn = MAX(learn, ch->skills[SKILL_CONS_UNDEAD].learned);
      act("$N seems to be undead", FALSE, ch, 0, victim, TO_CHAR);
    }

    if (IsGiantish(victim)&& ch->skills[SKILL_CONS_GIANT].learned) {
      if (!skill)
	skill = SKILL_CONS_GIANT;
      learn = MAX(learn, ch->skills[SKILL_CONS_GIANT].learned);
      act("$N seems to be a giantish creature", FALSE, ch, 0, victim, TO_CHAR);
    }
    if (IsPerson(victim) && ch->skills[SKILL_CONS_PEOPLE].learned) {
      if (!skill)
	skill = SKILL_CONS_PEOPLE;
      learn = MAX(learn, ch->skills[SKILL_CONS_PEOPLE].learned);
      act("$N seems to be a human or demi-human", 
	  FALSE, ch, 0, victim, TO_CHAR);
    }
    if (IsOther(victim)&& ch->skills[SKILL_CONS_OTHER].learned) {
      if (!skill)
	skill = SKILL_CONS_OTHER;
      learn = MAX(learn, ch->skills[SKILL_CONS_OTHER].learned/2);
      act("$N seems to be a monster you know about", 
	  FALSE, ch, 0, victim, TO_CHAR);
    }

    if (learn > 95) learn = 95;

    if (learn == 0) return;

    WAIT_STATE(ch, PULSE_VIOLENCE*2);

    num = GetApprox(GET_MAX_HIT(victim), learn);
    fnum = ((float)num/(float)GET_MAX_HIT(ch));

    sprintf(buf, "Est Max hits are: %s\n\r", DescRatio(fnum));
    send_to_char(buf, ch);

    num = GetApprox(GET_AC(victim), learn);
    fnum = ((float)num/(float)GET_AC(ch));

    sprintf(buf, "Est. armor class is : %s\n\r", DescRatio(fnum));
    send_to_char(buf, ch);

    if (learn > 60) {
      sprintf(buf, "Est. # of attacks: %s\n\r", 
	      DescAttacks(GetApprox((int)victim->mult_att, 
			 	  learn)));
      send_to_char(buf, ch);
    }
    if (learn > 70) {

      num =   GetApprox((int)victim->specials.damnodice, 
			learn);
      num2 =  GetApprox((int)victim->specials.damsizedice, 
			learn);

      fnum = (float)num*(num2/2.0);
      sprintf(buf, "Est. damage of attacks is %s\n\r", 
	      DescDamage(fnum));
            
      send_to_char(buf, ch);
    }

    if (learn > 80) {
      
      num =   GetApprox(GET_HITROLL(victim), learn);
      num2 =  21 - CalcThaco(ch);
      if (num2 > 0)
	fnum = ((float)num/(float)num2);
      else
	fnum = 2.0;

      sprintf(buf, "Est. Thaco: %s\n\r", DescRatio(fnum));
	    
      send_to_char(buf, ch);

      num =   GetApprox(GET_DAMROLL(victim), learn);
      num2 =  GET_DAMROLL(ch);
      fnum = (num/(float)num2);

      sprintf(buf, "Est. Dam bonus is: %s\n\r", DescRatio(fnum));

      send_to_char(buf, ch);
    }
  }

}

void do_spells(struct char_data *ch, char *argument, int cmd)
{
  int spl, i;
  char buf[MAX_EXIST_SPELL*80];
  extern char *spells[];
  extern int spell_status[];
  extern struct skill_data skill_info[MAX_SKILLS+10];
  
  if (IS_NPC(ch))    {
    send_to_char("You ain't nothin' but a hound-dog.\n\r", ch);
    return;
  }
  
  *buf=0;

  for (;isspace(*argument);argument++);

  if (!*argument) {
    for (i = 1, spl = 0; i <= MAX_EXIST_SPELL; i++, spl++) { 
      if (GetMaxLevel(ch) > LOW_IMMORTAL || 
          skill_info[i].min_level[MIN_LEVEL_CLERIC] < ABS_MAX_LVL)
	sprintf(buf + strlen(buf),
		"[%2d] %-20s  Mana: %3d, Cl: %2d, Mu: %2d, Dr: %2d\n\r",
		i, spells[spl], 
		skill_info[i].min_usesmana, 
		skill_info[i].min_level[MIN_LEVEL_CLERIC],
                skill_info[i].min_level[MIN_LEVEL_MAGIC],
                skill_info[i].min_level[MIN_LEVEL_DRUID]);
     }
    strcat(buf, "\n\r");
    page_string(ch->desc, buf, 1);
  } else {
    int RaceMax,j,k,l;
    int spell_data[MAX_EXIST_SPELL+1][2];

    for(i=0;i<=MAX_EXIST_SPELL;i++) 
      spell_data[i][0]=spell_data[i][1]=0;

    switch(*argument) {
    case 'C':
    case 'c':
    case 'P':
    case 'p':
      

      if (GetMaxLevel(ch) < LOW_IMMORTAL)
	RaceMax = RacialMax[GET_RACE(ch)][CLERIC_LEVEL_IND];
      else 
	RaceMax = 50;

      for (i = 1, spl = 0; i <= MAX_EXIST_SPELL; i++, spl++) {
        if(  skill_info[i].min_level[MIN_LEVEL_CLERIC] <= RaceMax &&
           skill_info[i].min_level[MIN_LEVEL_CLERIC] < LOW_IMMORTAL) {
	  spell_data[i][1] = skill_info[i].min_level[MIN_LEVEL_CLERIC];
	  spell_data[i][0] = i;
	}
      }

      for(i=0;i<MAX_EXIST_SPELL;i++)
	for(j=MAX_EXIST_SPELL;j>i;--j)
	  if(spell_data[j-1][1] > spell_data[j][1]) {
	    k=spell_data[j][0];
	    l=spell_data[j][1];
	    spell_data[j][0]=spell_data[j-1][0];
	    spell_data[j][1]=spell_data[j-1][1];
	    spell_data[j-1][0]=k;
	    spell_data[j-1][1]=l;
	  }
      
      
      send_to_char(" Num      Spell Name      Level       Min. Mana Use\n\r",
		   ch);
      i=1;
      while( spell_data[i][1] <= RaceMax ) {
	if(spell_data[i][1] > 0)
	  sprintf(buf + strlen(buf),
		  "[%-3d] %-20s  %-2d            %-3d\n\r",
		  spell_data[i][0],spells[spell_data[i][0]-1],
		  skill_info[spell_data[i][0]].min_level[MIN_LEVEL_CLERIC],
		  skill_info[spell_data[i][0]].min_usesmana);
	i++;
      }
      strcat(buf,"\n\r");
      page_string(ch->desc, buf, 1);
      break;
    case 'M':
    case 'm':

      if (GetMaxLevel(ch) < LOW_IMMORTAL)
        RaceMax = RacialMax[GET_RACE(ch)][MAGE_LEVEL_IND];
      else
        RaceMax = 50;

      for (i = 1, spl = 0; i <= MAX_EXIST_SPELL; i++, spl++) {
        if(  skill_info[i].min_level[MIN_LEVEL_MAGIC] <= RaceMax &&
           skill_info[i].min_level[MIN_LEVEL_MAGIC] < LOW_IMMORTAL) {
          spell_data[i][1] = skill_info[i].min_level[MIN_LEVEL_MAGIC];
          spell_data[i][0] = i;
        }
      }

      for(i=0;i<MAX_EXIST_SPELL;i++)
        for(j=MAX_EXIST_SPELL;j>i;--j)
          if(spell_data[j-1][1] > spell_data[j][1]) {
            k=spell_data[j][0];
            l=spell_data[j][1];
            spell_data[j][0]=spell_data[j-1][0];
            spell_data[j][1]=spell_data[j-1][1];
            spell_data[j-1][0]=k;
            spell_data[j-1][1]=l;
          }


      i=1;
      while( spell_data[i][1] <= RaceMax ) {
        if(spell_data[i][1] > 0)
          sprintf(buf + strlen(buf),
                  "[%-3d] %-20s  %-2d            %-3d\n\r",
                  spell_data[i][0],spells[spell_data[i][0]-1],
                  skill_info[spell_data[i][0]].min_level[MIN_LEVEL_MAGIC],
                  skill_info[spell_data[i][0]].min_usesmana);
        i++;
      }
      strcat(buf,"\n\r");
      send_to_char(" Num      Spell Name      Level       Min. Mana Use\n\r",
                   ch);
      page_string(ch->desc, buf, 1);

      break;
    case 'D':
    case 'd':

      if (GetMaxLevel(ch) < LOW_IMMORTAL)
        RaceMax = RacialMax[GET_RACE(ch)][DRUID_LEVEL_IND];
      else
        RaceMax = 50;

      for (i = 1, spl = 0; i <= MAX_EXIST_SPELL; i++, spl++) {
        if(  skill_info[i].min_level[MIN_LEVEL_DRUID] <= RaceMax &&
           skill_info[i].min_level[MIN_LEVEL_DRUID] < LOW_IMMORTAL) {
          spell_data[i][1] = skill_info[i].min_level[MIN_LEVEL_DRUID];
          spell_data[i][0] = i;
        }
      }

      for(i=0;i<MAX_EXIST_SPELL;i++)
        for(j=MAX_EXIST_SPELL;j>i;--j)
          if(spell_data[j-1][1] > spell_data[j][1]) {
            k=spell_data[j][0];
            l=spell_data[j][1];
            spell_data[j][0]=spell_data[j-1][0];
            spell_data[j][1]=spell_data[j-1][1];
            spell_data[j-1][0]=k;
            spell_data[j-1][1]=l;
          }


      i=1;
      while( spell_data[i][1] <= RaceMax ) {
        if(spell_data[i][1] > 0)
          sprintf(buf + strlen(buf),
                  "[%-3d] %-20s  %-2d            %-3d\n\r",
                  spell_data[i][0],spells[spell_data[i][0]-1],
                  skill_info[spell_data[i][0]].min_level[MIN_LEVEL_DRUID],
                  skill_info[spell_data[i][0]].min_usesmana);
        i++;
      }
      strcat(buf,"\n\r");
      send_to_char(" Num      Spell Name      Level       Min. Mana Use\n\r",
                   ch);
      page_string(ch->desc, buf, 1);

      break;
    default:
      send_to_char("Nope, sorry.  I don't think those can cast.\n\r",ch);
      break;
    }
    return;
  }
}
void do_world(struct char_data *ch, char *argument, int cmd)
{
  static char buf[100];
  long ct, ot;
  char *tmstr, *otmstr;
  extern long Uptime;
  extern long room_count;
  extern long mob_count;
  extern long obj_count;

  sprintf(buf, "Base Source: SillyMUD Version %s.\n", VERSION);
  send_to_char(buf, ch);
  ot = Uptime;
  otmstr = asctime(localtime(&ot));
  *(otmstr + strlen(otmstr) - 1) = '\0';
  sprintf(buf, "Start time was: %s (EST)\n\r", otmstr);
  send_to_char(buf, ch);
  
  ct = time(0);
  tmstr = asctime(localtime(&ct));
  *(tmstr + strlen(tmstr) - 1) = '\0';
  sprintf(buf, "Current time is: %s (EST)\n\r", tmstr);
  send_to_char(buf, ch);
#if HASH  
  sprintf(buf, "Total number of rooms in world: %d\n\r", (int)room_db.klistlen);
#else
  sprintf(buf, "Total number of rooms in world: %d\n\r", (int)room_count);
#endif
  send_to_char(buf, ch);
  sprintf(buf, "Total number of zones in world: %d\n\r\n\r",
	  top_of_zone_table + 1);
  send_to_char(buf, ch);
  sprintf(buf,"Total number of distinct mobiles in world: %d\n\r",
	  top_of_mobt + 1);
  send_to_char(buf, ch);
  sprintf(buf,"Total number of distinct objects in world: %d\n\r\n\r",
	  top_of_objt + 1);
  send_to_char(buf, ch);
  sprintf(buf,"Total number of registered players: %d\n\r",top_of_p_table + 1);
  send_to_char(buf, ch);

  sprintf(buf, "Total number of monsters in game: %d\n\r", (int)mob_count);
  send_to_char(buf, ch);

  sprintf(buf, "Total number of objects in game: %d\n\r", (int)obj_count);
  send_to_char(buf, ch);

}

void do_attribute(struct char_data *ch, char *argument, int cmd)
{
  char buf[MAX_STRING_LENGTH];
  struct affected_type *aff;

  struct time_info_data my_age;


  age2(ch, &my_age);

  sprintf(buf,
	  "You are %d years and %d months, %d cms, and you weigh %d lbs.\n\r",
	  my_age.year, my_age.month,
	  ch->player.height,
	  ch->player.weight);

  send_to_char(buf, ch);
  
  sprintf(buf, "You are carrying %d lbs of equipment.\n\r",
	  IS_CARRYING_W(ch));
  send_to_char(buf, ch); 
  
  sprintf(buf,"You are %s \n\r",ArmorDesc(ch->points.armor));
  send_to_char(buf,ch);
  
  if ((GetMaxLevel(ch) > 15) || (HasClass(ch, CLASS_MAGIC_USER) || 
				 HasClass(ch, CLASS_MONK))) {
    if ((GET_STR(ch)==18) && (HasClass(ch, CLASS_WARRIOR))) {
       sprintf(buf,"You have %d/%d STR, %d INT, %d WIS, %d DEX, %d CON, %d CHR\n\r",
	    GET_STR(ch), GET_ADD(ch), GET_INT(ch), GET_WIS(ch), GET_DEX(ch), GET_CON(ch), GET_CHR(ch));
       send_to_char(buf,ch);
     } else {
       sprintf(buf,"You have %d STR %d INT %d WIS %d DEX %d CON %d CHR\n\r",
	    GET_STR(ch), GET_INT(ch), GET_WIS(ch), GET_DEX(ch), GET_CON(ch),
	       GET_CHR(ch));
       send_to_char(buf,ch);
    }
  }  

  sprintf(buf, "Your hit bonus and damage bonus are %s and %s respectively.\n\r",
	  HitRollDesc(GET_HITROLL(ch)), DamRollDesc(GET_DAMROLL(ch)));
  send_to_char(buf, ch);

  /*
  **   by popular demand -- affected stuff
  */
  send_to_char("\n\rAffecting Spells:\n\r--------------\n\r", ch);
  if (ch->affected) {
    for(aff = ch->affected; aff; aff = aff->next) {
      if (aff->type <= MAX_EXIST_SPELL && aff->location != APPLY_INTRINSIC) {
	switch(aff->type) {
	case SKILL_SNEAK:
	case SPELL_POISON:
	case SPELL_CURSE:
	  break;
	default:
	  sprintf(buf, "Spell : '%s'\n\r",spells[aff->type-1]);
	  send_to_char(buf, ch);
	  break;
	}
      }
    }
  }
}

void do_value(struct char_data *ch, char *argument, int cmd)
{
  char buf[100],buf2[100], name[100];
  struct obj_data *obj=0;
  struct char_data *vict=0;

  /* Spell Names */

  
  /* For Objects */
  extern char *item_types[];
  extern char *extra_bits[];
  extern char *apply_types[];
  extern char *affected_bits[];
  extern char *affected_bits2[];
  extern char *immunity_names[];


  if (!HasClass(ch, CLASS_THIEF) && !IsIntrinsic(ch, SKILL_EVALUATE)) {
    send_to_char("Sorry, you can't do that here", ch);
    return;
  }
  
  argument = one_argument(argument, name);

  if ((obj = get_obj_in_list_vis(ch, name, ch->carrying))==0) {    
    if ((vict = get_char_room_vis(ch, name))==0) {
      send_to_char("Who, or what are you talking about?\n\r", ch);
      return;
    } else {
      only_argument(argument, name);
      if ((obj = get_obj_in_list_vis(ch, name, vict->carrying))==0) {
	act("You can't see that on $M", FALSE, ch, obj, vict, TO_CHAR);
	act("$n looks you over", FALSE, ch, 0, vict, TO_VICT);
	act("$n looks $N over", FALSE, ch, 0, vict, TO_NOTVICT);
	return;
      }
    }
  }

  WAIT_STATE(ch, PULSE_VIOLENCE*2);

  if (!SpyCheck(ch)) {  /* failed spying check */
    if (obj && vict) {
      act("$n looks at you, and $s eyes linger on $p",
	  FALSE, ch, obj, vict, TO_VICT);
      act("$n studies $N", 
	  FALSE, ch, 0, vict, TO_ROOM);
      
    } else if (obj) {
      act("$n intensely studies $p", FALSE, ch, obj, 0, TO_ROOM);
    } else {
      return;
    }
  }

  if(!HasClass(ch, CLASS_THIEF)) { /* it had better be an intrinsic */
    if(GET_RACE(ch) == RACE_DWARF &&
       (GET_ITEM_TYPE(obj) != ITEM_WEAPON && 
	GET_ITEM_TYPE(obj) != ITEM_TREASURE &&
	GET_ITEM_TYPE(obj) != ITEM_ARMOR &&
	GET_ITEM_TYPE(obj) != ITEM_ROCK &&
	GET_ITEM_TYPE(obj) != ITEM_CONTAINER &&
	GET_ITEM_TYPE(obj) != ITEM_BOAT &&
	GET_ITEM_TYPE(obj) != ITEM_WORN)) {
      send_to_char("You can't figure out a damn thing about it.\n\r", ch);
      return;
    }
  }

  sprintf(buf, "Object: %s.  Item type: ", obj->short_description);
  sprinttype(GET_ITEM_TYPE(obj),item_types,buf2);
  strcat(buf,buf2); strcat(buf,"\n\r");
  send_to_char(buf, ch);

  if (!ch->skills) return;

  
  if (number(1,101) < ch->skills[SKILL_EVALUATE].learned/3) {
    if (obj->obj_flags.bitvector) {
      send_to_char("Item will give you following abilities:  ", ch);
      sprintbit((unsigned long)obj->obj_flags.bitvector,affected_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf, ch);
    }
  }
   
  if (number(1,101) < ch->skills[SKILL_EVALUATE].learned/2) {
    send_to_char("Item is: ", ch);
    sprintbit((unsigned long) obj->obj_flags.extra_flags,extra_bits,buf);
    strcat(buf,"\n\r");
    send_to_char(buf,ch);
  }

  sprintf(buf,"Weight: %d, Value: %d, Ego: %d  %s\n\r",
	    obj->obj_flags.weight, 
	  GetApprox(obj->obj_flags.cost, 
		    ch->skills[SKILL_EVALUATE].learned-10), 
	  GetApprox(obj->obj_flags.cost_per_day, 
		    ch->skills[SKILL_EVALUATE].learned-10), 
	  obj->obj_flags.cost_per_day>LIM_ITEM_COST_MIN?"[RARE]":" ");
  send_to_char(buf, ch);

  if (ITEM_TYPE(obj) == ITEM_WEAPON) {
    sprintf(buf, "Damage Dice is '%dD%d'\n\r",
	    GetApprox(obj->obj_flags.value[1],
		      ch->skills[SKILL_EVALUATE].learned-10),
	    GetApprox(obj->obj_flags.value[2],
		      ch->skills[SKILL_EVALUATE].learned-10));
    send_to_char(buf, ch);
  } else if (ITEM_TYPE(obj) == ITEM_ARMOR) {

      sprintf(buf, "AC-apply is %d\n\r",
	      GetApprox(obj->obj_flags.value[0],
			ch->skills[SKILL_EVALUATE].learned-10));
      send_to_char(buf, ch);
  }  
}

char *AlignDesc(int a)
{
  if (a <= -900) {
    return("Really really bad");
  } else if (a <= -500) {
    return("Not nice at all");
  } else if (a <= -351) {
    return("obnoxious as hell");
  } else if (a <= -100) {
    return("just plain annoying");
  } else if (a <= 100) {
    return("You feel balanced");
  } else if (a <= 350) {
    return("Polite");
  } else if (a <= 500) {
    return("Sweet, caring, all that BS.");
  } else if (a <= 900) {
    return("A real goody-goody");
  } else{
    return("So good it makes you sick");
  }
}


char *ArmorDesc(int a)
{
  if (a >= 90) {
    return("barely armored");
  } else if (a >= 50) { 
    return("Lightly armored");
  } else if (a >= 30) {
    return("Medium-armored");
  } else if (a >= 10) {
    return("Fairly well armored");
  } else if (a >= -10) {
    return("Well armored");
  } else if (a >= -30) {
    return("Quite well armored");
  } else if (a >= -50) {
    return("Very well armored");
  } else if (a >= -90) {
    return("Extremeley well armored");
  } else {
    return("armored like a tank");
  }
}

char *HitRollDesc(int a)
{
  if (a < -5) {
    return("Quite bad");
  } else if (a < -1) {
    return("Pretty lousy");
  } else if (a <= 1) {
    return("Not Much of one");
  } else if (a < 3) {
    return("Not bad");
  } else if (a < 8) {
    return("Damn good");
  } else {
    return("Very good");
  }
}

char *DamRollDesc(int a)
{
  if (a < -5) {
    return("Quite bad");
  } else if (a < -1) {
    return("Pretty lousy");
  } else if (a <= 1) {
    return("Not Much of one");
  } else if (a < 3) {
    return("Not bad");
  } else if (a < 8) {
    return("Damn good");
  } else {
    return("Very good");
  }
}

char *DescRatio(float f)  /* theirs / yours */
{
  if (f > 1.0) {
    return("More than twice yours");
  } else if (f > .75) {
    return("More than half again greater than yours");
  } else if (f > .6) {
    return("At least a third greater than yours");
  } else if (f > .4) {
    return("About the same as yours");
  } else if (f > .3) {
    return("A little worse than yours");
  } else if (f > .1) {
    return("Much worse than yours");
  } else {
    return("Extremely inferior");
  }  
}

char *EgoDesc(int a)
{
  if (a > 1470) {
    return("Loki-sized");
  } else if (a > 900) {
    return("Ripper-sized");
  } else if (a > 800) {
    return("John Galt");
  } else if (a > 700) {
    return("Freudian");
  } else if (a > 675) {
    return("egomaniac");
  } else if (a > 650) {
    return("herculean");
  } else if (a > 625) {
    return("monstrous");
  } else if (a > 600) {
    return("colossal");
  } else if (a > 575) {
    return("gargantuan");
  } else if (a > 550) {
    return("mammoth");
  } else if (a > 525) {
    return("humongous");
  } else if (a > 500) {
    return("gigantic");
  } else if (a > 475) {
    return("enormous");
  } else if (a > 450) {
    return("huge");
  } else if (a > 425) {
    return("impressive");
  } else if (a > 400) {
    return("large");
  } else if (a > 375) {
    return("strong");
  } else if (a > 350) {
    return("notable");
  } else if (a > 300) {
    return("snotty");
  } else if (a > 275) {
    return("fair");
  } else if (a > 250) {
    return("average");
  } else if (a > 225) {
    return("moderate");
  } else if (a > 200) {
    return("normal");
  } else if (a > 175) {
    return("boring");
  } else if (a > 150) {
    return("unimpressive");
  } else if (a > 125) {
    return("mediocre");
  } else if (a > 105) {
    return("small");
  } else if (a > 85) {
    return("computer geek");
  } else if (a > 75) {
    return("humble");
  } else if (a > 60) {
    return("wimpy");
  } else if (a > 45) {
    return("trifling");
  } else if (a > 30) {
    return("miniscule");
  } else if (a > 15) {
    return("teenie weenie");
  } else {
    return("itty bitty");
  }
}


char *DescDamage(float dam)
{
  if (dam < 1.0) {
    return("Minimal Damage");
  } else if (dam <= 2.0) {
    return("Slight damage");
  } else if (dam <= 4.0) {
    return("A bit of damage");
  } else if (dam <= 10.0) {
    return("A decent amount of damage");
  } else if (dam <= 15.0) {
    return("A lot of damage");
  } else if (dam <= 25.0) {
    return("A whole lot of damage");
  } else if (dam <= 35.0) {
    return("A very large amount");
  } else {
    return("A TON of damage");
  }
}

char *DescAttacks(float a)
{
  if (a < 1.0) {
    return("Not many");
  } else if (a < 2.0) {
    return("About average");
  } else if (a < 3.0) {
    return("A few");
  } else if (a < 5.0) {
    return("A lot");
  } else if (a < 9.0) {
    return("Many");
  } else {
    return("A whole bunch");
  }
}


void do_display(struct char_data *ch, char *arg, int cmd)
{
 int i;

 if(IS_NPC(ch))
    return;

 i = atoi(arg);

 switch(i) {
 case 0: if(ch->term == 0) {
            send_to_char("Display unchanged.\n\r", ch);
            return;
	  }
          ch->term = 0;
          ScreenOff(ch);
          send_to_char("Display now turned off.\n\r", ch);
          return;
 
 case 1: if(ch->term == 1) {
            send_to_char("Display unchanged.\n\r", ch);
            return;
	   }
          ch->term = VT100;
          InitScreen(ch);
          send_to_char("Display now set to VT100.\n\r", ch);
          return;

 default: if(ch->term == VT100) {
            send_to_char("Term type is currently VT100.\n\r", ch);
            return;
	  }
          send_to_char("Display is currently OFF.\n\r", ch);
          return;
 }
}

void ScreenOff(struct char_data *ch)
{
 char buf[255];

 sprintf(buf, VT_MARGSET, 0, ch->size- 1);
 send_to_char(buf, ch);
 send_to_char(VT_HOMECLR, ch);
}

void do_resize(struct char_data *ch, char *arg, int cmd)
{
 int i;

 if(IS_NPC(ch))
   return;

 i = atoi(arg);

 if(i < 7) {
    send_to_char("Screen size must be greater than 7.\n\r", ch);
    return;
  }

 ch->size = i;

 if(ch->term == VT100) {
    ScreenOff(ch);
    InitScreen(ch);
  }

 send_to_char("Ok.\n\r", ch);
 return;
}

int MobLevBonus(struct char_data *ch)
{
  int t=0;
  extern struct index_data *mob_index;

  if (mob_index[ch->nr].func == magic_user)
    t+=5;
  if (mob_index[ch->nr].func == BreathWeapon)
    t+=7;
  if (mob_index[ch->nr].func == fighter)
    t+=3;
  if (mob_index[ch->nr].func == snake)
    t+=3;

  t+=(ch->mult_att-1)*3;

  if (GET_HIT(ch) > GetMaxLevel(ch)*8)
    t+=1;
  if (GET_HIT(ch) > GetMaxLevel(ch)*12)
    t+=1;
  if (GET_HIT(ch) > GetMaxLevel(ch)*16)
    t+=1;
  if (GET_HIT(ch) > GetMaxLevel(ch)*20)
    t+=1;

  return(t);
}

void do_report(struct char_data *ch, char *argument, int cmd)
{
  /* do a 'say' containing one's vital statistics */
  char buf[256];

  if (!IS_PC(ch)) {
    return;
  }

  sprintf(buf, "H:%d/%d    M:%d/%d    V:%d/%d   Exp:%d",
	  GET_HIT(ch), GET_MAX_HIT(ch), GET_MANA(ch), 
	  GET_MAX_MANA(ch), GET_MOVE(ch), GET_MAX_MOVE(ch),
	  GET_EXP(ch));

  do_say(ch, buf, 0);

}

void list_groups(struct char_data *ch)
{
  struct descriptor_data *i;
  struct char_data *person;
  struct follow_type *f;
  int count = 0;
  char buf[200];


  /* go through the descriptor list */
  for (i = descriptor_list;i;i=i->next) {
  /* find everyone who is a master  */
    if (!i->connected) {
      person = (i->original ? i->original:i->character);

  /* list the master and the group name */
      if (!person->master && IS_AFFECTED(person, AFF_GROUP)) {
	if (person->specials.gname && CAN_SEE(ch, person)) {
	  sprintf(buf, "%s %s\n\r", fname (GET_NAME(person)), 
		  person->specials.gname);
	  send_to_char(buf, ch);
	  /* list the members that ch can see */
	  count = 0;
	  for(f=person->followers; f; f=f->next) {
	    if (IS_AFFECTED(f->follower, AFF_GROUP) && IS_PC(f->follower)) {
	      count++;
	      if (CAN_SEE(ch, f->follower)) {
		sprintf(buf, "          %s\n\r", fname(GET_NAME(f->follower)));
	      } else {
		sprintf(buf, "          Someone\n\r");
	      }
	      send_to_char(buf, ch);
	    }
	  }
  /* if there are no group members, then remove the group title */
	  if (count < 1) {
	    send_to_char("Your group name has been  removed\n\r",person);
	    free(person->specials.gname);
	    person->specials.gname = 0;
	  }
	}
      }
    }
  }
}
