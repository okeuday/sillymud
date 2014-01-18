/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Development
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include "structs.h"
#include "limits.h"
#include "trap.h"
#include "race.h"


const char *spell_wear_off_msg[] = {
  "RESERVED DB.C",
  "You feel less protected.",
  "!Teleport!",
  "You feel less righteous.",
  "You feel a cloak of blindness dissolve.",
  "!Burning Hands!",
  "!Call Lightning",
  "You feel more in control of your destiny",
  "You feel stronger now",
  "!Clone!",
  "!Color Spray!",
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure Blind!",
  "!Cure Critic!",
  "!Cure Light!",
  "You feel better.",
  "You sense the red in your vision disappear.",
  "The detect invisible wears off.",
  "The detect magic wears off.",
  "The detect poison wears off.",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",
  "!Fireball!",
  "!Harm!",
  "!Heal",
  "You feel exposed.",
  "!Lightning Bolt!",
  "!Locate object!",
  "!Magic Missile!",
  "You feel less sick.",
  "You feel less protected.",
  "!Remove Curse!",
  "The white aura around your body fades.",
  "!Shocking Grasp!",
  "You feel less tired.",
  "You don't feel as strong.",
  "!Summon!",
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "You feel less aware of your surroundings.",
  "",  /* NO MESSAGE FOR SNEAK*/
  "!Hide!",
  "!Steal!",
  "!Backstab!",
  "!Pick Lock!",
  "!Kick!",
  "!Bash!",
  "!Rescue!",
  "!Identify!",  /* 53 */
  "You feel disoriented as you lose your infravision.",
  "!cause light!",
  "!cause crit!",
  "!flamestrike!",
  "!dispel good!",
  "You feel somewhat stronger now...",
  "!dispel magic!",
  "!knock!",
  "!know alignment!",
  "!animate dead!",
  "You feel freedom of movement.",
  "!remove paralysis!",
  "!fear!",
  "!acid blast!",
  "You feel a tightness at your throat. ",
  "You feel heavier now, your flying ability is gone.",
  "spell1, please report.",   /* 70 */
  "spell2, please report.",
  "spell72, please report",
  "Your magic shield fades away happily.",
  "spell74, please report",
  "spell6, please report.",
  "spell7, please report.",
  "spell8, please report.",
  "spell9, please report.",
  "spell10, please report.",
  "spell11, please report.",  /* 80  */
  "The red glow around your body fades",
  "spell82, please report.",
  "spell83, please report.",
  "spell84, please report.",
  "spell85, please report.",
  "spell86, please report.",
  "spell87, please report.",
  "spell88, please report.",
  "spell89, please report.",
  "spell90, please report.",  /* 90 */
  "spell91, please report.",
  "Your skin returns to normal.",
  "spell93, please report.",
  "Your clarity of vision disappears",
  "spell95, please report",
  "The pink glow around your body fades.",   /* 96 */
  "spell 97, please report.",
  "spell 98 please report." ,
  "spell 99 please report." ,
  "spell 100 please report.",
  "spell 101 please report.",
  "spell 102 please report.",
  "spell 103 please report.",
  "You feel heavier, your flying spell is leaving you.",
  "spell 105 please report.",
  "You feel freedom of movement.",
  "You lose your tracking ability.",
  "Your tracking ability fades away.",   /* 108 */
  "spell 109 please report.",
  "",
  "spell 111 please report.",
  "spell 112 please report.",
  "spell 113 please report.",
  "spell 114 please report.",
  "spell 115 please report.",
  "spell 116 please report.",
  "spell 117 please report.",
  "You don't feel so aided anymore",
  "spell 119 please report.",
  "You can now change forms once again.",
  "spell 121 please report.",
  "spell 122 please report.",
  "spell 123 please report.",
  "spell 124 please report.",
  "spell 125 please report.",
  "spell 126 please report.",
  "spell 127 please report.",
  "spell 128 please report.",
  "spell 129 please report.",
  "spell 130 please report.",
  "spell 131 please report.",
  "spell 132 please report.",
  "spell 133 please report.",
  "spell 134 please report.",
  "spell 135 please report.",
  "spell 136 please report.",
  "spell 137 please report.",
  "spell 138 please report.",
  "spell 139 please report.",
  "spell 140 please report.",
  "You don't feel so in touch with the trees anymore",
  "spell 142 please report.",
  "spell 143 please report.",
  "",
  "spell 145 please report.",
  "You are free of your entanglements",
  "You are free of the snare!",
  "spell 148 please report.",
  "Your barkskin fades away.",
  "spell 150 please report.",
  "spell 151 please report.",
  "Your equipment finally cools down!",
  "spell 153 please report.",
  "spell 154 please report.",
  "You feel slower!",
  "You feel faster!",
  "",
  "spell 158 please report.",
  "spell 159 please report.",
  "spell 160 please report.",
  "You feel louder.",
  "spell 162 please report.",
  "spell 163 please report.",
  "spell 164 please report.",
  "You feel less in control of dragons",
  "spell 166 please report.",
  "spell 167 please report.",
  "spell 168 please report.",
  "spell 169 please report.",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "You feel like you might be able to swim again.",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "Your swirling sphere vanishes!",  /* 206 */
  "Your dim blue sphere vanishes!",
  "Your dim white sphere vanishes!",
  "Your bright glowing sphere vanishes!",
  "Your dark purple sphere vanishes!",  /* 210 */
  "Your bright green sphere vanishes!",
  "Your dull red sphere vanishes!",
  "Your matte black sphere vanishes!",
  "Your stone sphere vanishes!",
  "Your steel sphere vanishes!",
  "Your pulsing sphere vanishes!", /* resist blunt */
  "",				/* brewing */
  "Your vision finally returns.", /* 218 */
  "Your head clears.",
  "",
  "",				/* 221 */
  "",
  "",
  "\n"
};

const char *spell_wear_off_room_msg[] = {
  "RESERVED DB.C",
  "$n's mystic armor fades slowly away",
  "!Teleport!",
  "$n looks less blessed",
  "$n blinks $s eyes.",
  "!Burning Hands!",
  "!Call Lightning",
  "$n shivers and shakes",
  "$n seems a bit stronger",
  "!Clone!",
  "!Color Spray!",
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure Blind!",
  "!Cure Critic!",
  "!Cure Light!",
  "$n looks better",
  "$n blinks $s eyes, the reddish hue vanishes",
  "$n blinks $s eyes, the yellowish hue vanishes",
  "$n blinks $s eyes, the bluish hue vanishes",
  "$n blinks $s eyes, the whitish hue vanishes",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",
  "!Fireball!",
  "!Harm!",
  "!Heal",
  "$n slowly fades into existence",
  "!Lightning Bolt!",
  "!Locate object!",
  "!Magic Missile!",
  "$n looks healthier",
  "$n seems less holier-than-thou",
  "!Remove Curse!",
  "The white aura around $n's body fades.",
  "!Shocking Grasp!",
  "$n murmurs and shakes in $s sleep.",
  "$n looks weaker.",
  "!Summon!",
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "$n stops looking around so much",
  "",  /* NO MESSAGE FOR SNEAK*/
  "!Hide!",
  "!Steal!",
  "!Backstab!",
  "!Pick Lock!",
  "!Kick!",
  "!Bash!",
  "!Rescue!",
  "!Identify!",
  "$n stumbles, and blinks $s eyes, the reddish hue fades",
  "!cause light!",
  "!cause crit!",
  "!flamestrike!",
  "!dispel magic!",
  "$n looks stronger!",
  "!dispel good!",
  "!knock!",
  "!know alignment!",
  "!animate dead!",
  "$n jerks out of $s paralyzed state",
  "!remove paralysis!",
  "!fear!",
  "!acid blast!",
  "$n frantically sucks for air.",
  "The magical forces holding $n aloft vanish",
  "spell1, please report.",   /* 70 */
  "spell2, please report.",
  "spell72, please report.",
  "$n's magic shield fades away happily.",
  "spell74, please report.",
  "spell6, please report.",
  "spell7, please report.",
  "spell8, please report.",
  "spell9, please report.",
  "spell10, please report.",
  "spell11, please report.",  /* 80  */
  "The red glow around $n's body fades",
  "spell82, please report.",
  "spell83, please report.",
  "spell84, please report.",
  "spell85, please report.",
  "spell86, please report.",
  "spell87, please report.",
  "spell88, please report.",
  "spell89, please report.",
  "spell90, please report.",  /* 90 */
  "spell91, please report.",
  "$n's skin loses its stony appearance.",
  "spell93, please report.",
  "$n blinks rapidly, as the silvery hue fades from $s eyes",
  "spell95, please report",
  "The pink glow around $n's body fades.",   /* 96 */
  "spell 97, please report.",
  "spell 98 please report." ,
  "spell 99 please report." ,
  "spell 100 please report.",
  "spell 101 please report.",
  "spell 102 please report.",
  "spell 103 please report.",
  "The magical shield protecting $n fades away sadly.",
  "spell 105 please report.",
  "$n seems to be free of the webs that hold $m",
  "$n looks confused.",
  "$n looks REALLY confused",   /* 108 */
  "spell 109 please report.",
  "",
  "spell 111 please report.",
  "spell 112 please report.",
  "spell 113 please report.",
  "spell 114 please report.",
  "spell 115 please report.",
  "spell 116 please report.",
  "spell 117 please report.",
  "$n seems less aided",
  "spell 119 please report.",
  "",
  "spell 121 please report.",
  "spell 122 please report.",
  "spell 123 please report.",
  "spell 124 please report.",
  "spell 125 please report.",
  "spell 126 please report.",
  "spell 127 please report.",
  "spell 128 please report.",
  "spell 129 please report.",
  "spell 130 please report.",
  "spell 131 please report.",
  "spell 132 please report.",
  "spell 133 please report.",
  "spell 134 please report.",
  "spell 135 please report.",
  "spell 136 please report.",
  "spell 137 please report.",
  "spell 138 please report.",
  "spell 139 please report.",
  "spell 140 please report.",
  "",
  "spell 142 please report.",
  "spell 143 please report.",
  "",
  "spell 145 please report.",
  "$n wrenches free of $s entanglements",
  "$n manages to get out of a magical snare",
  "spell 148 please report.",
  "$n's skin stops resembling bark",
  "spell 150 please report.",
  "spell 151 please report.",
  "$n's equipment stops burning $m",
  "spell 153 please report.",
  "spell 154 please report.",
  "$n seems slower",
  "$n seems faster",
  "",
  "spell 158 please report.",
  "spell 159 please report.",
  "spell 160 please report.",
  "spell 161 please report.",
  "spell 162 please report.",
  "spell 163 please report.",
  "spell 164 please report.",
  "",
  "spell 166 please report.",
  "spell 167 please report.",
  "spell 168 please report.",
  "spell 169 please report.",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "$n's swirling sphere vanishes!",  /* 206 */
  "$n's dim blue sphere vanishes!",
  "$n's dim white sphere vanishes!",
  "$n's bright glowing sphere vanishes!",
  "$n's dark purple sphere vanishes!",  /* 210 */
  "$n's bright green sphere vanishes!",
  "$n's dull red sphere vanishes!",
  "$n's matte black sphere vanishes!",
  "$n's stone sphere vanishes!",
  "$n's steel sphere vanishes!",
  "$n's pulsing sphere vanishes!",
  "",				/* brewing */
  "$n's vision finally returns.",
  "$n's head clears.",
  "",
  "",				/* 221 */
  "",
  "",
  "\n"  
};

