/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#define MAX_BUF_LENGTH              240

#define TYPE_UNDEFINED               -1
#define SPELL_RESERVED_DBC            0  /* SKILL NUMBER ZERO */
#define SPELL_ARMOR                   1 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_TELEPORT                2 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BLESS                   3 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BLINDNESS               4 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BURNING_HANDS           5 /* Reserved Skill[] DO NOT CHANGE */
/* druid */
#define SPELL_CALL_LIGHTNING          6 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CHARM_PERSON            7 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CHILL_TOUCH             8 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CLONE                   9 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_COLOUR_SPRAY           10 /* Reserved Skill[] DO NOT CHANGE */
/*druid*/
#define SPELL_CONTROL_WEATHER        11 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_FOOD            12 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_WATER           13 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_BLIND             14 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_CRITIC            15 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_LIGHT             16 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURSE                  17 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_EVIL            18 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_INVISIBLE       19 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_MAGIC           20 /* Reserved Skill[] DO NOT CHANGE */
/* druid */
#define SPELL_DETECT_POISON          21 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISPEL_EVIL            22 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_EARTHQUAKE             23 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ENCHANT_WEAPON         24 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ENERGY_DRAIN           25 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_FIREBALL               26 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HARM                   27 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HEAL                   28 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_INVISIBLE              29 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LIGHTNING_BOLT         30 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LOCATE_OBJECT          31 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_MAGIC_MISSILE          32 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_POISON                 33 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_PROTECT_FROM_EVIL      34 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_CURSE           35 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SANCTUARY              36 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SHOCKING_GRASP         37 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SLEEP                  38 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_STRENGTH               39 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SUMMON                 40 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_VENTRILOQUATE          41 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_WORD_OF_RECALL         42 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_POISON          43 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SENSE_LIFE             44 /* Reserved Skill[] DO NOT CHANGE */

/* types of attacks and skills must NOT use same numbers as spells! */

#define SKILL_SNEAK                  45 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_HIDE                   46 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_STEAL                  47 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_BACKSTAB               48 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_PICK_LOCK              49 /* Reserved Skill[] DO NOT CHANGE */

#define SKILL_KICK                   50 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_BASH                   51 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_RESCUE                 52 /* MAXIMUM SKILL NUMBER  */

/* END OF SKILL RESERVED "NO TOUCH" NUMBERS */


/* NEW SPELLS are to be inserted here */
#define SPELL_IDENTIFY               53
#define SPELL_INFRAVISION            54
#define SPELL_CAUSE_LIGHT            55
#define SPELL_CAUSE_CRITICAL         56
#define SPELL_FLAMESTRIKE            57
#define SPELL_DISPEL_GOOD            58
#define SPELL_WEAKNESS               59
#define SPELL_DISPEL_MAGIC           60
#define SPELL_KNOCK                  61
#define SPELL_KNOW_ALIGNMENT         62
#define SPELL_ANIMATE_DEAD           63
#define SPELL_PARALYSIS              64
#define SPELL_REMOVE_PARALYSIS       65
#define SPELL_FEAR                   66
#define SPELL_ACID_BLAST             67
#define SPELL_WATER_BREATH           68
#define SPELL_FLY                    69
#define SPELL_CONE_OF_COLD           70
#define SPELL_METEOR_SWARM           71
#define SPELL_ICE_STORM              72
#define SPELL_SHIELD                 73
#define SPELL_MON_SUM_1              74  /* done */
#define SPELL_MON_SUM_2              75
#define SPELL_MON_SUM_3              76
#define SPELL_MON_SUM_4              77
#define SPELL_MON_SUM_5              78
#define SPELL_MON_SUM_6              79
#define SPELL_MON_SUM_7              80  /* done  */
#define SPELL_FIRESHIELD             81
#define SPELL_CHARM_MONSTER          82 
#define SPELL_CURE_SERIOUS           83
#define SPELL_CAUSE_SERIOUS          84
#define SPELL_REFRESH                85
#define SPELL_SECOND_WIND            86
#define SPELL_TURN                   87
#define SPELL_SUCCOR                 88
#define SPELL_LIGHT                  89
#define SPELL_CONT_LIGHT             90
#define SPELL_CALM                   91
#define SPELL_STONE_SKIN             92
#define SPELL_CONJURE_ELEMENTAL      93
#define SPELL_TRUE_SIGHT             94
#define SPELL_MINOR_CREATE           95
#define SPELL_FAERIE_FIRE            96
#define SPELL_FAERIE_FOG             97
#define SPELL_CACAODEMON             98
#define SPELL_POLY_SELF              99
#define SPELL_MANA                  100
#define SPELL_ASTRAL_WALK           101
#define SPELL_RESURRECTION          102

