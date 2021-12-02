/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "protos.h"

/*
  external stuff
*/

extern struct index_data *mob_index;
extern struct room_data *world;

int FreeHates( struct char_data *ch)
{
  struct char_list *k, *n;

  for (k=ch->hates.clist; k; k = n) {
    n = k->next;
    free(n);
  }

}


int FreeFears( struct char_data *ch)
{
  struct char_list *k, *n;

  for (k=ch->fears.clist; k; k = n) {
    n = k->next;
    free(n);
  }

}


int RemHated( struct char_data *ch, struct char_data *pud) 
{
  struct char_list *oldpud, *t;

  if (pud) {
     for (oldpud = ch->hates.clist; oldpud; oldpud = oldpud->next) {
       if (!oldpud) return(FALSE);
       if (oldpud->op_ch) {
        if (oldpud->op_ch == pud) {
	  t = oldpud;
	  if (ch->hates.clist == t) {
	    ch->hates.clist = 0;
	    free(t);
	    break;
	  } else {
	    for (oldpud = ch->hates.clist; oldpud->next != t; 
		 oldpud = oldpud->next);
	    oldpud->next = oldpud->next->next;
	    free(t);
            break;
	  }
	}
      } else {
	if (!strcmp(oldpud->name,GET_NAME(pud))) {
	  t = oldpud;
	  if (ch->hates.clist == t) {
	    ch->hates.clist = 0;
	    free(t);
            break;
	  } else {
	    for (oldpud = ch->hates.clist; oldpud->next != t; 
		 oldpud = oldpud->next);
	    oldpud->next = oldpud->next->next;
	    free(t);
            break;
	  }
	}
      }
     }
  } 

  if (!ch->hates.clist) {
    REMOVE_BIT(ch->hatefield, HATE_CHAR);
  }
  if (!ch->hatefield)
    REMOVE_BIT(ch->specials.act, ACT_HATEFUL);

  return( (pud) ? TRUE : FALSE);
}



int AddHated( struct char_data *ch, struct char_data *pud) 
{

  struct char_list *newpud;

  if (ch == pud)
    return(FALSE);

  if (pud) {

    if (!can_see(ch, pud))
      return(FALSE);

    CREATE(newpud, struct char_list, 1);
    newpud->op_ch = pud;
    strcpy(newpud->name, GET_NAME(pud));
    newpud->next = ch->hates.clist;
    ch->hates.clist = newpud;
    if (!IS_SET(ch->specials.act, ACT_HATEFUL)) {
      SET_BIT(ch->specials.act, ACT_HATEFUL);
    }
    if (!IS_SET(ch->hatefield, HATE_CHAR)) {
      SET_BIT(ch->hatefield, HATE_CHAR);
    }
    if (IS_IMMORTAL(pud)) 
      send_to_char("---Someone hates you.\n\r",pud);
  } 
  
  return( (pud) ? TRUE : FALSE );
}

int AddHatred( struct char_data *ch, int parm_type, int parm)
{
  switch(parm_type) {
  case OP_SEX :
    if (!IS_SET(ch->hatefield, HATE_SEX))
      SET_BIT(ch->hatefield, HATE_SEX);
    ch->hates.sex = parm;
    break;
  case OP_RACE:
    if (!IS_SET(ch->hatefield, HATE_RACE))
      SET_BIT(ch->hatefield, HATE_RACE);
    ch->hates.race = parm;
    break;
  case OP_GOOD:
    if (!IS_SET(ch->hatefield, HATE_GOOD))
      SET_BIT(ch->hatefield, HATE_GOOD);
    ch->hates.good = parm;
    break;
  case OP_EVIL:
    if (!IS_SET(ch->hatefield, HATE_EVIL))
      SET_BIT(ch->hatefield, HATE_EVIL);
    ch->hates.evil = parm;
    break;
  case OP_CLASS:
    if (!IS_SET(ch->hatefield, HATE_CLASS))
      SET_BIT(ch->hatefield, HATE_CLASS);
    ch->hates.class = parm;
    break;
  case OP_VNUM:
    if (!IS_SET(ch->hatefield, HATE_VNUM))
      SET_BIT(ch->hatefield, HATE_VNUM);
    ch->hates.vnum = parm;
    break;
  }
  if (!IS_SET(ch->specials.act, ACT_HATEFUL)) {
    SET_BIT(ch->specials.act, ACT_HATEFUL);
  }
}

