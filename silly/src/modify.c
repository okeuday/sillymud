/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "protos.h"

#define REBOOT_AT    10  /* 0-23, time of optional reboot if -e lib/reboot */


#define TP_MOB    0
#define TP_OBJ	   1
#define TP_ERROR  2


struct room_data *world;              /* dyn alloc'ed array of rooms     */



char *string_fields[] =
{
	"name",
	"short",
	"long",
	"description",
	"title",
	"delete-description",
	"delnoise",
	"delfarnoise",
	"\n"
};

char *room_fields[] =
{
	"name",   /* 1 */
	"desc",
	"fs",
	"exit",
	"exdsc",  /* 5 */
        "extra",  /* 6 */
	"riv",    /* 7 */
	"tele",   /* 8 */
	"tunn",   /* 9 */
	"\n"
};

/* maximum length for text field x+1 */
int length[] =
{
	15,
	60,
	256,
	240,
	60,
	
};


int room_length[] =
{
	80,
	1024,
        50,
	50,
	512,
	512,
        50,
	100,
        50
};

char *skill_fields[] = 
{
	"learned",
	"affected",
	"duration",
	"recognize",
	"\n"
};

int max_value[] =
{
	255,
	255,
	10000,
	1
};

/* ************************************************************************
*  modification of malloc'ed strings                                      *
************************************************************************ */

/* Add user input to the 'current' string (as defined by d->str) */

void string_add(struct descriptor_data *d, char *str)
{
  char *scan;
  int terminator = 0;
  
  /* determine if this is the terminal string, and truncate if so */
  for (scan = str; *scan; scan++)
    if (terminator = (*scan == '@'))  {
      *scan = '\0';
      break;
    }
  
  if (!(*d->str))	{
    if (strlen(str) > d->max_str)  	{
      send_to_char("String too long - Truncated.\n\r",
		   d->character);
      *(str + d->max_str) = '\0';
      terminator = 1;
    }
    CREATE(*d->str, char, strlen(str) + 3);
    strcpy(*d->str, str);
  } else	{
    if (strlen(str) + strlen(*d->str) > d->max_str)	{
      send_to_char("String too long. Last line skipped.\n\r",
		   d->character);
      terminator = 1;
    } else 	{
      if (!(*d->str = (char *) realloc(*d->str, strlen(*d->str) + 
				       strlen(str) + 3)))     	{
	perror("string_add");
	assert(0);
      }
      strcat(*d->str, str);
    }
  }
  
  if (terminator)	{
    d->str = 0;
    if (d->connected == CON_EXDSCR)	{
      SEND_TO_Q(MENU, d);
      d->connected = CON_SLCT;
    }
  } else
    strcat(*d->str, "\n\r");
}


#undef MAX_STR

/* interpret an argument for do_string */
void quad_arg(char *arg, int *type, char *name, int *field, char *string)
{
  char buf[MAX_STRING_LENGTH];

  
  /* determine type */
  arg = one_argument(arg, buf);
  if (is_abbrev(buf, "char"))
    *type = TP_MOB;
  else if (is_abbrev(buf, "obj"))
    *type = TP_OBJ;
  else {
    *type = TP_ERROR;
    return;
  }
  
  /* find name */
  arg = one_argument(arg, name);
  
  /* field name and number */
  arg = one_argument(arg, buf);
  if (!(*field = old_search_block(buf, 0, strlen(buf), string_fields, 0)))
    return;
  
  /* string */
  for (; isspace(*arg); arg++);
  for (; *string = *arg; arg++, string++);
  
  return;
}

	 


