/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>

#include "protos.h"

/*   external vars  */

extern struct zone_data *zone_table;
extern int top_of_zone_table;
#if HASH
extern struct hash_header room_db;
#else
extern struct room_data *room_db[];
#endif
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct title_type titles[MAX_CLASS][ABS_MAX_LVL];
extern struct time_info_data time_info;
extern struct weather_data weather_info;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern int top_of_mobt;
extern int top_of_objt;
extern struct int_app_type int_app[26];
extern struct wis_app_type wis_app[26];
extern struct player_index_element *player_table;
extern char *room_bits[];


char EasySummon = 1;


#if PLAYER_AUTH
void do_auth(struct char_data *ch, char *argument, int cmd)
{
  char name[50], word[20];
  char buf[256],buf2[256];
  int done=FALSE;
  struct descriptor_data *d;


  if (IS_NPC(ch)) return;

  /* parse the argument */
  /* get char name */
   argument = one_argument(argument, name);
  /*
    search through descriptor list for player name
  */
  for (d=descriptor_list; d && !done; d = d->next) {
     if (d->character)
	if (GET_NAME(d->character) &&
           (str_cmp(GET_NAME(d->character), name) == 0))  {
              done = TRUE;
	      break;
        }
  }

  /*
    if not found, return error
  */
  if (!d) {
    send_to_char("That player was not found.\n\r", ch);
    return;
  }

  if (*argument) {
    /* get response (rest of argument) */
    one_argument(argument, word);
    if (str_cmp(word,"yes")==0) {
      d->character->generic = NEWBIE_START;
      sprintf(buf2,"%s has just accepted %s into the game.",
	      ch->player.name,name);
      log(buf2);
      SEND_TO_Q("You have been accepted.  Press enter\n\r", d);
    } else if (str_cmp(word,"no")==0){
      SEND_TO_Q("You have been denied.  Press enter\n\r", d);
      sprintf(buf2,"%s has just denied %s from the game.",
              ch->player.name,name);
      log(buf2);
      d->character->generic = NEWBIE_AXE;
    } else {
      SEND_TO_Q(argument, d);
      SEND_TO_Q("\n\r", d);
      sprintf(buf, "You send '%s'\n\r", argument);
      send_to_char(buf, ch);
      return;
    }
  } else {
    send_to_char("Auth[orize] {Yes | No | Message} \n\r", ch);
    return;
  }
  return;
}
#endif

void do_imptest(struct char_data *ch, char *arg, int cmd)
{
  struct char_data *i;
  struct StrHeap *H=0;
  int x = 0;

  if (strcmp(arg," test test test")) {   /* don't use this command on the */
    return;                             /* regular game */
  }

  if (GetMaxLevel(ch) < IMPLEMENTOR) return;

  H = InitHeap();

  for (i=character_list; i&&x++<100; i=i->next) {
       StringHeap(i->player.short_descr, H);
  } 

  DisplayStringHeap(H, ch, TO_CHAR, TRUE);

}

void do_passwd(struct char_data *ch, char *argument, int cmdnum)
{
   int player_i, pos;
   char name[30], npasswd[20], pass[PASSWORD_SIZE];
   struct char_file_u tmp_store;
   FILE *fl;

   /*
    *  sets the specified user's password.
    */

   /*
    *  get user's name:
    */
   argument = one_argument(argument, name);
   argument = one_argument(argument, npasswd);

   /*
    *   Look up character
    */
   
   if ((player_i = load_char(name, &tmp_store)) > -1)  {

   /*
    *  encrypt new password.
    */

      if (!*npasswd || strlen(npasswd) > 10) {
	send_to_char("Illegal password\n\r", ch);
	return;
      }

      memcpy(pass, crypt(npasswd, tmp_store.name), PASSWORD_SIZE);
      
   /*
    *  put new password in place of old password
    */

      memcpy(tmp_store.pwd, pass, PASSWORD_SIZE);

   /*
    *   save char to file
    */

      pos = player_table[player_i].nr;

      if (!(fl = fopen(PLAYER_FILE, "r+")))	{
      	perror("do_passwd");
       	assert(0);
      }

      fseek(fl, pos * sizeof(struct char_file_u), 0);

      fwrite(&tmp_store, sizeof(struct char_file_u), 1, fl);

      fclose(fl);

     return;
   } else {
     send_to_char("I don't recognize that name\n\r", ch);
     return;
   }
}

void do_setsev(struct char_data *ch, char *arg, int cmd)
{
  char buf[255];
  char buf2[255];
  int sev;

  arg = one_argument(arg, buf);
  
  if (strlen(buf) != 0) {
    sev = atoi(buf);
    if (sev < 0) {
      send_to_char("That number was incorrect, please choose > 0\n\r",ch);
      return;
    }
    ch->specials.sev = sev;
    sprintf(buf2,"Your severety level have been set to %d.\n\r",ch->specials.sev);
    send_to_char(buf2,ch);
    return;
  } else {
      send_to_char("Please give a number between 0 and 5\n\r",ch);
      return;
  }
}


/* Bamfin and bamfout - courtesy of DM from Epic */
void dsearch(char *string, char *tmp)
{
 char *c, buf[255], buf2[255], buf3[255];
 int i, j;

 i = 0;
 while(i == 0) {
    if(strchr(string, '~')==NULL) {
       i = 1;
       strcpy(tmp, string);
    } else {
       c = strchr(string, '~');
       j = c-string;
       switch(string[j+1]) {
         case 'N': strcpy(buf2, "$n"); break;
         case 'H': strcpy(buf2, "$s"); break;
         default:  strcpy(buf2, ""); break;
       }
       strcpy(buf, string);
       buf[j] = '\0';
       strcpy(buf3, (string+j+2));
       sprintf(tmp, "%s%s%s" ,buf, buf2, buf3);
       strcpy(string, tmp);

     }
  }
}

void do_bamfin(struct char_data *ch, char *arg, int cmd)
{
 char buf[255];
 int len;

 for (; *arg == ' '; arg++);  /* pass all those spaces */

 if(!*arg) {
   send_to_char("Bamfin <bamf definition>\n\r", ch);
   send_to_char(" Additional arguments can include ~N for where you\n\r", ch);
   send_to_char(" Want your name (if you want your name).  If you use\n\r",ch);
   send_to_char(" ~H, it puts in either his or her depending on your\n\r",ch);
   send_to_char(" sex.  If you use the keyword 'def' for your bamf,\n\r", ch);
   send_to_char(" it turns on the default bamf.  \n\r", ch);
   return;
 }

 if(!strcmp(arg, "def")) {
    REMOVE_BIT(ch->specials.pmask, BIT_POOF_IN);
    free(ch->specials.poofin);
    ch->specials.poofin = 0;
    send_to_char("Ok.\n\r", ch);
    return;
 }

 len = strlen(arg);

 if(len > 150) {
   send_to_char("String too long.  Truncated to:\n\r", ch);
   arg[150] = '\0';
   sprintf(buf, "%s\n\r", arg);
   send_to_char(buf, ch);   
   len = 150;
 }

 if (ch->specials.poofin && len >= strlen(ch->specials.poofin)) {
   free(ch->specials.poofin);
   ch->specials.poofin = (char *)malloc(len+1);
 } else { 
   if (!ch->specials.poofin)
     ch->specials.poofin = (char *)malloc(len+1);
 }

 strcpy(buf, arg);
 dsearch(buf, ch->specials.poofin);
 SET_BIT(ch->specials.pmask, BIT_POOF_IN);
 send_to_char("Ok.\n\r", ch);
 return;
}

void do_bamfout(struct char_data *ch, char *arg, int cmd)
{
 char buf[255];
 int len;

 for (; *arg == ' '; arg++);  /* pass all those spaces */

 if(!*arg) {
   send_to_char("Bamfout <bamf definition>\n\r", ch);
   send_to_char("  Additional arguments can include ~N for where you\n\r", ch);
   send_to_char("  Want your name (if you want your name). If you use\n\r",ch);
   send_to_char("  ~H, it puts in either his or her depending on your\n\r",ch);
   send_to_char("  sex.  If you use the keyword 'def' for your bamf,\n\r", ch);
   send_to_char("  it turns on the default bamf.  \n\r", ch);
   return;
 }

 if(!strcmp(arg, "def")) {
    REMOVE_BIT(ch->specials.pmask, BIT_POOF_OUT);
    free(ch->specials.poofout);
    ch->specials.poofout = 0;
    send_to_char("Ok.\n\r", ch);
    return;
  }

 len = strlen(arg);

 if(len > 150) {
   send_to_char("String too long.  Truncated to:\n\r", ch);
   arg[150] = '\0';
   sprintf(buf, "%s\n\r", arg);
   send_to_char(buf, ch);
   len = 150;
 }

 if (ch->specials.poofout && len >= strlen(ch->specials.poofout)) {
   free(ch->specials.poofout);
   ch->specials.poofout = (char *)malloc(len+1);
 } else if (!ch->specials.poofout) {
   ch->specials.poofout = (char *)malloc(len+1);
 }


 strcpy(buf, arg);
 dsearch(buf, ch->specials.poofout);
 SET_BIT(ch->specials.pmask, BIT_POOF_OUT);
 send_to_char("Ok.\n\r", ch);
 return;
}

void do_instazone(struct char_data *ch, char *argument, int cmdnum)
{
  char cmd, c, buf[80];
  int i, start_room, end_room, j, arg1, arg2, arg3;
  struct char_data *p;
  struct obj_data *o;
  struct room_data *room;
  FILE *fp;
  
  
  if (IS_NPC(ch))
    return;
  
  /*
   *   read in parameters (room #s)
   */
  start_room = -1; end_room = -1;
  sscanf(argument, "%d%c%d", &start_room, &c, &end_room);
  
  if ((start_room == -1) || (end_room == -1)) {
    send_to_char("Instazone <start_room> <end_room>\n\r", ch);
    return;
  }
  
  fp = (FILE *)MakeZoneFile(ch);
  
  if (!fp) {
    send_to_char("Couldn't make file.. try again later\n\r", ch);
    return;
  }
  
  for (i = start_room; i<=end_room; i++) {
    room = real_roomp(i);
    if (room) {
      /*
       *  first write out monsters
       */
      for (p = room->people; p; p = p->next_in_room) {
	if (IS_NPC(p)) {
	  cmd = 'M';
	  arg1 = MobVnum(p);
	  arg2 = mob_index[p->nr].number;
	  arg3 = i;
	  Zwrite(fp, cmd, 0, arg1, arg2, arg3, p->player.short_descr);
	  for (j = 0; j<MAX_WEAR; j++) {
	    if (p->equipment[j]) {
	      if (p->equipment[j]->item_number >= 0) {
		cmd = 'E'; 
		arg1 = ObjVnum(p->equipment[j]);
		arg2 = obj_index[p->equipment[j]->item_number].number;
		arg3 = j;
		strcpy(buf, p->equipment[j]->short_description);
		Zwrite(fp, cmd,1,arg1, arg2, arg3, 
		       buf);
		RecZwriteObj(fp, p->equipment[j]);
	      }
	    }
	  }
	  for (o = p->carrying; o; o=o->next_content) {
	    if (o->item_number >= 0) {
	      cmd = 'G';
	      arg1 = ObjVnum(o);
	      arg2 = obj_index[o->item_number].number;
	      arg3 = 0;
              strcpy(buf, o->short_description);
	      Zwrite(fp, cmd, 1, arg1, arg2, arg3, buf);
	      RecZwriteObj(fp, o);
	    }
	  }
	}
      }
      /*
       *  write out objects in rooms
       */
      for (o = room->contents; o; o= o->next_content) {
	if (o->item_number >= 0) {
	  cmd = 'O';
	  arg1 = ObjVnum(o);
	  arg2 = obj_index[o->item_number].number;
	  arg3 = i;
          strcpy(buf, o->short_description);
	  Zwrite(fp, cmd, 0, arg1, arg2, arg3, buf);
	  RecZwriteObj(fp, o);
	}
      }
      /*
       *  lastly.. doors
       */
      
      for (j = 0; j < 6; j++) {
	/*
	 *  if there is an door type exit, write it.
	 */
	if (room->dir_option[j]) {  /* is a door */
	  if (room->dir_option[j]->exit_info) {
	    cmd = 'D';
	    arg1 = i ;
	    arg2 = j;
	    arg3 = 0;
	    if (IS_SET(room->dir_option[j]->exit_info, EX_CLOSED)) {
	      arg3 = 1;
	    }
	    if (IS_SET(room->dir_option[j]->exit_info, EX_LOCKED)) {
	      arg3 = 2;
	    }
	    Zwrite(fp, cmd, 0, arg1, arg2, arg3, room->name);
	  }
	}
      }
    }
  }
  fclose(fp);

}


void do_highfive(struct char_data *ch, char *argument, int cmd)
{
  char buf[80];
  char mess[120];
  struct char_data *tch;


  if (argument) {
    only_argument(argument, buf);
    if ((tch = get_char_room_vis(ch,buf)) != 0) {
      if ((GetMaxLevel(tch) >= DEMIGOD) && (!IS_NPC(tch)) && 
	  (GetMaxLevel(ch)  >= DEMIGOD) && (!IS_NPC(ch))) {
       sprintf(mess, "Time stops for a moment as %s and %s high five.\n\r",
	    ch->player.name, tch->player.name);
       send_to_all(mess);
      } else {
       act("$n gives you a high five", TRUE, ch, 0, tch, TO_VICT);
       act("You give a hearty high five to $N", TRUE, ch, 0, tch, TO_CHAR);
       act("$n and $N do a high five.", TRUE, ch, 0, tch, TO_NOTVICT);
      }
    } else {
      sprintf(buf,"I don't see anyone here like that.\n\r");
      send_to_char(buf,ch);
    }
  }
}


