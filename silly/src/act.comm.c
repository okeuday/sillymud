/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <string.h>

#include "protos.h"

#define BIRD_NOISE_NUM 4	/* how many noises we have, in constants.c */
#define RAT_NOISE_NUM  3
/* extern variables */

extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern const char *rat_noises[];
extern const char *half_orc_words[];
extern const char *half_orc_noises[];
extern const char *ogre_speak[];

void do_say(struct char_data *ch, char *argument, int cmd)
{
  char buf[MAX_INPUT_LENGTH+40], buf2[MAX_INPUT_LENGTH+40];
  
  if (apply_soundproof(ch))
    return;
  
  /*  for (i = 0; *(argument + i) == ' '; i++); */
  for (; *argument == ' '; argument++);
  
  if (!*(argument))
    send_to_char("Yes, but WHAT do you want to say?\n\r", ch);
  else {
    if (IS_NPC(ch)||(IS_SET(ch->specials.act, PLR_ECHO))) {
      sprintf(buf,"You say '%s'\n\r", argument);
      send_to_char(buf, ch);
    }

    if(GET_RACE(ch) == RACE_OGRE) {
      ogre_garble(argument, buf2, ch);
      sprintf(buf, "$n says '%s'", buf2);
    } else if( GET_RACE(ch) == RACE_DRAAGDIM ) {
      rat_garble(argument, buf2, ch);
      sprintf(buf, "$n says '%s'", buf2);
    } else if(GET_RACE(ch) == RACE_HALFORC) {
      half_orc_garble(argument, buf2, ch);
      sprintf(buf, "$n says '%s'", buf2);
    } else {
      sprintf(buf,"$n says '%s'", argument);
    }
    act(buf,FALSE,ch,0,0,TO_ROOM);
  }
}




void do_shout(struct char_data *ch, char *argument, int cmd)
{
  char buf1[MAX_INPUT_LENGTH+40], buf2[MAX_INPUT_LENGTH+40];
  struct descriptor_data *i;
  extern int Silence;
  
  if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOSHOUT)) {
    send_to_char("You can't shout!!\n\r", ch);
    return;
  }

  if (IS_NPC(ch) && 
      (Silence == 1) &&
      (IS_SET(ch->specials.act, ACT_POLYSELF))) {
    send_to_char("Polymorphed shouting has been banned.\n\r", ch);
    send_to_char("It may return after a bit.\n\r", ch);
    return;
  }

  if (apply_soundproof(ch))
    return;
  
  for (; *argument == ' '; argument++);
  
  if (ch->master && IS_AFFECTED(ch, AFF_CHARM)) {
    if (!IS_IMMORTAL(ch->master)) {
      send_to_char("I don't think so :-)", ch->master);
      return;
    }
  }
  
  if (!(*argument))
    send_to_char("Shout? Yes! Fine! Shout we must, but WHAT??\n\r", ch);
  else	{
    if (IS_NPC(ch) || IS_SET(ch->specials.act, PLR_ECHO)) {
      sprintf(buf1,"You shout '%s'\n\r", argument);
      send_to_char(buf1, ch);
    }

    if(GET_RACE(ch) == RACE_DRAAGDIM) {
      rat_garble(argument, buf2, ch);
      sprintf(buf1, "$n shouts '%s'", buf2);
    } else if(GET_RACE(ch) == RACE_HALFORC) {
      half_orc_garble(argument, buf2, ch);
      sprintf(buf1, "$n shouts '%s'", buf2);
    } else if(GET_RACE(ch) == RACE_OGRE) {
      ogre_garble(argument, buf2, ch);
      sprintf(buf1, "$n shouts '%s'", buf2);
    } else {
      sprintf(buf1, "$n shouts '%s'", argument);
    }

    act("$n lifts up $s head and shouts loudly", TRUE, ch, 0, 0, TO_ROOM);
    
    GET_MOVE(ch) -= 2;
    
    for (i = descriptor_list; i; i = i->next)
      if (i->character != ch && !i->connected &&
	  (IS_NPC(i->character) ||
	   (!IS_SET(i->character->specials.act, PLR_NOSHOUT) &&
	    !IS_SET(i->character->specials.act, PLR_DEAF))) &&
	  !check_soundproof(i->character)) {
	  act(buf1, 0, ch, 0, i->character, TO_VICT);
      }
  }
}

