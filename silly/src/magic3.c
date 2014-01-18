
/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement

  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <assert.h>

#include "protos.h"

/* Extern structures */
extern struct room_data *world;
extern struct obj_data  *object_list;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;

#define STATE(d) ((d)->connected)
#define IS_IMMUNE(ch, bit) (IS_SET((ch)->M_immune, bit))

/* Extern procedures */

char *strdup(char *source);

/*
  druid spells
*/

void spell_tree_travel(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (!affected_by_spell(ch, SPELL_TREE_TRAVEL)) {
    af.type      = SPELL_TREE_TRAVEL;
    
    af.duration  = 24;
    af.modifier  = -5;
    af.location  = APPLY_AC;
    af.bitvector = AFF_TREE_TRAVEL;
    affect_to_char(victim, &af);
    
    send_to_char("You feel as one with the trees... Groovy!\n\r", victim);
  } else {
    send_to_char("Nothing seems to happen\n\r", ch);
  }

}

void spell_transport_via_plant(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj)
{
  struct room_data *rp;
  struct obj_data *o;

  /* find the tree in the room */

  rp = real_roomp(ch->in_room);
  for (o = rp->contents; o; o = o->next_content) {
    if (ITEM_TYPE(o) == ITEM_TREE)
      break;
  }

  if (!o) {
    send_to_char("You need to have a tree nearby\n\r", ch);
    return;
  }

  if (ITEM_TYPE(obj) != ITEM_TREE) {
    send_to_char("Thats not a tree!\n\r", ch);
    return;
  }
 
  if (obj->in_room < 0) {
    send_to_char("That tree is nowhere to be found\n\r", ch);
    return;
  }

  if (!real_roomp(obj->in_room)) {
    send_to_char("That tree is nowhere to be found\n\r", ch);
    return;
  }

  act("$n touches $p, and slowly vanishes within!", FALSE, ch, o, 0, TO_ROOM);
  act("You touch $p, and join your forms.", FALSE, ch, o, 0, TO_CHAR);
  char_from_room(ch);
  char_to_room(ch, obj->in_room);
  act("$p rustles slightly, and $n magically steps from within!", FALSE, ch, obj, 0, TO_ROOM);
  act("You are instantly transported to $p!", FALSE, ch, obj, 0, TO_CHAR);
  do_look(ch, "\0", 0);
  
}

void spell_speak_with_plants(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj)
{
  char buffer[128];

  assert(ch && obj);

  if (ITEM_TYPE(obj) != ITEM_TREE) {
    send_to_char("Sorry, you can't talk to that sort of thing\n\r", ch);
    return;
  }

  sprintf(buffer, "%s says 'Hi $n, how ya doin?'", 
	  fname(obj->name));
  act(buffer, FALSE, ch, obj, 0, TO_CHAR);
  act("$p rustles slightly.", FALSE, ch, obj, 0, TO_ROOM);

}

#define TREE 6110

void spell_changestaff(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  struct obj_data *s;
  struct char_data *t;

  /* player must be holding staff at the time */

  if (!ch->equipment[HOLD]) {
    send_to_char("You must be holding a staff!\n\r", ch);
    return;
  }

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }

  s = unequip_char(ch, HOLD);
  if (ITEM_TYPE(s) != ITEM_STAFF) {
    act("$p is not sufficient to complete this spell", 
	FALSE, ch, s, 0, TO_CHAR);
    extract_obj(s);
    return;
  }

  if (!s->obj_flags.value[2]) {
    act("$p is not sufficiently powerful to complete this spell", 
	FALSE, ch, s, 0, TO_CHAR);
    extract_obj(s);
    return;
  }


  act("$p vanishes in a burst of flame!", FALSE, ch, s, 0, TO_ROOM);
  act("$p vanishes in a burst of flame!", FALSE, ch, s, 0, TO_CHAR);

  t = read_mobile(TREE, VIRTUAL);
  char_to_room(t, ch->in_room);
  GET_EXP(t)=0;

  act("$n springs up in front of you!", FALSE, t, 0, 0, TO_ROOM);

  if(too_many_followers(ch)){
    act("$N takes one look at the size of your posse and justs says no!",
	TRUE, ch, ch->equipment[WIELD], victim, TO_CHAR);
    act("$N takes one look at the size of $n's posse and just says no!",
	TRUE, ch, ch->equipment[WIELD], victim, TO_ROOM);
  } else {
	
    af.type      = SPELL_CHARM_PERSON;
    
  if (IS_PC(ch) || ch->master) {
    af.duration  = follow_time(ch);
 
    af.duration += s->obj_flags.value[2]; /* num charges */

    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(t, &af);
  } else {
    SET_BIT(t->specials.affected_by, AFF_CHARM);
  }
    add_follower(t, ch);
    
    extract_obj(s);
  }
}

/* mage spells */
void spell_pword_kill(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
  int max = 80;

  max += level;
  max += level/2;

  if (GET_MAX_HIT(victim) <= max || GetMaxLevel(ch) > 53 ) {
    damage(ch, victim, GET_MAX_HIT(victim)*12, SPELL_PWORD_KILL);
  } else {
    send_to_char("They are too powerful to destroy this way\n\r", ch);
  }
    
}