void do_addhost(struct char_data *ch, char *argument, int command)
{
}

void do_removehost(struct char_data *ch, char *argument, int command)
{
}

void do_listhosts(struct char_data *ch, char *argument, int command)
{
}

void do_silence(struct char_data *ch, char *argument, int cmd)
{
  char buf[255];
  extern int Silence;
  if ((GetMaxLevel(ch) < DEMIGOD) || (IS_NPC(ch))) {
    send_to_char("You cannot Silence.\n\r",ch);
    return;
  }
  
  if (Silence == 0) {
    Silence = 1;
    send_to_char("You have now silenced polyed mobles.\n\r",ch);
    sprintf(buf,"%s has stopped Polymophed characters from shouting.",ch->player.name);
    logE(buf);
  } else {
    Silence = 0;
    send_to_char("You have now unsilenced mobles.\n\r",ch);
    sprintf(buf,"%s has allowed Polymophed characters to shout.",ch->player.name);
    logE(buf);
  }
}
void do_wizlock(struct char_data *ch, char *argument, int cmd)
{
#if SITELOCK
  char *test;
  int a, length, b;
  char buf[255];

  extern int numberhosts;
  extern char hostlist[MAX_BAN_HOSTS][30];
  
#endif
  extern int WizLock;


  if ((GetMaxLevel(ch) < DEMIGOD) || (IS_NPC(ch))) {
    send_to_char("You cannot WizLock.\n\r",ch);
    return;
  }


#if SITELOCK
  /* 4 commands */
  /* all,  add (place),  list,  rem  (place) */

   for(test = argument; *test && isspace(*test) ; test++);
   if (!*test) return;
   argument = test;

   /*
   **  get first piece..
   */

   argument = one_argument(argument, buf);
   if (!*buf) {
     send_to_char("Wizlock {all | off | add <host> | rem <host> | list}\n\r", ch);
     return;
   }

   if (str_cmp(buf, "all")==0) {

     if (WizLock) {
       send_to_char("Its already on!\n\r", ch);
     } else {
       send_to_char("WizLock is now on\n\r",ch);
       log("WizLock is now on.");
       WizLock = TRUE;
     }

   } else if (str_cmp(buf, "off")==0) {

     if (!WizLock) {
       send_to_char("Its already off!\n\r", ch);
     } else {
       send_to_char("WizLock is now off\n\r",ch);
       log("WizLock is now off.");
       WizLock = FALSE;
     }

   } else if (str_cmp(buf, "add")==0) {

     argument = one_argument(argument,buf);
     if (!*buf) {
       send_to_char("Wizlock add <host_name>\n\r", ch);
       return;
     }
     length = strlen(buf);
     if ((length <= 3) || (length >= 30)) {
         send_to_char("Host is too long or short, please try again\n\r", ch);
         return;
      } 
      for( a = 0 ; a <= numberhosts-1 ; a++) {
         if (strncmp(hostlist[a],buf,length) == 0) {
	    send_to_char("Host is already in database\n\r", ch);
	    return;
         }
      }
      strcpy(hostlist[numberhosts],buf);
      sprintf(buf,"%s has added host %s to the access denied list.",
	      GET_NAME(ch),hostlist[numberhosts]);
      log(buf);
      numberhosts++;
      return;

   } else if (str_cmp(buf, "rem")==0) {

      if (numberhosts <= 0) {
         send_to_char("Host list is empty.\n\r", ch);
         return;
      }

      for(test = argument; *test && isspace(*test) ; test++);
      if (!*test) {
	send_to_char("Wizlock rem <host_name>\n\r", ch);
	return;
      }
      argument = test;

      argument = one_argument(argument, buf);

      if (!*buf) {
	send_to_char("Wizlock rem <host_name>\n\r", ch);
	return;
      }

      length = strlen(buf);
      if ((length <= 3) || (length >= 30)) {
         send_to_char("Host length is bad, please try again\n\r", ch);
         return;
      }
 
      for( a = 0 ; a <= numberhosts-1 ; a++) {
         if (strncmp(hostlist[a],buf,length) == 0) {
	    for( b = a ; b <= numberhosts ; b++) 
	      strcpy(hostlist[b],hostlist[b+1]);
 	    sprintf(buf,"%s has removed host %s from the access denied list.",
		    GET_NAME(ch),hostlist[numberhosts]);
	    log(buf);
	    numberhosts--;
	    return;
	 }
      }
      send_to_char("Host is not in database\n\r", ch);
      return;

   } else if (str_cmp(buf, "list")==0) {

      if (numberhosts <= 0) {
         send_to_char("Host list is empty.\n\r", ch);
         return;
      }
      for( a = 0 ; a <= numberhosts-1 ; a++) {
         sprintf(buf,"Host: %s\n",hostlist[a]);
         send_to_char(buf, ch);
      }
      return;

   } else {
     send_to_char("Wizlock {all | add <host> | rem <host> | list}\n\r", ch);
     return;
   }


#else
  if (WizLock) {
    send_to_char("WizLock is now off\n\r",ch);
    logE("Wizlock is now off.");
    WizLock = FALSE;
  } else {
    send_to_char("WizLock is now on\n\r",ch);
    logE("WizLock is now on.");
    WizLock = TRUE;
  }
#endif
  return;

}

void do_rload(struct char_data *ch, char *argument, int cmd)
{

   char i;
   int start= -1, end = -2; 

   if (IS_NPC(ch)) return;
   if (GetMaxLevel(ch) < IMMORTAL) return;

   for (i = 0; *(argument + i) == ' '; i++);
   if (!*(argument + i)) {
      send_to_char("Load? Fine!  Load we must, But what?\n\r", ch);
      return;
   }
   sscanf(argument,"%d %d", &start, &end);
      
   if ((start <= end) && (start != -1) && (end != -2)) {
     RoomLoad(ch,start,end);
   }
}


void do_rsave(struct char_data *ch, char *argument, int cmd)
{
   char i, buf[256];
   int start= -1, end = -2; 

   if (IS_NPC(ch)) return;
   if (GetMaxLevel(ch) < IMMORTAL) return;

   for (i = 0; *(argument + i) == ' '; i++);
   if (!*(argument + i)) {
      send_to_char("Save? Fine!  Save we must, But what?  The whales?\n\r",
		   ch);
      return;
    }
    sscanf(argument,"%d %d", &start, &end);
      
    if ((start <= end) && (start != -1) && (end != -2)) {
      sprintf(buf,"mv rooms/%s rooms/%s.bak", GET_NAME(ch), GET_NAME(ch));
      system(buf);
      RoomSave(ch,start,end);
    }
}


void do_emote(struct char_data *ch, char *argument, int cmd)
{
	int i;
	char buf[MAX_INPUT_LENGTH];

	if (check_soundproof(ch)) {
	  return;
	}


	for (i = 0; *(argument + i) == ' '; i++);

	if (!*(argument + i))
		send_to_char("Yes.. But what?\n\r", ch);
	else	{
		sprintf(buf,"$n %s", argument + i);
		act(buf,FALSE,ch,0,0,TO_ROOM);
		send_to_char("Ok.\n\r", ch);
	}
}



void do_echo(struct char_data *ch, char *argument, int cmd)
{
	int i;
	char buf[MAX_INPUT_LENGTH];
	
	if (IS_NPC(ch))
		return;

	for (i = 0; *(argument + i) == ' '; i++);

	if (!*(argument + i)) {
	  if (IS_SET(ch->specials.act, PLR_ECHO)) {
	    send_to_char("echo off\n\r", ch);
	    REMOVE_BIT(ch->specials.act, PLR_ECHO);
	  } else {
	    SET_BIT(ch->specials.act, PLR_ECHO);
	    send_to_char("echo on\n\r", ch);
	  }
	} else	{
	  if (IS_IMMORTAL(ch)) {
		sprintf(buf,"%s\n\r", argument + i);
		send_to_room_except(buf, ch->in_room, ch);
		send_to_char("Ok.\n\r", ch);
	      }
	}
}

void do_system(struct char_data *ch, char *argument, int cmd)
{
	int i;
	char buf[256];
	
	if (IS_NPC(ch))
		return;

	for (i = 0; *(argument + i) == ' '; i++);

	if (!*(argument + i))
		send_to_char("That must be a mistake...\n\r", ch);
	else	{
		sprintf(buf,"\n\r%s\n\r", argument + i);
                send_to_all(buf);
	}
}



void do_trans(struct char_data *ch, char *argument, int cmd)
{
  struct descriptor_data *i;
	struct char_data *victim;
	char buf[100];
	sh_int target;

	if (IS_NPC(ch))
		return;

	only_argument(argument,buf);
	if (!*buf)
		send_to_char("Who do you wich to transfer?\n\r",ch);
	else if (str_cmp("all", buf)) {
		if (!(victim = get_char_vis_world(ch,buf, NULL)))
			send_to_char("No-one by that name around.\n\r",ch);
		else {
			act("$n is ripped out of reality.", FALSE, victim, 0, 0, TO_ROOM);
			target = ch->in_room;
			char_from_room(victim);
			char_to_room(victim,target);
			act("Reality shifts, and $n appears.", FALSE, victim, 0, 0, TO_ROOM);
			act("Reality transforms itself as $n's hand grabs you.",FALSE,ch,0,victim,TO_VICT);
			do_look(victim,"",15);
			send_to_char("Ok.\n\r",ch);
		}
	} else { /* Trans All */
    for (i = descriptor_list; i; i = i->next)
			if (i->character != ch && !i->connected) {
				victim = i->character;
				act("$n disappears in a cloud of mushrooms.", FALSE, victim, 0, 0, TO_ROOM);
				target = ch->in_room;
				char_from_room(victim);
				char_to_room(victim,target);
				act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
				act("$n has transferred you!",FALSE,ch,0,victim,TO_VICT);
				do_look(victim,"",15);
			}

		send_to_char("Ok.\n\r",ch);
	}
}



void do_at(struct char_data *ch, char *argument, int cmd)
{
  char command[MAX_INPUT_LENGTH], loc_str[MAX_INPUT_LENGTH];
  int loc_nr, location, original_loc;
  struct char_data *target_mob;
  struct obj_data *target_obj;
  
  if (IS_NPC(ch))
    return;
  
  half_chop(argument, loc_str, command);
  if (!*loc_str)
    {
      send_to_char("You must supply a room number or a name.\n\r", ch);
      return;
    }
  
  
  if (isdigit(*loc_str))
    {
      loc_nr = atoi(loc_str);
      if (NULL==real_roomp(loc_nr)) {
	send_to_char("No room exists with that number.\n\r", ch);
	return;
      }
      location = loc_nr;
    } else if (target_mob = get_char_vis(ch, loc_str)) {
      location = target_mob->in_room;
    } else if (target_obj=get_obj_vis_world(ch, loc_str, NULL))
      if (target_obj->in_room != NOWHERE)
	location = target_obj->in_room;
      else
	{
	  send_to_char("The object is not available.\n\r", ch);
	  return;
	}
    else
      {
	send_to_char("No such creature or object around.\n\r", ch);
	return;
      }
  
  /* a location has been found. */
  
  original_loc = ch->in_room;
  char_from_room(ch);
  char_to_room(ch, location);
  command_interpreter(ch, command);
  
  /* check if the guy's still there */
  for (target_mob = real_roomp(location)->people; target_mob; target_mob =
       target_mob->next_in_room)
    if (ch == target_mob) {
      char_from_room(ch);
      char_to_room(ch, original_loc);
    }
}

void do_goto(struct char_data *ch, char *argument, int cmd)
{
  char buf[MAX_INPUT_LENGTH];
  int loc_nr, location, i;
  struct char_data *target_mob, *pers, *v;
  struct obj_data *target_obj;
  extern int top_of_world;
  
  void do_look(struct char_data *ch, char *argument, int cmd);
  
  if (IS_NPC(ch))
    return;
  if ((GetMaxLevel(ch) >0 ) && (GetMaxLevel(ch) < LOW_IMMORTAL)) {
     send_to_char("Huh?\n\r", ch);
     return;
   }

  only_argument(argument, buf);
  if (!*buf)	{
    send_to_char("You must supply a room number or a name.\n\r", ch);
    return;
  }
  
  if (isdigit(*buf) && NULL==index(buf, '.'))	{
    loc_nr = atoi(buf);
    if (NULL==real_roomp(loc_nr)) {
      if (GetMaxLevel(ch)<CREATOR || loc_nr < 0) {
	send_to_char("No room exists with that number.\n\r", ch);
	return;
      } else {
#if HASH
#else
	if (loc_nr < WORLD_SIZE) {
#endif
      	   send_to_char("You form order out of chaos.\n\r", ch);
	   CreateOneRoom(loc_nr);

#if HASH
#else
	 } else {
	   send_to_char("Sorry, that room # is too large.\n\r", ch);
	   return;
	 }
#endif
      }
    }
    location = loc_nr;
  }
  else if (target_mob = get_char_vis_world(ch, buf, NULL))
    location = target_mob->in_room;
  else if (target_obj=get_obj_vis_world(ch, buf, NULL))
    if (target_obj->in_room != NOWHERE)
      location = target_obj->in_room;
    else   	{
      send_to_char("The object is not available.\n\r", ch);
      send_to_char("Try where #.object to nail its room number.\n\r", ch);
      return;
    }
  else	{
    send_to_char("No such creature or object around.\n\r", ch);
    return;
  }
  
  /* a location has been found. */
  
  
  if (!real_roomp(location)) {
    logE("Massive error in do_goto. Everyone Off NOW.");
    return;
  }
  
  if (IS_SET(real_roomp(location)->room_flags, PRIVATE) && GetMaxLevel(ch) < 59) {
    for (i = 0, pers = real_roomp(location)->people; pers; pers =
	 pers->next_in_room, i++);
    if (i > 1)
      {
	send_to_char(
	   "There's a private conversation going on in that room.\n\r", ch);
	return;
      }
  }
  
  if (IS_SET(ch->specials.act, PLR_STEALTH)) {
    for (v = real_roomp(ch->in_room)->people; v; v= v->next_in_room) {
      if ((ch != v) && (GetMaxLevel(v) >= LOW_IMMORTAL)) {
	if (!IS_SET(ch->specials.pmask, BIT_POOF_OUT) || !ch->specials.poofout)
	  act("$n disappears in a cloud of mushrooms.", 
	      FALSE, ch, 0, v, TO_VICT);
	else {
	  act(ch->specials.poofout, FALSE, ch, 0, v, TO_VICT);
	}
      }
    }
  } else {
    if (!IS_SET(ch->specials.pmask, BIT_POOF_OUT) || !ch->specials.poofout)
	act("$n disappears in a cloud of mushrooms.", 
	    FALSE, ch, 0, 0, TO_ROOM);
    else
      if (*ch->specials.poofout != '!')
	act(ch->specials.poofout, FALSE, ch, 0, 0, TO_ROOM);
      else
	command_interpreter(ch, (ch->specials.poofout+1));
  }
  
  if (ch->specials.fighting)
    stop_fighting(ch);
  char_from_room(ch);
  char_to_room(ch, location);
  
  if (IS_SET(ch->specials.act, PLR_STEALTH)) {
    for (v = real_roomp(ch->in_room)->people; v; v= v->next_in_room) {
      if ((ch != v) && (GetMaxLevel(v) >= LOW_IMMORTAL)) {

	if (!IS_SET(ch->specials.pmask, BIT_POOF_IN)|| !ch->specials.poofin)
	  act("$n appears with an explosion of rose-petals.", 
	      FALSE, ch, 0,v,TO_VICT);	
	else
	  act(ch->specials.poofin, FALSE, ch, 0, v, TO_VICT);
      }
    }
  } else {

    if (!IS_SET(ch->specials.pmask, BIT_POOF_IN)|| !ch->specials.poofin)
      act("$n appears with an explosion of rose-petals.", 
	  FALSE, ch, 0,v,TO_ROOM);	
    else
      if (*ch->specials.poofin != '!')
	act(ch->specials.poofin, FALSE, ch, 0, v, TO_ROOM);
      else
	command_interpreter(ch, (ch->specials.poofin+1));
  }
  do_look(ch, "",15);
}