void do_commune(struct char_data *ch, char *argument, int cmd)
{
  static char buf1[MAX_INPUT_LENGTH];
  struct descriptor_data *i;
  
  
  for (; *argument == ' '; argument++);
  
  if (!(*argument))
    send_to_char("Communing among the gods is fine, but WHAT?\n\r",ch);
  else {
    if (IS_NPC(ch) || IS_SET(ch->specials.act, PLR_ECHO)) {
      sprintf(buf1,"You think '%s'\n\r", argument);
      send_to_char(buf1, ch);
    }
    sprintf(buf1, "$n thinks '%s'", argument);
    
    for (i = descriptor_list; i; i = i->next)
      if (i->character != ch && !i->connected && !IS_NPC(i->character) &&
	  !IS_SET(i->character->specials.act, PLR_NOSHOUT) &&
	  (GetMaxLevel(i->character) >= LOW_IMMORTAL))
	act(buf1, 0, ch, 0, i->character, TO_VICT);
  }
}


void do_tell(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict;
  char name[100], message[MAX_INPUT_LENGTH+20],
  buf[MAX_INPUT_LENGTH+60], buf2[MAX_INPUT_LENGTH+60];

  if (apply_soundproof(ch))
    return;
  
  half_chop(argument,name,message);

  if(!*name || !*message) {
    send_to_char("Who do you wish to tell what??\n\r", ch);
    return;
  } else if (!(vict = get_char_vis(ch, name))) {
    send_to_char("No-one by that name here..\n\r", ch);
    return;
  } else if (ch == vict) {
    send_to_char("You try to tell yourself something.\n\r", ch);
    return;
  } else if (GET_POS(vict) == POSITION_SLEEPING)	{
    act("$E is asleep, shhh.",FALSE,ch,0,vict,TO_CHAR);
    return;
  } else if (IS_NPC(vict) && !(vict->desc)) {
    send_to_char("No player here by that name...\n\r", ch);
    return;
  } else if (!vict->desc) {
    send_to_char("They can't hear you, they are link dead.\n\r", ch);
    return;
  }
  
  if (check_soundproof(vict))
    return;
  
  if (IS_NPC(ch) || IS_SET(ch->specials.act, PLR_ECHO)) { 
    sprintf(buf,"You tell %s '%s'\n\r",
	 (IS_NPC(vict) ? vict->player.short_descr : GET_NAME(vict)), message);
    send_to_char(buf, ch);
  }

  if(GET_RACE(ch) == RACE_DRAAGDIM) {
    rat_garble(message, buf2, ch);
    sprintf(buf,"$n tells you '%s'", buf2);
  } else if(GET_RACE(ch) == RACE_HALFORC) {
    half_orc_garble(message, buf2, ch);
    sprintf(buf,"$n tells you '%s'", buf2);
  } else if(GET_RACE(ch) == RACE_OGRE) {
    ogre_garble(message, buf2, ch);
    sprintf(buf,"$n tells you '%s'", buf2);
  } else {
    sprintf(buf,"$n tells you '%s'", message);
  }
  act(buf, TRUE, ch, 0, vict, TO_VICT);
  /* send_to_char(buf, vict); */
}



void do_whisper(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict;
  char name[100], message[MAX_INPUT_LENGTH],
  buf[MAX_INPUT_LENGTH];

	if (apply_soundproof(ch))
	  return;
  
  half_chop(argument,name,message);
  
  if(!*name || !*message)
    send_to_char("Who do you want to whisper to.. and what??\n\r", ch);
  else if (!(vict = get_char_room_vis(ch, name)))
    send_to_char("No-one by that name here..\n\r", ch);
  else if (vict == ch) {
    act("$n whispers quietly to $mself.",FALSE,ch,0,0,TO_ROOM);
    send_to_char
      ("You can't seem to get your mouth close enough to your ear...\n\r",ch);
  }  else    {
    if (check_soundproof(vict))
      return;
    
    sprintf(buf,"$n whispers to you, '%s'",message);
    act(buf, FALSE, ch, 0, vict, TO_VICT);
    if (IS_NPC(ch) || (IS_SET(ch->specials.act, PLR_ECHO))) {
      sprintf(buf,"You whisper to %s, '%s'\n\r",
	      (IS_NPC(vict) ? vict->player.name : GET_NAME(vict)), message);
      send_to_char(buf, ch);
    }
    act("$n whispers something to $N.", FALSE, ch, 0, vict, TO_NOTVICT);
  }
}