void spell_pword_blind(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{

  if (GET_MAX_HIT(victim) <= 100 || GetMaxLevel(ch) > 53) {
    SET_BIT(victim->specials.affected_by, AFF_BLIND);
  } else {
    send_to_char("They are too powerful to blind this way\n\r", ch);
  }
    
}


void spell_chain_lightn(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
  int lev = level;
  struct char_data *t, *next;
  
  /* victim = levd6 damage */
  damage(ch, victim, dice(lev,6), SPELL_LIGHTNING_BOLT);
  lev--;

  for (t = real_roomp(ch->in_room)->people; t; t=next) {
    next = t->next_in_room;
    if (!in_group(ch, t) && t != victim && !IS_IMMORTAL(t)) {
      damage(ch, t, dice(lev,6), SPELL_LIGHTNING_BOLT);
      lev--;
    }
  }
}


void spell_scare(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
  if (GetMaxLevel(victim) <= 5)
    do_flee(victim, "\0", 0);
}

void spell_haste(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (affected_by_spell(victim, SPELL_HASTE)) {
    send_to_char("They are allready hasted.\n", ch);
    return;
  }

  if (IS_NPC(victim)) {
    send_to_char("It doesn't seem to work right on them.\n", ch);
    return;
  }

  if (IS_IMMUNE(victim, IMM_HOLD)) {
    act("$N seems to ignore your spell.", FALSE, ch, 0, victim, TO_CHAR);
    act("$n just tried to haste you, but you ignored it.", FALSE, ch, 0,
	victim, TO_VICT);
    if (!in_group(ch, victim)) {
      if (!IS_PC(ch))
	hit(victim, ch, TYPE_UNDEFINED);
    }
    return;
  }


  af.type      = SPELL_HASTE;
  af.duration  = level;
  af.modifier  = 1;
  af.location  = APPLY_HASTE;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  send_to_char("You feel fast!\n\r", victim);
  if (!IS_NPC(victim)) 
    victim->player.time.birth -= SECS_PER_MUD_YEAR;
  else {
    if (victim->desc && victim->desc->original)
      victim->desc->original->player.time.birth -= SECS_PER_MUD_YEAR;
  }

  
  if (!in_group(ch, victim)) {
    if (!IS_PC(ch))
      hit(victim, ch, TYPE_UNDEFINED);
  }

}

void spell_slow(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (affected_by_spell(victim, SPELL_SLOW)) {
    send_to_char("already slowed\n", ch);
    return;
  }

  if (IS_IMMUNE(victim, IMM_HOLD)) {
    act("$N seems to ignore your spell", FALSE, ch, 0, victim, TO_CHAR);
    act("$n just tried to slow you, but you ignored it.", FALSE, ch, 0,
	victim, TO_VICT);
    if (!in_group(ch, victim)) {
      if (!IS_PC(ch))
	hit(victim, ch, TYPE_UNDEFINED);
    }
    return;
  }

  af.type      = SPELL_SLOW;
  af.duration  = level;
  af.modifier  = -1;
  af.location  = APPLY_HASTE;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  send_to_char("You feel very slow!\n\r", victim);

  if (!in_group(ch, victim)) {
    if (!IS_PC(ch))
      hit(victim, ch, TYPE_UNDEFINED);
  }
}

#define KITTEN  3090
#define PUPPY   3091
#define BEAGLE  3092
#define ROTT    3093
#define WOLF    3094
#define HUGE_WOLF  13733
#define GRIZZLY 9024
#define BENGAL  9027

void spell_familiar(byte level, struct char_data *ch,
		       struct char_data **victim, struct obj_data *obj)
{

  struct affected_type af;
  struct char_data *f;

  if (affected_by_spell(ch, SPELL_FAMILIAR)) {
    send_to_char("You can't have more than 1 familiar per day\n\r",ch);
    return;
  }

  /*
    depending on the level, one of the pet shop kids.
    */

  if (level < 2) 
    f = read_mobile(KITTEN, VIRTUAL);
  else if (level < 4)
    f = read_mobile(PUPPY, VIRTUAL);
  else if (level < 6)
    f = read_mobile(BEAGLE, VIRTUAL);
  else if (level < 8)
    f = read_mobile(ROTT, VIRTUAL);
  else if (level < 20) {
    f = read_mobile(WOLF, VIRTUAL);
  }else if (level < 30) {
    f = read_mobile(GRIZZLY, VIRTUAL);
  }else if (level < 37) {
    f = read_mobile(HUGE_WOLF, VIRTUAL);
  } else {
    f = read_mobile(BENGAL, VIRTUAL);
  }
  char_to_room(f, ch->in_room);


  af.type      = SPELL_FAMILIAR;
  af.duration  = 24;
  af.modifier  = -1;
  af.location  = APPLY_ARMOR;
  af.bitvector = 0;
  affect_to_char(ch, &af);

  act("$n appears in a flash of light!\n\r", FALSE, f, 0, 0, TO_ROOM);

  SET_BIT(f->specials.affected_by, AFF_CHARM);
  GET_EXP(f) = 0;
  add_follower(f, ch);
  IS_CARRYING_W(f) = 0;
  IS_CARRYING_N(f) = 0;

  *victim = f;

}

/* cleric */

void spell_aid(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
  /* combo bless, cure light woundsish */
  struct affected_type af;

  if (affected_by_spell(victim, SPELL_AID)) {
    send_to_char("Already in effect\n\r", ch);
    return;
  }

  GET_HIT(victim)+=number(1,8);

  update_pos(victim);

  act("$n looks aided", FALSE, victim, 0, 0, TO_ROOM);
  send_to_char("You feel better!\n\r", victim);

  af.type      = SPELL_AID;
  af.duration  = 10;
  af.modifier  = 1;
  af.location  = APPLY_HITROLL;
  af.bitvector = 0;
  affect_to_char(victim, &af);  

}

#define GOLEM 38

void spell_golem(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
  int count=0;
  int armor;
  struct char_data *gol;
  struct obj_data *helm=0,*jacket=0,*leggings=0,*sleeves=0,*gloves=0,
                  *boots=0,*o;
  struct room_data *rp;
  
/* you need:  helm, jacket, leggings, sleeves, gloves, boots */

  rp = real_roomp(ch->in_room);
  if (!rp) return;

  for (o = rp->contents; o; o = o->next_content) {
    if (ITEM_TYPE(o) == ITEM_ARMOR) {
      if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_HEAD)) {
	if (!helm) {
	  count++;
	  helm = o;
	  continue;  /* next item */
	}
      }
      if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_FEET)) {
	if (!boots) {
	  count++;
	  boots = o;
	  continue;  /* next item */
	}
      }
      if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_BODY)) {
	if (!jacket) {
	  count++;
	  jacket = o;
	  continue;  /* next item */
	}
      }
      if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_LEGS)) {
	if (!leggings) {
	  count++;
	  leggings = o;
	  continue;  /* next item */
	}
      }
      if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_ARMS)) {
	if (!sleeves) {
	  count++;
	  sleeves = o;
	  continue;  /* next item */
	}
      }
      if (IS_SET(o->obj_flags.wear_flags, ITEM_WEAR_HANDS)) {
	if (!gloves) {
	  count++;
	  gloves = o;
	  continue;  /* next item */
	}
      }
    }
  }

  if (count < 6) {
    send_to_char("You don't have all the correct pieces!\n\r", ch);
    return;
  }

  if (count > 6) {
    send_to_char("Smells like an error to me!\n\r", ch);
    return;
  }

  if (!boots || !sleeves || !gloves || !helm || !jacket || !leggings) {
    /* shouldn't get this far */
    send_to_char("You don't have all the correct pieces!\n\r", ch);
    return;
  }

  gol = read_mobile(GOLEM, VIRTUAL);
  char_to_room(gol, ch->in_room);

  /* add up the armor values in the pieces */
  armor = boots->obj_flags.value[0]*2;
  armor += helm->obj_flags.value[0]*2;
  armor += gloves->obj_flags.value[0]*2;
  armor += (leggings->obj_flags.value[0]*4);
  armor += (sleeves->obj_flags.value[0]*4);
  armor += (jacket->obj_flags.value[0]*6);

  GET_AC(gol) -= armor;

  gol->points.max_hit = dice( (armor/6), 10) + level;
  GET_HIT(gol) = GET_MAX_HIT(gol);

  GET_LEVEL(gol, WARRIOR_LEVEL_IND) = (armor/6) + level/4;

  SET_BIT(gol->specials.affected_by, AFF_CHARM);
  GET_EXP(gol) = 0;
  IS_CARRYING_W(gol) = 0;
  IS_CARRYING_N(gol) = 0;

  gol->player.class = CLASS_WARRIOR;

  if (GET_LEVEL(gol, WARRIOR_LEVEL_IND) > 10)
    gol->mult_att+=0.5;
  
  if (GET_LEVEL(gol, WARRIOR_LEVEL_IND) > 20)
    gol->mult_att+=0.5;

  if (GET_LEVEL(gol, WARRIOR_LEVEL_IND) > 30)
    gol->mult_att+=0.5;
  
  
  /* add all the effects from all the items to the golem */
  AddAffects(gol,boots);
  AddAffects(gol,gloves);
  AddAffects(gol,jacket);
  AddAffects(gol,sleeves);
  AddAffects(gol,leggings);
  AddAffects(gol,helm);

  act("$n waves $s hand over a pile of armor on the floor", FALSE, ch, 0, 0, 
      TO_ROOM);
  act("You wave your hands over the pile of armor", FALSE, ch, 0, 0, TO_CHAR);

  act("The armor flys together to form a humanoid figure!", FALSE, ch, 0, 0, 
      TO_ROOM);

  act("$N is quickly assembled from the pieces", FALSE, ch, 0, gol, TO_CHAR);

  add_follower(gol, ch);

  extract_obj(helm);
  extract_obj(boots);
  extract_obj(gloves);
  extract_obj(leggings);
  extract_obj(sleeves);
  extract_obj(jacket);

}


/***************/


void spell_feeblemind(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  int t,i;

  if (!saves_spell(victim, SAVING_SPELL)) {

/* eld - I took the liberty of adding this little dandy..  In my opinion,  */
/*       this spell should not be accumulative.                            */

    if(affected_by_spell(victim, SPELL_FEEBLEMIND)) {
       send_to_char("They are already dumb enough as it is!\n\r", ch);
       return;
    } 

    send_to_char("You feel really really dumb\n\r", victim);

    af.type      = SPELL_FEEBLEMIND;
    af.duration  = 24;
    af.modifier  = -5;
    af.location  = APPLY_INT;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    af.type      = SPELL_FEEBLEMIND;
    af.duration  = 24;
    af.modifier  = 70;
    af.location  = APPLY_SPELLFAIL;
    af.bitvector = 0;
    affect_to_char(victim, &af);

      /*
      last, but certainly not least
      */

    if (!victim->skills)
      return;

    t = number(1,100);

    while (1) {
      for (i=0;i<MAX_SKILLS;i++) {
	if (victim->skills[i].learned)
	  t--;
	if (t==0) {
	  victim->skills[i].learned = 0;
	  victim->skills[i].flags = 0;
	  break;
	}

/* eld - what happens if you get outside the for loop?  Yer screwed...  */
/*       this fixes it by giving the function something to do (return)  */

      }
      return;
    }
  }
}



