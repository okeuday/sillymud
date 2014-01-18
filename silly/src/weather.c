/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <string.h>

#include "protos.h"

/* uses */

extern struct time_info_data time_info;
extern struct weather_data weather_info;
extern struct room_data *room_db;
extern struct descriptor_data *descriptor_list;

/* what stage is moon in?  (1 - 32) */
unsigned char moontype;   

int gSeason;   /* global variable --- the season */
int gMoonSet = 3;
int gSunRise = 5;		/* all global as well :) */
int gSunSet = 18;
int gMoonRise = 22;

int gLightLevel = 4;  /* defaults to sunlight */

/* Here comes the code */

void weather_and_time(int mode)
{
  another_hour(mode);
  if(mode)
    weather_change();
}

void another_hour(int mode)
{
  char moon[20], buf[100];
  int tmp, i;
  
  time_info.hours++;
  
  tmp = time_info.hours;
  
  if (mode) {			/* tick */
    
    /* as a test, save a piece of the world every mud hour */
    SaveTheWorld();
    if (tmp == 0) {
      for (i=0;i<29;i++)   /* save the rest of the world automatically */
	SaveTheWorld();
    }
    if (tmp == gMoonRise) {
      if (moontype < 4) {
	strcpy(moon, "new");
      } else if (moontype < 12) {
	strcpy(moon, "waxing");
      } else if (moontype < 20) {
	strcpy(moon, "full");
      } else  if (moontype < 28) {
	strcpy(moon, "waning");
      } else {
	strcpy(moon, "new");
      }
      switch_light(MOON_RISE);
      sprintf(buf,"The %s moon slowly rises from the western horizon.\n\r",moon);
      send_to_outdoor(buf);
      if((moontype > 16) && (moontype < 22)) {
	gLightLevel++;   /* brighter during these moons */
      }
    }
    if (tmp == gSunRise) {
      weather_info.sunlight = SUN_RISE;
      send_to_outdoor("The sun begins to rise from the western horizon.\n\r");
    }
    if (tmp == gSunRise+1) {
      weather_info.sunlight = SUN_LIGHT;
      switch_light(SUN_LIGHT);
      send_to_outdoor("The day has begun.\n\r");
      GlobalSunProblemCheck(TRUE);
    }
    if (tmp == gSunSet) {
      weather_info.sunlight = SUN_SET;
      send_to_outdoor("The sun slowly disappears into the eastern horizon.\n\r");
    }
    if (tmp == gSunSet+1) {
      weather_info.sunlight = SUN_DARK;
      switch_light(SUN_DARK);
      send_to_outdoor("Nightime has settled upon the realms.\n\r");
      GlobalSunProblemCheck(FALSE);
    }
    if (tmp == gMoonSet) {
      if((moontype > 15) && (moontype < 25)) {
	switch_light(MOON_SET);
        send_to_outdoor("Darkness once again fills the realm as the moon sets.\n\r");
      } else {
	send_to_outdoor("The moon sets.\n\r");
      }
      
    }
    if (tmp == 12) {
      send_to_outdoor("It is noon.\n\r");
    }
    
    if (time_info.hours > 23)  /* Changed by HHS due to bug ???*/ {
      time_info.hours -= 24;
      time_info.day++;
      switch(time_info.day) {
      case 0:
      case 6:
      case 13:
      case 20:
      case 27:
      case 34:
	PulseMobiles(EVENT_WEEK);
	break;
      }  
      /* check the season */
      ChangeSeason(time_info.month);
      
      moontype++;
      if (moontype > 32)
	moontype = 1;
      
      if (time_info.day>34)  {
	time_info.day = 0;
	time_info.month++;
	GetMonth(time_info.month);
	PulseMobiles(EVENT_MONTH);
	
	if(time_info.month>16)	       	{
	  time_info.month = 0;
	  time_info.year++;
	}
      }
      
      ChangeSeason(time_info.month);      
    }
  }
}

void ChangeSeason(int month)
{
  extern int gSeason;
  switch (month){
  case 0:
  case 1:
  case 2:
  case 3:
  case 16:
    gSunRise = 9;  /* very late  */
    gSunSet = 16;  /* very early */
    gSeason = SEASON_WINTER;
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    gSunRise = 7;  /* late  */
    gSunSet = 18;  /* early */
    gSeason = SEASON_SPRING;
    break;
  case 8:
  case 9:
  case 10:
  case 11:
    gSunRise = 5;  /* early  */
    gSunSet = 20;  /* late   */
    gSeason = SEASON_SUMMER;
    break;
  case 12:
  case 13:
  case 14:
  case 15:
    gSunRise = 7;  /* late  */
    gSunSet = 18;  /* early */
    gSeason = SEASON_FALL;
    break;
  default:
    gSeason = SEASON_WINTER;
    gSunRise = 9;  /* very late  */
    gSunSet = 16;  /* very early */
    break;
  }
}