#define SPELL_H_FEAST               103
#define SPELL_FLY_GROUP             104
#define SPELL_DRAGON_BREATH         105
#define SPELL_WEB                   106
#define SPELL_MINOR_TRACK           107
#define SPELL_MAJOR_TRACK           108


#define SPELL_GOLEM                 109
#define SPELL_FAMILIAR              110
#define SPELL_CHANGESTAFF           111
#define SPELL_HOLY_WORD             112
#define SPELL_ENCHANT_ARMOR         113
#define SPELL_PWORD_KILL            114
#define SPELL_PWORD_BLIND           115

#define SPELL_CHAIN_LIGHTNING       116
#define SPELL_SCARE                 117
#define SPELL_AID                   118
#define SPELL_COMMAND               119

/* druid */
#define SKILL_CHANGE_FORM           120 /* changed. */
#define SPELL_FEEBLEMIND            121

/* druid... */
#define SPELL_SHILLELAGH            122
#define SPELL_GOODBERRY             123
#define SPELL_FLAME_BLADE           124
#define SPELL_ANIMAL_GROWTH         125
#define SPELL_INSECT_GROWTH         126
#define SPELL_CREEPING_DEATH        127
#define SPELL_COMMUNE               128  /* whatzone*/

#define SPELL_ANIMAL_SUM_1          129
#define SPELL_ANIMAL_SUM_2          130
#define SPELL_ANIMAL_SUM_3          131

#define SPELL_FIRE_SERVANT          132
#define SPELL_EARTH_SERVANT         133
#define SPELL_WATER_SERVANT         134
#define SPELL_WIND_SERVANT          135

#define SPELL_REINCARNATE           136
#define SPELL_CHARM_VEGGIE          137
#define SPELL_VEGGIE_GROWTH         138
#define SPELL_TREE                  139

#define SPELL_ANIMATE_ROCK          140
#define SPELL_TREE_TRAVEL           141
#define SPELL_TRAVELLING            142  /* faster move outdoors */
#define SPELL_ANIMAL_FRIENDSHIP     143
#define SPELL_INVIS_TO_ANIMALS      144
#define SPELL_SLOW_POISON           145
#define SPELL_ENTANGLE              146
#define SPELL_SNARE                 147
#define SPELL_GUST_OF_WIND          148
#define SPELL_BARKSKIN              149
#define SPELL_SUNRAY                150
#define SPELL_WARP_WEAPON           151
#define SPELL_HEAT_STUFF            152
#define SPELL_FIND_TRAPS            153
#define SPELL_FIRESTORM             154

/* other */
#define SPELL_HASTE                 155
#define SPELL_SLOW                  156
#define SPELL_DUST_DEVIL            157
#define SPELL_KNOW_MONSTER          158

#define SPELL_TRANSPORT_VIA_PLANT   159
#define SPELL_SPEAK_WITH_PLANT      160
#define SPELL_SILENCE               161
#define SPELL_SENDING               162
#define SPELL_TELEPORT_WO_ERROR     163
#define SPELL_PORTAL                164
#define SPELL_DRAGON_RIDE           165
#define SPELL_MOUNT                 166
#define SPELL_THORN_SPRAY           167

/* room spell like deal */

#define SPELL_BLADE_BARRIER        

/* maybe */
#define SPELL_SUMMON_OBJ            

#define SKILL_DUAL_WIELD             169  /* */

#define SKILL_FIRST_AID              170
#define SKILL_SIGN                   171
#define SKILL_RIDE                   172
#define SKILL_SWITCH_OPP             173
#define SKILL_DODGE                  174
#define SKILL_REMOVE_TRAP            175
#define SKILL_RETREAT                176
#define SKILL_QUIV_PALM              177
#define SKILL_SAFE_FALL              178
#define SKILL_FEIGN_DEATH            179
#define SKILL_HUNT                   180
#define SKILL_LOCATE_TRAP            181
#define SKILL_SPRING_LEAP            182
#define SKILL_DISARM                 183
#define SKILL_READ_MAGIC             184
#define SKILL_EVALUATE               185
#define SKILL_SPY                    186
#define SKILL_DOORBASH               187
#define SKILL_SWIM                   188
#define SKILL_CONS_UNDEAD            189
#define SKILL_CONS_VEGGIE            190
#define SKILL_CONS_DEMON             191
#define SKILL_CONS_ANIMAL            192
#define SKILL_CONS_REPTILE           193
#define SKILL_CONS_PEOPLE            194
#define SKILL_CONS_GIANT             195
#define SKILL_CONS_OTHER             196
#define SKILL_DISGUISE               197
#define SKILL_CLIMB                  198
#define SKILL_INSET                  199  /* skill at insetting stones */

/* breaths occupy 200 to 205 in spello, so for consistency.. 206 is next */

