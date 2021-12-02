/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "protos.h"


#define MAX_MSGS 99	            /* Max number of messages.          */
#define MAX_MESSAGE_LENGTH 2048     /* that should be enough            */
#define NUM_BOARDS 3

struct message {
  char *date;
  char *title;
  char *author;
  char *text;
};

struct board {
  struct message msg[MAX_MSGS+1];
  int number;
};

static struct board_lock_struct {
  struct char_data *locked_for;
  bool lock;
} board_lock[NUM_BOARDS];


int min_read_level[] = { 0, 51, 1};
int min_write_level[] = { 1, 51, 1};
int min_remove_level[] = { 51, 51, 51};

struct board boards[NUM_BOARDS];
struct board *curr_board;
struct message *curr_msg;  
extern struct char_data *character_list;

/* This sets the minimum level needed to read/write/look at these boards
   mainly included to enable the creation of a "wizard-only" board        */

char save_file[NUM_BOARDS][20] = { 
  "mortal.board" , "wiz.board", "skexie.board" };

    
/* These are the binary files in which to save/load messages */

static int fwrite_string(char *buf, FILE *fl);

/* board.c version 1.2 - Jun 1991 by Twilight.

1.2 changes:

c   Added a board and message structure
   took out all pointers in an effort to insure integrity in memory.
   Added differentiation between minimum read levels and minimum write/remove
   levels.

1.1 changes:

   Major repairs-- now allows multiple boards define at compile-time.  Set the
   constants NUM_BOARDS and add the new V-Numbers to the if/then structure directly
   below.  Also you must attach the board.c procedure in spec_assign.c as usual.

   Log message removals and restrict them to level 15 and above.
   Fixed act message resulting from message removal 
   Removed unused procedure "fix_long_desc"
   Added a message to inform others in room of a read in progress
   Added minimum level check for each board 
   (defined in array min_board_level[NUM_BOARDS]

*/

static void board_load_board()
{
  FILE *the_file;
  int ind;
  int bnum;
  char buf[256];
  
  memset(boards, 0, sizeof(boards)); /* Zero out the array, make sure no */
                                     /* Funky pointers are left in the   */
                                     /* Allocated space                  */

  for ( bnum = 0 ; bnum < NUM_BOARDS ; bnum++ ) {
    board_lock[bnum].lock = 0;
    board_lock[bnum].locked_for = 0;
  }

  for (bnum = 0; bnum < NUM_BOARDS; bnum++) {
    boards[bnum].number = -1;
    the_file = fopen(save_file[bnum], "r");
    if (!the_file) {
      sprintf(buf,"Can't open message file for board %d.\n\r",bnum);
      logE(buf);
      continue;
    }

    fscanf( the_file, " %d ", &boards[bnum].number);
    if (boards[bnum].number < 0 || boards[bnum].number > MAX_MSGS || 
	feof(the_file)) {
      logE("Board-message file corrupt, nonexistent, or empty.\n\r");
      boards[bnum].number = -1;
      fclose(the_file);
      continue;
    }

    curr_board = &boards[bnum];

    for (ind = 0; ind <= curr_board->number; ind++) {
      curr_msg = &curr_board->msg[ind];
      curr_msg->title = (char *)fread_string (the_file);
      curr_msg->author = (char *)fread_string (the_file);
      curr_msg->date = (char *)fread_string (the_file);
      curr_msg->text = (char *)fread_string (the_file);
    }
    fclose(the_file);
  }
}