const char *spell_wear_off_soon_msg[] = {
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "You are starting to gain back your will",
  "You feel somewhat stronger",
  "!Clone!",
  "!Color Spray!",
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure Blind!",
  "!Cure Critic!",
  "!Cure Light!",
  "",
  "",
  "",
  "",
  "",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",
  "!Fireball!",
  "!Harm!",
  "!Heal",
  "",
  "!Lightning Bolt!",
  "!Locate object!",
  "!Magic Missile!",
  "",
  "",
  "!Remove Curse!",
  "The white aura around your body flickers slightly.",
  "!Shocking Grasp!",
  "You feel more awake.",
  "You feel a bit weaker",
  "!Summon!",
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "",
  "",  /* NO MESSAGE FOR SNEAK*/
  "!Hide!",
  "!Steal!",
  "!Backstab!",
  "!Pick Lock!",
  "!Kick!",
  "!Bash!",
  "!Rescue!",
  "!Identify!",
  "Your infravision begins to fade",
  "!cause light!",
  "!cause crit!",
  "!flamestrike!",
  "!dispel good!",
  "You feel a bit stronger",
  "!dispel magic!",
  "!knock!",
  "!know alignment!",
  "!animate dead!",
  "",
  "!remove paralysis!",
  "!fear!",
  "!acid blast!",
  "Your throat feels tight, your water breathing spell is leaving you ",
  "You feel heavier now, your flying ability is leaving you.",
  "spell1, please report.",   /* 70 */
  "spell2, please report.",
  "spell3, please report.",
  "Your shield spell starts to flicker",
  "spell5, please report.",
  "spell6, please report.",
  "spell7, please report.",
  "spell8, please report.",
  "spell9, please report.",
  "spell10, please report.",
  "spell11, please report.",  /* 80  */
  "The red glow around your body flickers",
  "spell82, please report.",
  "spell83, please report.",
  "spell84, please report.",
  "spell85, please report.",
  "spell86, please report.",
  "spell87, please report.",
  "spell88, please report.",
  "spell89, please report.",
  "spell90, please report.",  /* 90 */
  "spell91, please report.",
  "",
  "spell93, please report.",
  "",
  "spell95, please report",
  "The pink glow around your body flickers",   /* 96 */
  "spell 97, please report.",
  "spell 98 please report." ,
  "spell 99 please report." ,
  "spell 100 please report.",
  "spell 101 please report.",
  "spell 102 please report.",
  "spell 103 please report.",
  "The magical shield around your body flickers",
  "spell 105 please report.",
  "The webs seem a bit less strong.",
  "",
  "",   /* 108 */
  "spell 109 please report.",
  "",
  "spell 111 please report.",
  "spell 112 please report.",
  "spell 113 please report.",
  "spell 114 please report.",
  "spell 115 please report.",
  "spell 116 please report.",
  "spell 117 please report.",
  "",
  "spell 119 please report.",
  "You will soon be able to change forms again.",
  "spell 121 please report.",
  "spell 122 please report.",
  "spell 123 please report.",
  "spell 124 please report.",
  "spell 125 please report.",
  "spell 126 please report.",
  "spell 127 please report.",
  "spell 128 please report.",
  "spell 129 please report.",
  "spell 130 please report.",
  "spell 131 please report.",
  "spell 132 please report.",
  "spell 133 please report.",
  "spell 134 please report.",
  "spell 135 please report.",
  "spell 136 please report.",
  "spell 137 please report.",
  "spell 138 please report.",
  "spell 139 please report.",
  "spell 140 please report.",
  "You feel less in common with trees.",
  "spell 142 please report.",
  "spell 143 please report.",
  "",
  "spell 145 please report.",
  "",
  "",
  "spell 148 please report.",
  "Your barkskin is starting to fade",
  "spell 150 please report.",
  "spell 151 please report.",
  "Your armor cools down a bit",
  "spell 153 please report.",
  "spell 154 please report.",
  "You feel the world speeding up a bit",
  "You feel the world slowing down a bit",
  "",
  "spell 158 please report.",
  "spell 159 please report.",
  "spell 160 please report.",
  "spell 161 please report.",
  "spell 162 please report.",
  "spell 163 please report.",
  "spell 164 please report.",
  "",
  "spell 166 please report.",
  "spell 167 please report.",
  "spell 168 please report.",
  "spell 169 please report.",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "Your swirling sphere flickers and wavers.",  /* 206 */
  "Your dim blue sphere flickers and wavers.",
  "Your dim white sphere flickers and wavers.",
  "Your bright glowing sphere flickers and wavers.",
  "Your dark purple sphere flickers and wavers.",  /* 210 */
  "Your bright green sphere flickers and wavers.",
  "Your dull red sphere flickers and wavers.",
  "Your matte black sphere flickers and wavers.",
  "Your stone sphere flickers and wavers.",
  "Your steel sphere flickers and wavers.",
  "Your pulsing sphere flickers and wavers.",
  "",				/* brewing */
  "Your eyesight is slowly returning.",
  "Your head is starting to clear.",
  "",
  "",				/* 221 */
  "",
  "",
  "\n"  
};

const char *spell_wear_off_soon_room_msg[] = {
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "$n seems more in control of $mself",
  "",
  "!Clone!",
  "!Color Spray!",
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure Blind!",
  "!Cure Critic!",
  "!Cure Light!",
  "",
  "",
  "",
  "",
  "",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",
  "!Fireball!",
  "!Harm!",
  "!Heal",
  "",
  "!Lightning Bolt!",
  "!Locate object!",
  "!Magic Missile!",
  "",
  "",
  "!Remove Curse!",
  "The white aura around $n's body flickers slightly.",
  "!Shocking Grasp!",
  "$n seems a bit more awake",
  "$n seems a bit weaker",
  "!Summon!",
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "",
  "",  /* NO MESSAGE FOR SNEAK*/
  "!Hide!",
  "!Steal!",
  "!Backstab!",
  "!Pick Lock!",
  "!Kick!",
  "!Bash!",
  "!Rescue!",
  "!Identify!",
  "",
  "!cause light!",
  "!cause crit!",
  "!flamestrike!",
  "!dispel good!",
  "$n's eyes stop looking so red.  Must have used Visine",
  "!dispel magic!",
  "!knock!",
  "!know alignment!",
  "!animate dead!",
  "",
  "!remove paralysis!",
  "!fear!",
  "!acid blast!",
  "$n gasps for air for a moment or two",
  "The magic force keeping $n aloft flickers slightly, it will vanish soon.",
  "spell1, please report.",   /* 70 */
  "spell2, please report.",
  "spell3, please report.",
  "$n's shield of force flickers slightly",
  "spell5, please report.",
  "spell6, please report.",
  "spell7, please report.",
  "spell8, please report.",
  "spell9, please report.",
  "spell10, please report.",
  "spell11, please report.",  /* 80  */
  "The red glow around $n's body flickers",
  "spell82, please report.",
  "spell83, please report.",
  "spell84, please report.",
  "spell85, please report.",
  "spell86, please report.",
  "spell87, please report.",
  "spell88, please report.",
  "spell89, please report.",
  "spell90, please report.",  /* 90 */
  "spell91, please report.",
  "",
  "spell93, please report.",
  "",
  "spell95, please report",
  "The pink glow around $n's body flickers",   /* 96 */
  "spell 97, please report.",
  "spell 98 please report." ,
  "spell 99 please report." ,
  "spell 100 please report.",
  "spell 101 please report.",
  "spell 102 please report.",
  "spell 103 please report.",
  "$n's shield starts to fade away",
  "spell 105 please report.",
  "The webs surrounding $n seem to lose their grip a bit.",
  "",
  "",   /* 108 */
  "spell 109 please report.",
  "",
  "spell 111 please report.",
  "spell 112 please report.",
  "spell 113 please report.",
  "spell 114 please report.",
  "spell 115 please report.",
  "spell 116 please report.",
  "spell 117 please report.",
  "",
  "spell 119 please report.",
  "",
  "spell 121 please report.",
  "spell 122 please report.",
  "spell 123 please report.",
  "spell 124 please report.",
  "spell 125 please report.",
  "spell 126 please report.",
  "spell 127 please report.",
  "spell 128 please report.",
  "spell 129 please report.",
  "spell 130 please report.",
  "spell 131 please report.",
  "spell 132 please report.",
  "spell 133 please report.",
  "spell 134 please report.",
  "spell 135 please report.",
  "spell 136 please report.",
  "spell 137 please report.",
  "spell 138 please report.",
  "spell 139 please report.",
  "spell 140 please report.",
  "",
  "spell 142 please report.",
  "spell 143 please report.",
  "",
  "spell 145 please report.",
  "$n is entangled by leaves and bushes",
  "$n is ensnared",
  "spell 148 please report.",
  "$n's barklike skin starts to go away",
  "spell 150 please report.",
  "spell 151 please report.",
  "",
  "spell 153 please report.",
  "spell 154 please report.",
  "",
  "",
  "",
  "spell 158 please report.",
  "spell 159 please report.",
  "spell 160 please report.",
  "spell 161 please report.",
  "spell 162 please report.",
  "spell 163 please report.",
  "spell 164 please report.",
  "",
  "spell 166 please report.",
  "spell 167 please report.",
  "spell 168 please report.",
  "spell 169 please report.",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "$n's swirling sphere flickers and wavers.",  /* 206 */
  "$n's dim blue sphere flickers and wavers.",
  "$n's dim white sphere flickers and wavers.",
  "$n's bright glowing sphere flickers and wavers.",
  "$n's dark purple sphere flickers and wavers.",  /* 210 */
  "$n's bright green sphere flickers and wavers.",
  "$n's dull red sphere flickers and wavers.",
  "$n's matte black sphere flickers and wavers.",
  "$n's stone sphere flickers and wavers.",
  "$n's steel sphere flickers and wavers.",
  "$n's pulsing sphere flickers and wavers.",
  "",				/* brewing */
  "$n's eyesight is slowly returning.",
  "$n's head is starting to clear.",
  "",
  "",
  "",
  "",
  "\n"  
};



const int rev_dir[] = 
{
	2,
	3,
	0,
	1,	
	5,
	4
}; 

const int TrapDir[] = 
{
	TRAP_EFF_NORTH,
	TRAP_EFF_EAST,
	TRAP_EFF_SOUTH,
	TRAP_EFF_WEST,	
	TRAP_EFF_UP,
	TRAP_EFF_DOWN
}; 

const int movement_loss[]=
{
	1,  /* Inside     */
	2,  /* City       */
	2,  /* Field      */
	3,  /* Forest     */
	4,  /* Hills      */
	6,  /* Mountains  */
        8,  /* Swimming   */
        10, /* Unswimable */
        2,  /* Flying     */
        20, /* Submarine  */
	4,  /* Desert     */
	1   /* Tree       */
};

const  char *exits[] =
    {	
      "North",
      "East ",
      "South",
      "West ",
      "Up   ",
      "Down "
      };




const char *dirs[] = 
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"\n"
};


const char *ItemDamType[] = 
{
	"burned",
	"frozen",
	"electrified",
	"crushed",
	"corroded"
};

const char *weekdays[7] = { 
	"the Day of the Moon",
	"the Day of the Bull",
	"the Day of the Deception",
	"the Day of Thunder",
	"the Day of Freedom",
	"the day of the Great Gods",
	"the Day of the Sun" };

const char *month_name[17] = {
	"Month of Winter",           /* 0 */
	"Month of the Winter Wolf",
	"Month of the Frost Giant",
	"Month of the Old Forces",
	"Month of the Grand Struggle",
	"Month of the Spring",
	"Month of Nature",
	"Month of Fertility",
	"Month of the Dragon",
	"Month of the Sun",
	"Month of the Heat",
	"Month of the Battle",
	"Month of the Dark Sunglasses",
	"Month of the Shadows",
	"Month of the Long Shadows",
	"Month of the Ancient Darkness",
	"Month of the Great Evil"
};

const int sharp[] = {
   0,
   0,
   0,
   1,    /* Slashing */
   0,
   0,
   0,
   0,    /* Bludgeon */
   0,
   0,
   0,
   0 };  /* Pierce   */
const int sf_where[] = {
  4,				/* light */
  4,				/* finger */
  4,				/* " */
  4,				/* neck */
  4,				/* neck */
  10,				/* body */
  14,				/* head */
  6,				/* legs */
  4,				/* feet */
  16,				/* hands */
  10,				/* arms */
  8,				/* shield */
  10,				/* about body */
  2,				/* about waiste */
  4,				/* wrist */
  4,				/* wrist */
  12,				/* wield */
  6				/* hold */
};

const char *where[] = {
	"<used as light>      ",
	"<worn on finger>     ",
	"<worn on finger>     ",
	"<worn around neck>   ",
	"<worn around neck>   ",
	"<worn on body>       ",
	"<worn on head>       ",
	"<worn on legs>       ",
	"<worn on feet>       ",
	"<worn on hands>      ",
	"<worn on arms>       ",
	"<worn as shield>     ",
	"<worn about body>    ",
	"<worn about waist>   ",
	"<worn around wrist>  ",
	"<worn around wrist>  ",
	"<wielded>            ",
	"<held>               " 
}; 

const char *drinks[]=
{
	"water",
	"beer",
	"wine",
	"ale",
	"dark ale",
	"whiskey",
	"lemonade",
	"firebreather",
	"local specialty",
	"slime mold juice",
	"milk",
	"tea",
	"coffee",
	"blood",
	"salt water",
	"coca cola",
	"\n"
};

const char *drinknames[]=
{
	"water",
	"beer",
	"wine",
	"ale",
	"ale",
	"whisky",
	"lemonade",
	"firebreather",
	"local",
	"juice",
	"milk",
	"tea",
	"coffee",
	"blood",
	"salt",
	"cola",
	"\n"
};