void do_ask(struct char_data *ch, char *argument, int cmd)
{
  struct char_data *vict;
  char name[100], message[MAX_INPUT_LENGTH],
  buf[MAX_INPUT_LENGTH];

	if (apply_soundproof(ch))
	  return;
  
  half_chop(argument,name,message);
  
  if(!*name || !*message)
    send_to_char("Who do you want to ask something.. and what??\n\r", ch);
  else if (!(vict = get_char_room_vis(ch, name)))
    send_to_char("No-one by that name here..\n\r", ch);
  else if (vict == ch)	{
    act("$n quietly asks $mself a question.",FALSE,ch,0,0,TO_ROOM);
    send_to_char("You think about it for a while...\n\r", ch);
  }  else	{
	if (check_soundproof(vict))
	  return;

    sprintf(buf,"$n asks you '%s'",message);
    act(buf, FALSE, ch, 0, vict, TO_VICT);
    
    if (IS_NPC(ch) || (IS_SET(ch->specials.act, PLR_ECHO))) {
      sprintf(buf,"You ask %s, '%s'\n\r",
	    (IS_NPC(vict) ? vict->player.name : GET_NAME(vict)), message);
      send_to_char(buf, ch);
    }
    act("$n asks $N a question.",FALSE,ch,0,vict,TO_NOTVICT);
  }
}



#define MAX_NOTE_LENGTH 1000      /* arbitrary */

void do_write(struct char_data *ch, char *argument, int cmd)
{
  struct obj_data *paper = 0, *pen = 0;
  char papername[MAX_INPUT_LENGTH], penname[MAX_INPUT_LENGTH],
       buf[MAX_STRING_LENGTH];
  
  argument_interpreter(argument, papername, penname);
  
  if (!ch->desc)
    return;
  
  if (!*papername)  /* nothing was delivered */    {   
      send_to_char("write (on) papername (with) penname.\n\r", ch);
      return;
    }

  if (!*penname) {
      send_to_char("write (on) papername (with) penname.\n\r", ch);
      return;
  }
  if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))	{
	  sprintf(buf, "You have no %s.\n\r", papername);
	  send_to_char(buf, ch);
	  return;
   }
   if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying)))	{
	  sprintf(buf, "You have no %s.\n\r", papername);
	  send_to_char(buf, ch);
	  return;
    }

  /* ok.. now let's see what kind of stuff we've found */
  if (pen->obj_flags.type_flag != ITEM_PEN) {
      act("$p is no good for writing with.",FALSE,ch,pen,0,TO_CHAR);
  } else if (paper->obj_flags.type_flag != ITEM_NOTE)    {
      act("You can't write on $p.", FALSE, ch, paper, 0, TO_CHAR);
  } else if (paper->action_description) {
    send_to_char("There's something written on it already.\n\r", ch);
    return;
  } else {
      /* we can write - hooray! */
      send_to_char
	("Ok.. go ahead and write.. end the note with a @.\n\r", ch);
      act("$n begins to jot down a note.", TRUE, ch, 0,0,TO_ROOM);
      ch->desc->str = &paper->action_description;
      ch->desc->max_str = MAX_NOTE_LENGTH;
    }
}

char *RandomWord()
{
  static char *string[55] = {
    "argle",
    "bargle",
    "glop",
    "glyph",
    "hussamah",			/* 5 */
    "rodina",
    "mustafah",
    "angina",
    "the",
    "fribble",			/* 10 */
    "fnort",
    "frobozz",
    "zarp",
    "ripple",
    "yrk",			/* 15 */
    "yid",
    "yerf",
    "oork",
    "grapple",
    "red",			/* 20 */
    "blue",
    "you",
    "me",
    "ftagn",
    "hastur",			/* 25 */
    "brob",
    "gnort",
    "lram",
    "truck",
    "kill",			/* 30 */
    "cthulhu",
    "huzzah",
    "acetacytacylic",
    "hydrooxypropyl",
    "summah",			/* 35 */
    "hummah",
    "cookies",
    "stan",
    "will",
    "wadapatang",		/* 40 */
    "pterodactyl",
    "frob",
    "yuma",
    "gumma",
    "lo-pan",			/* 45 */
    "sushi",
    "yaya",
    "yoyodine",
    "ren",
    "stimpy",			/* 50 */
    "blogark",
    "vacetophenetidin",
    "barbellate",
    "achoo",
    "hydrocephalus"		/* 55 */
      
  };

  return(string[number(0,54)]);

}

