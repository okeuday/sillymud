#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>
#include "protos.h"


struct my_char_data {
  struct char_file_u grunt;
  short AXE;
}**dummy;


void muck(int orig_ammt, char name[80]);
void specific_axe(int orig_ammt, char name[80]);
void inactive_god_axe (int orig_ammt,time_t CURRENT_TIME);
int change_struct( int orig_ammt);
void inactive_axe(int orig_ammt, time_t CURRENT_TIME);
void zero_bank();
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
  for(i=0;i<orig_ammt;i++) 
	dummy[i]->AXE=FALSE;

  printf("\nStructures initialized.");
  printf("\nCurrent time: %s\n",time_print(CURRENT_TIME));
  do {
    printf("\n    What to do?");
    printf("\n0 -\t Exit");
    printf("\n1 -\t Routine deletion of low level inactive characters.");
    printf("\n2 -\t Routine demotion of inactive gods.");
    printf("\n3 -\t Delete a specific player.");
    printf("\n4 -\t Muck with a player in depth.");
    printf("\n5 -\t Find those who have a certain item number.");
    printf("\n6 -\t Zero all bank accounts.");
    /*     printf("\n7 -\t Attempt to convert to 400 skill spaces."); */
    printf("\n\nChanges are updated upon entering 0");
    printf("\nWell? > ");
    gets(tempbuf);
    choice=atoi(tempbuf);
    switch(choice) {
    case 1:
      inactive_axe(orig_ammt,CURRENT_TIME);
      break;
    case 2:
      inactive_god_axe(orig_ammt,CURRENT_TIME);
      break;
    case 3:
      printf("\nWhom do you want to delete?\n");
      get_string(tempbuf);
      specific_axe(orig_ammt,tempbuf);
      break;
    case 4:
      printf("\nWhom do you want to mess around with?\n");
      get_string(tempbuf);
      muck(orig_ammt,tempbuf);
      break;
    case 5:
      printf("\nWhat item number?\n");
      j=get_int(1,30000,FALSE);
      access_rent_files(orig_ammt,j);
      break;
    case 6:
      zero_bank();
      break;
    case 7:
      change_struct(orig_ammt);
      break;
    default:
      after_ammt=spit_out_remains(argv,orig_ammt);
      choice=0;
      printf("Playerfile adjusted by %d players.\n",after_ammt-orig_ammt);
      printf("\nbye!\n");
      break;
    }
  }while(choice);
}

int change_struct( int orig_ammt)
{
 /*
  int i,j;
  FILE *fl;


  for(i=0;i<200;i++) {
    grunt_insert.skills[i].learned=0;
    grunt_insert.skills[i].flags=0;
  }
    

  if(!(fl = fopen("LARGE", "w"))) {
    printf("\nCan not open %s, bye!\n","LARGE");
    exit(0);
  }

  for(i=0;i<orig_ammt;i++) {
    fwrite(&(dummy[i]->grunt1), sizeof(struct char_file_u_1), 1, fl);
    fwrite(&grunt_insert,sizeof(struct char_file_u_insert), 1, fl);
    fwrite(&(dummy[i]->grunt2), sizeof(struct char_file_u_2), 1, fl);
  }
  fclose(fl);
  printf("\nAll done!\n");
  exit(1);
 */
}

void menu2() {
  printf("\n\n\n\t You can anything below, and more :)");
  printf("\n1\t- edit levels");
  printf("\n2\t- change the room that someone starts in.");
  printf("\n3\t- change race (ghost is 29 (no can cast)).");
  printf("\n> ");
}