/* modification of malloc'ed strings in chars/objects */
void do_string(struct char_data *ch, char *arg, int cmd)
{
  
  char name[MAX_STRING_LENGTH], string[MAX_STRING_LENGTH];
  struct extra_descr_data *ed, *tmp;
  int field, type;
  struct char_data *mob;
  struct obj_data *obj;
  if (IS_NPC(ch))
    return;
  
  quad_arg(arg, &type, name, &field, string);
  
  if (type == TP_ERROR)	{
    send_to_char(
		 "Syntax:\n\rstring ('obj'|'char') <name> <field> [<string>].",
		 ch);
    return;
  }
  
  if (!field)	{
    send_to_char("No field by that name. Try 'help string'.\n\r",
		 ch);
    return;
  }
  
  if (type == TP_MOB)	{
    /* locate the beast */
    if (!(mob = get_char_vis(ch, name))) {
      send_to_char("I don't know anyone by that name...\n\r", ch);
      return;
    }
    
    switch(field)	{
    case 1:
      if (!IS_NPC(mob) && GetMaxLevel(ch) < IMPLEMENTOR) {
	send_to_char("You can't change that field for players.", ch);
	return;
      }
      if (!*string) {
	send_to_char("You have to supply a name!\n\r", ch);
	return;
      }
      ch->desc->str = &mob->player.name;
      if (!IS_NPC(mob))
	send_to_char("WARNING: You have changed the name of a player.\n\r", ch);
      break;
    case 2:
      if (!IS_NPC(mob)) {
	send_to_char("That field is for monsters only.\n\r", ch);
	return;
      }
      if (!*string) {
	send_to_char("You have to supply a description!\n\r", ch);
	return;
      }
      ch->desc->str = &mob->player.short_descr;
      break;
    case 3:
      if (!IS_NPC(mob)){
	send_to_char("That field is for monsters only.\n\r", ch);
	return;
      }
      ch->desc->str = &mob->player.long_descr;
      break;
    case 4:ch->desc->str = &mob->player.description; break;
    case 5:
      if (IS_NPC(mob))  {
	send_to_char("Monsters have no titles.\n\r",ch);
	return;
      }
      if ((GetMaxLevel(ch) >= GetMaxLevel(mob)) && (ch != mob)) 
	ch->desc->str = &mob->player.title;
      else {
	send_to_char("Sorry, can't set the title of someone of higher level.\n\r", ch);
	return;
      }
      break;
    case 7:
      if (mob->player.sounds) {
	free(mob->player.sounds);
	mob->player.sounds = 0;
      }
      return;
      break;
    case 8:
      if (mob->player.distant_snds) {
	free(mob->player.distant_snds);
	mob->player.distant_snds = 0;
      }
      return;
      break;
    default:
      send_to_char("That field is undefined for monsters.\n\r", ch);
      return;
      break;
    }
  } else {   /* type == TP_OBJ */
    /* locate the object */
    if (!(obj = get_obj_vis(ch, name)))    	{
      send_to_char("Can't find such a thing here..\n\r", ch);
      return;
    }
    
    switch(field)  	{
      
    case 1: 
      if (!*string) {
	send_to_char("You have to supply a keyword.\n\r", ch);
	return;
      } else {
	ch->desc->str = &obj->name;
	break;
      }
      break;
    case 2: 
      if (!*string) {
	send_to_char("You have to supply a keyword.\n\r", ch);
	return;
      } else {
        ch->desc->str = &obj->short_description; 
	break;
      }
    case 3: ch->desc->str = &obj->description; break;
    case 4:
      if (!*string)  	{
	send_to_char("You have to supply a keyword.\n\r", ch);
	return;
      }
      /* try to locate extra description */
      for (ed = obj->ex_description; ; ed = ed->next)
	if (!ed) {
	  CREATE(ed , struct extra_descr_data, 1);
	  ed->next = obj->ex_description;
	  obj->ex_description = ed;
	  CREATE(ed->keyword, char, strlen(string) + 1);
	  strcpy(ed->keyword, string);
	  ed->description = 0;
	  ch->desc->str = &ed->description;
	  send_to_char("New field.\n\r", ch);
	  break;
	}
	else if (!str_cmp(ed->keyword, string)) /* the field exists */
	  {
	    free(ed->description);
	    ed->description = 0;
	    ch->desc->str = &ed->description;
	    send_to_char(
			 "Modifying description.\n\r", ch);
	    break;
	  }
      ch->desc->max_str = MAX_STRING_LENGTH;
      return; /* the stndrd (see below) procedure does not apply here */
      break;
    case 6: /* deletion */
      if (!*string)  	{
	send_to_char("You must supply a field name.\n\r", ch);
	return;
      }
      /* try to locate field */
      for (ed = obj->ex_description; ; ed = ed->next)
	if (!ed) {
	  send_to_char("No field with that keyword.\n\r", ch);
	  return;
	} else if (!str_cmp(ed->keyword, string)) {
	  free(ed->keyword);
	  if (ed->description)
	    free(ed->description);
	  
	  /* delete the entry in the desr list */						
	  if (ed == obj->ex_description)
	    obj->ex_description = ed->next;
	  else {
	    for(tmp = obj->ex_description; tmp->next != ed; 
		tmp = tmp->next);
	    tmp->next = ed->next;
	  }
	  free(ed);
	  
	  send_to_char("Field deleted.\n\r", ch);
	  return;
	}
      break;				
    default:
      send_to_char(
		   "That field is undefined for objects.\n\r", ch);
      return;
      break;
    }
  }
  
  if (*ch->desc->str)	{
    free(*ch->desc->str);
  }
  
  if (*string) {   /* there was a string in the argument array */ 
    if (strlen(string) > length[field - 1])	{
      send_to_char("String too long - truncated.\n\r", ch);
      *(string + length[field - 1]) = '\0';
    }
    CREATE(*ch->desc->str, char, strlen(string) + 1);
    strcpy(*ch->desc->str, string);
    ch->desc->str = 0;
    send_to_char("Ok.\n\r", ch);
  } else {  /* there was no string. enter string mode */
    send_to_char("Enter string. terminate with '@'.\n\r", ch);
    *ch->desc->str = 0;
    ch->desc->max_str = length[field - 1];
  }
}




