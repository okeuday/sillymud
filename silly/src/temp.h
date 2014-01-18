struct char_file_u_1 {
  byte sex;
  byte class;
  byte level[8];
  time_t birth;  /* Time of birth of character     */
  int played;    /* Number of secs played in total */

  int   race;
  ubyte weight;
  ubyte height;

  char title[80];
  sh_int hometown;
  char description[240];
  bool talks[MAX_TOUNGE];
  long extra_flags;

  sh_int load_room;            /* Which room to place char in  */

  struct char_ability_data abilities;

  struct char_point_data points;

  struct char_skill_data skills[200];
};

struct char_file_u_2 {
  struct affected_type affected[MAX_AFFECT];
    
  /* specials */
  
  byte spells_to_learn;
  int alignment;
  long affected_by;
  long affected_by2;
  
  time_t last_logon;  /* Time (in secs) of last logon */
  long    act;          /* ACT Flags                    */
  
  /* char data */
  char name[20];
  char pwd[11];
  sh_int apply_saving_throw[5];
  int conditions[3];
  int startroom;  /* which room the player should start in */
};

struct char_file_u_insert {
  struct char_skill_data skills[200];
};
