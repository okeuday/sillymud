/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <sys/time.h>

#include "protos.h"

#define OBJ_SAVE_FILE "pcobjs.obj"
#define OBJ_FILE_FREE "\0\0\0"

extern struct room_data *world;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern int top_of_objt;
extern struct player_index_element *player_table;
extern int top_of_p_table;



/* ************************************************************************
* Routines used for the "Offer"                                           *
************************************************************************* */

int add_obj_cost(struct char_data *ch, struct char_data *re,
                  struct obj_data *obj, struct obj_cost *cost, int hoarder)
{
  char buf[MAX_INPUT_LENGTH];
  int  temp;
  struct obj_data *hobj;

  if((cost->total_cost == 100) && (cost->no_carried == 0)) {
    hobj = obj;
      /* Add cost for an item and it's contents, and next->contents */
  }

  if (obj) {
    if ((obj->item_number > -1) && (cost->ok) && ItemEgoClash(ch,obj,0) >-5 ) {
      temp = MAX(0, obj->obj_flags.cost_per_day);
      cost->total_cost += temp;
      if (re) {
#if  0
	sprintf(buf, "%30s : %d coins/day\n\r", obj->short_description, temp);
	send_to_char(buf, ch);
#endif	
      }
      cost->no_carried++;
      hoarder = add_obj_cost(ch, re, obj->contains, cost, hoarder);
      hoarder = add_obj_cost(ch, re, obj->next_content, cost, hoarder);
    } else {
      if(ItemEgoClash(ch,obj,0) < 0 && obj->obj_flags.cost_per_day > 0) { 
	if (re) 
	  act("$p refuses to be rented with a wimp like you!",
	      TRUE,ch,obj,0,TO_CHAR);
	cost->ok = FALSE;
      }	else if (cost->ok) {
	if (re) {
	  act("$n tells you 'I refuse storing $p'",FALSE,re,obj,ch,TO_VICT);
	  cost->ok = FALSE;
	} else {
#if NODUPLICATES
#else
	  act("Sorry, but $p don't keep in storage.",FALSE,ch,obj,0,TO_CHAR);
#endif
	  cost->ok = FALSE;

	}
      }
    }
  }

return(hoarder);
  
}


bool recep_offer(struct char_data *ch,	struct char_data *receptionist,
		 struct obj_cost *cost)
{
  int i;
  char buf[MAX_INPUT_LENGTH];
  int hoarder = 0;
  
  cost->total_cost = 0; /* Minimum cost */
  cost->no_carried = 0;
  cost->ok = TRUE; /* Use if any "-1" objects */
  
  hoarder = add_obj_cost(ch, receptionist, ch->carrying, cost, hoarder);
  
  for(i = 0; i<MAX_WEAR; i++)
    hoarder = add_obj_cost(ch, receptionist, ch->equipment[i], cost, hoarder);
  
  if (!cost->ok)
    return(FALSE);

#if NEW_RENT
  cost->total_cost = 0;
#endif

  if(hoarder) {
    cost->total_cost += (int) (ch->points.gold + ch->points.bankgold)/10;
  }
  
  if (cost->no_carried == 0) {
    if (receptionist)
      act("$n tells you 'But you are not carrying anything?'",FALSE,receptionist,0,ch,TO_VICT);
    return(FALSE);
  }
  
  if (cost->no_carried > MAX_OBJ_SAVE) {
    if (receptionist) {
      sprintf(buf,"$n tells you 'Sorry, but I can't store more than %d items.",
	      MAX_OBJ_SAVE);
      act(buf,FALSE,receptionist,0,ch,TO_VICT);
    }
    return(FALSE);
  }
  
  if (HasClass(ch, CLASS_MONK)) {
    if (cost->no_carried > 20) {
      send_to_char("Your vows forbid you to carry more than 20 items\n\r", ch);
      return(FALSE);
    }
  }
  