void do_stat(struct char_data *ch, char *argument, int cmd)
{
  extern char *spells[];
  struct affected_type *aff;
  char arg1[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  struct room_data *rm=0;
  struct char_data *k=0;
  struct obj_data  *j=0;
  struct obj_data  *j2=0;
  struct extra_descr_data *desc;
  struct follow_type *fol;
  int i, virtual;
  int i2, count;
  bool found;
  
  /* for objects */
  extern char *item_types[];
  extern char *wear_bits[];
  extern char *extra_bits[];
  extern char *drinks[];
  
  /* for rooms */
  extern char *dirs[];

  extern char *exit_bits[];
  extern char *sector_types[];
  
  /* for chars */
  extern char *equipment_types[];
  extern char *affected_bits[];
  extern char *affected_bits2[];
  extern char *immunity_names[];
  extern char *apply_types[];
  extern char *pc_class_types[];
  extern char *npc_class_types[];
  extern char *action_bits[];
  extern char *player_bits[];
  extern char *position_types[];
  extern char *connected_types[];
  extern char *RaceName[];
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, arg1);
  
  /* no argument */
  if (!*arg1) {
    send_to_char("Stats on who or what?\n\r",ch);
    return;
  } else {
    /* stats on room */
    if (!str_cmp("room", arg1)) {
      rm = real_roomp(ch->in_room);
      sprintf(buf, "Room name: %s, Of zone : %d. V-Number : %d, R-number : %d\n\r",
	      rm->name, rm->zone, rm->number, ch->in_room);
      send_to_char(buf, ch);
      
      sprinttype(rm->sector_type,sector_types,buf2);
      sprintf(buf, "Sector type : %s ", buf2);
      send_to_char(buf, ch);
      
      strcpy(buf,"Special procedure : ");
      strcat(buf,(rm->funct) ? "Exists\n\r" : "No\n\r");
      send_to_char(buf, ch);
      
      send_to_char("Room flags: ", ch);
      sprintbit((unsigned long) rm->room_flags,room_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      
      send_to_char("Description:\n\r", ch);
      send_to_char(rm->description, ch);
      
      strcpy(buf, "Extra description keywords(s): ");
      if(rm->ex_description) {
	strcat(buf, "\n\r");
	for (desc = rm->ex_description; desc; desc = desc->next) {
	  strcat(buf, desc->keyword);
	  strcat(buf, "\n\r");
	}
	strcat(buf, "\n\r");
	send_to_char(buf, ch);
      } else {
	strcat(buf, "None\n\r");
	send_to_char(buf, ch);
      }
      
      strcpy(buf, "------- Chars present -------\n\r");
      for (k = rm->people; k; k = k->next_in_room)
	{
	  strcat(buf, GET_NAME(k));
	  strcat(buf,(!IS_NPC(k) ? "(PC)\n\r" : (!IS_MOB(k) ? "(NPC)\n\r" : "(MOB)\n\r")));
	}
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
      
      strcpy(buf, "--------- Contents ---------\n\r");
      for (j = rm->contents; j; j = j->next_content)
	{
	  strcat(buf, j->name);
	  strcat(buf, "\n\r");
	}
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
      
      send_to_char("------- Exits defined -------\n\r", ch);
      for (i = 0; i <= 5; i++) {
	if (rm->dir_option[i]) {
	  if (rm->dir_option[i]->keyword) {
	    sprintf(buf,"Direction %s . Keyword : %s\n\r",
		    dirs[i], rm->dir_option[i]->keyword);
	    send_to_char(buf, ch);
	  } else {
	    sprintf(buf,"Direction %s \n\r",   dirs[i]);
	    send_to_char(buf, ch);
	  }
	  strcpy(buf, "Description:\n\r  ");
	  if(rm->dir_option[i]->general_description)
	    strcat(buf, rm->dir_option[i]->general_description);
	  else
	    strcat(buf,"UNDEFINED\n\r");
	  send_to_char(buf, ch);
	  sprintbit((unsigned) rm->dir_option[i]->exit_info,exit_bits,buf2);
	  sprintf(buf, "Exit flag: %s \n\rKey no: %d\n\rTo room (R-Number): %d\n\r",
		  buf2, rm->dir_option[i]->key,
		  rm->dir_option[i]->to_room);
	  send_to_char(buf, ch);
	}
      }
      return;
    }
    
    count = 1;
    
    /* mobile in world */
    if (k = get_char_vis_world(ch, arg1, &count)){

      struct time_info_data ma;
      
      switch(k->player.sex) {
      case SEX_NEUTRAL : 
	strcpy(buf,"NEUTRAL-SEX"); 
	break;
      case SEX_MALE :
	strcpy(buf,"MALE");
	break;
      case SEX_FEMALE :
	strcpy(buf,"FEMALE");
	break;
	default : 
	  strcpy(buf,"ILLEGAL-SEX!!");
	break;
      }
      
      sprintf(buf2, " %s - Name : %s [R-Number%d], In room [%d]\n\r",
	      (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
	      GET_NAME(k), k->nr, k->in_room);
      strcat(buf, buf2);
      send_to_char(buf, ch);
      if (IS_MOB(k)) {
	sprintf(buf, "V-Number [%d]\n\r", mob_index[k->nr].virtual);
	send_to_char(buf, ch);
      }
      
      strcpy(buf,"Short description: ");
      strcat(buf, (k->player.short_descr ? k->player.short_descr : "None"));
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      
      strcpy(buf,"Title: ");
      strcat(buf, (k->player.title ? k->player.title : "None"));
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      
      send_to_char("Long description: ", ch);
      if (k->player.long_descr)
	send_to_char(k->player.long_descr, ch);
      else
	send_to_char("None", ch);
      send_to_char("\n\r", ch);
      
      if (IS_NPC(k)) {
	strcpy(buf,"Monster Class: ");
	sprinttype(k->player.class,npc_class_types,buf2);
      } else {
	strcpy(buf,"Class: ");
	sprintbit((unsigned)k->player.class,pc_class_types, buf2);
      }
      strcat(buf, buf2);
      
      sprintf(buf2,"   Level [%d/%d/%d/%d/%d/%d/%d] Alignment[%d]\n\r",
	      k->player.level[0], k->player.level[1], 
	      k->player.level[2], k->player.level[3], 
	      k->player.level[4], k->player.level[5],
              k->player.level[6], GET_ALIGNMENT(k));

      strcat(buf, buf2);
      send_to_char(buf, ch);
      
      sprintf(buf,"Birth : [%ld]secs, Logon[%ld]secs, Played[%d]secs\n\r", 
	      k->player.time.birth,
	      k->player.time.logon,
	      k->player.time.played);
      
      send_to_char(buf, ch);

      age2(k, &ma);
      
      sprintf(buf,"Age: [%d] Y, [%d] M, [%d] D, [%d] H. ",
	      ma.year, ma.month, ma.day, ma.hours);
      send_to_char(buf,ch);
      
      sprintf(buf," Height [%d]cm, Wgt [%d]pounds \n\r", GET_HEIGHT(k), GET_WEIGHT(k));
      send_to_char(buf,ch);
      
      sprintf(buf,"Str:[%d/%d] Int:[%d] Ws:[%d] Dex:[%d] Con:[%d] Ch:[%d], Ego: [%d]\n\r",
	      GET_STR(k), GET_ADD(k),
	      GET_INT(k),
	      GET_WIS(k),
	      GET_DEX(k),
	      GET_CON(k),
	      GET_CHR(k),
	      GET_EGO(k));
      send_to_char(buf,ch);
      
      sprintf(buf,"Mana:[%d/%d+%d] Hit:[%d/%d] Move:[%d/%d+%d]\n\r",
	      GET_MANA(k),mana_limit(k),mana_gain(k),
	      GET_HIT(k),hit_limit(k),
	      GET_MOVE(k),move_limit(k),move_gain(k) );
      send_to_char(buf,ch);
      
      sprintf(buf,"AC:[%d/10], Coins: [%d], Exp: [%d], Hitroll: [%d], Damroll: [%d] sf[%d]\n\r",
	      GET_AC(k),
	      GET_GOLD(k),
	      GET_EXP(k),
	      k->points.hitroll,
	      k->points.damroll,
	      k->specials.spellfail);
      send_to_char(buf,ch);
      
      sprinttype(GET_POS(k),position_types,buf2);
      sprintf(buf,"Position: %s, Fighting: %s",buf2,
	      ((k->specials.fighting) ? GET_NAME(k->specials.fighting) : "Nobody") );
      if (k->desc) {
	sprinttype(k->desc->connected,connected_types,buf2);
	strcat(buf,", Connected: ");
	strcat(buf,buf2);
      }
      strcat(buf,"\n\r");
      send_to_char(buf, ch);
      
      strcpy(buf,"Default position: ");
      sprinttype((k->specials.default_pos),position_types,buf2);
      strcat(buf, buf2);
      if (IS_NPC(k)) 	{
	strcat(buf,",NPC flags: ");
	sprintbit(k->specials.act,action_bits,buf2);
      }  else	{
	strcat(buf,",PC flags: ");
	sprintbit(k->specials.act,player_bits,buf2);
      }
      
      strcat(buf, buf2);
      
      sprintf(buf2,",Timer [%d] \n\r", k->specials.timer);
      strcat(buf, buf2);
      send_to_char(buf, ch);
      
      if (IS_MOB(k)) {
	strcpy(buf, "\n\rMobile Special procedure : ");
	strcat(buf, (mob_index[k->nr].func ? "Exists " : "None "));
	send_to_char(buf, ch);
      }
      
      if (IS_NPC(k)) {
	sprintf(buf, " NPC Bare Hand Damage %dd%d.\n\r",
		k->specials.damnodice, k->specials.damsizedice);
	send_to_char(buf, ch);
      }
      
      sprintf(buf,"Carried weight: %d   Carried items: %d ",
	      IS_CARRYING_W(k),
	      IS_CARRYING_N(k) );

      for(i=0,i2=0;i<MAX_WEAR;i++)
	if (k->equipment[i]) i2++;
      sprintf(buf2,"Items in equipment: %d\n\r", i2);
      strcat(buf,buf2);
      send_to_char(buf, ch);
      
      sprintf(buf,"Apply saving throws: [%d] [%d] [%d] [%d] [%d], ",
	      k->specials.apply_saving_throw[0],
	      k->specials.apply_saving_throw[1],
	      k->specials.apply_saving_throw[2],
	      k->specials.apply_saving_throw[3],
	      k->specials.apply_saving_throw[4]);
      send_to_char(buf, ch);
      
      sprintf(buf, "Thirst: %d, Hunger: %d, Drunk: %d\n\r",
	      k->specials.conditions[THIRST],
	      k->specials.conditions[FULL],
	      k->specials.conditions[DRUNK]);
      send_to_char(buf, ch);
      
      sprintf(buf, "Master is '%s'    ",
	      ((k->master) ? GET_NAME(k->master) : "NOBODY"));
      send_to_char(buf, ch);
      send_to_char("Followers are:\n\r", ch);
      for(fol=k->followers; fol; fol = fol->next)
	act("    $N", FALSE, ch, 0, fol->follower, TO_CHAR);
      
      /* immunities */
      send_to_char("Immune to:",ch); 
      sprintbit(k->M_immune, immunity_names, buf);
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      /* resistances */
      send_to_char("Resistant to:",ch); 
      sprintbit(k->immune, immunity_names, buf);
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      /* Susceptible */
      send_to_char("Susceptible to:",ch); 
      sprintbit(k->susc, immunity_names, buf);
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      /*  race, action pointer */
      send_to_char("Race: ",ch);
      sprinttype((k->race),RaceName,buf2);
      send_to_char(buf2, ch);
      sprintf(buf, "  Generic pointer: %d\n\r", (int)k->generic);
      send_to_char(buf, ch);
      
      /* Showing the bitvector */
      sprintbit((unsigned)k->specials.affected_by,affected_bits,buf);
      send_to_char("Affected by: ", ch);
      send_to_char(buf, ch);
      sprintbit((unsigned)k->specials.affected_by2,affected_bits2,buf);
      strcat(buf, "\n\r");
      send_to_char(buf, ch);

      /* Routine to show what spells a char is affected by */
      if (k->affected) {
	send_to_char("\n\rAffecting Spells:\n\r--------------\n\r", ch);
	for(aff = k->affected; aff; aff = aff->next) {

	  /* This is somewhat of a hack in order to  */
          if(aff->location == APPLY_IMMUNE && !(aff->modifier) &&
             aff->bitvector) {
            sprintf(buf, "Spell : '%s'\n\r",spells[aff->type-1]);
            sprintf(buf,"     Modifies %s by %ld points\n\r",
                    apply_types[aff->location],aff->bitvector);
            send_to_char(buf,ch);
            sprintf(buf,"     Expires in %3d hours, Resistance Bits set ",
                    aff->duration);
            send_to_char(buf, ch);
            sprintbit((unsigned)aff->bitvector,immunity_names,buf);
            strcat(buf,"\n\r");
            send_to_char(buf, ch);
          } else {
	    sprintf(buf, "Spell : '%s'\n\r",spells[aff->type-1]);
	    send_to_char(buf, ch);
	    sprintf(buf,"     Modifies %s by %ld points\n\r",
		    apply_types[aff->location], aff->modifier);
	    send_to_char(buf, ch);
	    sprintf(buf,"     Expires in %3d hours, Bits set ",
		    aff->duration);
	    send_to_char(buf, ch);
	    if (aff->location != APPLY_BV2)
	      sprintbit((unsigned)aff->bitvector,affected_bits,buf);
	    else 
	      sprintbit((unsigned)aff->bitvector,affected_bits2,buf);
	    strcat(buf,"\n\r");
	    send_to_char(buf, ch);
	  }
	}
      }
      return;
    }
    /* stat on object */
    if (j=(struct obj_data *)get_obj_vis_world(ch, arg1, &count)) {
      virtual = (j->item_number >= 0) ? obj_index[j->item_number].virtual : 0;
      sprintf(buf, "Object name: [%s], R-number: [%d], V-number: [%d] Item type: ",
	      j->name, j->item_number, virtual);
      sprinttype(GET_ITEM_TYPE(j),item_types,buf2);
      strcat(buf,buf2); strcat(buf,"\n\r");
      send_to_char(buf, ch);
      sprintf(buf, "Short description: %s\n\rLong description:\n\r%s\n\r",
	      ((j->short_description) ? j->short_description : "None"),
	      ((j->description) ? j->description : "None") );
      send_to_char(buf, ch);
      if(j->ex_description){
	strcpy(buf, "Extra description keyword(s):\n\r----------\n\r");
	for (desc = j->ex_description; desc; desc = desc->next) {
	  strcat(buf, desc->keyword);
	  strcat(buf, "\n\r");
	}
	strcat(buf, "----------\n\r");
	send_to_char(buf, ch);
      } else {
	strcpy(buf,"Extra description keyword(s): None\n\r");
	send_to_char(buf, ch);
      }
      
      send_to_char("Can be worn on :", ch);
      sprintbit((unsigned)j->obj_flags.wear_flags,wear_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf, ch);
      
      send_to_char("Set char bits  :", ch);
      sprintbit((unsigned)j->obj_flags.bitvector,affected_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf, ch);
      
      send_to_char("Extra flags: ", ch);
      sprintbit((unsigned)j->obj_flags.extra_flags,extra_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf,ch);
      
      sprintf(buf,"Weight: %d, Value: %d, Cost/day: %d, Timer: %d, Ego: %d\n\r",
	      j->obj_flags.weight,j->obj_flags.cost,
	      j->obj_flags.cost_per_day,  j->obj_flags.timer, 
	      GET_OBJ_EGO(j));
      send_to_char(buf, ch);
      
      strcpy(buf,"In room: ");
      if (j->in_room == NOWHERE)
	strcat(buf,"Nowhere");
      else {
	sprintf(buf2,"%d",j->in_room);
	strcat(buf,buf2);
      }
      strcat(buf," ,In object: ");
      strcat(buf, (!j->in_obj ? "None" : fname(j->in_obj->name)));
      
      switch (j->obj_flags.type_flag) {
      case ITEM_LIGHT : 
	sprintf(buf, "Colour : [%d]\n\rType : [%d]\n\rHours : [%d]",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[2]);
	break;
      case ITEM_SCROLL : 
	sprintf(buf, "Spells : %d, %d, %d, %d",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[2],
		j->obj_flags.value[3] );
	break;
      case ITEM_WAND :
      case ITEM_STAFF:
	sprintf(buf, "Level: %d Spell : %d\n\rCharges : %d",
		j->obj_flags.value[0],
		j->obj_flags.value[3],
		j->obj_flags.value[2]);
	break;
      case ITEM_WEAPON :
	sprintf(buf, "Tohit : %d\n\rTodam : %dD%d\n\rType : %d",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[2],
		j->obj_flags.value[3]);
	break;
      case ITEM_FIREWEAPON : 
	sprintf(buf, "Tohit: %d\n\rDam: %d\n\rType: %d Min Weight: %d\n\r",
		j->obj_flags.value[2],
		j->obj_flags.value[3],
		j->obj_flags.value[0],
		j->obj_flags.value[1]);
	break;
      case ITEM_MISSILE : 
	sprintf(buf, "Tohit : %d\n\rTodam : %d\n\rType : %d",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[3]);
	break;
      case ITEM_ARMOR :
	sprintf(buf, "AC-apply : [%d]\n\rFull Strength : [%d]",
		j->obj_flags.value[0],
		j->obj_flags.value[1]);
	
	break;
      case ITEM_POTION : 
	sprintf(buf, "Spells : %d, %d, %d, %d",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[2],
		j->obj_flags.value[3]); 
	break;
      case ITEM_TRAP :
	sprintf(buf, "level: %d, att type: %d, damage class: %d, charges: %d", 
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[2],
		j->obj_flags.value[3]);
	break;
      case ITEM_CONTAINER :
	sprintf(buf, "Max-contains : %d\n\rLocktype : %d\n\rCorpse : %s",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[3]?"Yes":"No");
	break;
      case ITEM_DRINKCON :
	sprinttype(j->obj_flags.value[2],drinks,buf2);
	sprintf(buf, "Max-contains : %d\n\rContains : %d\n\rPoisoned : %d\n\rLiquid : %s",
		j->obj_flags.value[0],
		j->obj_flags.value[1],
		j->obj_flags.value[3],
		buf2);
	break;
      case ITEM_NOTE :
	sprintf(buf, "Tounge : %d",
		j->obj_flags.value[0]);
	break;
      case ITEM_KEY :
	sprintf(buf, "Keytype : %d",
		j->obj_flags.value[0]);
	break;
      case ITEM_FOOD :
	sprintf(buf, "Makes full : %d\n\rPoisoned : %d",
		j->obj_flags.value[0],
		j->obj_flags.value[3]);
	break;
	default :
	  sprintf(buf,"Values 0-3 : [%d] [%d] [%d] [%d]",
		  j->obj_flags.value[0],
		  j->obj_flags.value[1],
		  j->obj_flags.value[2],
		  j->obj_flags.value[3]);
	break;
      }
      send_to_char(buf, ch);
      
      strcpy(buf,"\n\rEquipment Status: ");
      if (!j->carried_by)
	strcat(buf,"NONE");
      else {
	found = FALSE;
	for (i=0;i < MAX_WEAR;i++) {
	  if (j->carried_by->equipment[i] == j) {
	    sprinttype(i,equipment_types,buf2);
	    strcat(buf,buf2);
	    found = TRUE;
	  }
	}
	if (!found)
	  strcat(buf,"Inventory");
      }
      send_to_char(buf, ch);
      
      strcpy(buf, "\n\rSpecial procedure : ");
      if (j->item_number >= 0)
	strcat(buf, (obj_index[j->item_number].func ? "exists\n\r" : "No\n\r"));
      else
	strcat(buf, "No\n\r");
      send_to_char(buf, ch);
      
      strcpy(buf, "Contains :\n\r");
      found = FALSE;
      for(j2=j->contains;j2;j2 = j2->next_content) {
	strcat(buf,fname(j2->name));
	strcat(buf,"\n\r");
	found == TRUE;
      }
      if (!found)
	strcpy(buf,"Contains : Nothing\n\r");
      send_to_char(buf, ch);
      
      send_to_char("Can affect char :\n\r", ch);
      for (i=0;i<MAX_OBJ_AFFECT;i++) {
	sprinttype(j->affected[i].location,apply_types,buf2);
	sprintf(buf,"    Affects : %s By %ld\n\r", buf2,j->affected[i].modifier);
	send_to_char(buf, ch);
      }			
      return;
    } else {
      send_to_char("No mobile or object by that name in the world\n\r", ch);
    }
  }
}

void do_set(struct char_data *ch, char *argument, int cmd)
{
    char field[20], name[20], parmstr[50];
    struct char_data *mob;
    int parm, parm2;
    char buf[256];


    extern char PeacefulWorks;
    extern char EasySummon;

#ifndef TEST_SERVER
    if ((GetMaxLevel(ch) < SILLYLORD) || (IS_NPC(ch)))
       return;
#endif
    argument = one_argument(argument, field);
    argument = one_argument(argument, name);
    argument = one_argument(argument, parmstr);

    if ((mob = get_char_vis(ch, name)) == NULL) {
      send_to_char("I don't see that here \n\r",ch);
      return;
    }

    if (!strcmp(field,"align")) {
      sscanf(parmstr,"%d",&parm);
      GET_ALIGNMENT(mob) = parm; 
    } else if (!strcmp(field, "time")) {
      sscanf(parmstr, "%d", &parm);
      time_info.hours = parm;
      send_to_char("You have changed the time.\n\r",ch);
    } else if (!strcmp(field, "onelife")) {
      SET_BIT(mob->specials.affected_by2, AFF2_ONE_LIFER);
      send_to_char("You now have but one life to live\n", ch);
    } else if (!strcmp(field,"class")) {
      sscanf(parmstr,"%d",&parm);
      /*
      ** this will do almost nothing. (hopefully);
      */
        mob->player.class = parm;
    } else if (!strcmp(field,"exp")) {
      sscanf(parmstr,"%d",&parm);
      GET_EXP(mob) = parm;
    } else if (!strcmp(field, "lev")) {
      parm2 = 0; /* mage */
      sscanf(parmstr,"%d", &parm);
      argument=one_argument(argument, parmstr);
      sscanf(parmstr,"%d", &parm2);
      if (!IS_NPC(mob)) {
         if ((GetMaxLevel(mob) >= GetMaxLevel(ch)) && (ch != mob)) {
	   send_to_char(GET_NAME(ch), mob);
	   send_to_char(" just tried to change your level.\n\r",mob);
	   return;
         } else if(GetMaxLevel(mob) < LOW_IMMORTAL &&
		   GetMaxLevel(ch) <  IMPLEMENTOR && parm2 > 50) {
	   send_to_char("Thou shalt not create new immortals.\n\r", ch);
	 }
       } else {
	 if (parm2 < MAX_CLASS)
	   GET_LEVEL(mob, parm2) = parm;
	 return;
       }

       if (parm < 0) {
	 send_to_char("bug fix. :-)\n\r", ch);
	 return;
       }

       if (parm < GetMaxLevel(ch) || !strcmp(GET_NAME(ch), "Loki")) {
	 if (GetMaxLevel(ch) >= IMPLEMENTOR) {
	   if (parm2 < MAX_CLASS)
	     GET_LEVEL(mob, parm2) = parm;
	 } else {
	   if (parm > DEMIGOD) {
	     send_to_char("Sorry, you can't advance past 54th level\n", ch);
	     return;
	   }
	   if (parm2 < MAX_CLASS)
	     GET_LEVEL(mob, parm2) = parm;
	 }
       }
    } else if (!strcmp(field, "sex")) {
      if (is_number(parmstr)) {
        sscanf(parmstr,"%d",&parm);
        GET_SEX(mob) = parm;
      } else {
	send_to_char("argument must be a number\n\r", ch);
      }
    } else if (!strcmp(field, "race")) {
      if (is_number(parmstr)) {
         sscanf(parmstr,"%d",&parm);
         GET_RACE(mob) = parm;
      } else {
	send_to_char("argument must be a number\n\r", ch);
      }
    } else if (!strcmp(field, "hunger")) {
      sscanf(parmstr,"%d",&parm);
      GET_COND(mob, FULL) = parm;
    } else if (!strcmp(field, "thirst")) {
      sscanf(parmstr,"%d",&parm);
      GET_COND(mob, THIRST) = parm;
    } else if (!strcmp(field, "zone")) {
      sscanf(parmstr,"%d",&parm);
      GET_ZONE(mob) = parm;
    } else if (!strcmp(field, "hit")) {
      sscanf(parmstr,"%d",&parm);
      GET_HIT(mob) = parm;
    } else if (!strcmp(field, "mhit")) {
      sscanf(parmstr,"%d",&parm);
      mob->points.max_hit = parm;
    } else if (!strcmp(field, "tohit")) {
      sscanf(parmstr,"%d", &parm);
      GET_HITROLL(mob)=parm;
    } else if (!strcmp(field, "todam")) {
      sscanf(parmstr,"%d", &parm);
      GET_DAMROLL(mob)=parm;
    } else if (!strcmp(field, "ac")) {
      sscanf(parmstr,"%d", &parm);
      GET_AC(mob)=parm;
    } else if (!strcmp(field, "bank")) {
      sscanf(parmstr, "%d", &parm);
      GET_BANK(mob) = parm;
    } else if (!strcmp(field, "gold")) {
      sscanf(parmstr, "%d", &parm);
      GET_GOLD(mob) = parm;
    } else if (!strcmp(field, "prac")) {
      sscanf(parmstr, "%d", &parm);
      mob->specials.spells_to_learn = parm;
    } else if (!strcmp(field, "age")) {
      sscanf(parmstr, "%d", &parm);
      mob->player.time.birth -= SECS_PER_MUD_YEAR*parm;      

    } else if (!strcmp(field, "str")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.str = parm;
      mob->tmpabilities.str = parm;
    } else if (!strcmp(field, "add")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.str_add = parm;
      mob->tmpabilities.str_add = parm;
    } else if (!strcmp(field, "saves")) {
      parm = 0;
      parm2 = 0;
      sscanf(parmstr, "%d %d", &parm, &parm2);
      mob->specials.apply_saving_throw[parm] = parm2;

    } else if (!strcmp(field, "skills")) {
      parm = 0;
      parm2 = 0;
      sscanf(parmstr, "%d %d", &parm, &parm2);
      if (mob->skills) {
	mob->skills[parm].learned = parm2;
	sprintf(buf, "You just set skill %d to value %d\n\r", parm, parm2);
	send_to_char(buf, ch);
      }

    } else if (!strcmp(field, "stadd")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.str_add = parm;
      mob->tmpabilities.str_add = parm;

    } else if (!strcmp(field, "int")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.intel = parm;
      mob->tmpabilities.intel = parm;

    } else if (!strcmp(field, "wis")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.wis = parm;
      mob->tmpabilities.wis = parm;

    } else if (!strcmp(field, "dex")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.dex = parm;
      mob->tmpabilities.dex = parm;

    } else if (!strcmp(field, "con")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.con = parm;
      mob->tmpabilities.con = parm;

    } else if (!strcmp(field, "chr")) {
      sscanf(parmstr, "%d", &parm);
      mob->abilities.chr = parm;
      mob->tmpabilities.chr = parm;

    } else if (!strcmp(field, "pkill")) {
      if (PeacefulWorks) {
	PeacefulWorks = FALSE;
	EasySummon = FALSE;
	sprintf(buf, "Peaceful rooms and Easy Summon disabled by %s", GET_NAME(ch));
      } else {
	PeacefulWorks = TRUE;
	EasySummon = TRUE;
	sprintf(buf, "Peaceful rooms and Easy Summon enabled by %s", GET_NAME(ch));
      }
      logE(buf);

    } else if (!strcmp(field, "mana")) {
      sscanf(parmstr, "%d", &parm);
      mob->points.mana     = parm;
      mob->points.max_mana = parm;

    } else if (!strcmp(field, "start")) {
      sscanf(parmstr, "%d", &parm);
      mob->specials.start_room = parm;

    } else {
       send_to_char(" Only align allowed \n\r",ch);
       return;
     }
}




void do_shutdow(struct char_data *ch, char *argument, int cmd)
{
  send_to_char("If you want to shut something down - say so!\n\r", ch);
}



void do_shutdown(struct char_data *ch, char *argument, int cmd)
{
  extern int mudshutdown, reboot;
  char buf[100], arg[MAX_INPUT_LENGTH];
  
  if (IS_NPC(ch))
    return;
  
  one_argument(argument, arg);
  
  if (!*arg) {
    sprintf(buf, "Shutdown by %s.", GET_NAME(ch) );
    send_to_all(buf);
    logE(buf);
    mudshutdown = 1;
  } else if (!str_cmp(arg, "reboot")) {
    sprintf(buf, "Reboot by %s.", GET_NAME(ch));
    send_to_all(buf);
    logE(buf);
    mudshutdown = reboot = 1;
  } else
    send_to_char("Go shut down someone your own size.\n\r", ch);
}


void do_snoop(struct char_data *ch, char *argument, int cmd)
{
  static char arg[MAX_STRING_LENGTH];
  struct char_data *victim;
  
  if (!ch->desc)
    return;
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, arg);
  
  if(!*arg)	{
    send_to_char("Snoop who ?\n\r",ch);
    return;
  }
  
  if(!(victim=get_char_vis(ch, arg)))	{
    send_to_char("No such person around.\n\r",ch);
    return;
  }
  
  if(!victim->desc)	{
    send_to_char("There's no link.. nothing to snoop.\n\r",ch);
    return;
  }
  if(victim == ch)	{
    send_to_char("Ok, you just snoop yourself.\n\r",ch);
    if(ch->desc->snoop.snooping) {
      if (ch->desc->snoop.snooping->desc)
	ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
      else {
	char buf[MAX_STRING_LENGTH];
	sprintf(buf, "caught %s snooping %s who didn't have a descriptor!",
		ch->player.name, ch->desc->snoop.snooping->player.name);
	logE(buf);
/*
logically.. this person has returned from being a creature? 
*/
      }
      ch->desc->snoop.snooping = 0;
    }
    return;
  }
  
  if(victim->desc->snoop.snoop_by) {
    send_to_char("Busy already. \n\r",ch);
    return;
  }
  
  if (GetMaxLevel(victim)>=GetMaxLevel(ch))	{
    send_to_char("You failed.\n\r",ch);
    return;
  }
  
  send_to_char("Ok. \n\r",ch);
  
  if(ch->desc->snoop.snooping)
    if (ch->desc->snoop.snooping->desc)
      ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
  
  ch->desc->snoop.snooping = victim;
  victim->desc->snoop.snoop_by = ch;
  return;
}



void do_switch(struct char_data *ch, char *argument, int cmd)
{
  static char arg[80];
  struct char_data *victim;
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, arg);
  
  if (!*arg)	{
    send_to_char("Switch with who?\n\r", ch);
  }	else	{
    if (!(victim = get_char(arg)))
      send_to_char("They aren't here.\n\r", ch);
    else   	{
      if (ch == victim)   {
	send_to_char("He he he... We are jolly funny today, eh?\n\r", ch);
	return;
      }
      
      if (!ch->desc || ch->desc->snoop.snoop_by || ch->desc->snoop.snooping)
	{
	  send_to_char("Mixing snoop & switch is bad for your health.\n\r", ch);
	  return;
	}
      
      if(victim->desc || (!IS_NPC(victim)))       {
	send_to_char(
		     "You can't do that, the body is already in use!\n\r",ch);
      }  else	{
	send_to_char("Ok.\n\r", ch);
	
	ch->desc->character = victim;
	ch->desc->original = ch;
	
	victim->desc = ch->desc;
	ch->desc = 0;
      }
    }
  }
}