void muck(int orig_ammt, char name[80]) {
  void menu2();
  register i;
  int l, count, f;
  char temp[10];
  char buffer[255];

  name[0]=toupper(name[0]);
  printf("Do you wish to search from the beginning? (Y/N) ");
  gets(buffer);
  if((buffer[0] == 'y') || (buffer[0] == 'Y'))
    f = 0;
  else
    f = our_pos;

  for(i=f;i<orig_ammt;i++) {
    if(!(strcmp(name,dummy[i]->grunt.name))) {
      our_pos = i;
      menu2();
      gets(temp);
      l=atoi(temp);
      switch(l) {
      case 1:
        printf("%s:\n", dummy[i]->grunt.name);
	printf("Levels: M:%d C:%d W:%d T:%d D:%d K:%d",
	       dummy[i]->grunt.level[0],
	       dummy[i]->grunt.level[1],dummy[i]->grunt.level[2],
	       dummy[i]->grunt.level[3],dummy[i]->grunt.level[4],
	       dummy[i]->grunt.level[5]);

        for(count = 0; count <= 5; count++) {
           printf("Current Level %d: %d\n", count, dummy[i]->grunt.level[count]);
           printf("New Level [%d]: ", dummy[i]->grunt.level[count]);
           gets(buffer);
           if(buffer[0] == '\0')
             f = dummy[i]->grunt.level[count];
           else
             f = atoi(buffer);
           dummy[i]->grunt.level[count] = f;
	}

	printf("%s\'s levels: M:%d C:%d W:%d T:%d D:%d K:%d",
	       dummy[i]->grunt.name, dummy[i]->grunt.level[0],
               dummy[i]->grunt.level[1],dummy[i]->grunt.level[2],
               dummy[i]->grunt.level[3],dummy[i]->grunt.level[4],
               dummy[i]->grunt.level[5]);
	break;
      case 2:
	printf("\n%s currently starts in room %d.",dummy[i]->grunt.load_room);
	printf("\nNew room?\n");
	dummy[i]->grunt.load_room=get_int(0,40000,TRUE);
	printf("\nStart room set to %d.",dummy[i]->grunt.load_room);
	break;
      case 3:
	printf("\nCurrent race is %d. (go look it up in race.h)",
	       dummy[i]->grunt.race);
	printf("\nNew race is?\n");
	dummy[i]->grunt.race=get_int(0,49,TRUE);
	printf("\nRace set to %d.",dummy[i]->grunt.race);
	break;
      default:
	printf("\nAck, wrong choice! :P");
	break;
      }
    }
  }
}
	
      

void specific_axe(int orig_ammt, char *name) {
  register i;
  int j=FALSE;

  name[0]=toupper(name[0]);
  for(i=0;i<orig_ammt,!j;i++) {
    if(!(strcmp(name,dummy[i]->grunt.name))) {
      dummy[i]->AXE=TRUE;
      printf("%s's head found, putting it on the block.\n",
	     dummy[i]->grunt.name);
      j=TRUE;
    }
  }
  if(!j)
    printf("%s's head could not be found.",name);
}

void inactive_god_axe(int orig_ammt, time_t CURRENT_TIME) {
  register i,j,max;
  int amt=0;
  i=j=max=0;

  for(i=0;i<orig_ammt;i++) {
    if((CURRENT_TIME - dummy[i]->grunt.last_logon) > (long) (86400 * 30)) {
      for(j=0,max=0;j<6;j++) {
        if (dummy[i]->grunt.level[j] > max) {
          max = dummy[i]->grunt.level[j];
        }
      }
      if(max > 51 && max < 59) {
	printf("%s prepared for demotion.",dummy[i]->grunt.name);
	printf("  Was %d, now is",max);
	max--;
	for(j=0;j<6;j++) dummy[i]->grunt.level[j]=max;
	printf(" %d.\n",max);
        amt++;
      } else if( max == 51) {
	printf("%s will be deleted.\n", dummy[i]->grunt.name);
        dummy[i]->AXE=TRUE;
      }
    }
  }
}


void inactive_axe(int orig_ammt, time_t CURRENT_TIME) {
  register i,j,max;
  int amt=0;
  i=j=max=0;

  for(i=0;i<orig_ammt;i++) {
    if((CURRENT_TIME - dummy[i]->grunt.last_logon) > (long) (86400 * 120)) {
      for(j=0,max=0;j<6;j++) {
	if (dummy[i]->grunt.level[j] > max) {
	  max = dummy[i]->grunt.level[j];
	}
      }
      if(max <51) {
	dummy[i]->AXE=TRUE;
	amt++;
      }
    }
  }
  printf("\n%d players prepared for axing.",amt);
}