  if (receptionist) {

    sprintf(buf, "$n tells you 'It will cost you %d coins per day'",
	    cost->total_cost);
    act(buf,FALSE,receptionist,0,ch,TO_VICT);
  
    if (cost->total_cost > GET_GOLD(ch)) {
      if (GetMaxLevel(ch) < LOW_IMMORTAL)
	act("$n tells you 'Which I can see you can't afford'",
	    FALSE,receptionist,0,ch,TO_VICT);
      else {
	act("$n tells you 'Well, since you're a God, I guess it's okay'",
	    FALSE,receptionist,0,ch,TO_VICT);
	cost->total_cost = 0;
      }
    }
  }
  
  if ( cost->total_cost > GET_GOLD(ch) )
    return(FALSE);
  else
    return(TRUE);
}


/* ************************************************************************
* General save/load routines                                              *
************************************************************************* */

void update_file(struct char_data *ch, struct obj_file_u *st)
{
  FILE *fl;
  char buf[200];

  /*
    write the aliases and bamfs:

    */
  write_char_extra(ch);
  sprintf(buf, "rent/%s", lower(ch->player.name));
#if 0
  for(p=buf;*p && *p != ' ';p++);
  *p = '\0';
  log("buf");
#endif
  if (!(fl = fopen(buf, "w")))  {
    perror("saving PC's objects");
    assert(0);  
  }
  
  rewind(fl);

  strcpy(st->owner, GET_NAME(ch));

  WriteObjs(fl, st);

  fclose(fl);
  
}


/* ************************************************************************
* Routines used to load a characters equipment from disk                  *
************************************************************************* */

void obj_store_to_char(struct char_data *ch, struct obj_file_u *st)
{
  struct obj_data *obj;
  int i, j;
  
  void obj_to_char(struct obj_data *object, struct char_data *ch);
  
  for(i=0; i<st->number; i++) {
    if (st->objects[i].item_number > -1 && 
	real_object(st->objects[i].item_number) > -1) {
      obj = read_object(st->objects[i].item_number, VIRTUAL);
      obj->obj_flags.value[0] = st->objects[i].value[0];
      obj->obj_flags.value[1] = st->objects[i].value[1];
      obj->obj_flags.value[2] = st->objects[i].value[2];
      obj->obj_flags.value[3] = st->objects[i].value[3];
      obj->obj_flags.extra_flags = st->objects[i].extra_flags;
      obj->obj_flags.weight      = st->objects[i].weight;
      obj->obj_flags.timer       = st->objects[i].timer;
      obj->obj_flags.bitvector   = st->objects[i].bitvector;

/*  new, saving names and descrips stuff o_s_t_c*/
      if (obj->name)
	free(obj->name);
      if (obj->short_description)
         free(obj->short_description);
      if (obj->description)
         free(obj->description);

      obj->name = (char *)malloc(strlen(st->objects[i].name)+1);
      obj->short_description = (char *)malloc(strlen(st->objects[i].sd)+1);
      obj->description = (char *)malloc(strlen(st->objects[i].desc)+1);

      strcpy(obj->name, st->objects[i].name);
      strcpy(obj->short_description, st->objects[i].sd);
      strcpy(obj->description, st->objects[i].desc);
/* end of new, possibly buggy stuff */
      
      for(j=0; j<MAX_OBJ_AFFECT; j++)
	obj->affected[j] = st->objects[i].affected[j];
      
      obj_to_char(obj, ch);
    }
  }
}