void do_return(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *mob, *per;

  void do_snoop(struct char_data *ch, char *argument, int cmd);
  
  if(GetMaxLevel(ch) < LOW_IMMORTAL)
    if(ch->specials.fighting) {
      send_to_char("You are far too busy fighting to return now!\n\r",ch);
      return;
    }
  
  if(!ch->desc)
    return;
  
  if(!ch->desc->original)   { 
    send_to_char("Arglebargle, glop-glyf!?!\n\r", ch);
    return;
  } else {
    
    send_to_char("You pause for a moment and remold your flesh into it's original shape.\n\r",ch);

    if (ch->desc->snoop.snoop_by) {  /* force the snooper to stop */
      do_snoop(ch->desc->snoop.snoop_by, 
	       GET_NAME(ch->desc->snoop.snoop_by), 0);
    }
    
    if (IS_SET(ch->specials.act, ACT_POLYSELF) && cmd) {
      mob = ch;
      per = ch->desc->original;
      
      act("$n turns liquid, and reforms as $N", TRUE, mob, 0, per, TO_ROOM);

      char_from_room(per);
      char_to_room(per,mob->in_room);

      SwitchStuff(mob, per);

    }
    
    ch->desc->character = ch->desc->original;
    ch->desc->original = 0;
    
    ch->desc->character->desc = ch->desc; 
    ch->desc = 0;
    

    if (IS_SET(ch->specials.act, ACT_POLYSELF) && cmd) {
      extract_char(mob);
      WAIT_STATE(ch, PULSE_VIOLENCE);
    }
  }
}