const int RacialMax[MAX_RACE+1][6] = {

                /* m      c      f       t        d       k */
/* mutt */ {      25,     25,   25,      25,      25,     25},
/* Hum  */ {      30,     20,   51,      40,      51,     51},
/* elf  */ {      51,     30,   20,      51,      40,     20},
/* dwrf */ {      20,     51,   51,      35,      15,     40},
/* 1/2  */ {      20,     40,   30,      51,      51,     20},
/* gnm  */ {      51,     51,   30,      40,      20,     20},
/* rep  */ {      20,     30,   40,      50,      51,     40},
/* Myst */ {      50,     50,   50,      1,       50,      1},
/* were */ {      30,     30,   51,      50,      40,     20},
/* drag */ {      51,     51,   51,      30,      20,     51},
/* unded*/ {      35,     35,   35,      35,      35,     35},
/* ork  */ {      20,     30,   51,      50,      40,     20},
/* insct*/ {      30,     20,   40,      51,      50,     30},
/* arach*/ {      20,     30,   40,      51,      50,     30},
/* saur */ {      20,     30,   51,      40,      50,     30},
/* fish */ {      20,     40,   30,      50,      51,     30},
/* bird */ {      40,     30,   30,      50,      51,     20},
/* giant*/ {      20,     40,   51,      50,      30,     30},
/* pred */ {      40,     30,   51,      20,      30,     50},
/* para */ {       0,      0,    0,       0,       0,      0},
/* slime*/ {       0,      0,    0,       0,       0,      0},
/* demon*/ {      51,     30,   40,      50,      30,     20},
/* snake*/ {      40,     30,   30,      51,      50,     20},
/* herb */ {      30,     40,   20,      30,      51,     50},
/* tree */ {       0,      0,    0,       0,       0,      0},
/* vegan*/ {       1,      1,   40,       1,      51,      0},
/* elmnt*/ {       0,      0,    0,       0,       0,      0},
/* planr*/ {       0,      0,    0,       0,       0,      0},
/* devil*/ {      50,     51,   40,      20,      30,     40},
/* ghost*/ {       0,      0,    0,       0,       0,      0},
/* gobln*/ {      20,     30,   50,      51,      40,     20},
/* troll*/ {      20,     40,   51,      50,      30,     30},
/* vegmn*/ {       0,      0,   40,       0,      51,      0},
/* mndf */ {      51,      1,   30,      20,       0,      0},
/* prim */ {      30,     30,   40,      50,      20,     51},
/* enfn */ {      50,     30,   51,      40,      20,     30},
/* drow */ {      51,     51,   40,      51,       0,      0},
/* golem*/ {       0,      0,    0,       0,       0,      0},
/* skexi*/ {      50,     25,   20,      51,       0,      0},
/* trog */ {      30,     40,   50,      51,      30,     20},
/* patrn*/ {      51,     30,   50,      30,      20,     40},
/* labrn*/ {      50,     30,   51,      40,      20,     30},
/* sartn*/ {      50,     51,   30,      20,      40,     30},
/* tytn */ {      50,     30,   51,      30,      40,     20},
/* smrf */ {      10,     10,   10,      10,      10,     10},
/* roo */  {      40,     30,   51,      30,      20,     50},
/* horse*/ {      30,     40,   20,      30,      51,     50},
/* drgdm*/ {      51,     51,   30,      30,       0,      0},
/* astrl*/ {       0,      0,    0,       0,       0,      0},
/* god  */ {       0,      0,    0,       0,       0,      0},
/* helf */ {      51,     25,   51,      40,       0,      0},
/* horc */ {       0,     20,   50,      51,       0,      0},
/* hmn2 */ {      51,     51,   51,      51,      51,     51},
/* vamp */ {      51,     51,    1,      20,       0,      0},
/* ogre */ {       0,      0,   51,       0,       0,      0},
/* fairy*/ {      51,      25,  25,      51,       0,      0}
};

/*  fire cold elec blow acid */

int ItemSaveThrows[22][5] = {
  {15, 2, 10, 10, 10},
  {19, 2, 16, 2, 7},
  {11, 2, 2, 13, 9},
  {7,  2, 2, 10, 8},
  {6,  2, 2, 7, 13}, 
  {10, 10, 10, 10, 10},  /* not defined */
  {10, 10, 10, 10, 10},  /* not defined */
  {6,  2, 2, 7, 13},  /* treasure */
  {6,  2, 2, 7, 13},  /* armor */
  {7,  6, 2, 20, 5},  /* potion */
  {10, 10, 10, 10, 10},  /* not defined */
  {10, 10, 10, 10, 10},  /* not defined */
  {10, 10, 10, 10, 10},  /* not defined */
  {10, 10, 10, 10, 10},  /* not defined */
  {19, 2, 2, 16, 7},
  {7,  6, 2, 20, 5},  /* drinkcon */
  {6,  2, 2, 7, 13}, 
  {6,  3, 2, 3, 10},
  {6,  2, 2, 7, 13},  /* treasure */
  {11, 2, 2, 13, 9},
  {7,  2, 2, 10, 8} 
};


const int drink_aff[][3] = {
	{ 0,1,10 },  /* Water    */
	{ 3,2,5 },   /* beer     */
	{ 5,2,5 },   /* wine     */
	{ 2,2,5 },   /* ale      */
	{ 1,2,5 },   /* ale      */
	{ 6,1,4 },   /* Whiskey  */
	{ 0,1,8 },   /* lemonade */
	{ 10,0,0 },  /* firebr   */
	{ 3,3,3 },   /* local    */
	{ 0,4,-8 },  /* juice    */
	{ 0,3,6 },
	{ 0,1,6 },
	{ 0,1,6 },
	{ 0,2,-1 },
	{ 0,1,-2 },
	{ 0,1,5 },
	{ 0, 0, 0}
};

const char *color_liquid[]=
{
	"clear",
	"brown",
	"clear",
	"brown",
	"dark",
	"golden",
	"red",
	"green",
	"clear",
	"light green",
	"white",
	"brown",
	"black",
	"red",
	"clear",
	"black",
	"\n"
};