static int board_show_board(struct char_data *ch, char *arg, int bnum)
{
  int i;
  char buf[MAX_STRING_LENGTH+50], tmp[MAX_INPUT_LENGTH];
                          /* ^ had a few bus errors, *shrug* */
  one_argument(arg, tmp);

  if (!*tmp || !isname(tmp, "board bulletin"))
    return(0);

  if ((GetMaxLevel(ch) < min_read_level[bnum]) && (bnum !=5)) 
    /* Skip if board 5 (Reimb board) */
{ 
    send_to_char("You try and look at the messages on the board but you\n\r",ch);
    send_to_char("cannot comprehend their meaning.\n\r",ch);
    act("$n tried to read the board, but looks bewildered.",TRUE,ch, 0, 0, TO_ROOM);
    return(1);
  }

  curr_board = &boards[bnum];

  act("$n studies the board.", TRUE, ch, 0, 0, TO_ROOM);

  strcpy(buf,"This is a bulletin board. Usage: READ/REMOVE <messg #>, WRITE <header>\n\r");
  if (boards[bnum].number == -1)
    strcat(buf, "The board is empty.\n\r");
  else {
    sprintf(buf + strlen(buf), "There are %d messages on the board.\n\r",
	    curr_board->number);
    sprintf(buf + strlen(buf), "\n\rBoard Topic:\n\r%s------------\n\r",curr_board->msg[0].text);
    for ( i = 1 ; i <= curr_board->number ; i++ ) 
/*      if (((GET_MAX_LEVEL(ch) < min_read_level[bnum]) &&
           (strcmp(ch->name, curr_board->msg[i].author))) ||
          (GET_MAX_LEVEL(ch) >= min_read_level[bnum]))  */
       sprintf(buf + strlen(buf), "%-2d : %-15s (%s) -- %s\n\r", i , 
               curr_board->msg[i].author, curr_board->msg[i].date,
               curr_board->msg[i].title);
  }
  page_string(ch->desc, buf, 1);
  return(1);
}

static char *fix_returns(char *text_string)
{
  char *localbuf;
  int point=0;
  int point2 = 0;

  if (!text_string) {
    CREATE(localbuf,char,2);
    strcpy(localbuf,"\n");
    return(localbuf);
  }

  if (!(*text_string)) {
    CREATE(localbuf,char,strlen("(NULL)")+1);
    strcpy(localbuf,"(NULL)");
    return(localbuf);
  }

  CREATE(localbuf,char,strlen(text_string));

  while(*(text_string+point) != '\0') 
    if (*(text_string+point) != '\r') {
      *(localbuf+point2) = *(text_string+point);
      point2++;
      point++;
    }
    else
      point++;
  *(localbuf + point2) = '\0'; /* You never made sure of null termination */
  return(localbuf);
}

static void board_save_board(int bnum)
{
  FILE *the_file;
  int ind;
  char buf[256];
  char *temp_add;

  /* We're assuming the board number is valid since it was passed by
     out own code */

  curr_board = &boards[bnum];

  the_file = fopen(save_file[bnum], "w");

  if (!the_file) {
      logE("Unable to open/create savefile for bulletin board..\n\r");
      return;
    }

  fprintf(the_file," %d ", curr_board->number);
  for (ind = 0; ind <= curr_board->number; ind++) {
    curr_msg = &curr_board->msg[ind];
    fwrite_string(curr_msg->title, the_file);
    fwrite_string(curr_msg->author, the_file);
    fwrite_string(curr_msg->date, the_file);
    fwrite_string((temp_add = fix_returns(curr_msg->text)), the_file);
    free(temp_add);
  }
  fclose(the_file);
  return;
}

static int board_check_locks(int bnum, struct char_data *ch)
{
  char buf[MAX_INPUT_LENGTH];
  struct char_data *tmp_char;
  bool found = FALSE;
  if (!board_lock[bnum].lock) return(0);
  
  /* FIRST lets' see if this character is even in the game anymore! -WG-*/
  for (tmp_char = character_list; tmp_char; tmp_char = tmp_char->next)
    {
      if (tmp_char == board_lock[bnum].locked_for)
        {
          found = TRUE;
          break;
        }
    }
  if (!found)
    {
      logE("Board: board locked for a user not in game.");
      board_lock[bnum].lock = 0;
      board_lock[bnum].locked_for = NULL;
      return(0);
    }

  /* Check for link-death of lock holder */

  if (!board_lock[bnum].locked_for->desc) {
    sprintf(buf,"You push %s aside and approach the board.\n\r",board_lock[bnum].locked_for->player.name);
    send_to_char(buf, ch);
  }

  /* Else see if lock holder is still in write-string mode */

  else if (board_lock[bnum].locked_for->desc->str) { /* Lock still holding */
    sprintf(buf,"You try to approach the board but %s blocks your way.\n\r",board_lock[bnum].locked_for->player.name);
    send_to_char(buf, ch);
    return (1);
  }

  /* Otherwise, the lock has been lifted */

  board_save_board(bnum);
  board_lock[bnum].lock = 0;
  board_lock[bnum].locked_for = 0;
  return(0);
}

