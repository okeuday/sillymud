/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "protos.h"

#define DUAL_WIELD(ch) (ch->equipment[WIELD] && ch->equipment[HOLD]&&\
			ITEM_TYPE(ch->equipment[WIELD])==ITEM_WEAPON && \
			ITEM_TYPE(ch->equipment[HOLD])==ITEM_WEAPON)

#define ITEM_SCRAPS 30

/* Structures */

struct char_data *combat_list = 0;   /* head of l-list of fighting chars    */
struct char_data *missile_list = 0;   /* head of l-list of fighting chars    */
struct char_data *combat_next_dude = 0; /* Next dude global trick           */
struct char_data *missile_next_dude = 0; /* Next dude global trick           */
struct zone_data *zone_table;         /* table of reset data             */

char PeacefulWorks = 1;  /* set in @set */
char DestroyedItems;  /* set in MakeScraps */

/* External structures */
#if HASH
extern struct hash_header room_db;
#else
extern struct room_data *room_db;
#endif
extern struct message_list fight_messages[MAX_MESSAGES];
extern struct obj_data  *object_list;
extern struct index_data *mob_index;
extern struct char_data *character_list;
extern struct skill_data skill_info[];
extern struct index_data *obj_index;
extern char *ItemDamType[];
extern int ItemSaveThrows[22][5];
extern struct str_app_type str_app[];
extern int WizLock;
extern struct descriptor_data *descriptor_list;
extern struct title_type titles[MAX_CLASS][ABS_MAX_LVL];
extern struct int_app_type int_app[26];
extern struct wis_app_type wis_app[26];
extern char *room_bits[];
extern byte backstab_mult[];


/* Weapon attack texts */
struct attack_hit_type attack_hit_text[] =
{
  {"hit",    "hits"},            /* TYPE_HIT      */
  {"pound",  "pounds"},          /* TYPE_BLUDGEON */
  {"pierce", "pierces"},         /* TYPE_PIERCE   */
  {"slash",  "slashes"},         /* TYPE_SLASH    */
  {"whip",   "whips"},           /* TYPE_WHIP     */
  {"claw",   "claws"},           /* TYPE_CLAW     */
  {"bite",   "bites"},           /* TYPE_BITE     */
  {"sting",  "stings"},          /* TYPE_STING    */
  {"crush",  "crushes"},         /* TYPE_CRUSH    */
  {"cleave", "cleaves"},
  {"stab",   "stabs"},
  {"smash",  "smashes"},
  {"smite",  "smites"},
  {"blast",  "blasts"}
};




/* The Fight related routines */


void appear(struct char_data *ch)
{
  act("$n suddenly appears!", FALSE, ch,0,0,TO_ROOM);
  
  if (affected_by_spell(ch, SPELL_INVISIBLE))
    affect_from_char(ch, SPELL_INVISIBLE);

  if (affected_by_spell(ch, SPELL_INVIS_TO_ANIMALS))
    affect_from_char(ch, SPELL_INVIS_TO_ANIMALS);
  
  REMOVE_BIT(ch->specials.affected_by, AFF_INVISIBLE);
}



int LevelMod(struct char_data *ch, struct char_data *v, int exp)
{
  float ratio=0.0;
  float fexp;

  ratio = (float)GET_AVE_LEVEL(v)/GET_AVE_LEVEL(ch);

  if (ratio < 1.0)  {
    fexp = ratio*exp;
  } else {
    fexp = exp;
  }

  return((int)fexp);

}

int RatioExp( struct char_data *ch, struct char_data *victim, int total)
{
  if (!IS_SET(victim->specials.act, ACT_AGGRESSIVE) &&
      !IS_SET(victim->specials.act, ACT_META_AGG) &&
      !IS_AFFECTED(victim, AFF_CHARM))
    if (GetMaxLevel(ch) > 20)
      total = LevelMod(ch, victim, total);

  if ((IS_SET(victim->specials.act, ACT_AGGRESSIVE) ||
      IS_SET(victim->specials.act, ACT_META_AGG)) &&
      !IS_AFFECTED(victim, AFF_CHARM)) {
    /* make sure that poly mages don't abuse, by reducing their bonus */
    if (IS_NPC(ch)) {
      total *=3;
      total/=4;
    }
  }

  return(total);
}



void load_messages()
{
  FILE *f1;
  int i,type;
  struct message_type *messages;
  char chk[100];
  
  if (!(f1 = fopen(MESS_FILE, "r"))){
    perror("read messages");
    assert(0);
  }
  
  /*
    find the memset way of doing this...
    */
  
  for (i = 0; i < MAX_MESSAGES; i++)	{ 
    fight_messages[i].a_type = 0;
    fight_messages[i].number_of_attacks=0;
    fight_messages[i].msg = 0;
  }
  
  fscanf(f1, " %s \n", chk);
  
  i = 0;
  
  while(*chk == 'M')	{
    fscanf(f1," %d\n", &type);
    
    if(i>=MAX_MESSAGES){
      logE("Too many combat messages.");
      exit(0);
    }
    
    CREATE(messages,struct message_type,1);
    fight_messages[i].number_of_attacks++;
    fight_messages[i].a_type=type;
    messages->next=fight_messages[i].msg;
    fight_messages[i].msg=messages;
    
    messages->die_msg.attacker_msg      = fread_string(f1);
    messages->die_msg.victim_msg        = fread_string(f1);
    messages->die_msg.room_msg          = fread_string(f1);
    messages->miss_msg.attacker_msg     = fread_string(f1);
    messages->miss_msg.victim_msg       = fread_string(f1);
    messages->miss_msg.room_msg         = fread_string(f1);
    messages->hit_msg.attacker_msg      = fread_string(f1);
    messages->hit_msg.victim_msg        = fread_string(f1);
    messages->hit_msg.room_msg          = fread_string(f1);
    messages->god_msg.attacker_msg      = fread_string(f1);
    messages->god_msg.victim_msg        = fread_string(f1);
    messages->god_msg.room_msg          = fread_string(f1);
    fscanf(f1, " %s \n", chk);
    i++;
  }
  
  fclose(f1);
}


void update_pos( struct char_data *victim )
{
  
  if ((GET_HIT(victim) > 0) && (GET_POS(victim) > POSITION_STUNNED)) {
    return;
  } else if (GET_HIT(victim) > 0 ) {
    if (!IS_AFFECTED(victim, AFF_PARALYSIS)) {
      if (!MOUNTED(victim))
	GET_POS(victim) = POSITION_STANDING;
      else 
	GET_POS(victim) == POSITION_MOUNTED;
    } else {
      GET_POS(victim) = POSITION_STUNNED;
    }
  } else if (GET_HIT(victim) <= -11) {
    GET_POS(victim) = POSITION_DEAD;
  } else if (GET_HIT(victim) <= -6) {
    GET_POS(victim) = POSITION_MORTALLYW;
  } else if (GET_HIT(victim) <= -3) {
    GET_POS(victim) = POSITION_INCAP;
  } else {
    GET_POS(victim) = POSITION_STUNNED;
  }

}


int check_peaceful(struct char_data *ch, char *msg)
{
  struct room_data *rp;

  extern char PeacefulWorks;
  
  if (!PeacefulWorks) return(0);

  rp = real_roomp(ch->in_room);
  if (rp && rp->room_flags&PEACEFUL) {
    send_to_char(msg, ch);
    return 1;
  }
  return 0;
}

/* start one char fighting another (yes, it is horrible, I know... )  */
void set_fighting(struct char_data *ch, struct char_data *vict)
{
  
  if (ch->specials.fighting) {
    logE("Fighting character set to fighting another.");
    return;
  }
  
  if (vict->attackers <= 5) {
    vict->attackers+=1;
  } else {
    logE("more than 6 people attacking one target");
  }
  ch->next_fighting = combat_list;
  combat_list = ch;
  
  if(IS_AFFECTED(ch,AFF_SLEEP))
    affect_from_char(ch,SPELL_SLEEP);
  
  ch->specials.fighting = vict;
  GET_POS(ch) = POSITION_FIGHTING;
}



/* remove a char from the list of fighting chars */
void stop_fighting(struct char_data *ch)
{
  struct char_data *tmp;
  
  if (!ch->specials.fighting) {
    char buf[300];
    sprintf(buf, "%s not fighting at invocation of stop_fighting",
	    GET_NAME(ch));
    return;
  }

  /* so vampires can feed :) */
  if(IS_AFFECTED2(ch, AFF2_FEEDING))
    REMOVE_BIT(ch->specials.affected_by2, AFF2_FEEDING);

  if(ch->specials.bitten)
    ch->specials.bitten = FALSE;

  if(IS_AFFECTED2(ch, AFF2_BERSERK) && ch->specials.fighting->attackers < 2) {
    struct affected_type af;
    
    REMOVE_BIT(ch->specials.affected_by2, AFF2_BERSERK);
    affect_from_char(ch, SKILL_BERSERK);
    
    af.type      = SKILL_BERSERK;
    af.duration  = 2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    
    affect_to_char(ch, &af);

    GET_MOVE(ch) -= 40;

    send_to_char("Boy, that was really exhausting.\n\r",ch);
  }  

  ch->specials.fighting->attackers-=1;
  if (ch->specials.fighting->attackers < 0) {
    logE("too few people attacking");
    ch->specials.fighting->attackers = 0;
  }
  
  if (ch == combat_next_dude)
    combat_next_dude = ch->next_fighting;
  
  if (combat_list == ch)
    combat_list = ch->next_fighting;
  else	{
    for (tmp = combat_list; tmp && (tmp->next_fighting != ch); 
	 tmp = tmp->next_fighting);
    if (!tmp) {
      logE("Char fighting not found Error (fight.c, stop_fighting)");
      abort();
    }
    tmp->next_fighting = ch->next_fighting;
  }
  
  ch->next_fighting = 0;
  ch->specials.fighting = 0;
  if (MOUNTED(ch))
    GET_POS(ch) = POSITION_MOUNTED;
  else 
    GET_POS(ch) = POSITION_STANDING;
  update_pos(ch);
}



#define MAX_NPC_CORPSE_TIME 5
#define MAX_PC_CORPSE_TIME 10