void spell_shillelagh(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int i;
  int count=0;
  
  assert(ch && obj);
  assert(MAX_OBJ_AFFECT >= 2);
  
  if ((GET_ITEM_TYPE(obj) == ITEM_WEAPON) &&
      !IS_SET(obj->obj_flags.extra_flags, ITEM_MAGIC)) {

    if (!isname("club", obj->name)) {
      send_to_char("That isn't a club!\n\r", ch);
      return;
    }
    
    for (i=0; i < MAX_OBJ_AFFECT; i++) {
      if (obj->affected[i].location == APPLY_NONE) 
	count++;
      if (obj->affected[i].location == APPLY_HITNDAM ||
          obj->affected[i].location == APPLY_HITROLL ||
          obj->affected[i].location == APPLY_DAMROLL)
	return;
    }

    if (count < 2) return;
    /*  find the slots */
    i = getFreeAffSlot(obj);
    
    SET_BIT(obj->obj_flags.extra_flags, ITEM_MAGIC);
    
    obj->affected[i].location = APPLY_HITNDAM;
    obj->affected[i].modifier = 1;

    obj->obj_flags.value[1] = 2;
    obj->obj_flags.value[2] = 4;

    act("$p glows yellow.",FALSE,ch,obj,0,TO_CHAR);
    SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD|ITEM_ANTI_EVIL);

  }
}


void spell_goodberry(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *tmp_obj;

  assert(ch);
  assert((level >= 0) && (level <= ABS_MAX_LVL));


  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }


  CREATE(tmp_obj, struct obj_data, 1);
  clear_object(tmp_obj);

  tmp_obj->name = strdup("berry blue blueberry");
  tmp_obj->short_description = strdup("a plump blueberry");
  tmp_obj->description = strdup("A scrumptions blueberry lies here.");

  tmp_obj->obj_flags.type_flag = ITEM_FOOD;
  tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_HOLD;
  tmp_obj->obj_flags.value[0] = 10;
  tmp_obj->obj_flags.weight = 1;
  tmp_obj->obj_flags.cost = 10;
  tmp_obj->obj_flags.cost_per_day = 1;

  /*
    give it a cure light wounds spell effect
    */

  SET_BIT(tmp_obj->obj_flags.extra_flags, ITEM_MAGIC);

  tmp_obj->affected[0].location = APPLY_EAT_SPELL;
  tmp_obj->affected[0].modifier = SPELL_CURE_LIGHT;
  

  tmp_obj->next = object_list;
  object_list = tmp_obj;

  obj_to_char(tmp_obj,ch);

  tmp_obj->item_number = -1;

  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("$p suddenly appears in your hand.",TRUE,ch,tmp_obj,0,TO_CHAR);
}

void spell_flame_blade(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *tmp_obj;

  assert(ch);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if (ch->equipment[WIELD]) {
    send_to_char("You can't be wielding a weapon\n\r", ch);
    return;
  }

  CREATE(tmp_obj, struct obj_data, 1);
  clear_object(tmp_obj);

  tmp_obj->name = strdup("blade flame");
  tmp_obj->short_description = strdup("a flame blade");
  tmp_obj->description = strdup("A flame blade burns brightly here");

  tmp_obj->obj_flags.type_flag = ITEM_WEAPON;
  tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_WIELD;
  tmp_obj->obj_flags.value[0] = 0;
  tmp_obj->obj_flags.value[1] = 1;
  tmp_obj->obj_flags.value[2] = 4;
  tmp_obj->obj_flags.value[3] = 3;
  tmp_obj->obj_flags.weight = 1;
  tmp_obj->obj_flags.cost = 10;
  tmp_obj->obj_flags.cost_per_day = 1;

  SET_BIT(tmp_obj->obj_flags.extra_flags, ITEM_MAGIC);

  tmp_obj->affected[0].location = APPLY_DAMROLL;
  tmp_obj->affected[0].modifier = 4+GET_LEVEL(ch, DRUID_LEVEL_IND)/5;

  tmp_obj->next = object_list;
  object_list = tmp_obj;

  equip_char(ch, tmp_obj, WIELD);

  tmp_obj->item_number = -1;

  act("$p appears in your hand.",TRUE,ch,tmp_obj,0,TO_CHAR);
  act("$p appears in $n's hand.",TRUE,ch,tmp_obj,0,TO_ROOM);
}




void spell_animal_growth(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }


  if (!IsAnimal(victim)) {
    send_to_char("Thats not an animal\n\r", ch);
    return;
  }

  if (affected_by_spell(victim, SPELL_ANIMAL_GROWTH)) {
    act("$N is already affected by that spell",FALSE, ch, 0, victim, TO_CHAR);
    return;
  }

  if (GetMaxLevel(victim)*2 > GetMaxLevel(ch)) {
    send_to_char("You can't make it more powerful than you!\n\r", ch);
    return;
  }

  if (IS_PC(victim)) {
    send_to_char("It would be in bad taste to cast that on a player\n\r", ch);
    return;
  }

  act("$n grows to double $s original size!", FALSE, victim, 0,0, TO_ROOM);
  act("You grow to double your original size!", FALSE,victim,0,0,TO_CHAR);

  af.type      = SPELL_ANIMAL_GROWTH;
  af.duration  = 24;
  af.modifier  = GET_MAX_HIT(victim);
  af.location  = APPLY_HIT;
  af.bitvector = AFF_GROWTH;
  affect_to_char(victim, &af);

  af.type      = SPELL_ANIMAL_GROWTH;
  af.duration  = 24;
  af.modifier  = 5;
  af.location  = APPLY_HITNDAM;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.type      = SPELL_ANIMAL_GROWTH;
  af.duration  = 24;
  af.modifier  = 3;
  af.location  = APPLY_SAVE_ALL;
  af.bitvector = 0;
  affect_to_char(victim, &af);
/*

  GET_LEVEL(victim, WARRIOR_LEVEL_IND) = 2*GetMaxLevel(victim);
*/
}

void spell_insect_growth(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;


  if (GET_RACE(victim)!=RACE_INSECT) {
    send_to_char("Thats not an insect.\n\r", ch);
    return;
  }

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }


  if (affected_by_spell(victim, SPELL_INSECT_GROWTH)) {
    act("$N is already affected by that spell",FALSE, ch, 0, victim, TO_CHAR);
    return;
  }

  if (GetMaxLevel(victim)*2 > GetMaxLevel(ch)) {
    send_to_char("You can't make it more powerful than you!\n\r", ch);
    return;
  }

  if (IS_PC(victim)) {
    send_to_char("It would be in bad taste to cast that on a player\n\r", ch);
    return;
  }

  act("$n grows to double $s original size!", FALSE, victim,0,0,TO_ROOM);
  act("You grow to double your original size!", FALSE,victim,0,0,TO_CHAR);

  af.type      = SPELL_INSECT_GROWTH;
  af.duration  = 24;
  af.modifier  = GET_MAX_HIT(victim);
  af.location  = APPLY_HIT;
  af.bitvector = AFF_GROWTH;
  affect_to_char(victim, &af);

  af.type      = SPELL_INSECT_GROWTH;
  af.duration  = 24;
  af.modifier  = 5;
  af.location  = APPLY_HITNDAM;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.type      = SPELL_INSECT_GROWTH;
  af.duration  = 24;
  af.modifier  = 3;
  af.location  = APPLY_SAVE_ALL;
  af.bitvector = 0;
  affect_to_char(victim, &af);
/*
  GET_LEVEL(victim, WARRIOR_LEVEL_IND) = 2*GetMaxLevel(victim);
*/
}

#define CREEPING_DEATH 39

