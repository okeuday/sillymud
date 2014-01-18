/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "protos.h"

/* because I don't want to recompile */

#define MANA_MU 1
#define MANA_CL 1


#define SPELLO(nr, beat, pos, mlev, clev, dlev, mana, tar, func, sf) { \
      	       skill_info[nr].spell_pointer = (func);    \
               skill_info[nr].beats = (beat);            \
               skill_info[nr].minimum_position = (pos);  \
	       skill_info[nr].min_usesmana = (mana);     \
               skill_info[nr].min_level[MIN_LEVEL_CLERIC] = (clev); \
               skill_info[nr].min_level[MIN_LEVEL_MAGIC] = (mlev);  \
               skill_info[nr].min_level[MIN_LEVEL_DRUID] = (dlev);  \
               skill_info[nr].targets = (tar);           \
	       skill_info[nr].spellfail = (sf);          \
       	}



/* 100 is the MAX_MANA for a character */
#define USE_MANA(ch, sn) \
  MAX((int)skill_info[sn].min_usesmana, 100/MAX(2,(2+GET_LEVEL(ch, BestMagicClass(ch))-SPELL_LEVEL(ch,sn))))

#define SPELL_MEMORIZED 2

#define MEMORIZED(ch, spl) ((ch)->skills[spl].flags & SPELL_MEMORIZED)
#define FORGET(ch, spl) (IS_SET((ch)->skills[spl].flags, SPELL_MEMORIZED))

/* Global data */

extern struct room_data *world;
extern struct char_data *character_list;
extern char *spell_wear_off_msg[];
extern char *spell_wear_off_soon_msg[];
extern char *spell_wear_off_room_msg[];
extern char *spell_wear_off_soon_room_msg[];
extern struct obj_data *object_list;
extern struct index_data *obj_index;
extern struct con_app_type con_app[];
extern struct weather_data weather_info;
extern int sf_where[];

/*  internal procedures */
void SpellWearOffSoon(int s, struct char_data *ch);
void check_drowning( struct char_data *ch);
int check_falling( struct char_data *ch);
int IsIntrinsic(struct char_data *ch, int spl);
int CastIntrinsic(struct char_data *ch, int spl);
void check_decharm( struct char_data *ch);
void SpellWearOff(int s, struct char_data *ch);

/* struct spell_info_type spell_info[MAX_SPL_LIST]; */
struct skill_data skill_info[MAX_SPL_LIST];

char *spells[]=
{
   "spirit armor",               /* 1 */
   "teleport",
   "bless",
   "blindness",
   "burning hands",
   "call lightning",
   "charm person",
   "chill touch",
   "clone",
   "colour spray",
   "control weather",     /* 11 */
   "create food",
   "create water",
   "cure blind",
   "cure critic",
   "cure light",
   "curse",
   "detect evil",
   "detect invisibility",
   "detect magic",
   "detect poison",       /* 21 */
   "dispel evil",
   "earthquake",
   "enchant weapon",
   "energy drain",
   "fireball",
   "harm",
   "heal",
   "invisibility",
   "lightning bolt",
   "locate object",      /* 31 */
   "magic missile",
   "poison",
   "protection from evil",
   "remove curse",
   "sanctuary",
   "shocking grasp",
   "sleep",
   "strength",
   "summon",
   "ventriloquate",      /* 41 */
   "word of recall",
   "remove poison",
   "sense life",         /* 44 */
   "sneak",        /* 45 */
   "hide",
   "steal",
   "backstab",
   "pick",
   "kick",         /* 50 */
   "bash",
   "rescue",
   "identify",           /* 53 */
   "infravision",        
   "cause light",        
   "cause critical",
   "flamestrike",
   "dispel good",      
   "weakness",
   "dispel magic",
   "knock",
   "know alignment",
   "animate dead",
   "paralyze",
   "remove paralysis",
   "fear",
   "acid blast",  /* 67 */
   "water breath",
   "fly",
   "cone of cold",   /* 70 */
   "meteor swarm",
   "ice storm",
   "shield",
   "monsum one",
   "monsum two",
   "monsum three",
   "monsum four",
   "monsum five",
   "monsum six",
   "monsum seven",  /* 80 */
   "fireshield",
   "charm monster",
   "cure serious",
   "cause serious",
   "refresh",
   "second wind",
   "turn",
   "succor",
   "create light",
   "continual light",	/* 90 */
   "calm",
   "stone skin",
   "conjure elemental",
   "true sight",
   "minor creation",
   "faerie fire",
   "faerie fog",
   "cacaodemon",
   "polymorph self",
   "mana",	/* 100 */
   "astral walk",
   "resurrection",
   "heroes feast",  /* 103 */
   "group fly",
   "breath",
   "web",
   "minor track",
   "major track",
   "golem",
   "find familiar",	/* 110 */
   "changestaff",
   "holy word",
   "enchant armor",
   "power word kill",
   "power word blind",
   "chain lightning",
   "scare",
   "aid",
   "command",
   "****",	/* 120 */
   "feeblemind",
   "shillelagh",
   "goodberry",
   "flame blade",
   "animal growth",
   "insect growth",
   "creeping death",
   "commune",
   "animal summon one",
   "animal summon two",	/* 130 */
   "animal summon three",
   "fire servant",
   "earth servant",
   "water servant",
   "wind servant",
   "reincarnate",
   "charm vegatable",
   "vegetable growth",
   "tree",
   "animate rock",	/* 140 */
   "tree travel",
   "travelling",
   "animal friendship",
   "invis to animals",
   "slow poison",
   "entangle",
   "snare",
   "gust of wind",
   "barkskin",
   "sunray",	/* 150 */
   "warp weapon",
   "heat stuff",
   "find traps",
   "firestorm",
   "haste",
   "slowness",
   "dust devil",
   "know monster",
   "transport via plant",
   "speak with plants",	/* 160 */
   "silence",
   "sending",
   "teleport without error",
   "portal",
   "dragon ride",
   "mount",
   "thorn spray",
   "****",
   "dual wield",
   "first aid",	/* 170 */
   "sign language",
   "riding",
   "switch opponents",
   "dodge",
   "remove trap",
   "retreat",
   "quivering palm",
   "safe fall",
   "feign death",
   "hunt",	/* 180 */
   "locate trap",
   "spring leap",
   "disarm",
   "read magic",
   "evaluate",
   "spy",
   "doorbash",
   "swim",
   "necromancy",
   "vegetable lore",	/* 190 */
   "demonology",
   "animal lore",
   "reptile lore",
   "people lore",
   "giant lore",
   "other lore",
   "disguise",
   "climb",
   "inset",
   "****",			/* 200 */
   "fire breath",
   "gas breath",
   "frost breath",
   "acid breath",
   "lightning breath",
   "resist hold",
   "resist electricity",
   "resist cold",
   "resist drain",
   "resist poison",		/* 210 */
   "resist acid",
   "resist fire",
   "resist energy",
   "resist pierce",
   "resist slash",
   "resist blunt",		/* 216 */
   "brewing", 
   "sun blindness",		/* 218 */
   "berserk",
   "palm",
   "peek",			/* 221 */
   "insect lore",
   "avian lore",		/* 223 */
   "\n"
};