void make_corpse(struct char_data *ch)
{
  struct obj_data *corpse, *o;
  struct obj_data *money;	
  char buf[MAX_INPUT_LENGTH];
  int i, ADeadBody=FALSE;
  
  struct obj_data *create_money( int amount );
  
  CREATE(corpse, struct obj_data, 1);
  clear_object(corpse);
  
  corpse->item_number = NOWHERE;
  corpse->in_room = NOWHERE;
  
  if (!IS_NPC(ch) || (!IsUndead(ch))) {
    sprintf(buf, "corpse %s",ch->player.name);
    corpse->name = strdup(buf);
    
    sprintf(buf, "The corpse of %s is lying here.", 
	    (IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
    corpse->description = strdup(buf);
    
    sprintf(buf, "the corpse of %s",
	    (IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
    corpse->short_description = strdup(buf);
    
    ADeadBody = TRUE;
    
  } else if (IsUndead(ch)) {
    corpse->name = strdup("dust pile");
    corpse->description = strdup("A pile of dust lies here.");
    corpse->short_description = strdup("a pile of dust");	   
  } 
  
  corpse->contains = ch->carrying;
  if(GET_GOLD(ch)>0) {
    money = create_money(GET_GOLD(ch));
    GET_GOLD(ch)=0;
    obj_to_obj(money,corpse);
  }
  
  corpse->obj_flags.type_flag = ITEM_CONTAINER;
  corpse->obj_flags.wear_flags = ITEM_TAKE;
  corpse->obj_flags.value[0] = 0; /* You can't store stuff in a corpse */
  corpse->obj_flags.value[3] = 1; /* corpse identifyer */
  if (ADeadBody) {
    corpse->obj_flags.weight = GET_WEIGHT(ch)+IS_CARRYING_W(ch);
  } else {
    corpse->obj_flags.weight = 1+IS_CARRYING_W(ch);
  }
  corpse->obj_flags.cost_per_day = -1;
  if (IS_NPC(ch)) 
    corpse->obj_flags.timer = MAX_NPC_CORPSE_TIME;
  else
    corpse->obj_flags.timer = MAX_PC_CORPSE_TIME;
  
  for (i=0; i<MAX_WEAR; i++)
    if (ch->equipment[i])
      obj_to_obj(unequip_char(ch, i), corpse);
  
  ch->carrying = 0;
  IS_CARRYING_N(ch) = 0;
  IS_CARRYING_W(ch) = 0;
  
  if (IS_NPC(ch)) {
    corpse->char_vnum = mob_index[ch->nr].virtual;
    corpse->char_f_pos = 0;
    if(corpse->char_vnum == 1102) { /* thorn bush */
      i = number(1,8);
      for(;i>0;i--) {
	o = read_object(1111, VIRTUAL);	/* they have thorns... */
	if(!o)
	  break;
	obj_to_obj(o, corpse);
      }
    }
  } else {
    if (ch->desc) {
      corpse->char_f_pos = ch->desc->pos;
      corpse->char_vnum = 0;
    } else {
      corpse->char_f_pos = 0;
      corpse->char_vnum = 100;
    }
  }
  corpse->carried_by = 0;
  corpse->equipped_by = 0;
  
  corpse->next = object_list;
  object_list = corpse;
  
  for(o = corpse->contains; o; o = o->next_content)
    o->in_obj = corpse;
  
  object_list_new_owner(corpse, 0);
  
  obj_to_room(corpse, ch->in_room);
  check_falling_obj(corpse, ch->in_room); /* hmm */

}

void change_alignment(struct char_data *ch, struct char_data *victim)
{

  int change;

  if (IS_NPC(ch)) return;

  if (IS_GOOD(ch) && (IS_GOOD(victim))) {
    change = (GET_ALIGNMENT(victim)  / 200) * (MAX(1,GetMaxLevel(victim) - GetMaxLevel(ch))); 
  } else if (IS_EVIL(ch) && (IS_GOOD(victim))) {
    change = (GET_ALIGNMENT(victim) / 30) * (MAX(1, GetMaxLevel(victim) - GetMaxLevel(ch)));
  } else if (IS_EVIL(victim) && (IS_GOOD(ch))) {
    change = (GET_ALIGNMENT(victim) / 30) * (MAX(1, GetMaxLevel(victim) - GetMaxLevel(ch)));
  } else if (IS_EVIL(ch) && (IS_EVIL(victim))) {
    change = ((GET_ALIGNMENT(victim) / 200)+1) * (MAX(1, GetMaxLevel(victim) - GetMaxLevel(ch)));
  } else {
    change = ((GET_ALIGNMENT(victim) / 200)+1) * (MAX(1, GetMaxLevel(victim) - GetMaxLevel(ch)));
  }

  if (change == 0) {
    if (GET_ALIGNMENT(victim) > 0) 
      change = 1;
    else if (GET_ALIGNMENT(victim) < 0)
      change = -1;
  }
  
  GET_ALIGNMENT(ch) -= change;

  GET_ALIGNMENT(ch) = MAX(GET_ALIGNMENT(ch), -1000);
  GET_ALIGNMENT(ch) = MIN(GET_ALIGNMENT(ch), 1000);

}

void death_cry(struct char_data *ch)
{
  int door, was_in;
  
  if (ch->in_room == -1)
    return;
  
  act("Your blood freezes as you hear $n's death cry.", FALSE, ch,0,0,TO_ROOM);
  was_in = ch->in_room;
  
  for (door = 0; door <= 5; door++) {
    if (CAN_GO(ch, door))	{
      ch->in_room = (real_roomp(was_in))->dir_option[door]->to_room;
      act("Your blood freezes as you hear someones death cry.",FALSE,ch,0,0,TO_ROOM);
      ch->in_room = was_in;
    }
  }
}



void raw_kill(struct char_data *ch)
{
  if((IS_MOB(ch)) && (!IS_SET(ch->specials.act, ACT_POLYSELF)) && (mob_index[ch->nr].func))
    (*mob_index[ch->nr].func)(ch, 0, "", ch, EVENT_DEATH);

  if (ch->specials.fighting)
    stop_fighting(ch);
  
  death_cry(ch);
  /*
    remove the problem with poison, and other spells
    */
  spell_dispel_magic(IMPLEMENTOR,ch,ch,0);
  
  /*
    give them some food and water so they don't whine.
    */
  GET_COND(ch,THIRST)=20;
  GET_COND(ch,FULL)=20;
  
  /*
   *   return them from polymorph
   */
  
  make_corpse(ch);
  zero_rent(ch);
  if (IS_AFFECTED2(ch, AFF2_ONE_LIFER) && GET_HIT(ch) < -9) {
    free(GET_NAME(ch));
    GET_NAME(ch) = strdup("Odin's heroic minion");
  }
  extract_char(ch);
}



void die(struct char_data *ch)
{
  extern struct player_index_element *player_table;
  extern int top_of_p_table;

  struct char_data *pers;
  int i,tmp;
  char buf[80];
  int fraction;
  struct descriptor_data *fd;
   
  /* need at least 1/fraction worth of exp for the minimum needed for */
  /* the pc's current level, or else you lose a level.  If all three  */
  /* classes are lacking in exp, you lose one level in each class. */

  fraction = 16;
  if (IS_NPC(ch) && (IS_SET(ch->specials.act, ACT_POLYSELF))) {
    /*
     *   take char from storage, to room
     */
    if (ch->desc) {  /* hmmm */
       fd = ch->desc;
       pers = ch->desc->original;
       char_from_room(pers);
       char_to_room(pers, ch->in_room);
       SwitchStuff(ch, pers);
       extract_char(ch);
       ch = pers;
     } else {
       /* we don't know who the original is.  Gets away with it, i guess*/
       return;
     }
  } else {
    fd = ch->desc;
  }

#if LEVEL_LOSS
  if(IS_PC(ch))
     for(i=0;i<MAX_CLASS;i++) {
       if (GET_LEVEL(ch,i) > 1) {
	 if (GET_LEVEL(ch,i) >= LOW_IMMORTAL) break;
	 if (GET_EXP(ch) < (titles[i][GET_LEVEL(ch, i)].exp/fraction)) {
	   tmp = (ch->points.max_hit)/GetMaxLevel(ch);
	   ch->points.max_hit -= tmp;
	   GET_LEVEL(ch, i) -= 1;
	   ch->specials.spells_to_learn -= MAX(1, MAX(2, wis_app[GET_RWIS(ch)].bonus)/HowManyClasses(ch));
	   send_to_char("\n\rInsufficient experience has cost you a level.\n\r",
			ch);
	 }
       }
     }
#endif

#if NEWEXP
   if (GetMaxLevel(ch) > 15)
     gain_exp(ch, -GET_EXP(ch)/2);
   else if (GetMaxLevel(ch) > 10)
     gain_exp(ch, -GET_EXP(ch)/3);
   else if (GetMaxLevel(ch) > 5)
     gain_exp(ch, -GET_EXP(ch)/4);
   else
     gain_exp(ch, -GET_EXP(ch)/5);
#else
     gain_exp(ch, -GET_EXP(ch)/2);
#endif
#if LEVEL_LOSS

  /* warn people if their next death will result in a level loss */
  for(i=0;i<MAX_CLASS;i++) {
    if (GET_LEVEL(ch,i) > 1) {
      if (GET_EXP(ch) < (titles[i][GET_LEVEL(ch, i)].exp/fraction)) {
        send_to_char("\n\r\n\rWARNING WARNING WARNING WARNING WARNING WARNING\n\r",
                     ch);
        send_to_char("Your next death will result in the loss of a level,\n\r",
                     ch);
        sprintf(buf,"unless you get at least %d more exp points.\n\r",
                (titles[i][GET_LEVEL(ch, i)].exp/fraction) - GET_EXP(ch));
        send_to_char(buf,ch);
      }
    }
  }
#endif

  /*
   **      Set the talk[2] to be TRUE, i.e. DEAD
   */
  ch->player.talks[2] = 1;  /* char is dead */

  DeleteHatreds(ch);
  DeleteFears(ch);

  if (IS_SET(ch->specials.affected_by2, AFF2_ONE_LIFER) &&
      !IS_IMMORTAL(ch)) {
    send_to_char("ack, you're dead\n\r", ch);

    for (i = 0; i <= top_of_p_table; i++)	{
      if (!str_cmp((player_table + i)->name, GET_NAME(ch))) {
	free((player_table +i)->name);
	(player_table +i)->name = (char *)malloc(strlen("111111"));
	strcpy((player_table +i)->name, "111111");
      }
    }
  }

  raw_kill(ch);

  if (IS_SET(ch->specials.affected_by2, AFF2_ONE_LIFER)) {
    close_socket(fd);
  }

}



void group_gain(struct char_data *ch, struct char_data *victim)
{
  char buf[256];
  int no_members, share;
  struct char_data *k;
  struct follow_type *f;
  int total, pc;
  
  if (!(k=ch->master))
    k = ch;
  
  /* can't get exp for killing players */
  
  if (!IS_NPC(victim)) {
    return;
  }
  
  if (IS_AFFECTED(k, AFF_GROUP) &&
      (k->in_room == ch->in_room))
    no_members = GET_AVE_LEVEL(k);
  else
    no_members = 0;

  pc = FALSE;
  
  for (f=k->followers; f; f=f->next)
    if (IS_AFFECTED(f->follower, AFF_GROUP) &&
	(f->follower->in_room == ch->in_room)) {
      no_members+=GET_AVE_LEVEL(f->follower);
      if (IS_PC(f->follower))
	pc++;
    }

  if (pc > 10)
    pc = 10;

  if (no_members >= 1)
    share = (GET_EXP(victim)/no_members);
  else
    share = 0;
    
  if (IS_AFFECTED(k, AFF_GROUP) &&
      (k->in_room == ch->in_room)) {

      total = share*GET_AVE_LEVEL(k);

      if (pc) {
	total *= (100 + (3*pc));
	total /= 100;
      }


      RatioExp(k, victim, total);

      sprintf(buf,"You receive your share of %d experience.", total);
      act(buf, FALSE, k, 0, 0, TO_CHAR);
      gain_exp(k,total);
      change_alignment(k, victim);
  }
  
  for (f=k->followers; f; f=f->next) {
    if (IS_AFFECTED(f->follower, AFF_GROUP) &&
	(f->follower->in_room == ch->in_room)) {

        total = share*GET_AVE_LEVEL(f->follower);

	if (IS_PC(f->follower)) {
	  total *= (100 + (1*pc));
	  total /= 100;
	} else 
	  total /= 2;


	if (IS_PC(f->follower)) {
	  total = RatioExp(f->follower, victim, total);
	  sprintf(buf,"You receive your share of %d experience.", total);
	  act(buf, FALSE, f->follower,0,0,TO_CHAR);
	  gain_exp(f->follower,  total);
	
	  change_alignment(f->follower, victim);
	} else {
	  if (f->follower->master && IS_AFFECTED(f->follower, AFF_CHARM)) {
	    total = RatioExp(f->follower->master, victim, total);
	    if (f->follower->master->in_room ==
		f->follower->in_room) {
	      sprintf(buf,"You receive $N's share of %d experience.", total);
	      act(buf, FALSE, f->follower->master,0,f->follower,TO_CHAR);
	      gain_exp(f->follower->master,  total);
	      change_alignment(f->follower, victim);
	    }
	  } else {
	    total = RatioExp(f->follower, victim, total);
	    sprintf(buf,"You receive your share of %d experience.", total);
	    act(buf, FALSE, f->follower,0,0,TO_CHAR);
	    gain_exp(f->follower,  total);
	    
	    change_alignment(f->follower, victim);
	  }
	}
    }
  }
}

char *replace_string(char *str, char *weapon, char *weapon_s)
{
  static char buf[256];
  char *cp;
  
  cp = buf;
  
  for (; *str; str++) {
    if (*str == '#') {
      switch(*(++str)) {
      case 'W' : 
	for (; *weapon; *(cp++) = *(weapon++));
	break;
      case 'w' : 
	for (; *weapon_s; *(cp++) = *(weapon_s++));
	break;
	default :
	  *(cp++) = '#';
	break;
      }
    } else {
      *(cp++) = *str;
    }
    
    *cp = 0;
  } /* For */
  
  return(buf);
}



void dam_message(int dam, struct char_data *ch, struct char_data *victim,
                 int w_type)
{
  struct obj_data *wield;
  char *buf;
  int snum;
  float perc;
  
  static struct dam_weapon_type {
    char *to_room;
    char *to_char;
    char *to_victim;
  } dam_weapons[] = {
    
    {
      "$n misses $N.",                           /*    0    */
      "You miss $N.",
      "$n misses you." 
    }, { 
      "$n scratches $N with $s #w.",                       /*  1.. 2  */
      "You scratch $N as you #w $M.",
      "$n scratches you as $e #W you." 
    }, {
      "$n barely #W $N.",                                   /*  3.. 4  */
      "You barely #w $N.",
      "$n barely #W you."
    }, {
      "$n #W $N.",                                          /*  5.. 6  */
      "You #w $N.",
      "$n #W you."
    }, {
      "$n #W $N hard.",                                     /*  7..10  */
      "You #w $N hard.",
      "$n #W you hard."
    }, {
      "$n #W $N very hard.",                                /* 11..14  */
      "You #w $N very hard.",
      "$n #W you very hard."
    }, {
      "$n #W $N extremely well.",                          /* 15..20  */
      "You #w $N extremely well.",
      "$n #W you extremely well."
    }, {
      "$n massacres $N with $s #w.",     /* > 20    */
      "You massacre $N with your #w.",
      "$n massacres you with $s #w."
    }
  };
  
  w_type -= TYPE_HIT;   /* Change to base of table with text */
  
  wield = ch->equipment[WIELD];
#if 0  
  if (dam <= 0) {
    snum = 0;
  } else if (dam <= 2) {
    snum = 1;
  } else if (dam <= 4) {
    snum = 2;
  } else if (dam <= 10) {
    snum = 3;
  } else if (dam <= 15) {
    snum = 4;
  } else if (dam <= 25) {
    snum = 5;
  } else if (dam <= 35) {
    snum = 6;
  } else {
    snum = 7;
  }
#else

  if (GET_HIT(victim) > 0)
    perc = (float)dam/(float)GET_HIT(victim);
  else
    perc = .99;

  if (dam == 0) {
    snum = 0;
  } else if (perc < .02) {
    snum = 1;
  } else if (perc < .04) {
    snum = 2;
  } else if (perc < .08) {
    snum = 3;
  } else if (perc < .16) {
    snum = 4;
  } else if (perc < .32) {
    snum = 5;
  } else if (perc < .64) {
    snum = 6;
  } else {
    snum = 7;
  }

#endif

 
  buf = replace_string(dam_weapons[snum].to_room, attack_hit_text[w_type].plural, attack_hit_text[w_type].singular);
  act(buf, FALSE, ch, wield, victim, TO_NOTVICT);
  buf = replace_string(dam_weapons[snum].to_char, attack_hit_text[w_type].plural, attack_hit_text[w_type].singular);
  act(buf, FALSE, ch, wield, victim, TO_CHAR);
  buf = replace_string(dam_weapons[snum].to_victim, attack_hit_text[w_type].plural, attack_hit_text[w_type].singular);
  act(buf, FALSE, ch, wield, victim, TO_VICT);
  
}

#if 1
int DamCheckDeny(struct char_data *ch, struct char_data *victim, int type)
{
  struct room_data *rp;
  char buf[MAX_INPUT_LENGTH];

  assert(GET_POS(victim) > POSITION_DEAD);
  
  rp = real_roomp(ch->in_room);
  if (rp && (rp->room_flags&PEACEFUL) && type!=SPELL_POISON && 
      type!=SPELL_HEAT_STUFF) {
    sprintf(buf, "damage(,,,%d) called in PEACEFUL room", type);
    logE(buf);
    return(TRUE); /* true, they are denied from fighting */
  }
  return(FALSE);

}

int DamDetailsOk( struct char_data *ch, struct char_data *v, int dam, int type)
{

  if (dam < 0) return(FALSE);
  if (ch->in_room != v->in_room) return(FALSE);
  if ((ch == v) && 
      ((type != SPELL_POISON) && (type != SPELL_HEAT_STUFF))) return(FALSE);

  if (MOUNTED(ch)) {
    if (MOUNTED(ch) == v) {
      FallOffMount(ch, v);
      Dismount(ch, MOUNTED(ch), POSITION_SITTING);
    }
  }

  return(TRUE);

}


int SetCharFighting(struct char_data *ch, struct char_data *v)
{
  if (GET_POS(ch) > POSITION_STUNNED) {	
    if (!(ch->specials.fighting)) {
       set_fighting(ch, v);
       GET_POS(ch) = POSITION_FIGHTING;
    } else {
       return(FALSE);
    }
  }
  return(TRUE);

}


int SetVictFighting(struct char_data *ch, struct char_data *v)
{

  if ((v != ch) && (GET_POS(v) > POSITION_STUNNED) && 
     (!(v->specials.fighting))) {
     if (ch->attackers < 6) {
        set_fighting(v, ch);
        GET_POS(v) = POSITION_FIGHTING;
      }
  } else {
      return(FALSE);	
  }
  return(TRUE);
}

int DamageTrivia(struct char_data *ch, struct char_data *v, int dam, int type)
{
  if (v->master == ch)
    stop_follower(v);
  
  if (IS_AFFECTED(ch, AFF_INVISIBLE) || IS_AFFECTED2(ch, AFF2_ANIMAL_INVIS))
    appear(ch);

  if (IS_AFFECTED(ch, AFF_SNEAK)) {
    affect_from_char(ch, SKILL_SNEAK);
  }
  if (IS_AFFECTED(ch, AFF_HIDE)){
    REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);
  }
  
  if (IS_AFFECTED(v, AFF_SANCTUARY)) {
      dam = MAX((int)(dam/2), 0);  /* Max 1/2 damage when sanct'd */
  }
  
  dam = PreProcDam(v,type,dam);
  
  if (dam > -1) {
    dam = WeaponCheck(ch, v, type, dam);

    DamageStuff(v, type, dam);
  
    dam=MAX(dam,0);

  
    /*
     *  check if this hit will send the target over the edge to -hits
     */
    if (GET_HIT(v)-dam < 1) {
      if (IS_AFFECTED(v, AFF_LIFE_PROT)) {
	BreakLifeSaverObj(v);
	dam = 0;
	GET_HIT(ch) += (int) (GET_MAX_HIT(ch) / 8);
	REMOVE_BIT(ch->specials.affected_by, AFF_LIFE_PROT);
      }
    }

    if (MOUNTED(v)) {
      if (!RideCheck(v, -(dam/2))) {
	FallOffMount(v, MOUNTED(v));
	WAIT_STATE(v, PULSE_VIOLENCE*2);
	Dismount(v, MOUNTED(v), POSITION_SITTING);
      }
    } else if (RIDDEN(v)) {
      if (!RideCheck(RIDDEN(v), -dam)) {
	FallOffMount(RIDDEN(v), v);
	WAIT_STATE(RIDDEN(v), PULSE_VIOLENCE*2);
	Dismount(RIDDEN(v), v, POSITION_SITTING);
      }
    }
  }

  return(dam);
}

int DoDamage(struct char_data *ch, struct char_data *v, int dam, int type)
{

  if (dam >= 0) {
    GET_HIT(v)-=dam;  

    if (type >= TYPE_HIT) {
      if (IS_AFFECTED(v, AFF_FIRESHIELD)&&
	  !IS_AFFECTED(ch, AFF_FIRESHIELD)) {
	if (damage(v, ch, dam, SPELL_FIREBALL)) {
	  if (GET_POS(ch) == POSITION_DEAD)
	    return(TRUE);
	}
      }
    }
    update_pos(v);
  } else {
/*
    if (!number(0,8)) {
      act("Your attack doesn't harm $N!", FALSE, ch, 0, v, TO_CHAR);
      act("$n's attack doesn't harm $N!", FALSE, ch, 0, v, TO_NOTVICT);
      act("$n's attack doesn't harm you!", FALSE, ch, 0, v, TO_VICT);
    }
*/
  }
  return(FALSE);

}


int DamageMessages( struct char_data *ch, struct char_data *v, int dam,
		    int attacktype)
{
  int nr, max_hit, i, j;
  struct message_type *messages;
  char buf[500];

  if (attacktype == SKILL_KICK) return; /* filter out kicks,
					  hard coded in do_kick */

  else if ((attacktype >= TYPE_HIT) && (attacktype <= TYPE_BLAST)) {
    dam_message(dam, ch, v, attacktype);
    if (ch->equipment[WIELD]) {
      BrittleCheck(ch, dam);
    }
  } else {
    
    for(i = 0; i < MAX_MESSAGES; i++) {
      if (fight_messages[i].a_type == attacktype) {
	nr=dice(1,fight_messages[i].number_of_attacks);

	for(j=1,messages=fight_messages[i].msg;(j<nr)&&(messages);j++)
	  messages=messages->next;
	
	if (!IS_NPC(v) && (GetMaxLevel(v) > MAX_MORT)){
	  act(messages->god_msg.attacker_msg, 
	      FALSE, ch, ch->equipment[WIELD], v, TO_CHAR);
	  act(messages->god_msg.victim_msg, 
	      FALSE, ch, ch->equipment[WIELD], v, TO_VICT);
	  act(messages->god_msg.room_msg, 
	      FALSE, ch, ch->equipment[WIELD], v, TO_NOTVICT);
	} else if (dam > 0) {
	  if (GET_POS(v) == POSITION_DEAD) {
	    act(messages->die_msg.attacker_msg, 
		FALSE, ch, ch->equipment[WIELD], v, TO_CHAR);
	    act(messages->die_msg.victim_msg, 
		FALSE, ch, ch->equipment[WIELD], v, TO_VICT);
	    act(messages->die_msg.room_msg, 
		FALSE, ch, ch->equipment[WIELD], v, TO_NOTVICT);
	  } else {
	    act(messages->hit_msg.attacker_msg, 
		FALSE, ch, ch->equipment[WIELD], v, TO_CHAR);
	    act(messages->hit_msg.victim_msg, 
		FALSE, ch, ch->equipment[WIELD], v, TO_VICT);
	    act(messages->hit_msg.room_msg, 
		FALSE, ch, ch->equipment[WIELD], v, TO_NOTVICT);
	  }
	} else if (dam == 0) {
	  act(messages->miss_msg.attacker_msg, 
	      FALSE, ch, ch->equipment[WIELD], v, TO_CHAR);
	  act(messages->miss_msg.victim_msg, 
	      FALSE, ch, ch->equipment[WIELD], v, TO_VICT);
	  act(messages->miss_msg.room_msg, 
	      FALSE, ch, ch->equipment[WIELD], v, TO_NOTVICT);
	}
      }
    }
  }
  switch (GET_POS(v)) {
  case POSITION_MORTALLYW:
    act("$n is mortally wounded, and will die soon, if not aided.", 
	TRUE, v, 0, 0, TO_ROOM);
    act("You are mortally wounded, and will die soon, if not aided.", 
	FALSE, v, 0, 0, TO_CHAR);
    break;
  case POSITION_INCAP:
    act("$n is incapacitated and will slowly die, if not aided.", 
	TRUE, v, 0, 0, TO_ROOM);
    act("You are incapacitated and you will slowly die, if not aided.", 
	FALSE, v, 0, 0, TO_CHAR);
    break;
  case POSITION_STUNNED:
    act("$n is stunned, but will probably regain consciousness again.", 
	TRUE, v, 0, 0, TO_ROOM);
    act("You're stunned, but you will probably regain consciousness again.", 
	FALSE, v, 0, 0, TO_CHAR);
    break;
  case POSITION_DEAD:
    act("$n is dead! R.I.P.", TRUE, v, 0, 0, TO_ROOM);
    act("You are dead!  Sorry...", FALSE, v, 0, 0, TO_CHAR);
    break;
    
  default:  /* >= POSITION SLEEPING */
    
    max_hit=hit_limit(v);
    
    if (dam > (max_hit/5)) {
      act("That Really HURT!",FALSE, v, 0, 0, TO_CHAR);  
    }
    if (GET_HIT(v) < (max_hit/(100./(v->specials.pct))) && GET_HIT(v) > 0) {   /* Wimpy pct coding */
      act("You wish that your wounds would stop BLEEDING so much!",
	  FALSE,v,0,0,TO_CHAR);
      if (IS_NPC(v) && (IS_SET(v->specials.act, ACT_FIGURINE)) && GET_HIT(v) < GET_MAX_HIT(v)/10) {
          struct char_data *t;
          if(v->master)
             t = v->master;
          act("$n shakes and begins to shrink.", FALSE, v, 0, 0, TO_ROOM);
          REMOVE_BIT(v->specials.affected_by, AFF_CHARM);
          stop_follower(v);
          char_from_room(v);
          char_to_room(v, 3);
          if(t)
             obj_to_char(v->link, t);
          return;
    }

      if (IS_NPC(v) && (IS_SET(v->specials.act, ACT_WIMPY))) {
	strcpy(buf, "flee");
	command_interpreter(v, buf);
      } else if (!IS_NPC(v)) {
	if (IS_SET(v->specials.act, PLR_WIMPY)) {
	  strcpy(buf, "flee");
	  command_interpreter(v, buf);
	}

      }
    }
    if (MOUNTED(v)) {
      /* chance they fall off */
      RideCheck(v, -dam/2);
    }
    if (RIDDEN(v)) {
      /* chance the rider falls off */
      RideCheck(RIDDEN(v), -dam);
    }
    break;
  }
}


int DamageEpilog(struct char_data *ch, struct char_data *victim)
{
  int exp, mula;
  char buf[256];
  bool gotsome;
  struct room_data *rp;

  extern char DestroyedItems;

  gotsome = FALSE;

  if (IS_PC(victim) && 
     !(victim->desc)) {
     do_flee(victim, "", 0);
     if (GET_POS(victim) != POSITION_DEAD)
       return(FALSE);
  }

  if (!AWAKE(victim))
    if (victim->specials.fighting)
      stop_fighting(victim);
  
  if (GET_POS(victim) == POSITION_DEAD) {

    /*
      special for no-death rooms
      */
    rp = real_roomp(victim->in_room);
    if (rp && IS_SET(rp->room_flags, NO_DEATH)) {
      GET_HIT(victim) = 1;
      GET_POS(victim) = POSITION_STANDING;
      strcpy(buf, "flee");
      command_interpreter(victim, buf);
      return(FALSE);
    }

    if (ch->specials.fighting == victim)
      stop_fighting(ch);
    if (IS_NPC(victim) && !IS_SET(victim->specials.act, ACT_POLYSELF)) {
      if(ch->specials.loot) {  /* If autoloot is set.. let's get the gold! */
        if(GET_GOLD(victim)>0) {
          mula = GET_GOLD(victim);
          GET_GOLD(victim)=0;
          GET_GOLD(ch) += mula;
          sprintf(buf, "You loot %d gold from the body of %s.\n\r", mula, victim->player.short_descr);
          send_to_char(buf, ch);
          gotsome = TRUE;
        }
        else
          gotsome = FALSE;
      }
 
      if (IS_AFFECTED(ch, AFF_GROUP)) {
	group_gain(ch, victim);
        if((ch->specials.split) && gotsome) {
          sprintf(buf, "split %d", mula);
          command_interpreter(ch, buf);
        }
      } else {
	/* Calculate level-difference bonus */
	exp = GET_EXP(victim);
	
	exp = MAX(exp, 1);

	if (!IS_PC(victim)) {
	  gain_exp(ch, exp);
	}
	change_alignment(ch, victim);
      }
    }
    if (IS_PC(victim)) {
      if (victim->in_room > -1) {
	if (IS_NPC(ch)&&!IS_SET(ch->specials.act, ACT_POLYSELF)) {
	   sprintf(buf, "%s killed by %s at %s",
		GET_NAME(victim), ch->player.short_descr,
		(real_roomp(victim->in_room))->name);

	} else {
	   if ((IS_GOOD(ch) && !IS_EVIL(victim))  ||
	       IS_EVIL(ch) && IS_NEUTRAL(victim)) {
	     sprintf(buf, "%s killed by %s at %s -- <Player kill, Illegal>",
		     GET_NAME(victim), ch->player.name, 
		     (real_roomp(victim->in_room))->name);
	   } else {
	     sprintf(buf, "%s killed by %s at %s",
		     GET_NAME(victim), GET_NAME(ch),
		     (real_roomp(victim->in_room))->name);
	   }

	}
      } else {
	sprintf(buf, "%s killed by %s at Nowhere.",
		GET_NAME(victim),
		(IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
      }
      log_sev(buf, 6);
    }
#if 0
    /* this has been added to try and track down people killing
       shopkeepers 
       */
    if (IS_NPC(victim)) {
      if (IS_SET(victim->specials.act, ACT_SPEC)) {
	if (mob_index[victim->nr].func) {
	  if (mob_index[victim->nr].func == shop_keeper) {
	    sprintf(buf, "%s (shopkeeper) killed by %s\n",
		    GET_NAME(victim), GET_NAME(ch));
	    log_sev(buf, 6);
	  }
	}
      }
    }
#endif
    die(victim);
    /*
     *  if the victim is dead, return TRUE.
     */
    victim = 0;
    return(TRUE);
  } else {
    if (DestroyedItems) {
      if (check_falling(victim)) /* 0 = ok, 1 = dead */
	return(TRUE);
      DestroyedItems = 0;
    }
    return(FALSE);
  }


}

int AreaDamage(struct char_data *ch, int dam, int attacktype, 
	       char *same_room_hitmsg, char *same_room_missmsg, 
	       char *global_msg, bool save_negates, bool heat_blinder)
{

  struct char_data *tmp_victim, *temp;
  struct obj_data *obj, *next;
  int dam_type;
  char buf[100];

  for( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim;
      tmp_victim = temp) {
    temp = tmp_victim->next_in_room;

    if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
       if (GetMaxLevel(tmp_victim)<LOW_IMMORTAL) {
	 if (!in_group(ch, tmp_victim)) {
	   if ( saves_spell(tmp_victim, SAVING_SPELL) ) {
	     if(save_negates) {
	       dam = 0;
	       send_to_char(same_room_missmsg, tmp_victim);
	     } else {
	       dam >>= 1;
	     }
	   }
	   send_to_char(same_room_hitmsg, tmp_victim);
	   if(heat_blinder)
	     heat_blind(tmp_victim);
	   MissileDamage(ch, tmp_victim, dam, attacktype);
	 } else {
	   send_to_char(same_room_missmsg, tmp_victim);
	 }
       } else {
	 send_to_char(same_room_missmsg, tmp_victim);
       }
     }
  }

  if(*global_msg) {
    for(tmp_victim = character_list; tmp_victim; tmp_victim = temp) {
      temp = tmp_victim->next;
      
      if (ch->in_room != tmp_victim->in_room && tmp_victim->in_room != NOWHERE)
	if (real_roomp(ch->in_room)->zone ==
	    real_roomp(tmp_victim->in_room)->zone) {
	  send_to_char(global_msg, tmp_victim);
	}
    }
  }

  /* damage stuff on ground */

  dam_type = GetItemDamageType(attacktype);
  
  if(dam_type) {

    obj = real_roomp(ch->in_room)->contents;
    while(obj) {
      next = obj->next_content;
      if (obj->item_number >= 0 && obj->item_number != ITEM_SCRAPS) {
	int i;
	i = DamagedByAttack(obj, dam_type);
	
	if(i == -1)
	  MakeScrap(ch, obj);
	else if(i != 0) {
	  sprintf(buf, "%s is %s.\n\r",obj->short_description,
		  ItemDamType[dam_type-1]);
	  send_to_room(buf, ch->in_room);
	  if (DamageItem(ch, obj, i)) {
	    MakeScrap(ch, obj);
	  }
	}
      }
      obj = next;
    }
  }
}

	     
  
  

int MissileDamage(struct char_data *ch, struct char_data *victim,
	          int dam, int attacktype)
{

   if (DamCheckDeny(ch, victim, attacktype))
     return(FALSE);

   dam = SkipImmortals(victim, dam);

   if (!DamDetailsOk(ch, victim, dam, attacktype))
     return(FALSE);

   SetVictFighting(ch, victim);
/*
  make the ch hate the loser who used a missile attack on them.
*/
   if (!IS_PC(victim)) {
     if (!Hates(victim, ch)) {
       AddHated(victim, ch);
     }
   }
   dam = DamageTrivia(ch, victim, dam, attacktype);

   if (DoDamage(ch, victim, dam, attacktype))
     return(TRUE);

   DamageMessages(ch, victim, dam, attacktype);

   if (DamageEpilog(ch, victim)) return(TRUE);

   return(FALSE);  /* not dead */

}

int damage(struct char_data *ch, struct char_data *victim,
	          int dam, int attacktype)
{

   if (DamCheckDeny(ch, victim, attacktype))
     return(FALSE);

   dam = SkipImmortals(victim, dam);

   if (!DamDetailsOk(ch, victim, dam, attacktype))
     return(FALSE);

   SetVictFighting(ch, victim);
   SetCharFighting(ch, victim);

   dam = DamageTrivia(ch, victim, dam, attacktype);

   if (DoDamage(ch, victim, dam, attacktype))
     return(TRUE);

   DamageMessages(ch, victim, dam, attacktype);

   if (DamageEpilog(ch, victim)) return(TRUE);

   return(FALSE);  /* not dead */
}

#else    /* this is for an example of the old code, slightly modified &/

int damage(struct char_data *ch, struct char_data *victim,
	   int dam, int attacktype)
{
  char buf[MAX_INPUT_LENGTH];
  struct message_type *messages;
  int i,j,nr,max_hit,exp;
  struct room_data	*rp;
  
  int hit_limit(struct char_data *ch);
  
  assert(GET_POS(victim) > POSITION_DEAD);
  
  rp = real_roomp(ch->in_room);
  if (rp && rp->room_flags&PEACEFUL &&
      attacktype!=SPELL_POISON /* poison is allowed */
      ) {
    char	buf[MAX_INPUT_LENGTH];
    sprintf(buf, "damage(,,,%d) called in PEACEFUL room", attacktype);
    log(buf);
    return;
  }
  
  dam = SkipImmortals(victim, dam);
  if (dam == -1)
    return(FALSE);
  
  if (ch->in_room != victim->in_room)
    return(FALSE);
  
  if (victim != ch) {
    if (GET_POS(victim) > POSITION_STUNNED) {
      if (!(victim->specials.fighting))
	if ((!IS_NPC(ch))||(!IS_SET(ch->specials.act, ACT_IMMORTAL))) {
	  if (ch->attackers < 6) {
	    set_fighting(victim, ch);
	    GET_POS(victim) = POSITION_FIGHTING;
	  }
	} else {
	  return(FALSE);
	}
    }
  }
  
  if (victim != ch) {
    if (GET_POS(ch) > POSITION_STUNNED) {	
      if (!(ch->specials.fighting))
	if ((!IS_NPC(ch))||(!IS_SET(ch->specials.act, ACT_IMMORTAL))) {
	  set_fighting(ch, victim);
	  GET_POS(ch) = POSITION_FIGHTING;
	} else {
	  return(FALSE);
	}
    }
  }
  
  if (victim->master == ch)
    stop_follower(victim);
  
  if (IS_AFFECTED(ch, AFF_INVISIBLE) || IS_AFFECTED2()
    appear(ch);

  if (IS_AFFECTED(ch, AFF_SNEAK)) {
    affect_from_char(ch, SKILL_SNEAK);
  }
  
  if (IS_AFFECTED(victim, AFF_SANCTUARY))
    dam = MAX((int)(dam/2), 0);  /* Max 1/2 damage when sanct'd */
  
  dam = PreProcDam(victim, attacktype,dam);
  
  dam = WeaponCheck(ch, victim, attacktype, dam);

  DamageStuff(victim, attacktype, dam);
  
  dam=MAX(dam,0);
  
  /*
   *  check if this hit will send the victim over the edge
   */
  if (GET_HIT(victim)-dam < 1) {
    if (IS_AFFECTED(victim, AFF_LIFE_PROT)) {
      BreakLifeSaverObj(victim);
      dam = 0;
      REMOVE_BIT(ch->specials.affected_by, AFF_LIFE_PROT);
    }
  }

  
  GET_HIT(victim)-=dam;  

  if (IS_AFFECTED(victim, AFF_FIRESHIELD)&&!IS_AFFECTED(ch, AFF_FIRESHIELD)) {
    if (damage(victim, ch, dam, SPELL_FIREBALL)) {
      update_pos(victim);
      if (GET_POS(victim) != POSITION_DEAD)
        return(FALSE);
      else
        return(TRUE);
    }
  }
  update_pos(victim);
  
  if ((attacktype >= TYPE_HIT) && (attacktype <= TYPE_SMITE)) {
    if (!ch->equipment[WIELD]) {
      dam_message(dam, ch, victim, TYPE_HIT);
    } else {
      dam_message(dam, ch, victim, attacktype);
  /*
   *  check if attacker's weapon is brittle
   */
      BrittleCheck(ch, dam);
  
    }
  } else {
    
    for(i = 0; i < MAX_MESSAGES; i++) {
      if (fight_messages[i].a_type == attacktype) {
	nr=dice(1,fight_messages[i].number_of_attacks);
	for(j=1,messages=fight_messages[i].msg;(j<nr)&&(messages);j++)
	  messages=messages->next;
	
	if (!IS_NPC(victim) && (GetMaxLevel(victim) > MAX_MORT)){
	  act(messages->god_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
	  act(messages->god_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
	  act(messages->god_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	} else if (dam != 0) {
	  if (GET_POS(victim) == POSITION_DEAD) {
	    act(messages->die_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
	    act(messages->die_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
	    act(messages->die_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	  } else {
	    act(messages->hit_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
	    act(messages->hit_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
	    act(messages->hit_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	  }
	} else { /* Dam == 0 */
	  act(messages->miss_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
	  act(messages->miss_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
	  act(messages->miss_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	}
      }
    }
  }
  switch (GET_POS(victim)) {
  case POSITION_MORTALLYW:
    act("$n is mortally wounded, and will die soon, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
    act("You are mortally wounded, and will die soon, if not aided.", FALSE, victim, 0, 0, TO_CHAR);
    break;
  case POSITION_INCAP:
    act("$n is incapacitated and will slowly die, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
    act("You are incapacitated and you will slowly die, if not aided.", FALSE, victim, 0, 0, TO_CHAR);
    break;
  case POSITION_STUNNED:
    act("$n is stunned, but will probably regain consciousness again.", TRUE, victim, 0, 0, TO_ROOM);
    act("You're stunned, but you will probably regain consciousness again.", FALSE, victim, 0, 0, TO_CHAR);
    break;
  case POSITION_DEAD:
    act("$n is dead! R.I.P.", TRUE, victim, 0, 0, TO_ROOM);
    act("You are dead!  Sorry...", FALSE, victim, 0, 0, TO_CHAR);
    break;
    
  default:  /* >= POSITION SLEEPING */
    
    max_hit=hit_limit(victim);
    
    if (dam > (max_hit/5))
      act("That Really HURT!",FALSE, victim, 0, 0, TO_CHAR);
    
    if (GET_HIT(victim) < (max_hit/5) && GET_HIT(victim) > 0) {
      
      act("You wish that your wounds would stop BLEEDING so much!",FALSE,victim,0,0,TO_CHAR);
      if (IS_NPC(victim)) {
	if (IS_SET(victim->specials.act, ACT_WIMPY))
	  do_flee(victim, "", 0);
      } else {
	if (IS_SET(victim->specials.act, PLR_WIMPY))
	  do_flee(victim, "", 0);
      }
      
    }
    break;
  }
  
  if (IS_PC(victim) && !(victim->desc)) {
    do_flee(victim, "", 0);
    act("$n is rescued by divine forces.", FALSE, victim, 0, 0, TO_ROOM);
    update_pos(victim);
  }
  
  if (GET_POS(victim) == POSITION_DEAD) {
    if (ch->specials.fighting == victim)
      stop_fighting(ch);
  }
  
  if (!AWAKE(victim))
    if (victim->specials.fighting)
      stop_fighting(victim);
  
  if (GET_POS(victim) == POSITION_DEAD) {
    if (IS_NPC(victim) || victim->desc)
      if (IS_AFFECTED(ch, AFF_GROUP)) {
	group_gain(ch, victim);
      } else {
	/* Calculate level-difference bonus */
	exp = GET_EXP(victim);

	exp = RatioExp(ch, victim, exp);
	
	exp = MAX(exp, 1);
	exp = MIN(exp, 100000);

	if (!IS_PC(victim)) {
	  gain_exp(ch, exp);
	}
	change_alignment(ch, victim);
      }
    if (!IS_NPC(victim)) {
      if (victim->in_room > -1) {
	sprintf(buf, "%s killed by %s at %s",
		GET_NAME(victim),
		(IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)),
		(real_roomp(victim->in_room))->name);
      } else {
	sprintf(buf, "%s killed by %s at Nowhere.",
		GET_NAME(victim),
		(IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
      }
      log(buf);
    }
    die(victim);
    /*
     *  if the victim is dead, return TRUE.
     */
    victim = 0;
    return(TRUE);
  } else {
    return(FALSE);
  }
}

#endif

int GetWeaponType(struct char_data *ch, struct obj_data **wielded) 
{
  int w_type;

  if (ch->equipment[WIELD] &&
      (ch->equipment[WIELD]->obj_flags.type_flag == ITEM_WEAPON)) {

    *wielded = ch->equipment[WIELD];
    w_type = Getw_type(*wielded);
  } else if(HasClass(ch, CLASS_MONK)) {
    w_type = GetMonkw_type(ch);
  } else {
    if (IS_NPC(ch) && (ch->specials.attack_type >= TYPE_HIT))
      w_type = ch->specials.attack_type;
    else
      w_type = TYPE_HIT;

    *wielded = 0;  /* no weapon */

  }
  return(w_type);

}

int GetMonkw_type(struct char_data *ch)
{
  int wtype;

  switch(number(1,10) + (int) (GET_LEVEL(ch,MONK_LEVEL_IND) / 10)) {
  case 0 :
  case 1 :
  case 2 :
    wtype = TYPE_HIT; break;
  case 3 :
  case 4 :
    wtype = TYPE_BITE; break;
  case 5 :
  case 6 :
    wtype = TYPE_BLUDGEON; break;
  case 7 :
  case 8 :
    wtype = TYPE_SMITE; break;
  case 9 :
  case 10:
  case 11:
    wtype = TYPE_SMASH; break;
  case 12:
  case 13:
    wtype = TYPE_CRUSH; break;
  case 14:
  case 15:
  default:
    wtype = TYPE_BLAST; break;
  }

  return(wtype);
}


int Getw_type(struct obj_data *wielded) 
{
  int w_type;

  switch (wielded->obj_flags.value[3]) {
    case 0  : w_type = TYPE_SMITE; break;
    case 1  : w_type = TYPE_STAB;  break;
    case 2  : w_type = TYPE_WHIP; break;
    case 3  : w_type = TYPE_SLASH; break;
    case 4  : w_type = TYPE_SMASH; break;
    case 5  : w_type = TYPE_CLEAVE; break;
    case 6  : w_type = TYPE_CRUSH; break;
    case 7  : w_type = TYPE_BLUDGEON; break;
    case 8  : w_type = TYPE_CLAW; break;
    case 9  : w_type = TYPE_BITE; break;
    case 10 : w_type = TYPE_STING; break;
    case 11 : w_type = TYPE_PIERCE; break;
    case 12 : w_type = TYPE_BLAST; break;
    default : w_type = TYPE_HIT; break;
  }
  return(w_type);
}

int HitCheckDeny(struct char_data *ch, struct char_data *victim, int type)
{
  struct room_data *rp;
  char buf[256];
  extern char PeacefulWorks;

  rp = real_roomp(ch->in_room);
  if (rp && rp->room_flags&PEACEFUL && PeacefulWorks) {
    sprintf(buf, "hit() called in PEACEFUL room");
    logE(buf);
    stop_fighting(ch);
    return(TRUE);
  }
  
  if (ch->in_room != victim->in_room) {
    sprintf(buf, "NOT in same room when fighting : %s, %s", ch->player.name, victim->player.name);
    logE(buf);
    stop_fighting(ch);
    return(TRUE);
  }

  if(!IS_AFFECTED2(ch, AFF2_BERSERK)) {
    if (GET_RACE(ch) != RACE_OGRE && GET_MOVE(ch) < -10) {
      send_to_char("You're too exhausted to fight.\n\r",ch);
      stop_fighting(ch);
      return(TRUE);
    } else if ( GET_MOVE(ch) < -30) {
      send_to_char("You're too exhausted to fight.\n\r",ch);
      stop_fighting(ch);
      return(TRUE);
    }
  }
  
  if (victim->attackers >= 6 && ch->specials.fighting != victim) {
    send_to_char("You can't attack them,  no room!\n\r", ch);
    return(TRUE);
  }

/*
   if the character is already fighting several opponents, and he wants
   to hit someone who is not currently attacking him, then deny them.
   if he is already attacking that person, he can continue, even if they
   stop fighting him.
*/  
  if ((ch->attackers >= 6) && (victim->specials.fighting != ch) &&
      ch->specials.fighting != victim) {
    send_to_char("There are too many other people in the way.\n\r", ch);
    return(TRUE);
  }

  if (!IS_PC(ch)) {
    if (ch->specials.fighting && IS_PC(ch->specials.fighting) &&
	!ch->specials.fighting->desc) {
      do_flee(ch,"\0",0);
/*
    stop_fighting(ch);
*/
      return(TRUE);
    }
  }

  if (IS_PC(ch) && !ch->desc) {
    return(TRUE);
  }

  if (victim == ch) {
    if (Hates(ch,victim)) {
      RemHated(ch, victim);
    }
    return(TRUE);
  }

  if (GET_POS(victim) == POSITION_DEAD)
    return(TRUE);

  if (MOUNTED(ch)) {
    if (!RideCheck(ch, -5)) {
      FallOffMount(ch, MOUNTED(ch));
      Dismount(ch, MOUNTED(ch), POSITION_SITTING);
      return(TRUE);
    }
  } else {
    if (RIDDEN(ch)) {
      if (!RideCheck(RIDDEN(ch),-10)) {
	FallOffMount(RIDDEN(ch), ch);
	Dismount(RIDDEN(ch), ch, POSITION_SITTING);
	return(TRUE);
      }
    }
  }
  
#if 0
  if (!IS_PC(ch) && !IS_PC(victim)) {

    if (affected_by_spell(ch, SPELL_CHARM_PERSON) ||
	!IS_AFFECTED(ch, AFF_CHARM)) {

      if (IS_SET(ch->specials.act, ACT_SENTINEL))
	return(FALSE);

      if (!Hates(ch, victim)) {
	/*
	  they may flee
	  */
	diff = GET_ALIGNMENT(ch) - GET_ALIGNMENT(victim);
	diff /= 10;
	if (diff < 0)
	  diff *= -1;
	
	if (diff < number(0,50)) {
	  do_flee(ch, "\0", 0);
	  return(TRUE);
	} else {
	  if (victim->specials.fighting == ch) {
	    int vsum, csum;
	    float perc;

	    vsum = GetSumRaceMaxLevInRoom(victim);
	    csum = GetSumRaceMaxLevInRoom(ch);

	    /*
	      ch/vict - percentage of total levels
	      */
	    perc = csum / vsum;
	    perc *= 100;   /* multiply by 100 so integers can be included */
	    
	    
	    if (IS_SET(ch->specials.act, ACT_AGGRESSIVE))
	      perc += (float)2*number(1, GetMaxLevel(ch));
	    else 
	      perc += (float)number(1, GetMaxLevel(ch));
	    
	    if (perc < 90.0) {
	      do_flee(ch, "\0", 0);
	      return(TRUE);
	    } 
	  }
	}
      }
    }
  } else if (!IS_PC(ch)) {
    int vsum, csum;
    float perc;


    if (IS_SET(ch->specials.act, ACT_SENTINEL))
      return(FALSE);

    if (affected_by_spell(ch, SPELL_CHARM_PERSON) ||
	!IS_AFFECTED(ch, AFF_CHARM)) {
      
      vsum = GetSumRaceMaxLevInRoom(victim);
      csum = GetSumRaceMaxLevInRoom(ch);

/*
  ch/vict - percentage of total levels
*/
      perc = csum / vsum;
      perc *= 100;   /* multiply by 100 so integers can be included */


      if (IS_SET(ch->specials.act, ACT_AGGRESSIVE))
	perc += (float)2*number(1, GetMaxLevel(ch));
      else 
	perc += (float)number(1, GetMaxLevel(ch));

      if (perc < 90.0) {
	do_flee(ch, "\0", 0);
	return(TRUE);
      } 
    }
  }
#endif
  return(FALSE);

}

int CalcThaco(struct char_data *ch)
{  
  int calc_thaco;
  extern struct str_app_type str_app[];
  extern int thaco[4][ABS_MAX_LVL];

  
  /* Calculate the raw armor including magic armor */
  /* The lower AC, the better                      */
  
  if (!IS_NPC(ch))
    calc_thaco = thaco[BestFightingClass(ch)][GET_LEVEL(ch, BestFightingClass(ch))];
  else
    /* THAC0 for monsters is set in the HitRoll */
    calc_thaco = 20;
  
  calc_thaco -= str_app[STRENGTH_APPLY_INDEX(ch)].tohit;
  calc_thaco -= GET_HITROLL(ch);
  calc_thaco += GET_COND(ch, DRUNK)/5;
  return(calc_thaco);
}

int HitOrMiss(struct char_data *ch, struct char_data *victim, int calc_thaco) 
{
  int diceroll, victim_ac;

  extern struct dex_app_type dex_app[];

  diceroll = number(1,20);
  
  /* nifty new dwarf attack stuff. */

  if( (GET_RACE(ch) == RACE_DWARF || GET_RACE(ch) == RACE_GNOME) && 
     IsGoblinoid(victim))
    diceroll++;

  if(GET_RACE(victim) == RACE_DWARF && IsGiant(ch))
    diceroll -= 4;

  victim_ac  = GET_AC(victim)/10;
  
  if (!AWAKE(victim))
    victim_ac -= dex_app[GET_DEX(victim)].defensive;

  if (GET_POS(victim) <= POSITION_INCAP)
    return(TRUE);

  if (GET_POS(victim) == POSITION_STUNNED)
    victim_ac += 3;  /* +3 to hit stunned */
  if (GET_POS(victim) <= POSITION_SITTING)
    victim_ac += 2;

  if (GET_POS(ch) < POSITION_FIGHTING)
    victim_ac -= 4;
  
  victim_ac = MAX(-10, victim_ac);  /* -10 is lowest */
  victim_ac = MIN(10, victim_ac);  /* 10 is the highest */

  if ((diceroll < 20) && AWAKE(victim) &&
      ((diceroll==1) || ((calc_thaco-diceroll) > victim_ac))) {
    return(FALSE);
  } else {
    return(TRUE);
  }
}

int MissVictim(struct char_data *ch, struct char_data *v, int type, int w_type,
	       int (*dam_func)())
{
  struct obj_data *o;

  if (type <= 0) type = w_type;
  if (dam_func == MissileDamage) {
    if (ch->equipment[WIELD]) {
      o = unequip_char(ch, WIELD);
      if (o) {
	act("$p falls to the ground harmlessly", FALSE, ch, o, 0, TO_CHAR);
	act("$p falls to the ground harmlessly", FALSE, ch, o, 0, TO_ROOM);
	obj_to_room(o, ch->in_room);
      }
    }
  }
  (*dam_func)(ch, v, 0, w_type);
}


int GetWeaponDam(struct char_data *ch, struct char_data *v, 
		 struct obj_data *wielded)
{
   int dam, j;
   struct obj_data *obj;
   extern struct str_app_type str_app[];

    
    dam  = str_app[STRENGTH_APPLY_INDEX(ch)].todam;
    dam += GET_DAMROLL(ch);
    
    if (!wielded) {
      if (IS_NPC(ch) || HasClass(ch, CLASS_MONK))
	dam += dice(ch->specials.damnodice, ch->specials.damsizedice);
      else
	dam += number(0,2);  /* Max. 2 dam with bare hands */
    } else { 
      if (wielded->obj_flags.value[2] > 0) {

	if (wielded->obj_flags.value[0] == 2) {
	  if (GET_HEIGHT(v) < 250) {
	    dam += dice(wielded->obj_flags.value[1],wielded->obj_flags.value[2]/2);
#if 0
	    send_to_char("Your weapon seems to be less effective against non-giant-sized opponents\n", ch);
#endif
	  } else {
	    dam += dice(wielded->obj_flags.value[1],wielded->obj_flags.value[2]);
	  }
	} else {
	  dam += dice(wielded->obj_flags.value[1],wielded->obj_flags.value[2]);
	}
      } else {
        act("$p snaps into peices!", TRUE, ch, wielded, 0, TO_CHAR);
	act("$p snaps into peices!", TRUE, ch, wielded, 0, TO_ROOM);
	if ((obj = unequip_char(ch, WIELD))!=NULL) {
	  MakeScrap(ch, obj);
	  dam += 1;
	}
      }
      if (wielded->obj_flags.weight > str_app[GET_STR(ch)].wield_w) {
	if (ch->equipment[HOLD]) {
	  /*
	    its too heavy to wield properly
	    */
	  dam /= 2;
	}
      }

      /*
	check for the various APPLY_RACE_SLAYER and APPLY_ALIGN_SLAYR
	 here.
       */
#ifdef APPLY_RACE_SLAYER
      for(j=0; j<MAX_OBJ_AFFECT; j++) {
	if (wielded->affected[j].location ==
	    APPLY_RACE_SLAYER) {
	  if (wielded->affected[j].modifier == GET_RACE(v))
	    dam *= 2;
	}
	if (wielded->affected[j].location ==
	    APPLY_ALIGN_SLAYER) {
	  if (((int)wielded->affected[j].modifier) > 0)
	    if (IS_GOOD(v))
	      dam *= 2;
	  else if (((int)wielded->affected[j].modifier) < 0)
	    if (IS_EVIL(v))
	      dam *= 2;
	  else
	    if (!IS_GOOD(v) && !IS_EVIL(v))
	      dam *=2;
	}
      }
#endif
    }
    
    if (GET_POS(v) < POSITION_FIGHTING)
      dam *= 1+(POSITION_FIGHTING-GET_POS(v))/3;
    /* Position  sitting  x 1.33 */
    /* Position  resting  x 1.66 */
    /* Position  sleeping x 2.00 */
    /* Position  stunned  x 2.33 */
    /* Position  incap    x 2.66 */
    /* Position  mortally x 3.00 */
    
    if (GET_POS(v) <= POSITION_DEAD)
      return(0);
    
    dam = MAX(1, dam);  /* Not less than 0 damage */
 
    return(dam);
    
}

#if 0
int LoreBackstabBonus(struct char_data *ch, struct char_data *v)
{
  int mult = 0;
  int learn=0;

  if(!ch->skills);
  return(0);

  if (IsAnimal(v) && ch->skills[SKILL_CONS_ANIMAL].learned) {
    learn = ch->skills[SKILL_CONS_ANIMAL].learned;
  }
  if (IsVeggie(v) && ch->skills[SKILL_CONS_VEGGIE].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_VEGGIE].learned);
  }
  if (IsDiabolic(v) && ch->skills[SKILL_CONS_DEMON].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_DEMON].learned);
  }
  if (IsReptile(v) && ch->skills[SKILL_CONS_REPTILE].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_REPTILE].learned);
  }
  if (IsUndead(v) && ch->skills[SKILL_CONS_UNDEAD].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_UNDEAD].learned);
  }  
  if (IsGiantish(v)&& ch->skills[SKILL_CONS_GIANT].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_GIANT].learned);
  }
  if (IsPerson(v) && ch->skills[SKILL_CONS_PEOPLE].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_PEOPLE].learned);
  }
  if (IsOther(v)&& ch->skills[SKILL_CONS_OTHER].learned) {
    learn = MAX(learn, ch->skills[SKILL_CONS_OTHER].learned/2);
  }

  if (learn > 40)  
    mult += 1;
  if (learn > 74)
    mult += 1;

  if (mult > 0) 
    send_to_char("Your lore aids your attack!\n\r", ch);

  return(mult);
}
#endif

int GetBackstabMult(struct char_data *ch, struct char_data *v)
{
  int mult, our_skill;
  char buf[80];

  if (GET_LEVEL(ch, THIEF_LEVEL_IND)) {
    mult = backstab_mult[GET_LEVEL(ch, THIEF_LEVEL_IND)];
  } else {
    mult = backstab_mult[GetMaxLevel(ch)];
  }
  if(!ch->skills)
    return(mult);

  switch(GET_RACE(v)) {
  case RACE_HALFBREED:
  case RACE_HUMAN:
  case RACE_ELVEN:
  case RACE_DWARF:
  case RACE_HALFLING:
  case RACE_GNOME:
  case RACE_LYCANTH:
  case RACE_ORC:
  case RACE_GOBLIN:
  case RACE_MFLAYER:
  case RACE_PRIMATE:
  case RACE_ENFAN:
  case RACE_DROW:
  case RACE_SMURF:
  case RACE_TROGMAN:
  case RACE_HALFELF:
  case RACE_HALFORC:
  case RACE_HUMANTWO:
  case RACE_VAMPIRE:
  case RACE_OGRE:
  case RACE_FAERIE:
  case RACE_PATRYN:
  case RACE_SARTAN:
  case RACE_DRAAGDIM:
    our_skill = SKILL_CONS_PEOPLE;
    break;

  case RACE_GIANT:
  case RACE_TROLL:
  case RACE_TYTAN:
    our_skill = SKILL_CONS_GIANT;
    break;

  case RACE_PREDATOR:
  case RACE_HERBIV:
  case RACE_FISH:
  case RACE_ROO:
  case RACE_LABRAT:
  case RACE_PARASITE:
  case RACE_HORSE:
    our_skill = SKILL_CONS_ANIMAL;
    break;

  case RACE_VEGGIE:
  case RACE_VEGMAN:
  case RACE_TREE:
    our_skill = SKILL_CONS_VEGGIE;
    break;

  case RACE_DEMON:
  case RACE_DEVIL:
    our_skill = SKILL_CONS_DEMON;
    break;

  case RACE_INSECT:
  case RACE_ARACHNID:
    our_skill = SKILL_CONS_INSECT;
    break;

  case RACE_BIRD:
  case RACE_SKEXIE:
    our_skill = SKILL_CONS_AVIAN;
    break;

  case RACE_UNDEAD:
  case RACE_GHOST:
    our_skill = SKILL_CONS_UNDEAD;
    break;

  case RACE_REPTILE: 
  case RACE_DINOSAUR:
  case RACE_SNAKE:
    our_skill = SKILL_CONS_REPTILE;
    break;

  case RACE_GOD:
  case RACE_SPECIAL:
  case RACE_SLIME:
  case RACE_ELEMENT:
  case RACE_ASTRAL:
  case RACE_PLANAR:
  case RACE_GOLEM:
    our_skill = SKILL_CONS_OTHER;
    break;
    
  default:
    our_skill = 0;
    break;
  }

  if(!our_skill) {
    sprintf(buf, "Warning, race %d was unaccounted for in GetBackstabMult()",
	    GET_RACE(v));
    logE(buf);
    return(mult);
  }

  /* for learned = 0, mult *= .05, for learned = 95, mult *= 1.00 */
  /*  */
  mult *= MAX((ch->skills[our_skill].learned +10), 100)/100;

  if(GET_RACE(v) == RACE_UNDEAD || GET_RACE(v) == RACE_GHOST ||
	 GET_RACE(v) == RACE_SLIME)
    mult = 1;

  if(IS_SET(v->specials.act, ACT_HUGE))
    mult >>= 2;

  mult= MAX(mult,1);

  if(OnlyClass(ch, CLASS_THIEF)) {

    if(ch->skills[our_skill].learned > 70)
      mult++;

    if(ch->skills[our_skill].learned > 90)
      mult++;
  }
  return(mult);
}

int HitVictim(struct char_data *ch, struct char_data *v, int dam, 
		   int type, int w_type, int (*dam_func)())
{
  char buf[80];
  extern byte backstab_mult[];
  int dead;

    if (type == SKILL_BACKSTAB) {
      int tmp;

      tmp = GetBackstabMult(ch, v);
      sprintf(buf, "BS multiplier for %dth level char is %d.", GetMaxLevel(ch),
	      tmp);
      logE(buf);
      dam *= tmp;
      dead = (*dam_func)(ch, v, dam, type);

    } else {
/*
  reduce damage for dodge skill:
*/
#if SKILL_DODGE
      if (v->skills && v->skills[SKILL_DODGE].learned) {
	if (number(1,101) <= v->skills[SKILL_DODGE].learned) {
	  dam -= number(1,3);
	  if (HasClass(v, CLASS_MONK)) {
	    MonkDodge(ch, v, &dam);
	  }
	}
      }
#endif
      if( (GET_RACE(ch) == RACE_DWARF || GET_RACE(ch) == RACE_GNOME) && 
	 IsGoblinoid(v))
	dam++;

       dead = (*dam_func)(ch, v, dam, w_type);
    }
      
      /*
       *  if the victim survives, lets hit him with a 
       *  weapon spell
       */
      
    if (!dead) {
	WeaponSpell(ch,v,w_type);
    
    }
}


void root_hit(struct char_data *ch, struct char_data *victim, int type, 
	      int (*dam_func)())
{
  int w_type, thaco, dam;
  struct obj_data *wielded=0;  /* this is rather important. */

  if (HitCheckDeny(ch, victim, type)) return;

  GET_MOVE(ch) -= 1;

  w_type = GetWeaponType(ch, &wielded);
  if (w_type == TYPE_HIT)
    w_type = GetFormType(ch);  /* races have different types of attack */

  thaco = CalcThaco(ch);

  if (HitOrMiss(ch, victim, thaco)) {
    if ((dam = GetWeaponDam(ch, victim, wielded)) > 0) {
       HitVictim(ch, victim, dam, type, w_type, dam_func);
    } else {
       MissVictim(ch, victim, type, w_type, dam_func);
    }
  } else {
    MissVictim(ch, victim, type, w_type, dam_func);
  }

}

void MissileHit(struct char_data *ch, struct char_data *victim, int type)
{
  root_hit(ch, victim, type, MissileDamage); 
}

void hit(struct char_data *ch, struct char_data *victim, int type)
{
  root_hit(ch, victim, type, damage); 
}


float WoundWearyness(struct char_data *ch)
{
  float zap=0.000;

  if (IS_PC(ch)) {
    if (GET_HIT(ch) < GET_MAX_HIT(ch)/5)
      zap = .333;
    if (GET_HIT(ch) < GET_MAX_HIT(ch)/10)
      zap = .666;
  } else {
    if (GET_HIT(ch) < GET_MAX_HIT(ch)/5 || GET_HIT(ch) <= 8)
      zap = .333;
    if (GET_HIT(ch) < GET_MAX_HIT(ch)/10 || GET_HIT(ch) <= 4)
      zap = .666;
  }


  if (zap > .001) {
    if(GET_RACE(ch) == RACE_OGRE) {
      send_to_char("Aaarrrggghhh, this is beginning to hurt! Now you're mad!\n\r",ch);
      act("$n is beginning to look really pissed.",FALSE, ch, 0, 0, TO_ROOM);
        return(-zap);
    } else {
      send_to_char("Your wounds throb painfully, weakening your attack\n\r",
		   ch);
      return(zap);
    }
  }
  return(0.000);
}


/* control the fights going on */
void perform_violence(int pulse)
{
  struct char_data *ch, *vict;
  struct obj_data *tmp,*tmp2;
  float x;
  int perc;
  
  for (ch = combat_list; ch; ch=combat_next_dude)	{
    struct room_data *rp;
    
    combat_next_dude = ch->next_fighting;
    assert(ch->specials.fighting);
    
    rp = real_roomp(ch->in_room);
    if (rp && rp->room_flags&PEACEFUL) {
      char	buf[MAX_INPUT_LENGTH];
      sprintf(buf,"perform_violence() found %s fighting in a PEACEFUL room.",
	      ch->player.name);
      stop_fighting(ch);
      logE(buf);
    } else if (ch == ch->specials.fighting) {
      stop_fighting(ch);
    } else {

      if(IS_SET(ch->specials.affected_by2, AFF2_FEEDING)) {
	if(ch->in_room != ch->specials.fighting->in_room) {
	  stop_fighting(ch);
	  continue;
	} else {
	  if( ch->attackers > 1) {
	    REMOVE_BIT(ch->specials.affected_by2, AFF2_FEEDING);
	    ch->specials.fighting->specials.bitten = FALSE;
	    send_to_char("You can no longer feed with all these people fighting you.\n\r", ch->specials.fighting);
	  } else if (ch->specials.fighting->attackers > 1) {
	    REMOVE_BIT(ch->specials.affected_by2, AFF2_FEEDING);
	    ch->specials.fighting->specials.bitten = FALSE;
	    send_to_char("How can you eat when things are attacking your food?\n\r", ch);
	  }
	}
      }

      if(IS_AFFECTED2(ch, AFF2_FEEDING)) {
	VampireBite(ch, ch->specials.fighting);
	continue;
      }
      
      if(ch->specials.bitten) {
	continue;		/* bitten victims do not fight */
      }
      
      if (IS_NPC(ch)) {
	struct char_data *rec;
	DevelopHatred(ch, ch->specials.fighting);
	rec = ch->specials.fighting;
	if (!IS_PC(ch->specials.fighting)) {
	  while (rec->master) {
	    if (rec->master->in_room == ch->in_room) {
	      AddHated(ch, rec->master);
	      rec = rec->master;
	    } else {
	      break;
	    }
	  }
	}
      }
      
      if (AWAKE(ch) && (ch->in_room==ch->specials.fighting->in_room) &&
	  (!IS_AFFECTED(ch, AFF_PARALYSIS))) {
	
	if (!IS_NPC(ch)) {

	  /* set x = # of attacks */
	  x = ch->mult_att;

	  /* if dude is a monk, and is wielding something */

	  if (HasClass(ch, CLASS_MONK)) {
	    if (ch->equipment[WIELD]) {
	      /* set it to one, they only get one attack */
	      x = 1.000;
	    }
	  }

	  if (MOUNTED(ch)) {
	      x /= 2.0;
	  }

	  
	  /* heavy woundage = fewer attacks */
	  x -= WoundWearyness(ch);
	  
	  /* work through all of their attacks, until there is not
	     a full attack left */
	  
	  tmp = 0;
	  tmp2 = 0;
	  
	  if (DUAL_WIELD(ch)) {
	    tmp = unequip_char(ch, HOLD);
	  }
	  
	  /* have to check for monks holding things. */
	  if(ch->equipment[HOLD] && !(ch->equipment[WIELD]) &&
	     ITEM_TYPE(ch->equipment[HOLD])==ITEM_WEAPON && 
	     HasClass(ch, CLASS_MONK)) {
	    tmp2 = unequip_char(ch, HOLD);
	  }
	  
	  while (x > 0.999) {
	    if (ch->specials.fighting)
	      hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
	    else {
	      x = 0.0;
	      break;
	    }
	    x -= 1.0;
	  }
	  
	  if(GET_RACE(ch) == RACE_MFLAYER && ch->specials.fighting)
	    MindflayerAttack(ch, ch->specials.fighting);
	
	  if (x > .01) {

	    /* check to see if the chance to make the last attack
	       is successful 
	       */

	    perc = number(1,100);
	    if (perc <= (int)(x*100.0)) {
	      if (ch->specials.fighting)
		hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
	    }
	  }

	  if (tmp)
	    equip_char(ch, tmp, HOLD);
	  if(tmp2)
	    equip_char(ch, tmp2, HOLD);


#if 1
	  
	  /* check for the second attack */
	  if (DUAL_WIELD(ch) && ch->skills) {
	    struct obj_data *weapon;
	    int perc;
	    /* check the skill */
	    if ((perc = number(1,101)) < 
		ch->skills[SKILL_DUAL_WIELD].learned){
	    /* if a success, remove the weapon in the wielded hand,
	       place the weapon in the off hand in the wielded hand.
	     */
	      weapon = unequip_char(ch, WIELD);
	      tmp = unequip_char(ch, HOLD);
	      equip_char(ch, tmp, WIELD);
	      /* adjust to_hit based on dex */
	      if (ch->specials.fighting) {
		hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
	      }
	      /* get rid of the to_hit adjustment */
	      /* put the weapons back, checking for destroyed items */
	      if (ch->equipment[WIELD]) {
		tmp = unequip_char(ch, WIELD);
		equip_char(ch, tmp, HOLD);
		equip_char(ch, weapon, WIELD);
	      }
	    } else {
	      if (number(1,20) > GET_DEX(ch)) {
		tmp = unequip_char(ch, HOLD);
		obj_to_room(tmp, ch->in_room);
		act("You fumble and drop $p", 0, ch, tmp, tmp, TO_CHAR);
		act("$n fumbles and drops $p", 0, ch, tmp, tmp, TO_ROOM);
		if (number(1,20) > GET_DEX(ch)) {
		  tmp = unequip_char(ch, WIELD);
		  obj_to_room(tmp, ch->in_room);
		  act("and you fumble and drop $p too!", 
		      0, ch, tmp, tmp, TO_CHAR);
		  act("and then fumbles and drops $p as well!", 
		      0, ch, tmp, tmp, TO_ROOM);
		}
	      }
	    }
	  }
#endif
	} else {
	  x = ch->mult_att;
	  
	  while (x > 0.999) {
	    if (ch->specials.fighting)
	      hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
	    else {
	      if ((vict = FindAHatee(ch)) != NULL) {
		if (vict->attackers < 6)
		  hit(ch, vict, TYPE_UNDEFINED);
	      } else if ((vict = FindAnAttacker(ch)) != NULL) {
		if (vict->attackers < 6)
		  hit(ch, vict, TYPE_UNDEFINED);
	      }
	    }
	    x -= 1.0;
	  }
	  if(GET_RACE(ch) == RACE_MFLAYER && ch->specials.fighting)
	    MindflayerAttack(ch, ch->specials.fighting);

	  if (x > .01) {

	    /* check to see if the chance to make the last attack
	       is successful 
	       */

	    perc = number(1,100);
	    if (perc <= (int)(x*100.0)) {
	      if (ch->specials.fighting) {
		hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
	      } else {
		if ((vict = FindAHatee(ch)) != NULL) {
		  if (vict->attackers < 6)
		    hit(ch, vict, TYPE_UNDEFINED);
		} else if ((vict = FindAnAttacker(ch)) != NULL) {
		  if (vict->attackers < 6)
		    hit(ch, vict, TYPE_UNDEFINED);
		}
	      }
	    }
	  }
	}
      } else { /* Not in same room or not awake */
	stop_fighting(ch);
      }
    }
  }
}


struct char_data *FindVictim( struct char_data *ch)
{
  struct char_data *tmp_ch;
  unsigned char found=FALSE;
  unsigned short ftot=0,ttot=0,ctot=0,ntot=0,mtot=0, ktot=0, dtot=0;
  unsigned short total;
  unsigned short fjump=0,njump=0,cjump=0,mjump=0,tjump=0,kjump=0,djump=0;
  
  if (ch->in_room < 0) return(0);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;
       tmp_ch=tmp_ch->next_in_room) {
    if ((CAN_SEE(ch,tmp_ch))&&(!IS_SET(tmp_ch->specials.act,PLR_NOHASSLE))&&
	(!IS_AFFECTED(tmp_ch, AFF_SNEAK)) && (ch!=tmp_ch)) {
      if (!IS_SET(ch->specials.act, ACT_WIMPY) || !AWAKE(tmp_ch)) {
	if ((tmp_ch->specials.zone != ch->specials.zone &&
	    !strchr(zone_table[ch->specials.zone].races, GET_RACE(tmp_ch))) ||
	    IS_SET(tmp_ch->specials.act, ACT_ANNOYING)) {
/*
	if (!IS_NPC(tmp_ch)||(IS_SET(tmp_ch->specials.act, ACT_ANNOYING))) {
*/
	  if (!in_group(ch, tmp_ch)) {
	    found = TRUE;  /* a potential victim has been found */ 
	    if (!IS_NPC(tmp_ch)) {
	      if(HasClass(tmp_ch, CLASS_WARRIOR))
		ftot++;
	      else if (HasClass(tmp_ch, CLASS_CLERIC))
		ctot++;
	      else if (HasClass(tmp_ch,CLASS_MAGIC_USER))
		mtot++;
	      else if (HasClass(tmp_ch, CLASS_THIEF))
		ttot++;
	      else if (HasClass(tmp_ch, CLASS_DRUID)) 
		dtot++;
	      else if (HasClass(tmp_ch, CLASS_MONK)) 
		ktot++;
	    } else {
	      ntot++;
	    }
	  }
	}
      }
    }
  }
  
  /* if no legal enemies have been found, return 0 */
  
  if (!found) {
    return(0);
  }
  
  /* 
    give higher priority to fighters, clerics, thieves,magic users if int <= 12
    give higher priority to fighters, clerics, magic users thieves is inv > 12
    give higher priority to magic users, fighters, clerics, thieves if int > 15
    */
  
  /*
    choose a target  
    */
  
  if (ch->abilities.intel <= 3) {
    fjump=2; cjump=2; tjump=2; njump=2; mjump=2; kjump = 2; djump = 0;
  } else if (ch->abilities.intel <= 9) {
    fjump=4; cjump=3;tjump=2;njump=2;mjump=1; kjump = 2; djump =2;
  } else if (ch->abilities.intel <= 12) {
    fjump=3; cjump=3;tjump=2;njump=2;mjump=2; kjump = 3; djump = 2;
  } else if (ch->abilities.intel <= 15) {
    fjump=3; cjump=3;tjump=2;njump=2;mjump=3; kjump = 2; djump = 2;
  } else {  
    fjump=3;cjump=3;tjump=2;njump=1;mjump=3; kjump = 3; djump = 2;
  }
  
  total = (fjump*ftot)+(cjump*ctot)+(tjump*ttot)+(njump*ntot)+(mjump*mtot)+
    (djump*dtot)+(kjump*ktot);
  
  total = (int) number(1,(int)total);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;
       tmp_ch=tmp_ch->next_in_room) {
    if ((CAN_SEE(ch,tmp_ch))&&(!IS_SET(tmp_ch->specials.act,PLR_NOHASSLE))&&
	(!IS_AFFECTED(tmp_ch, AFF_SNEAK)) && (ch != tmp_ch)) {
      if (!IS_SET(ch->specials.act, ACT_WIMPY) || !AWAKE(tmp_ch)) {
	if ((tmp_ch->specials.zone != ch->specials.zone &&
	    !strchr(zone_table[ch->specials.zone].races, GET_RACE(tmp_ch))) ||
	    IS_SET(tmp_ch->specials.act, ACT_ANNOYING)) {
	  if (!in_group(ch, tmp_ch)) {
	    if (IS_NPC(tmp_ch)) {
	      total -= njump;
	    } else if (HasClass(tmp_ch,CLASS_WARRIOR)) {
	      total -= fjump;
	    } else if (HasClass(tmp_ch,CLASS_CLERIC)) {
	      total -= cjump;
	    } else if (HasClass(tmp_ch,CLASS_MAGIC_USER)) {
	      total -= mjump;
	    } else if (HasClass(tmp_ch, CLASS_THIEF)) {
	      total -= tjump;
	    } else if (HasClass(tmp_ch, CLASS_DRUID)) {
	      total -= djump;
	    } else if (HasClass(tmp_ch, CLASS_MONK)) {
	      total -= kjump;
	    }
	    if (total <= 0)
	      return(tmp_ch);
	  }
	}
      }
    }
  }
  
  if (ch->specials.fighting)
    return(ch->specials.fighting);
  
  return(0);
}

struct char_data *FindAnyVictim( struct char_data *ch)
{
  struct char_data *tmp_ch;
  unsigned char found=FALSE;
  unsigned short ftot=0,ttot=0,ctot=0,ntot=0,mtot=0, ktot=0, dtot=0;
  unsigned short total;
  unsigned short fjump=0,njump=0,cjump=0,mjump=0,tjump=0,kjump=0,djump=0;
  
  if (ch->in_room < 0) return(0);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;tmp_ch=tmp_ch->next_in_room) {
    if ((CAN_SEE(ch,tmp_ch))&&(!IS_SET(tmp_ch->specials.act,PLR_NOHASSLE))) {
      if (!(IS_AFFECTED(ch, AFF_CHARM)) || (ch->master != tmp_ch)) {
	if (!SameRace(ch, tmp_ch) || (!IS_NPC(tmp_ch))) {
	  found = TRUE;  /* a potential victim has been found */ 
	  if (!IS_NPC(tmp_ch)) {
	    if(HasClass(tmp_ch, CLASS_WARRIOR))
	      ftot++;
	    else if (HasClass(tmp_ch, CLASS_CLERIC))
	      ctot++;
	    else if (HasClass(tmp_ch,CLASS_MAGIC_USER))
	      mtot++;
	    else if (HasClass(tmp_ch, CLASS_THIEF))
	      ttot++;
	    else if (HasClass(tmp_ch, CLASS_DRUID)) 
	      dtot++;
	    else if (HasClass(tmp_ch, CLASS_MONK)) 
	      ktot++;
	  } else {
	    ntot++;
	  }
	}
      }
    }
  }
  
  /* if no legal enemies have been found, return 0 */
  
  if (!found) {
    return(0);
  }
  
  /* 
    give higher priority to fighters, clerics, thieves, magic users if int <= 12
    give higher priority to fighters, clerics, magic users thieves is inv > 12
    give higher priority to magic users, fighters, clerics, thieves if int > 15
    */
  
  /*
    choose a target  
    */
  
  if (ch->abilities.intel <= 3) {
    fjump=2; cjump=2; tjump=2; njump=2; mjump=2; kjump = 2; djump = 0;
  } else if (ch->abilities.intel <= 9) {
    fjump=4; cjump=3;tjump=2;njump=2;mjump=1; kjump = 2; djump =2;
  } else if (ch->abilities.intel <= 12) {
    fjump=3; cjump=3;tjump=2;njump=2;mjump=2; kjump = 3; djump = 2;
  } else if (ch->abilities.intel <= 15) {
    fjump=3; cjump=3;tjump=2;njump=2;mjump=3; kjump = 2; djump = 2;
  } else {  
    fjump=3;cjump=3;tjump=2;njump=1;mjump=3; kjump = 3; djump = 2;
  }
  
  total = (fjump*ftot)+(cjump*ctot)+(tjump*ttot)+(njump*ntot)+(mjump*mtot)+
    (djump*dtot)+(kjump*ktot);
  
  total = number(1,(int)total);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;tmp_ch=tmp_ch->next_in_room) {
    if ((CAN_SEE(ch,tmp_ch))&&(!IS_SET(tmp_ch->specials.act,PLR_NOHASSLE))) {
      if (!SameRace(tmp_ch, ch) || (!IS_NPC(tmp_ch))) {
	if (IS_NPC(tmp_ch)) {
	  total -= njump;
	} else if (HasClass(tmp_ch,CLASS_WARRIOR)) {
	  total -= fjump;
	} else if (HasClass(tmp_ch,CLASS_CLERIC)) {
	  total -= cjump;
	} else if (HasClass(tmp_ch,CLASS_MAGIC_USER)) {
	  total -= mjump;
	} else if (HasClass(tmp_ch, CLASS_THIEF)) {
	  total -= tjump;
	} else if (HasClass(tmp_ch, CLASS_DRUID)) {
	  total -= djump;
	} else if (HasClass(tmp_ch, CLASS_MONK)) {
	  total -= kjump;
	}
	if (total <= 0)
	  return(tmp_ch);
      }
    }      
  }
  
  if (ch->specials.fighting)
    return(ch->specials.fighting);
  
  return(0);
  
}

int BreakLifeSaverObj( struct char_data *ch)
{

      int found=FALSE, i, j;
      char buf[200];
      struct obj_data *o;

      /*
       *  check eq for object with the effect
       */
      for (i = 0; i< MAX_WEAR && !found; i++) {
	if (ch->equipment[i]) {
	  o = ch->equipment[i];
          for (j=0; j<MAX_OBJ_AFFECT; j++) {
            if (o->affected[j].location == APPLY_SPELL) {
              if (IS_SET(o->affected[j].modifier,AFF_LIFE_PROT)) {
		 found = i;		 
	      }
	    }
	  }
	}
      }
      if (found) {

	/*
         *  break the object.
         */

	 sprintf(buf,"%s shatters with a blinding flash of light!\n\r", 
		 ch->equipment[found]->name);
	 send_to_char(buf, ch);
	 if ((o = unequip_char(ch, found)) != NULL) {
	   MakeScrap(ch, o);
	 }
      }

}

int BrittleCheck(struct char_data *ch, int dam)
{
  char buf[200];
  struct obj_data *obj;

  if (dam <= 0)
    return(FALSE);

  if (ch->equipment[WIELD]) {
    if (IS_OBJ_STAT(ch->equipment[WIELD], ITEM_BRITTLE)) {
       if ((obj = unequip_char(ch,WIELD))!=NULL) {
	 sprintf(buf, "%s shatters.\n\r", obj->short_description);
	 send_to_char(buf, ch);
	 MakeScrap(ch, obj);
         return(TRUE);
       }
    }
  }
}

int PreProcDam(struct char_data *ch, int type, int dam)
{
  
  unsigned Our_Bit;
  
  /*
    long, intricate list, with the various bits and the various spells and
    such determined
    */
  
  switch (type) {
  case SPELL_FIREBALL:
  case SPELL_BURNING_HANDS:
  case SPELL_FLAMESTRIKE:
  case SPELL_FIRE_BREATH:
  case SPELL_HEAT_STUFF:
  case SPELL_FIRESTORM:
    Our_Bit = IMM_FIRE;
    break;
    
  case SPELL_SHOCKING_GRASP:
  case SPELL_LIGHTNING_BOLT:
  case SPELL_CALL_LIGHTNING:
  case SPELL_LIGHTNING_BREATH:
  case SPELL_CHAIN_LIGHTNING:
    Our_Bit = IMM_ELEC;
    break;

  case SPELL_CHILL_TOUCH:		     
  case SPELL_CONE_OF_COLD:		     
  case SPELL_ICE_STORM:			     
  case SPELL_FROST_BREATH:
    Our_Bit = IMM_COLD;
    break;
    
  case SPELL_MAGIC_MISSILE:
  case SPELL_COLOUR_SPRAY:
  case SPELL_GAS_BREATH:
  case SPELL_METEOR_SWARM:
  case SPELL_SUNRAY:

    Our_Bit = IMM_ENERGY;
    break;
    
  case SPELL_ENERGY_DRAIN:
    Our_Bit = IMM_DRAIN;
    break;
    
  case SPELL_ACID_BREATH:
  case SPELL_ACID_BLAST:
    Our_Bit = IMM_ACID;
    break;

  case SKILL_BACKSTAB:
  case TYPE_PIERCE:
  case TYPE_STING:
  case TYPE_STAB:
    Our_Bit = IMM_PIERCE;
    break;

  case TYPE_SLASH:
  case TYPE_WHIP:
  case TYPE_CLEAVE:
  case TYPE_CLAW:
    Our_Bit = IMM_SLASH;
    break;

  case TYPE_BLUDGEON:
  case TYPE_HIT:
  case SKILL_KICK:
  case TYPE_CRUSH:
  case TYPE_BITE:
  case TYPE_SMASH:
  case TYPE_SMITE:
  case TYPE_BLAST:
    Our_Bit = IMM_BLUNT;
    break;

  case SPELL_POISON:
    Our_Bit = IMM_POISON;
    break;

  default:
    return(dam);
    break;
  }
  
  if (IS_SET(ch->susc, Our_Bit))
    dam <<= 1;
  
  if (IS_SET(ch->immune, Our_Bit))
    dam >>= 1;
  
  if (IS_SET(ch->M_immune, Our_Bit))
    dam = -1;
  
  return(dam);
}


int DamageOneItem( struct char_data *ch, int dam_type, struct obj_data *obj)
{
  int num;
  char buf[256];
  
  num = DamagedByAttack(obj, dam_type);
  
  if (num == -1) {  /* destroy object*/
    return(TRUE);
  } else if (num != 0) {
    sprintf(buf, "%s is %s.\n\r",obj->short_description, 
	    ItemDamType[dam_type-1]);
    send_to_char(buf,ch);
    if (DamageItem(ch, obj, num)) {
      return(TRUE);
    }
  }
  return(FALSE);
}


void MakeScrap( struct char_data *ch, struct obj_data *obj)
{
  char buf[200];
  struct obj_data *t, *x;

  extern char DestroyedItems;

  if(obj_index[obj->item_number].virtual == ITEM_SCRAPS) {
    act("$p is reduced to nothingness.", TRUE, ch, obj, 0, TO_CHAR);
    act("$p is reduced to nothingness.", TRUE, ch, obj, 0, TO_ROOM);

    if (obj->carried_by) {
      obj_from_char(obj);
    } else if (obj->equipped_by) {
      obj = unequip_char(ch, obj->eq_pos);
    }

    extract_obj(obj);
  } else {

    act("$p is reduced to nothing but scraps.", TRUE, ch, obj, 0, TO_CHAR);
    act("$p is reduced to nothing but scraps.", TRUE, ch, obj, 0, TO_ROOM);
    
    t = read_object(30, VIRTUAL);
    
    sprintf(buf, "Scraps from %s lie in a pile here.", 
	    obj->short_description);
    
    free(t->description);
    t->description = strdup(buf);
    if (obj->carried_by) {
      obj_from_char(obj);
    } else if (obj->equipped_by) {
      obj = unequip_char(ch, obj->eq_pos);
    }
    obj_to_room(t, ch->in_room);
    t->obj_flags.value[0] = 20;
    
    while (obj->contains) {
      x = obj->contains;
      obj_from_obj(x);
      obj_to_room(x, ch->in_room);
    }
    
    
    check_falling_obj(t, ch->in_room);
    
    extract_obj(obj);

    DestroyedItems = 1;
  }
  
}

void DamageAllStuff( struct char_data *ch, int dam_type)
{
  int j;
  struct obj_data *obj, *next;
  
  /* this procedure takes all of the items in equipment and inventory
     and damages the ones that should be damaged */
  
  /* equipment */
  
  for (j = 0; j < MAX_WEAR; j++) {
    if (ch->equipment[j] && ch->equipment[j]->item_number>=0) {
      obj = ch->equipment[j];
      if (DamageOneItem(ch, dam_type, obj)) { /* TRUE == destroyed */
	if ((obj = unequip_char(ch,j))!=NULL) {
	  MakeScrap(ch, obj);
	} else {
	  logE("hmm, really wierd!");
	}
      }
    }
  }
  
  /* inventory */
  
  obj = ch->carrying;
  while (obj) {
    next = obj->next_content;
    if (obj->item_number >= 0) {
      if (DamageOneItem(ch, dam_type, obj)) {
	MakeScrap(ch, obj);
      }
    }
    obj = next;
  }

  /* stuff on ground */

  obj = real_roomp(ch->in_room)->contents;
  while(obj) {
    next = obj->next_content;
    if (obj->item_number >= 0) {
      if(number(0,1)) {
	if (DamageOneItem(ch, dam_type, obj)) {
	  MakeScrap(ch, obj);
	}
      }
    }
    obj = next;
  }
}


int DamageItem(struct char_data *ch, struct obj_data *o, int num)
{
  /*  damage weaons or armor */
  
  if (ITEM_TYPE(o) == ITEM_ARMOR) {
    o->obj_flags.value[0] -= num;
    if (o->obj_flags.value[0] < 0) {
      return(TRUE);
    }    
  } else if (ITEM_TYPE(o) == ITEM_WEAPON) {
    o->obj_flags.value[2] -= num;
    if (o->obj_flags.value[2] <= 0) {
      return(TRUE);
    }
  }
  return(FALSE);
}

int ItemSave( struct obj_data *i, int dam_type) 
{
  int num, j;

  if (IS_OBJ_STAT(i,ITEM_BRITTLE)) {
    return(FALSE);
  }
  
  num = number(1,20);
  if (num <= 1) return(FALSE);
  if (num >= 20) return(TRUE);
  
  for(j=0; j<MAX_OBJ_AFFECT; j++)
    if ((i->affected[j].location == APPLY_SAVING_SPELL) || 
	(i->affected[j].location == APPLY_SAVE_ALL)) {
      num -= i->affected[j].modifier;
    }
  if (i->affected[j].location != APPLY_NONE) {
    num += 1;
  }
  if (i->affected[j].location == APPLY_HITROLL) {
    num += i->affected[j].modifier;
  }
  
  if (ITEM_TYPE(i) != ITEM_ARMOR)
    num += 1;
  
  if (num <= 1) return(FALSE);
  if (num >= 20) return(TRUE);
  
  if (num >= ItemSaveThrows[(int)GET_ITEM_TYPE(i)-1][dam_type-1]) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}



int DamagedByAttack( struct obj_data *i, int dam_type)
{
  int num = 0;
  
  if ((ITEM_TYPE(i) == ITEM_ARMOR) || (ITEM_TYPE(i) == ITEM_WEAPON)){
    while (!ItemSave(i,dam_type)) {
      num+=1;
      if (num > 75)
	return(num);  /* so anything with over 75 ac points will not be
			 destroyed */
    }
    return(num);
  } else {
    if (ItemSave(i, dam_type)) {
      return(0);
    } else {
      return(-1);
    }
  }
}

int WeaponCheck(struct char_data *ch, struct char_data *v, int type, int dam)
{
  int Immunity, total, j;
  
  Immunity = -1;
  if (IS_SET(v->M_immune, IMM_NONMAG)) {
    Immunity = 0;
  }
  if (IS_SET(v->M_immune, IMM_PLUS1)) {
    Immunity = 1;
  }
  if (IS_SET(v->M_immune, IMM_PLUS2)) {
    Immunity = 2;
  }
  if (IS_SET(v->M_immune, IMM_PLUS3)) {
    Immunity = 3;
  }
  if (IS_SET(v->M_immune, IMM_PLUS4)) {
    Immunity = 4;
  }
  
  if (Immunity < 0)
    return(dam);
  
  if ((type < TYPE_HIT) || (type > TYPE_SMITE))  {
    return(dam);
  } else {
    if ( type == TYPE_HIT || IS_NPC(ch) || 
	(IS_SET(ch->player.class, CLASS_MONK) && !ch->equipment[WIELD])) {

      if (GetMaxLevel(ch) > ((Immunity+1)*(Immunity+1))+6) {
	return(dam);
      } else {
	act("$N ignores your puny attack.", FALSE, ch, 0, v, TO_CHAR);
	return(0);
      }
    } else {
      total = 0;
      if (!ch->equipment[WIELD])
	return(0);
      for(j=0; j<MAX_OBJ_AFFECT; j++)
	if ((ch->equipment[WIELD]->affected[j].location == APPLY_HITROLL) ||
	    (ch->equipment[WIELD]->affected[j].location == APPLY_HITNDAM)) {
	  total += ch->equipment[WIELD]->affected[j].modifier;
	}
      if (total > Immunity) {
	return(dam);
      } else {
	act("$N ignores your puny weapon.", FALSE, ch, 0, v, TO_CHAR);
	return(0);
      }     
    }
  }
}


int DamageStuff(struct char_data *v, int type, int dam)
{
  int num, dam_type;
  struct obj_data *obj;
  
  if (type >= TYPE_HIT && type <= TYPE_SMITE) {
    num = number(3,17);  /* wear_neck through hold */
    if (v->equipment[num]) {
      if ((type == TYPE_BLUDGEON && dam > 10) ||
	  (type == TYPE_CRUSH && dam > 5) ||
	  (type == TYPE_SMASH && dam > 10) ||
	  (type == TYPE_BITE && dam > 15) ||
	  (type == TYPE_CLAW && dam > 20) ||
	  (type == TYPE_SLASH && dam > 30) ||
	  (type == TYPE_SMITE && dam > 10) ||
	  (type == TYPE_HIT && dam > 20)) {
	if (DamageOneItem(v, BLOW_DAMAGE, v->equipment[num])) {
	  if ((obj = unequip_char(v,num))!=NULL) {
	    MakeScrap(v, obj);
	  }
	}
      }
    }
  } else {
    dam_type = GetItemDamageType(type);
    if (dam_type) {
      num = number(1,50); /* as this number increases or decreases
			     the chance of item damage decreases
			     or increases */
      if (dam >= num)
	DamageAllStuff(v, dam_type);
    }
  }
  
}


int GetItemDamageType( int type)
{
  
  switch(type) {
  case SPELL_FIREBALL:
  case SPELL_FLAMESTRIKE:
  case SPELL_FIRE_BREATH:
    return(FIRE_DAMAGE);
    break;
    
  case SPELL_LIGHTNING_BOLT:
  case SPELL_CALL_LIGHTNING:
  case SPELL_LIGHTNING_BREATH:
    return(ELEC_DAMAGE);
    break;
    
  case SPELL_CONE_OF_COLD:
  case SPELL_ICE_STORM:
  case SPELL_FROST_BREATH:
    return(COLD_DAMAGE);
    break;
    
  case SPELL_COLOUR_SPRAY:
  case SPELL_METEOR_SWARM:
  case SPELL_GAS_BREATH:
    return(BLOW_DAMAGE);
    break;
    
  case SPELL_ACID_BREATH:
  case SPELL_ACID_BLAST:
    return(ACID_DAMAGE);
  default:
    return(0);
    break;  
  }
  
}

int SkipImmortals(struct char_data *v, int amnt)
{
  /* You can't damage an immortal! */
  
  if ((GetMaxLevel(v)>MAX_MORT) && !IS_NPC(v)) 
    amnt = 0;
  
  /* special type of monster */		
  if (IS_NPC(v) && (IS_SET(v->specials.act, ACT_IMMORTAL))) {
    amnt = -1;
  }

  if (IS_PC(v) && !v->desc) {  /* link dead pc */
    amnt = -1;
  }

  return(amnt);
  
}


int WeaponSpell( struct char_data *c, struct char_data *v, int type)
{
  int j, num;
  
  if ((c->in_room == v->in_room) && (GET_POS(v) != POSITION_DEAD)) {
    if ((c->equipment[WIELD]) && ((type >= TYPE_BLUDGEON) &&
				  (type <= TYPE_SMITE))) {
      for(j=0; j<MAX_OBJ_AFFECT; j++) {
	if (c->equipment[WIELD]->affected[j].location ==
	    APPLY_WEAPON_SPELL) {
	  num = c->equipment[WIELD]->affected[j].modifier;
	  ((*skill_info[num].spell_pointer)
	   (6, c, "", SPELL_TYPE_WAND, v, 0));
	}
      }
    }
  }
}

struct char_data *FindAnAttacker(struct char_data *ch) 
{
  struct char_data *tmp_ch;
  unsigned char found=FALSE;
  unsigned short ftot=0,ttot=0,ctot=0,ntot=0,mtot=0, ktot=0, dtot=0;
  unsigned short total;
  unsigned short fjump=0,njump=0,cjump=0,mjump=0,tjump=0,kjump=0,djump=0;
  
  if (ch->in_room < 0) return(0);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;
       tmp_ch=tmp_ch->next_in_room) {
       if (ch!=tmp_ch) {
	  if (tmp_ch->specials.fighting == ch) {
	      found = TRUE;  /* a potential victim has been found */ 
	      if (!IS_NPC(tmp_ch)) {
		if(HasClass(tmp_ch, CLASS_WARRIOR))
		  ftot++;
		else if (HasClass(tmp_ch, CLASS_CLERIC))
		  ctot++;
		else if (HasClass(tmp_ch,CLASS_MAGIC_USER))
		  mtot++;
		else if (HasClass(tmp_ch, CLASS_THIEF))
		  ttot++;
		else if (HasClass(tmp_ch, CLASS_DRUID)) 
		  dtot++;
		else if (HasClass(tmp_ch, CLASS_MONK)) 
		  ktot++;
	      } else {
		ntot++;
	      }
	    }
	}
     }
  
  /* if no legal enemies have been found, return 0 */
  
  if (!found) {
    return(0);
  }
  
  /* 
    give higher priority to fighters, clerics, thieves, magic users if int <= 12
    give higher priority to fighters, clerics, magic users thieves is inv > 12
    give higher priority to magic users, fighters, clerics, thieves if int > 15
    */
  
  /*
    choose a target  
    */
  
  if (ch->abilities.intel <= 3) {
    fjump=2; cjump=2; tjump=2; njump=2; mjump=2; kjump = 2; djump = 0;
  } else if (ch->abilities.intel <= 9) {
    fjump=4; cjump=3;tjump=2;njump=2;mjump=1; kjump = 2; djump =2;
  } else if (ch->abilities.intel <= 12) {
    fjump=3; cjump=3;tjump=2;njump=2;mjump=2; kjump = 3; djump = 2;
  } else if (ch->abilities.intel <= 15) {
    fjump=3; cjump=3;tjump=2;njump=2;mjump=3; kjump = 2; djump = 2;
  } else {  
    fjump=3;cjump=3;tjump=2;njump=1;mjump=3; kjump = 3; djump = 2;
  }
  
  total = (fjump*ftot)+(cjump*ctot)+(tjump*ttot)+(njump*ntot)+(mjump*mtot)+
    (djump*dtot)+(kjump*ktot);
  
  total = number(1,(int)total);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;
       tmp_ch=tmp_ch->next_in_room) {
	    if (tmp_ch->specials.fighting == ch) {
	      if (IS_NPC(tmp_ch)) {
		total -= njump;
	      } else if (HasClass(tmp_ch,CLASS_WARRIOR)) {
		total -= fjump;
	      } else if (HasClass(tmp_ch,CLASS_CLERIC)) {
		total -= cjump;
	      } else if (HasClass(tmp_ch,CLASS_MAGIC_USER)) {
		total -= mjump;
	      } else if (HasClass(tmp_ch, CLASS_THIEF)) {
		total -= tjump;
	      } else if (HasClass(tmp_ch, CLASS_DRUID)) {
		total -= djump;
	      } else if (HasClass(tmp_ch, CLASS_MONK)) {
		total -= kjump;
	      }
	      if (total <= 0)
		return(tmp_ch);
	    }
	  }
  
  if (ch->specials.fighting)
    return(ch->specials.fighting);
  
  return(0);
}

void shoot( struct char_data *ch, struct char_data *victim)
{
  struct obj_data *bow, *arrow;
  int tohit=0, todam=0;
  
  /*
  **  check for bow and arrow.
  */

  bow = ch->equipment[HOLD];
  arrow = ch->equipment[WIELD];

  if (!bow) {
    send_to_char("You need a missile weapon (like a bow)\n\r", ch);
    return;
  } else if (!arrow) {
    send_to_char("You need a projectile to shoot!\n\r", ch);
  } else if (!bow && !arrow) {
    send_to_char("You need a bow-like item, and a projectile to shoot!\n\r",ch);
  } else {
    /*
    **  for bows:  value[0] = arror type
    **             value[1] = strength required
    **             value[2] = + to hit
    **             value[3] = + to damage
    */

    if (bow->obj_flags.value[0] != arrow->obj_flags.value[0]) {
      send_to_char("That projectile does not fit in that projector.\n\r", ch);
      return;
    }
    /*
    **  check for str problem:  same as wield, but with bow.
    */
    if (bow->obj_flags.value[1] > str_app[STRENGTH_APPLY_INDEX(ch)].wield_w) {
      send_to_char("That bow is too heavy for you to use!\n\r", ch);
      return;
    }

    /*
    **  check for bonuses on the bow.
    */
    tohit = bow->obj_flags.value[2];
    todam = bow->obj_flags.value[3];

    /*
    **   temporarily add those bonuses.
    */
    GET_HITROLL(ch)+=tohit;
    GET_DAMROLL(ch)+=todam;

    act("$n shoots $p at $N!", FALSE, ch, arrow, victim, TO_NOTVICT);
    act("$n launches $p at you", FALSE, ch, arrow, victim, TO_VICT);
    act("You shoot at $N with $p", FALSE, ch, arrow, victim, TO_CHAR);

    /*
    **   fire the weapon.
    */
    MissileHit(ch, victim, TYPE_UNDEFINED);

    GET_HITROLL(ch)-=tohit;
    GET_DAMROLL(ch)-=todam;

  }
}

struct char_data *FindMetaVictim( struct char_data *ch)
{
  struct char_data *tmp_ch;
  unsigned char found=FALSE;
  unsigned short total=0;

  
  if (ch->in_room < 0) return(0);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;tmp_ch=tmp_ch->next_in_room) {
    if ((CAN_SEE(ch,tmp_ch))&&(!IS_SET(tmp_ch->specials.act,PLR_NOHASSLE))) {
      if (!(IS_AFFECTED(ch, AFF_CHARM)) || (ch->master != tmp_ch)) {
	if (!SameRace(ch, tmp_ch)) {
	   found = TRUE;
	   total++;
	}
      }
    }
  }
  
  /* if no legal enemies have been found, return 0 */
  
  if (!found) {
    return(0);
  }
  
  total = number(1,(int)total);
  
  for (tmp_ch=(real_roomp(ch->in_room))->people;tmp_ch;tmp_ch=tmp_ch->next_in_room) {
    if ((CAN_SEE(ch,tmp_ch))&&(!IS_SET(tmp_ch->specials.act,PLR_NOHASSLE))) {
      if (!SameRace(tmp_ch, ch)){
	total--;
	if (total == 0)
	  return(tmp_ch);
      }
    }
  }
  
  if (ch->specials.fighting)
    return(ch->specials.fighting);
  
  return(0);
  
}


/*
  returns, extracts, switches etc.. anyone.
*/
void NailThisSucker( struct char_data *ch)
{

  struct char_data *pers;

  death_cry(ch);
    
  if (IS_NPC(ch) && (IS_SET(ch->specials.act, ACT_POLYSELF))) {
    /*
     *   take char from storage, to room     
     */
    pers = ch->desc->original;
    char_from_room(pers);
    char_to_room(pers, ch->in_room);
    SwitchStuff(ch, pers);
    extract_char(ch);
    ch = pers;
  }
  zero_rent(ch);
  extract_char(ch);
}


int GetFormType(struct char_data *ch)
{
  int num;

  num = number(1,100);
  switch(GET_RACE(ch)) {
  case RACE_REPTILE:
    if (num <= 50) {
      return(TYPE_CLAW);
    } else {
      return(TYPE_BITE);
    }
    break;
  case RACE_LYCANTH:
  case RACE_DRAGON:
  case RACE_PREDATOR:
  case RACE_LABRAT:
    if (num <= 33) {
      return(TYPE_BITE);
    } else {
      return(TYPE_CLAW);
    }
    break;
  case RACE_INSECT:
    if (num <= 50) {
      return(TYPE_BITE);
    } else {
      return(TYPE_STING);
    }
    break;
  case RACE_ARACHNID:
  case RACE_DINOSAUR:
  case RACE_FISH:
  case RACE_SNAKE:
    return(TYPE_BITE);
    break;
  case RACE_BIRD:
  case RACE_SKEXIE:
    return(TYPE_CLAW);
    break;
  case RACE_GIANT:
  case RACE_GOLEM:
    return(TYPE_BLUDGEON);
    break;
  case RACE_DEMON:
  case RACE_DEVIL:
  case RACE_TROLL:
  case RACE_TROGMAN:
    return(TYPE_CLAW);
    break;
  case RACE_TREE:
    return(TYPE_SMITE);
    break;
  case RACE_MFLAYER:
    if (num <= 60) {
      return(TYPE_WHIP);
    } else if (num < 80) {
      return(TYPE_BITE);
    } else {
      return(TYPE_HIT);
    }
    break;
  case RACE_PRIMATE:
    if (num <= 70) {
      return(TYPE_HIT);
    } else {
      return(TYPE_BITE);
    }
    break;
  case RACE_TYTAN:
    return(TYPE_BLAST);
    break;
  default:
    return(TYPE_HIT);
  }
}

int MonkDodge( struct char_data *ch, struct char_data *v, int *dam)
{

  if (number(1, 20000) < v->skills[SKILL_DODGE].learned*
                         GET_LEVEL(v , MONK_LEVEL_IND)) { 
    *dam = 0;
    act("You dodge the attack", FALSE, ch, 0, v, TO_VICT);
    act("$N dodges the attack", FALSE, ch, 0, v, TO_CHAR);
    act("$N dodges $n's attack", FALSE, ch, 0, v, TO_NOTVICT);
  } else {
    *dam -= GET_LEVEL(ch, MONK_LEVEL_IND)/6;
  }

  return(0);
}