void spell_creeping_death(byte level, struct char_data *ch,
  struct char_data *victim, int dir)
{
  struct affected_type af;
  struct char_data *cd;


  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }


  /* obj is really the direction that the death wishes to travel in */

  cd = read_mobile(CREEPING_DEATH, VIRTUAL);
  if (!cd) {
    send_to_char("None available\n\r", ch);
    return;
  }

  char_to_room(cd, ch->in_room);
  cd->points.max_hit += (number(1,4)*100)+600;

  cd->points.hit = cd->points.max_hit;

  act("$n makes a horrid coughing sound", FALSE, ch, 0, 0, TO_ROOM);
  send_to_char("You feel an incredibly nasty feeling inside\n\r", ch);
  
  act("A huge gout of poisonous insects spews forth from $n's mouth!", 
      FALSE, ch, 0, 0, TO_ROOM);
  send_to_char("A huge gout of insects spews out of your mouth!\n\r",ch);
  send_to_char("My GOD thats disgusting!!!!!!!!!!\n\r", ch);

  act("The insects coalesce into a solid mass - $n", FALSE, ch, 0, 0, TO_ROOM);

  cd->generic = dir;

  /* move the creeping death in the proper direction */
  
  do_move(cd, "\0", dir);

  GET_POS(ch) = POSITION_STUNNED;
  
  af.type      = SPELL_CREEPING_DEATH;
  af.duration  = 2;
  af.modifier  = 10500;
  af.location  = APPLY_SPELLFAIL;
  af.bitvector = 0;
  affect_to_char(ch, &af);

}


void spell_commune(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct char_data *c;
  struct room_data *rp, *dp;
  char buf[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];


  /* look up the creatures in the mob list, find the ones in
     this zone, in rooms that are outdoors, and tell the 
     caster about them */

  buffer[0] = '\0';
  buf[0] = '\0';

  dp = real_roomp(ch->in_room);
  if (!dp) return;
  if (IS_SET(dp->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r", ch);
    return;
  }

  for (c = character_list; c; c = c->next) {
    rp = real_roomp(c->in_room);
    if (!rp) return;

    if (rp->zone == dp->zone) {
      if (!IS_SET(rp->room_flags, INDOORS)) {
	sprintf(buf, "%s is in %s\n\r", (IS_NPC(c)?c->player.short_descr:GET_NAME(c)),rp->name);
	if (strlen(buf)+strlen(buffer) > MAX_STRING_LENGTH-2)
	  break;
	strcat(buffer, buf);
	strcat(buffer, "\r");	
      }
    }
  }

  page_string(ch->desc, buffer, 1);

}

#define ANISUM1  9007
#define ANISUM2  9014
#define ANISUM3  9021

void spell_animal_summon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  struct char_data *mob;
  int num, i;
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

  if (IS_SET(rp->room_flags, INDOORS)) {
    send_to_char("You can only do this outdoors\n", ch);
    return;
  }


  if (affected_by_spell(ch, SPELL_ANIMAL_SUM_1)) {
    send_to_char("You can only do this once every 48 hours!\n\r", ch);
    return;
  }

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r", ch);
    return;
  }



  switch(level) {
  case 1:
    num = ANISUM1;
    break;
  case 2:
    num = ANISUM2;
    break;
  case 3:
    num = ANISUM3;
    break;
  }


  act("$n performs a complicated ritual!", TRUE, ch, 0, 0, TO_ROOM);
  act("You perform the ritual of summoning", TRUE, ch, 0, 0, TO_CHAR);

  for (i=0;i<5;i++) {

    mob = read_mobile(num+number(0,5), VIRTUAL);

    if (!mob) continue;

    char_to_room(mob, ch->in_room);
    act("$n strides into the room", FALSE, mob, 0, 0, TO_ROOM);
  if(too_many_followers(ch)){
        act("$N takes one look at the size of your posse and justs says no!",
            TRUE, ch, 0, victim, TO_CHAR);
        act("You take one look at the size of $n's posse and just say no!",
            TRUE, ch, 0, victim, TO_ROOM);

  } else {

    /* charm them for a while */
    if (mob->master)
      stop_follower(mob);

    add_follower(mob, ch);

    af.type      = SPELL_CHARM_PERSON;

    if (IS_PC(ch) || ch->master) {
      af.duration  = GET_CHR(ch)+10;
      af.modifier  = 0;
      af.location  = 0;
      af.bitvector = AFF_CHARM;
      affect_to_char(mob, &af);
      
    } else {
      SET_BIT(mob->specials.affected_by, AFF_CHARM);
    }
  }
    if (IS_SET(mob->specials.act, ACT_AGGRESSIVE)) {
      REMOVE_BIT(mob->specials.act, ACT_AGGRESSIVE);
    }
    if (!IS_SET(mob->specials.act, ACT_SENTINEL)) {
      SET_BIT(mob->specials.act, ACT_SENTINEL);
    }
    
  }
  
  af.type =      SPELL_ANIMAL_SUM_1;
  af.duration  = 36;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char(ch, &af);


}

#define FIRE_ELEMENTAL  40

void spell_elemental_summoning(byte level, struct char_data *ch,
  struct char_data *victim, int spell)
{
  int vnum;
  struct char_data *mob;
  struct affected_type af;

  if (affected_by_spell(ch, spell)) {
    send_to_char("You can only do this once per 24 hours\n\r", ch);
    return;
  }

  vnum = spell - SPELL_FIRE_SERVANT;
  vnum += FIRE_ELEMENTAL;

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r", ch);
    return;
  }

  mob = read_mobile(vnum, VIRTUAL);
  
  if (!mob) {
    send_to_char("None available\n\r", ch);
    return;
  }
  
  act("$n performs a complicated ritual!", TRUE, ch, 0, 0, TO_ROOM);
  act("You perform the ritual of summoning", TRUE, ch, 0, 0, TO_CHAR);

  char_to_room(mob, ch->in_room);
  act("$n appears through a momentary rift in the ether!", 
      FALSE, mob, 0, 0, TO_ROOM);
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
      
      if (IS_PC(ch) || ch->master) {
	af.duration  = 48;
	af.modifier  = 0;
	af.location  = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char(mob, &af);
	
      } else {
	SET_BIT(mob->specials.affected_by, AFF_CHARM);
      }
    }

  af.type =      spell;
  af.duration  = 24;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char(ch, &af);

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


void spell_reincarnate(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct char_data *newch;
  struct char_file_u st;
  struct descriptor_data *d;
  FILE *fl;  
  
  if (!obj) return;

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }

  if (real_roomp(ch->in_room)->sector_type != SECT_FOREST) {
    send_to_char("You must cast this spell in the forest!\n\r", ch);
    return;
  }
  
  if (IS_CORPSE(obj)) {

    if (obj->char_vnum) {
      send_to_char("This spell only works on players\n\r", ch);
      return;
    }
    
    if (obj->char_f_pos) {

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
	st.points.exp *= 2;
	st.talks[2] = TRUE;
	st.abilities.con -= 1;

	st.race = GetNewRace(&st);

      act("The forest comes alive with the sounds of birds and animals",
	    TRUE, ch, 0, 0, TO_CHAR);
      act("The forest comes alive with the sounds of birds and animals",
	    TRUE, ch, 0, 0, TO_ROOM);
	act("$p dissappears in the blink of an eye.", 
	    TRUE, ch, obj, 0, TO_CHAR);
	act("$p dissappears in the blink of an eye.", 
	    TRUE, ch, obj, 0, TO_ROOM);
	GET_MANA(ch) = 1;
	GET_MOVE(ch) = 1;
	GET_HIT(ch) = 1;
	GET_POS(ch) = POSITION_SITTING;
	act("$n collapses from the effort!",TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("You collapse from the effort\n\r",ch);

        fseek(fl, obj->char_f_pos * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);
	ObjFromCorpse(obj);	

	CREATE(newch, struct char_data, 1);
	clear_char(newch);

	store_to_char(&st, newch);

	reset_char(newch);

	newch->next = character_list;
	character_list = newch;

	char_to_room(newch, ch->in_room);
	newch->invis_level = 51;

	set_title(newch);
	GET_HIT(newch) = 1;
	GET_MANA(newch) = 1;
	GET_MOVE(newch) = 1;
	GET_POS(newch) = POSITION_SITTING;
	save_char(newch, AUTO_RENT);

	/* if they are in the descriptor list, suck them into the game
	   */

	  for (d = descriptor_list;d;d=d->next) {
	     if (d->character && (strcmp(GET_NAME(d->character),
	                                GET_NAME(newch))==0)) {
	        if (STATE(d) != CON_PLYNG) {
		  free_char(d->character);
		  d->character = newch;
		  STATE(d) = CON_PLYNG;
		  newch->desc = d;
		  send_to_char("You awake to find yourself changed\n\r",
			       newch);
		  break;
		}
	     }
	  }

      } else {
	send_to_char
	  ("The spirit does not have the strength to be reincarnated\n\r", ch);
      }
      fclose(fl);
    }
  }  
}