static void board_write_msg(struct char_data *ch, char *arg, int bnum) {

  int highmessage;
  char buf[MAX_STRING_LENGTH];
  long ct; /* clock time */
  char *tmstr;

  extern struct time_info_data time_info;
  extern char *month_name[];

  if ( bnum == -1 ) {
    logE("Board special procedure called for non-board object.\r\n");
    send_to_char("This board is not in operation at this time.\n\r", ch);
    return;
  }

  curr_board = &boards[bnum];

  if (GetMaxLevel(ch) < min_write_level[bnum]) {
    send_to_char("You pick up a quill to write, but realize you're not powerful enough\n\r",ch);
    send_to_char("to submit intelligent material to THIS board.\n\r",ch);
    return;
  }

  if ( (curr_board->number) > (MAX_MSGS - 1) ) {
    send_to_char("The board is full already.\n\r", ch);
    return;
  }

  /* Check for locks, return if lock is found on this board */

  if (board_check_locks(bnum, ch))
    return;

  /* skip blanks */

  for(; isspace(*arg); arg++);

  if (!*arg) {
    send_to_char("The board has now been saved permanently to disk.\n\rTo write a new message, use WRITE followed by a title.\n\r", ch);
    return;
  }

  /* Now we're committed to writing a message.  Let's lock the board. */

  board_lock[bnum].lock = 1;
  board_lock[bnum].locked_for = ch;

  /* Lock set */

  highmessage = boards[bnum].number;
  curr_msg = &curr_board->msg[++highmessage];

  if (!(strcmp("Topic",arg))) {
    curr_msg = &curr_board->msg[0];
    free(curr_msg->title);
    free(curr_msg->text);
    free(curr_msg->author);
    free(curr_msg->date);
    (boards[bnum].number)--;
  }
  curr_msg->title = (char *)malloc(strlen(arg)+1);
  strcpy(curr_msg->title, arg);
  curr_msg->author = (char *)malloc(strlen(GET_NAME(ch))+1);
  strcpy(curr_msg->author, GET_NAME(ch));
  ct = time(0);
  tmstr = (char *)asctime(localtime(&ct));
  *(tmstr + strlen(tmstr) - 1) = '\0';
  sprintf(buf,"%.10s",tmstr);
  curr_msg->date = (char *)malloc(strlen(buf)+1);
  strcpy(curr_msg->date, buf);
  send_to_char("Write your message. Terminate with a @.\n\r\n\r", ch);
  act("$n starts to write a message.", TRUE, ch, 0, 0, TO_ROOM);

  /* Take care of free-ing and zeroing if the message text is already
     allocated previously */

  if (curr_msg->text)
    free (curr_msg->text);
  curr_msg->text = 0;

  /* Initiate the string_add procedures from comm.c */

  ch->desc->str = &curr_msg->text;
  ch->desc->max_str = MAX_MESSAGE_LENGTH;
  (boards[bnum].number)++;
  if (boards[bnum].number < 0)
    boards[bnum].number = 0;
}

static int board_display_msg(struct char_data *ch, char *arg, int bnum)
{
  char buf[512], number[MAX_INPUT_LENGTH], buffer[MAX_STRING_LENGTH];
  int tmessage;

  one_argument(arg, number);

  if (!*number || !isdigit(*number))
    return(0);

  if (!(tmessage = atoi(number))) return(0);

  curr_board = &boards[bnum];

  if ((boards[bnum].number != -1) &&
      (tmessage >= 0 && tmessage <= curr_board->number) &&
      (GetMaxLevel(ch) < min_read_level[bnum]) &&
      (strcmp(GET_NAME(ch), curr_board->msg[tmessage].author))) ;
  else 
  if ( GetMaxLevel(ch) < min_read_level[bnum] ) {
    send_to_char("You try and look at the messages on the board but you\n\r",
                 ch);
    send_to_char("cannot comprehend their meaning.\n\r\n\r",ch);
    act("$n tried to read the board, but looks bewildered.",TRUE,ch, 0, 0,
        TO_ROOM);
    return(1);
  }

  if (boards[bnum].number == -1) {
    send_to_char("The board is empty!\n\r", ch);
    return(1);
  }
  
  if (tmessage < 0 || tmessage > curr_board->number) {
    send_to_char("That message exists only in your imagination..\n\r",ch);
    return(1);
  }

  curr_msg = &curr_board->msg[tmessage];

  sprintf(buffer, "Message %2d (%s): %-15s -- %s", tmessage, curr_msg->date, curr_msg->author, curr_msg->title );
  sprintf(buffer + strlen(buffer), "\n\r----------\n\r%s", (curr_msg->text?curr_msg->text:"(null)"));
  page_string(ch->desc, buffer, 1);
  return(1);

/*
  sprintf(buf, "$n reads message %d titled : %s.",tmessage, curr_msg->title);
  act(buf, TRUE, ch, 0, 0, TO_ROOM);
*/
}