int RemHatred( struct char_data *ch, unsigned short bitv)
{
      REMOVE_BIT(ch->hatefield, bitv);
      if (!ch->hatefield)
	REMOVE_BIT(ch->specials.act, ACT_HATEFUL);
}


int Hates( struct char_data *ch, struct char_data *v)
{
  struct char_list *i;

  if (IS_AFFECTED(ch, AFF_PARALYSIS))
    return(FALSE);

  if (ch == v)
    return(FALSE);

  if (IS_SET(ch->hatefield, HATE_CHAR)) {
    if (ch->hates.clist) {
      for (i = ch->hates.clist; i; i = i->next) {
        if (i->op_ch) {
	  if ((i->op_ch == v) && 
	    (!strcmp(i->name, GET_NAME(v))))
	    return(TRUE);
	} else {
          if (!strcmp(i->name, GET_NAME(v)))
	    return(TRUE);
	}
      }
    }
  }
  if (IS_SET(ch->hatefield, HATE_RACE)) {
    if (ch->hates.race != -1) {
      if (ch->hates.race == GET_RACE(v)) {
	char buf[256];
	sprintf(buf, "You hate %d", GET_RACE(v));
	send_to_char(buf, ch);
	return(TRUE);
      }
    }
  }

  if (IS_SET(ch->hatefield, HATE_SEX)) {
    if (ch->hates.sex == GET_SEX(v))
      return(TRUE);
  }
  if (IS_SET(ch->hatefield, HATE_GOOD)) {
    if (ch->hates.good < GET_ALIGNMENT(v))
      return(TRUE);
  }
  if (IS_SET(ch->hatefield, HATE_EVIL)) {
    if (ch->hates.evil > GET_ALIGNMENT(v))
      return(TRUE);
  }
  if (IS_SET(ch->hatefield, HATE_CLASS)) {
    if (HasClass(v, ch->hates.class)) {
      return(TRUE);
    }
  }
  if (IS_SET(ch->hatefield, HATE_VNUM)) {
    if (ch->hates.vnum == mob_index[v->nr].virtual)
      return(TRUE);
  }
  return(FALSE);
}

int Fears( struct char_data *ch, struct char_data *v)
{
  struct char_list *i;
  char buf[255];
  
  if (IS_AFFECTED(ch, AFF_PARALYSIS))
    return(FALSE);
  
  if (!IS_SET(ch->specials.act, ACT_AFRAID))
    return(FALSE);
  
  if (IS_SET(ch->fearfield, FEAR_CHAR)) {
    if (ch->fears.clist) {
      for (i = ch->fears.clist; i; i = i->next) {
	if (i) {
	  if (i->op_ch) {
	    if (i->name[0] != '\0') {
	      if ((i->op_ch == v) &&
		  (!strcmp(i->name, GET_NAME(v))))
	        return(TRUE);
	    } else {
	      /* lets see if this clears the problem */
	      RemFeared(ch, i->op_ch);
	    }
	  } else {
	    if (i->name[0] != '\0') {
	      if (!strcmp(i->name, GET_NAME(v)))
		return(TRUE);
	    }
	  }	  
	}
      }
    }
  }
  if (IS_SET(ch->fearfield, FEAR_RACE)) {
    if (ch->fears.race != -1) {
      if (ch->fears.race == GET_RACE(v))
	return(TRUE);
    }
  }
  if (IS_SET(ch->fearfield, FEAR_SEX)) {
    if (ch->fears.sex == GET_SEX(v))
      return(TRUE);
  }
  if (IS_SET(ch->fearfield, FEAR_GOOD)) {
    if (ch->fears.good < GET_ALIGNMENT(v))
      return(TRUE);
  }
  if (IS_SET(ch->fearfield, FEAR_EVIL)) {
    if (ch->fears.evil > GET_ALIGNMENT(v))
      return(TRUE);
  }
  if (IS_SET(ch->fearfield, FEAR_CLASS)) {
    if (HasClass(v, ch->hates.class)) {
      return(TRUE);
    }
  }
  if (IS_SET(ch->fearfield, FEAR_VNUM)) {
    if (ch->fears.vnum == mob_index[v->nr].virtual)
      return(TRUE);
  }
  return(FALSE);
}