const char *fullness[] = {
	"less than half ",
	"about half ",
	"more than half ",
	""
};
/* new level titles supplied by alfred */
const struct title_type titles[6][ABS_MAX_LVL] = {
  {   {"Man",         "Woman",               0},
      {"Supplicant",  "Supplicant",          1},
      {"Apprentice",  "Apprentice",          2500},
      {"Student",     "Student",             5000},
      {"Scholar",     "Scholar",             10000},
      {"Trickster",   "Trickster",           20000},
      {"Illusionist", "Illusionist",         40000},
      {"Cabalist",    "Cabalist",            60000},
      {"Apparitionist","Apparitionist",      90000},
      {"Medium",      "Gypsy",               135000},
      {"Scribe",      "Scribe",              250000}, /* 10 */
      {"Sage",        "Sage",                375000},
      {"Seer",        "Seeress",             550000},
      {"Divinater",   "Divinatress",         750000},
      {"Alchemist",   "Alchemist",           950000},
      {"Revoker",      "Evoker",              1200000},
      {"Necromancer", "Necromancer",         1500000},
      {"Abjures",     "Abjuress",            1800000},
      {"Invoker",     "Invoker",             2200000},
      {"Enchanter",   "Enchantress",         2500000},
      {"Conjurer",    "Conjuress",           3000000}, /* 20 */
      {"Summoner",    "Summoner",            4300000},
      {"Magician",    "Magician",            4750000},
      {"Spiritualist","Spiritualist",        5200000},
      {"Savant",      "Savanti",             5650000},
      {"Shaman",      "Shamaness",           6150000},
      {"Mystic",      "Mystic",              6650000},
      {"Wiccen",      "Wicca",               7250000},
      {"Mentalist",   "Mentalist",           8000000},
      {"Mnemonicist", "Mnemonicist",         8750000},
      {"Neuromancer", "Neuromancer",         9500000}, /* 30 */
      {"Spellbinder", "Spellbinder",         10300000},
      {"Synergist",   "Synergist",           11100000},
      {"Oculist",     "Oculist",             11900000},
      {"Thaumaturgist","Thaumaturgist",      12700000},
      {"SpellMaster", "Spell Mistress",      13600000},
      {"Dispeller",   "Dispeller",           14500000},
      {"Warlock",     "War Witch",           15500000},
      {"Sorcerer",    "Sorceress",           16500000},
      {"Wizard",      "Wizard",              17500000},
      {"Great Wizard","Great Wizard",        18500000}, /* 40 */
      {"Magus",       "Incantrix",           19500000},
      {"Serpent Mage","Serpent Mage",        20500000},
      {"Arch Magi",   "Majestrix",           21600000},
      {"Arch Magi",   "Majestrix",           22700000},
      {"Arch Magi",   "Majestrix",           23800000},
      {"Arch Magi",   "Majestrix",           24900000},
      {"Arch Magi",   "Majestrix",           26000000},
      {"Arch Magi",   "Majestrix",           27000000},
      {"Arch Magi",   "Majestrix",           28000000},
      {"Arch Magi",   "Majestrix",           29000000}, /* 50 */
  {"Immortal Warlock","Immortal Enchantress",30000000},
  {"Immortal Warlock","Immortal Enchantress",31000000},
  {"Immortal Warlock","Immortal Enchantress",32000000},
  {"Immortal Warlock","Immortal Enchantress",33000000},
  {"Immortal Warlock","Immortal Enchantress",34000000},
  {"Immortal Warlock","Immortal Enchantress",35000000},
  {"Avatar of Magic", "Empress of Magic",    36000000},
  {"God of magic",    "Goddess of magic",    37000000},
  {"Implementor",     "Implementrix",        38000000},
  {"Implementor",      "Implementrix",       39000000} /* 60 */
},

{   {"Man",         "Woman",                 0},
    {"Layman",      "Laywoman",              1},
    {"Seeker",      "Seeker",                1500},
    {"Believer",    "Believer",              3000},
    {"Novice",      "Novice",                6000},
    {"Initiate",    "Initiate",              13000},
    {"Attendant",   "Attendant",             27500},
    {"Acolyte",     "Acolyte",               55000},
    {"Minion",      "Minion",                110000},
    {"Adept",       "Adept",                 225000},
    {"Priest",      "Priestess",             400000}, /* 10 */
    {"Missionary",  "Missionary",            600000},
    {"Levite",      "Levitess",              800000},
    {"Curate",      "Curate",                1000000},
    {"Chaplain",    "Chaplain",              1200000},
    {"Padre",       "Matron",                1400000},
    {"Canon",       "Canon",                 1600000},
    {"Vicar",       "Vicaress",              1850000},
    {"Deacon",      "Deaconess",             2100000},
    {"Shaman",      "Shamaness",             2400000},
    {"Speaker",     "Speaker",               3000000}, /* 20 */
    {"Confessor",   "Confessor",             3300000},
    {"Expositer",   "Expositress",           3600000},
    {"Mystic",      "Mystic",                3900000},
    {"Hermit",      "Hermitess",             4200000},
    {"Faith Healer","Faith Healer",          4600000},
    {"Healer",      "Healer",                5000000},
    {"Lay Healer",  "Lay Healer",            5400000},
    {"Illuminator", "Illuminatrix",          5900000},
    {"Evangelist",  "Evangelist",            6400000},
    {"Voice",       "Voice",                 6900000}, /* 30 */
    {"Templar",     "Templar",               7500000},
    {"Hospitalier", "Hospitalier",           8100000},
    {"Chuirgeon",   "Chuirgeoness",          8700000},
    {"Chuirgeon General","Chuirgeon General",9400000},
    {"Inquisitor",  "Inquisitrix",          10100000},
    {"Abbot",       "Mother Superior",      10800000},
    {"Reverend",    "Reverend Mother",      11600000},
    {"Bishop",      "Bishop",               12400000},
    {"Arch Bishop", "Arch Bishop",          13200000},
    {"Cardinal",    "Mother Superior",      14100000},
    {"Patriarch",   "Matriarch",            15000000},
    {"Grand Inquisitor","Grand Inquisitrix",15900000},
    {"High Priest", "High Priestess",       17500000},
    {"High Priest", "High Priestess",       19000000},
    {"High Priest", "High Priestess",       20500000},
    {"High Priest", "High Priestess",       22000000},
    {"High Priest", "High Priestess",       23500000},
    {"High Priest", "High Priestess",       25000000},
    {"High Priest", "High Priestess",       26500000},
    {"High Priest", "High Priestess",       28000000},
    
    {"Immortal Cardinal","Immortal Priestess",30000000},
    {"Immortal Cardinal","Immortal Priestess",31000000},
    {"Immortal Cardinal","Immortal Priestess",32000000},
    {"Immortal Cardinal","Immortal Priestess",33000000},
    {"Immortal Cardinal","Immortal Priestess",34000000},
    {"Immortal Cardinal","Immortal Priestess",35000000},
    {"Inquisitor","Inquisitress",             36000000},
    {"God","Goddess",                         37000000},
    {"Implementor","Implementress",           38000000},
    {"Implementor","Implementress",           39000000} },

{ {"Man",         "Woman",         0},
 {"Swordpupil",   "Swordpupil",    1},
 {"Bootlicker",   "Bootlicker",    2000},
 {"Runner",       "Runner",        4000},
 {"Sentry",       "Sentry",        8000},
 {"Man-at-arms",  "Woman-at-arms", 16000},
 {"Mercenary",    "Mercenary",     32000},
 {"Scout",        "Scout",         64000},
 {"Fighter",      "Fighter",       125000},
 {"Warrior",      "Warrior",       250000},
 {"Swordsman",    "Swordswoman",   450000},  /* 10 */
 {"Fencer",       "Fence",         650000},
 {"Combatant",    "Combatrix",     850000},
 {"Protector",    "Protector",     1050000},
 {"Defender",     "Defender",      1300000},
 {"Warder",       "Warder",        1550000},
 {"Guardian",     "Guardian",      1800000},
 {"Veteran",      "Veteran",       2050000},
 {"Hero",         "Heroine",       2300000},
 {"Swashbuckler", "Swashbuckler",  2550000},
 {"Myrmidon",     "Amazon",        2800000},   /* 20 */
 {"Esquire",      "Esquire",       3475000},
 {"Blademaster",  "Blademistress", 3825000},
 {"Reeve",        "Reeve",         4175000},
 {"Lieutenant",   "Lieutenant",    4575000},
 {"Captain",      "Captain",       4975000},
 {"Raider",       "Raider",        5400000},
 {"Champion",     "Lady Champion", 5900000},
 {"Dragoon",      "Lady Dragoon",  6400000},
 {"Armiger",      "Armigress",     6900000},
 {"Srcutifer",    "Scrutifer",     7500000},   /* 30 */
 {"Lancer",       "Lancer",        8100000},
 {"Banneret",     "Banneret",      8700000},
 {"Chevalier",    "Chevaliere",    9400000},
 {"Knight Errant","Valkyrie",      10100000},
 {"Knight",       "Lady Knight",   10800000},
 {"Marshall",     "Lady Marshall", 11500000},
 {"Keitar",       "Lady Keitar",   12200000},
 {"Paladin",      "Lady Paladin",  12900000},
 {"Justiciar",    "Justictrix",    13700000},
 {"Grand Marshall","Grand Marshall",14500000},
 {"Grand Knight", "Grand Knight",  15300000},
 {"Grand Paladin","Grand Paladin", 16300000},
 {"Lord",         "Lady",          17300000},
 {"Lord",         "Lady",          18300000},
 {"Lord",         "Lady",          20000000},
 {"Lord",         "Lady",          21500000},
 {"Lord",         "Lady",          23000000},
 {"Lord",         "Lady",          24500000},
 {"Lord",         "Lady",          26000000},
 {"Lord",         "Lady",          27500000},
 {"Immortal",     "Immortal",      30000000},
 {"Immortal",     "Immortal",      31000000},
 {"Immortal",     "Immortal",      32000000},
 {"Immortal",     "Immortal",      33000000},
 {"Immortal",     "Immortal",      34000000},
 {"Immortal",     "Immortal",      35000000},
 {"Extirpator",   "Queen",         36000000},
 {"God",          "Goddess",       37000000},
 {"Implementor",  "Implementrix",  38000000},
 {"Implementor",  "Implementrix",  39000000} },

{   {"Man",          "Woman",           0},
    {"Delinquent",   "Delinquent",      1},
    {"Miscreant",    "Miscreant",       1250},
    {"Footpad",      "Footpad",         2500},
    {"Pilferer",     "Pilferess",       5000},
    {"Filcher",      "Filcheress",      10000},
    {"Pincher",      "Pincheress",      20000},
    {"Snatcher",     "Snatcheress",     30000},
    {"Ninja",        "Ninja",           60000},
    {"Pick-Pocket",  "Pick-Pocket",     100000},
    {"Cut-Purse",    "Cut-Purse",       140000},  /* 10 */
    {"Sharper",      "Sharper",         200000},
    {"Burgler",      "Burgler",         300000},
    {"Robber",       "Robber",          420000},
    {"Mugger",       "Mugger",          540000},
    {"Magsman",      "Magswoman",       660000},
    {"Bandito",      "Bandita",         780000},
    {"Highwayman",   "Bandit",          930000},
    {"Brigand",      "Brigand",         1080000},
    {"Agent",        "Agent",           1230000},
    {"Outlaw",       "Outlaw",          1900000}, /* 20 */
    {"Blade",        "Blade",           2600000},
    {"Quick-Blade",  "Quick-Blade",     2900000},
    {"Knifer",       "Knifer",          3200000},
    {"Sneak",        "Sneak",           3500000},
    {"Thief",        "Thief",           3800000},
    {"Special Agent","Special Agent",   4150000},
    {"Collector",    "Collector",       4500000},
    {"Hand",         "Hand",            4850000},
    {"Unseen",       "Unseen",          5200000},
    {"Thug",         "Thug",            5550000},
    {"Cut-Throat",   "Cut-Throat",      5900000},
    {"Grand-Thief",  "Grand-Thief",     6300000},
    {"Repossessor",  "Repossessor",     6700000},
    {"Killer",       "Killer",          7100000},
    {"Secret Agent", "Secret Agent",    7500000},
    {"Renegade",     "Renegade",        7900000},
    {"Murderer",     "Murderess",       8300000},
    {"Butcher",      "Butcheress",      8750000},
    {"Slayer",       "Slayer",          9200000},
    {"Executioner",  "Executioner",     9650000},
    {"Assassin",     "Assassin",        10075000},
    {"Master Assassin","Mistress Assassin",10575000},
    {"Master Thief", "Master Thief",    11075000},
    {"Master Thief", "Master Thief",    11575000},
    {"Master Thief", "Master Thief",    12075000},
    {"Master Thief", "Master Thief",    12575000},
    {"Master Thief", "Master Thief",    13075000},
    {"Master Thief", "Master Thief",    14000000},
    {"Master Thief", "Master Thief",    15000000},
    {"Master Thief", "Master Thief",    16000000},
    {"Immortal Assasin","Immortal Assasin",30000000},
    {"Immortal Assasin","Immortal Assasin",31000000},
    {"Immortal Assasin","Immortal Assasin",32000000},
    {"Immortal Assasin","Immortal Assasin",33000000},
    {"Immortal Assasin","Immortal Assasin",34000000},
    {"Immortal Assasin","Immortal Assasin",35000000},
    {" Demi God","Demi Goddess",           41000000},
    {" God","Goddess",                     42000000},
    {"Implementor","Implementrix",         43000000},
    {"Implementor","Implementrix",         44000000} },

{   {"Man"," Woman",0},
    {"Aspirant","Aspirant",                              1},
    {"Ovate","Ovate",                                 2000},
    {"Sprout","Sprout",                               4000},
    {"Courser","Courser",                             7500},
    {"Tracker","Tracker",                            12500},
    {"Guide","Guide",                                20000},
    {"Pathfinder","Pathfinder",                      35000},
    {"Green-Peace Keeper","Green-Peace Keeper",      60000},
    {"Warder","Warder",                              90000},
    {"Pagan","Pagan",                               125000},  /* 10 */
    {"Watcher","Watcher",                           200000},
    {"Woodsman","Woodsman",                         300000},
    {"Pict","Pict",                                 425000},
    {"Tree Hugger","Tree Hugger",                   550000},
    {"Celt","Celt",                                 675000},
    {"Animist","Animist",                           900000},
    {"Hedge","Hedge",                              1150000},
    {"Tender","Tender",                            1400000},
    {"Strider","Strider",                          2000000},
    {"Druid","Druidess",                           2300000},  /* 20 */
    {"Aquarian","Aquarian",                        2600000},
    {"Arbolist","Arbolist",                        2900000},
    {"Dionysian","Dionysian",                      3400000},
    {"Herbalist","Herbalist",                      3800000},
    {"Naturalist","Naturalist",                    4200000},
    {"Silvian","Silvian",                          4600000},
    {"Forrestal","Forrestal",                      5100000},
    {"Ancient","Ancient",                          5600000},
    {"Archdruid","Archdruidess",                   6200000},
    {"Great Druid","Great Druidess",               6800000}, /*30*/
    {"Grand Druid","Grand Druid",                  7500000},
    {"Master of Fire","Mistress of Fire",          8200000},
    {"Master of Stone","Mistress of Stone",        9050000},
    {"Master of Air","Mistress of Air",            9800000},
    {"Master of Water","Mistress of Water",        10700000},
    {"Hierophant Initiate","Hierophant Initiate",  11600000},
    {"Hierophant Druid","Heirophant Druidess",     12600000},
    {"Hierophant Adept","Heirophant Adept",        13600000},
    {"Heirophant Master","Heirophant Master",      14700000},
    {"Numinous Hierophant", "Numinous Hierophant", 15800000},/*40*/
    {"Mystic Hierophant","Mystic Hierophant",      16900000},
    {"Arcane Hierophant", "Arcane Hierophant",     18100000},
    {"Cabal Hierophant", "Cabal Hierophant",       19300000},
    {"Cabal Hierophant", "Cabal Hieorphant",       21500000},
    {"Cabal Hierophant", "Cabal Hierophant",       22700000},
    {"Cabal Hierophant", "Cabal Hierophant",       24000000},
    {"Cabal Hierophant", "Cabal Hierophant",       25300000},
    {"Cabal Hierophant", "Cabal Hierophant",       26700000},
    {"Cabal Hierophant", "Cabal Hierophant",       28100000},
    {"Cabal Hierophant", "Cabal Hierophant",       29500000},
    {"Immortal Hierophant","Immortal Hierophant",  31000000},
    {"Immortal Hierophant","Immortal Hierophant",  36000000},
    {"Immortal Hierophant","Immortal Hierophant",  37000000},
    {"Immortal Hierophant","Immortal Hierophant",  38000000},
    {"Immortal Hierophant","Immortal Hierophant",  39000000},
    {"Immortal Hierophant","Immortal Hierophant",  40000000},
    {"Demi God","Demi Goddess",                    41000000},
    {"God","Goddess",                              42000000},
    {"Implementor","Implementrix",                 43000000},
    {"Implementor","Implementrix",                 44000000} 
},

{   {"Man"," Woman",                                       0},
    {"White Belt","White Belt",                            1},
    {"Initiate","Initiate",                                1000},
    {"Brother","Sister",                                   2000},
    {"Layman","Laywoman",                                  4250},
    {"Student","Student",                                  8500},
    {"Practitioner","Practitioner",                        17000},
    {"Exponent","Exponent",                                34000},
    {"Adept","Adept",                                      68000},
    {"Monk","Monk",                                        98000},
    {"Shodan","Shodan",                                    200000}, /* 10 */
    {"Shinobi","Shinobi",                                  325000},
    {"Genin","Kuniochi",                                   450000},
    {"Disciple","Disciple",                                575000},
    {"Chunin","Chunin",                                    725000},
    {"Nidan","Nidan",                                      925000},
    {"Expert","Expert",                                   1200000},
    {"Jonin","Jonin",                                     1500000},
    {"Hwarang","Hwarang",                                 2100000},
    {"Sandan","Sandan",                                   2500000},
    {"Sabom","Sabom",                                     2900000}, /* 20 */
    {"Sensei", "Sensei",                                  3300000},
    {"Sifu", "Sifu",                                      3800000},
    {"Guru", "Guru",                                      4400000},
    {"Pendakar", "Pendakar",                              5000000},
    {"Yodan", "Yodan",                                    5600000},
    {"Master", "Master",                                  6200000},
    {"Superior Master", "Superior Master",                6900000},
    {"Ginsu Master", "Ginsu Master",                      7600000},
    {"Godan", "Godan",                                    8300000},
    {"Leopard Master", "Leopard Master",                  8900000}, /*30*/
    {"Tiger Master", "Tiger Master",                      9700000},
    {"Snake Master", "Snake Master",                     10500000},
    {"Crane Master", "Crane Master",                     11300000},
    {"Dragon Master", "Dragon Master",                   12100000},
    {"Rokudan", "Rokudan",                               13000000},
    {"Master of Seasons", "Master of Seasons",           13900000},
    {"Master of the Winds", "Master of the Winds",       14800000},
    {"Master of Harmony", "Master of Harmony",           15700000},
    {"Shogun", "Shogun",                                 16700000},
    {"Shichidan", "Shichidan",                           17700000},/*40*/
    {"Hachidan", "Hachidan",                             18700000},
    {"Kudan", "Kudan",                                   19700000},
    {"Grand Master", "Grand Master",                     21000000},
    {"Grand Master", "Grand Master",                     22000000},
    {"Grand Master", "Grand Master",                     23000000},
    {"Grand Master", "Grand Master",                     24000000},
    {"Grand Master", "Grand Master",                     25000000},
    {"Grand Master", "Grand Master",                     26000000},
    {"Grand Master", "Grand Master",                     27000000},
    {"Grand Master", "Grand Master",                     28000000},
    {"Immortal Grand Master","Immortal Grand Master",    30000000},
    {"Immortal Grand Master","Immortal Grand Master",    31000000},
    {"Immortal Grand Master","Immortal Grand Master",    32000000},
    {"Immortal Grand Master","Immortal Grand Master",    33000000},
    {"Immortal Grand Master","Immortal Grand Master",    34000000},
    {"Immortal Grand Master","Immortal Grand Master",    35000000},
    {"Demi God","Demi Goddess",                          36000000},
    {"God","Goddess",                                    37000000},
    {"Implementor","Implementrix",                       38000000},
    {"Implementor","Implementrix",                       39000000}

  }
};