void do_force(struct char_data *ch, char *argument, int cmd)
{
  struct descriptor_data *i;
  struct char_data *vict;
  char name[100], to_force[100],buf[100]; 
  
  if (IS_NPC(ch) && (cmd != 0))
    return;
  
  half_chop(argument, name, to_force);
  
  if (!*name || !*to_force)
    send_to_char("Who do you wish to force to do what?\n\r", ch);
  else if (str_cmp("all", name)) {
    if (!(vict = get_char_vis(ch, name)))
      send_to_char("No-one by that name here..\n\r", ch);
    else {
      if ((GetMaxLevel(ch) <= GetMaxLevel(vict)) && (!IS_NPC(vict)))
	send_to_char("Oh no you don't!!\n\r", ch);
      else {
	sprintf(buf, "$n has forced you to '%s'.", to_force);
	act(buf, FALSE, ch, 0, vict, TO_VICT);
	send_to_char("Ok.\n\r", ch);
	command_interpreter(vict, to_force);
      }
    }
  } else { /* force all */
    for (i = descriptor_list; i; i = i->next)
      if (i->character != ch && !i->connected) {
       	vict = i->character;
       	if ((GetMaxLevel(ch) <= GetMaxLevel(vict)) &&
            (!IS_NPC(vict)))
	  send_to_char("Oh no you don't!!\n\r", ch);
	else {
	  sprintf(buf, "$n has forced you to '%s'.", to_force);
	  act(buf, FALSE, ch, 0, vict, TO_VICT);
	  command_interpreter(vict, to_force);
	}
      }
    send_to_char("Ok.\n\r", ch);
  }
}



void do_load(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *mob;
  struct obj_data *obj;
  char type[100], num[100];
  int number;
  
  extern int top_of_mobt;
  extern int top_of_objt;
  
  if (IS_NPC(ch))
    return;
  
  argument = one_argument(argument, type);
  
  only_argument(argument, num);
  if (isdigit(*num))
    number = atoi(num);
  else
    number = -1;
  
  if (is_abbrev(type, "mobile"))	{
    if (number<0) {
      for (number = 0; number<=top_of_mobt; number++)
	if (isname(num, mob_index[number].name))
	  break;
      if (number>top_of_mobt)
	number = -1;
    } else {
      number = real_mobile(number);
    }
    if ( number<0 || number>top_of_mobt) {
      send_to_char("There is no such monster.\n\r", ch);
      return;
    }
    mob = read_mobile(number, REAL);
    char_to_room(mob, ch->in_room);
    
    act("$n makes a quaint, magical gesture with one hand.", TRUE, ch,
	0, 0, TO_ROOM);
    act("$n has summoned $N from the ether!", FALSE, ch, 0, mob, TO_ROOM);
    act("You bring forth $N from the the cosmic ether.", FALSE, ch, 0, mob, TO_CHAR);
    
  } else if (is_abbrev(type, "object"))	{
    if (number<0) {
      for (number = 0; number<=top_of_objt; number++)
	if (isname(num, obj_index[number].name))
	  break;
      if (number>top_of_objt)
	number = -1;
    } else {
      number = real_object(number);
    }
    if ( number<0 || number>top_of_objt) {
      send_to_char("There is no such object.\n\r", ch);
      return;
    }
    
    if (GetMaxLevel(ch)<IMPLEMENTOR) {
      switch(obj_index[number].virtual) {
      case 5021:
	send_to_char("no.  No more bows!  And don't kill the worm either!\n\r",ch);
	return;
      case 5112:
	send_to_char("no, no more Ruby rings!  And don't kill for it either!\n\r",ch);
	return;
      case 233:
      case 21150:
      case 30012:
      case 30013:
      case 30014:
      case 30015:
      case 30016:
      case 30017:
      case 30018:
	send_to_char("When DM\'s fly out of Ripper\'s butt.\n\r", ch);
	return;
      case 24: case 25: case 26: case 27: case 28:
        send_to_char("Sorry, private items.\n\r", ch);
        return;
      case 21113: case 21117: case 21120: case 21121: case 21122:
	send_to_char("You can't load this item, sorry.\n\r", ch);
	return;

      }
    }
    
    obj = read_object(number, REAL);
    obj_to_char(obj, ch);
    act("$n gyrates $s hips wildly.", TRUE, ch, 0, 0, TO_ROOM);
    act("$n has created $p!", FALSE, ch, obj, 0, TO_ROOM);
    act("You now have $p.", FALSE, ch, obj, 0, TO_CHAR);
  } else if (is_abbrev(type, "room")) {
    int	start, end;
    
    if (GetMaxLevel(ch) < CREATOR)
      return;
    
    switch (sscanf(num, "%d %d", &start, &end)) {
    case 2: /* we got both numbers */
      RoomLoad(ch, start, end);
      break;
    case 1: /* we only got one, load it */
      RoomLoad(ch, start, start);
      break;
    default:
      send_to_char("Load? Fine!  Load we must, But what?\n\r", ch);
      break;
    }
  } else {
    send_to_char("Usage: load (object|mobile) (number|name)\n\r"
		 "       load room start [end]\n\r", ch);
  }
}