void load_char_objs(struct char_data *ch)
{
  FILE *fl;
  bool found = FALSE;
  float timegold;
  struct obj_file_u st;
  char buf[200];


/*
  load in aliases and poofs first
*/

  load_char_extra(ch);

  
  sprintf(buf, "rent/%s", lower(ch->player.name));

  
  /* r+b is for Binary Reading/Writing */
  if (!(fl = fopen(buf, "r+b")))  {
    log("Char has no equipment");
    return;
  }

  rewind(fl);

  if (!ReadObjs(fl, &st)) {
    log("No objects found");
    fclose(fl);
    return;
  }

  if (str_cmp(st.owner, GET_NAME(ch)) != 0) {
    log("Hmm.. bad item-file write. someone is losing thier objects");
    fclose(fl);
    return;
  }

/*
  if the character has been out for 12 real hours, they are fully healed
  upon re-entry.  if they stay out for 24 full hours, all affects are
  removed, including bad ones.
*/

    if (st.last_update + 12*SECS_PER_REAL_HOUR < time(0))
      RestoreChar(ch);

    if (st.last_update + 24*SECS_PER_REAL_HOUR < time(0))
      RemAllAffects(ch);
    
    if (ch->in_room == NOWHERE &&
	st.last_update + 1*SECS_PER_REAL_HOUR > time(0)) {
	/* you made it back from the crash in time, 1 hour grace period. */
      log("Character reconnecting.");
      found = TRUE;
    } else {
      char	buf[MAX_STRING_LENGTH];
      if (ch->in_room == NOWHERE)
	log("Char reconnecting after autorent");
#if NEW_RENT
      timegold = (int) ((100*((float)time(0) - st.last_update)) / 
			(SECS_PER_REAL_DAY));
#else
      timegold = (int) ((st.total_cost*((float)time(0) - st.last_update)) / 
			(SECS_PER_REAL_DAY));
#endif
      sprintf(buf, "Char ran up charges of %g gold in rent", timegold);
      log(buf);
      sprintf(buf, "You ran up charges of %g gold in rent.\n\r", timegold);
      send_to_char(buf, ch);
      GET_GOLD(ch) -= timegold;
      found = TRUE;    
      if (GET_GOLD(ch) < 0) {
	log("Char ran out of money in rent");
        send_to_char("You ran out of money, you deadbeat.\n\r", ch);
	GET_GOLD(ch) = 0;
	found = FALSE;
      }
    }

  fclose(fl);

  if (found)
      obj_store_to_char(ch, &st);
  else {
    ZeroRent(GET_NAME(ch));
  }
  
  /* Save char, to avoid strange data if crashing */
  save_char(ch, AUTO_RENT);


  
}


/* ************************************************************************
* Routines used to save a characters equipment from disk                  *
************************************************************************* */

/* Puts object in store, at first item which has no -1 */
void put_obj_in_store(struct obj_data *obj, struct obj_file_u *st)
{
  int j;
  struct obj_file_elem *oe;
  char buf[256];

  if (st->number>=MAX_OBJ_SAVE) {
    printf("holy shit, you want to rent more than %d items?!\n", st->number);
    return;
  }

  oe = st->objects + st->number;
  
  oe->item_number = obj_index[obj->item_number].virtual;
  oe->value[0] = obj->obj_flags.value[0];
  oe->value[1] = obj->obj_flags.value[1];
  oe->value[2] = obj->obj_flags.value[2];
  oe->value[3] = obj->obj_flags.value[3];
  
  oe->extra_flags = obj->obj_flags.extra_flags;
  oe->weight  = obj->obj_flags.weight;
  oe->timer  = obj->obj_flags.timer;
  oe->bitvector  = obj->obj_flags.bitvector;

/*  new, saving names and descrips stuff */
      if (obj->name)
         strcpy(oe->name, obj->name);
      else {
	sprintf(buf, "object %d has no name!", obj_index[obj->item_number].virtual);
	log(buf);
	
      }
	
      if (obj->short_description)
         strcpy(oe->sd, obj->short_description);
      else
	*oe->sd = '\0';
      if (obj->description)
         strcpy(oe->desc, obj->description);
      else 
	*oe->desc = '\0';

/* end of new, possibly buggy stuff */


  for(j=0; j<MAX_OBJ_AFFECT; j++)
    oe->affected[j] = obj->affected[j];

  st->number++;
}

int contained_weight(struct obj_data *container)
{
  struct obj_data *tmp;
  int	rval = 0;

  for (tmp = container->contains; tmp; tmp = tmp->next_content)
    rval += GET_OBJ_WEIGHT(tmp);
  return rval;
}

/* Destroy inventory after transferring it to "store inventory" */
void obj_to_store(struct obj_data *obj, struct obj_file_u *st,
                  struct char_data * ch, int delete)
{
  static char buf[240];
  
  if (!obj)
    return;

  obj_to_store(obj->contains, st, ch, delete);
  obj_to_store(obj->next_content, st, ch, delete);
    