const char *RaceName[] = {
  "Half-Breed",
  "Human",
  "Elven",
  "Dwarven",
  "Halfling",
  "Gnome",
  "Reptilian",
  "Mysterion",
  "Lycanthropian",
  "Draconian",
  "Undead",
  "Orcish",
  "Insectoid",
  "Arachnoid",
  "Saurian",
  "Icthyoid",
  "Avian",
  "Giant",
  "Carnivorous",
  "Parasitic",
  "Slime",
  "Demonic",
  "Snake",
  "Herbivorous",
  "Tree",
  "Vegatable",
  "Elemental",
  "Planar",
  "Diabolic",
  "Ghostly",
  "Goblinoid",
  "Trollish",
  "Veggie",
  "Mindflayer",
  "Primate",
  "Enfan",
  "Drow",
  "Golem",
  "Skexie",
  "Troglodyte",
  "Patryn",
  "Labrynthian",
  "Sartan",
  "Tytan",
  "Smurf",
  "Kangaroo",
  "Horse",
  "Ratperson",
  "Astralion",
  "God",
  "Half-Elven",
  "Half-Orc",
  "Human",			/* needed a 51 all max race */
  "Vampire",
  "Ogre",
  "Fairy",
  "\n"
};

const char *item_types[] = {
	"UNDEFINED",
	"LIGHT",
	"SCROLL",
	"WAND",
	"STAFF",
	"WEAPON",
	"FIRE WEAPON",
	"MISSILE",
	"TREASURE",
	"ARMOR",
	"POTION",
	"WORN",
	"OTHER",
	"TRASH",
	"TRAP",
	"CONTAINER",
	"NOTE",
	"LIQUID CONTAINER",
	"KEY",
	"FOOD",
	"MONEY",
	"PEN",
	"BOAT",
        "AUDIO",
        "BOARD",
        "TREE",
        "ROCK",
	"\n"
};

const char *wear_bits[] = {
	"TAKE",
	"FINGER",
	"NECK",
	"BODY",
	"HEAD",
	"LEGS",
	"FEET",
	"HANDS",
	"ARMS",
	"SHIELD",
	"ABOUT",
	"WAIST",
	"WRIST",
	"WIELD",
	"HOLD",
	"THROW",
	"LIGHT-SOURCE",
	"\n"
};

const char *extra_bits[] = {
	"GLOW",
	"HUM",
	"METAL",
	"MINERAL",
	"ORGANIC",
	"INVISIBLE",
	"MAGIC",
	"NODROP",
	"BLESS",
	"ANTI-GOOD",
	"ANTI-EVIL",
	"ANTI-NEUTRAL",
        "ANTI-CLERIC",
        "ANTI-MAGE",
        "ANTI-THIEF",
        "ANTI-WARRIOR",
        "BRITTLE",
        "RESISTANT",
	"ILLUSION",
        "ANTI-MEN",
        "ANTI-WOMEN",
	"\n"
};

const char *room_bits[] = {
	"DARK",
	"DEATH",
	"NO_MOB",
	"INDOORS",
	"PEACEFUL",
	"NOSTEAL",
	"NO_SUM",
	"NO_MAGIC",
	"TUNNEL",
	"PRIVATE",
        "SILENCE",
        "LARGE",
        "NO_DEATH",
        "SAVE_ROOM",
	"\n"
};

const char *exit_bits[] = {
	"IS-DOOR",
	"CLOSED",
	"LOCKED",
        "SECRECT",
        "RSLOCKED",
        "PICKPROOF",
        "CLIMB",
	"\n"
};

const char *sector_types[] = {
	"Inside",
	"City",
	"Field",
	"Forest",
	"Hills",
	"Mountains",
	"Water Swim",
	"Water NoSwim",
        "Air",
        "Underwater",
        "Desert",
        "Tree",
	"\n"
};

const char *equipment_types[] = {
	"Special",
	"Worn on right finger",
	"Worn on left finger",
	"First worn around Neck",
	"Second worn around Neck",
	"Worn on body",
	"Worn on head",
	"Worn on legs",
	"Worn on feet",
	"Worn on hands",
	"Worn on arms",
	"Worn as shield",
	"Worn about body",
	"Worn around waist",
	"Worn around right wrist",
	"Worn around left wrist",
	"Wielded",
	"Held",
	"\n"
};
	
const char *affected_bits[] = 
{	
  "BLIND",
  "INVISIBLE",
  "DETECT-EVIL",
  "DETECT-INVISIBLE",
  "DETECT-MAGIC",
  "SENSE-LIFE",
  "HOLD",
  "SANCTUARY",
  "DRAGON-RIDE",
  "GROWTH",
  "CURSE",
  "FLYING",
  "POISON",
  "TREE-TRAVEL",
  "PARALYSIS",
  "INFRAVISION",
  "WATER-BREATH",
  "SLEEP",
  "TRAVELING",
  "SNEAK",
  "HIDE",
  "SILENCE",
  "CHARM",
  "FOLLOW",
  "SAVED-OBJECTS",
  "TRUE-SIGHT",
  "SCRYING",
  "FIRESHIELD",
  "GROUP",
  "UNDEF_6",
  "TEAM-GREY",
  "TEAM-AMBER",
  "\n"
};

const char *affected_bits2[] = 
{
  "INVIS-TO-ANIMALS",
  "HEAT-STUFF",
  "LOGGED",
  "ONE-LIFER",
  "SUN-BLIND",
  "FEEDING",
  "BERSERK",
  "\n"
};

const char *immunity_names[] = 
{	"FIRE",
	"COLD",
	"ELECTRICITY",
	"ENERGY",
	"BLUNT",
	"PIERCE",
        "SLASH",
	"ACID",
	"POISON",
	"DRAIN",
	"SLEEP",
	"CHARM",
	"HOLD",
	"NON-MAGIC",
	"+1",
	"+2",
	"+3",
	"+4",
	"\n"
};

const char *apply_types[] = {
	"NONE",			/* 0 */
	"STR",
	"DEX",
	"INT",
	"WIS",
	"CON",			/* 5 */
	"CHR",
	"SEX",
	"LEVEL",
	"AGE",
	"CHAR_WEIGHT",		/* 10 */
	"CHAR_HEIGHT",
	"MANA",
	"HIT",
	"MOVE",
	"GOLD",			/* 15 */
	"EXP",
	"ARMOR",
	"HITROLL",
	"DAMROLL",
	"SAVING_PARA",		/* 20 */
	"SAVING_ROD",
	"SAVING_PETRI",
	"SAVING_BREATH",
	"SAVING_SPELL",
	"SAVING_ALL",		/* 25 */
	"RESISTANCE",
	"SUSCEPTIBILITY",
	"IMMUNITY",
	"SPELL AFFECT",
	"WEAPON SPELL",		/* 30 */
	"EAT SPELL",
	"BACKSTAB",
	"KICK",
	"SNEAK",
	"HIDE",			/* 35 */
	"BASH",
	"PICK",
	"STEAL",
	"TRACK",
        "HIT-N-DAM",		/* 40 */
	"SPELLFAIL",
	"ATTACKS",
	"HASTE",
	"SLOW",
	"OTHER",		/* 45 */
	"FIND-TRAPS",
	"RIDE",
	"RACE_SLAYER",
        "ALIGN_SLAYER",
        "MANA_REGEN",		/* 50 */
        "HIT_REGEN",
        "MOVE_REGEN",
	"MOVE_BONUS",
	"INTRINSIC",
	"\n"
};

const char *pc_class_types[] = {
       	"Magic User",
	"Cleric",
	"Warrior",
	"Thief",
	"Druid",
	"Monk",
	"\n"
};

const char *npc_class_types[] = {
	"Normal",
	"Undead",
	"\n"
};

const char *action_bits[] = {
	"SPEC",
	"SENTINEL",
	"SCAVENGER",
	"ISNPC",
	"NICE-THIEF",
	"AGGRESSIVE",
	"STAY-ZONE",
	"WIMPY",
        "ANNOYING",
	"HATEFUL",
	"AFRAID",
	"IMMORTAL",
	"HUNTING",
        "DEADLY",
	"POLYMORPHED",
	"META_AGGRESSIVE",
	"GUARDING",
        "ILLUSION",
        "HUGE",
        "SCRIPT",
        "GREET",
        "FIGURINE",
        "BRIEF",
        "SHOWEXITS",
	"\n"
};


const char *player_bits[] = {
	"BRIEF",
	"",
	"COMPACT",
	"DONTSET",
        "WIMPY",
	"NOHASSLE",
	"STEALTH",
        "HUNTING",
	"DEAF",
	"ECHO",
        "SHOWEXITS",
	"",
	"",
	"",
	"NOSHOUT",
        "",
        "",
        "",
        "",
        "",
        "NOBEEP",
	"\n"
};


const char *position_types[] = {
	"Dead",
	"Mortally wounded",
	"Incapacitated",
	"Stunned",
	"Sleeping",
	"Resting",
	"Sitting",
	"Fighting",
	"Standing",
	"\n"
};

const char *connected_types[]	=	{
	"Playing",
	"Get name",
	"Confirm name",
	"Read Password",
	"Get new password",
	"Confirm new password",
	"Get sex",
	"Read messages of today",
	"Read Menu",
	"Get extra description",
	"Get class",
        "Link Dead",
        "New Password",
        "Password Confirm",
        "Wizlocked",
        "Get Race",
        "Racpar",
        "Auth",
        "City Choice",
        "Stat Order",
        "Delete",
        "Delete",
        "Stat Order",
        "Wizard MOTD",
        "Editing",
	"\n"
};

/* [class], [level] (all) */
const int thaco[MAX_CLASS][ABS_MAX_LVL] = {
/* mage */
   { 100,20,20,20,20,20,19,19,19,19,19,18,18,18,18,18,17,17,17,17,17,16,16,16,16
,16,15,15,15,15,15,14,14,14,14,14,13,13,13,13,13,12,12,12,12,12,11,11,11,11,11,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
/* cleric */
   { 100,20,20,20,19,19,19,18,18,18,17,17,17,16,16,16,15,15,15,14,14,14,13,13,13
,12,12,12,11,11,11,10,10,10,9,9,9,8,8,8,7,7,7,6,6,6,5,5,5,4,4,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1},
/* fighter */
   { 100,20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,11,10,10,9,9,8
,8,7,7,6,6,5,5,4,4,3,3,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1},
/* thief */
   { 100,20,20,20,19,19,19,19,18,18,18,17,17,17,17,16,16,16,15,15,15,15,14,14,14
,13,13,13,13,12,12,12,11,11,11,11,10,10,10,9,9,9,9,8,8,8,7,7,7,7,6,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1},
/* druid */
   { 100,20,20,20,19,19,19,18,18,18,17,17,17,16,16,16,15,15,15,14,14,14,13,13,13
,12,12,12,11,11,11,10,10,10,9,9,9,8,8,8,7,7,7,6,6,6,5,5,5,4,4,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1},
/* monk  */
   { 100,20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,11,10,10,9,9,8
,8,7,7,6,6,5,5,4,4,3,3,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1}

};

/* [ch] strength apply (all) */
const struct str_app_type str_app[31] = {
	{ -5,-4,   0,  0 },  /* 0  */
	{ -5,-4,   1,  1 },  /* 1  */
	{ -3,-2,   1,  2 },
	{ -3,-1,   5,  3 },  /* 3  */
	{ -2,-1,  10,  4 },
	{ -2,-1,  20,  5 },  /* 5  */
	{ -1, 0,  25,  6 },
	{ -1, 0,  30,  7 },
	{  0, 0,  40,  8 },
	{  0, 0,  50,  9 },
	{  0, 0,  55, 10 }, /* 10  */
	{  0, 0,  70, 11 },
	{  0, 0,  80, 12 },
	{  0, 0,  90, 13 },
	{  0, 0, 100, 14 },
	{  0, 0, 110, 15 }, /* 15  */
	{  0, 1, 120, 16 },
	{  1, 1, 130, 18 },
	{  1, 2, 140, 20 }, /* 18  */
	{  3, 7, 485, 40 },
	{  3, 8, 535, 40 }, /* 20  */
	{  4, 9, 635, 40 },
	{  4,10, 785, 40 },
	{  5,11, 935, 40 },
	{  6,12,1235, 40 },
	{  7,14,1535, 40 }, /* 25            */
	{  1, 3, 155, 22 }, /* 18/01-50      */
	{  2, 3, 170, 24 }, /* 18/51-75      */
	{  2, 4, 185, 26 }, /* 18/76-90      */
	{  2, 5, 255, 28 }, /* 18/91-99      */
	{  3, 6, 355, 30 }  /* 18/100   (30) */
};

/* [dex] skillapply (thieves only) */
const struct dex_skill_type dex_app_skill[26] = {
	{-99,-99,-90,-99,-60},   /* 0 */
	{-90,-90,-60,-90,-50},   /* 1 */
	{-80,-80,-40,-80,-45},
	{-70,-70,-30,-70,-40},
	{-60,-60,-30,-60,-35},
	{-50,-50,-20,-50,-30},   /* 5 */
	{-40,-40,-20,-40,-25},
	{-30,-30,-15,-30,-20},
	{-20,-20,-15,-20,-15},
	{-15,-10,-10,-20,-10},
	{-10, -5,-10,-15, -5},   /* 10 */
	{ -5,  0, -5,-10,  0},
	{  0,  0,  0, -5,  0},
	{  0,  0,  0,  0,  0},
	{  0,  0,  0,  0,  0},
	{  0,  0,  0,  0,  0},   /* 15 */
	{  0,  5,  0,  0,  0},
	{  5, 10,  0,  5,  5},
	{ 10, 15,  5, 10, 10},
	{ 15, 20, 10, 15, 15},
	{ 15, 20, 10, 15, 15},   /* 20 */
	{ 20, 25, 10, 15, 20},
	{ 20, 25, 15, 20, 20},
	{ 25, 25, 15, 20, 20},
	{ 25, 30, 15, 25, 25},
	{ 25, 30, 15, 25, 25}    /* 25 */
};

/* [level] backstab multiplyer (thieves only) */
const byte backstab_mult[ABS_MAX_LVL] = {
	1,   /* 0 */
	2,   /* 1 */
	2,
	2,
	2,
	2,   /* 5 */
	2,
	2,
	3,   /* 8 */
	3,
	3,   /* 10 */
	3,
	3,
	3,
	3,
	3,   /* 15 */
	4,			/* 16 */
	4,
	4,
	4,
	4,   /* 20 */
	4,
	4,
	4,
	5,    /* 25 */
	5,
	5,
	5,
	5,
	5,   /* 30 */
	5,
	5,
	6,
	6,
	6,   /* 35 */
	6,
	6,
	6,
	7,
	7,  /* 40 */
	7,
	7,
	7,
	8,
	8,  /* 45 */
	8,
	8,
	8,
	9,
	10,  /* 50? */
        11,
        11,
        12,
        12,
        13,
        14,
        15,
        16,
        17,
        18, /* 60 */
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28  /* 70 */

};

/* [dex] apply (all) */
struct dex_app_type dex_app[26] = {
	{-7,-7, 60},   /* 0 */
	{-6,-6, 50},   /* 1 */
	{-4,-4, 50},
	{-3,-3, 40},
	{-2,-2, 30},
	{-1,-1, 20},   /* 5 */
	{ 0, 0, 10},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},   /* 10 */
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0,-10},   /* 15 */
	{ 1, 1,-20},
	{ 2, 2,-30},
	{ 2, 2,-40},
	{ 3, 3,-40},
	{ 3, 3,-40},   /* 20 */
	{ 4, 4,-50},
	{ 4, 4,-50},
	{ 4, 4,-50},
	{ 5, 5,-60},
	{ 5, 5,-60}    /* 25 */
};