void bisect_arg(char *arg, int *field, char *string)
{
  char buf[MAX_INPUT_LENGTH];

  
  /* field name and number */
  arg = one_argument(arg, buf);
  if (!(*field = old_search_block(buf, 0, strlen(buf), room_fields, 0)))
    return;
  
  /* string */
  for (; isspace(*arg); arg++);
  for (; *string = *arg; arg++, string++);
  
  return;
}


void do_edit(struct char_data *ch, char *arg, int cmd)
{
  int field, dflags, dir, exroom, dkey, rspeed, rdir,
  tele_room, tele_time, tele_mask, moblim, tele_cnt;
  unsigned r_flags;
  int s_type;
  char name[MAX_INPUT_LENGTH], string[512], buf[132];
  struct extra_descr_data *ed, *tmp;
  struct room_data	*rp;
  
  rp = real_roomp(ch->in_room);
  
  if ((IS_NPC(ch)) || (GetMaxLevel(ch)<LOW_IMMORTAL))
    return;

  if (!ch->desc) /* someone is forced to do something. can be bad! */
    return;      /* the ch->desc->str field will cause problems... */


#ifndef TEST_SERVER
  if((GetMaxLevel(ch) < 55) && (rp->zone != GET_ZONE(ch))) {
     send_to_char("Sorry, you are not authorized to edit this zone.\n\r", ch);
     return;
  }
#endif
  bisect_arg(arg, &field, string);
  
  if (!field)	{
    send_to_char("No field by that name. Try 'help edit'.\n\r", ch);
    return;
  }
  
  r_flags = -1;
  s_type = -1;
  
  switch(field) {
    
  case 1: ch->desc->str = &rp->name; break;
  case 2: ch->desc->str = &rp->description; break;
  case 3: sscanf(string,"%u %d ",&r_flags,&s_type);
    if ((r_flags < 0)  || (s_type < 0) || (s_type > 11)) {
      send_to_char("didn't quite get those, please try again.\n\r",ch);
      send_to_char("flags must be 0 or positive, and sectors must be from 0 to 11\n\r",ch);
      send_to_char("edit fs <flags> <sector_type>\n\r",ch);
      return;
    }
    rp->room_flags = r_flags;
    rp->sector_type = s_type;
    
    if (rp->sector_type == SECT_WATER_NOSWIM) {
      send_to_char("P.S. you need to do speed and flow\n\r",ch);
      send_to_char("For this river. (set to 0 as default)\n\r",ch);
      rp->river_speed = 0;
      rp->river_dir = 0;
      return;
    }
    return;
    break;
    
  case 4: sscanf(string,"%d %d %d %d ", &dir, &dflags, &dkey, &exroom);
    
    /*
      check if the exit exists
      */
    if ((dir < 0) || (dir > 5)) {
      send_to_char("You need to use numbers for that (0 - 5)",ch);
      return;
    }
    
    if (rp->dir_option[dir]) {
      send_to_char("modifying exit.\n\r",ch);
      switch(dflags) {
      case 1: rp->dir_option[dir]->exit_info = EX_ISDOOR;
              break;
      case 2: rp->dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF;
              break;
      case 3: rp->dir_option[dir]->exit_info = EX_CLIMB;
              break;
      case 4: rp->dir_option[dir]->exit_info = EX_CLIMB | EX_ISDOOR;
              break;
      case 5: rp->dir_option[dir]->exit_info = EX_CLIMB | EX_ISDOOR | EX_PICKPROOF;
	break;
      default: rp->dir_option[dir]->exit_info = 0;
      }

      rp->dir_option[dir]->key = dkey;
      if (real_roomp(exroom) != NULL) {
	rp->dir_option[dir]->to_room = exroom;
      } else {
	send_to_char("Deleting exit.\n\r",ch);
	free(rp->dir_option[dir]);
	rp->dir_option[dir] = 0;
	return;
      }
    } else if (real_roomp(exroom)==NULL) {
      send_to_char("Hey, John Yaya, that's not a valid room.\n\r", ch);
      return;
    } else {
      send_to_char("New exit\n\r",ch);
      CREATE(rp->dir_option[dir], 
	     struct room_direction_data, 1);
      switch(dflags) {
      case 1: rp->dir_option[dir]->exit_info = EX_ISDOOR;
              break;
      case 2: rp->dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF;
              break;
      case 3: rp->dir_option[dir]->exit_info = EX_CLIMB;
              break;
      case 4: rp->dir_option[dir]->exit_info = EX_CLIMB | EX_ISDOOR;
              break;
      case 5: rp->dir_option[dir]->exit_info = EX_CLIMB | EX_ISDOOR | EX_PICKPROOF;
              break;
      default: rp->dir_option[dir]->exit_info = 0;
      }

      rp->dir_option[dir]->key = dkey;
      rp->dir_option[dir]->to_room = exroom;
    }
    
    if (rp->dir_option[dir]->exit_info>0) {
      string[0] = 0;
      send_to_char("enter keywords, 1 line only. \n\r",ch);
      send_to_char("terminate with an @ on the same line.\n\r",ch);
      ch->desc->str = &rp->dir_option[dir]->keyword; 
      break;
    } else {
      return;
    }   
    
  case 5: dir = -1;
    sscanf(string,"%d", &dir); 
    if ((dir >=0) && (dir <= 5)) {
      send_to_char("Enter text, term. with '@' on a blank line",ch);
      string[0] = 0;
      if (rp->dir_option[dir]) {
	ch->desc->str = &rp->dir_option[dir]->general_description;
      } else {
	CREATE(rp->dir_option[dir], 
	       struct room_direction_data, 1);
	ch->desc->str = &rp->dir_option[dir]->general_description;
      }
    } else {
      send_to_char("Illegal direction\n\r",ch);
      send_to_char("Must enter 0-5.I will ask for text.\n\r",ch);
      return;		  
    }
    break;
  case 6: 
    /* 
      extra descriptions 
      */
    if (!*string)  	{
      send_to_char("You have to supply a keyword.\n\r", ch);
      return;
    }
    /* try to locate extra description */
    for (ed = rp->ex_description; ; ed = ed->next)
      if (!ed) {
	CREATE(ed , struct extra_descr_data, 1);
	ed->next = rp->ex_description;
	rp->ex_description = ed;
	CREATE(ed->keyword, char, strlen(string) + 1);
	strcpy(ed->keyword, string);
	ed->description = 0;
	ch->desc->str = &ed->description;
	send_to_char("New field.\n\r", ch);
	break;
      }  else if (!str_cmp(ed->keyword, string)) {
	/* the field exists */
	free(ed->description);
	ed->description = 0;
	ch->desc->str = &ed->description;
	send_to_char( "Modifying description.\n\r", ch);
	break;
      }
    ch->desc->max_str = MAX_STRING_LENGTH;
    return; 
    break;
    
  case 7:
    /*  this is where the river stuff will go */
    rspeed = 0; rdir = 0;
    sscanf(string,"%d %d ",&rspeed,&rdir);
    if ((rdir>= 0) && (rdir <= 5)) {
      rp->river_speed = rspeed;
      rp->river_dir = rdir;
    } else {
      send_to_char("Illegal dir. : edit riv <speed> <dir>\n\r",ch);
    }
    return;

  case 8:
    /*  this is where the teleport stuff will go */
    tele_room = -1; tele_time = -1; tele_mask = -1;
    sscanf(string,"%d %d %d",&tele_time,&tele_room,&tele_mask);
    if (tele_room < 0 || tele_time < 0 || tele_mask < 0) {
      send_to_char(" edit tele <time> <room_nr> <tele-flags>\n\r", ch);
      return;
      break;
    } else {
      if (IS_SET(TELE_COUNT, tele_mask)) {
	sscanf(string,"%d %d %d %d", 
	       &tele_time, &tele_room, &tele_mask, &tele_cnt);
	if (tele_cnt < 0) {
	  send_to_char
	    (" edit tele <time> <room_nr> <tele-flags> [tele-count]\n\r", ch);
	  return;
	} else {
	  real_roomp(ch->in_room)->tele_time = tele_time;
	  real_roomp(ch->in_room)->tele_targ = tele_room;
	  real_roomp(ch->in_room)->tele_mask = tele_mask;
	  real_roomp(ch->in_room)->tele_cnt  = tele_cnt;
	}
      } else {
	real_roomp(ch->in_room)->tele_time = tele_time;
	real_roomp(ch->in_room)->tele_targ = tele_room;
	real_roomp(ch->in_room)->tele_mask = tele_mask;
	real_roomp(ch->in_room)->tele_cnt  = 0;
        return;
      }
    }
    
    return;
  case 9:
    if (sscanf(string, "%d", &moblim) < 1) {
      send_to_char("edit tunn <mob_limit>\n\r", ch);
      return;
      break;
    } else {
      real_roomp(ch->in_room)->moblim = moblim;
      if (!IS_SET(real_roomp(ch->in_room)->room_flags, TUNNEL))
	SET_BIT(real_roomp(ch->in_room)->room_flags, TUNNEL);
        return;
      break;
    }
  case 10: 
    /*
      deletion
      */
      if (!*string)  	{
	send_to_char("You must supply a field name.\n\r", ch);
	return;
      }
      /* try to locate field */
      for (ed = rp->ex_description; ; ed = ed->next)
	if (!ed) {
	  send_to_char("No field with that keyword.\n\r", ch);
	  return;
	} else if (!str_cmp(ed->keyword, string)) {
	  free(ed->keyword);
	  if (ed->description)
	    free(ed->description);
	  
	  /* delete the entry in the desr list */						
	  if (ed == rp->ex_description)
	    rp->ex_description = ed->next;
	  else {
	    for(tmp = rp->ex_description; tmp->next != ed; 
		tmp = tmp->next);
	    tmp->next = ed->next;
	  }
	  free(ed);
	  
	  send_to_char("Field deleted.\n\r", ch);
	  return;
	}
      break;				
    
  default:
    send_to_char("I'm so confused :-)\n\r",ch);
    return;
    break;
  }
  
  if (*ch->desc->str)	{
    free(*ch->desc->str);
  }
  
  if (*string) {   /* there was a string in the argument array */ 
    if (strlen(string) > room_length[field - 1])	{
      send_to_char("String too long - truncated.\n\r", ch);
      *(string + length[field - 1]) = '\0';
    }
    CREATE(*ch->desc->str, char, strlen(string) + 1);
    strcpy(*ch->desc->str, string);
    ch->desc->str = 0;
    send_to_char("Ok.\n\r", ch);
  } else {  /* there was no string. enter string mode */
    send_to_char("Enter string. terminate with '@'.\n\r", ch);
    *ch->desc->str = 0;
    ch->desc->max_str = room_length[field - 1];
  }
  
}