  if ((obj->obj_flags.timer < 0) && (obj->obj_flags.timer != OBJ_NOTIMER)) {
#if NODUPLICATES
#else
    sprintf(buf, "You're told: '%s is just old junk, I'll throw it away for you.'\n\r", obj->short_description);
    send_to_char(buf, ch);
#endif
  } else if (obj->obj_flags.cost_per_day < 0) {

#if NODUPLICATES
#else
    if(ch != NULL) {
      sprintf(buf, "You're told: '%s is just old junk, I'll throw it away for you.'\n\r", obj->short_description);
      send_to_char(buf, ch);
    }
#endif

    if (delete) {
       if (obj->in_obj) 
	 obj_from_obj(obj);
       extract_obj(obj);
     }
  } else if (obj->item_number == -1) {
    if (delete) {
       if (obj->in_obj) 
	 obj_from_obj(obj);
       extract_obj(obj);
     }
  }else {
    int weight = contained_weight(obj);
          GET_OBJ_WEIGHT(obj) -= weight;
    put_obj_in_store(obj, st);
    GET_OBJ_WEIGHT(obj) += weight;
    if (delete) {
      if (obj->in_obj)
	obj_from_obj(obj);
      extract_obj(obj);
    }
  }
}



/* write the vital data of a player to the player file */
void save_obj(struct char_data *ch, struct obj_cost *cost, int delete)
{
  static struct obj_file_u st;
  int i;
  char buf[128];
    
  st.number = 0;
  st.gold_left = GET_GOLD(ch);

  sprintf(buf, "saving %s:%d", fname(ch->player.name), GET_GOLD(ch));
  slog(buf);

  st.total_cost = cost->total_cost;
  st.last_update = time(0);
  st.minimum_stay = 0; /* XXX where does this belong? */
  
  for(i=0; i<MAX_WEAR; i++)
    if (ch->equipment[i]) {
      if (delete) {
	obj_to_store(unequip_char(ch, i), &st, ch, delete);
      } else {
	obj_to_store(ch->equipment[i], &st, ch, delete);
      }
    }
  
  obj_to_store(ch->carrying, &st, ch, delete);
  if (delete)
    ch->carrying = 0;
  
  update_file(ch, &st);
  
}



/* ************************************************************************
* Routines used to update object file, upon boot time                     *
************************************************************************* */

void update_obj_file()
{
  FILE *fl, *char_file;
  struct obj_file_u st;
  struct char_file_u ch_st;
  long i;
  long days_passed, secs_lost;
  char buf[200];
  
  int find_name(char *name);
  extern int errno;


  if (!(char_file = fopen(PLAYER_FILE, "r+"))) {
    perror("Opening player file for reading. (reception.c, update_obj_file)");
    assert(0);
  }
  
  for (i=0; i<= top_of_p_table; i++) {
    sprintf(buf, "rent/%s", lower(player_table[i].name));
    /* r+b is for Binary Reading/Writing */
    if ((fl = fopen(buf, "r+b")) != NULL) {

      if (ReadObjs(fl, &st)) {
	if (str_cmp(st.owner, player_table[i].name) != 0) {
       sprintf(buf, "Ack!  Wrong person written into object file! (%s/%s)", st.owner, player_table[i].name);
	  log(buf);
	  abort();
	} else {
	  sprintf(buf, "   Processing %s[%d].", st.owner, i);
	  log(buf);
	  days_passed = ((time(0) - st.last_update) / SECS_PER_REAL_DAY);
	  secs_lost = ((time(0) - st.last_update) % SECS_PER_REAL_DAY);
	  
	  fseek(char_file, (long) (player_table[i].nr *
				   sizeof(struct char_file_u)), 0);
	  fread(&ch_st, sizeof(struct char_file_u), 1, char_file);
	 
          if (ch_st.load_room == AUTO_RENT) {  /* this person was autorented */
	    ch_st.load_room = NOWHERE;
	    st.last_update = time(0)+3600;  /* one hour grace period */

	    sprintf(buf, "   Deautorenting %s", st.owner);
	    log(buf);

#if LIMITED_ITEMS
	    fprintf(stderr, "Counting limited items\n");
	    CountLimitedItems(&st);
	    fprintf(stderr, "Done\n");
#endif
            fseek(char_file, (long) (player_table[i].nr *
					 sizeof(struct char_file_u)), 0);
	    fwrite(&ch_st, sizeof(struct char_file_u), 1, char_file);
#if 0
	    rewind(fl);
	    WriteObjs(fl, &st);
#endif
	    fclose(fl);
	  } else {
 
	    if (days_passed > 0) {
	      
	      if ((st.total_cost*days_passed) > st.gold_left) {
		
		sprintf(buf, "   Dumping %s from object file.", ch_st.name);
		log(buf);
		
		ch_st.points.gold = 0;
		ch_st.load_room = NOWHERE;
		fseek(char_file, (long) (player_table[i].nr *
					 sizeof(struct char_file_u)), 0);
		fwrite(&ch_st, sizeof(struct char_file_u), 1, char_file);
		
		fclose(fl);
		ZeroRent(ch_st.name);
		
	      } else {
		
		sprintf(buf, "   Updating %s", st.owner);
		log(buf);
		st.gold_left  -= (st.total_cost*days_passed);
		st.last_update = time(0)-secs_lost;
#if 0
		rewind(fl);
		WriteObjs(fl, &st);
#endif
		fclose(fl);
#if LIMITED_ITEMS
		CountLimitedItems(&st);
#endif
		
	      }
	    } else {
	      
#if LIMITED_ITEMS
	      CountLimitedItems(&st);
#endif
	      sprintf(buf, "  same day update on %s", st.owner);
	      log(buf);
#if 0
	      rewind(fl);
	      WriteObjs(fl, &st);
#endif
	      fclose(fl);
	    }
	  }
	}
      }
    } else {
      /* do nothing */
    }
  }
  fclose(char_file);
}