void purge_one_room(int rnum, struct room_data *rp, int *range)
{
  struct char_data	*ch;
  struct obj_data	*obj;
  extern long room_count;

  if (rnum==0 ||		/* purge the void?  I think not */
      rnum < range[0] || rnum > range[1])
    return;

  while (rp->people) {
    ch = rp->people;
    send_to_char("A god strikes the heavens making the ground around you erupt into a", ch);
    send_to_char("fluid fountain boiling into the ether.  All that's left is the Void.", ch);
    char_from_room(ch);
    char_to_room(ch, 0);	/* send character to the void */
    do_look(ch, "", 15);
    act("$n tumbles into the Void.", TRUE, ch, 0, 0, TO_ROOM);
  }

  while (rp->contents) {
    obj = rp->contents;
    obj_from_room(obj);
    obj_to_room(obj, 0);	/* send item to the void */
  }

  completely_cleanout_room(rp); /* clear out the pointers */
#if HASH  
  hash_remove(&room_db, rnum);	/* remove it from the database */
#else
  room_remove(room_db, rnum);
#endif
  room_count--;

}


/* clean a room of all mobiles and objects */
void do_purge(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict, *next_v;
  struct obj_data *obj, *next_o;
  
  char name[100];
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, name);
  
  if (*name) {  /* argument supplied. destroy single object or char */
    if (strcmp(name, "links")==0 && GetMaxLevel(ch)>= IMPLEMENTOR) {

      struct descriptor_data *d;

      for (d = descriptor_list; d; d = d->next){
	close_socket(d);
      }
      return;
    }
    if (vict = get_char_room_vis(ch, name))	{
      if ((!IS_NPC(vict) || IS_SET(vict->specials.act, ACT_POLYSELF)) && 
	  (GetMaxLevel(ch)<IMPLEMENTOR)) {
	send_to_char("I'm sorry...  I can't let you do that.\n\r", ch);
	return;
      }
	
	act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);
	
	if (IS_NPC(vict)) {
	  extract_char(vict);
	} else {
	  if (vict->desc)	{
	    close_socket(vict->desc);
	    vict->desc = 0;
	    extract_char(vict);
	  } else {
	    extract_char(vict);
	  }
	}
      } else if (obj = get_obj_in_list_vis
		         (ch, name, real_roomp(ch->in_room)->contents)) {
	act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
	extract_obj(obj);
      } else	{
	argument = one_argument(argument, name);
	if (0==str_cmp("room", name)) {
	  int	range[2];
	  register int i;
          struct room_data *rp;
	  if (GetMaxLevel(ch)<IMPLEMENTOR) {
	    send_to_char("I'm sorry, I can't let you do that.\n\r", ch);
	    if (strcmp(GET_NAME(ch), "Haplo")==0) {
	      send_to_char("(Fuck you Phil) :-)\n\r", ch);	      
	    }
	    return;
	  }
	  argument = one_argument(argument,name);
	  if (!isdigit(*name)) {
	    send_to_char("purge room start [end]",ch);
	    return;
	  }
	  range[0] = atoi(name);
	  argument = one_argument(argument,name);
	  if (isdigit(*name))
	    range[1] = atoi(name);
	  else
	    range[1] = range[0];
	  
	  if (range[0]==0 || range[1]==0) {
	    send_to_char("usage: purge room start [end]\n\r", ch);
	    return;
	  }
#if HASH
	  hash_iterate(&room_db, purge_one_room, range);
#else
	  if (range[0]>=WORLD_SIZE || range[1]>=WORLD_SIZE) {
	    send_to_char("only purging to WORLD_SIZE\n\r", ch);
	    return;
	  }
	  for (i=range[0];i<=range[1];i++) {
	    if ((rp = real_roomp(i)) != 0) {
	      purge_one_room(i, rp, range);
	    }
	  }
#endif
	} else {
	  send_to_char("I don't see that here.\n\r", ch);
	  return;
	}
      }
      
      send_to_char("Ok.\n\r", ch);
    }  else {   /* no argument. clean out the room */
      if (GetMaxLevel(ch) < DEMIGOD)
	return;
      if (IS_NPC(ch))	{
	send_to_char("You would only kill yourself..\n\r", ch);
	return;
      }
      
      act("$n gestures... You are surrounded by thousands of tiny scrubbing bubbles!", 
	  FALSE, ch, 0, 0, TO_ROOM);
      send_to_room("The world seems a little cleaner.\n\r", ch->in_room);

      for (vict = real_roomp(ch->in_room)->people; vict; vict = next_v) {
	next_v = vict->next_in_room;
	if (IS_NPC(vict) && (!IS_SET(vict->specials.act, ACT_POLYSELF)))
	  extract_char(vict);
      }
      
      for (obj = real_roomp(ch->in_room)->contents; obj; obj = next_o) {
	next_o = obj->next_content;
	extract_obj(obj);
      }
    }
}



/* Give pointers to the five abilities */
void roll_abilities(struct char_data *ch)
{
  int i, j, k, temp;
  float avg;
  ubyte table[MAX_STAT];
  ubyte rools[4];
  
  for(i=0; i<MAX_STAT; table[i++]=0)  ;
  
  do {
    for(i=0; i<MAX_STAT; i++) {
      
      for(j=0; j<4; j++)
	rools[j] = number(1,6);
    
      temp = (unsigned int)rools[0]+(unsigned int)rools[1]+(unsigned int)rools[2]+(unsigned int)rools[3] -
	MIN((int)rools[0], MIN((int)rools[1], MIN((int)rools[2],(int)rools[3])));
      
      for(k=0; k<MAX_STAT; k++)
	if (table[k] < temp)
	  SWITCH(temp, table[k]);
    }
    for(j=0,avg=0;j<MAX_STAT;j++)
      avg += table[j];
    avg /= j;

  } while(avg<12.0);

  for (i=0;i<MAX_STAT;i++) {
    switch(ch->desc->stat[i]) {
    case 's' :
      ch->abilities.str = table[i];
      break;
    case 'i':
      ch->abilities.intel = table[i];
      break;
    case 'd':
      ch->abilities.dex = table[i];
      break;
    case 'w':
      ch->abilities.wis = table[i];
      break;
    case 'o':
      ch->abilities.con = table[i];
      break;
    case 'h':
      ch->abilities.chr  = table[i];
      break;
    default:
      break;
    }
  }
  
  if (ch->abilities.str == 0)
    ch->abilities.str = 11;
  if (ch->abilities.intel == 0)
    ch->abilities.intel = 11;
  if (ch->abilities.dex == 0)
    ch->abilities.dex = 11;
  if (ch->abilities.wis == 0)
    ch->abilities.wis = 11;
  if (ch->abilities.con == 0)
    ch->abilities.con = 11;
  if (ch->abilities.chr == 0)
    ch->abilities.chr = 11;

  
  ch->abilities.str_add = 0;

  if (GetMaxLevel(ch) < 2) {
    ch->points.max_hit = HowManyClasses(ch)*10;

    if (HasClass(ch, CLASS_MAGIC_USER)) {
      ch->points.max_hit += number(1,4);
    }
    if (HasClass(ch, CLASS_CLERIC)) {
      ch->points.max_hit += number(1,8);
    }
    if (HasClass(ch, CLASS_WARRIOR)) {
      ch->points.max_hit += number(1,10);
      if (ch->abilities.str == 18)
	ch->abilities.str_add = number(0,100);
    }
    if (HasClass(ch, CLASS_THIEF)) {
      ch->points.max_hit += number(1,6);
    }
    if (HasClass(ch, CLASS_MONK)) {
      ch->points.max_hit += number(1,6); 
    }
    if (HasClass(ch, CLASS_DRUID)) {
      ch->points.max_hit += number(1,8);
    }
    
    ch->points.max_hit /= HowManyClasses(ch);
  }
 
  if(GET_RACE(ch) == RACE_HUMANTWO) {
    ImprovePreferedStat(ch, 1, FALSE);
  } else if (GET_RACE(ch) == RACE_HALFELF && number(0,1)) {
    ImprovePreferedStat(ch, 1, FALSE);
  } else if (GET_RACE(ch) == RACE_ELVEN) {
    ch->abilities.dex++;
    ch->abilities.con--;
  } else if (GET_RACE(ch) == RACE_DWARF) {
    ch->abilities.con++;
    ch->abilities.chr--;
  } else if (GET_RACE(ch) == RACE_GNOME) {
    ch->abilities.wis++;
    ch->abilities.dex--;
  } else if (GET_RACE(ch) == RACE_HALFLING) {
    ch->abilities.dex++;
    ch->abilities.str--;
  } else if (GET_RACE(ch) == RACE_HALFORC) {
    ch->abilities.str++;
    ch->abilities.con++;
    ch->abilities.chr--;
    ch->abilities.chr--;
  } else if (GET_RACE(ch) == RACE_MFLAYER) {
    ch->abilities.str--;
    ch->abilities.intel++;
    ch->abilities.intel++;
    ch->abilities.chr--;
    ch->abilities.chr--;
  } else if (GET_RACE(ch) == RACE_DROW) {
    ch->abilities.chr--;
    ch->abilities.chr--;
    ch->abilities.dex++;
    ch->abilities.con--;
  } else if (GET_RACE(ch) == RACE_SKEXIE) {
    ch->abilities.str--;
    ch->abilities.dex++;
    ch->abilities.con--;
  } else if (GET_RACE(ch) == RACE_DRAAGDIM) {
    ch->abilities.str--;
    ch->abilities.con--;
    ch->abilities.intel++;
    ch->abilities.dex++;
  } else if (GET_RACE(ch) == RACE_VAMPIRE) {
    ch->abilities.intel++;
  } else if (GET_RACE(ch) == RACE_OGRE) {
    ch->abilities.str++;
    ch->abilities.str++;
    ch->abilities.intel--;
    ch->abilities.chr--;
    ch->abilities.chr--;
    ch->abilities.con++;
    ch->abilities.wis--;
    ch->abilities.dex--;
  } else if (GET_RACE(ch) == RACE_FAERIE) {
    ch->abilities.str--;
    ch->abilities.con--;
    ch->abilities.intel++;
    ch->abilities.dex++;
  } else if (GET_RACE(ch) == RACE_VEGMAN) {
    ch->abilities.dex--;
  }


  if (HasClass(ch, CLASS_WARRIOR)) {
    if (ch->abilities.str >= 18) {
      ch->abilities.str = 18;
      if(ch->abilities.str_add == 0)
	ch->abilities.str_add = number(0,100);
    }
  } else {
    if(ch->abilities.str > 18) 
      ch->abilities.str = 18;
  }
  
  /* ensure racial mods are in line. */
  ch->abilities.dex = MAX(3,MIN(18,ch->abilities.dex));
  ch->abilities.con = MAX(3,MIN(18,ch->abilities.con));
  ch->abilities.intel = MAX(3,MIN(18,ch->abilities.intel));
  ch->abilities.wis = MAX(3,MIN(18,ch->abilities.wis));
  ch->abilities.chr = MAX(3,MIN(18,ch->abilities.chr));

  ch->tmpabilities = ch->abilities;
}



void do_start(struct char_data *ch)
{
  int r_num;
  struct obj_data *obj;
  
  extern struct dex_skill_type dex_app_skill[];
  
  send_to_char("Welcome to SillyMud.  Enjoy the game...\n\r",ch);
  ch->specials.start_room = NOWHERE;
  
  StartLevels(ch);
  
  GET_EXP(ch) = 1;
  
  set_title(ch);
  
  roll_abilities(ch);
  /*
   *  This is the old style of determining hit points.  I modified it so that
   *  characters get the standard AD&D + 10 hp to start.
   *	ch->points.max_hit  = 10;  
   */
  
  /* 
    outfit char with valueless items
    */
  if ((r_num = real_object(12)) >= 0) {
    obj = read_object(r_num, REAL);
    obj_to_char(obj,ch);   /* bread   */ 
    obj = read_object(r_num, REAL);
    obj_to_char(obj,ch);   /* bread   */
  }
  
  if ((r_num = real_object(13)) >= 0) {
    obj = read_object(r_num, REAL);
    obj_to_char(obj,ch);   /* water   */
    
    obj = read_object(r_num, REAL);
    obj_to_char(obj,ch);   /* water   */
  }

  if (HasClass(ch, CLASS_CLERIC) || HasClass(ch, CLASS_MAGIC_USER)) {
    ch->skills[SKILL_READ_MAGIC].learned = 95;
  }
  
  if (OnlyClass(ch, CLASS_THIEF))
    ch->skills[SKILL_CONS_PEOPLE].learned = 95;
  
  if (IS_SET(ch->player.class, CLASS_THIEF)) {
    if (GET_RACE(ch) == RACE_HUMAN) {
      ch->skills[SKILL_SNEAK].learned = 10;
      ch->skills[SKILL_HIDE].learned =  5;
      ch->skills[SKILL_STEAL].learned = 15;
      ch->skills[SKILL_BACKSTAB].learned = 10;
      ch->skills[SKILL_PICK_LOCK].learned = 10;
    } else if (GET_RACE(ch) == RACE_ELVEN) {
      ch->skills[SKILL_SNEAK].learned = 10;
      ch->skills[SKILL_HIDE].learned =  5;
      ch->skills[SKILL_STEAL].learned = 15;
      ch->skills[SKILL_BACKSTAB].learned = 10;
      ch->skills[SKILL_PICK_LOCK].learned = 10;
    } else if (GET_RACE(ch) == RACE_DWARF) {
      ch->skills[SKILL_SNEAK].learned = 10;
      ch->skills[SKILL_HIDE].learned =  5;
      ch->skills[SKILL_STEAL].learned = 15;
      ch->skills[SKILL_BACKSTAB].learned = 10;
      ch->skills[SKILL_PICK_LOCK].learned = 10;
    } else if (GET_RACE(ch) == RACE_HALFLING) {
      ch->skills[SKILL_SNEAK].learned = 10;
      ch->skills[SKILL_HIDE].learned =  5;
      ch->skills[SKILL_STEAL].learned = 15;
      ch->skills[SKILL_BACKSTAB].learned = 10;
      ch->skills[SKILL_PICK_LOCK].learned = 10;
    } else if (GET_RACE(ch) == RACE_GNOME) {
      ch->skills[SKILL_SNEAK].learned = 10;
      ch->skills[SKILL_HIDE].learned =  5;
      ch->skills[SKILL_STEAL].learned = 15;
      ch->skills[SKILL_BACKSTAB].learned = 10;
      ch->skills[SKILL_PICK_LOCK].learned = 10;
    }
  }
  
  GET_HIT(ch) = GET_MAX_HIT(ch);
  GET_MANA(ch) = GET_MAX_MANA(ch);
  /*   ch->points.max_move += GET_CON(ch) + number(1,20) - 9; */
  GET_MOVE(ch) = GET_MAX_MOVE(ch);
  
  GET_COND(ch,THIRST) = 24;
  GET_COND(ch,FULL) = 24;
  GET_COND(ch,DRUNK) = 0;
  
  
  ch->player.time.played = 0;
  ch->player.time.logon = time(0);
  
  /* 33% chance for infravision */
  if(GET_RACE(ch) == RACE_HALFLING || GET_RACE(ch) == RACE_HALFELF ||
     GET_RACE(ch) == RACE_HALFORC)
    if(!number(0,2))
      SET_BIT(ch->specials.affected_by, AFF_INFRAVISION);

      

}