/* **********************************************************************
*  Modification of character skills                                     *
********************************************************************** */


void do_setskill(struct char_data *ch, char *arg, int cmd)
{
	send_to_char("This routine is disabled untill it fitts\n\r", ch);
	send_to_char("The new structures (sorry Quinn) ....Bombman\n\r", ch);
	return;
}








/* db stuff *********************************************** */


/* One_Word is like one_argument, execpt that words in quotes "" are */
/* regarded as ONE word                                              */

char *one_word(char *argument, char *first_arg )
{
  int begin, look_at;
  
  begin = 0;
  
  do
    {
      for ( ;isspace(*(argument + begin)); begin++);
      
      if (*(argument+begin) == '\"') {  /* is it a quote */
	
	begin++;
	
	for (look_at=0; (*(argument+begin+look_at) >= ' ') && 
	     (*(argument+begin+look_at) != '\"') ; look_at++)
				*(first_arg + look_at) = LOWER(*(argument + begin + look_at));
	
	if (*(argument+begin+look_at) == '\"')
	  begin++;
	
      } else {
	
	for (look_at=0; *(argument+begin+look_at) > ' ' ; look_at++)
	  *(first_arg + look_at) = LOWER(*(argument + begin + look_at));
	
      }
      
      *(first_arg + look_at) = '\0';
      begin += look_at;
    }
  while (fill_word(first_arg));
  
  return(argument+begin);
}