void CountLimitedItems(struct obj_file_u *st)
{
    int i, cost_per_day;
    struct obj_data *obj;

    if (!st->owner[0]) return;  /* don't count empty rent units */

    for(i=0; i<st->number; i++) {
      if (st->objects[i].item_number > -1 && 
          real_object(st->objects[i].item_number) > -1) {
	    /*
            ** eek.. read in the object, and then extract it.
	    ** (all this just to find rent cost.)  *sigh*
            */
            obj = read_object(st->objects[i].item_number, VIRTUAL);
	    cost_per_day = obj->obj_flags.cost_per_day;
	    /*
            **  if the cost is > LIM_ITEM_COST_MIN, then mark before extractin
            */
	    if (cost_per_day > LIM_ITEM_COST_MIN) {
	      obj_index[obj->item_number].number++;  
	    } else {
	      if (IS_OBJ_STAT(obj, ITEM_MAGIC) ||
		  IS_OBJ_STAT(obj, ITEM_GLOW) ||
		  IS_OBJ_STAT(obj, ITEM_HUM) ||
		  IS_OBJ_STAT(obj, ITEM_INVISIBLE) ||
		  IS_OBJ_STAT(obj, ITEM_BLESS)) {
		obj_index[obj->item_number].number++;  
	      }
	    }
	}
    }
}


void PrintLimitedItems()
{
/*  int i; */
  char buf[200];
#if 0
  for (i=0;i<=top_of_objt;i++) {
    if (obj_index[i].number > 0) {
      sprintf(buf, "item> %d [%d]", obj_index[i].virtual, obj_index[i].number);
      log(buf);
    }
  }
#endif
}



/* ************************************************************************
* Routine Receptionist                                                    *
************************************************************************* */
#define DONATION_ROOM 99