void weather_change()
{
  int diff, change;
  
  if((time_info.month>=9)&&(time_info.month<=16))
    diff=(weather_info.pressure>985 ? -2 : 2);
  else
    diff=(weather_info.pressure>1015? -2 : 2);
  
  weather_info.change += (dice(1,4)*diff+dice(2,6)-dice(2,6));
  
  weather_info.change = MIN(weather_info.change,12);
  weather_info.change = MAX(weather_info.change,-12);
  
  weather_info.pressure += weather_info.change;
  
  weather_info.pressure = MIN(weather_info.pressure,1040);
  weather_info.pressure = MAX(weather_info.pressure,960);
  
  change = 0;
  
  switch(weather_info.sky){
  case SKY_CLOUDLESS :
    {
      if (weather_info.pressure<990)
	change = 1;
      else if (weather_info.pressure<1010)
	if(dice(1,4)==1)
	  change = 1;
      break;
    }
  case SKY_CLOUDY :
    {
      if (weather_info.pressure<970)
	change = 2;
      else if (weather_info.pressure<990)
	if(dice(1,4)==1)
	  change = 2;
	else
	  change = 0;
      else if (weather_info.pressure>1030)
	if(dice(1,4)==1)
	  change = 3;
      
      break;
    }
  case SKY_RAINING :
    {
      if (weather_info.pressure<970)
	if(dice(1,4)==1)
	  change = 4;
	else
	  change = 0;
      else if (weather_info.pressure>1030)
	change = 5;
      else if (weather_info.pressure>1010)
	if(dice(1,4)==1)
	  change = 5;
      
      break;
    }
  case SKY_LIGHTNING :
    {
      if (weather_info.pressure>1010)
	change = 6;
      else if (weather_info.pressure>990)
	if(dice(1,4)==1)
	  change = 6;
      
      break;
    }
    default : 
      {
	change = 0;
	weather_info.sky=SKY_CLOUDLESS;
	break;
      }
  }
  
  ChangeWeather(change);
  
}

void ChangeWeather( int change)
{

   if (change < 0)
      change = 0;
   if (change > 7)
     change = 6;
   
   switch(change){
   case 0 : break;
   case 1 :
     {
       send_to_outdoor("The sky is getting cloudy.\n\r");
       weather_info.sky=SKY_CLOUDY;
       break;
     }
   case 2 :
     {
       if ((time_info.month > 3) && (time_info.month < 14)) {
	 send_to_desert("A strong wind begins to sweep across the land\n\r");
         send_to_arctic("It starts to snow\n\r");
	 send_to_out_other("It starts to rain.\n\r");
       } else {
	 send_to_desert(
	      "A strong, cold wind begins to sweep across the land\n\r");
         send_to_arctic("It starts to snow heavily.\n\r");
	 send_to_out_other("It starts to snow.\n\r");
       }
       weather_info.sky=SKY_RAINING;
       break;
     }
   case 3 :
     {
       send_to_outdoor("The clouds disappear.\n\r");
       weather_info.sky=SKY_CLOUDLESS;
       break;
     }
   case 4 :
     {
       if ((time_info.month > 3) && (time_info.month < 14)) {
	 send_to_desert(
	      "You are caught in a blinding sandstorm\n\r");
         send_to_arctic("You are caught in a blinding blizzard\n\r");
	 send_to_out_other("You are caught in lightning storm.\n\r");
       } else  {
	 send_to_desert(
	      "You are caught in a blinding sandstorm\n\r");
         send_to_arctic("You are caught in a blizzard\n\r");
	 send_to_out_other("You are caught in a blizzard. \n\r" );
       }
       weather_info.sky=SKY_LIGHTNING;
       break;
     }
   case 5 :
     {
       if ((time_info.month > 3) && (time_info.month < 14))  {
	 send_to_desert(
	      "The sandstorm slowly quiets\n\r");
         send_to_arctic("The snowstorm slowly dies down\n\r");
	 send_to_out_other("The rainstorm slowly dies down\n\r" );
       } else  {
	 send_to_desert(
	      "The sandstorm slowly quiets\n\r");
         send_to_arctic("It has stopped snowing\n\r");
	 send_to_out_other("The snow has stopped. \n\r");
       }
       weather_info.sky=SKY_CLOUDY;
       break;
     }
   case 6 :
     {
       if ((time_info.month > 3) && (time_info.month < 14)) {
	 send_to_desert(
	      "The sandstorm dies down, but the wind continues\n\r");
         send_to_arctic("The blizzard has died down, but the snow continutes\n\r");
	 send_to_out_other(
                "The lightning has gone, but it is still raining.\n\r");
       } else {
	 send_to_desert(
	      "The sandstorm dies down, but the wind continues\n\r");
	 send_to_arctic("The blizzard is over, but it is still snowing.\n\r");
	 send_to_out_other("The blizzard is over, but it is still snowing.\n\r");
       }
       weather_info.sky=SKY_RAINING;
       break;
     }
     default : break;
   }
 }