/* [con] apply (all) */
struct con_app_type con_app[26] = {
	{-4,20},   /* 0 */
	{-3,25},   /* 1 */
	{-2,30},
	{-2,35},
	{-1,40},
	{-1,45},   /* 5 */
	{-1,50},
	{ 0,55},
	{ 0,60},
	{ 0,65},
	{ 0,70},   /* 10 */
	{ 0,75},
	{ 0,80},
	{ 0,85},
	{ 0,88},
	{ 1,90},   /* 15 */
	{ 2,95},		/* 16 */
	{ 3,97},		/* 17 */
	{ 4,99},		/* 18 */
	{ 4,99},
	{ 5,99},   /* 20 */
	{ 6,99},
	{ 6,99},
	{ 7,99},
	{ 8,99},
	{ 9,100}   /* 25 */
};

/* [int] apply (all) */
struct int_app_type int_app[26] = {
	0,
	1,    /* 1 */
	2,
	3,
	4,
	5,   /* 5 */
	6,
	8,
	10,
	12,
	14,   /* 10 */
	16,
	18,
	20,
	22,
	25,   /* 15 */
	28,
	32,
	35,
	40,
	45,   /* 20 */
	50,
	60,
	70,
	80,
	99    /* 25 */
};

/* [wis] apply (all) */
struct wis_app_type wis_app[26] = {
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	3,
	3,
	4,
	5,
	5,
	5,
	6,
	6,
	6,
	6,
	8
};

struct chr_app_type chr_apply[26] = {
        { 0, -70},		/* 0 */
        { 0, -70},
        { 1, -60},
        { 1, -50},
        { 1, -40},		/* 4 */
        { 2, -30},
        { 2, -20},
        { 3, -10},
        { 4,  0},	
        { 5,  0},		/* 9 */
        { 6,  0},	
        { 7,  0},	
        { 8,  0},	
        { 9, +5},		/* 13 */
        { 10, +10},	
        { 12, +15},	
        { 14, +20},	
        { 17, +25},	
        { 20, +30}, /* 18 */
        {20, +40},
        {25, +50},
        {25, +60},
        {25, +70},
        {25, +80},
        {25, +90},
        {25, +95},
};


const char *spell_desc[] = {
  "!ERROR!",
  "$n is surrounded by a armor-like magical shield",
  "!Teleport!",
  "",
  "$n is temporarily blinded",
  "",
  "!Call Lightning",
  "",
  "!Chill Touch!",
  "!Clone!",
  "!Color Spray!",
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure Blind!",
  "!Cure Critic!",
  "!Cure Light!",
  "",
  "$n's eyes glow white",
  "$n's eyes glow yellow",
  "",
  "",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",
  "!Fireball!",
  "!Harm!",
  "!Heal",
  "",
  "!Lightning Bolt!",
  "!Locate object!",
  "!Magic Missile!",		/* haha, made you look! */
  "$n looks very ill.",
  "",
  "!Remove Curse!",
  "$n is surrounded by a white aura",
  "!Shocking Grasp!",
  "",
  "",
  "!Summon!",
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "",
  "",  /* NO MESSAGE FOR SNEAK*/
  "!Hide!",
  "!Steal!",
  "!Backstab!",
  "!Pick Lock!",
  "!Kick!",
  "!Bash!",
  "!Rescue!",
  "!Identify!",  /* 53 */
  "$n's eyes glow redly",
  "!cause light!",
  "!cause crit!",
  "!flamestrike!",
  "!dispel good!",
  "$n looks weak and pitiful",
  "!dispel magic!",
  "!knock!",
  "!know alignment!",
  "!animate dead!",
  "$n seems unable to move.",
  "!remove paralysis!",
  "!fear!",
  "!acid blast!",
  "$n looks rather fishy",
  "$n is flying",
  "spell1, please report.",   /* 70 */
  "spell2, please report.",
  "spell72, please report",
  "$n is surrounded by a weak shield of magic.",
  "spell74, please report",
  "spell6, please report.",
  "spell7, please report.",
  "spell8, please report.",
  "spell9, please report.",
  "spell10, please report.",
  "spell11, please report.",  /* 80  */
  "$n is surrounded by glowing red flames!",
  "spell82, please report.",
  "spell83, please report.",
  "spell84, please report.",
  "spell85, please report.",
  "spell86, please report.",
  "spell87, please report.",
  "spell88, please report.",
  "spell89, please report.",
  "spell90, please report.",  /* 90 */
  "spell91, please report.",
  "$n's skin has taken on the consitency of hard granite",
  "spell93, please report.",
  "$n's eyes glow with a silvery hue",
  "spell95, please report",
  "$n is surrounded by a pink aura",   /* 96 */
  "spell 97, please report.",
  "spell 98 please report." ,
  "spell 99 please report." ,
  "spell 100 please report.",
  "spell 101 please report.",
  "spell 102 please report.",
  "spell 103 please report.",
  "$n is flying",
  "spell 105 please report.",
  "$n is covered by sticky webs.",
  "",
  "",   /* 108 */
  "spell 109 please report.",
  "",
  "spell 111 please report.",
  "spell 112 please report.",
  "spell 113 please report.",
  "spell 114 please report.",
  "spell 115 please report.",
  "spell 116 please report.",
  "spell 117 please report.",
  "$e seems aided by divine blessing",
  "spell 119 please report.",
  "spell 120 please report.",
  "spell 121 please report.",
  "spell 122 please report.",
  "spell 123 please report.",
  "spell 124 please report.",
  "spell 125 please report.",
  "spell 126 please report.",
  "spell 127 please report.",
  "spell 128 please report.",
  "spell 129 please report.",
  "spell 130 please report.",
  "spell 131 please report.",
  "spell 132 please report.",
  "spell 133 please report.",
  "spell 134 please report.",
  "spell 135 please report.",
  "spell 136 please report.",
  "spell 137 please report.",
  "spell 138 please report.",
  "spell 139 please report.",
  "spell 140 please report.",
  "$n seems a bit shadowy",
  "spell 142 please report.",
  "spell 143 please report.",
  "",
  "spell 145 please report.",
  "spell 146 please report.",
  "spell 147 please report.",
  "spell 148 please report.",
  "$n's skin seems rough, and brown, like bark!",
  "spell 150 please report.",
  "spell 151 please report.",
  "$n's armor is smoking and sizzling",
  "spell 153 please report.",
  "spell 154 please report.",
  "$e seems to be moving very rapidly",
  "$n seems to be slowed",
  "",
  "spell 158 please report.",
  "spell 159 please report.",
  "spell 160 please report.",
  "",
  "spell 162 please report.",
  "spell 163 please report.",
  "spell 164 please report.",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "\n"  
};

