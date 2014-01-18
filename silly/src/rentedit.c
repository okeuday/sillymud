#include <stdio.h>
#include "structs.h"
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


int ReadObjs( FILE *fl, struct obj_file_u *st)
{
  int i;
  char buf[128];

  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }

  fread(st->owner, sizeof(st->owner), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->gold_left, sizeof(st->gold_left), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->total_cost, sizeof(st->total_cost), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->last_update, sizeof(st->last_update), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->minimum_stay, sizeof(st->minimum_stay), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
  fread(&st->number, sizeof(st->number), 1, fl);
  if (feof(fl)) {
    fclose(fl);
    return(FALSE);
  }
   
  for (i=0;i<st->number;i++) {
     fread(&st->objects[i], sizeof(struct obj_file_elem), 1, fl);
  }
  
  

}


#define RENTDIR "/cis/reef3/jdb/src/silly/lib/rent/"
#define PLYRFILE "/cis/reef3/jdb/src/silly/lib/players"

void main (int argc,char *argv[])
{
  FILE *fl;
  FILE *rf;
  struct obj_file_u st;
  struct char_file_u grunt;
  int num_tried,num_succeed,i,bingo,j,found;
  char RentFileName[80];	/* sure as heck better not be longer */

  if(argc != 2) {
    printf("Usage: %s <item number>\n",argv[0]);
    exit(1);
  }

  if((fl=fopen(PLYRFILE, "r"))==NULL) {
    printf("Dangnabit, can't open the player file.\n");
    exit(1);
  }

  bingo = atoi(argv[1]);

  if(!bingo) 
    exit(1);
  printf("Hunting for item number %d\n",bingo);
  num_tried=num_succeed=0;
  
  for(j=0;!feof(fl);j++) {
    fread(&grunt, sizeof(struct char_file_u), 1, fl);
    
    if(feof(fl))
      continue;

    if(strlen(grunt.name) > 1) {
      for(i=0;grunt.name[i] != '\0' && i < 25;i++)
	grunt.name[i] = tolower(grunt.name[i]);
      
      sprintf(RentFileName,"%s%s",RENTDIR,grunt.name);
      /*       printf("%s",RentFileName); */

      if((rf=fopen(RentFileName,"r"))==NULL)
	continue;
      if(feof(rf))
	continue;

      num_tried++;
      if(ReadObjs(rf,&st)) {
	/* 	printf("  [%d]\n",st.number); */
	num_succeed++;
	
	/* 	printf("%s has %d objects in rent.\n",grunt.name,st.number); */
	for(i=0,found=0;i<st.number;i++) {
	  /* 	  printf("[%d]\n",st.objects[i].item_number); */
	  if(st.objects[i].item_number == bingo)  { 
	  
	  /* if(st.objects[i].value[3] > 0 && st.objects[i].value[3] < 13) {
	    if( ((st.objects[i].value[2]+1)/2)*st.objects[i].value[1] < 4.0 )
	    for(j=0;j<5;j++) {
	    if(st.objects[i].affected[j].location == 19 && 
	    st.objects[i].affected[j].modifier > 3) { */
	    found++;
	  }
	}
      }
      if(found)
	printf("%s has %d of them!.\n",grunt.name,found);
    } 
    fclose(rf);
  }
  printf("%d people, tried to open %d files, opened %d.\n",
	 j,num_tried,num_succeed);
}