void do_sign(struct char_data *ch, char *argument, int cmd)
{
  int i;
  char buf[MAX_INPUT_LENGTH+40];
  char buf2[MAX_INPUT_LENGTH];
  char *p;
  int diff;
  struct char_data *t;
  struct room_data *rp;
    
  for (i = 0; *(argument + i) == ' '; i++);
  
  if (!*(argument + i))
    send_to_char("Yes, but WHAT do you want to sign?\n\r", ch);
  else	{

    rp = real_roomp(ch->in_room);
    if (!rp) return;

    if (!HasHands(ch)) {
      send_to_char("Yeah right... WHAT HANDS!!!!!!!!\n\r", ch);
      return;
    }

    strcpy(buf, argument+i);
    buf2[0] = '\0';
    /*
      work through the argument, word by word.  if you fail your
      skill roll, the word comes out garbled.
      */
    p = strtok(buf, " ");  /* first word */

    diff = strlen(buf);

    while (p) {
      if (ch->skills && number(1,75+strlen(p))<ch->skills[SKILL_SIGN].learned){
	strcat(buf2, p);
      } else {
	strcat(buf2, RandomWord());
      }
      strcat(buf2, " ");
      diff -= 1;
      p = strtok(0, " ");  /* next word */
    }
    /*
      if a recipient fails a roll, a word comes out garbled.
      */

    /*
      buf2 is now the "corrected" string.
      */

    sprintf(buf,"$n signs '%s'", buf2);

    for (t = rp->people;t;t=t->next_in_room) {
      if (t != ch) {
	if (t->skills && number(1,diff) < t->skills[SKILL_SIGN].learned) {
	  act(buf, FALSE, ch, 0, t, TO_VICT);
	} else {
	  act("$n makes funny motions with $s hands", 
	      FALSE, ch, 0, t, TO_VICT);	  
	}
      }
    }

    if (IS_NPC(ch)||(IS_SET(ch->specials.act, PLR_ECHO))) {
      sprintf(buf,"You sign '%s'\n\r", argument + i);
      send_to_char(buf, ch);
    }
  }
}

void bird_garble(char *buf, char *buf2, struct char_data *ch)
{
#if 0
  char *p;

  buf2[0] = '\0';
  
  p = strtok(buf, " ");
  
  while(p) {
    if(number(1,18) > GET_INT(ch)) {
      strcat(buf2, bird_noises[number(0,BIRD_NOISE_NUM)]);
      strcat(buf2, " ");
    }
    strcat(buf2, p);
    p = strtok(0, " ");
    if(p)
      strcat(buf2, " ");
  }
#endif
}

void rat_garble(char *buf, char *buf2, struct char_data *ch)
{
  char *p;

  buf2[0] = '\0';

  p = strtok(buf, " ");

  while(p) {
    if(number(1,18) > GET_INT(ch)) {
      strcat(buf2, rat_noises[number(0,RAT_NOISE_NUM)]);
      strcat(buf2, " ");
    }
    strcat(buf2, p);
    p = strtok(0, " ");
    if(p)
      strcat(buf2, " ");
  }
}

void ogre_garble(char *buf, char *buf2, struct char_data *ch)
{
  char *p;

  /* This always happens to ogres.  Always.  Roleplaying for the */
  /* imaginative impaired.  Cross our fingers, should be fun :)  */
  buf2[0] = '\0';

  p = strtok(buf, " ");

  switch(number(0,9)) {
  case 0:
    strcat(buf2, "Oy, ");
    break;
  case 1:
    strcat(buf2, "Oy slim, ");
    break;
  case 2:
    strcat(buf2, "Flippen' eck! ");
    break;
  case 3:
    strcat(buf2, "OY! ");
    break;
  default:
    break;
  }
  
  while(p) {
    if(IsArticle(p)) {
      if(number(1,20) > GET_INT(ch)) {
	strcat(buf2, p);
	strcat(buf2, " ");
	strcat(buf2,ogre_speak[number(0,36)]); /* see constants.c */
      } else {
	strcat(buf2, p);
      }
    } else {
      strcat(buf2, p);
    }
    p = strtok(0, " ");
    if(p)
      strcat(buf2, " ");
  }
}

void half_orc_garble(char *buf, char *buf2, struct char_data *ch)
{
  char *p;

  buf2[0] = '\0';

  p = strtok(buf, " ");		

  if(GET_INT(ch) < number(1,18)) {
    strcat(buf2, half_orc_words[number(0,14)]);
    strcat(buf2, ", ");
  }
  
  while(p) {
    if (number(1,18) > GET_INT(ch) && !number(0,1)) {
      strcat(buf2, half_orc_noises[number(0,5)]);
      strcat(buf2, " ");
    }
    strcat(buf2, p);
    p = strtok(0, " ");
    if(p)
      strcat(buf2, " ");
  }
  /*   if(!blah) {
    if(GET_INT(ch) < number(1,18)) {
      strcat(buf2, " ");
      strcat(buf2, half_orc_words[number(0,14)]);
      strcat(buf2, ".");
    }
  }
 */
}