int spit_out_remains(char *argv[], int ammt) {
  FILE *fl;
  int i,j;

  if(!(fl = fopen(argv[2], "w"))) {
    printf("\nCan not open %s, bye!\n",argv[2]);
    exit(0);
  }
  i=j=0;
  for(;i<ammt;i++) {
    if(!(dummy[i]->AXE)) {
      fwrite(&(dummy[i]->grunt), sizeof(struct char_file_u), 1, fl);
      j++;
    }
  }
  return j;
}
  
int load_playerfile(char *argv[]) {
  int ammt=0;
  FILE *fl;

  if (!(fl = fopen(argv[1], "r")))        {
    printf("\nCan not open %s, bye!\n",argv[1]);
    exit(0);
  }
  dummy=(struct my_char_data **)malloc(5500 * sizeof(dummy));
  access_rent_files();
  if(dummy==NULL) {
    printf("ack\n");
  }
  for (;!feof(fl);)    {
    dummy[ammt]=(struct my_char_data *)malloc(sizeof(struct my_char_data));
    fread(&(dummy[ammt]->grunt),  sizeof(struct char_file_u), 1, fl); 
    /*     printf("\n[%s] <%d>",dummy[ammt]->grunt.name,ammt);  */
    if (!feof(fl)) 
      if(strcmp(dummy[ammt]->grunt.name,"111111")) {
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
/*
int SearchForNameFromPos(char *arg, int pos)
{
 register int i;

 if(pos > orig_amnt)
   return(-1);

 for(i = pos; i < orig_amnt; i++)
   if(!strcasecmp(dummy[i]->grunt.name, arg))
     return(i);

 return(-1);
}
*/

void access_rent_files(int number, int ITEM, char buf[40]) {
/*
  int ReadObjs(FILE *fl, struct obj_file_u *st);

  char buff2[80];
  FILE *fl;
  int i,j,tried,succeed;
  struct obj_file_u muck;

  for(i=0;i<number;i++) {
    j=tried=succeed=0;
    while(buf[j] != '\0' && j<30) {
      buf[j]=tolower(buf[j]);
      j++;
    } 
    buf[j]='\0';
    if(buf[0]=='e')
    printf("\nAtteming to open %s's rent file.",buf);
         if(strlen(buff) > sprintf) {
      (,"~/lib/rent/%s",buff); 
      if((fl=fopen(buf,"r")) != NULL ) {
	if(ReadObjs(fl,&muck)) {
	  succeed++;
	  printf("\nReading the %d objects in %s's rent file.",
		 muck.number,buff);
	  for(j=0;j<muck.number && j< 250;j++) {
	    if(muck.objects[j].item_number == (sh_int) ITEM) {
	      printf("\n%s has it!",buff);
	    } 
	  }
	} 
	
    }
  }
  printf("\nAttempted to read %d files, read %d in actuality.",i,succeed);
*/
}

int ReadObjs( FILE *fl, struct obj_file_u *st)
{
  int i;

  if (feof(fl)) {
    fclose(fl);
    printf("\nEmpty file.");
    return(FALSE);
  }
    
  fread(st->owner, sizeof(st->owner), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    printf("\nFailed to read file owner's name, [%s].",st->owner);
    return(FALSE);
  }
  fread(&st->gold_left, sizeof(st->gold_left), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    printf("\nFailed to read gold_left.");
    return(FALSE);
  }
  fread(&st->total_cost, sizeof(st->total_cost), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    printf("\nFailed to read total_cost.");
    return(FALSE);
  }
  fread(&st->last_update, sizeof(st->last_update), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    printf("\nFailed to read last update time.");
    return(FALSE);
  }
  fread(&st->minimum_stay, sizeof(st->minimum_stay), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    printf("\nFailed to read minimum stay.");
    return(FALSE);
  }
  fread(&st->number, sizeof(st->number), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    printf("\nFailed to read the number of items stored.");
    return(FALSE);
  }
  for (i=0;i<st->number;i++) {
    fread(&st->objects[i], sizeof(struct obj_file_elem), 1, fl);
  }
  return(TRUE);
}


void zero_bank()
{
  register i;

  for(i=0;i<orig_ammt;i++) {
    dummy[i]->grunt.points.bankgold = 0;
  }
  printf("All bank accounts set to 0.\n");
}
