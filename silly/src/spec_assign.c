/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>

#include "protos.h"

#if HASH
extern struct hash_header room_db;
#else
extern struct room_data *room_db;
#endif
extern struct index_data *mob_index;
extern struct index_data *obj_index;
void boot_the_shops();
void assign_the_shopkeepers();

struct special_proc_entry {
  int	vnum;
  int	(*proc)();
};

/* ********************************************************************
*  Assignments                                                        *
******************************************************************** */

/* put here so we don't have to recompile EVERYTHING */
int death_knight(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);


/* assign special procedures to mobiles */
void assign_mobiles()
{

  static struct special_proc_entry specials[] = {

    { 1, puff },
    { 2, Ringwraith },
    { 3, tormentor },
    { 4, Inquisitor},
    { 6, AcidBlob },
    { 25, magic_user },

    { 30, MageGuildMaster }, 
    { 31, ClericGuildMaster }, 
    { 32, ThiefGuildMaster }, 
    { 33, WarriorGuildMaster },
    { 34, MageGuildMaster }, 
    { 35, ClericGuildMaster }, 
    { 36, ThiefGuildMaster }, 
    { 37, WarriorGuildMaster },
    { 38, fighter},  /* armor golem */
    { 39, creeping_death},  

    {199, AGGRESSIVE},
    {200, AGGRESSIVE},
/*
**  D&D standard
*/

    { 210, snake },	   /* spider */
    { 211, fighter},       /* gnoll  */
    { 220, fighter},       /* fighter */
    { 221, fighter},       /* bugbear */
    { 223, ghoul },	   /* ghoul */
    { 226, fighter },	   /* ogre */
    { 236, ghoul },	    /* ghast */
    { 227, snake },	/* spider */
    { 230, BreathWeapon }, /* baby black */
    { 232, blink },       /* blink dog */
    { 233, BreathWeapon }, /* baby blue */
    { 234, cleric }, /* cleric */
    { 239, shadow },      /* shadow    */
    { 240, snake },       /* toad      */
    { 243, BreathWeapon }, /* teenage white */
    { 247, fighter}, /* minotaur */
    { 251, CarrionCrawler },
    { 261, fighter },
    { 262, regenerator },
    { 264, fighter },
    { 265, magic_user},
    { 266, magic_user },
    { 267, Devil},
    { 269, Demon},
    { 271, regenerator },
    { 248, snake },       /* snake       */
    { 249, snake },       /* snake       */
    { 250, snake },       /* snake       */
    { 257, magic_user },  /* magic_user  */

    {600, DruidChallenger},
    {601, DruidChallenger},
    {602, DruidChallenger},
    {603, DruidChallenger},
    {604, DruidChallenger},
    {605, DruidChallenger},
    {606, DruidChallenger},
    {607, DruidChallenger},
    {608, DruidChallenger},
    {609, DruidChallenger},
    {610, DruidChallenger},
    {611, DruidChallenger},
    {612, DruidChallenger},
    {613, DruidChallenger},
    {614, DruidChallenger},
    {615, DruidChallenger},
    {616, DruidChallenger},
    {617, DruidChallenger},
    {618, DruidChallenger},
    {619, DruidChallenger},
    {620, DruidChallenger},
    {621, DruidChallenger},
    {622, DruidChallenger},
    {623, DruidChallenger},
    {624, DruidChallenger},
    {625, DruidChallenger},
    {626, DruidChallenger},
    {627, DruidChallenger},
    {628, DruidChallenger},
    {629, DruidChallenger},
    {630, DruidChallenger},
    {631, DruidChallenger},
    {632, DruidChallenger},
    {633, DruidChallenger},
    {634, DruidChallenger},
    {635, DruidChallenger},
    {636, DruidChallenger},
    {637, DruidChallenger},
    {638, DruidChallenger},
    {639, DruidChallenger},
    {640, DruidChallenger},
    {641, DruidGuildMaster},
    {642, DruidGuildMaster},

    {651, MonkChallenger},
    {652, MonkChallenger},
    {653, MonkChallenger},
    {654, MonkChallenger},
    {655, MonkChallenger},
    {656, MonkChallenger},
    {657, MonkChallenger},
    {658, MonkChallenger},
    {659, MonkChallenger},
    {660, MonkChallenger},
    {661, MonkChallenger},
    {662, MonkChallenger},
    {663, MonkChallenger},
    {664, MonkChallenger},
    {665, MonkChallenger},
    {666, MonkChallenger},
    {667, MonkChallenger},
    {668, MonkChallenger},
    {669, MonkChallenger},
    {670, MonkChallenger},
    {671, MonkChallenger},
    {672, MonkChallenger},
    {673, MonkChallenger},
    {674, MonkChallenger},
    {675, MonkChallenger},
    {676, MonkChallenger},
    {677, MonkChallenger},
    {678, MonkChallenger},
    {679, MonkChallenger},
    {680, MonkChallenger},
    {681, MonkChallenger},
    {682, MonkChallenger},
    {683, MonkChallenger},
    {684, MonkChallenger},
    {685, MonkChallenger},
    {686, MonkChallenger},
    {687, MonkChallenger},
    {688, MonkChallenger},
    {689, MonkChallenger},
    {690, MonkChallenger},
    {691, monk_master},

/*
  frost giant area 
*/

    { 9401, fighter },
    { 9403, magic_user },
    { 9404, fighter },
    { 9405, thief },
    { 9406, fighter },
    { 9407, fighter },
    { 9408, fighter },
    { 9409, fighter },
    { 9410, fighter },
    { 9411, fighter },
    { 9412, fighter },
    { 9413, fighter },
    { 9414, fighter },
    { 9415, fighter },
    { 9416, fido},
    { 9418, BreathWeapon },
    { 9419, BreathWeapon },
    { 9420, fighter },
    { 9421, fighter },
    { 9424, StormGiant },
    { 9426, MonkChallenger },
    { 9427, cleric },
    { 9428, magic_user },
    { 9429, fighter },
    { 9430, regenerator },
    { 9431, snake },
    { 9432, magic_user },
    { 9433, fighter },
    { 9434, fighter },
    { 9435, snake  },
    { 9436, fido },

/*
**   shire
*/
    { 1000, magic_user },
    { 1035, fighter},
    { 1012, fighter},
    { 1026, fighter}, 
    { 1010, fighter},
    { 1011, fighter},
    { 1012, fighter},
    { 1014, fighter},
    { 1015, fighter},
    { 1016, fighter},
    { 1017, fighter},
    { 1001, fighter},
    { 1023, fighter},
    { 1031, receptionist },
    { 1032, fighter},

    { 1701, monk},
    { 1702, monk},
    { 1703, monk},
    { 1704, monk},
    { 1705, monk},
    { 1706, monk},
    { 1707, monk},
    { 1708, monk},
    { 1709, monk},
    { 1710, monk},
    { 1711, monk},
    { 1712, monk},
    { 1713, monk},
    { 1714, monk},
    { 1715, monk},
    { 1716, monk},
    { 1717, monk},
    { 1718, monk},

    /* Thand's thief area */
    { 1105, thief},
    { 1108, thief},
    { 1115, fido},
    /* end of Thand's thief area */

    { 6001, real_rabbit},
    { 6005, real_fox},
/*
  prydain
*/
    { 6601, PrydainGuard},
    { 6602, PrydainGuard},
    { 6604, fighter},
    { 6605, PrydainGuard},
    { 6606, PrydainGuard},
    { 6608, fighter},
    { 6613, fighter},
    { 6619, PrydainGuard},
    { 6620, PrydainGuard},
    { 6621, fighter},
    { 6622, fighter},
    { 6614, PrydainGuard},
    { 6609, BreathWeapon},
    { 6642, BreathWeapon},
    { 6640, jugglernaut },
    { 6612, magic_user },
    { 6615, magic_user },
    { 6616, magic_user },
    { 6624, magic_user },
    { 6637, magic_user },
    { 6634, fighter },
    { 6635, BreathWeapon},
    { 6625, Demon},
    { 6638, StatTeller},

/*
  deadhame
*/
    { 24782, fighter },

/*
**  G1
*/
    { 9200, fighter },
    { 9201, fighter },
    { 9202, fighter },
    { 9203, fighter },
    { 9204, fighter },
    { 9206, fighter },
    { 9207, fighter },
    { 9211, fighter },
    { 9212, fighter },
    { 9216, fighter },
    { 9213, CarrionCrawler},
    { 9208, cleric },
    { 9217, BreathWeapon},
    { 9218, fighter},

/*
**  chessboard
*/
    { 1400, chess_game },  /* black pieces */
    { 1401, chess_game },
    { 1402, chess_game },
    { 1403, chess_game },
    { 1404, chess_game },
    { 1405, chess_game },
    { 1406, chess_game },
    { 1407, chess_game },
    { 1408, chess_game },
    { 1409, chess_game },
    { 1410, chess_game },
    { 1411, chess_game },
    { 1412, chess_game },
    { 1413, chess_game },
    { 1414, chess_game },
    { 1415, chess_game },
    
    { 1448, chess_game },  /* white pieces */
    { 1449, chess_game },
    { 1450, chess_game },
    { 1451, chess_game },
    { 1452, chess_game },
    { 1453, chess_game },
    { 1454, chess_game },
    { 1455, chess_game },
    { 1456, chess_game },
    { 1457, chess_game },
    { 1458, chess_game },
    { 1459, chess_game },
    { 1460, chess_game },
    { 1461, chess_game },
    { 1462, chess_game },
    { 1463, chess_game },
    
#if 0
    { 1401, fighter}, 
    { 1404, fighter}, 
    { 1406, fighter}, 
    { 1457, fighter}, 
    { 1460, fighter}, 
    { 1462, fighter}, 
#endif

    { 1499, sisyphus }, 
    { 1471, paramedics }, 
    { 1470, jabberwocky },
    { 1472, flame }, 
    { 1437, banana }, 
    { 1428, jugglernaut },
    { 1495, delivery_elf },  
    { 1493, delivery_beast },

/*
**  Bandits Temple
*/
    { 2113, ghoul },
    { 2115, ghost },
    { 2116, ghost },
    { 2111, cleric },
    { 2112, cleric },
    { 2117, druid_protector },


    /* Astral plane */
    { 2715, astral_portal },
    { 2716, astral_portal },
    { 2717, astral_portal },
    { 2718, astral_portal },
    { 2719, astral_portal },
    { 2720, astral_portal },
    { 2721, astral_portal },
    { 2722, astral_portal },
    { 2723, astral_portal },
    { 2724, astral_portal },
    { 2725, astral_portal },
    { 2726, astral_portal },
    { 2727, astral_portal },
    { 2728, astral_portal },
    { 2729, astral_portal },
    { 2730, astral_portal },
    { 2731, astral_portal },
    { 2732, astral_portal },
    { 2733, astral_portal },
    { 2734, astral_portal },
    { 2735, astral_portal },
    { 2736, astral_portal },


/*
**  Valley of the Mage
*/
    { 21106, snake },
    { 21107, RustMonster},
    { 21108, wraith},
    { 21111, web_slinger},
    { 21112, trapper},
    { 21114, troguard},
    { 21121, trogcook},
    { 21122, shaman},
    { 21123, troguard},
    { 21124, golgar},
    { 21118, troguard},
    { 21119, troguard},
/*
    { 21130, Valik},
*/
    { 21135, regenerator},
    { 21138, ghostsoldier},
    { 21139, ghostsoldier},
/*
    { 21140, keystone},
    { 21141, lattimore},
    { 21142, guardian},
*/
    { 21144, troguard},
    { 21145, troguard},
    { 21146, coldcaster},
    { 21147, RustMonster},

/*
**  New Thalos
*/
    { 3600, MageGuildMaster },
    { 3601, ClericGuildMaster },
    { 3602, WarriorGuildMaster },
    { 3603, ThiefGuildMaster },
    { 3604, receptionist},
    { 3619, fighter},
    { 3620, fighter},
    { 3632, fighter},
    { 3634, fighter},
    { 3636, fighter},
    { 3638, fighter},
    { 3639, fighter}, /* caramon */
    { 3641, cleric},  /* curley g. */
    { 3640, magic_user},  /* raist */
    { 3656, NewThalosGuildGuard},
    { 3657, NewThalosGuildGuard},
    { 3658, NewThalosGuildGuard},
    { 3659, NewThalosGuildGuard},
    { 3661, SultanGuard},   /* wandering */
    { 3662, SultanGuard},   /* not */
    { 3682, SultanGuard},   /* royal */
    { 3670, BreathWeapon},  /* Cryohydra */
    { 3674, BreathWeapon},  /* Behir */
    { 3675, BreathWeapon},  /* Chimera */
    { 3676, BreathWeapon},  /* Couatl */
    { 3681, cleric },       /* High priest */
    { 3689, NewThalosMayor }, /* Guess */
    { 3644, fido},
    { 3635, thief}, 
/*
**  Skexie
*/
    { 15813, magic_user},
    { 15815, magic_user},
    { 15820, magic_user },	
    { 15821, vampire },	
    { 15844, cleric },	
    { 15847, fighter },	
    { 15831, fighter },	
    { 15832, fighter },	
    { 15822, fighter },	
    { 15819, fighter },	
    { 15805, fighter },	
/*
**  Challenge
*/
    { 15858, BreathWeapon },
    { 15861, magic_user },
    { 15862, magic_user },
    { 15863, fighter },
    { 15864, sisyphus },
    { 15877, magic_user },
    { 15868, snake },
    { 15866, magic_user },
    { 15810, magic_user },
    { 15880, fighter },
    { 15879, BreathWeapon },
    { 15873, magic_user },
    { 15871, magic_user },
    { 15852, fighter },
    { 15875, cleric },
    { 15869, magic_user },
    
/*
**  abyss
*/
    { 25000, magic_user },      /* Demi-lich  */
    { 25001, Keftab }, 
    { 25009, BreathWeapon },    /* hydra */
    { 25002, vampire },	        /* Crimson */
    { 25003, StormGiant },      /* MistDaemon */
    { 25006, StormGiant },      /* Storm giant */
    { 25014, StormGiant },      /* DeathKnight */    
    { 25009, BreathWeapon },    /* hydra */
    { 25017, AbyssGateKeeper }, /* Abyss Gate Keeper */
    { 25013, fighter},          /* kalas */
    { 25008, magic_user},       /* efreeti */
    { 25034, fighter },         /* marilith */
    { 25035, magic_user},       /* balor  */
/*
**  Paladin's guild
*/
    { 25100, PaladinGuildGuard},
    { 25101, PaladinGuildGuard},
/*
** Shark's Junk
*/
    { 196, GreyParamedic},
    { 197, AmberParamedic},
    { 198, GameGuard}, 
/*
**  Abyss Fire Giants
*/
    { 25500, fighter },
    { 25501, fighter },
    { 25502, fighter },
    { 25505, fighter },
    { 25504, BreathWeapon},
    { 25503, cleric  },

/*
**  Temple Labrynth
*/

    { 10900, temple_labrynth_liar },
    { 10901, temple_labrynth_liar },
    { 10902, temple_labrynth_sentry},

/*
**  Gypsy Village
*/

    { 16106, fido},
    { 16107, CaravanGuildGuard},
    { 16108, CaravanGuildGuard},
    { 16109, CaravanGuildGuard},
    { 16110, CaravanGuildGuard},
    { 16111, WarriorGuildMaster},
    { 16112, MageGuildMaster},
    { 16113, ThiefGuildMaster},
    { 16114, ClericGuildMaster},
    { 16122, receptionist},
    { 16105, StatTeller},

/*
**  Draagdim
*/

    { 2500, NudgeNudge },  /* jailer */
/*
**  mordilnia
*/
    {18200, magic_user},
    {18205, receptionist},
    {18206, MageGuildMaster},
    {18207, ClericGuildMaster},    
    {18208, ThiefGuildMaster},
    {18209, WarriorGuildMaster},    
    {18210, MordGuildGuard},  /*18266 3*/  
    {18211, MordGuildGuard},  /*18276 1*/
    {18212, MordGuildGuard},  /*18272 0*/
    {18213, MordGuildGuard},  /*18256 2*/
    {18215, MordGuard },    
    {18216, janitor},
    {18217, fido},    
    {18218, fighter},    
    {18221, fighter},
    {18222, MordGuard},
    {18223, MordGuard},    

/*
**  Graecia:
*/
    {13706, fighter},
    {13709, fighter},
    {13711, fighter},
    {13714, fighter},
    {13721, fighter},
    {13722, fighter},
    {13732, snake},
    {13762, fighter},
    {13764, fighter},
    {13766, fighter},
    {13769, fighter},
    {13771, fighter},
    {13775, fighter},

    {13779, magic_user},
    {13784, magic_user},
    {13785, magic_user},
    {13787, magic_user},
    {13789, magic_user},
    {13791, magic_user},
    {13793, magic_user},
    {13795, magic_user},
    {13797, magic_user},
    { 13843, fighter },


    
/*
**  Eastern Path
*/
    
    {16001, fighter},
    {16006, fighter},
    {16007, fighter},
    {16008, fighter},
    {16009, fighter},
    {16016, fighter},
    {16017, fighter},
    {16020, snake },
    {16021, cleric},
    {16022, fighter},
    {16023, thief},
    {16027, fighter},
    {16033, fighter},
    {16034, fighter},
    {16035, fighter},
    {16036, magic_user},
    {16043, fighter},
    {16050, fighter},
    {16052, fighter},
    {16048, fighter},
    {16056, cleric},
    {16057, magic_user},
    {16037, DwarvenMiners },
    {16014, magic_user},
    {16039, Tyrannosaurus_swallower},

/*
**  undercaves.. level 1
*/
    {16201, magic_user},
    {16204, magic_user},
    {16205, magic_user},
    {16206, magic_user},
    {16210, fighter},
    {16211, magic_user},
    {16214, fighter},
    {16215, fighter},
    {16216, cleric},
    {16217, magic_user},
    {16219, death_knight},
/*
** Sauria
*/
    {21803, Tyrannosaurus_swallower},
    {21810, Tyrannosaurus_swallower},

/*
**  Bay Isle
*/
    {16610, Demon},
    {16620, BreathWeapon},
    {16640, cleric},
    {16650, cleric},

#if 0
/*
**  King's Mountain
*/
    {16700, BreathWeapon},
    {16702, shadow},
    {16703, magic_user},
    {16709, vampire},
    {16710, Devil},
    {16711, Devil},
    {16712, Devil},
    {16713, ghoul},
    {16714, ghoul},
    {16715, wraith},
    {16717, fighter},
    {16720, Devil},
    {16721, Devil},
    {16724, Devil},
    {16725, magic_user},
    {16726, cleric},
    {16727, Devil},
    {16728, Devil},
    {16730, Devil},
    {16731, Devil},
    {16732, Demon},
    {16733, Demon},
    {16734, Demon},
    {16735, Demon},
    {16736, cleric},
    {16738, BreathWeapon},
#endif

/*
**  Sewer Rats
*/
    {7002, attack_rats},
    {2531, DragonHunterLeader},
    {3063, HuntingMercenary},

/*
**  Mages Tower
*/
    {1500, shadow},
    {1504, magic_user},
    {1506, magic_user},
    {1507, magic_user},
    {1508, magic_user},
    {1510, magic_user},
    {1514, magic_user},
    {1515, magic_user},
    {1516, magic_user},
    {1517, magic_user},
    {1518, magic_user},
    {1520, magic_user},
    {1521, magic_user},
    {1522, magic_user},
    {1523, magic_user},
    {1524, magic_user},
    {1525, magic_user},
    {1526, magic_user},
    {1527, magic_user},
    {1528, magic_user},
    {1529, magic_user},
    {1530, magic_user},
    {1531, magic_user},
    {1532, magic_user},
    {1533, magic_user},
    {1534, magic_user},
    {1537, magic_user},
    {1538, magic_user},
    {1540, magic_user},
    {1541, magic_user},
    {1548, magic_user},
    {1549, magic_user},
    {1552, magic_user},
    {1553, magic_user},
    {1554, magic_user},
    {1556, magic_user},
    {1557, magic_user},
    {1559, magic_user},
    {1560, magic_user},
    {1562, magic_user},
    {1564, magic_user},
    {1565, magic_user},
/*
**  Forest of Rhowyn
*/

    {13901, ThrowerMob },

/*
**  Quikland
*/
    {6202, fighter},
    {6204, magic_user},
    {6206, fighter},
    {6207, fighter},
    {6208, fighter},
/*
** Dwarf Village
*/
    {6500, fighter},
    {6501, fighter},
    {6502, wraith},
    {6506, fighter},
    {6507, fighter},
    {6508, fighter},
    {6514, fighter},
    {6516, fighter},
    {6516, snake},

/*
**  Lycanthropia
*/
    {16901, fighter},
    {16902, fighter},
    {16903, fighter},
    {16904, fighter},
    {16905, fighter},
    {16906, fighter},
    {16907, magic_user},
    {16908, fighter},
    {16910, fighter},
    {16911, fighter},

/*
**  Main City
*/

    { 3000, magic_user }, 
    { 3060, MidgaardCityguard }, 
    { 3080, fighter},
    { 3067, MidgaardCityguard }, 
    { 3061, janitor },
    { 3062, fido }, 
    { 3066, fido },
    { 3005, receptionist },
    { 3020, MageGuildMaster }, 
    { 3021, ClericGuildMaster }, 
    { 3022, ThiefGuildMaster }, 
    { 3023, WarriorGuildMaster },

    { 3007, sailor },    /* Sailor */
    { 3024, guild_guard }, 
    { 3025, guild_guard }, 
    { 3026, guild_guard },
    { 3027, guild_guard },
    { 3070, RepairGuy }, 
    { 3071, RepairGuy },
    { 3069, MidgaardCityguard },	/* post guard */
    { 3068, ninja_master },
    { 3073, loremaster },
    { 3074, hunter },

/*
**  Lower city
*/
    { 3143, mayor },
    { 7009, MidgaardCityguard },

/*
**   Hammor's Stuff
*/
    { 3900, eric_johnson }, { 3901, andy_wilcox }, { 3950, zombie_master },
    { 3952, BreathWeapon },

/* 
**  MORIA 
*/
    { 4000, snake }, 
    { 4001, snake }, 
    { 4053, snake },

    { 4103, thief }, 
    { 4100, magic_user }, 
    { 4101, regenerator },
    { 4102, snake },

/*
**  Pyramid
*/

    { 5308, RustMonster },
    { 5303, vampire },

/*
**  Arctica
*/
    { 6800, fighter},
    { 6803, fighter},
    { 6801, BreathWeapon },
    { 6802, BreathWeapon },
    { 6815, magic_user },
    { 6821, snake },
    { 6824, BreathWeapon },
    { 6825, thief },

/* 
** SEWERS 
*/
    { 7009, fighter},
    { 7006, snake },
    { 7008, snake },
    { 7042, magic_user },	/* naga       */
    { 7040, BreathWeapon },     /* Red    */
    { 7041, magic_user },	/* sea hag    */
    { 7045, ettin },		/* berserk teacher */
    { 7200, magic_user },	/* mindflayer */ 
    { 7201, magic_user },	/* senior     */
    { 7202, magic_user },	/* junior     */
    

/* 
** FOREST 
*/

    { 6111, magic_user },	/* tree */
    { 6113, snake },
    { 6114, snake },
    { 6112, BreathWeapon }, /* green */
    { 6910, magic_user },

/*
**  Great Eastern Desert
*/
    { 5000, thief },	/* rag. dervish */
    { 5002, snake },	/* coral snake */
    { 5003, snake },	/* scorpion    */
    { 5004, snake },	/* purple worm  */
    { 5014, cleric },	/* myconoid */
    { 5005, BreathWeapon }, /* brass */

/*
**  Drow (edition 1)
*/
    { 5010, magic_user },	/* dracolich */
    { 5104, cleric },
    { 5103, magic_user },	/* drow mage */
    { 5107, cleric },	/* drow mat. mot */
    { 5108, magic_user },	/* drow mat. mot */
    { 5109, cleric },	/* yochlol */

/*
**   Thalos
*/
    { 5200, magic_user },	/* beholder    */

/*
**  Zoo
*/
    { 9021, snake },	/* Gila Monster */

/*
**  Castle Python
*/
    { 11001, fighter},  /* lord python */
    { 11002, fighter},
    { 11004, fighter},
    { 11005, fighter},
    { 11006, fighter},
    { 11007, fighter},
    { 11016, receptionist },
    { 11017, NudgeNudge },

/*
**  miscellaneous
*/
    { 9061, vampire},	/* vampiress  */

/*
**  White Plume Mountain
*/

    { 17004, magic_user }, /* gnyosphinx   */
    { 17017, magic_user }, /* ogre magi   */
    { 17014, ghoul },	/* ghoul  */
    { 17009, geyser },	/* geyser  */
    { 17011, vampire },	/* vampire Amelia  */
    { 17002, wraith },	/* wight*/
    { 17005, shadow },	/* shadow */
    { 17010, green_slime }, /* green slime */

/*
**  Arachnos
*/

#if 0
    { 20001, snake },	/* Young (large) spider */
#endif

    { 20003, snake },	/* wolf (giant) spider  */

    { 20005, snake },	/* queen wasp      */

#if 0
    { 20006, snake },	/* drone spider    */
#endif

    { 20010, snake },	/* bird spider     */
    { 20009, magic_user }, /* quasit         */
    { 20014, magic_user }, /* Arachnos        */
    { 20015, magic_user }, /* Ki Rin          */
    { 20011, OldHag },		/* hermit (hey, DM wrote it!) */
    { 20002, BreathWeapon }, /* Yevaud */
    { 20017, BreathWeapon }, /* Elder  */
    { 20016, BreathWeapon }, /* Baby   */
    { 20012, fighter}, /* donjonkeeper */

/*
**  SSMinnow
*/

    { 21700, fighter },		/* Captain Saltmere */
    { 21701, fighter },		/* First Mate */
    { 21702, thief },		/* Stowaway */
    { 21705, magic_user },		/* Elven Priestss */
    { 21706, fighter },		/* Jedadia */
    { 21712, fighter },		/* Crew Memeber */
    { 21714, snake },		/* Zatagaster */
    { 21716, magic_user },

/*
**  Sunsor's elf area
*/

    { 22605, timnus },         /* timnus */
    { 22604, baby_bear},	/* mother bear */
    { 22624, baby_bear},	/* baby bears. */

#if 0
/*
**   The Darklands
*/

    { 24050, cleric },
    { 24052, magic_user2 }, 
    { 24053, magic_user2 }, 
    { 24054, magic_user2 }, 
    { 24055, magic_user2 }, 
    { 24056, magic_user2 }, 
    { 24057, magic_user2 }, 
    { 24058, magic_user2 }, 
    { 24059, magic_user2 }, 
#endif

/*
**   Abbarach
*/
    { 27001, magic_user },
    { 27002, magic_user },
    { 27003, magic_user },
    { 27004, magic_user },
    { 27005, magic_user },
    { 27006, Tytan },
    { 27007, replicant },
    { 27016, AbbarachDragon },
    { 27014, magic_user },
    { 27017, magic_user },
    { 27018, magic_user },
    { 27019, magic_user },
    { 27025, Samah}, /* in skills.c ... shoot me*/

    { 27401, fighter},
    { 27403, fighter},
    { 27407, fighter},
    { 27408, fighter},
    { 27409, fighter},
    { 27411, fighter},
    { 27415, fighter},
    { 27416, fighter},
    { 27417, fighter},
    { 27418, fighter},
    { 27419, fighter},
    { 27420, fighter},

    { 27404, magic_user},
    { 27405, magic_user},
    { 27422, magic_user},

    { 27413, cleric},
    { 27414, cleric},

    { 27429, AGGRESSIVE },
    { 27430, AGGRESSIVE },

    { 7526, winger},
    { 7522, magic_user},
    { 7531, magic_user},
    {7510, fighter},
    {7514, fighter},
    {7515, fighter},
    {7516, fighter},
    {7527, fighter},
    {7528, fighter},
    {7530, fighter},

    { -1, NULL },
  };

  int	i, rnum;
  char buf[MAX_STRING_LENGTH];

  for (i=0; specials[i].vnum>=0; i++) {
    rnum = real_mobile(specials[i].vnum);
    if (rnum<0) {
      sprintf(buf, "mobile_assign: Mobile %d not found in database.",
	      specials[i].vnum);
      logE(buf);
    } else {
      mob_index[rnum].func = specials[i].proc;
    }
  }

	boot_the_shops();
	assign_the_shopkeepers();
}