void spell_charm_veggie(byte level, struct char_data *ch,
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

  if (!IsVeggie(victim)) {
    send_to_char("This can only be used on plants!\n\r", ch);
    return;
  }

  if (GetMaxLevel(victim) > GetMaxLevel(ch) + 10) {
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
    
    if (GET_INT(victim))
      af.duration  = 24*GET_CHR(ch)/GET_INT(victim);
    else
      af.duration  = 24*18;
    
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(victim, &af);
    
    act("Isn't $n just such a nice fellow?",FALSE,ch,0,victim,TO_VICT);
  }
}


void spell_veggie_growth(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;


  if (!IsVeggie(victim)) {
    send_to_char("Thats not a plant-creature!\n\r", ch);
    return;
  }

  if (affected_by_spell(victim, SPELL_VEGGIE_GROWTH)) {
    act("$N is already affected by that spell",FALSE, ch, 0, victim, TO_CHAR);
    return;
  }

  if (GetMaxLevel(victim)*2 > GetMaxLevel(ch)) {
    send_to_char("You can't make it more powerful than you!\n\r", ch);
    return;
  }

  if (IS_PC(victim)) {
    send_to_char("It would be in bad taste to cast that on a player\n\r", ch);
    return;
  }

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }



  act("$n grows to double $s original size!", FALSE, victim,0,0,TO_ROOM);
  act("You grow to double your original size!", FALSE,victim,0,0,TO_CHAR);

  af.type      = SPELL_VEGGIE_GROWTH;
  af.duration  = 2*level;
  af.modifier  = GET_MAX_HIT(victim);
  af.location  = APPLY_HIT;
  af.bitvector = AFF_GROWTH;
  affect_to_char(victim, &af);

  af.type      = SPELL_VEGGIE_GROWTH;
  af.duration  = 2*level;
  af.modifier  = 5;
  af.location  = APPLY_HITNDAM;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.type      = SPELL_VEGGIE_GROWTH;
  af.duration  = 2*level;
  af.modifier  = 3;
  af.location  = APPLY_SAVE_ALL;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  GET_LEVEL(victim, WARRIOR_LEVEL_IND) = 2*GetMaxLevel(victim);

}

#define SAPLING  45

void spell_tree(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct char_data *mob;
  int mobn;


  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r", ch);
    return;
  }


  mobn = SAPLING;
  if (level > 15) {
    mobn++;
  }
  if (level > 23) {
    mobn++;
  }
  if (level > 30) {
    mobn++;
  }
  if (level > 40) {
    mobn++;
  }
  mob = read_mobile(mobn, VIRTUAL);
  if (mob) {
    spell_poly_self(level, ch, mob, 0);
  } else {
    send_to_char("You couldn't summon an image of that creature\n\r", ch);
  }
  return;

}

#define LITTLE_ROCK  50

void spell_animate_rock(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj)
{
  struct char_data *mob;
  struct affected_type af;
  int mobn=LITTLE_ROCK;

  if (ITEM_TYPE(obj) != ITEM_ROCK) {
    send_to_char("Thats not the right kind of rock\n\r", ch);
    return;
  }

  /* get the weight of the rock, make the follower based on the weight */
  
  if (GET_OBJ_WEIGHT(obj) > 20)
    mobn++;
  if (GET_OBJ_WEIGHT(obj) > 40)
    mobn++;
  if (GET_OBJ_WEIGHT(obj) > 80)
    mobn++;
  if (GET_OBJ_WEIGHT(obj) > 160)
    mobn++;
  if (GET_OBJ_WEIGHT(obj) > 320)
    mobn++;

  mob = read_mobile(mobn, VIRTUAL);
  if (mob) {

    char_to_room(mob, ch->in_room);
    /* charm them for a while */
    if (mob->master)
      stop_follower(mob);
    
    add_follower(mob, ch);

    af.type      = SPELL_ANIMATE_ROCK;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(mob, &af);
    
    af.type      = SPELL_CHARM_PERSON;
    
    if (IS_PC(ch) || ch->master) {
      af.duration  = 24;
      af.modifier  = 0;
      af.location  = 0;
      af.bitvector = AFF_CHARM;
      affect_to_char(mob, &af);
      
    } else {
      SET_BIT(mob->specials.affected_by, AFF_CHARM);
    }
    
    /*
      get rid of aggressive, add sentinel
      */
    
    if (IS_SET(mob->specials.act, ACT_AGGRESSIVE)) {
      REMOVE_BIT(mob->specials.act, ACT_AGGRESSIVE);
    }
    if (!IS_SET(mob->specials.act, ACT_SENTINEL)) {
      SET_BIT(mob->specials.act, ACT_SENTINEL);
    }
    
    extract_obj(obj);

  } else {
    send_to_char("Sorry, the spell isn't working today\n\r", ch);
    return;
  }

}


void spell_travelling(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if ( affected_by_spell(victim, SPELL_TRAVELLING) )
		return;

  af.type      = SPELL_TRAVELLING;
  af.duration  = level;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_TRAVELLING;

  affect_to_char(victim, &af);
  act("$n seems fleet of foot", FALSE, victim, 0, 0, TO_ROOM);
  send_to_char("You feel fleet of foot.\n\r", victim);
}

void spell_animal_friendship(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));

  if ( affected_by_spell(ch, SPELL_ANIMAL_FRIENDSHIP) ) {
    send_to_char("You can only do this once per day\n\r", ch);
    return;
  }

  if (IS_GOOD(victim) || IS_EVIL(victim))  {
    send_to_char("Only neutral mobs allowed\n\r", ch);
    return;
  }

  if (!IsAnimal(victim)) {
    send_to_char("Thats no animal!\n\r", ch);
    return;
  }

  if (GetMaxLevel(ch) < GetMaxLevel(victim)) {
    send_to_char("You do not have enough willpower to charm that yet\n\r", ch);
    return;
  }

  if (GetMaxLevel(victim) > 10+GetMaxLevel(ch)/2) {
    send_to_char("That creature is too powerful to charm\n\r", ch);
    return;
  }  

    if (too_many_followers(ch)) {
      act("$N takes one look at the size of your posse and justs says no!",
          TRUE, ch, ch->equipment[WIELD], victim, TO_CHAR);
      act("$N takes one look at the size of $n's posse and just says no!",
          TRUE, ch, ch->equipment[WIELD], victim, TO_ROOM);
      return;
    }


  if (IsImmune(victim, IMM_CHARM)) {
      return;
  }

  if (!saves_spell(victim, SAVING_SPELL))
    return;

  if (victim->master)
    stop_follower(victim);

  add_follower(victim, ch);

  af.type      = SPELL_ANIMAL_FRIENDSHIP;
  af.duration  = 24;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char(ch, &af);


  af.type      = SPELL_ANIMAL_FRIENDSHIP;
  af.duration  = 36;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_CHARM;
  affect_to_char(victim, &af);
      
  /*
    get rid of aggressive, add sentinel
    */
  REMOVE_BIT(victim->specials.act, ACT_AGGRESSIVE);

  SET_BIT(victim->specials.act, ACT_SENTINEL);  

}