int RemFeared( struct char_data *ch, struct char_data *pud) 
{

  struct char_list *oldpud, *t, *tmp;

  if (!IS_SET(ch->specials.act, ACT_AFRAID)) 
    return(FALSE);

  if (pud && (ch->fears.clist!=0)) {
    tmp = ch->fears.clist;    
    for (oldpud = ch->fears.clist; (oldpud!=0); oldpud = tmp) {       
      if (oldpud==0) return(FALSE);
      tmp = oldpud->next;
      if (oldpud->op_ch) {
        if (oldpud->op_ch == pud) {
	  t = oldpud;
	  if (ch->fears.clist == t) {
	    ch->fears.clist = 0;
	    free(t);
            break;
	  } else {
	    for (oldpud = ch->fears.clist; oldpud->next != t; 
		 oldpud = oldpud->next);
	    oldpud->next = oldpud->next->next;
	    free(t);
            break;
	  }
	}
      } else {
	if (!strcmp(oldpud->name,GET_NAME(pud))) {
	  t = oldpud;
	  if (ch->fears.clist == t) {
	    ch->fears.clist = 0;
	    free(t);
            break;
	  } else {
	    for (oldpud = ch->fears.clist; oldpud->next != t; 
		 oldpud = oldpud->next);
	    oldpud->next = oldpud->next->next;
	    free(t);
            break;
	  }
	}
      }
    }
  }
  if (!ch->fears.clist)
    REMOVE_BIT(ch->fearfield, FEAR_CHAR);
  if (!ch->fearfield)
    REMOVE_BIT(ch->specials.act, ACT_AFRAID);
  return( (pud) ? TRUE : FALSE);
}



int AddFeared( struct char_data *ch, struct char_data *pud) 
{

  struct char_list *newpud;

  if (pud) {

    if (!can_see(ch, pud))
      return(FALSE);

    CREATE(newpud, struct char_list, 1);
    newpud->op_ch = pud;
    strcpy(newpud->name,GET_NAME(pud));
    newpud->next = ch->fears.clist;
    ch->fears.clist = newpud;
    
    if (!IS_SET(ch->specials.act, ACT_AFRAID)) {
      SET_BIT(ch->specials.act, ACT_AFRAID);
    }
    if (!IS_SET(ch->fearfield, FEAR_CHAR)) {
      SET_BIT(ch->fearfield, FEAR_CHAR);
    }
    if (IS_IMMORTAL(pud)) 
      send_to_char("---Someone fears you.(as well they should)\n\r",pud);
  } 
  
  return( (pud) ? TRUE : FALSE);
}


int AddFears( struct char_data *ch, int parm_type, int parm)
{
  switch(parm_type) {
  case OP_SEX :
    if (!IS_SET(ch->fearfield, FEAR_SEX))
      SET_BIT(ch->fearfield, FEAR_SEX);
    ch->fears.sex = parm;
    break;
  case OP_RACE:
    if (!IS_SET(ch->fearfield, FEAR_RACE))
      SET_BIT(ch->fearfield, FEAR_RACE);
    ch->fears.race = parm;
    break;
  case OP_GOOD:
    if (!IS_SET(ch->fearfield, FEAR_GOOD))
      SET_BIT(ch->fearfield, FEAR_GOOD);
    ch->fears.good = parm;
    break;
  case OP_EVIL:
    if (!IS_SET(ch->fearfield, FEAR_EVIL))
      SET_BIT(ch->fearfield, FEAR_EVIL);
    ch->fears.evil = parm;
    break;
  case OP_CLASS:
    if (!IS_SET(ch->fearfield, FEAR_CLASS))
      SET_BIT(ch->fearfield, FEAR_CLASS);
    ch->fears.class = parm;
    break;
  case OP_VNUM:
    if (!IS_SET(ch->fearfield, FEAR_VNUM))
      SET_BIT(ch->fearfield, FEAR_VNUM);
    ch->fears.vnum = parm;
    break;
  }
  if (!IS_SET(ch->specials.act, ACT_AFRAID)) {
     SET_BIT(ch->specials.act, ACT_AFRAID);
   }
}


struct char_data *FindAHatee( struct char_data *ch) 
{
   struct char_data *tmp_ch;