/* assign special procedures to objects */
void assign_objects()
{
  obj_index[real_object_exists(15)].func = (funcp_index_item)SlotMachine;
  obj_index[real_object_exists(29)].func = (funcp_index_item)warpstone;
  obj_index[real_object_exists(30)].func = (funcp_index_item)scraps;
  obj_index[real_object_exists(23)].func = (funcp_index_item)jive_box;
  obj_index[real_object_exists(31)].func = (funcp_index_item)portal;
  obj_index[real_object_exists(3097)].func = (funcp_index_item)board;
  obj_index[real_object_exists(3098)].func = (funcp_index_item)board;
  obj_index[real_object_exists(3099)].func = (funcp_index_item)board;
  obj_index[real_object_exists(25102)].func = (funcp_index_item)board;
  obj_index[real_object_exists(21122)].func = (funcp_index_item)nodrop;
  obj_index[real_object_exists(21130)].func = (funcp_index_item)soap;
  obj_index[real_object_exists(22698)].func = (funcp_index_item)YouthPotion;
#if EGO
  obj_index[real_object_exists(40000)].func = (funcp_index_item)BitterBlade;
#endif
}


/* assign special procedures to rooms */
void assign_rooms()
{
  static struct special_proc_entry specials[] = {

    {   99,  Donation},
    { 500,   druid_challenge_prep_room},
    { 501,   druid_challenge_room},
    { 550,   monk_challenge_prep_room},
    { 551,   monk_challenge_room},
    { 3030,  dump },
    { 13547, dump },
    { 3054,  pray_for_items },

    { 2188,  Magic_Fountain},
    { 2189,  Magic_Fountain},

    { 13518, Fountain},
    { 11014, Fountain},
    { 5234,  Fountain},
    { 3141,  Fountain},
    { 13406,  Fountain},
    { 22642,  Fountain},
    { 22644,  Fountain},
    { 22646,  Fountain},
    { 22648,  Fountain},
    { 13530, pet_shops },

    { 2500, entering_turbo_lift },
    { 2639, turbo_lift },

    { 2000,  bank },
    { 13521, bank },
    { -1, NULL},
  };
  int i;
  struct room_data *rp;
  char buf[80];
  
  for (i=0; specials[i].vnum>=0; i++) {
    rp = real_roomp(specials[i].vnum);
    if (rp==NULL) {
      sprintf(buf,"assign_rooms: room %d unknown",specials[i].vnum);
      logE(buf);
    } else
      rp->funct = specials[i].proc;
  }
}