const byte saving_throws[MAX_CLASS][5][ABS_MAX_LVL] = {
{
/* mage */
  {16,14,14,14,14,14,13,13,13,13,13,11,11,11,11,11,10,10,10,10,10, 8, 6, 4, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0},
  {13,11,11,11,11,11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 3, 2, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
  {15,13,13,13,13,13,11,11,11,11,11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5, 4, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
  {17,15,15,15,15,15,13,13,13,13,13,11,11,11,11,11, 9, 9, 9, 9, 9, 7, 5, 3, 3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
  {14,12,12,12,12,12,10,10,10,10,10, 8, 8, 8, 8, 8, 6, 6, 6, 6, 6, 4, 3, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0}
}, 
/* cleric */
{
  {11,10,10,10, 9, 9, 9, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 2, 2, 2, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
  {16,14,14,14,13,13,13,11,11,11,10,10,10, 9, 9, 9, 8, 8, 8, 6, 6, 5, 4, 3, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0},
  {15,13,13,13,12,12,12,10,10,10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 5, 5, 4, 3, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {18,16,16,16,15,15,15,13,13,13,12,12,12,11,11,11,10,10,10, 8, 8, 7, 6, 5, 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {17,15,15,15,14,14,14,12,12,12,11,11,11,10,10,10, 9, 9, 9, 7, 7, 6, 5, 4, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}, 
/* warrior */
{
  {15,13,13,13,13,12,12,12,12,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 7, 6, 5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {16,14,14,14,14,12,12,12,12,10,10,10,10, 8, 8, 8, 8, 6, 6, 6, 6, 4, 3, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {14,12,12,12,12,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 5, 3, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {18,16,16,16,16,15,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,11, 9, 5, 5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {17,15,15,15,15,13,13,13,13,11,11,11,11, 9, 9, 9, 9, 7, 7, 7, 7, 5, 3, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}, 
/* thief */
{
  {16,14,14,13,13,11,11,10,10, 8, 8, 7, 7, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {18,16,16,15,15,13,13,12,12,10,10, 9, 9, 7, 7, 6, 6, 5, 5, 5, 5, 4, 3, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {17,15,15,14,14,12,12,11,11, 9, 9, 8, 8, 6, 6, 5, 5, 4, 4, 4, 4, 3, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {20,17,17,16,16,13,13,12,12, 9, 9, 8, 8, 5, 5, 4, 4, 4, 4, 4, 4, 3, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {19,17,17,16,16,14,14,13,13,11,11,10,10, 8, 8, 7, 7, 6, 6, 6, 6, 4, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}, /* druid */
{
  {11,10,10,10, 9, 9, 9, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 2, 2, 2, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
  {16,14,14,14,13,13,13,11,11,11,10,10,10, 9, 9, 9, 8, 8, 8, 6, 6, 5, 4, 3, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0},
  {15,13,13,13,12,12,12,10,10,10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 5, 5, 4, 3, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {18,16,16,16,15,15,15,13,13,13,12,12,12,11,11,11,10,10,10, 8, 8, 7, 6, 5, 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {17,15,15,15,14,14,14,12,12,12,11,11,11,10,10,10, 9, 9, 9, 7, 7, 6, 5, 4, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}, 
{ /* monk */
  {16,14,14,13,13,11,11,10,10, 8, 8, 7, 7, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {18,16,16,15,15,13,13,12,12,10,10, 9, 9, 7, 7, 6, 6, 5, 5, 5, 5, 4, 3, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {17,15,15,14,14,12,12,11,11, 9, 9, 8, 8, 6, 6, 5, 5, 4, 4, 4, 4, 3, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {20,17,17,16,16,13,13,12,12, 9, 9, 8, 8, 5, 5, 4, 4, 4, 4, 4, 4, 3, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {19,17,17,16,16,14,14,13,13,11,11,10,10, 8, 8, 7, 7, 6, 6, 6, 6, 4, 2, 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}

};


void spello(int nr, byte beat, byte pos, byte mlev, byte clev, byte dlev, 
  ubyte mana, sh_int tar, void *func, sh_int sf)
{
  skill_info[nr].spell_pointer = func;
  skill_info[nr].beats = beat;
  skill_info[nr].minimum_position = pos;
  skill_info[nr].min_usesmana = mana;
  skill_info[nr].min_level[MIN_LEVEL_CLERIC] = clev; /* Kiku's changes to */
  skill_info[nr].min_level[MIN_LEVEL_MAGIC] = mlev;  /* the skill_data    */
  skill_info[nr].min_level[MIN_LEVEL_DRUID] = dlev; 
  skill_info[nr].targets = tar;
  skill_info[nr].spellfail = sf;
  skill_info[nr].percent = 60;
}


int SPELL_LEVEL(struct char_data *ch, int sn) 
{
  int min;

  min = ABS_MAX_LVL;

  if (HasClass(ch, CLASS_MAGIC_USER))
    min = MIN(min, skill_info[sn].min_level[MIN_LEVEL_MAGIC]);


  if (HasClass(ch, CLASS_CLERIC))
    min = MIN(min, skill_info[sn].min_level[MIN_LEVEL_CLERIC]);


  if (HasClass(ch, CLASS_DRUID))
    min = MIN(min, skill_info[sn].min_level[MIN_LEVEL_DRUID]);


  return(min);

}


void affect_update( int pulse )
{
  static struct affected_type *af, *next_af_dude;
  register struct char_data *i;
  register struct obj_data *j;
  struct obj_data *next_thing;
  struct char_data  *next_char;
  struct room_data *rp;
  int dead=FALSE, room, k;
  
  extern struct time_info_data time_info;
  
  
  void update_char_objects( struct char_data *ch ); /* handler.c */
  void do_save(struct char_data *ch, char *arg, int cmd); /* act.other.c */
  
  
  for (i = character_list; i; i = next_char) {
    next_char = i->next;
    /*
     *  check the effects on the char
     */
    dead=FALSE;
    for (af = i->affected; af&&!dead; af = next_af_dude) {
      next_af_dude = af->next;
      if (af->duration >= 1) {
	af->duration--;

	if (af->duration == 1 && af->location != APPLY_INTRINSIC) {
	  SpellWearOffSoon(af->type, i);
	}

      } else {
	/* It must be a spell */
	if ((af->type > 0) && (af->type < FIRST_BREATH_WEAPON)) { 
	  if (!af->next || (af->next->type != af->type) ||
	      (af->next->duration > 0)) {

	    k = af->type;

	    if(af->location != APPLY_INTRINSIC)
	      SpellWearOff(k, i);
	    affect_remove(i, af);

	  }
	} else if (af->type>=FIRST_BREATH_WEAPON &&
		   af->type <=LAST_BREATH_WEAPON ) {
	  extern funcp bweapons[];
	  bweapons[af->type-FIRST_BREATH_WEAPON](-af->modifier/2, i, "",
						 SPELL_TYPE_SPELL, i, 0);
	  if (!i->affected) {
	    /* oops, you're dead :) */
	    dead = TRUE;
	    break;
	  }
	  affect_remove(i, af);
	}
      }
    }
    if (!dead) {

      if (GET_POS(i) >= POSITION_STUNNED && (i->desc || !IS_PC(i))) {
	/* note - because of poison, this one has to be in the 
	   opposite order of the others.  The logic:

	   hit_gain() modifies the characters hps if they are poisoned.
	   but if they were in the opposite order,
	   the total would be: hps before poison + gain.  But of course,
           the hps after poison are lower, but No one cares!
	   and that is why the gain is added to the hits, not vice versa
	   */
        GET_HIT(i)  = MIN(hit_gain(i) + GET_HIT(i),  hit_limit(i));
        GET_MANA(i) = MIN(GET_MANA(i) + mana_gain(i), mana_limit(i));
        GET_MOVE(i) = MIN(GET_MOVE(i) + move_gain(i), move_limit(i));
        if (GET_POS(i) == POSITION_STUNNED) 
	  update_pos( i );
      } else if (GET_POS(i) == POSITION_INCAP) {
        /* do nothing */  damage(i, i, 0, TYPE_SUFFERING);
      } else if (!IS_NPC(i) && (GET_POS(i) == POSITION_MORTALLYW)) {
	damage(i, i, 1, TYPE_SUFFERING);
      }

      if (IS_PC(i))	{
        update_char_objects(i);
        if (GetMaxLevel(i) < DEMIGOD && i->in_room != 3 && i->in_room != 2)
	  check_idling(i);
        rp = real_roomp(i->in_room);
	if (rp) {
	  if (rp->sector_type == SECT_WATER_SWIM ||
	      rp->sector_type == SECT_WATER_NOSWIM) {
	    if(GET_RACE(i) == RACE_HALFLING) {
	      gain_condition(i,FULL,-2);
	      gain_condition(i,DRUNK,-2);
	    } else if(GET_RACE(i) == RACE_OGRE) {
	      gain_condition(i,FULL,-3);
	      gain_condition(i,DRUNK,-2);
	    } else {
	      gain_condition(i,FULL,-1);
	      gain_condition(i,DRUNK,-1);
	    }
	  } else if (rp->sector_type == SECT_DESERT) {
	    if(GET_RACE(i) == RACE_HALFLING) {
	      gain_condition(i,FULL,-2);
	      gain_condition(i,DRUNK,-3);
	      gain_condition(i,THIRST,-3);
	    } else if(GET_RACE(i) == RACE_OGRE) {
              gain_condition(i,FULL,-3);
              gain_condition(i,DRUNK,-2);
	      gain_condition(i,THIRST,-3);
            } else {
	      gain_condition(i,FULL,-1);
	      gain_condition(i,DRUNK,-2);
	      gain_condition(i,THIRST,-2);
	    }
	  } else if (rp->sector_type == SECT_MOUNTAIN ||
		     rp->sector_type == SECT_HILLS) {
            if(GET_RACE(i) == RACE_HALFLING) {
	      gain_condition(i,FULL,-3);
	      gain_condition(i,DRUNK,-3);
	    } else if(GET_RACE(i) == RACE_OGRE) {
              gain_condition(i,FULL,-3);
              gain_condition(i,DRUNK,-3);
            } else {
	      gain_condition(i,FULL,-2);
	      gain_condition(i,DRUNK,-2);
	    }
	  } else {
            if(GET_RACE(i) == RACE_HALFLING) {
	      gain_condition(i,FULL,-2);
	      gain_condition(i,DRUNK,-2);
	      gain_condition(i,THIRST,-2);
	    } else if(GET_RACE(i) == RACE_OGRE) {
              gain_condition(i,FULL,-3);
              gain_condition(i,DRUNK,-3);
              gain_condition(i,THIRST,-3);
            } else {
	      gain_condition(i,FULL,-1);
	      gain_condition(i,DRUNK,-1);
	      gain_condition(i,THIRST,-1);
	    }
	  }
	}
        if (i->specials.tick == time_info.hours) {/* works for 24, change for
						     anything else        */
	  if (!IS_IMMORTAL(i) && i->in_room != 3)  /* the special case for room
						      3 is a hack to keep
						      link dead people who
						      have no stuff from
						      being saved without
						      stuff...  */
  	     do_save(i,"",0);
        }
      }
      check_nature(i);  /* check falling, check drowning, etc */
    }
  }
  
  /*
   *  update the objects
   */
  
  for(j = object_list; j ; j = next_thing){
    next_thing = j->next; /* Next in object list */
    
    /* If this is a corpse */
    if ( (GET_ITEM_TYPE(j) == ITEM_CONTAINER) && 
	(j->obj_flags.value[3]) ) {
      /* timer count down */
      if (j->obj_flags.timer > 0) j->obj_flags.timer--;
      
      if (!j->obj_flags.timer) {
	if (j->carried_by)
      act("$p biodegrades in your hands. Everything in it falls to the floor", 
	      FALSE, j->carried_by, j, 0, TO_CHAR);
	else if ((j->in_room != NOWHERE) && 
		 (real_roomp(j->in_room)->people)){
	  act("$p dissolves into a fertile soil.",
	      TRUE, real_roomp(j->in_room)->people, j, 0, TO_ROOM);
	  act("$p dissolves into a fertile soil.",
	      TRUE, real_roomp(j->in_room)->people, j, 0, TO_CHAR);
	}	
	ObjFromCorpse(j);
      }
    } else {
      
      /*
       *  Sound objects
       */
      if (ITEM_TYPE(j) == ITEM_AUDIO) {
	if (((j->obj_flags.value[0]) && 
	     (pulse % j->obj_flags.value[0])==0) ||
	    (!number(0,5))) {
	  if (j->carried_by) {
	    room = j->carried_by->in_room;
	  } else if (j->equipped_by) {
	    room = j->equipped_by->in_room;
	  } else if (j->in_room != NOWHERE) {
	    room = j->in_room;
	  } else {
	    room = RecGetObjRoom(j);
	  }
	  /*
	   *  broadcast to room
	   */
	  
	  if (j->action_description) {	  
	    MakeNoise(room, j->action_description, j->action_description);
	  }
	}
      } else {
	if (obj_index[j->item_number].func) {
	  (*obj_index[j->item_number].func)(0, 0, 0, j, PULSE_TICK);
	}
      }
    }    
  }
}  


void clone_char(struct char_data *ch)
{
  send_to_char("Nosir, i don't like it\n\r", ch);
}



void clone_obj(struct obj_data *obj)
{

}



/* Check if making CH follow VICTIM will create an illegal */
/* Follow "Loop/circle"                                    */
bool circle_follow(struct char_data *ch, struct char_data *victim)
{
  struct char_data *k;

  for(k=victim; k; k=k->master) {
    if (k == ch)
      return(TRUE);
  }
  
  return(FALSE);
}



/* Called when stop following persons, or stopping charm */
/* This will NOT do if a character quits/dies!!          */
void stop_follower(struct char_data *ch)
{
  struct follow_type *j, *k;

  if (!ch->master) return;

  if (IS_AFFECTED(ch, AFF_CHARM)) {
    act("You realize that $N is a jerk!", FALSE, ch, 0, ch->master, TO_CHAR);
    act("$n realizes that $N is a jerk!", FALSE, ch, 0, ch->master, 
	TO_NOTVICT);
    act("$n hates your guts!", FALSE, ch, 0, ch->master, TO_VICT);
    if (affected_by_spell(ch, SPELL_CHARM_PERSON))
      affect_from_char(ch, SPELL_CHARM_PERSON);
  } else {
    act("You stop following $N.", FALSE, ch, 0, ch->master, TO_CHAR);
    if (!IS_SET(ch->specials.act,PLR_STEALTH)) {
      act("$n stops following $N.", FALSE, ch, 0, ch->master, TO_NOTVICT);
      act("$n stops following you.", FALSE, ch, 0, ch->master, TO_VICT);
    }
  }
  
  if (ch->master->followers->follower == ch) { /* Head of follower-list? */
    k = ch->master->followers;
    ch->master->followers = k->next;
    free(k);
  } else { /* locate follower who is not head of list */
    
    for(k = ch->master->followers; k->next && k->next->follower!=ch; 
	k=k->next)  
      ;
    
    if (k->next) {
      j = k->next;
      k->next = j->next;
      free(j);
    } else {
      assert(FALSE);
    }
  }
  
  ch->master = 0;
  REMOVE_BIT(ch->specials.affected_by, AFF_CHARM | AFF_GROUP);
}



/* Called when a character that follows/is followed dies */
void die_follower(struct char_data *ch)
{
  struct follow_type *j, *k;
  
  if (ch->master)
    stop_follower(ch);
  
  for (k=ch->followers; k; k=j) {
    j = k->next;
    stop_follower(k->follower);
  }
}



/* Do NOT call this before having checked if a circle of followers */
/* will arise. CH will follow leader                               */
void add_follower(struct char_data *ch, struct char_data *leader)
{
  struct follow_type *k;
  
  assert(!ch->master);
  
  ch->master = leader;
  
  CREATE(k, struct follow_type, 1);
  
  k->follower = ch;
  k->next = leader->followers;
  leader->followers = k;
  
  
  act("You now follow $N.", FALSE, ch, 0, leader, TO_CHAR);
  if (!IS_SET(ch->specials.act, PLR_STEALTH)) {
    act("$n starts following you.", TRUE, ch, 0, leader, TO_VICT);
    act("$n now follows $N.", TRUE, ch, 0, leader, TO_NOTVICT);
  }
}


struct syllable {
  char org[10];
  char new[10];
};

struct syllable syls[] = {
  { " ", " " },
  { "ar", "abra"   },
  { "au", "kada"    },
  { "bless", "fido" },
  { "blind", "nose" },
  { "bur", "mosa" },
  { "cu", "judi" },
  { "ca", "jedi" },
  { "de", "oculo"},
  { "en", "unso" },
  { "light", "dies" },
  { "lo", "hi" },
  { "mor", "zak" },
  { "move", "sido" },
  { "ness", "lacri" },
  { "ning", "illa" },
  { "per", "duda" },
  { "ra", "gru"   },
  { "re", "candus" },
  { "son", "sabru" },
  { "se",  "or"},
  { "tect", "bulgo" },
  { "tri", "cula" },
  { "ven", "nofo" },
  {"a", "a"},{"b","b"},{"c","q"},{"d","e"},{"e","z"},{"f","y"},{"g","o"},
  {"h", "p"},{"i","u"},{"j","y"},{"k","t"},{"l","r"},{"m","w"},{"n","i"},
  {"o", "a"},{"p","s"},{"q","d"},{"r","f"},{"s","g"},{"t","h"},{"u","j"},
  {"v", "z"},{"w","x"},{"x","n"},{"y","l"},{"z","k"}, {"",""}
};

say_spell( struct char_data *ch, int si )
{
  char buf[MAX_STRING_LENGTH], splwd[MAX_BUF_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  
  int j, offs;
  struct char_data *temp_char;
  extern struct syllable syls[];
  
  strcpy(buf, "");
  strcpy(splwd, spells[si-1]);
  
  offs = 0;
  if(si == SPELL_FIREBALL && GET_SEX(ch) == SEX_MALE) {
    sprintf(buf2,"$n utters the words, 'Hmm hmmhmm, heh heh, FIRE! FIRE! FIRE!'");
    sprintf(buf, "$n utters the words, 'Hmm hmmhmm, heh heh, FIRE! FIRE! FIRE!'");
  } else {
  
    while(*(splwd+offs)) {
      for(j=0; *(syls[j].org); j++)
	if (strncmp(syls[j].org, splwd+offs, strlen(syls[j].org))==0) {
	  strcat(buf, syls[j].new);
	  if (strlen(syls[j].org))
	    offs+=strlen(syls[j].org);
	  else
	    ++offs;
	}
    }
    
    sprintf(buf2,"$n utters the words, '%s'", buf);
    sprintf(buf, "$n utters the words, '%s'", spells[si-1]);
  }
  for(temp_char = real_roomp(ch->in_room)->people;
      temp_char;
      temp_char = temp_char->next_in_room)
    if(temp_char != ch) {
      if (GET_RACE(ch) == GET_RACE(temp_char))
	act(buf, FALSE, ch, 0, temp_char, TO_VICT);
      else
	act(buf2, FALSE, ch, 0, temp_char, TO_VICT);
      
    }
}



bool saves_spell(struct char_data *ch, sh_int save_type)
{
  int save;
  
  /* Negative apply_saving_throw makes saving throw better! */
  
  save = ch->specials.apply_saving_throw[save_type];

  if(GET_RACE(ch) == RACE_DWARF || GET_RACE(ch) == RACE_GNOME || GET_RACE(ch) 
     == RACE_HALFLING ) {
    save -= con_app[GET_CON(ch)].hitp;
  }
  
  if (!IS_NPC(ch)) {
    
    save += saving_throws[BestMagicClass(ch)][save_type][GET_LEVEL(ch,BestMagicClass(ch))];
    if (GetMaxLevel(ch) > MAX_MORT)
      return(TRUE);
  }

  if (GET_RACE(ch) == RACE_GOD)   /* gods always save */
    return(1);
  
  return(MAX(1,save) < number(1,20));
}

bool ImpSaveSpell(struct char_data *ch, sh_int save_type, int mod)
{
  int save;
  
  /* Positive mod is better for save */
  
  /* Negative apply_saving_throw makes saving throw better! */
  
  save = ch->specials.apply_saving_throw[save_type] - mod;
  
  if (!IS_NPC(ch)) {
    
    save += saving_throws[BestMagicClass(ch)][save_type]
      [GET_LEVEL(ch,BestMagicClass(ch))];
    if (GetMaxLevel(ch) >= LOW_IMMORTAL)
      return(TRUE);
  }
  
  return(MAX(1,save) < number(1,20));
}



char *skip_spaces(char *string)
{
  for(;*string && (*string)==' ';string++);

  return(string);
}



/* Assumes that *argument does start with first letter of chopped string */

void do_cast(struct char_data *ch, char *argument, int cmd)
{
  struct obj_data *tar_obj;
  struct char_data *tar_char;
  char name[MAX_INPUT_LENGTH];
  int qend, spl, i;
  int spell_class, max, cost;
  bool target_ok, intrinsic = FALSE;
  
  if (IS_NPC(ch) && (!IS_SET(ch->specials.act, ACT_POLYSELF)))
    return;

  if (!IsHumanoid(ch)) {
    send_to_char("Sorry, you don't have the right form for that.\n\r",ch);
    return;
  }
  
  if (apply_soundproof(ch))
     return;

  if(GET_MANA(ch) < 1 && GetMaxLevel(ch) < LOW_IMMORTAL) {
    send_to_char("You are much too fatigued to cast right now.\n\r",ch);
    return;
  }

  argument = skip_spaces(argument);
  
  /* If there is no chars in argument */
  if (!(*argument)) {
    send_to_char("Cast which what where?\n\r", ch);
    return;
  }
  
  if (*argument != '\'') {
    send_to_char("Magic must always be enclosed by the holy magic symbols : '\n\r",ch);
    return;
  }
  
  /* Locate the last quote && lowercase the magic words (if any) */
  
  for (qend=1; *(argument+qend) && (*(argument+qend) != '\'') ; qend++)
    *(argument+qend) = LOWER(*(argument+qend));
  
  if (*(argument+qend) != '\'') {
    send_to_char("Magic must always be enclosed by the holy magic symbols : '\n\r",ch);
    return;
  }
  
  spl = old_search_block(argument, 1, qend-1,spells, 0);
  
  if (!spl) {
    send_to_char("Your lips do not move, no magic appears.\n\r",ch);
    return;
  }

  if(IsIntrinsic(ch, spl)) 
    if(CastIntrinsic(ch, spl)) 
      intrinsic = TRUE;


  if (!IS_IMMORTAL(ch) && !intrinsic) {
    if (BestMagicClass(ch) == WARRIOR_LEVEL_IND) {
      if(IsIntrinsic(ch,spl)) {
	send_to_char("You are unable to draw upon your innate abilities.\n\r",
		     ch);
      } else 
	send_to_char("You wave your hands about and grunt to no avail.\n\r",
		     ch);
      return;
    } else if (BestMagicClass(ch) == THIEF_LEVEL_IND) {
      if(IsIntrinsic(ch,spl)) {
        send_to_char("You are unable to draw upon your innate abilities.\n\r",
                     ch);
      } else
	send_to_char("You make weird noises and hop about to no avail.\n\r", 
		     ch);
      return;
    } else if (BestMagicClass(ch) == MONK_LEVEL_IND) {
      if(IsIntrinsic(ch,spl)) {
        send_to_char("You are unable to draw upon your innate abilities.\n\r",
                     ch);
      } else {
	send_to_char("You wave your hands and feet furiously in the air.\n\r",
		     ch);
	send_to_char("Bummer, you aren't a mage.\n\r",ch);
      }
      return;
    }
  }


  if (!ch->skills)
    return;
  
  if ((spl > 0) && (spl < MAX_SKILLS) && skill_info[spl].spell_pointer) {
    if (GET_POS(ch) < skill_info[spl].minimum_position) {
      switch(GET_POS(ch)) {
      case POSITION_SLEEPING :
	send_to_char("You dream about great magical powers.\n\r", ch);
	break;
      case POSITION_RESTING :
	send_to_char("You can't concentrate enough while resting.\n\r",ch);
	break;
      case POSITION_SITTING :
	send_to_char("You can't do this sitting!\n\r", ch);
	break;
      case POSITION_FIGHTING :
	send_to_char("Impossible! You can't concentrate enough!.\n\r", ch);
	break;
      default:
	send_to_char("It seems like you're in pretty bad shape!\n\r",ch);
	break;
      } /* Switch */
    }	else {
      
      if (!IS_IMMORTAL(ch) && !intrinsic) {
	if ((skill_info[spl].min_level[MIN_LEVEL_MAGIC] > 
	     GET_LEVEL(ch,MAGE_LEVEL_IND)) &&
	    (skill_info[spl].min_level[MIN_LEVEL_CLERIC] >
	     GET_LEVEL(ch,CLERIC_LEVEL_IND)) &&
	    (skill_info[spl].min_level[MIN_LEVEL_DRUID] >
	     GET_LEVEL(ch, DRUID_LEVEL_IND))) {
	  send_to_char("Sorry, you can't do that.\n\r", ch);
	  return;
	}
      }
      argument+=qend+1;	/* Point to the last ' */
      for(;*argument == ' '; argument++);
      
      /* **************** Locate targets **************** */
      
      target_ok = FALSE;
      tar_char = 0;
      tar_obj = 0;
      
      if (IS_SET(skill_info[spl].targets, TAR_VIOLENT) &&
	  check_peaceful(ch, "Impolite magic is banned here."))
	return;

      if (!IS_SET(skill_info[spl].targets, TAR_IGNORE)) {
	
	argument = one_argument(argument, name);
	
	if (*name) {
	  if (IS_SET(skill_info[spl].targets, TAR_CHAR_ROOM)) {
	    if (tar_char = get_char_room_vis(ch, name)) {
	      if (tar_char == ch || tar_char == ch->specials.fighting ||
		  tar_char->attackers < 6 || 
		  tar_char->specials.fighting == ch)
		target_ok = TRUE;
	      else {
		send_to_char("Too much fighting, you can't get a clear shot.\n\r", ch);
		target_ok = FALSE;
	      }
	    } else {
	      target_ok = FALSE;
	    }
	  }
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_CHAR_WORLD))
	    if (tar_char = get_char_vis(ch, name))
	      target_ok = TRUE;
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_OBJ_INV))
	    if (tar_obj = get_obj_in_list_vis(ch, name, ch->carrying))
	      target_ok = TRUE;
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_OBJ_ROOM))
	    if (tar_obj = get_obj_in_list_vis(ch, name, real_roomp(ch->in_room)->contents))
	      target_ok = TRUE;
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_OBJ_WORLD))
	    if (tar_obj = get_obj_vis(ch, name))
	      target_ok = TRUE;
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_OBJ_EQUIP)) {
	    for(i=0; i<MAX_WEAR && !target_ok; i++)
	      if (ch->equipment[i] && str_cmp(name, ch->equipment[i]->name) == 0) {
		tar_obj = ch->equipment[i];
		target_ok = TRUE;
	      }
	  }
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_SELF_ONLY))
	    if (str_cmp(GET_NAME(ch), name) == 0) {
	      tar_char = ch;
	      target_ok = TRUE;
	    }
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_NAME)) {
	    tar_obj = (void*)name;
	    target_ok = TRUE;
	  }
	  
	  if (tar_char) {
	    if (IS_NPC(tar_char)) 
	      if (IS_SET(tar_char->specials.act, ACT_IMMORTAL)) {
		send_to_char("You can't cast magic on that!",ch);
		return;
	      }
	  }
	  
	  
	} else { /* No argument was typed */
	  
	  if (IS_SET(skill_info[spl].targets, TAR_FIGHT_SELF))
	    if (ch->specials.fighting) {
	      tar_char = ch;
	      target_ok = TRUE;
	    }
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_FIGHT_VICT))
	    if (ch->specials.fighting) {
	      /* WARNING, MAKE INTO POINTER */
	      tar_char = ch->specials.fighting;
	      target_ok = TRUE;
	    }
	  
	  if (!target_ok && IS_SET(skill_info[spl].targets, TAR_SELF_ONLY)) {
	    tar_char = ch;
	    target_ok = TRUE;
	  }
	  
	}
	
      } else {
	target_ok = TRUE; /* No target, is a good target */
      }
      
      if (!target_ok) {
	if (*name) {
	  if (IS_SET(skill_info[spl].targets, TAR_CHAR_WORLD))
	    send_to_char("Nobody playing by that name.\n\r", ch);
	  else if (IS_SET(skill_info[spl].targets, TAR_CHAR_ROOM))
	    send_to_char("Nobody here by that name.\n\r", ch);
	  else if (IS_SET(skill_info[spl].targets, TAR_OBJ_INV))
	    send_to_char("You are not carrying anything like that.\n\r", ch);
	  else if (IS_SET(skill_info[spl].targets, TAR_OBJ_ROOM))
	    send_to_char("Nothing here by that name.\n\r", ch);
	  else if (IS_SET(skill_info[spl].targets, TAR_OBJ_WORLD))
	    send_to_char("Nothing at all by that name.\n\r", ch);
	  else if (IS_SET(skill_info[spl].targets, TAR_OBJ_EQUIP))
	    send_to_char("You are not wearing anything like that.\n\r", ch);
	  else if (IS_SET(skill_info[spl].targets, TAR_OBJ_WORLD))
	    send_to_char("Nothing at all by that name.\n\r", ch);
	  
	} else { /* Nothing was given as argument */
	  if (skill_info[spl].targets < TAR_OBJ_INV)
	    send_to_char("Who should the spell be cast upon?\n\r", ch);
	  else
	    send_to_char("What should the spell be cast upon?\n\r", ch);
	}
	return;
      } else { /* TARGET IS OK */
	if ((tar_char == ch)&&IS_SET(skill_info[spl].targets, TAR_SELF_NONO)) {
	  send_to_char("You can not cast this spell upon yourself.\n\r", ch);
	  return;
	}
	else if ((tar_char != ch) && 
		 IS_SET(skill_info[spl].targets, TAR_SELF_ONLY)) {
	  send_to_char("You can only cast this spell upon yourself.\n\r", ch);
	  return;
	} else if (IS_AFFECTED(ch, AFF_CHARM) &&(ch->master == tar_char)) {
	  send_to_char("You are afraid that it could harm your master.\n\r", 
		       ch);
	  return;
	}
      }
      
      
      if (cmd == 283) { /* recall */
	if (!MEMORIZED(ch, spl)) {
	  send_to_char("You don't have that spell memorized!\n\r", ch);
	  return;
	}
      } else {      
	if (GetMaxLevel(ch) < LOW_IMMORTAL && !intrinsic) {
	  if (GET_MANA(ch) < (unsigned int)USE_MANA(ch, (int)spl)) {
	    send_to_char("You can't summon enough energy to cast the spell.\n\r", ch);
	    return;
	  }
	}
      }

      if (spl != SPELL_VENTRILOQUATE)  /* :-) */
	say_spell(ch, spl);


      if ((skill_info[spl].spell_pointer == 0) && spl>0) {
	send_to_char("Sorry, this magic has not yet been implemented :(\n\r", 
		     ch);
	return;
      } 

      /* assume we have a valid spell & caster */

      if(HasClass(ch, CLASS_DRUID)) {
	if(skill_info[spl].min_level[MIN_LEVEL_MAGIC] < 
	   skill_info[spl].min_level[MIN_LEVEL_CLERIC] &&
	   skill_info[spl].min_level[MIN_LEVEL_MAGIC] <=
	   skill_info[spl].min_level[MIN_LEVEL_DRUID]) {
	  spell_class = CLASS_MAGIC_USER;
	} else if(skill_info[spl].min_level[MIN_LEVEL_CLERIC] <
		  skill_info[spl].min_level[MIN_LEVEL_MAGIC] &&
		  skill_info[spl].min_level[MIN_LEVEL_CLERIC] <=
		  skill_info[spl].min_level[MIN_LEVEL_DRUID]) {
	  spell_class = CLASS_CLERIC;
	} else {
	  spell_class = CLASS_DRUID;
	}
      } else {
	if(skill_info[spl].min_level[MIN_LEVEL_MAGIC] <
           skill_info[spl].min_level[MIN_LEVEL_CLERIC])
	  spell_class = CLASS_MAGIC_USER;
	else 
	  spell_class = CLASS_CLERIC;
      }
      
      if (OnlyClass(ch, CLASS_DRUID)) {
	spell_class = CLASS_DRUID;
      }

      max = ch->specials.spellfail;

      if(!IS_IMMORTAL(ch)) {
	if(spell_class == CLASS_DRUID) {
	  if(EqWBits(ch, ITEM_METAL)) {
	    send_to_char("You can't cast that spell while touching metal!\n\r",
			 ch);
	    return;
	  }
	} else {
	  int i;
	  
	  if(!intrinsic) {
	    if(spell_class == CLASS_CLERIC)
	      spell_class = ITEM_ANTI_CLERIC;
	    else if(spell_class == CLASS_MAGIC_USER)
	      spell_class = ITEM_ANTI_MAGE;
	    else 
	      logE("Logic error in sf routine(spell_parser.c), go get Ripper!");
	    for(i=0;i<MAX_WEAR;i++) {
	      if (ch->equipment[i]) {
		if(IS_SET(ch->equipment[i]->obj_flags.extra_flags,spell_class))
		  max += sf_where[i];
	      }
	    }
	  }
	}
      }
      
      WAIT_STATE(ch, skill_info[spl].beats);
      
      max += GET_COND(ch, DRUNK)*10; /* 0 - 240 */
	
      if (ch->attackers > 0)
	max += skill_info[spl].spellfail;
      else if (ch->specials.fighting)
	max += skill_info[spl].spellfail/3;

        /* Gecko Druid Spell Fail */
        if (HasClass(ch, CLASS_DRUID) && !IS_NEUTRAL(ch))
          max += MIN(650, MAX(0, abs(GET_ALIGNMENT(ch)) - 350));
      
      if (number(1,max) > (intrinsic ? 95 : ch->skills[spl].learned)) { 
	send_to_char("You lost your concentration!\n\r", ch);
	if(!intrinsic) {
	  cost = (int)USE_MANA(ch, (int)spl);
	  GET_MANA(ch) -= (cost>>1);
	  LearnFromMistake(ch, spl, 0, 95);
	  return;
	}
      }
      if (tar_char) {
	if (GET_POS(tar_char) == POSITION_DEAD) {
	  send_to_char("The magic fizzles against the dead body.\n", ch);
	  return;
	}
      }
      
      if (check_nomagic(ch, 
			"Your magical spell is destroyed by unknown forces.", 
			"$n's spell dissolves like so much wet toilet paper."))
	return;
      
	
      send_to_char("Ok.\n\r",ch);
      ((*skill_info[spl].spell_pointer) (GET_LEVEL(ch, BestMagicClass(ch)), ch, argument, SPELL_TYPE_SPELL, tar_char, tar_obj));
      cost = (int)USE_MANA(ch, (int)spl);
      if (cmd == 283) /* recall */ {
	FORGET(ch, spl);
      } else if(!intrinsic) {
	if(OUTSIDE(ch) && weather_info.sky < 2)
	  cost -= cost>>2;
	GET_MANA(ch) -= cost;
      }
    }	/* if GET_POS < min_pos */
    return;
  }

  switch (number(1,5)){
  case 1: send_to_char("Bylle Grylle Grop Gryf???\n\r", ch); break;
  case 2: send_to_char("Olle Bolle Snop Snyf?\n\r",ch); break;
  case 3: send_to_char("Olle Grylle Bolle Bylle?!?\n\r",ch); break;
  case 4: send_to_char("Gryffe Olle Gnyffe Snop???\n\r",ch); break;
  default: send_to_char("Bolle Snylle Gryf Bylle?!!?\n\r",ch); break;
  }
}


void assign_spell_pointers()
{
  int i;
  
  for(i=0; i<MAX_SPL_LIST; i++) {
    skill_info[i].spell_pointer = 0;
  }
  
  
  /* From spells1.c */

/* mage only */  
  spello(32,12,POSITION_FIGHTING, 1, LOW_IMMORTAL, LOW_IMMORTAL, 15, 
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_magic_missile,10);
  
  spello( 8,12,POSITION_FIGHTING, 4, LOW_IMMORTAL,  LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_chill_touch,10);
  
  spello( 5,24,POSITION_FIGHTING, 6, LOW_IMMORTAL,  LOW_IMMORTAL, 15,
	 TAR_IGNORE | TAR_VIOLENT, cast_burning_hands,5);
  
  spello(37,12,POSITION_FIGHTING, 2, LOW_IMMORTAL,  LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM|TAR_FIGHT_VICT|TAR_VIOLENT, cast_shocking_grasp,0);

  
  spello(30,24,POSITION_FIGHTING, 10, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT,cast_lightning_bolt,20);

  spello(10,24,POSITION_FIGHTING, 14,LOW_IMMORTAL, LOW_IMMORTAL, 20, 
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_colour_spray,40);

  spello(26,36,POSITION_FIGHTING, 25, LOW_IMMORTAL,  LOW_IMMORTAL, 50,
	 TAR_IGNORE | TAR_VIOLENT, cast_fireball,25);

/* cleric */
  spello(23,24,POSITION_FIGHTING, LOW_IMMORTAL, 16, LOW_IMMORTAL, 15,
	 TAR_IGNORE | TAR_VIOLENT, cast_earthquake,30);
    
  spello(22,24,POSITION_FIGHTING, LOW_IMMORTAL, 20,  LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_dispel_evil, 30);
  
  
  spello( 6,36,POSITION_FIGHTING, LOW_IMMORTAL, 45, 18, 20,
	 TAR_CHAR_ROOM|TAR_FIGHT_VICT|TAR_VIOLENT, cast_call_lightning, 50);
  
  spello(25,36,POSITION_FIGHTING, 22, LOW_IMMORTAL, LOW_IMMORTAL, 35, 
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_energy_drain, 100);
  
  spello(27,36,POSITION_FIGHTING, LOW_IMMORTAL, 25,  LOW_IMMORTAL, 35,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_harm, 510);
  
  
  /* Spells2.c */
  
  spello( 1,12,POSITION_STANDING, LOW_IMMORTAL,  2, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM, cast_armor, 0);
  
  spello( 2,12,POSITION_STANDING, 17, LOW_IMMORTAL,  LOW_IMMORTAL, 33,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_teleport, 60);
  
  spello( 3,12,POSITION_STANDING,LOW_IMMORTAL,  1,  LOW_IMMORTAL, 5,
	 TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_CHAR_ROOM, cast_bless, 0);
  
  spello( 4,24,POSITION_FIGHTING, 12,  14,  LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_blindness, 60);
  
  spello(7,12,POSITION_STANDING, 4, LOW_IMMORTAL,  IMMORTAL, 10,
	 TAR_CHAR_ROOM | TAR_SELF_NONO | TAR_FIGHT_VICT | TAR_VIOLENT,
	 cast_charm_person, 0);
  
  /* */	spello( 9,12,POSITION_STANDING, LOKI+1, LOKI+1, LOKI+1,	40, 
	       TAR_CHAR_ROOM, cast_clone, 0);
  
  spello(11,36,POSITION_STANDING, LOW_IMMORTAL, 26, 15, 25,
	 TAR_IGNORE, cast_control_weather,0);
  
  spello(12,12,POSITION_STANDING,LOW_IMMORTAL,  5, IMMORTAL, 5,
	 TAR_IGNORE, cast_create_food,0); 
  
  spello(13,12,POSITION_STANDING,LOW_IMMORTAL,  2, IMMORTAL, 5,
	 TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_create_water,0);
  
  spello(14,12,POSITION_STANDING,LOW_IMMORTAL,  6, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM, cast_cure_blind,0);
  
  spello(15,24,POSITION_FIGHTING,LOW_IMMORTAL,  10, 13, 11,
	 TAR_CHAR_ROOM, cast_cure_critic, 20);
  
  spello(16,12,POSITION_FIGHTING,LOW_IMMORTAL,  1, 2, 5,
	 TAR_CHAR_ROOM, cast_cure_light, 10);
  
  spello(17,24,POSITION_STANDING,13, 12, LOW_IMMORTAL, 20,
	 TAR_CHAR_ROOM | TAR_OBJ_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_FIGHT_VICT | TAR_VIOLENT, cast_curse, 5);
  
  spello(18,12,POSITION_STANDING, LOW_IMMORTAL, 1, 6, 5,
	 TAR_CHAR_ROOM, cast_detect_evil,0);
  
  spello(19,12,POSITION_STANDING, 2,  5, 7, 5,
	 TAR_CHAR_ROOM, cast_detect_invisibility,0);
  
  spello(20,12,POSITION_STANDING, 1,  3, 5, 5,
	 TAR_CHAR_ROOM, cast_detect_magic,0);
  
  spello(21,12,POSITION_STANDING,LOW_IMMORTAL,  LOW_IMMORTAL, 1, 5,
	 TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_detect_poison,0);
  
  spello(24,48,POSITION_STANDING,14, 16, LOW_IMMORTAL, 100,
	 TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_enchant_weapon,0);
  
  spello(28,12,POSITION_FIGHTING,LOW_IMMORTAL, 25, LOW_IMMORTAL, 50,
	 TAR_CHAR_ROOM, cast_heal,310);
  
  spello(29,12,POSITION_STANDING, 4, LOW_IMMORTAL, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM | TAR_OBJ_EQUIP, cast_invisibility, 0);
  
  spello(31,12,POSITION_STANDING, LOW_IMMORTAL, 11, LOW_IMMORTAL, 20,
	 TAR_OBJ_WORLD, cast_locate_object, 0);
  
  spello(33,24,POSITION_FIGHTING,LOW_IMMORTAL,  13, 8, 10,
	 TAR_CHAR_ROOM | TAR_SELF_NONO | TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_FIGHT_VICT | TAR_VIOLENT, cast_poison, 60);
  
  spello(34,12,POSITION_STANDING,LOW_IMMORTAL,  7, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM, cast_protection_from_evil, 0);
  
  spello(35,12,POSITION_STANDING,LOW_IMMORTAL, 8, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_OBJ_ROOM, cast_remove_curse, 0);
  
  spello(36,36,POSITION_STANDING, LOW_IMMORTAL, 26, LOW_IMMORTAL, 50,
	 TAR_CHAR_ROOM, cast_sanctuary, 0);
  
  spello(38,24,POSITION_STANDING, 3, LOW_IMMORTAL, 5, 15,
	 TAR_CHAR_ROOM | TAR_VIOLENT |TAR_FIGHT_VICT, cast_sleep, 0);
  
  spello(39,12,POSITION_STANDING, 6, LOW_IMMORTAL, LOW_IMMORTAL, 10,
	 TAR_CHAR_ROOM, cast_strength, 0);
  
  spello(40,36,POSITION_STANDING, 27,  19, 30, 20,
	 TAR_CHAR_WORLD, cast_summon, 0);
  
  /* */	spello(41,12,POSITION_STANDING, 1, LOW_IMMORTAL, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM | TAR_OBJ_ROOM | TAR_SELF_NONO, cast_ventriloquate, 0);
  
  spello(42,12,POSITION_STANDING,LOW_IMMORTAL, 15, 27, 5,
	 TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_word_of_recall, 0);
  
  spello(43,12,POSITION_STANDING,LOW_IMMORTAL,  17, 8, 5,
	 TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, cast_remove_poison,0);
  
  spello(44,12,POSITION_STANDING,LOW_IMMORTAL,  4, LOW_IMMORTAL, 5,
	 TAR_CHAR_ROOM, cast_sense_life,0);
#if 0				/* new skill stuff */

  spello(45,0,POSITION_STANDING,LOKI+1,LOKI+1, LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(46,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(47,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(48,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(49,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(50,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(51,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1,200,
	 TAR_IGNORE, 0, 0);
  spello(52,0,POSITION_STANDING,LOKI+1,LOKI+1,LOKI+1, 200,
	 TAR_IGNORE, 0, 0);
#endif  
  spello(53,1,POSITION_STANDING,IMMORTAL,IMMORTAL, IMMORTAL, 100, 
	 TAR_CHAR_ROOM | TAR_OBJ_ROOM | TAR_OBJ_INV, cast_identify, 0);
  
  spello(54,12,POSITION_STANDING, 8, LOW_IMMORTAL, 5,  7,
	 TAR_CHAR_ROOM, cast_infravision, 0); 
  
  spello(55,12,POSITION_FIGHTING, LOW_IMMORTAL,  1, 2,  8,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_cause_light, 10);
  
  spello(56,24,POSITION_FIGHTING, LOW_IMMORTAL,  10,  13, 11,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT , cast_cause_critic, 20);
  
  spello(57,36,POSITION_FIGHTING, LOW_IMMORTAL , 15, LOW_IMMORTAL, 20,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_flamestrike, 50);
  
  spello(58,36,POSITION_FIGHTING, LOW_IMMORTAL, 20, LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_dispel_good, 50);
  
  spello(59,12,POSITION_FIGHTING, 6, LOW_IMMORTAL, LOW_IMMORTAL, 10,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_weakness, 30);
  
  spello(60,12,POSITION_FIGHTING, 9, 10, 9, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_OBJ_ROOM | TAR_OBJ_INV |
	 TAR_VIOLENT , cast_dispel_magic, 50);
  
  spello(61,12,POSITION_STANDING, 3, LOW_IMMORTAL, LOW_IMMORTAL, 10,
	 TAR_IGNORE, cast_knock, 0);
  
  spello(62,12,POSITION_STANDING, 7, 4, 2, 10,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_know_alignment, 0);
  
  spello(63,12,POSITION_STANDING, 16, 8, LOW_IMMORTAL, 5,
	 TAR_OBJ_ROOM, cast_animate_dead, 0);
  
  spello(64,36,POSITION_FIGHTING, 20, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_paralyze, 90);
  
  spello(65,12,POSITION_FIGHTING, LOW_IMMORTAL, 6, 9 , 10,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_remove_paralysis, 30);
  
  spello( 66, 12, POSITION_FIGHTING, 8, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_fear, 40);
  
  spello(67,24,POSITION_FIGHTING, 6, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_acid_blast, 20);
  
  spello(68,12,POSITION_STANDING, 9, LOW_IMMORTAL, 6, 15,
	 TAR_CHAR_ROOM, cast_water_breath, 0);
  
  spello(69,12,POSITION_STANDING, 11, 22, 14, 15,
	 TAR_CHAR_ROOM, cast_flying, 0);
  
  spello(70,24,POSITION_FIGHTING, 17, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_IGNORE | TAR_VIOLENT,  cast_cone_of_cold, 40);
  
  spello(71,24,POSITION_FIGHTING, 36, LOW_IMMORTAL, LOW_IMMORTAL, 35,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_meteor_swarm, 90);
  
  spello(72,12,POSITION_FIGHTING, 11, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_IGNORE | TAR_VIOLENT,  cast_ice_storm, 20);
  
  spello(73,24,POSITION_FIGHTING, 1, LOW_IMMORTAL, LOW_IMMORTAL, 1,
	 TAR_CHAR_ROOM, cast_shield, 15);
  
  spello(74,24,POSITION_STANDING, 4, LOW_IMMORTAL, LOW_IMMORTAL, 10,
	 TAR_IGNORE, cast_mon_sum1,0);
  
  spello(75,24,POSITION_STANDING, 7, LOW_IMMORTAL, LOW_IMMORTAL, 12,
	 TAR_IGNORE, cast_mon_sum2,0);
  
  spello(76,24,POSITION_STANDING, 9, LOW_IMMORTAL, LOW_IMMORTAL, 15,
	 TAR_IGNORE, cast_mon_sum3,0);
  
  spello(77,24,POSITION_STANDING, 12, LOW_IMMORTAL, LOW_IMMORTAL, 17,
	 TAR_IGNORE, cast_mon_sum4,0);
  
  spello(78,24,POSITION_STANDING, 15, LOW_IMMORTAL, LOW_IMMORTAL, 20,
	 TAR_IGNORE, cast_mon_sum5,0);
  
  spello(79,24,POSITION_STANDING, 18, LOW_IMMORTAL, LOW_IMMORTAL, 22,
	 TAR_IGNORE, cast_mon_sum6,0);
  
  spello(80,24,POSITION_STANDING, 22, LOW_IMMORTAL, LOW_IMMORTAL, 25,
	 TAR_IGNORE, cast_mon_sum7,0);
  
  spello(81,24,POSITION_STANDING, 40, 45, 48, 40,
	 TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_fireshield,0);
  
  spello(82,12,POSITION_STANDING, 10, LOW_IMMORTAL, 12, 5,
	 TAR_CHAR_ROOM | TAR_VIOLENT, cast_charm_monster,0);
  
  spello(83,12,POSITION_FIGHTING, 45, 7, 8, 9,
	 TAR_CHAR_ROOM, cast_cure_serious, 14);
  
  spello(84,12,POSITION_FIGHTING, 45, 7, 8, 9,
	 TAR_CHAR_ROOM | TAR_VIOLENT, cast_cause_serious, 14);
  
  spello(85,12,POSITION_STANDING, 6, 3, 4, 5,
	 TAR_CHAR_ROOM, cast_refresh,0);
  
  spello(86,12,POSITION_FIGHTING, 20, 9, 14, 5,
	 TAR_CHAR_ROOM, cast_second_wind,25);
  
  spello(87,12,POSITION_STANDING, LOW_IMMORTAL, 1, 12, 5,
	 TAR_CHAR_ROOM | TAR_SELF_NONO | TAR_VIOLENT, cast_turn,0);
  
  spello(88,24,POSITION_STANDING, 24, 23, LOW_IMMORTAL, 15,
	 TAR_IGNORE, cast_succor,0);
  
  spello(89,12,POSITION_STANDING, 1, 2, 3,  5,
	 TAR_IGNORE, cast_light,0);
  
  spello(90,24,POSITION_STANDING, 10, 26, 16, 10,
	 TAR_IGNORE, cast_cont_light,0);
  
  spello(91,24,POSITION_STANDING, 4, 2, LOW_IMMORTAL, 15,
	 TAR_CHAR_ROOM, cast_calm,0);
  
  spello(92,24,POSITION_STANDING, 26, LOW_IMMORTAL, 23, 20,
	 TAR_SELF_ONLY, cast_stone_skin,0);
  
  spello(93,24,POSITION_STANDING, 16, 13, 11, 30,
	 TAR_IGNORE, cast_conjure_elemental,0);
  
  spello(94,24,POSITION_STANDING, LOW_IMMORTAL, 19, 24, 20,
	 TAR_CHAR_ROOM, cast_true_seeing,0);
  
  spello(95,24,POSITION_STANDING, 8, LOW_IMMORTAL, LOW_IMMORTAL, 30,
	 TAR_IGNORE, cast_minor_creation,0);
  
  spello(96,12,POSITION_STANDING, 5, 4, 1, 10,
	 TAR_CHAR_ROOM | TAR_SELF_NONO | TAR_VIOLENT, cast_faerie_fire,0);
  
  spello(97,24,POSITION_STANDING, 16, 11, 10, 20,
	 TAR_IGNORE, cast_faerie_fog,0);
  
  spello(98,24,POSITION_STANDING, 30, 29, LOW_IMMORTAL, 50,
	 TAR_IGNORE, cast_cacaodemon,0);
  
  spello(99,12,POSITION_STANDING, 9, LOW_IMMORTAL, LOW_IMMORTAL, 10,
	 TAR_IGNORE, cast_poly_self,0);
    
  spello(100,12,POSITION_FIGHTING, LOKI+1, LOKI+1, LOKI+1,
	 200, TAR_IGNORE, cast_mana,0);
  
  spello( 101,12,POSITION_STANDING, LOW_IMMORTAL, 30, LOW_IMMORTAL, 30,
	 TAR_CHAR_WORLD, cast_astral_walk,0);
  
  spello( 102,36,POSITION_STANDING, LOW_IMMORTAL, 36, LOW_IMMORTAL, 33,
	 TAR_OBJ_ROOM, cast_resurrection,0);

  spello( 103,12,POSITION_STANDING, LOW_IMMORTAL, 24, 24, 40,
	 TAR_IGNORE, cast_heroes_feast,0);

  spello( 104,12,POSITION_STANDING, 24, LOW_IMMORTAL, 22, 30,
	 TAR_IGNORE, cast_fly_group,0);

  spello(105, 250, POSITION_STANDING, LOKI+1, LOKI+1,
	 LOKI+1, 200, TAR_IGNORE | TAR_VIOLENT, cast_dragon_breath,0);

  spello( 106,12,POSITION_FIGHTING, 11, LOW_IMMORTAL, LOW_IMMORTAL, 3,
	 TAR_CHAR_ROOM, cast_web,60);

  spello( 107,12,POSITION_STANDING, 12, LOW_IMMORTAL, 7, 10,
	 TAR_CHAR_ROOM, cast_minor_track,0);

  spello( 108,12,POSITION_STANDING, 20, LOW_IMMORTAL, 17, 20,
	 TAR_CHAR_ROOM, cast_major_track,0);

/* new stuff */

  spello(109,24,POSITION_STANDING, IMMORTAL, 15, 20, 30,
	 TAR_IGNORE, cast_golem,0);

  spello(110,24,POSITION_STANDING, 2, IMMORTAL, 2, 30,
	 TAR_IGNORE, cast_familiar,0);

  spello(111,24,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 30, 30,
	 TAR_IGNORE, cast_changestaff,0);

  spello(112,24,POSITION_FIGHTING, IMMORTAL, 31, IMMORTAL, 30,
	 TAR_IGNORE | TAR_VIOLENT,  cast_holyword,80);

  /* unholy word nuked and enchant armor replaced*/

  spello(113,24,POSITION_STANDING, 15, 14, IMMORTAL, 60,
	 TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_enchant_armor,0);

  spello(114,24,POSITION_FIGHTING, 23, IMMORTAL, IMMORTAL, 10,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_pword_kill,40);

  spello(115,24,POSITION_FIGHTING, 16, IMMORTAL, IMMORTAL, 10,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_pword_blind,20);

  spello(116,24,POSITION_FIGHTING, 25, IMMORTAL, 25, 30,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_chain_lightn,80);

  spello(117,24,POSITION_FIGHTING, 4, IMMORTAL, IMMORTAL, 3,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_scare,10);

  spello(118,24,POSITION_FIGHTING, LOW_IMMORTAL, 4, IMMORTAL, 5,
	 TAR_CHAR_ROOM, cast_aid,10);

  spello(119,24,POSITION_FIGHTING, LOW_IMMORTAL, 1, IMMORTAL, 3,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_command,30);
#if 0
  spello(120, 12, POSITION_STANDING, IMMORTAL, IMMORTAL, IMMORTAL, 20,
	 TAR_IGNORE, cast_change_form, 0);
#endif
  spello(121,24,POSITION_FIGHTING, 34, IMMORTAL, IMMORTAL, 30,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT |TAR_VIOLENT,cast_feeblemind,70);

  spello(122,12,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 3, 10,
	 TAR_OBJ_INV, cast_shillelagh, 0);

  spello(123,12,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 2, 10,
	 TAR_IGNORE ,cast_goodberry,0);

  spello(124,12,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 5, 10,
	 TAR_IGNORE,cast_flame_blade,0);

  spello(125,24,POSITION_FIGHTING, LOW_IMMORTAL, IMMORTAL, 23, 20,
	 TAR_CHAR_ROOM | TAR_SELF_NONO,cast_animal_growth,40);

  spello(126,24,POSITION_FIGHTING, LOW_IMMORTAL, IMMORTAL, 22, 20,
	 TAR_CHAR_ROOM | TAR_SELF_NONO,cast_insect_growth,40);

  spello(127,36,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 45, 50,
	 TAR_IGNORE,cast_creeping_death,0);

  spello(128,24,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 20, 10,
	 TAR_IGNORE,cast_commune,0);

  spello(129,24,POSITION_STANDING, IMMORTAL, LOW_IMMORTAL, 10, 15,
	 TAR_IGNORE, cast_animal_summon_1,0);
  spello(130,24,POSITION_STANDING, IMMORTAL, LOW_IMMORTAL, 15, 20,
	 TAR_IGNORE, cast_animal_summon_2,0);
  spello(131,24,POSITION_STANDING, IMMORTAL, LOW_IMMORTAL, 20, 25,
	 TAR_IGNORE, cast_animal_summon_3,0);


  spello(132,24,POSITION_STANDING, IMMORTAL, IMMORTAL, 27, 30,
	 TAR_IGNORE, cast_fire_servant,0);
  spello(133,24,POSITION_STANDING, IMMORTAL, IMMORTAL, 28, 30,
	 TAR_IGNORE, cast_earth_servant,0);
  spello(134,24,POSITION_STANDING, IMMORTAL, IMMORTAL, 29, 30,
	 TAR_IGNORE, cast_water_servant,0);
  spello(135,24,POSITION_STANDING, IMMORTAL, IMMORTAL, 30, 30,
	 TAR_IGNORE, cast_wind_servant,0);

  spello(136,36,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 26, 50,
	 TAR_OBJ_ROOM, cast_reincarnate,0);


  spello(137,12,POSITION_STANDING, IMMORTAL, LOW_IMMORTAL, 10, 5,
	 TAR_CHAR_ROOM | TAR_VIOLENT, cast_charm_veggie,0);

  spello(138,24,POSITION_FIGHTING, LOW_IMMORTAL, IMMORTAL, 15, 20,
	 TAR_CHAR_ROOM | TAR_SELF_NONO,cast_veggie_growth,40);

  spello(139,24,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 15, 30,
	 TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_tree,0);


  spello(140,24,POSITION_STANDING, LOW_IMMORTAL, IMMORTAL, 31, 15,
	 TAR_OBJ_INV | TAR_OBJ_ROOM, cast_animate_rock,0);

  spello( 141, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 8, 2,
	 TAR_SELF_ONLY, cast_tree_travel, 0);

  spello( 142, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 10, 2,
	 TAR_SELF_ONLY, cast_travelling, 0);

  spello( 143, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 4, 2,
	 TAR_CHAR_ROOM | TAR_SELF_NONO, cast_animal_friendship, 0);

  spello( 144, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 10, 10,
	 TAR_SELF_ONLY, cast_invis_to_animals, 0);

  spello( 145, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 6, 20,
	 TAR_CHAR_ROOM, cast_slow_poison, 0);

  spello( 146, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 13, 20,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_entangle, 0);

  spello( 147, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 7, 10,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_snare, 0);

  spello( 148, 12, POSITION_SITTING, 18, LOW_IMMORTAL, 11, 2,
	 TAR_IGNORE, cast_gust_of_wind, 30);

  spello( 149, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 3, 5,
	 TAR_CHAR_ROOM, cast_barkskin, 0);

  spello( 150, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 22, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_sunray, 30);

  spello( 151, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 16, 20,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT | 
	 TAR_OBJ_ROOM | TAR_OBJ_INV, cast_warp_weapon, 60);

  spello( 152, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 18, 30,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_heat_stuff, 30);

  spello( 153, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 15, 10,
	 TAR_SELF_ONLY, cast_find_traps, 0);

  spello( 154, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 22, 15,
	 TAR_IGNORE, cast_firestorm, 30);

  spello( 155, 12, POSITION_STANDING, 23, IMMORTAL, IMMORTAL, 20, 
	 TAR_CHAR_ROOM | TAR_VIOLENT, cast_haste, 0);

  spello( 156, 12, POSITION_STANDING, 19, IMMORTAL, IMMORTAL, 20, 
	 TAR_CHAR_ROOM | TAR_VIOLENT, cast_slow, 0);

  spello(157,24,POSITION_STANDING, LOW_IMMORTAL, 3, 1, 10,
	 TAR_IGNORE, cast_dust_devil,0);

  spello(158, 12, POSITION_FIGHTING, 9, IMMORTAL, 12, 20,
	 TAR_SELF_NONO| TAR_CHAR_ROOM | TAR_FIGHT_VICT, 
	 cast_know_monster, 50);

  spello( 159, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 10, 20,
	 TAR_OBJ_WORLD, cast_transport_via_plant, 0);

  spello( 160, 12, POSITION_STANDING, LOW_IMMORTAL, LOW_IMMORTAL, 7, 5,
	 TAR_OBJ_ROOM, cast_speak_with_plants, 0);

  spello( 161, 12, POSITION_FIGHTING, 21, 23, 25, 30,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_silence, 100);

  spello( 162, 12, POSITION_STANDING, LOKI, LOKI, LOKI, 20,
	 TAR_CHAR_WORLD, cast_sending, 0);
  spello( 163, 12, POSITION_STANDING, LOKI, LOKI, LOKI, 20,
	 TAR_CHAR_WORLD, cast_teleport_wo_error, 0);
  spello( 164, 12, POSITION_STANDING, 43, LOKI, LOKI, 50,
	 TAR_CHAR_WORLD, cast_portal, 0);

  spello( 165, 12, POSITION_STANDING, LOKI, LOKI, LOKI, 20,
	 TAR_IGNORE, cast_dragon_ride, 0);
  spello( 166, 12, POSITION_STANDING, LOKI, LOKI, LOKI, 20,
	 TAR_IGNORE, cast_mount, 0);

  spello( 167, 12, POSITION_FIGHTING, LOW_IMMORTAL, LOW_IMMORTAL, 1, 15,
	 TAR_CHAR_ROOM | TAR_FIGHT_VICT | TAR_VIOLENT, cast_thorn_spray, 10);
    spello(206,24,POSITION_STANDING, LOW_IMMORTAL, 12, LOW_IMMORTAL, 20,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_hold,0);
  spello(207,24,POSITION_STANDING, LOW_IMMORTAL, 21, LOW_IMMORTAL, 25,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_electricity,0);
  spello(208,24,POSITION_STANDING, LOW_IMMORTAL, 27, LOW_IMMORTAL, 30,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_cold,0);
  spello(209,24,POSITION_STANDING, LOW_IMMORTAL, 28, LOW_IMMORTAL, 35,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_drain,0);
  spello(210,24,POSITION_STANDING, LOW_IMMORTAL, 18, LOW_IMMORTAL, 40,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_poison,0);
  spello(211,24,POSITION_STANDING, LOW_IMMORTAL, 29, LOW_IMMORTAL, 45,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_acid,0);
  spello(212,36,POSITION_STANDING, LOW_IMMORTAL, 41, LOW_IMMORTAL, 50,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_fire,0);
  spello(213,36,POSITION_STANDING, LOW_IMMORTAL, 39, LOW_IMMORTAL, 55,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_energy,0);
  spello(214,36,POSITION_STANDING, LOW_IMMORTAL, 33, LOW_IMMORTAL, 60,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_pierce,0);
  spello(215,36,POSITION_STANDING, LOW_IMMORTAL, 47, LOW_IMMORTAL, 65,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_slash,0);
  spello(216,36,POSITION_STANDING, LOW_IMMORTAL, 48, LOW_IMMORTAL, 70,
         TAR_SELF_ONLY | TAR_CHAR_ROOM, cast_resist_blunt,0);
}


void SpellWearOffSoon(int s, struct char_data *ch)
{

  if (s > MAX_SKILLS+10) 
    return;

  if (spell_wear_off_soon_msg[s] && *spell_wear_off_soon_msg[s]) {
    send_to_char(spell_wear_off_soon_msg[s], ch);
    send_to_char("\n\r", ch);
  }

  if (spell_wear_off_soon_room_msg[s] && *spell_wear_off_soon_room_msg[s]) {
    act(spell_wear_off_soon_room_msg[s], FALSE, ch, 0, 0, TO_ROOM);
  }

}


void SpellWearOff(int s, struct char_data *ch)
{

  if (s > MAX_SKILLS+10) 
    return;

  if (spell_wear_off_msg[s] && *spell_wear_off_msg[s]) {
    send_to_char(spell_wear_off_msg[s], ch);
    send_to_char("\n\r", ch);
  }

  if (spell_wear_off_room_msg[s] && *spell_wear_off_room_msg[s]) {
    act(spell_wear_off_room_msg[s], FALSE, ch, 0, 0, TO_ROOM);
  }


  if (s == SPELL_CHARM_PERSON || s == SPELL_CHARM_MONSTER) {
    check_decharm(ch);
  }

  if (s == SPELL_FLY) {
    check_falling(ch);
  }

  if (s == SPELL_WATER_BREATH) {
    check_drowning(ch);
  }

}

void check_decharm( struct char_data *ch)
{
  struct char_data *m;

  if (!ch->master) return;

  m = ch->master;
  stop_follower(ch);   /* stop following the master */
  REMOVE_BIT(ch->specials.act, ACT_SENTINEL);
  AddFeared( ch, m);
  do_flee(ch, "", 0);

}


int check_falling( struct char_data *ch)
{
  struct room_data *rp, *targ;
  int done, count, saved;
  char buf[256];

  if (IS_AFFECTED(ch, AFF_FLYING))
    return(FALSE);

  rp = real_roomp(ch->in_room);
  if (!rp) return(FALSE);

  if (rp->sector_type != SECT_AIR) 
    return(FALSE);

  if (ch->skills && number(1,101) < ch->skills[SKILL_SAFE_FALL].learned) {
    act("You manage to slow your fall, enough to stay alive..", 
	TRUE, ch, 0, 0, TO_CHAR);
    saved = TRUE;
  } else {
    act("The world spins, and you sky-dive out of control",
	TRUE, ch, 0, 0, TO_CHAR);
    saved = FALSE;
  }

  done = FALSE;
  count = 0;

  while (!done && count < 100) {

/*
  check for an exit down.
  if there is one, go through it.
*/
    if (rp->dir_option[DOWN] && rp->dir_option[DOWN]->to_room > -1) {
      targ = real_roomp(rp->dir_option[DOWN]->to_room);
    } else {
      /*
	pretend that this is the smash room.
	*/
      if (count > 1) {

	send_to_char("You are smashed into tiny pieces.\n\r", ch);
	act("$n smashes against the ground at high speed", 
	    FALSE, ch, 0, 0, TO_ROOM);
	act("You are drenched with blood and gore", 
	    FALSE,ch, 0, 0, TO_ROOM);

/*
  should damage all their stuff
*/
	DamageAllStuff(ch, BLOW_DAMAGE);

	if (!IS_IMMORTAL(ch)) {
	  GET_HIT(ch) = 0;
	  sprintf(buf, "%s has fallen to death", GET_NAME(ch));
	  logE(buf);
	if (!ch->desc)
	  GET_GOLD(ch) = 0;
	  die(ch);
	}
	return(TRUE);
	
      } else {

	send_to_char("You land with a resounding THUMP!\n\r", ch);
	GET_HIT(ch) = 0;
	GET_POS(ch) = POSITION_STUNNED;
	act("$n lands with a resounding THUMP!", FALSE, ch, 0, 0, TO_ROOM);
/*
  should damage all their stuff
*/
	DamageAllStuff(ch, BLOW_DAMAGE);

	return(TRUE);

      }
    }

    act("$n plunges towards oblivion", FALSE, ch, 0, 0, TO_ROOM);
    send_to_char("You plunge from the sky\n\r", ch);
    char_from_room(ch);
    char_to_room(ch, rp->dir_option[DOWN]->to_room);
    act("$n falls from the sky", FALSE, ch, 0, 0, TO_ROOM);
    count++;

    do_look(ch, "", 0);

    if (IS_SET(targ->room_flags, DEATH) && !IS_IMMORTAL(ch)) {
      NailThisSucker(ch);
      return(TRUE);
    }

    if (targ->sector_type != SECT_AIR) {
      /* do damage, or kill */
      if (count == 1) {
	send_to_char("You land with a resounding THUMP!\n\r", ch);
	GET_HIT(ch) = 0;
	GET_POS(ch) = POSITION_STUNNED;
	act("$n lands with a resounding THUMP!", FALSE, ch, 0, 0, TO_ROOM);
/*
  should damage all their stuff
*/
	DamageAllStuff(ch, BLOW_DAMAGE);

	return(TRUE);

      } else if (!saved) {
        send_to_char("You are smashed into tiny pieces.\n\r", ch);
        if (targ->sector_type >= SECT_WATER_SWIM)
	  act("$n is smashed to a pulp by $s impact with the water", 
	    FALSE, ch, 0, 0, TO_ROOM);
        else 
	  act("$n is smashed to a bloody pulp by $s impact with the ground", 
	    FALSE, ch, 0, 0, TO_ROOM);
	act("You are drenched with blood and gore", FALSE,ch, 0, 0, TO_ROOM);

/*
  should damage all their stuff
*/
	DamageAllStuff(ch, BLOW_DAMAGE);

	if (!IS_IMMORTAL(ch)) {
	  GET_HIT(ch) = 0;
	  sprintf(buf, "%s has fallen to death", GET_NAME(ch));
	  logE(buf);
	if (!ch->desc)
	  GET_GOLD(ch) = 0;
	  die(ch);
	}
	return(TRUE);

      } else {
	send_to_char("You land with a resounding THUMP!\n\r", ch);
	GET_HIT(ch) = 0;
	GET_POS(ch) = POSITION_STUNNED;
	act("$n lands with a resounding THUMP!", FALSE, ch, 0, 0, TO_ROOM);
/*
  should damage all their stuff
*/
	DamageAllStuff(ch, BLOW_DAMAGE);

	return(TRUE);
	
      }
    } else {
/*
  time to try the next room
*/
      rp = targ;
      targ = 0;
    }
  }  

  if (count >= 100) {
    logE("Someone fucked up an air room.");
    char_from_room(ch);
    char_to_room(ch, 2);
    do_look(ch, "", 0);
    return(FALSE);
  }
}

void check_drowning( struct char_data *ch)
{
  struct room_data *rp;
  char buf[256];

  if (IS_AFFECTED(ch, AFF_WATERBREATH))
    return;

  if(GetMaxLevel(ch) >= LOW_IMMORTAL)
    return;

  rp = real_roomp(ch->in_room);

  if (!rp) return;

  if (rp->sector_type == SECT_UNDERWATER) {
      send_to_char("PANIC!  You're drowning!!!!!!", ch);
      GET_HIT(ch)-=number(1,30);
      GET_MOVE(ch) -= number(10,50);
      update_pos(ch);
      if (GET_HIT(ch) < -10) {
	sprintf(buf, "%s killed by drowning", GET_NAME(ch));
	logE(buf);
	if (!ch->desc)
	  GET_GOLD(ch) = 0;
	die(ch);
      }
   }
}


void check_falling_obj( struct obj_data *obj, int room)
{
  struct room_data *rp, *targ;
  int done, count;

  if (obj->in_room != room) {
    logE("unusual object information in check_falling_obj");
    return;
  }

  rp = real_roomp(room);
  if (!rp) return;

  if (rp->sector_type != SECT_AIR) 
    return;

  done = FALSE;
  count = 0;

  while (!done && count < 100) {

    if (rp->dir_option[DOWN] && rp->dir_option[DOWN]->to_room > -1) {
      targ = real_roomp(rp->dir_option[DOWN]->to_room);
    } else {
      /*
	pretend that this is the smash room.
	*/
      if (count > 1) {

	if (rp->people) {
	  act("$p smashes against the ground at high speed", 
	      FALSE, rp->people, obj, 0, TO_ROOM);
	  act("$p smashes against the ground at high speed", 
	      FALSE, rp->people, obj, 0, TO_CHAR);
	}
	return;

      } else {

	if (rp->people) {
	  act("$p lands with a loud THUMP!", 
	      FALSE, rp->people, obj, 0, TO_ROOM);
	  act("$p lands with a loud THUMP!", 
	      FALSE, rp->people, obj, 0, TO_CHAR);
	}
	return;

      }
    }

    if (rp->people) { /* have to reference a person */
      act("$p falls out of sight", FALSE, rp->people, obj, 0, TO_ROOM);
      act("$p falls out of sight", FALSE, rp->people, obj, 0, TO_CHAR);
    }
    obj_from_room(obj);
    obj_to_room(obj, rp->dir_option[DOWN]->to_room);
    if (targ->people) {
      act("$p falls from the sky", FALSE, targ->people, obj, 0, TO_ROOM);
      act("$p falls from the sky", FALSE, targ->people, obj, 0, TO_CHAR);
    }
    count++;

    if (targ->sector_type != SECT_AIR) {
      if (count == 1) {
	if (targ->people) {
	  act("$p lands with a loud THUMP!", FALSE, targ->people, obj, 0, TO_ROOM);
	  act("$p lands with a loud THUMP!", FALSE, targ->people, obj, 0, TO_CHAR);
	}
	return;
      } else {
	if (targ->people) {
	  if (targ->sector_type >= SECT_WATER_SWIM){	  
	    act("$p smashes against the water at high speed", 
		FALSE, targ->people, obj, 0, TO_ROOM);
	    act("$p smashes against the water at high speed", 
		FALSE, targ->people, obj, 0, TO_CHAR);
	  } else {
	    act("$p smashes against the ground at high speed", 
		FALSE, targ->people, obj, 0, TO_ROOM);
	    act("$p smashes against the ground at high speed", 
		FALSE, targ->people, obj, 0, TO_CHAR);
	  }
	}
	return;

      }
    } else {
/*
  time to try the next room
*/
      rp = targ;
      targ = 0;
    }
  }  

  if (count >= 100) {
    logE("Someone fucked up an air room.");
    obj_from_room(obj);
    obj_to_room(obj, 2);
    return;
  }
}

int check_nature( struct char_data *i)
{

  if (check_falling(i)) {
    return(TRUE);
  }
  check_drowning(i);

}

int IsIntrinsic(struct char_data *ch, int spl) 
{
  int i;

  for(i=0;i<MAX_RACE_INTRINSIC;i++)
    if(GET_RACE(ch) == skill_info[spl].race_intrinsic[i])
      return(TRUE);
  return(FALSE);
}

int CastIntrinsic(struct char_data *ch, int spl)
{
  struct affected_type af, *p;
  
  af.duration = 0;
  af.location = APPLY_INTRINSIC;
  af.type = spl;

  if(!IsIntrinsic(ch,spl)) {
    send_to_char("Hey bozo, what are you doing trying to use an intrinsic?\n\r", ch);
    return(FALSE);
  }

  /* I'd like for this to be in a table or something, but... */
  switch(GET_RACE(ch)) {
  case RACE_ELVEN:
    switch(spl) {
    case SPELL_FAERIE_FIRE:
      af.duration  = 12;
      break;
    case SPELL_FAERIE_FOG:
      af.duration  = 8;
      break;
    }
    break;
  case RACE_DWARF:
    switch(spl) {
    case SPELL_ENCHANT_WEAPON:
      af.duration = 24*7;
      break;
    case SPELL_ENCHANT_ARMOR:
      af.duration = 24*7;
      break;
    }
    break;
  case RACE_FAERIE:
    switch(spl) {
      case SPELL_CURE_LIGHT:
      af.duration = 8;
      break;
    case SPELL_FAERIE_FIRE:
      af.duration = 8;
      break;
    }
    break;
  case RACE_DROW:
    switch(spl) {
    case SPELL_FAERIE_FIRE:
      af.duration = 8;
      break;
    case SPELL_WEB:
      af.duration = 12;
      break;
    case SPELL_FAERIE_FOG:
      af.duration = 8;
      break;
    }
    break;
  case RACE_VAMPIRE:
    switch(spl) {
    case SPELL_FEAR:
      af.duration = 8;
      break;
    case SPELL_CHARM_PERSON:
      af.duration = 12;
      break;
    }
    break;
  }

  if(af.duration) {
    for(p=ch->affected; p; p = p->next) 
      if(p->type == spl)
	if(p->location == APPLY_INTRINSIC)
	  return(FALSE);
  } else {
    logE("Unaccounted racial intrinsic, no duration set in CastIntrinsic()");
    return(FALSE);
  }

  af.modifier = 0;
  af.bitvector = 0;

  affect_to_char(ch, &af);

  send_to_char("Calling on your innate powers...\n\r", ch);
  return(TRUE);
}