struct help_index_element *build_help_index(FILE *fl, int *num)
{
	int nr = -1, issorted, i;
	struct help_index_element *list = 0, mem;
	char buf[81], tmp[81], *scan;
	long pos;

	for (;;)
	{
		pos = ftell(fl);
		fgets(buf, 81, fl);
		*(buf + strlen(buf) - 1) = '\0';
		scan = buf;
		for (;;)
		{
			/* extract the keywords */
			scan = one_word(scan, tmp);

			if (!*tmp)
				break;

			if (!list) {
				CREATE(list, struct help_index_element, 1);
				nr = 0;
			} else  {
			   RECREATE(list, struct help_index_element, ++nr+1);
			}

			list[nr].pos = pos;
			CREATE(list[nr].keyword, char, strlen(tmp) + 1);
			strcpy(list[nr].keyword, tmp);
		}
		/* skip the text */
		do
			fgets(buf, 81, fl);
		while (*buf != '#');
		if (*(buf + 1) == '~')
			break;
	}
	/* we might as well sort the stuff */
	do
	{
		issorted = 1;
		for (i = 0; i < nr; i++)
			if (str_cmp(list[i].keyword, list[i + 1].keyword) > 0)
			{
				mem = list[i];
				list[i] = list[i + 1];
				list[i + 1] = mem;
				issorted = 0;
			}
	}
	while (!issorted);