void spell_invis_to_animals(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(ch && victim);
  
  if (!affected_by_spell(victim, SPELL_INVIS_TO_ANIMALS)) {
      
    act("$n seems to fade slightly.", TRUE, victim,0,0,TO_ROOM);
    send_to_char("You vanish, sort of.\n\r", victim);
      
    af.type      = SPELL_INVIS_TO_ANIMALS;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = APPLY_BV2;
    af.bitvector = AFF2_ANIMAL_INVIS;
    affect_to_char(victim, &af);   
  }
}


void spell_slow_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(ch && victim);
  
  if (affected_by_spell(victim, SPELL_POISON)) {
      
    act("$n seems to fade slightly.", TRUE, victim,0,0,TO_ROOM);
    send_to_char("You feel a bit better!.\n\r", victim);
      
    af.type      = SPELL_SLOW_POISON;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char(victim, &af);   
  }
}


void spell_snare(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }

  if (real_roomp(ch->in_room)->sector_type != SECT_FOREST) {
    send_to_char("You must cast this spell in the forest!\n\r", ch);
    return;
  }

  /* if victim fails save, movement = 0 */
  if (!saves_spell(victim, SAVING_SPELL)) {
    act("Roots and vines entangle your feet!", FALSE, victim, 0,0, TO_CHAR);
    act("Roots and vines entangle $n's feet!", FALSE, victim, 0,0, TO_ROOM);
    GET_MOVE(victim)=0;
  } else {
    FailSnare(ch, victim);
  }
}

void spell_entangle(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r",ch);
    return;
  }

  if (real_roomp(ch->in_room)->sector_type != SECT_FOREST) {
    send_to_char("You must cast this spell in the forest!\n\r", ch);
    return;
  }

  /* if victim fails save, paralyzed for a very short time */
  if (!saves_spell(victim, SAVING_SPELL)) {
    act("Roots and vines entwine around you!", FALSE, victim, 0,0, TO_CHAR);
    act("Roots and vines surround $n!", FALSE, victim, 0,0, TO_ROOM);
    GET_MOVE(victim)=0;
    
    af.type      = SPELL_ENTANGLE;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_PARALYSIS;
    affect_to_char(victim, &af);   

  } else {
    FailSnare(ch, victim);
  }
}

void spell_barkskin(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  assert((level >= 0) && (level <= ABS_MAX_LVL));
  
  if (!affected_by_spell(victim, SPELL_BARKSKIN)) {

    af.type      = SPELL_BARKSKIN;
    af.duration  = 24;
    af.modifier  = -10;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    af.type      = SPELL_BARKSKIN;
    af.duration  = 24;
    af.modifier  = -1;
    af.location  = APPLY_SAVE_ALL;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    af.type      = SPELL_BARKSKIN;
    af.duration  = 24;
    af.modifier  = 1;
    af.location  = APPLY_SAVING_SPELL;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    send_to_char("Your skin takes on a rough, bark-like texture.\n\r", 
		 victim);
    act("$n's skin takes on a rough, bark-like texture", FALSE, ch, 0, 0, 
	TO_ROOM);

  } else {
    send_to_char("Nothing new seems to happen\n\r", ch);
  }
}

void spell_gust_of_wind(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct char_data *tmp_victim, *temp;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  send_to_char("You wave your hands, and a gust of wind boils forth!\n\r", 
	       ch);
  act("$n sends a gust of wind towards you!\n\r",
	  FALSE, ch, 0, 0, TO_ROOM);

  for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
       tmp_victim = temp ) {
    temp = tmp_victim->next_in_room;
    if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
      if ((GetMaxLevel(tmp_victim)>LOW_IMMORTAL) &&(!IS_NPC(tmp_victim)))
	return;
      if (!in_group(ch, tmp_victim)) {
	if ( saves_spell(tmp_victim, SAVING_SPELL) )
	  continue;
	GET_POS(tmp_victim) = POSITION_SITTING;
	act("$n is sent sprawling.", FALSE, tmp_victim, 0, 0, TO_ROOM);
      } else {
	act("You are able to avoid the swirling gust.\n\r",
	    FALSE, ch, 0, tmp_victim, TO_VICT);
      }
    }
  }
}


void spell_silence(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  extern struct index_data *mob_index;
  
  assert(ch && victim);

  if (!saves_spell(victim, SAVING_SPELL)) {
    act("$n ceases to make noise!", TRUE, victim,0,0,TO_ROOM);
    send_to_char("You can't hear anything!.\n\r", victim);
    
    af.type      = SPELL_SILENCE;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_SILENCE;
    affect_to_char(victim, &af);
  }
  else {
    send_to_char("You feel quiet for a moment, but the effect fades\n\r", 
		 victim);
    if (!IS_PC(victim)) {
      if ((!victim->specials.fighting)) {
	set_fighting(victim, ch);
	if (mob_index[victim->nr].func) {
	  (*mob_index[victim->nr].func)(victim, 0,"");
	}
      }
    }
  }
}

void spell_warp_weapon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  
  assert(ch && (victim || obj));

  if (!obj) {
    if (!victim->equipment[WIELD]) {
      act("$N doesn't have a weapon wielded!", FALSE, ch, 0,
	  victim, TO_CHAR);
      return;
    }
    obj = victim->equipment[WIELD];
  }

  act("$p is warped and twisted by the power of the spell", FALSE,
      ch, obj, 0, TO_CHAR);
  act("$p is warped and twisted by the power of the spell", FALSE,
      ch, obj, 0, TO_ROOM);
  DamageOneItem(victim, BLOW_DAMAGE, obj);

  if (!IS_PC(victim))
    if (!victim->specials.fighting)
      set_fighting(victim,ch);
}



void spell_heat_stuff(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  
  assert(victim);

  if (affected_by_spell(victim, SPELL_HEAT_STUFF)) {
    send_to_char("Already affected\n\r", victim);
    return;
  }
  
  if (HitOrMiss(ch, victim, CalcThaco(ch))) {
    af.type = SPELL_HEAT_STUFF;
    af.duration = level;
    af.modifier = -2;
    af.location = APPLY_DEX;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    af.type = SPELL_HEAT_STUFF;
    af.duration = level;
    af.modifier = 0;
    af.location = APPLY_BV2;
    af.bitvector = AFF2_HEAT_STUFF;
    
    affect_to_char(victim, &af);
    send_to_char("Your armor starts to sizzle and smoke\n\r", victim);     
    act("$N's armor starts to sizzle\n\r", FALSE, ch, 0, victim, TO_CHAR);
    act("$N's armor starts to sizzle\n\r", FALSE, ch, 0, victim, TO_NOTVICT);

    if (!IS_PC(victim))
      if (!victim->specials.fighting)
	set_fighting(victim,ch);
  }
}

#define DUST_DEVIL 60

void spell_dust_devil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int vnum;
  struct char_data *mob;
  struct affected_type af;

  if (affected_by_spell(ch, SPELL_DUST_DEVIL)) {
    send_to_char("You can only do this once per 24 hours\n\r", ch);
    return;
  }

  if (IS_SET(real_roomp(ch->in_room)->room_flags, INDOORS)) {
    send_to_char("You can't cast this spell indoors!\n\r", ch);
    return;
  }
  

  vnum = DUST_DEVIL;

  mob = read_mobile(vnum, VIRTUAL);
  
  if (!mob) {
    send_to_char("None available\n\r", ch);
    return;
  }

  act("$n performs a complicated ritual!", TRUE, ch, 0, 0, TO_ROOM);
  act("You perform the ritual of summoning", TRUE, ch, 0, 0, TO_CHAR);

  char_to_room(mob, ch->in_room);
  act("$n appears through a momentary rift in the ether!", 
      FALSE, mob, 0, 0, TO_ROOM);

  /* charm them for a while */
  if (mob->master)
    stop_follower(mob);
  
  add_follower(mob, ch);
  
  af.type      = SPELL_CHARM_PERSON;
  
  if (IS_PC(ch) || ch->master) {
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(mob, &af);
    
  } else {
    SET_BIT(mob->specials.affected_by, AFF_CHARM);
  }


  af.type =      SPELL_DUST_DEVIL;
  af.duration  = 24;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char(ch, &af);

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