#define SPELL_RESIST_HOLD            206
#define SPELL_RESIST_ELECTRICITY     207
#define SPELL_RESIST_COLD            208
#define SPELL_RESIST_DRAIN           209
#define SPELL_RESIST_POISON          210
#define SPELL_RESIST_ACID            211
#define SPELL_RESIST_FIRE            212
#define SPELL_RESIST_ENERGY          213
#define SPELL_RESIST_PIERCE          214
#define SPELL_RESIST_SLASH           215
#define SPELL_RESIST_BLUNT           216
#define SKILL_BREWING                217
#define SPELL_SUN_BLIND              218 /* just to keep handler.c happy */
#define SKILL_BERSERK                219
#define SKILL_PALM                   220
#define SKILL_PEEK                   221
#define SKILL_CONS_INSECT            222
#define SKILL_CONS_AVIAN             223

#define MAX_EXIST_SPELL              223   /* move this and change it */

#define FIRST_BREATH_WEAPON	     400
#define SPELL_GEYSER                 400
#define SPELL_FIRE_BREATH            401
#define SPELL_GAS_BREATH             402
#define SPELL_FROST_BREATH           403
#define SPELL_ACID_BREATH            404
#define SPELL_LIGHTNING_BREATH       405
#define LAST_BREATH_WEAPON	     405

/* NOTE!!!!!!!!!!!!!!!
   all spells MUST be before these types.   Otherwise, certain aspects of
   fireshield, sanct, etc, will not work!
   */

#define TYPE_HIT                     406
#define TYPE_BLUDGEON                407
#define TYPE_PIERCE                  408
#define TYPE_SLASH                   409
#define TYPE_WHIP                    410   
#define TYPE_CLAW                    411  
#define TYPE_BITE                    412  
#define TYPE_STING                   413  
#define TYPE_CRUSH                   414  
#define TYPE_CLEAVE                  415
#define TYPE_STAB                    416
#define TYPE_SMASH                   417
#define TYPE_SMITE                   418
#define TYPE_BLAST                   419

#define TYPE_SUFFERING               420

/* More anything but spells and weapontypes can be insterted here! */


#define SPELL_GREEN_SLIME            421




#define MAX_TYPES 70

#define SAVING_PARA   0
#define SAVING_ROD    1
#define SAVING_PETRI  2
#define SAVING_BREATH 3
#define SAVING_SPELL  4


#define MAX_SPL_LIST	410


#define TAR_IGNORE	 (1<< 0)
#define TAR_CHAR_ROOM	 (1<< 1)
#define TAR_CHAR_WORLD	 (1<< 2)
#define TAR_FIGHT_SELF	 (1<< 3)
#define TAR_FIGHT_VICT	 (1<< 4)
#define TAR_SELF_ONLY	 (1<< 5) /* Only a check, use with ei. TAR_CHAR_ROOM */
#define TAR_SELF_NONO	 (1<< 6) /* Only a check, use with ei. TAR_CHAR_ROOM */
#define TAR_OBJ_INV	 (1<< 7)
#define TAR_OBJ_ROOM	 (1<< 8)
#define TAR_OBJ_WORLD	 (1<< 9)
#define TAR_OBJ_EQUIP	 (1<<10)
#define TAR_NAME	 (1<<11)
#define TAR_VIOLENT	 (1<<12)
#define TAR_ROOM         (1<<13)  /* spells which target the room  */

struct spell_info_type
{
	void (*spell_pointer) 
               (byte level, struct char_data *ch, char *arg, int type,
	        struct char_data *tar_ch, struct obj_data *tar_obj);
	byte minimum_position;  /* Position for caster 			*/
	ubyte min_usesmana;     /* Amount of mana used by a spell	 */
	byte beats;             /* Heartbeats until ready for next */

        byte min_level[MIN_LEVEL_NUM]; /* Each entry will hold min level */

	sh_int targets;         /* See below for use with TAR_XXX  */
        sh_int spellfail;       /* modifier for spell failure      */
};

/* Possible Targets:

   bit 0 : IGNORE TARGET
   bit 1 : PC/NPC in room
   bit 2 : PC/NPC in world
   bit 3 : Object held
   bit 4 : Object in inventory
   bit 5 : Object in room
   bit 6 : Object in world
   bit 7 : If fighting, and no argument, select tar_char as self
   bit 8 : If fighting, and no argument, select tar_char as victim (fighting)
   bit 9 : If no argument, select self, if argument check that it IS self.

*/

#define SPELL_TYPE_SPELL   0
#define SPELL_TYPE_POTION  1
#define SPELL_TYPE_WAND    2
#define SPELL_TYPE_STAFF   3
#define SPELL_TYPE_SCROLL  4


/* Attacktypes with grammar */

struct attack_hit_type {
  char *singular;
  char *plural;
};