const struct QuestItem QuestList[4][IMMORTAL] = {
  {  /* magic user */
    {0, ""},
    {2, "It can be found in the donation room, or on your head\n\r"},
    {1410, "Its a heavy bag of white powder\n\r"},
    {6010, "You can make pies out of them, deer seem to like them too\n\r"},
    {3013, "Its a yummy breakfast food, they go great with eggs and cheese\n\r"},
    {20, "If you twiddle your thumbs enough, you'll find one.\n\r"},
    {24764, "Dead people might wear them on their hands\n\r"},
    {112, "If you found one of these, it would be mighty strange!\n\r"},
    {106, "Eye of Toad and Toe of Frog, bring me one or the other\n\r"},
    {109, "A child's favorite place holds the object of my desire, on the dark river\n\r"},
/* 10 */
    {3628, "The latest in New padded footwear\n\r"},
    {113, "A child might play with one, when the skexies aren't around\n\r"},
    {19204, "A precious moon in a misty castle\n\r"},
    {20006, "Are you a fly? You might run into one. Beware..\n\r"},
    {1109,  "Little people have tiny weapons.. bring me one\n\r"},
    {6203,  "IReallyReallyWantACurvedBlade\n\r"},
    {21007, "I want to be taller and younger, find the nasty children\n\r"},
    {5228,  "Don't you find screaming women so disarming?\n\r"},
    {7204,  "Vaulted lightning\n\r"},
    {16043, "Precious elements can come in dull, dark mines\n\r"},
/* 20 */
    {20007, "You'll catch him napping, no guardian of passing time."},
    {16903, "Nature's mistake, carried by a man on a new moon, fish on full."},
    {5226,  "Sealed in the hands of a city's failed guardian."},
    {10900, "Anachronistic rectangular receptacle holds circular plane."}, /* Sentry, TL */
    {13840, "What kind of conditioner does one use for asps?"}, /* Medusa, GQ */
    {7406,  "If you don't bring a scroll of recall, you might die a fiery death"}, /* Room 7284, SM */
    {120,   "Dock down anchor"}, /* Ixitxachitl, NT */
    {21008, "Very useful, behind a hearth."}, /* Dog, OR */
    {10002, "He didn't put them to sleep with these, the results were quite deadly"}, /* On Corpse, DCE */
    {3648, "Unsummoned, they pose large problems. What you want is on their queen."}, /* Chieftess, HGS */
    {15805, "A single sample of fine plumage, held by a guard and a ghost."}, /* Guard & Brack, SK */
    {21141, "In the land of the troglodytes there is a headpiece with unpleasant powers."}, /* Case, TR */
    {1532, "Three witches have the flighty component you need."}, /* Pot, MT */
    {5304, "A spectral force holds the key to advancement in a geometric dead end."}, /* Spectre, PY */
    {9496,  "A great golden sword was once taken by the giants of the great with north.  return it. Bring friends."},
    {5105, "What you need is as dark as the heart of the elf who wields it."}, /* Weaponsmaster, DR */
    {21011, "The key to your current problem is in Orshingal, on a haughty strutter."}, /* Enfan, OR */
    {27004, "A small explosive pinapple shaped object. Ever see Monty-Python?, you might find it in the hands of sorcerous undead men"}, /* ???, Haplo's */
    {6616, "You might smile if drinking a can of this. Look in Prydain."}, /* ???, PRY */
    {21125, "With enough of this strong amber drink you'd forget about the nightly ghosts."}, /* Crate, ARO */
/* 40 */
    {5309,"Powerful items of magic follow... first bring me a medallion of mana\n\r"},
    {1585,"Bubble bubble, toil and trouble, bring me a staff on the double\n\r"},
    {21003,"I need some good boots, you know how strange it is to find them\n\r"},
    {13704,"Watch for a dragon, find his ring\n\r"},
    {252,  "Dead-makers cloaks, buried deep in stone\n\r"},
    {3670,   "bottled mana\n\r"},
    {1104, "The master of fireworks, take his silver"},
    {5020, "You're not a real mage till you get platinum from purple\n\r"},
    {1599,  "Grand Major\n\r"},
    {20002, "She's hiding in her room, but she has my hat!\n\r"}
  }, { /* cleric */
    {0, ""},
    {1, "It can be found in the donation room, or on your head\n\r"},
    {1110, "White and young, with no corners or sides, a golden treasure can be found inside\n\r"},
    {3070, "The armorer might have a pair, but they're not his best on hand\n\r"},
    {3057, "judicandus dies\n\r"},
    {6001, "I want clothes i can play chess on\n\r"},
    {16033,"A goblin's favorite food, the eastern path\n\r"},
    {107,  "Every righteous cleric should have one, but few do\n\r"},
    {4000, "I have a weakness for cheap rings\n\r"},
    {3025, "cleaver for hire, just outside of midgaard\n\r"},
/* 10 */
    {3649, "My wife needs something New, to help keep her girlish figure\n\r"},
    {7202, "mindflayers have small ones, especially in the sewers\n\r"},
    {19203, "the weapon of a traitor, lost in a fog\n\r"},
    {15814, "striped black and white, take what this gelfling-friend offers\n\r"},
    {119,   "Play with a gypsy child when he asks you to\n\r"},
    {5012,  "You might use these to kill a vampire, they are in the desert\n\r"},
    {6809,  "Really cool sunglasses, on a really cool guy, in a really cool place\n\r"},
    {17021, "The proof is in the volcano\n\r"},
    {3648,  "Giant women have great fashion sense\n\r"},
    {27001, "Undead have the strangest law enforcement tools, near a split in the river\n\r"},
/* 20*/
    {105, "A venomed bite will end your life - you need the jaws that do it."},    /* Rattler fangs, MidT */
    {3668, "Buy some wine from a fortuneteller's brother."},                       /* Gypsy Bartender */ 
    {1703, "On a cat, but not in the petting zoo."},                               /* Bengal tiger , MZ*/
    {13758, "Held by a doggie with who will bite you and bite you and bite you."}, /* Cerebus, Hades */
    {5240, "In the old city you'll find the accursed vibrant stone you require."},  /* Lamia, OT */
    {5013, "Where can you go for directions in the desert? Try the wet spot."},     /* Oasis Map, GED */
    {17011, "{An unholy symbol on an unholy creature under a fuming mountain."},    /* Amelia, WPM */
    {1708, "Some liar's got it in a cul-de-sac. The high priests of Odin know the way."}, /*Liar in room 10911, TL */
    {9203, "What would you use to swat a very large mosquito? Get it from the giants."}, /* Giant, HGS */
    {21109, "A bow made of dark wood, carried by a troglodyte."},                    /* Trog, CT */
/* 30 */
    {15817, "In an secret cold place, a dark flower is carried by a midnight one."}, /* Bechemel, SK */
    {9430, "Argent Ursa, Crevasse of the Arctic Enlargements"},
    {6112, "If you would be king, you'd need one. With a wooded worm."},             /* Dragon, DH-D */
    {1758, "Carried by a hag in the dark lake under the sewers."},                   /* Sea Hag, SM */
    {27411, "This Roo's better than you and she has the stick to prove it."},        /* Queen Roo, LDU */

    {5317, "The dead don't lie still when properly prepared."}, /* Mummy, PY */
    {5033, "You can get it off a drider, but he won't give it to you."},  /* Drider, DR */
    {16615, "South of a bay, past a portal, into a tower, be prepared"},  /* Hoeur, HR */
    {121,   "To the far southeast, in the lair of a pair of arachnids."},          /* Cave Spiders, TR */
    {13901, "On the shore, down the river from the troll-bridge"}, /* RHY */
/* 40 */
    {5104,  "Four heads are better than one\n\r"},
    {15806, "You don't stand a ghost of a chance against a glow of white and a cloak of fire"}, 
    {16022, "a Powerful, blunt, and fragile weapon\n\r"},
    {122,   "The sole possession of a devil down-under\n\r"},
    {7220,  "The highest thing at the top of a chain"},
    {13785, "From the fairest\n\r"},
    {1597,  "Mana in a green ring\n\r"},
    {1563,  "Famous, blue and very very rare"},
    {5001,  "Search for a banded male\n\r"},
    {20003, "Ensnared for power, she holds the helmet of the wise"}

  }, { /* warrior */ 
    {0, ""},
    {11, "Something you might find in the donation room, or on your body\n\r"},
    {16034, "Goblins have been known to play with these, especially in dark caves\n\r"},
    {6000,  "A decent weapon, just the right size for a tree\n\r"},
    {24760, "Dead men's feet look like this\n\r"},
    {1413,  "You were SUPPOSED to bell the CAT!\n\r"},
    {18256, "In the city of Mordilnia, a shield of roygiv\n\r"},
    {8121,  "A bag that opens with a ripping sound\n\r"},
    {108,   "Floating for safety on the dark\n\r"},
    {123,   "A mule stole my hat, now he fights in front of an audience\n\r"},
/* 10 */
    {3621,"Thank goodness when I broke my arm, I still had my New shield\n\r"},
    {117,  "If you get this, someone will be quite howling angry\n\r"},
    {7405, "Sewer Secret Light Sources\n\r"},
    {6205, "my eyes just arent as fast to focus as they used to be\n\r"},
    {4051, "These warriors seem scarred, but its all in their head"}, /* Scarred Warrior, MM */
    {5219,  "Fresh deer.. yum!\n\r"},
    {16015,"An ugly bird in the mountains once told me: 'A despotic ruler rules with one of these'\n\r"},
    {1718, "Hey, that's not a painting at all! But boy is she ugly! In the new city."}, /* Mimic, NT */
    {5032, "Bushwhacked, Bushwhacked, West, West, Green. Start at the obvious\n\r"},
    {3685, "Mightier than a sword, wielded by a four man\n\r"},
/* 20 */
    {5100, "Learn humility: I want a common sword\n\r"},
    {16902, "They'd all be normal in a moonless world. You need to steal a silver stole"}, /* Werefox, LY */
    {17022, "A lion with a woman's torso holds the book you need."}, /* Gynosphinx, WPM */
    {5206,  "To hold the girth of a corpulent man, it must be ferrous. In the old city hall."}, /* Cabinet, OT */
    {1737, "In the hands of an elf with a green thumb."}, /* Gardener, NT */
    {5306, "my mommy gave me a knife, but i lost it\n\r"},
    {21006, "Childlike, maybe, but they're not children. You need the locked up cloth."}, /* Case, OR */
    {9204, "The largest in the hands of the largest of the large"}, /* Giant Chief, HGS */
    {1721, "Get the toolbook of the trade from the royal cook in the new city."}, /* Chef, NT */
    {16901, "Only an elephant's might be as big as this bores' mouthpiece."}, /* Boarish, LY */
/* 30 */
    {6511, "A bearded woman might be so engaged, but a guard's got this one."},
    {5101, "Dark elves can be noble too, but they won't let you take their arms."}, /* Drow Noble, DR */
    {1761, "In a suspended polygon, in a chest which is not."}, /* Mimic, PY */
    {15812, "You think that water can't be sharp? Look under birdland."}, /* Ice Pick, SK */
    {16046, "A miner's tool in the dwarven mines"}, /* Shovel, ?? */
    {21114, "These skeletal beasts will reel you in, you want the crowbar."}, /* Cave Fisher, MVE */
    {13762, "Once in Hades, the key to getting out lies with a long dead warrior."}, /* Skeletal Warrior, Hades */
    {20005, "This usurper will think you very tasty, defeat him for the thing you need."}, /* Yevaud, AR */
    {5019, "A nasty potion in the hands of an even nastier desert worm."}, /* Worm, GED */
    {10002, "These can be found in the zoo, on 'Al'"}, /* On Corpse, DCE */
/* 40 */
    {5221,"Weapons are the keys to the remaining quests. First, bring me a Stone golem's sword"},
    {9442,"The weapon of the largest giant in the rift"},
    {15808,"Weapon of champions"},
    {13775,"By the light of the moon\n\r"},
    {21004,"By name, you can assume its the largest weapon in the game\n\r"},
    {3092, "He's always pissed, and so are his guards. take his weapon and make it yours\n\r"},
    {5002,"The weapon of the oldest wyrm\n\r"},
    {5107,"One Two Three Four Five Six\n\r"},
    {1430,"It rises from the ashes, and guards a tower\n\r"},
    {5019,"You're not a REAL fighter til you've had one of these, enchanted\n\r"}

  }, { /* thief   */
    {0, "",},
    {4,    "You might find one of these in the donation room, or in your hand\n\r"},
    {3071, "They're the best on hand for 5 coins\n\r"},
    {30,   "At the wrong end of a nasty spell, or a heavy hitter\n\r"},
    {3902, "Michelob or Guiness Stout. which is better?\n\r"},
    {24767, "I've heard that skeletons love bleach\n\r"},
    {6006,  "Nearly useless in a hearth\n\r"},
    {4104,  "Its what makes kobolds green\n\r"},
    {42,    "Do she-devils steal, as they flap their bat wings?\n\r"},
    {19202, "Animal light, lost in a fog\n\r"},
    {3647,  "These New boots were made for walking\n\r"},
    {4101,  "Hands only a warrior could love\n\r"},
    {116,   "Near a road to somewhere city\n\r"},
    {111,   "Only a fool would look at the end of the river\n\r"},
    {15812, "I'd love a really cool backstabbing weapon..  Make sure it doesn't melt\n\r"},
    {17023, "Being charming can be offensive, especially in a plumed white cap\n\r"},
    {9205,  "You could hide a giant in this stuff\n\r"},
    {10002, "feeling tired and fuzzy?  Exhibit some stealth, or you just might get eaten\n\r"},
    {3690,  "I am an old man, but I will crush you at chess\n\r"},
    {5000,  "Find a dark dwarf. Pick something silver\n\r"},
/* 20 */
    {15802, "It's easy work to work a rejected bird for the means to his former home."}, /* Skexie Reject, SK */
    {1750, "In the twisted forest of the Graecians a man in a black cloak has it."}, /* Put on 13731, GRF */
    {5012, "Vampire's bane in a wicker basket near a desert pool.a"}, /* Basket, GED */
    {20008, "The toothless dragon eats the means to your advancement."}, /* Young Wormkin, AR */
    {6810, "You are everywhere you look in this frozen northern maze of ice."}, /* Room 6854, ART */
    {255, "Get the happy stick from a desert worm."}, /* Young Worm, GED */
    {7190, "In a secret sewer place a squeaking rodent wears a trinket."}, /* Rat, SM */
    {7205, "The master flayer under the city has it on him, but not in use. Steal it!"}, /* Master mind, SM */
    {7230, "You could be stoned for skinning this subterranean reptilian monster."}, /* Basilisk, SM */
    {3690, "An old man at the park might have one, but these old men are in the new city."}, /* Old man, NT */
/* 30 */
    {1729, "In the forest north of the new city a traveller lost his way. It's on him."}, /* Lost Adventurer, MT */
    {1708, "It's growing on a cliff face, on the way to the lost kingdom."}, /* In room 21170, MVE */
    {1759, "The moon's phase can change a man. Find the badger in a tavern."}, /* Werebadger, LY */
    {1718, "You'll find it in the only ice cave a stone's throw from a desert."}, /* In room 10010, DCE */
    {5243, "I hope it is clear which stone you will need"}, /* Lamia, OT */
    {5302, "In a hanging desert artifact, the softest golem has the key to your success."}, /* Clay Golem, PY */
    {21008, "If your dog were this ugly, you'd lock him in a fireplace too!"}, /* Dog, OR */
    {9206, "It can be electrifying scaling a dragon; a big guy must have done it."}, /* Chieftain, HGS */
    {6524, "The dwarven mazekeeper has the only pair, if you can find him."}, /* Mazekeeper, Dwarf Mines */
    {1533, "Three witches in the mage's tower have the orb you need"}, /* Pot, MT */
/* 40 */
    {9425,"A huge gemstone, guarded by ice toads, beware their poison"},
    {5113, "The weapon of a maiden, shaped like a goddess"},
    {21014,"The dagger of a yellow-belly"},
    {5037, "A thief of great reknown, at least he doesn't use a spoon"},
    {1101, "Elven blade of ancient lore, matches insects blow for blow"}, 
    {27000,"It strikes like a rattlesnake, but not as deadly"},
    {27409,"The weapon of a primitive man, just right for killing his mortal foe"},
    {1594, "White wielded by white, glowing white\n\r"},
    {20001,"He judges your soul, wields a weapon that shares your name"},
    {13703,"Watch for a dragon, he wears that which you seek"}
  }
};

const char *att_kick_kill_ch[] = {
  "Your kick caves $N's chest in, which kills $M.",
  "Your kick destroys $N's arm and caves in one side of $S rib cage.",
  "Your kick smashes through $N's leg and into $S pelvis, killing $M.",
  "Your kick shatters $N's skull.",
  "Your kick at $N's snout shatters $S jaw, killing $M.",
  "You kick $N in the rump with such force that $E keels over dead.",
  "You kick $N in the belly, mangling several ribs and killing $M instantly.",
  "$N's scales cave in as your mighty kick kills $N.",
  "Your kick rips bark asunder and leaves fly everywhere, killing the $N.",
  "Bits of $N are sent flying as you kick him to pieces.",
  "You punt $N across the room, $E lands in a heap of broken flesh.",
  "You kick $N in the groin, $E dies screaming an octave higher.",
  ".",  /* GHOST */
  "Feathers fly about as you blast $N to pieces with your kick.",
  "Your kick splits $N to pieces, rotting flesh flies everywhere.",
  "Your kick topples $N over, killing it.",
  "Your foot shatters cartilage, sending bits of $N everywhere.",
  "You launch a mighty kick at $N's gills, killing it.",
  "Your kick at $N sends $M to the grave.",
  "."
  };