void spell_sunray(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
   struct char_data *t, *n;
   int dam;

  /*
    blind all in room
  */
  for (t= real_roomp(ch->in_room)->people;t;t=n) {
     n = t->next_in_room;
     if (!in_group(ch, t) && !IS_IMMORTAL(t)) {
        spell_blindness(level, ch, t, obj);
        /*
         hit undead target
         */
        if (t == victim) {
          if (GET_RACE(victim) == RACE_UNDEAD || 
	      GET_RACE(victim) == RACE_VEGMAN){
	    dam = dice(6,8);
	    if (saves_spell(victim, SAVING_SPELL)&&
		(GET_RACE(victim)!=RACE_VEGMAN))
	      dam >>= 1;
	    damage(ch, victim, dam, SPELL_SUNRAY);
	  }
	} else {
	  /*
	    damage other undead in room
	    */
	  if (GET_RACE(t) == RACE_UNDEAD || 
	      GET_RACE(t) == RACE_VEGMAN) {
	    dam = dice(3,6);
	    if (saves_spell(t, SAVING_SPELL)&&
		(GET_RACE(t)!=RACE_VEGMAN))
	      dam = 0;
	    damage(ch, t, dam, SPELL_SUNRAY);
	  }
	}
      }
   }
}

void spell_know_monster(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  char buf[256], buf2[256];
  int exp, lev, hits;

  extern char *pc_class_types[];
  extern char *immunity_names[];
  extern char *RaceName[];

/*
  depending on level, give info.. sometimes inaccurate
*/

  if (!IS_PC(victim)) {
    sprintf(buf,"$N belongs to the %s race.", RaceName[GET_RACE(victim)]);
    act(buf,FALSE, ch, 0, victim, TO_CHAR);
    if (level > 5) {
      exp = GetApprox(GET_EXP(victim), 40+level);
      sprintf(buf, "$N is worth approximately %d experience", exp);
      act(buf,FALSE, ch, 0, victim, TO_CHAR);
    }
    if (level > 10) {
      lev = GetApprox(GetMaxLevel(victim), 40+level);
      sprintf(buf, "$N fights like a %d level warrior, you think", lev);
      act(buf,FALSE, ch, 0, victim, TO_CHAR);      
    }
    if (level > 15) {
      if (IS_SET(victim->hatefield, HATE_RACE)) {
	sprintf(buf, "$n seems to hate the %s race", RaceName[victim->hates.race]);
	act(buf,FALSE, ch, 0, victim, TO_CHAR);      
      }
      if (IS_SET(victim->hatefield, HATE_CLASS)) {
	sprintbit((unsigned)victim->hates.class, pc_class_types, buf2);
	sprintf(buf, "$n seems to hate the %s class(es)", buf2);
	act(buf,FALSE, ch, 0, victim, TO_CHAR);      
      }
    }
    if (level > 20) {
      hits = GetApprox(GET_MAX_HIT(victim), 40+level);
      sprintf(buf,"$N probably has about %d hit points", hits);
      act(buf,FALSE, ch, 0, victim, TO_CHAR);      
    }
    if (level > 25) {
      if (victim->susc) {
	sprintbit(victim->susc, immunity_names, buf2);
	sprintf(buf, "$N is susceptible to %s\n\r", buf2);
	act(buf,FALSE, ch, 0, victim, TO_CHAR);
      }
    }
    if (level > 30) {
      if (victim->immune) {
	sprintbit(victim->immune, immunity_names, buf2);
	sprintf(buf, "$N is resistant to %s\n\r", buf2);
	act(buf,FALSE, ch, 0, victim, TO_CHAR);
      }
    }
    if (level > 35) {
      if (victim->M_immune) {
	sprintbit(victim->M_immune, immunity_names, buf2);
	sprintf(buf, "$N is immune to %s\n\r", buf2);
	act(buf,FALSE, ch, 0, victim, TO_CHAR);
      }
    }
    if (level > 40) {
      int att;
      att = GetApprox((int)victim->mult_att, 30+level);
      sprintf(buf,"$N gets approx %d.0 attack(s) per round", att);
      act(buf,FALSE, ch, 0, victim, TO_CHAR);
    }
    if (level > 45) {
      int no, s;
      no = GetApprox(victim->specials.damnodice, 30+level);
      s = GetApprox(victim->specials.damsizedice, 30+level);

      sprintf(buf,"Each does about %dd%d points of damage", 
	      no, s);
      act(buf,FALSE, ch, 0, victim, TO_CHAR);
    }
  } else {
    send_to_char("Thats not a REAL monster\n\r", ch);
    return;
  }

}

void spell_find_traps(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
/*
  raise their detect traps skill
*/
  af.type =      SPELL_FIND_TRAPS;
  af.duration  = level;
  af.modifier  = 15+level;
  af.location  = APPLY_FIND_TRAPS;
  af.bitvector = 0;
  affect_to_char(ch, &af);

}

void spell_firestorm(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
/*
  a-e -    2d8+level
*/
  int dam;

  assert(ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));

  dam = dice(2,8) + level + 1;

  send_to_char("Searing flame surround you!\n\r", ch);
  act("$n sends a firestorm whirling across the room!\n\r",
	  FALSE, ch, 0, 0, TO_ROOM);

  AreaDamage(ch, dam,  SPELL_BURNING_HANDS, 
	     "You are seared by the burning flame!\n\r",
	     "You are able to avoid the flames!\n\r",
	     "", FALSE, TRUE);
#if 0
  for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim; 
       tmp_victim = temp ) {
    temp = tmp_victim->next_in_room;
    if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
      if ((GetMaxLevel(tmp_victim)>LOW_IMMORTAL) && (!IS_NPC(tmp_victim)))
	return;
      if (!in_group(ch, tmp_victim)) {
	act("You are seared by the burning flame!\n\r",
	    FALSE, ch, 0, tmp_victim, TO_VICT);
	if ( saves_spell(tmp_victim, SAVING_SPELL) )
	  dam >>= 1;
	heat_blind(tmp_victim);
	MissileDamage(ch, tmp_victim, dam, SPELL_BURNING_HANDS);
      } else {
	act("You are able to avoid the flames!\n\r",
	    FALSE, ch, 0, tmp_victim, TO_VICT);
	heat_blind(tmp_victim);
      }
    }
  }
#endif
}


void spell_teleport_wo_error(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int location;
  struct room_data *rp;

/* replaces the current functionality of astral walk */
  assert(ch && victim);
  
  location = victim->in_room;
  rp = real_roomp(location);
  
  if (GetMaxLevel(victim) > MAX_MORT || 
      !rp ||
      IS_SET(rp->room_flags,  PRIVATE) ||
      IS_SET(rp->room_flags,  NO_SUM) ||
      IS_SET(rp->room_flags,  NO_MAGIC) ||
      (IS_SET(rp->room_flags,  TUNNEL) && 
       (MobCountInRoom(rp->people) > rp->moblim)))  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  if (!IsOnPmp(location)) {
    send_to_char("That place is on an extra-dimensional plane!\n", ch);
    return;
  }
  if (!IsOnPmp(ch->in_room)) {
    send_to_char("You're on an extra-dimensional plane!\n\r", ch);
    return;
  }

  
  if (dice(1,20) == 20) {
    send_to_char("You fail the magic, and spin out of control!\n\r", ch);
    spell_teleport(level, ch, ch, 0);
    return;
  } else {
    act("$n opens a door to another dimension and steps through!",
	FALSE,ch,0,0,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, location);
    act("You are blinded for a moment as $n appears in a flash of light!",
	FALSE,ch,0,0,TO_ROOM);
    do_look(ch, "",15);
    check_falling(ch);
    
    if (IS_SET(real_roomp(ch->in_room)->room_flags, DEATH) && 
	GetMaxLevel(ch) < LOW_IMMORTAL) {
      NailThisSucker(ch);
      return;
    }
  }

}

#define PORTAL 31

