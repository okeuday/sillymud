/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#define TO_ROOM    0
#define TO_VICT    1
#define TO_NOTVICT 2
#define TO_CHAR    3


#define SEND_TO_Q(messg, desc)  write_to_q((messg), &(desc)->output)


#define PLAYER_AUTH 0