	*num = nr;
	return(list);
}



void page_string(struct descriptor_data *d, char *str, int keep_internal)
{
  if (!d)
    return;
  
  if (keep_internal)	{
    CREATE(d->showstr_head, char, strlen(str) + 1);
    strcpy(d->showstr_head, str);
    d->showstr_point = d->showstr_head;
  }
  else
    d->showstr_point = str;
  
  show_string(d, "");
}



void show_string(struct descriptor_data *d, char *input)
{
  char buffer[MAX_STRING_LENGTH], buf[MAX_INPUT_LENGTH];
  register char *scan, *chk;
  int lines = 0, toggle = 1;
  int i;  
  one_argument(input, buf);
  
  if (*buf)    {
    if (d->showstr_head){
      free(d->showstr_head);
      d->showstr_head = 0;
    }
    d->showstr_point = 0;
    return;
  }
  
  if(d->character->term == 0)
     i = 22;
  else
     i = d->character->size - 7;
  /* show a chunk */
  for (scan = buffer;; scan++, d->showstr_point++) {
    if((((*scan = *d->showstr_point) == '\n') || (*scan == '\r')) &&
       ((toggle = -toggle) < 0))
      lines++;
    else if (!*scan || (lines >= i))      {
      *scan = '\0';
      SEND_TO_Q(buffer, d);
      
      /* see if this is the end (or near the end) of the string */
      for (chk = d->showstr_point; isspace(*chk); chk++);
      if (!*chk)	  {
	if (d->showstr_head)	      {
	  free(d->showstr_head);
	  d->showstr_head = 0;
	}
	d->showstr_point = 0;
      }
      return;
    }
  }
}