void do_advance(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[100], level[100], class[100];
	int adv, newlevel, lin_class;

	void gain_exp(struct char_data *ch, int gain);

	if (IS_NPC(ch))
		return;

	argument = one_argument(argument, name);

	if (*name)	{
		if (!(victim = get_char_room_vis(ch, name)))		{
			send_to_char("That player is not here.\n\r", ch);
			return;
		}
	} else {
		send_to_char("Advance who?\n\r", ch);
		return;
	}

	if (IS_NPC(victim)) {
		send_to_char("NO! Not on NPC's.\n\r", ch);
		return;
	}

	argument = one_argument(argument, class);

	if (!*class) {
	  send_to_char("Supply a class: M C W T\n\r",ch);
	  return;
	}

	switch(*class) {
	case 'M':
	case 'm':
	  lin_class = MAGE_LEVEL_IND;
	  break;

	case 'T':
	case 't':
	  lin_class = THIEF_LEVEL_IND;
	  break;
	  
	case 'W':
	case 'w':
	case 'F':
	case 'f':
	  lin_class = WARRIOR_LEVEL_IND;
	  break;
	  
	case 'C':
	case 'c':
	case 'P':
	case 'p':
	  lin_class = CLERIC_LEVEL_IND;
	  break;
#if 0
	case 'D':
	case 'd':
	  lin_class = DRUID_LEVEL_IND;
	  break;

	case 'K' :
	case 'k' :
	  lin_class = MONK_LEVEL_IND;
	  break;

#endif
	default:
#if 0
	  send_to_char("Must supply a class (M C W T D K B)\n\r", ch);
#endif
	  send_to_char("Must supply a class (M C W T)\n\r", ch);
	  return;
	  break;

	}
	
	argument = one_argument(argument, level);

	if (GET_LEVEL(victim, lin_class) == 0)
		adv = 1;
	else if (!*level) {
		send_to_char("You must supply a level number.\n\r", ch);
		return;
	} else {
		if (!isdigit(*level))		{
			send_to_char("Third argument must be a positive integer.\n\r",ch);
			return;
		}
		if ((newlevel = atoi(level)) < GET_LEVEL(victim, lin_class)) {
		  send_to_char("Can't diminish a player's status (yet).\n\r", ch);
		  return;
		}
		adv = newlevel - GET_LEVEL(victim,lin_class);
	}

	if (((adv + GET_LEVEL(victim, lin_class)) > 1) && (GetMaxLevel(ch) < IMPLEMENTOR)){
		send_to_char("Thou art not godly enough.\n\r", ch);
		return;
	}

	if ((adv + GET_LEVEL(victim, lin_class)) > IMPLEMENTOR)	{
		send_to_char("Implementor is the highest possible level.\n\r", ch);
		return;
	}

	if (((adv + GET_LEVEL(victim, lin_class)) < 1)&&
	    ((adv + GET_LEVEL(victim, lin_class)) != 1))
	{
		send_to_char("1 is the lowest possible level.\n\r", ch);
		return;
	}

	send_to_char("You feel generous.\n\r", ch);
  act("$n makes some strange gestures.\n\rA strange feeling comes upon you,"
      "\n\rLike a giant hand, light comes down from\n\rabove, grabbing your "
      "body, that begins\n\rto pulse with coloured lights from inside.\n\rYo"
      "ur head seems to be filled with daemons\n\rfrom another plane as your"
      " body dissolves\n\rinto the elements of time and space itself.\n\rSudde"
      "nly a silent explosion of light snaps\n\ryou back to reality. You fee"
      "l slightly\n\rdifferent.",FALSE,ch,0,victim,TO_VICT);
	
	
	if (GET_LEVEL(victim, lin_class) == 0) {
	  do_start(victim);
	} else {
	  if (GET_LEVEL(victim, lin_class) < IMPLEMENTOR) {

	    gain_exp_regardless(victim, 
				(titles[lin_class]
				[GET_LEVEL(victim, lin_class)+adv].exp)
				-GET_EXP(victim), lin_class);

	    send_to_char("Character is now advanced.\n\r", ch);
	  } else {
	    send_to_char("Some idiot just tried to advance your level.\n\r", victim);
	    send_to_char("IMPOSSIBLE! IDIOTIC!\n\r", ch);
	  }
	}
}


void do_reroll(struct char_data *ch, char *argument, int cmd)
{

  char buf[100];
  
  send_to_char("Use @set str..charisma..\n\r", ch);  

}


void do_restore(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *victim;
  char buf[100];
  int i;
  
  void update_pos( struct char_data *victim );

  if (cmd == 0) return;
  
  only_argument(argument,buf);
  if (!*buf)
    send_to_char("Who do you wish to restore?\n\r",ch);
  else
    if(!(victim = get_char(buf)))
      send_to_char("No-one by that name in the world.\n\r",ch);
    else {
      GET_MANA(victim) = GET_MAX_MANA(victim);
      GET_HIT(victim) = GET_MAX_HIT(victim);
      GET_MOVE(victim) = GET_MAX_MOVE(victim);
      
      if (IS_NPC(victim))
	return;
      
      if (GetMaxLevel(victim) < LOW_IMMORTAL) {
	GET_COND(victim,THIRST) = 24;
	GET_COND(victim,FULL) = 24;
      } else {
	GET_COND(victim,THIRST) = -1;
	GET_COND(victim,FULL) = -1;
      }
      
      if (GetMaxLevel(victim) >= CREATOR) {
	for (i = 0; i < MAX_SKILLS; i++) {
	  victim->skills[i].learned = 100;
	  victim->skills[i].flags   = 1;
	}
	
	if (GetMaxLevel(victim) >= GOD) {
	  victim->abilities.str_add = 100;
	  victim->abilities.intel = 25;
	  victim->abilities.wis = 25;
	  victim->abilities.dex = 25;
	  victim->abilities.str = 25;
	  victim->abilities.con = 25;
	  victim->abilities.chr = 25;
	}

	victim->tmpabilities = victim->abilities;
	
      }
      update_pos( victim );
      send_to_char("Done.\n\r", ch);
      act("You have been fully healed by $N!", FALSE, victim, 0, ch, TO_CHAR);
    }
}




void do_noshout(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict;
  struct obj_data *dummy;
  char buf[MAX_INPUT_LENGTH];
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, buf);
  
  if (!*buf)
    if (IS_SET(ch->specials.act, PLR_NOSHOUT))		{
      send_to_char("You can now hear shouts again.\n\r", ch);
      REMOVE_BIT(ch->specials.act, PLR_NOSHOUT);
    }		else		{
      send_to_char("From now on, you won't hear shouts.\n\r", ch);
      SET_BIT(ch->specials.act, PLR_NOSHOUT);
    }
  else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
    send_to_char("Couldn't find any such creature.\n\r", ch);
  else if (IS_NPC(vict))
    send_to_char("Can't do that to a beast.\n\r", ch);
  else if (GetMaxLevel(vict) >= GetMaxLevel(ch))
    act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
  else if (IS_SET(vict->specials.act, PLR_NOSHOUT) &&
	   (GetMaxLevel(ch) >= SAINT))	{
    send_to_char("You can shout again.\n\r", vict);
    send_to_char("NOSHOUT removed.\n\r", ch);
    REMOVE_BIT(vict->specials.act, PLR_NOSHOUT);
  }	else if (GetMaxLevel(ch) >= SAINT)	{
    send_to_char("The gods take away your ability to shout!\n\r", vict);
    send_to_char("NOSHOUT set.\n\r", ch);
    SET_BIT(vict->specials.act, PLR_NOSHOUT);
  } else {
    send_to_char("Sorry, you can't do that\n\r",ch);
  }
}


void do_nohassle(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict;
  struct obj_data *dummy;
  char buf[MAX_INPUT_LENGTH];
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, buf);
  
  if (!*buf)
    if (IS_SET(ch->specials.act, PLR_NOHASSLE))      {
	send_to_char("You can now be hassled again.\n\r", ch);
	REMOVE_BIT(ch->specials.act, PLR_NOHASSLE);
      }    else      {
	send_to_char("From now on, you won't be hassled.\n\r", ch);
	SET_BIT(ch->specials.act, PLR_NOHASSLE);
      }
  else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
    send_to_char("Couldn't find any such creature.\n\r", ch);
  else if (IS_NPC(vict))
    send_to_char("Can't do that to a beast.\n\r", ch);
  else if (GetMaxLevel(vict) > GetMaxLevel(ch))
    act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
  else
    send_to_char("The implementor won't let you set this on mortals...\n\r",ch);
  
}


void do_stealth(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict;
  struct obj_data *dummy;
  char buf[MAX_INPUT_LENGTH];
  
  if (IS_NPC(ch))
    return;
  
  only_argument(argument, buf);
  
  if (!*buf)
    if (IS_SET(ch->specials.act, PLR_STEALTH))      {
	send_to_char("STEALTH mode OFF.\n\r", ch);
	REMOVE_BIT(ch->specials.act, PLR_STEALTH);
      }    else      {
	send_to_char("STEALTH mode ON.\n\r", ch);
	SET_BIT(ch->specials.act, PLR_STEALTH);
      }
  else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
    send_to_char("Couldn't find any such creature.\n\r", ch);
  else if (IS_NPC(vict))
    send_to_char("Can't do that to a beast.\n\r", ch);
  else if (GetMaxLevel(vict) > GetMaxLevel(ch))
    act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
  else
    send_to_char("The implementor won't let you set this on mortals...\n\r",ch);

}

void print_room(int rnum, struct room_data *rp, struct string_block *sb)
{
  char	buf[MAX_STRING_LENGTH];
  int	dink,bits, scan;

  extern char *sector_types[];

  if ((rp->sector_type < 0) || (rp->sector_type > 9)) { /* non-optimal */
    rp->sector_type = 0;
  }
  sprintf(buf, "%5d %4d %-12s %s", rp->number, rnum,
	  sector_types[rp->sector_type], (rp->name?rp->name:"Empty"));
  strcat(buf, " [");

  dink=0;
  for (bits=rp->room_flags, scan=0; bits; scan++) {
    if (bits & (1<<scan)) {
      if (dink)
	strcat(buf, " ");
      strcat(buf, room_bits[scan]);
      dink=1;
      bits ^= (1<<scan);
    }
  }
  strcat(buf, "]\n\r");

  append_to_string_block(sb, buf);
}

void print_death_room(int rnum, struct room_data *rp, struct string_block *sb)
{
  if (rp && rp->room_flags&DEATH)
    print_room(rnum, rp, sb);
}

void print_private_room(int rnum, struct room_data *rp, struct string_block *sb)
{
  if (rp && rp->room_flags&PRIVATE)
    print_room(rnum, rp, sb);
}

struct show_room_zone_struct {
  int	blank;
  int	startblank, lastblank;
  int	bottom, top;
  struct string_block	*sb;
};

void show_room_zone(int rnum, struct room_data *rp,
			   struct show_room_zone_struct *srzs)
{
  char buf[MAX_STRING_LENGTH];

  if (!rp || rp->number<srzs->bottom || rp->number>srzs->top)
    return; /* optimize later*/
  
  if (srzs->blank && (srzs->lastblank+1 != rp->number) ) {
    sprintf(buf, "rooms %d-%d are blank\n\r", srzs->startblank, srzs->lastblank);
    append_to_string_block(srzs->sb, buf);
    srzs->blank = 0;
  }
  if (1==sscanf(rp->name, "%d", &srzs->lastblank) &&
      srzs->lastblank==rp->number) {
    if (!srzs->blank) {
      srzs->startblank = srzs->lastblank;
      srzs->blank = 1;
    }
    return;
  } else if (srzs->blank) {
    sprintf(buf, "rooms %d-%d are blank\n\r", srzs->startblank, srzs->lastblank);
    append_to_string_block(srzs->sb, buf);
    srzs->blank = 0;
  }
  
  print_room(rnum, rp, srzs->sb);
}

