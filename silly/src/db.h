
/* data files used by the game system */

#define DFLT_DIR          "lib"           /* default data directory     */

#define WORLD_FILE        "tinyworld.wld" /* room definitions           */
#define MOB_FILE          "tinyworld.mob" /* monster prototypes         */
#define OBJ_FILE          "tinyworld.obj" /* object prototypes          */
#define ZONE_FILE         "tinyworld.zon" /* zone defs & command tables */
#define CREDITS_FILE      "credits"       /* for the 'credits' command  */
#define NEWS_FILE         "news"          /* for the 'news' command     */
#define MOTD_FILE         "motd"          /* messages of today          */
#define PLAYER_FILE       "players"       /* the player database        */
#define TIME_FILE         "time"          /* game calendar information  */
#define IDEA_FILE         "ideas"         /* for the 'idea'-command     */
#define TYPO_FILE         "typos"         /*         'typo'             */
#define BUG_FILE          "bugs"          /*         'bug'              */
#define MESS_FILE         "messages"      /* damage message             */
#define SOCMESS_FILE      "actions"       /* messgs for social acts     */
#define HELP_KWRD_FILE    "help_table"    /* for HELP <keywrd>          */
#define HELP_PAGE_FILE    "help"          /* for HELP <CR>              */
#define INFO_FILE         "info"          /* for INFO                   */
#define WIZLIST_FILE      "wizlist"       /* for WIZLIST                */
#define POSEMESS_FILE     "poses"         /* for 'pose'-command         */

#define REAL 0
#define VIRTUAL 1


/* structure for the reset commands */
struct reset_com
{
	char command;   /* current command                      */ 
	bool if_flag;   /* if TRUE: exe only if preceding exe'd */
	int arg1;       /*                                      */
	int arg2;       /* Arguments to the command             */
	int arg3;       /*                                      */

	/* 
	*  Commands:              *
	*  'M': Read a mobile     *
	*  'O': Read an object    *
	*  'G': Give obj to mob   *
	*  'P': Put obj in obj    *
	*  'G': Obj to char       *
	*  'E': Obj to char equip *
	*  'D': Set state of door *
	*/
};



/* zone definition structure. for the 'zone-table'   */
struct zone_data
{
  char *name;             /* name of this zone                  */
  int lifespan;           /* how long between resets (minutes)  */
  int age;                /* current age of this zone (minutes) */
  int top;                /* upper limit for rooms in this zone */
  short start;            /* has this zone been reset yet?      */
  
  int reset_mode;         /* conditions for reset (see below)   */
  struct reset_com *cmd;  /* command table for reset	           */
  char races[50];  /* races that are tolerated here */
  float CurrTotGold, LastTotGold;
  float CurrToPCGold, LastToPCGold;

  /*
   *  Reset mode:                              *
   *  0: Don't reset, and don't update age.    *
   *  1: Reset if no PC's are located in zone. *
   *  2: Just reset.                           *
   *  4: and onwards indicate different types  *
   *     of terrain and planar stuff. easier   *
   *     to stick here, than alter the form.   *
   */
};




/* element in monster and object index-tables   */
struct index_data
{
	int virtual;    /* virtual number of this mob/obj           */
	long pos;       /* file position of this field              */
	int number;     /* number of existing units of this mob/obj	*/
	int (*func)();  /* special procedure for this mob/obj       */
	char *name;
        char *short_desc;
        char *long_desc;
};




/* for queueing zones for update   */
struct reset_q_element
{
	int zone_to_reset;            /* ref to zone_data */
	struct reset_q_element *next;	
};



/* structure for the update queue     */
struct reset_q_type
{
	struct reset_q_element *head;
	struct reset_q_element *tail;
} reset_q;



struct player_index_element
{
	char *name;
	int nr;
};


struct help_index_element
{
	char *keyword;
	long pos;
};


#define ZONE_NEVER  0
#define ZONE_EMPTY  1
#define ZONE_ALWAYS 2
#define ZONE_ASTRAL 4
#define ZONE_DESERT 8
#define ZONE_ARCTIC 16
#define ZONE_HADES  32
#define ZONE_OLYMPUS 64
#define ZONE_ABYSS   128
#define ZONE_PMP     256	/* Prime Material Plane */
#define ZONE_LIMBO   512