void night_watchman()
{
  long tc;
  struct tm *t_info;
  
  extern int mudshutdown;
  
  void send_to_all(char *messg);
  
  tc = time(0);
  t_info = localtime(&tc);
  
  if ((t_info->tm_hour == 8) && (t_info->tm_wday > 0) &&
      (t_info->tm_wday < 6))
    if (t_info->tm_min > 50)
      {
	logE("Leaving the scene for the serious folks.");
	send_to_all("Closing down. Thank you for flying DikuMUD.\n\r");
	mudshutdown = 1;
      }
    else if (t_info->tm_min > 40)
      send_to_all("ATTENTION: DikuMUD will shut down in 10 minutes.\n\r");
    else if (t_info->tm_min > 30)
      send_to_all("Warning: The game will close in 20 minutes.\n\r");
}


void check_reboot()
{
  long tc;
  struct tm *t_info;
  char dummy;
  FILE *boot;
  
  extern int mudshutdown, reboot;
  
  tc = time(0);
  t_info = localtime(&tc);
  
  if ((t_info->tm_hour + 1) == REBOOT_AT && t_info->tm_min > 30)
    if (boot = fopen("./reboot", "r"))
      {
	if (t_info->tm_min > 50)
	  {
	    logE("Reboot exists.");
	    fread(&dummy, sizeof(dummy), 1, boot);
	    if (!feof(boot))   /* the file is nonepty */
	      {
		logE("Reboot is nonempty.");
		if (system("./reboot"))
		  {
		    logE("Reboot script terminated abnormally");
		    send_to_all("The reboot was cancelled.\n\r");
		    system("mv ./reboot reboot.FAILED");
		    fclose(boot);
		    return;
		  }
		else
		  system("mv ./reboot reboot.SUCCEEDED");
	      }
	    
	    send_to_all("Automatic reboot. Come back in a little while.\n\r");
	    mudshutdown = reboot = 1;
	  }
	else if (t_info->tm_min > 40)
	  send_to_all("ATTENTION: DikuMUD will reboot in 10 minutes.\n\r");
	else if (t_info->tm_min > 30)
	  send_to_all(
	      "Warning: The game will close and reboot in 20 minutes.\n\r");
	
	fclose(boot);
      }
}