void do_show(struct char_data *ch, char *argument, int cmd)
{
  int	zone;
  char buf[MAX_STRING_LENGTH], zonenum[MAX_INPUT_LENGTH];
  struct index_data	*which_i;
  int	bottom, top, topi;
  struct string_block	sb;
  
  if (IS_NPC(ch))
    return;
  
  argument = one_argument(argument, buf);
  
  init_string_block(&sb);

  if (is_abbrev(buf, "zones")) {
    struct zone_data	*zd;
    int	bottom=0;
    append_to_string_block(&sb, "# Zone   name                                lifespan age     rooms     reset\n\r");

    for(zone=0; zone<=top_of_zone_table; zone++) {
      char	*mode;

      zd = zone_table+zone;
      switch(zd->reset_mode) {
      case 0: mode = "never";      break;
      case 1: mode = "ifempty";    break;
      case 2: mode = "always";     break;
      default:
	if (zd->reset_mode > 2) {
	  if (IS_SET(zd->reset_mode, ZONE_ALWAYS))
	    mode = "#always";
	  else if (IS_SET(zd->reset_mode, ZONE_EMPTY))
	    mode = "#empty";
	  else
	    mode = "#never";
	  
	} else {
	  mode = "!unknown!";
	}
      }
      sprintf(buf,"%4d %-40s %4dm %4dm %6d-%-6d %s\n\r", zone, zd->name,
	      zd->lifespan, zd->age, bottom, zd->top, mode);
      append_to_string_block(&sb, buf);
      bottom = zd->top+1;
    }
    
    
  } else if (is_abbrev(buf, "objects") &&
	     (which_i=obj_index,topi=top_of_objt) ||
	     is_abbrev(buf, "mobiles") &&
	     (which_i=mob_index,topi=top_of_mobt) ) {
    int		objn;
    struct index_data	*oi;
    
    only_argument(argument, zonenum);
    zone = -1;
    if (1==sscanf(zonenum,"%i", &zone) &&
	( zone<0 || zone>top_of_zone_table )) {
      append_to_string_block(&sb, "That is not a valid zone_number\n\r");
      return;
    }
    if (zone>=0) {
      bottom = zone ? (zone_table[zone-1].top+1) : 0;
      top = zone_table[zone].top;
    }
    
    append_to_string_block(&sb, "VNUM  rnum count names\n\r");
    for (objn=0; objn<=topi; objn++) {
      oi = which_i + objn;
      
      if (zone>=0 && (oi->virtual<bottom || oi->virtual>top) ||
	  zone<0 && !isname(zonenum, oi->name))
	continue; /* optimize later*/
      
      sprintf(buf,"%5d %4d %3d  %s\n\r", oi->virtual, objn,
	      oi->number, oi->name);
      append_to_string_block(&sb, buf);
    }
    
    
  } else if (is_abbrev(buf, "rooms")) {
    
    only_argument(argument, zonenum);
    
    append_to_string_block(&sb, "VNUM  rnum type         name [BITS]\n\r");
    if (is_abbrev(zonenum, "death")) {
#if HASH
      hash_iterate(&room_db, print_death_room, &sb);
#else
      room_iterate(room_db, print_death_room, &sb);
#endif
      
    } else if (is_abbrev(zonenum, "private")) {
#if HASH
      hash_iterate(&room_db, print_private_room, &sb);
#else
      room_iterate(room_db, print_private_room, &sb);
#endif
      
    } else if (1!=sscanf(zonenum,"%i", &zone) ||
	       zone<0 || zone>top_of_zone_table) {
      append_to_string_block(&sb, "I need a zone number with this command\n\r");
      
      
    } else {
      struct show_room_zone_struct	srzs;
      
      srzs.bottom = zone ? (zone_table[zone-1].top+1) : 0;
      srzs.top = zone_table[zone].top;
      
      srzs.blank = 0;
      srzs.sb = &sb;
#if HASH
      hash_iterate(&room_db, show_room_zone, &srzs);
#else
      room_iterate(room_db, show_room_zone, &srzs);
#endif
	
      if (srzs.blank){
	sprintf(buf, "rooms %d-%d are blank\n\r", srzs.startblank,
		srzs.lastblank);
	append_to_string_block(&sb, buf);
	srzs.blank = 0;
	}
	  }

  } else if (is_abbrev(buf, "top")) {
    
    int objn,top_ten[10],i,insert,tmp,bot;
    struct index_data *oi,*oi2;
    char buf[80];

    which_i=obj_index;
    topi=top_of_objt;
    bot=0;
    
    for(i=0;i<10;i++)
      top_ten[i]=0;

    append_to_string_block(&sb,"VNUM  rnum count names\n\r");

    for(objn=0; objn<= topi; objn++) {
      oi=which_i + objn;
      if(oi->number > bot) {
        for(i=0;i<10;i++) {
          oi2=which_i + top_ten[i];
          if(oi->number > oi2->number) {
            insert=objn;
            while(i<10) {
              tmp=top_ten[i];
              top_ten[i]=insert;
	      insert=tmp;
              if(insert==0)
                break;
              i++;
            }
            bot=top_ten[9];
            if(bot < 0)
              bot=0;
            break;
          }
        }
      }
    }
    for(i=0;i<10;i++) {
      oi = which_i + top_ten[i];
      sprintf(buf,"%5d %4d %3d  %s\n\r",
              oi->virtual, objn,oi->number, oi->name);
      append_to_string_block(&sb, buf);
    }
  } else {
    append_to_string_block(&sb,"Usage:\n\r"
		 "  show zones\n\r"
		 "  show (objects|mobiles) (zone#|name)\n\r"
		 "  show rooms (zone#|death|private)\n\r");
  }
  page_string_block(&sb,ch);
  destroy_string_block(&sb);
}


void do_debug(struct char_data *ch, char *argument, int cmd)
{
  char	arg[MAX_INPUT_LENGTH];
  int	i;

  i=0;
  one_argument(argument, arg);
  i = atoi(arg);

  if (i<0 || i>2) {
    send_to_char("valid values are 0, 1 and 2\n\r", ch);
  } else {
    sprintf(arg, "Debug level set to %d. May not be implemented\n\r", i);
    send_to_char(arg, ch);
  }
}

void do_invis(struct char_data *ch, char *argument, int cmd)
{
  char	buf[MAX_INPUT_LENGTH];
  int	level;

  if (cmd == 242 && !IS_IMMORTAL(ch)) return;

  if (cmd != 242) {
    if (affected_by_spell(ch, SPELL_INVISIBLE)) {
      affect_from_char(ch, SPELL_INVISIBLE);
    }
    REMOVE_BIT(ch->specials.affected_by, AFF_INVISIBLE);
    ch->invis_level = 0;
    return;
  }
  
  if (scan_number(argument, &level)) {
    if (level<0) level=0;
    else if (level>LOW_IMMORTAL) level = LOW_IMMORTAL;
    ch->invis_level = level;
    sprintf(buf,"Invis level set to %d.\n\r", level);
    send_to_char(buf, ch);
  } else {
    if (ch->invis_level>0) {
      ch->invis_level = 0;
      send_to_char("You are now totally visible.\n\r",ch);
    } else {
      ch->invis_level = LOW_IMMORTAL;
      send_to_char("You are now invisible to all but gods.\n\r",ch);
    }
  }
}

void do_create( struct char_data *ch, char *argument, int cmd)
{
  int i, count, start, end;

  if (!IS_IMMORTAL(ch) || IS_NPC(ch)) {
    return;
  }


  count = sscanf(argument, "%d %d", &start, &end);
  if (count < 2) {
    send_to_char(" create <start> <end>\n\r", ch);
    return;
  }
  if (start > end) {
    send_to_char(" create <start> <end>\n\r", ch);
    return;
  }

  send_to_char("You form much order out of Chaos\n\r", ch);
  for (i = start; i<= end; i++) {
    if (!real_roomp(i))
    CreateOneRoom(i);
  }

}


void CreateOneRoom( int loc_nr)
{
  struct room_data *rp;
  extern int top_of_zone_table;

  char buf[256];

  allocate_room(loc_nr);
  rp = real_roomp(loc_nr);
  bzero(rp, sizeof(*rp));
  
  rp->number = loc_nr;
  if (top_of_zone_table >= 0) {
    int	zone;
    
    for (zone=0;
	 rp->number > zone_table[zone].top && zone<=top_of_zone_table;
	 zone++);
    if (zone > top_of_zone_table) {
      fprintf(stderr, 
	      "Room %d is outside of any zone.\n", rp->number);
      zone--;
    }
    rp->zone = zone;
  }
  sprintf(buf, "%d", loc_nr);
  rp->name = (char *)strdup(buf);
  rp->description = (char *)strdup("Empty\n");
}

void do_set_log(struct char_data *ch, char *arg, int cmd)
{
 char buf[255], name[255];
 struct char_data *victim;
 struct obj_data *dummy;

 if(IS_NPC(ch))
   return;

 only_argument(arg, name);
 if(!*name) {
   send_to_char("Usage: @log <character\n\r", ch);
   return;
 }

 if(!generic_find(arg, FIND_CHAR_WORLD, ch, &victim, &dummy)) {
   send_to_char("No such person in the world.\n\r", ch);
   return;
 }

 if(IS_NPC(victim)) {
   send_to_char("Victim is an NPC.\n\r", ch);
   return;
 }

 if(IS_AFFECTED2(victim, AFF2_LOG_ME)) {
   REMOVE_BIT(victim->specials.affected_by2, AFF2_LOG_ME);
   send_to_char("Log Bit Removed.\n\r", ch);
   return;
 }

 else {
   SET_BIT(victim->specials.affected_by2, AFF2_LOG_ME);
   send_to_char("Log Bit Set.\n\r", ch);
   return;
 }
}

void PulseMobiles(int cmd);

void do_event(struct char_data *ch, char *arg, int cmd)
{
 int i;
 char buf[255];

 only_argument(arg, buf);
 if(IS_NPC(ch))
   return;

 i = atoi(buf);

 PulseMobiles(i);
}

void do_beep(struct char_data *ch, char *argument, int cmd)
{
 char buf[255], name[255];
 struct char_data *victim;
 struct obj_data *dummy;

 if(IS_NPC(ch))
   return;

 only_argument(argument, name);
 if(!*name) {
   if(IS_SET(ch->specials.act, PLR_NOBEEP)) {
      send_to_char("Beep now ON.\n\r", ch);
      REMOVE_BIT(ch->specials.act, PLR_NOBEEP);
    }
   else {
      send_to_char("Beep now OFF.\n\r", ch);
      SET_BIT(ch->specials.act, PLR_NOBEEP);
    }
   return;
 }

 if(!generic_find(argument, FIND_CHAR_WORLD, ch, &victim, &dummy)) {
   send_to_char("No such person in the world.\n\r", ch);
   return;
 }

 if(IS_NPC(victim)) {
   send_to_char("Victim is an NPC.\n\r", ch);
   return;
 }

 if(IS_SET(victim->specials.act, PLR_NOBEEP)) {
   sprintf(buf, "%s can not be beeped right now.\n\r", GET_NAME(victim));
   send_to_char(buf, ch);
   return;
 }

 else {
   sprintf(buf, "%c%s is beeping you.\n\r", 7, GET_NAME(ch));
   send_to_char(buf, victim);
   sprintf(buf, "%s has been beeped.\n\r", GET_NAME(victim));
   send_to_char(buf, ch);
   return;
 }
}

void do_cset(struct char_data *ch, char *arg, int cmd)
{
 char buf[1000], buf1[255], buf2[255], buf3[255], buf4[255];
 int i, radix;
 NODE *n;
 extern struct radix_list radix_head[];
 extern byte HashTable[];
 
 if(IS_NPC(ch))
    return;
 
 arg = one_argument(arg, buf1);
 arg = one_argument(arg, buf2);
 arg = one_argument(arg, buf3);
 arg = one_argument(arg, buf4);
 
 i = atoi(buf4);
 
 if(!strcmp(buf1, "show")) {
    radix = HashTable[*buf2];
    if(!radix_head[radix].next) {
       send_to_char("Sorry, command not found.\n\r", ch);
       return;
     }
 
    n = SearchForNodeByName(radix_head[radix].next, buf2, strlen(buf2));
    if(!n) {
       send_to_char("Sorry, command not found.\n\r", ch);
       return;
     }
    sprintf(buf, "Name: %s\n\rMinimum Position: %d\n\rMinimum Level: %d\n\rNumber: %d\n\rLog Bit: %s\n\r",
            n->name, n->min_pos, n->min_level, n->number, (n->log ? "On" : "Off"));
    send_to_char(buf, ch);
    return;
  }
 
 else if(!strcmp(buf1, "set")) {
    radix = HashTable[*buf2];
    if(!radix_head[radix].next) {
       send_to_char("Sorry, command not found.\n\r", ch);
       return;
     }
 
    n = SearchForNodeByName(radix_head[radix].next, buf2, strlen(buf2));
    if(!n) {
       send_to_char("Sorry, command not found.\n\r", ch);
       return;
     }
 
    if(!strcmp(buf3, "level")) {
       if(i < 0 || i > 60) {
          send_to_char("Level must be between 0 and 60.\n\r", ch);
          return;
	}
    
       n->min_level = i;
       send_to_char("Level Changed.\n\r", ch);
       return;
     }
 
    if(!strcmp(buf3, "position")) {
       if(i < 0 || i > 10) {
          send_to_char("Position must be between 0 and 10.\n\r", ch);
          return;
	}
    
       n->min_pos = i;
       send_to_char("Position Changed.\n\r", ch);
       return;
     }
 
  }

 
 else if(!strcmp(buf1, "log")) {
    radix = HashTable[*buf2];
    if(!radix_head[radix].next) {
       send_to_char("Sorry, command not found.\n\r", ch);
       return;
     }
 
    n = SearchForNodeByName(radix_head[radix].next, buf2, strlen(buf2));
    if(!n) {
       send_to_char("Sorry, command not found.\n\r", ch);
       return;
     }

     if(n->log) {
        send_to_char("Command logging removed.\n\r", ch);
        n->log = 0;
        return;
     }
     else {
        send_to_char("Command logging set.\n\r", ch);
        n->log = 1;
        return;
     }
 } 
 send_to_char("Usage: # <\"set\" | \"show\" | \"log\"> <cmd> <\"level\" | \"position\"> <level>\n\r", ch);
 return;
}
 