int receptionist(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type)
{
  char buf[240];
  struct obj_cost cost;
  struct char_data *recep = 0;
  struct char_data *temp_char;
  sh_int save_room;
  sh_int action_tabel[9];
  
  
  if (!ch->desc)
    return(FALSE); /* You've forgot FALSE - NPC couldn't leave */

   action_tabel[0] = 23;
   action_tabel[1] = 24;
   action_tabel[2] = 36;
   action_tabel[3] = 105;
   action_tabel[4] = 106;
   action_tabel[5] = 109;
   action_tabel[6] = 111;
   action_tabel[7] = 142;
   action_tabel[8] = 147;

  
  for (temp_char = real_roomp(ch->in_room)->people; (temp_char) && (!recep);
       temp_char = temp_char->next_in_room)
    if (IS_MOB(temp_char))
      if (mob_index[temp_char->nr].func == receptionist)
	recep = temp_char;
  
  if (!recep) {
    log("No_receptionist.\n\r");
    assert(0);
  }
  
  if(!number(0,2))
    
    for (temp_char = real_roomp(ch->in_room)->people; (temp_char);
	 temp_char = temp_char->next_in_room)
      if(temp_char != recep)
	if (IS_MOB(temp_char)) {
	  
	  struct room_direction_data    *exitp;
	  int   going_to,door;
	  struct room_data      *rp;
	  
	  act("$n pushes a button on $s desk.  A trap door opens!",TRUE,recep,
	      0,0,TO_ROOM);
	  send_to_char("You fall through!\n\r\n\r",temp_char);
	  act("$N falls through the trap door!",TRUE,recep,0,temp_char,TO_NOTVICT);
	  act("$n mutters something about not liking monsters in $s inn.",TRUE,
	      recep,0,0,TO_ROOM);
	  
	  door = 5;			/* down */
	  exitp = EXIT(temp_char, door);
	  if(exit_ok(exitp, &rp)) {	
	    going_to = exitp->to_room;
	    
	    char_from_room(temp_char);
	    char_to_room(temp_char,going_to);
	    do_look(temp_char,"",0);
	  } else {			/* must be some other direction */
	    int k;
	    for(k=0;k<5;k++) {
	      exitp = EXIT(temp_char, k);
	      if(exit_ok(exitp, &rp)) {
		going_to = exitp->to_room;
		
		char_from_room(temp_char);
		char_to_room(temp_char,going_to);
		do_look(temp_char,"",0);
	      }
	    }
	  }
	  return(FALSE);
	}
  
  if ((cmd != 92) && (cmd != 93)) {
    if (!cmd) {
      if (recep->specials.fighting) {
	return(citizen(recep,0,"",mob,type));
      }
    }

    if(!number(0,2)) {
      struct obj_data *i;
      for (i = real_roomp(ch->in_room)->contents; i; i = i->next_content) {
	if (IS_SET(i->obj_flags.wear_flags, ITEM_TAKE)) {
	  act("$n sweeps some trash into the donation room.",TRUE,recep,0,0,
	      TO_ROOM);
	  obj_from_room(i);
	  obj_to_room(i,DONATION_ROOM);
	  break;
	} 
      }
    }

    

    if (!number(0, 30))
      do_action(recep, "", action_tabel[number(0,8)]);
    return(FALSE);
  }
  
  if (!AWAKE(recep)) {
    act("$e isn't able to talk to you...", FALSE, recep, 0, ch, TO_VICT);
    return(TRUE);
  }
  
  if (!CAN_SEE(recep, ch))     {
      act("$n says, 'I just can't deal with people I can't see!'", FALSE, recep, 0, 0, TO_ROOM);
      act("$n bursts into tears", FALSE, recep, 0, 0, TO_ROOM);
      return(TRUE);
    }
  
  if (cmd == 92) { /* Rent  */
    if (recep_offer(ch, recep, &cost)) {
      
      act("$n stores your stuff in the safe, and helps you into your chamber.",
	  FALSE, recep, 0, ch, TO_VICT);
      act("$n helps $N into $S private chamber.",FALSE, recep,0,ch,TO_NOTVICT);
      
      save_obj(ch, &cost,1);
      save_room = ch->in_room;

      if (ch->specials.start_room != 2) /* hell */
	ch->specials.start_room = save_room;

      extract_char(ch);  /* you don't delete CHARACTERS when you extract
			    them */
      save_char(ch, save_room);
      ch->in_room = save_room;

    }
    
  } else {         /* Offer */
    recep_offer(ch, recep, &cost);
    act("$N gives $n an offer.", FALSE, ch, 0, recep, TO_ROOM);
  }
  
  return(TRUE);
}


/*
    removes a player from the list of renters
*/