void GetMonth( int month)
{
   if (month < 0)
      return;

   if (month <= 1) {
       send_to_outdoor(" It is bitterly cold outside\n\r");
      }
   else if (month <=2) {
       send_to_outdoor(" It is very cold \n\r");
      }
   else if (month <=3) {
       send_to_outdoor(" It is chilly outside \n\r");
      }
   else if (month == 4) {
        send_to_outdoor(" The flowers start to bloom \n\r");
        PulseMobiles(EVENT_SPRING);
   }
   else if (month == 8) {
        send_to_outdoor(" It is warm and humid. \n\r");
        PulseMobiles(EVENT_SUMMER);
      }
   else if (month == 12) {
        send_to_outdoor(" It starts to get a little windy \n\r");
        PulseMobiles(EVENT_FALL);
      }
   else if (month == 13) {
        send_to_outdoor(" The air is getting chilly \n\r"); 
      }
   else if (month == 14) {
        send_to_outdoor(" The leaves start to change colors. \n\r");
      }
   else if (month == 15) {
        send_to_outdoor(" It starts to get cold \n\r");
      }
   else if (month == 16) {
        send_to_outdoor(" It is bitterly cold outside \n\r");
        PulseMobiles(EVENT_WINTER);
      }
}

void switch_light(byte why)
{
 extern int gLightLevel;


 switch(why) {
 case MOON_SET:
   log_sev("setting all rooms to dark", 2);
   gLightLevel = 0;
   break;
 case SUN_LIGHT:
   log_sev("setting all rooms to light", 2);
   gLightLevel = 4;
   break;
 case SUN_DARK:
   log_sev("setting all rooms to dark", 2);
   gLightLevel = 0;
   break;
 case MOON_RISE:
   log_sev("setting all non-forest to light", 2);
   gLightLevel = 1;
   break;
 default:
   log_sev("Unknown switch on switch_light", 2);
   break;
 }

}

void GlobalSunProblemCheck(bool light)
{
  struct descriptor_data *i;

  for (i = descriptor_list; i; i = i->next)
    if (!i->connected) {
      if(OUTSIDE(i->character)) {
	if(SUNPROBLEM(i->character)) {
	  if(!IS_AFFECTED2(i->character, AFF2_SUN_BLIND)) {
	    if(light)
	      SunBlind(i->character);
	    else
	      RemoveSunBlind(i->character);
	  }
	}
      }
    }

}

void SunProblemCheck (struct char_data *ch)
{
  if(SUNPROBLEM(ch)) {
    if( (time_info.hours > 5 && time_info.hours < 18) && 
       (OUTSIDE(ch) && !IS_SET(real_roomp(ch->in_room)->room_flags, DARK)) ) {
      if(!IS_AFFECTED2(ch, AFF2_SUN_BLIND)) {
	SunBlind(ch);
      } 
    } else if( IS_AFFECTED2(ch, AFF2_SUN_BLIND) ) {
      RemoveSunBlind(ch);
    }
  }
}


void SunBlind(struct char_data *ch) 
{
  struct affected_type af;

  if(GetMaxLevel(ch) >= LOW_IMMORTAL)
    return;

  if(IS_AFFECTED2(ch, AFF2_SUN_BLIND))
    return;
  
  if(IS_AFFECTED(ch, AFF_BLIND))
    return;

  if(affected_by_spell(ch, SPELL_SUN_BLIND)) 
    affect_from_char(ch, SPELL_SUN_BLIND);
  
  send_to_char("Aaarrrggghh! The sun burns your eyes!\n\r", ch);
  
  af.type     = SPELL_SUN_BLIND;
  af.location = APPLY_HITROLL;
  af.modifier = -4;
  af.duration = (number(1,2) + (5 - GetMaxLevel(ch) / 10)); 
  af.bitvector= 0;
  affect_to_char(ch, &af);

  af.location  = APPLY_AC;
  af.modifier  = +20;
  affect_to_char(ch, &af);

  af.modifier  = 0;
  af.location  = APPLY_BV2;
  af.bitvector = AFF2_SUN_BLIND;
  affect_to_char(ch, &af);
}
  
void RemoveSunBlind(struct char_data *ch)
{
  struct affected_type af;

  if(GetMaxLevel(ch) >= LOW_IMMORTAL) 
    return;

  send_to_char("Your vision begins to return.\n\r", ch);
  act("$n's vision begins to return.\n\r", TRUE, ch, 0, 0, TO_ROOM);
  affect_from_char(ch, SPELL_SUN_BLIND);

  af.type     = SPELL_SUN_BLIND;
  af.location = APPLY_HITROLL;
  af.modifier = -2;
  af.duration = 1;
  af.bitvector=0;
  affect_to_char(ch, &af);

  af.location = APPLY_AC;
  af.modifier = +10;
  affect_to_char(ch, &af);

}