static int board_remove_msg(struct char_data *ch, char *arg, int bnum)
{
  /* This should now be fixed so that low level chars can remove armor and such. */

  int ind, tmessage;
  char buf[256], number[MAX_INPUT_LENGTH];
  
  one_argument(arg, number);
  
  if (!*number || !isdigit(*number))
    return(0);
  
  if (!(tmessage = atoi(number))) return(0);
  
  if ( bnum == -1 ) {
    logE("Board special procedure called for non-board object.\r\n");
    send_to_char("This board is not in operation at this time.\n\r", ch);
    return 1;
  }

  curr_board = &boards[bnum];

  if (GetMaxLevel(ch) < min_remove_level[bnum]) {
    send_to_char("You try and grab one of the notes of the board but get a nasty\n\r",ch);
    send_to_char("shock.  Maybe you'd better leave it alone.\n\r",ch);
    return 1;
  }

  if (curr_board->number < 1) {
    send_to_char("The board is empty!\n\r", ch);
    return(1);
  }

  if (tmessage < 0 || tmessage > curr_board->number) {
    send_to_char("That message exists only in your imagination..\n\r",
		 ch);
    return(1);
  }

  /* Check for board locks, return if lock is found */
  
  if (board_check_locks(bnum, ch))
    return(1);

  ind = tmessage;

  free(curr_board->msg[ind].text);
  free(curr_board->msg[ind].date);
  free(curr_board->msg[ind].author);
  free(curr_board->msg[ind].title);

  for ( ; ind < (curr_board->number) ; ind++ )
    curr_board->msg[ind] = curr_board->msg[ind+1];

/* You MUST do this, or the next message written after a remove will */
/* end up doing a free(curr_board->msg[ind].text) because it's not!! */
/* Causing strange shit to happen, because now the message has a     */
/* To a memory location that doesn't exist, and if THAT message gets */
/* Removed, it will destroy what it's pointing to. THIS is the board */
/* Bug we've been looking for!        -=>White Gold<=-               */

  curr_board->msg[curr_board->number].text = NULL;
  curr_board->msg[curr_board->number].date = NULL;
  curr_board->msg[curr_board->number].author = NULL;
  curr_board->msg[curr_board->number].title = NULL;

  curr_board->number--;

  send_to_char("Message removed.\n\r", ch);
  sprintf(buf, "%s just removed message %d.", ch->player.name, tmessage);

  /* Removal message also repaired */

  act(buf, FALSE, ch, 0, 0, TO_ROOM);
  sprintf((buf+strlen(buf)-1)," from board %d.",bnum);
  logE(buf);  /* Message removals now logged. */

  board_save_board(bnum);
  return(1);
}

int board(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type)
{
  static int has_loaded = 0;
  char buf[80];
  int bnum = -1;
  int obj_num;

  if (type != PULSE_COMMAND)
    return(FALSE);

  if (!ch->desc)
    return(0); /* By MS or all NPC's will be trapped at the board */
  
  if (!has_loaded)
    {
      board_load_board();
      has_loaded = 1;
    }

  if (!cmd)
    return(FALSE);

  /* Identify which board we're dealing with */
  
  obj_num = (obj->item_number);
  if (obj_num == (real_object(3099)))  bnum = 0;
  else if (obj_num == (real_object(3098)))  bnum = 1;
  else if (obj_num == (real_object(3097))) bnum = 2;

  switch (cmd) {
  case 15:  /* look */
    return(board_show_board(ch, arg, bnum));
  case 149: /* write */
    board_write_msg(ch, arg, bnum);
    return 1;
  case 63: /* read */
    return(board_display_msg(ch, arg, bnum));
  case 66: /* remove */
    return(board_remove_msg(ch, arg, bnum));
  default:
    return 0;
  }
}

static int fwrite_string (char *buf, FILE *fl)
{
  return (fprintf(fl, "%s~\n", buf));
}