void zero_rent( struct char_data *ch) 
{

  if (IS_NPC(ch))
    return;

  ZeroRent(GET_NAME(ch));

}

void ZeroRent( char *n)
{
  FILE *fl;
  char buf[200];

  sprintf(buf, "rent/%s", lower(n));

  if (!(fl = fopen(buf, "w"))) {
    perror("saving PC's objects");
    assert(0);
  }
  
  fclose(fl);
  return;
  
}

int ReadObjs( FILE *fl, struct obj_file_u *st)
{
  int i;
  char buf[128];

  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }

  fread(st->owner, sizeof(st->owner), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->gold_left, sizeof(st->gold_left), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->total_cost, sizeof(st->total_cost), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->last_update, sizeof(st->last_update), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->minimum_stay, sizeof(st->minimum_stay), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->number, sizeof(st->number), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
   
  for (i=0;i<st->number;i++) {
     fread(&st->objects[i], sizeof(struct obj_file_elem), 1, fl);
  }

}

int WriteObjs( FILE *fl, struct obj_file_u *st)
{
  int i;
  char buf[128];

  fwrite(st->owner, sizeof(st->owner), 1, fl);
  fwrite(&st->gold_left, sizeof(st->gold_left), 1, fl);
  fwrite(&st->total_cost, sizeof(st->total_cost), 1, fl);
  fwrite(&st->last_update, sizeof(st->last_update), 1, fl);
  fwrite(&st->minimum_stay, sizeof(st->minimum_stay), 1, fl);
  fwrite(&st->number, sizeof(st->number), 1, fl);
   
  for (i=0;i<st->number;i++) {
     fwrite(&st->objects[i], sizeof(struct obj_file_elem), 1, fl);
  }
}


void load_char_extra(struct char_data *ch)
{
  FILE *fp;
  char buf[80];
  char line[260];
  char  tmp[260];
  char *p, *s, *chk;
  int n;

  sprintf(buf, "rent/%s.aux", GET_NAME(ch));

  /*
    open the file.. read in the lines, use them as the aliases and
    poofin and outs, depending on tags:

    format:

    <id>:string

  */

  if ((fp = fopen(buf, "r")) == NULL) {
    return;  /* nothing to look at */
  }

  while (!feof(fp)) {
    chk = fgets(line, 260, fp);

    if (chk) {
      p = (char *)strtok(line, ":");
      s = (char *)strtok(0, "\0");
      if (p) {
	if (!strcmp(p,"out")) { /*setup bamfout */
	  do_bamfout(ch, s, 0);
	} else if (!strcmp(p, "in")) { /* setup bamfin */
	  do_bamfin(ch, s, 0);
	} else if (!strcmp(p, "zone")) { /* set zone permisions */
          GET_ZONE(ch) = atoi(s);
	} else if (!strcmp(p, "loot")) {
	  ch->specials.loot = atoi(s);
	} else if (!strcmp(p, "split")) {
	  ch->specials.split = atoi(s);
	} else if (!strcmp(p,"sev")) {
	  ch->specials.sev = atoi(s);
	} else if (!strcmp(p, "flee")) {
	  ch->specials.flee = atoi(s);
	} else if (!strcmp(p, "prompt")) {
	  ch->specials.prompt = atoi(s);
	} else {
	  if (s) {
	    s[strlen(s)]= '\0';
	    n = atoi(p);
	    if (n >=0 && n <= 9) {  /* set up alias */
	      sprintf(tmp, "%d %s", n, s+1);
	      do_alias(ch, tmp, 260);
	    }
	  }
	}
      }
    } else {
      break;
    }
  }
  fclose(fp);
}

