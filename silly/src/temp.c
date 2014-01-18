#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>
#include "protos.h"

struct char_file_old1 {
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

struct char_file_old2 {

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

struct char_file_insert {
  struct char_skill_data skills[200];
};

struct char_file_insert insert;

struct moron {
  struct char_file_old1 pt1;
  struct char_file_old2 pt2;
} **dummy;


int change_struct( int orig_ammt, char *s);
char *time_print(long et);
int load_playerfile(char *argv[]);
int orig_ammt=0, after_ammt=0;
int our_pos = 0;

int get_int(int min, int max, int zero_ok)
{
  char buff[20];   /* We should never need anything over 10^20th */
  int i,isok=FALSE;
  printf("\n");
  do {
    printf(">");
    gets(buff);
    i=atoi(buff);
    if( i > max || i < min)
      printf("Hey bozo, you can't do that.\n");
    else if(i==0 && !zero_ok)
      printf("Sorry peanut-head, that was unacceptable.\n");
    else isok=TRUE;
  }while(!isok);
  return i;
}

get_string(char *s)
{
  do {
    printf(">");
    gets(s);
    if(*s=='\0') printf("There was nothing there!\n");
    else if(*s==' ')  printf("Please do not start with a blank.\n");
  }while(*s=='\0' || *s==' ');
}

main(int argc, char *argv[])
{
  char tempbuf[255];
  time_t CURRENT_TIME;
  int choice,j;
  register i;
  int number_level[9];
  
  system("clear");
  if(argc != 3) {
    printf("please use this program in the following format:");
    printf("\n<%s> <input file> <output file>\n",argv[0]);
    exit(0);
  }
  if(!strcmp(argv[1],argv[2])) {
    printf("\nPlease use a seperate files for input and output.\n");
    exit(0);
  }
  orig_ammt=load_playerfile(argv);
  CURRENT_TIME=time(0);

  printf("\nCurrent time: %s\n",time_print(CURRENT_TIME));
  printf("Attempting to convert player file.\n");
  change_struct(orig_ammt, argv[2]);
  printf("Done, new player file is in %s.", argv[2]);
}

int change_struct( int orig_ammt, char *s)
{
  int i,j;
  FILE *fl;

  for(i=0;i<200;i++) {
    insert.skills[i].learned=0;
    insert.skills[i].flags=0;
  }

  if(!(fl = fopen(s, "w"))) {
    printf("\nCan not open %s, bye!\n",s);
    exit(0);
  }

  for(i=0;i<orig_ammt;i++) {
    fwrite(&(dummy[i]->pt1), sizeof(struct char_file_old1), 1, fl);
    fwrite(&insert,sizeof(struct char_file_insert), 1, fl);
    fwrite(&(dummy[i]->pt2), sizeof(struct char_file_old2), 1, fl);
  }
  fclose(fl);
  printf("\nAll done!\n");
}

  
int load_playerfile(char *argv[]) {
  int ammt=0;
  FILE *fl;

  if (!(fl = fopen(argv[1], "r")))        {
    printf("\nCan not open %s, bye!\n",argv[1]);
    exit(0);
  }
  dummy=(struct moron**)malloc(10000 * sizeof(struct moron));
  /*   access_rent_files(); */
  if(dummy==NULL) {
    printf("ack!\n");
    exit(1);
  }
  for (;!feof(fl);)    {
    dummy[ammt]=(struct moron *)malloc(sizeof(struct moron));
    fread(&(dummy[ammt]->pt1), sizeof(struct char_file_old1), 1, fl); 
    fread(&(dummy[ammt]->pt2), sizeof(struct char_file_old2), 1, fl); 
    /*     printf("\n[%s] <%d>",dummy[ammt]->grunt.name,ammt);  */
    if (!feof(fl)) {
      ammt++;
    }
  }
  printf("\n%d players read.\n",ammt);
  return ammt;
}
  

char *time_print(long et)
{
  char *buff;

  buff= (char *)asctime(localtime(&et));
  *(buff + strlen(buff) -1) = '\0';
  return(buff);
}