const char *att_kick_kill_victim[] = {
  "$n crushes you beneath $s foot, killing you.",
  "$n destroys your arm and half your ribs.  You die.",
  "$n neatly splits your groin in two, you collapse and die instantly.",
  "$n splits your head in two, killing you instantly.",
  "$n forces your jaw into the lower part of your brain.",
  "$n kicks you from behind, snapping your spine and killing you.",
  "$n kicks your stomach and you into the great land beyond!!",
  "Your scales are no defense against $n's mighty kick.",
  "$n rips you apart with a massive kick, you die in a flutter of leaves.",
  "You are torn to little pieces as $n splits you with $s kick.",
  "$n's kick sends you flying, you die before you land.",
  "Puny little $n manages to land a killing blow to your groin, OUCH!",
  ".", /* GHOST */
  "Your feathers fly about as $n pulverizes you with a massive kick.",
  "$n's kick rips your rotten body into shreds, and your various pieces die.",
  "$n kicks you so hard, you fall over and die.",
  "$n shatters your exoskeleton, you die.",
  "$n kicks you in the gills!  You cannot breath..... you die!.",
  "$n sends you to the grave with a mighty kick.",
  "."
  };
const char *att_kick_kill_room[] = {
  "$n strikes $N in chest, shattering the ribs beneath it.",
  "$n kicks $N in the side, destroying $S arm and ribs.",
  "$n nails $N in the groin, the pain killing $M.",
  "$n shatters $N's head, reducing $M to a twitching heap!",
  "$n blasts $N in the snout, destroying bones and causing death.",
  "$n kills $N with a massive kick to the rear.",
  "$n sends $N to the grave with a massive blow to the stomach!",
  "$n ignores $N's scales and kills $M with a mighty kick.",
  "$n sends bark and leaves flying as $e splits $N in two.",
  "$n blasts $N to pieces with a ferocious kick.",
  "$n sends $N flying, $E lands with a LOUD THUD, making no other noise.",
  "$N falls to the ground and dies clutching $S crotch due to $n's kick.",
  ".", /* GHOST */
  "$N disappears into a cloud of feathers as $n kicks $M to death.",
  "$n blasts $N's rotten body into pieces with a powerful kick.",
  "$n kicks $N so hard, it falls over and dies.",
  "$n blasts $N's exoskeleton to little fragments.",
  "$n kicks $N in the gills, killing it.",
  "$n sends $N to the grave with a mighty kick.",
  "."
  };
const char *att_kick_miss_ch[] = {
  "$N steps back, and your kick misses $M.",
  "$N deftly blocks your kick with $S forearm.",
  "$N dodges, and you miss your kick at $S legs.",
  "$N ducks, and your foot flies a mile high.",
  "$N steps back and grins evilly as your foot flys by $S face.",
  "$N laughs at your feeble attempt to kick $M from behind.",
  "Your kick at $N's belly makes it laugh.",
  "$N chuckles as your kick bounces off $S tough scales.",
  "You kick $N in the side, denting your foot.",
  "Your sloppy kick is easily avoided by $N.",
  "You misjudge $N's height and kick well above $S head.",
  "You stub your toe against $N's shin as you try to kick $M.",
  "Your kick passes through $N!!",  /* Ghost */
  "$N nimbly flitters away from your kick.",
  "$N sidesteps your kick and sneers at you.",
  "Your kick bounces off $N's leathery hide.",
  "Your kick bounces off $N's tough exoskeleton.",
  "$N deflects your kick with a fin.",
  "$N avoids your paltry attempt at a kick.",
  "."
  };
const char *att_kick_miss_victim[] = {
  "$n misses you with $s clumsy kick at your chest.",
  "You block $n's feeble kick with your arm.",
  "You dodge $n's feeble leg sweep.",
  "You duck under $n's lame kick.",
  "You step back and grin as $n misses your face with a kick.",
  "$n attempts a feeble kick from behind, which you neatly avoid.",
  "You laugh at $n's feeble attempt to kick you in the stomach.",
  "$n kicks you, but your scales are much too tough for that wimp.",
  "You laugh as $n dents $s foot on your bark.",
  "You easily avoid a sloppy kick from $n.",
  "$n's kick parts your hair but does little else.",
  "$n's light kick to your shin bearly gets your attention.",
  "$n passes through you with $s puny kick.",
  "You nimbly flitter away from $n's kick.",
  "You sneer as you sidestep $n's kick.",
  "$n's kick bounces off your tough hide.",
  "$n tries to kick you, but your too tough.",
  "$n tried to kick you, but you deflected it with a fin.",
  "You avoid $n's feeble attempt to kick you.",
  "."
  };

const char *att_kick_miss_room[] = {
  "$n misses $N with a clumsy kick.",
  "$N blocks $n's kick with $S arm.",
  "$N easily dodges $n's feeble leg sweep.",
  "$N easily ducks under $n's lame kick.",
  "$N steps back and grins evilly at $n's feeble kick to $S face misses.",
  "$n launches a kick at $N's behind, but fails miserably.",
  "$N laughs at $n's attempt to kick $M in the stomach.",
  "$n tries to kick $N, but $s foot bounces off of $N's scales.",
  "$n hurts $s foot trying to kick $N.",
  "$N avoids a lame kick launched by $n.",
  "$n misses a kick at $N due to $S small size.",
  "$n misses a kick at $N's groin, stubbing $s toe in the process.",
  "$n's foot goes right through $N!!!!",
  "$N flitters away from $n's kick.",
  "$N sneers at $n while sidestepping $s kick.",
  "$N's tough hide deflects $n's kick.",
  "$n hurts $s foot on $N's tough exterior.",
  "$n tries to kick $N, but is thwarted by a fin.",
  "$N avoids $n's feeble kick.",
  "."
  };

const char *att_kick_hit_ch[] = {
  "Your kick crashes into $N's chest.",
  "Your kick hits $N in the side.",
  "You hit $N in the thigh with a hefty sweep.",
  "You hit $N in the face, sending $M reeling.",
  "You plant your foot firmly in $N's snout, smashing it to one side.",
  "You nail $N from behind, sending him reeling.",
  "You kick $N in the stomach, winding $M.",
  "You find a soft spot in $N's scales and launch a solid kick there.",
  "Your kick hits $N, sending small branches and leaves everywhere.",
  "Your kick contacts with $N, dislodging little pieces of $M.",
  "Your kick hits $N right in the stomach, $N is rendered breathless.",
  "You stomp on $N's foot. After all, thats about all you can do to a giant.",
  ".", /* GHOST */
  "Your kick  sends $N reeling through the air.",
  "You kick $N and feel rotten bones crunch from the blow.",
  "You smash $N with a hefty roundhouse kick.",
  "You kick $N, cracking it's exoskeleton.",
  "Your mighty kick rearranges $N's scales.",
  "You leap off the ground and crash into $N with a powerful kick.",
  "."
  };

const char *att_kick_hit_victim[] = {
  "$n's kick crashes into your chest.",
  "$n's kick hits you in your side.",
  "$n's sweep catches you in the side and you almost stumble.",
  "$n hits you in the face, gee, what pretty colors...",
  "$n kicks you in the snout, smashing it up against your face.",
  "$n blasts you in the rear, ouch!",
  "Your breath rushes from you as $n kicks you in the stomach.",
  "$n finds a soft spot on your scales and kicks you, ouch!",
  "$n kicks you hard, sending leaves flying everywhere!",
  "$n kicks you in the side, dislodging small parts of you.",
  "You suddenly see $n's foot in your chest.",
  "$n lands a kick hard on your foot making you jump around in pain.",
  ".", /* GHOST */
  "$n kicks you, and you go reeling through the air.",
  "$n kicks you and your bones crumble.",
  "$n hits you in the flank with a hefty roundhouse kick.",
  "$n ruins some of your scales with a well placed kick.",
  "$n leaps off of the grand and crashes into you with $s kick.",
  "."
  };

const char *att_kick_hit_room[] = {
  "$n hits $N with a mighty kick to $S chest.",
  "$n whacks $N in the side with a sound kick.",
  "$n almost sweeps $N off of $S feet with a well placed leg sweep.",
  "$N's eyes roll as $n plants a foot in $S face.",
  "$N's snout is smashed as $n relocates it with $s foot.",
  "$n hits $N with an impressive kick from behind.",
  "$N gasps as $n kick $N in the stomach.",
  "$n finds a soft spot in $N's scales and launches a solid kick there.",
  "$n kicks $N.  Leaves fly everywhere!!",
  "$n hits $N with a mighty kick, $N loses parts of $Mself.",
  "$n kicks $N in the stomach, $N is rendered breathless.",
  "$n kicks $N in the foot, $N hops around in pain.",
  ".", /* GHOST */
  "$n sends $N reeling through the air with a mighty kick.",
  "$n kicks $N causing parts of $N to cave in!",
  "$n kicks $N in the side with a hefty roundhouse kick.",
  "$n kicks $N, cracking exo-skelelton.",
  "$n kicks $N hard, sending scales flying!",
  "$n leaps up and nails $N with a mighty kick.",
  "."
  };

/* 
  #define RACE_LIST_SIZE 40
  */
const struct RaceChoices RaceList[RACE_LIST_SIZE] = {
    {52,  "Human"},		/* 51 max type */
    {2,  "*Elf"},
    {3,  "*Dwarf"},
    {4,  "*Halfling"},
    {5,  "*Gnome"},
    {32, "Veggie"},
    {33, "*Mindflayer"},
    {36, "*Drow"},
    {47, "*Ratperson"},
    {50, "Half-Elf"},
    {51, "Half-Orc"},
    {53, "*Vampire"},
    {54, "*Ogre"},
    {55, "*Fairy"},
    {0,  "\n"}
};

const char *bird_noises[] = {
  "*SQUAWK*",
  "*chirp*",
  "*peep*",
  "*Caw*",
  "*tweet*",
  "\n"
};

const char *rat_noises[] = {
  "<SQUEAK>",
  "<Squeak>",
  "<squeak>",
  "<Chitter>",
  "\n"
};

const char *half_orc_words[] = {
  "Flippen a",
  "Your mother",
  "You suck",
  "You stink",
  "Butthead",                 /* 5 */
  "Wimp",
  "You turkey",
  "You geek",
  "You idiot",
  "You weenie",               /* 10 */
  "Moron",
  "You all suck",
  "Loser",
  "Hoser",
  "Doofus"                    /* 15 */
  };

const char *half_orc_noises[] = {
  "#Snarl#",
  "#Growl#",
  "*snicker*",
  "#grrrr#",
  "<grunt>",                  /* 5 */
  "<sneer>"
  };

const char *article_list[] = {
  "the",
  "it",
  "a",
  "an",
  "\n"
  };

const char *ogre_speak[] = {
  "slim",			/* 1 */
  "breakfast",
  "lunch",
  "dinner",
  "tasty",			/* 5 */
  "mighty tasty",
  "troll's leavings",
  "filling",
  "slab o'",
  "appetizer",			/* 10 */
  "dessert",
  "yummy",
  "fuggin'",
  "flippin' eck",
  "on a spit",			/* 15 */
  "toast and jam",
  "dark meat",
  "bag o' bones",
  "fat",
  "skimpy",			/* 20 */
  "lightweight",
  "flimsy",
  "flapjack",
  "griddlecake",
  "mutton chop",		/* 25 */
  "flank steak",
  "rump roast",
  "plump",
  "gravy",
  "bread n' butter",		/* 30 */
  "pulled pork",
  "pork rind",
  "cold plate",
  "hot potato",
  "cold turkey",		/* 35 */
  "cool beans",
  "hot crossed buns"		/* 37 */
  };

const struct BrewMeister BrewList[18] = {
  {"youth", 2108, 2109, 111, 20006, 106, 22698 },
  {"bless", 22694, -1, -1, -1, -1, 70},
  {"cure light", 2108, -1, -1, -1, -1, 71},
  {"detect evil", 106, -1, -1, -1, -1, 72},
  {"detect magic", 20006, -1, -1, -1, -1, 73},
  {"detect invisibility", 106, 2108, -1, -1, -1, 74},
  {"cure serious", 22692, 2108, 22694, -1, -1, 75},
  {"second wind", 107, 20006, 111, -1, -1, 76},
  {"cure critical", 107, 2109, 22693, -1, -1, 77},
  {"true sight", 114, 105, 106, 2108, 120, 78},
  {"fly", 109, 20006, -1, -1, -1, 79},
  {"heal", 114, 121, 111, 2108, 22694, 80},
  {"sanctuary", 5235, 114, 22691, 107, 120, 81},
  {"shield", 22691, -1, -1, -1, -1, 82},
  {"strength", 22690, 113, -1, -1, -1, 83},
  {"water breath", 108, 112, -1, -1, -1, 84},
  {"minor track", 111, 22690, 105, -1, -1, 85},
  {"major track", 111, 22690, 105, 110, -1, 86}
};