void write_char_extra( struct char_data *ch)
{
  FILE *fp;
  char buf[80];
  int i;

  sprintf(buf, "rent/%s.aux", GET_NAME(ch));

  /*
    open the file.. read in the lines, use them as the aliases and
    poofin and outs, depending on tags:

    format:

    <id>:string

  */

  if ((fp = fopen(buf, "w")) == NULL) {
    return;  /* nothing to write */
  }

  if (IS_IMMORTAL(ch)) {
    if (ch->specials.poofin) {
      fprintf(fp, "in: %s\n", ch->specials.poofin);
    }
    if (ch->specials.poofout) {
      fprintf(fp, "out: %s\n", ch->specials.poofout);
    }
    fprintf(fp, "zone: %d\n", GET_ZONE(ch));
    fprintf(fp, "sev: %d\n", ch->specials.sev);
  }

  fprintf(fp, "loot: %d\n", ch->specials.loot);
  fprintf(fp, "split: %d\n",ch->specials.split);
  fprintf(fp, "flee: %d\n", ch->specials.flee);
  fprintf(fp, "prompt: %d\n",ch->specials.prompt);

  if (ch->specials.A_list) {
    for (i=0;i<10;i++) {
      if (GET_ALIAS(ch, i)) {
	fprintf(fp, "%d: %s\n", i, GET_ALIAS(ch, i));
      }
    }
  }
  fclose(fp);
}


void obj_store_to_room(int room, struct obj_file_u *st)
{
  struct obj_data *obj;
  int i, j;
  
  
  for(i=0; i<st->number; i++) {
    if (st->objects[i].item_number > -1 && 
	real_object(st->objects[i].item_number) > -1) {
      obj = read_object(st->objects[i].item_number, VIRTUAL);
      obj->obj_flags.value[0] = st->objects[i].value[0];
      obj->obj_flags.value[1] = st->objects[i].value[1];
      obj->obj_flags.value[2] = st->objects[i].value[2];
      obj->obj_flags.value[3] = st->objects[i].value[3];
      obj->obj_flags.extra_flags = st->objects[i].extra_flags;
      obj->obj_flags.weight      = st->objects[i].weight;
      obj->obj_flags.timer       = st->objects[i].timer;
      obj->obj_flags.bitvector   = st->objects[i].bitvector;

/*  new, saving names and descrips stuff o_s_t_r */
      if (obj->name)
	free(obj->name);
      if (obj->short_description)
	free(obj->short_description);
      if (obj->description)
	free(obj->description);

      obj->name = (char *)malloc(strlen(st->objects[i].name)+1);
      obj->short_description = (char *)malloc(strlen(st->objects[i].sd)+1);
      obj->description = (char *)malloc(strlen(st->objects[i].desc)+1);

      strcpy(obj->name, st->objects[i].name);
      strcpy(obj->short_description, st->objects[i].sd);
      strcpy(obj->description, st->objects[i].desc);
/* end of new, possibly buggy stuff */
      
      for(j=0; j<MAX_OBJ_AFFECT; j++)
	obj->affected[j] = st->objects[i].affected[j];
      
      obj_to_room2(obj, room);
    }
  }
  /* bug report -  this is a static array, shouldn't be freed */
#if 0
  free(st->objects);
#endif
}

void load_room_objs(int room)
{
  FILE *fl;
  struct obj_file_u st;
  char buf[200];
  
  sprintf(buf, "world/%d", room);

  
  /* r+b is for Binary Reading/Writing */
  if (!(fl = fopen(buf, "r+b")))  {
    log("Room has no equipment");
    return;
  }

  rewind(fl);

  if (!ReadObjs(fl, &st)) {
    log("No objects found");
    fclose(fl);
    return;
  }

  fclose(fl);

  obj_store_to_room(room, &st);
  save_room(room);
}

void save_room(int room)
{
 struct obj_file_u st;
 struct obj_data *obj;
 struct room_data *rm = 0;
 char buf[255];
 static int last_room = -1;
 static FILE *f1 = 0;

 rm = real_roomp(room);

 obj = rm->contents;
 sprintf(buf, "world/%d", room);
 st.number = 0;

 if(obj) {
   if (room != last_room) {
     if (f1)
       fclose(f1);
     f1 = fopen(buf, "w");
   }
   if (!f1) 
     return;

   rewind(f1);
   obj_to_store(obj, &st, NULL, 0);
   sprintf(buf, "Room %d", room);
   strcpy(st.owner, buf);
   st.gold_left = 0;
   st.total_cost = 0;
   st.last_update = 0;
   st.minimum_stay = 0;
   WriteObjs(f1, &st);
  }
}