   if (ch->in_room < 0)
     return(0);

   for (tmp_ch=real_roomp(ch->in_room)->people; tmp_ch; 
        tmp_ch = tmp_ch->next_in_room) {
       if (Hates(ch, tmp_ch) && (can_see(ch, tmp_ch))) {
    	  if (ch->in_room == tmp_ch->in_room) {
	    if (ch != tmp_ch) {
	       return(tmp_ch);
	     } else {
	       RemHated(ch,tmp_ch);
	       return(0);
	     }
	  }
	}
     }
     return(0);
}

struct char_data *FindAFearee( struct char_data *ch) 
{
   struct char_data *tmp_ch;

   if (ch->in_room < 0)
     return(0);

   for (tmp_ch=real_roomp(ch->in_room)->people; tmp_ch; 
        tmp_ch = tmp_ch->next_in_room) {
       if (Fears(ch, tmp_ch) && (can_see(ch, tmp_ch))) {
    	  if ((ch->in_room == tmp_ch->in_room) &&
	    (ch != tmp_ch)) {
	    return(tmp_ch);
	  }
	}
     }
  return(0);
}


/*
  these two procedures zero out the character pointer
  for quiting players, without removing names
  thus the monsters will still hate them
*/


void ZeroHatred(struct char_data *ch, struct char_data *v)
{
  
  struct char_list *oldpud;
  
  for (oldpud = ch->hates.clist; oldpud; oldpud = oldpud->next) {
    if (oldpud) {
      if (oldpud->op_ch) {
        if (oldpud->op_ch == v) {
	  oldpud->op_ch = 0;
	}
      }
    }
  }
}


void ZeroFeared(struct char_data *ch, struct char_data *v)
{
  
  struct char_list *oldpud;
  
  for (oldpud = ch->fears.clist; oldpud; oldpud = oldpud->next) {
    if (oldpud) {
      if (oldpud->op_ch) {
        if (oldpud->op_ch == v) {
	  oldpud->op_ch = 0;
	}
      }
    }
  }
}


/*
  these two are to make the monsters completely forget about them.
*/
void DeleteHatreds(struct char_data *ch)
{
  
  struct char_data *i;
  extern struct char_data *character_list;

  for (i = character_list; i; i = i->next) {
    if (Hates(i, ch))
      RemHated(i, ch);
  }

}


void DeleteFears(struct char_data *ch)
{
  struct char_data *i;
  extern struct char_data *character_list;

  
  for (i = character_list; i; i = i->next) {
    if (Fears(i, ch))
      RemFeared(i, ch);
  }

}

void KillTheOrcs(struct char_data *ch)
{
  
  struct char_data *tmp_victim;
  
  if(GET_RACE(ch) != RACE_DWARF) 
    return;
  if(GetMaxLevel(ch) < 10)
    return;
  
  for ( tmp_victim = real_roomp(ch->in_room)->people; tmp_victim;
       tmp_victim = tmp_victim->next_in_room) {
    if(GET_RACE(tmp_victim) == RACE_ORC  && can_see(ch,tmp_victim) &&
       !ch->specials.fighting) {
      send_to_char("My god, there's a stinking orc!!\n\r",ch);
      if( (GET_HIT(ch) / GET_MAX_HIT(ch)  > 0.25 ) &&
	 (GET_MOVE(ch) / GET_MAX_MOVE(ch)) > 0.25 )  {
	if ( !saves_spell(ch, SAVING_SPELL) ) {
	  send_to_char("Your vision turns red as bloodlust overcomes you.!\n\r", ch);
	  send_to_char("ALL ORCS HAVE TO DIE!!!!!\n\r",ch);
	  act("$n spits in $N's face!",TRUE,ch, 0, tmp_victim, TO_NOTVICT);
	  act("$n spits in your face!",TRUE,ch, 0, tmp_victim, TO_VICT);
	  act("$n roars and attacks $N!",TRUE,ch,0,tmp_victim, TO_NOTVICT);
	  set_fighting(ch,tmp_victim);
	} else {
	  send_to_char("You manage to supress the urge to smash this puny orc to pieces.\n\r",ch);
	}
      } else {
	send_to_char("But you are too tired to kill it right now.\n\r",ch);
      }
      break;			/* break outta for loop */
    }
  }
}