#if 0
int workhours()
{
  long tc;
  struct tm *t_info;
  
  tc = time(0);
  t_info = localtime(&tc);
  
  return((t_info->tm_wday > 0) && (t_info->tm_wday < 6) && 
	 (t_info->tm_hour >= 9) && (t_info->tm_hour < 17));
}






/*
* This procedure is *heavily* system dependent. If your system is not set up
* properly for this particular way of reading the system load (It's weird all
* right - but I couldn't think of anything better), change it, or don't use -l.
* It shouldn't be necessary to use -l anyhow. It's oppressive and unchristian
* to harness man's desire to play. Who needs a friggin' degree, anyhow?
*/

int load()
{
  struct syslinfo {
    char	sl_date[12];	/* "Tue Sep 16\0" */
    char	sl_time[8];	/* "11:10\0" */
    char	sl_load1[6];	/* "12.0\0" */
    char	sl_load2[10];	/* "+2.3 14u\0" */
  } info;
  FILE *fl;
  int i, sum;
  static int previous[5];
  static int p_point = -1;
  extern int slow_death;
  
  if (!(fl = fopen("/tmp/.sysline", "r")))    {
    perror("sysline. (dying)");
    slow_death = 1;
    return(-1);
  }
  if (!fread(&info, sizeof(info), 1, fl))    {
    perror("fread sysline (dying)");
    slow_death = 1;
    return(-1);
  }
  fclose(fl);
  
  if (p_point < 0)    {
    previous[0] = atoi(info.sl_load1);
    for (i = 1; i< 5; i++)
      previous[i] = previous[0];
    p_point = 1;
    return(previous[0]);
  }  else    {
    /* put new figure in table */
    previous[p_point] = atoi(info.sl_load1);
    if (++p_point > 4)
      p_point = 0;
    
    for (i = 0, sum = 0; i < 5; i++)
      sum += previous[i];
    return((int) sum / 5);
  }
}

char *nogames()
{
  static char text[200];
  FILE *fl;
  
  if (fl = fopen("lib/nogames", "r"))
    {
      log("/usr/games/nogames exists");
      fgets(text, fl);
      return(text);
      fclose(fl);
    }
  else
    return(0);
}


/* emulate the game regulator */
void gr(int s)
{
  char *txt = 0, buf[1024];
  int ld = 0;
  static char *warnings[3] =
    {
      "If things don't look better within 3 minutes, the game will pause.\n\r",
      "The game will close temporarily 2 minutes from now.\n\r",
      "WARNING: The game will close in 1 minute.\n\r"
      };
  static int wnr = 0;
  
  extern int slow_death, mudshutdown;
  
  void send_to_all(char *messg);
  
  void coma(int s);
  
  if (((ld = load()) >= 6) || (txt = nogames()) || slow_death)
    {
      if (ld >= 6)	{
	  sprintf(buf, "The system load is greater than 6.0 (%d)\n\r", ld);
	  send_to_all(buf);
	}
      else if (slow_death)
	send_to_all("The game is dying.\n\r");
      else
	{
	  strcpy(buf,
		 "Game playing is no longer permitted on this machine:\n\r");
	  strcat(buf, txt);
	  strcat(buf, "\n\r"); 
	  send_to_all(buf);
	}
      
      if (wnr < 3)
	send_to_all(warnings[wnr++]);
      else
	if (ld >= 6)
	  {
	    coma(s);
	    wnr = 0;
	  }
	else
	  mudshutdown = 1;
    }
  else if (workhours())
    mudshutdown = 1;				/* this shouldn't happen */
  else if (wnr)
    {
      send_to_all("Things look brighter now - you can continue playing.\n\r");
      wnr = 0;
    }
}

#endif