void spell_portal(byte level, struct char_data *ch,
  struct char_data *tmp_ch, struct obj_data *obj)
{
  /* create a magic portal */
  struct obj_data *tmp_obj;
  struct extra_descr_data *ed;
  struct room_data *rp, *nrp;
  char buf[512];

  assert(ch);
  assert((level >= 0) && (level <= ABS_MAX_LVL));


  /*
    check target room for legality.
   */
  rp = real_roomp(ch->in_room);
  tmp_obj = read_object(PORTAL, VIRTUAL);
  if (!rp || !tmp_obj) {
    send_to_char("The magic fails.\n\r", ch);
    return;
  }

  if (IS_SET(rp->room_flags, NO_SUM) || IS_SET(rp->room_flags, NO_MAGIC)
      || IS_SET(rp->room_flags, PRIVATE)) {
    send_to_char("Eldritch wizardry obstructs thee.\n\r", ch);
    return;
  }

  if (IS_SET(rp->room_flags, TUNNEL)) {
    send_to_char("There is no room in here to summon!\n\r", ch);
    return;
  }

  if (!(nrp = real_roomp(tmp_ch->in_room))) {
    char str[180];
    sprintf(str, "%s not in any room.", GET_NAME(tmp_ch));
    log(str);
    send_to_char("Your magic cannot locate the target.\n\r", ch);
    return;
  }

  if (IS_SET(nrp->room_flags, NO_SUM) ||
      IS_SET(nrp->room_flags, PRIVATE) ||
      IS_SET(nrp->room_flags, TUNNEL)  ||
      IS_SET(nrp->room_flags, NO_MAGIC)) {
    send_to_char("Ancient Magiks bar your path.\n\r", ch);
    return;
  }

  if (!IsOnPmp(ch->in_room)) {
    send_to_char("You're on an extra-dimensional plane!\n\r", ch);
    return;
  }

  if (!IsOnPmp(tmp_ch->in_room)) {
    send_to_char("They're on an extra-dimensional plane!\n\r", ch);
    return;
  }

  sprintf(buf, "Through the mists of the portal, you can faintly see %s", nrp->name);

  CREATE(ed , struct extra_descr_data, 1);
  ed->next = tmp_obj->ex_description;
  tmp_obj->ex_description = ed;
  CREATE(ed->keyword, char, strlen(tmp_obj->name) + 1);
  strcpy(ed->keyword, tmp_obj->name);
  ed->description = strdup(buf);

  tmp_obj->obj_flags.value[0] = level/5;
  tmp_obj->obj_flags.value[1] = tmp_ch->in_room;

  obj_to_room(tmp_obj,ch->in_room);

  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_CHAR);

}

#define MOUNT_ONE 65
#define MOUNT_GOOD 69
#define MOUNT_EVIL 70
#define MOUNT_NEUT 71

void spell_mount(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct char_data *m;
  int mnr;

/* create a ridable mount, and automatically mount said creature */

  mnr = MOUNT_ONE;
  if (level < 30) {
    if (level < 12) 
      mnr++;
    if (level < 18) 
      mnr++;
    if (level < 24)
      mnr++;
  } else {
    if (IS_EVIL(ch)) {
      mnr = MOUNT_EVIL;
    } else if (IS_GOOD(ch)) {
      mnr = MOUNT_GOOD;
    } else {
      mnr = MOUNT_NEUT;
    }
  }

  m = read_mobile(mnr, VIRTUAL);
  if (m) {
    char_to_room(m, ch->in_room);
    act("In a flash of light, $N appears.", FALSE, ch, 0, m, TO_CHAR);
    act("In a flash of light, $N appears, and $n hops on $s back.", FALSE,
	ch, 0, m, TO_ROOM);
    send_to_char("You hop on your mount's back.\n\r", ch);
    MOUNTED(ch) = m;
    RIDDEN(m) = ch;
    GET_POS(ch) = POSITION_MOUNTED;
  } else {
    send_to_char("The horses aren't in database!\n\r", ch);
    return;
  }
}

void spell_dragon_ride(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if (affected_by_spell(ch, SPELL_DRAGON_RIDE)) {
    send_to_char("Already affected\n\r", ch);
    return;
  }

  af.type = SPELL_DRAGON_RIDE;
  af.duration = level;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = AFF_DRAGON_RIDE;
  affect_to_char(ch, &af);
}

void spell_thorn_spray(byte level, struct char_data *ch,
       struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert(victim && ch);
  assert((level >= 1) && (level <= ABS_MAX_LVL));


  if(obj) {                     /* only the spell form passes in the obj */
    obj_from_char(obj);
    extract_obj(obj);
    act("$p disappears in a puff of smoke!",TRUE,ch,obj,0,TO_ROOM);
    act("$p disappears in a puff of smoke!",TRUE,ch,obj,0,TO_CHAR);
  }

  dam = dice((int)(level / 2)+1,5);

  if (affected_by_spell(victim,SPELL_SHIELD))
    dam = 0;

  MissileDamage(ch, victim, dam, SPELL_THORN_SPRAY);
}
 
void spell_resist_hold(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_HOLD)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a dim swirling sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A dim swirling sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to holding spells */

    af.type      = SPELL_RESIST_HOLD;
    af.duration  = 4;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 4096;
    affect_to_char(ch, &af);
  }
}

void spell_resist_electricity(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if (affected_by_spell(ch, SPELL_RESIST_ELECTRICITY)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }

  else {
    act("$n is enclosed in a dim blue sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A dim blue sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to electricity */

    af.type      = SPELL_RESIST_ELECTRICITY;
    af.duration  = 4;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 4;
    affect_to_char(ch, &af);
  }
}

void spell_resist_cold(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_COLD)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a dim white sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A dim white sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to cold */

    af.type      = SPELL_RESIST_COLD;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 2;
    affect_to_char(ch, &af);
  }
}

void spell_resist_drain(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_DRAIN)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a bright glowing sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A bright glowing sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to draining */

    af.type      = SPELL_RESIST_DRAIN;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 512;
    affect_to_char(ch, &af);
  }
}

void spell_resist_poison(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_POISON)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a dark purple sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A dark purple sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to poisoning */

    af.type      = SPELL_RESIST_POISON;
    af.duration  = 2;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 256;
    affect_to_char(ch, &af);
  }
}

void spell_resist_acid(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_ACID)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else  {
    act("$n is enclosed in a bright green sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A bright green sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to acid */

    af.type      = SPELL_RESIST_ACID;
    af.duration  = 2;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 128;		/* acid, trust me */
    affect_to_char(ch, &af);
  }
}

void spell_resist_fire(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if (affected_by_spell(ch, SPELL_RESIST_FIRE)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else  {
    act("$n is enclosed in a dull red sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A dull red sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to fire */

    af.type      = SPELL_RESIST_FIRE;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 1;
    affect_to_char(ch, &af);
  }
}

void spell_resist_energy(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if (affected_by_spell(ch, SPELL_RESIST_ENERGY)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a matte black sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A matte black sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to energy */

    af.type      = SPELL_RESIST_ENERGY;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 8;
    affect_to_char(ch, &af);
  }
}

void spell_resist_pierce(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_PIERCE)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a stone sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A stone sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to piercing attacks */

    af.type      = SPELL_RESIST_PIERCE;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 32;
    affect_to_char(ch, &af);
  }
}

void spell_resist_slash(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_SLASH)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a steel sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A steel sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to slashing attacks*/

    af.type      = SPELL_RESIST_SLASH;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 64;
    affect_to_char(ch, &af);
  }
}

void spell_resist_blunt(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch);

  if ( affected_by_spell(ch, SPELL_RESIST_BLUNT)) {
      send_to_char("This spell is already in effect.\n\r", ch);
      return;
    }
  else {
    act("$n is enclosed in a pulsing sphere.", TRUE, ch, 0, 0, TO_ROOM);
    act("A pulsing sphere encloses you.", TRUE, ch, 0, 0, TO_CHAR);

    /* resistance to blunt attacks */

    af.type      = SPELL_RESIST_BLUNT;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = APPLY_IMMUNE;
    af.bitvector = 16;
    affect_to_char(ch, &af);
  }
}
