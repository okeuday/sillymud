/*-*-Mode:C;coding:utf-8;tab-width:8;c-basic-offset:2;indent-tabs-mode:()-*-
 * ex: set ft=cpp fenc=utf-8 sts=2 ts=8 sw=2 et:

  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>

#include "protos.h"

#define DFLT_PORT 4000        /* default port */
#define MAX_NAME_LENGTH 15
#define MAX_HOSTNAME   256
#if 1
#define OPT_USEC 250000       /* time delay corresponding to 4 passes/sec */
#else
#if TITAN
#define OPT_USEC 125000       /* lets see if this changes the lag any */
#else
#define OPT_USEC 250000       /* time delay corresponding to 4 passes/sec */
#endif
#endif

#define STATE(d) ((d)->connected)

extern int errno;
extern char *sector_types[];

/* extern struct char_data *character_list; */
#if HASH
extern struct hash_header room_db;	  /* In db.c */
#else
extern struct room_data *room_db;	  /* In db.c */
#endif

extern int top_of_world;            /* In db.c */
extern struct time_info_data time_info;  /* In db.c */
extern char help[];
extern char login[];
extern char *room_bits[];

struct descriptor_data *descriptor_list, *next_to_process;

int lawful = 0;		/* work like the game regulator */
int slow_death = 0;     /* Shut her down, Martha, she's sucking mud */
int mudshutdown = 0;       /* clean shutdown */
int reboot = 0;         /* reboot the game after a shutdown */
int no_specials = 0;    /* Suppress ass. of special routines */
long Uptime;            /* time that the game has been up */


int pulse = 0;

#if SITELOCK
char hostlist[MAX_BAN_HOSTS][30];  /* list of sites to ban           */
int numberhosts;
#endif

int maxdesc, avail_descs;
int tics = 0;        /* for extern checkpointing */

static unsigned char echo_on[]  = {IAC, WONT, TELOPT_ECHO, '\r', '\n', '\0'};
static unsigned char echo_off[] = {IAC, WILL, TELOPT_ECHO, '\0'};


/* *********************************************************************
*  main game loop and related stuff				       *
********************************************************************* */

int __main ()
{
  return(1);
}

/* jdb code - added to try to handle all the different ways the connections
   can die, and try to keep these 'invalid' sockets from getting to select
*/

int close_socket_fd( int desc)
{
  struct descriptor_data *d;
  extern struct descriptor_data *descriptor_list;

  for (d = descriptor_list;d;d=d->next) {
    if (d->descriptor == desc) {
      close_socket(d);
    }
  }
}

int main (int argc, char **argv)
{
  int cloudi = 0;  /* run within CloudI */
  int port, pos=1;
  char buf[512], *dir;
  extern int WizLock;
  
  port = DFLT_PORT;
  dir = DFLT_DIR;

  while ((pos < argc) && (*(argv[pos]) == '-'))	{
    switch (*(argv[pos] + 1))  {
    case 'c':
      cloudi = 1;
      logE("CloudI mode selected.");
      break;
    case 'l':
      lawful = 1;
      logE("Lawful mode selected.");
      break;
    case 'd':
      if (*(argv[pos] + 2))
	dir = argv[pos] + 2;
      else if (++pos < argc)
	dir = argv[pos];
      else   	{
	logE("Directory arg expected after option -d.");
	assert(0);
      }
      break;
    case 's':
      no_specials = 1;
      logE("Suppressing assignment of special routines.");
      break;
    default:
      sprintf(buf, "Unknown option -% in argument string.",
	      *(argv[pos] + 1));
      logE(buf);
      break;
    }
    pos++;
  }
  
  if (pos < argc)
    if (!isdigit(*argv[pos]))      	{
      fprintf(stderr, "Usage: %s [-l] [-s] [-d pathname] [ port # ]\n", 
	      argv[0]);
      assert(0);
    }  else if ((port = atoi(argv[pos])) <= 1024)  {
      printf("Illegal port #\n");
      assert(0);
    }
  
  Uptime = time(0);
  
  sprintf(buf, "Running game on port %d.", port);
  logE(buf);
  
  if (chdir(dir) < 0)	{
    perror("chdir");
    assert(0);
  }
  
  sprintf(buf, "Using %s as data directory.", dir);
  logE(buf);
  
  srandom(time(0));
  WizLock = FALSE;

#if SITELOCK
  log("Blanking denied hosts.");
  for(a = 0 ; a<= MAX_BAN_HOSTS ; a++) 
    strcpy(hostlist[a]," \0\0\0\0");
  numberhosts = 0;
#if LOCKGROVE
  log("Locking out Host: oak.grove.iup.edu.");
  strcpy(hostlist[0],"oak.grove.iup.edu");
  numberhosts = 1; 
  log("Locking out Host: everest.rutgers.edu.");
  strcpy(hostlist[1],"everest.rutgers.edu");
  numberhosts = 2;
#endif /* LOCKGROVE */
#endif



  /* close stdin */
  close(0);

  if (cloudi)
    run_the_game_with_cloudi();
  else
    run_the_game(port);
  return(0);
}



#define PROFILE(x)


/* Init sockets, run game, and cleanup sockets */
int run_the_game(int port)
{
  int s; 
  PROFILE(extern etext();)
    
    void signal_setup(void);
  int load(void);
  
  PROFILE(monstartup((int) 2, etext);)
    
    descriptor_list = NULL;
  
  logE("Signal trapping.");
  signal_setup();
  
  logE("Opening mother connection.");
  s = init_socket(port);
  
  if (lawful && load() >= 6)
    {
      logE("System load too high at startup.");
      coma(1);
    }
  
  boot_db();
  
  logE("Entering game loop.");
  
  game_loop(s);
  
  close_sockets(s); 
  
  PROFILE(monitor(0);)
    
  if (reboot)  {
    logE("Rebooting.");
    assert(52);           /* what's so great about HHGTTG, anyhow? */
  }
  
  logE("Normal termination of game.");
}






/* Accept new connects, relay commands, and call 'heartbeat-functs' */
int game_loop(int s)
{
  fd_set input_set, output_set, exc_set;
#if 0
  fd_set tin, tout, tex;
  fd_set mtin, mtout, mtex;
#endif
  static int cap;
  struct timeval last_time, now, timespent, timeout, null_time;
  static struct timeval opt_time;
  char comm[MAX_STRING_LENGTH];
  char promptbuf[80];
  struct descriptor_data *point, *next_point;
  sigset_t mask;
  struct room_data *rm;

  extern struct descriptor_data *descriptor_list;
  extern int pulse;
  extern int maxdesc;
  
  null_time.tv_sec = 0;
  null_time.tv_usec = 0;
  
  opt_time.tv_usec = OPT_USEC;  /* Init time values */
  opt_time.tv_sec = 0;
  gettimeofday(&last_time, (struct timezone *) 0);
  
  maxdesc = s;
  /* !! Change if more needed !! */
  avail_descs = getdtablesize() -2;
  
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGPIPE);
  sigaddset(&mask, SIGALRM);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGURG);
  sigaddset(&mask, SIGXCPU);
  sigaddset(&mask, SIGHUP);
  
  /* Main loop */
  while (!mudshutdown)  {

    /* Check what's happening out there */

    FD_ZERO(&input_set);
    FD_ZERO(&output_set);
    FD_ZERO(&exc_set);

    FD_SET(s, &input_set);
    
#if TITAN
    maxdesc = 0;
    if (cap < 20)
      cap = 20;
    for (point = descriptor_list; point; point = point->next)  {

      if (point->descriptor <= cap && point->descriptor >= cap-20) {
	FD_SET(point->descriptor, &input_set);
	FD_SET(point->descriptor, &exc_set);
	FD_SET(point->descriptor, &output_set);
      }

      if (maxdesc < point->descriptor)
	maxdesc = point->descriptor;
    }

    if (cap > maxdesc)
      cap = 0;
    else
      cap += 20;
#else
    for (point = descriptor_list; point; point = point->next)  {
      FD_SET(point->descriptor, &input_set);
      FD_SET(point->descriptor, &exc_set);
      FD_SET(point->descriptor, &output_set);
      
      if (maxdesc < point->descriptor)
	maxdesc = point->descriptor;
    }
#endif

    /* check out the time */
    gettimeofday(&now, (struct timezone *) 0);
    timespent = timediff(&now, &last_time);
    timeout = timediff(&opt_time, &timespent);
    last_time.tv_sec = now.tv_sec + timeout.tv_sec;
    last_time.tv_usec = now.tv_usec + timeout.tv_usec;
    if (last_time.tv_usec >= 1000000) {
      last_time.tv_usec -= 1000000;
      last_time.tv_sec++;
    }
    
    sigprocmask(SIG_BLOCK, &mask, 0);

    if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time) 
	< 0)   	{
      perror("Select poll");
/* one of the descriptors is broken... */
      for (point = descriptor_list; point; point = next_point)  {
	next_point = point->next;
	write_to_descriptor(point->descriptor, "\n\r");
      }
    }

    if (select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0) {
      perror("Select sleep");
      /*assert(0);*/
    }
    
    sigprocmask(SIG_UNBLOCK, &mask, 0);
    
    /* Respond to whatever might be happening */
    
    /* New connection? */
    if (FD_ISSET(s, &input_set))
      if (new_descriptor(s) < 0) {
	perror("New connection");
      }

    /* kick out the freaky folks */
    for (point = descriptor_list; point; point = next_point)  {
      next_point = point->next;   
      if (FD_ISSET(point->descriptor, &exc_set))  {
	FD_CLR(point->descriptor, &input_set);
	FD_CLR(point->descriptor, &output_set);
	close_socket(point);
      }
    }
    
    for (point = descriptor_list; point; point = next_point)  {
      next_point = point->next;
      if (FD_ISSET(point->descriptor, &input_set))
	if (process_input(point) < 0) 
	  close_socket(point);
    }
    
    /* process_commands; */
    for (point = descriptor_list; point; point = next_to_process){
      next_to_process = point->next;
      
      if ((--(point->wait) <= 0) && get_from_q(&point->input, comm))	{
	  if (point->character && point->connected == CON_PLYNG &&
	      point->character->specials.was_in_room !=	NOWHERE) {
      
	      point->character->specials.was_in_room = NOWHERE;
	      act("$n has returned.",	TRUE, point->character, 0, 0, TO_ROOM);
	    }
	  
	  point->wait = 1;
	  if (point->character)
	    point->character->specials.timer = 0;
	  point->prompt_mode = 1;
	  
	  if (point->str)
	    string_add(point, comm);
	  else if (!point->connected) {
	    if (point->showstr_point)
	      show_string(point, comm);
	    else
	      command_interpreter(point->character, comm);
          }
          else if(point->connected == CON_EDITING)
            RoomEdit(point->character, comm);
	  else 
	    nanny(point, comm); 
	}
    }
    
    /* either they are out of the game */
    /* or they want a prompt.          */
    
    for (point = descriptor_list; point; point = next_point) {
	next_point = point->next;
	if (FD_ISSET(point->descriptor, &output_set) && point->output.head)
	  if (process_output(point) < 0)
	    close_socket(point);
	  else
	    point->prompt_mode = 1;
      }
    
    /* give the people some prompts  */
    for (point = descriptor_list; point; point = point->next)
      if (point->prompt_mode) {
	if (point->str)
	  write_to_descriptor(point->descriptor, "-> ");
	else if (!point->connected)
	  if (point->showstr_point)
	    write_to_descriptor(point->descriptor,
				"[Return to continue/Q to quit]");
	  else { 
	    struct char_data *ch;
	    ch = point->character;

            if(point->character->term == VT100) {
	      int update = 0;
 
	      if(GET_MOVE(ch) != ch->last.move) {
		SET_BIT(update, INFO_MOVE);
		ch->last.move = GET_MOVE(ch);
	      }
	      if(GET_MAX_MOVE(ch) != ch->last.mmove) {
		SET_BIT(update, INFO_MOVE);
		ch->last.mmove = GET_MAX_MOVE(ch);
	      }
	      if(GET_HIT(ch) != ch->last.hit) {
		SET_BIT(update, INFO_HP);
		ch->last.hit = GET_HIT(ch);
	      }
	      if(GET_MAX_HIT(ch) != ch->last.mhit) {
		SET_BIT(update, INFO_HP);
		ch->last.mhit = GET_MAX_HIT(ch);
	      }
	      if(GET_MANA(ch) != ch->last.mana) {
		SET_BIT(update, INFO_MANA);
		ch->last.mana = GET_MANA(ch);
	      }
	      if(GET_MAX_MANA(ch) != ch->last.mmana) {
		SET_BIT(update, INFO_MANA);
		ch->last.mmana = GET_MAX_MANA(ch);
	      }
	      if(GET_GOLD(ch) != ch->last.gold) {
		SET_BIT(update, INFO_GOLD);
		ch->last.gold = GET_GOLD(ch);
	      }
	      if(GET_EXP(ch) != ch->last.exp) {
		SET_BIT(update, INFO_EXP);
		ch->last.exp = GET_EXP(ch);
	      }
	      
	      if(update)
		UpdateScreen(ch, update);
	    }

	    if (point->character->term != VT100) {
	      if (IS_IMMORTAL(point->character)) {
		rm = real_roomp(point->character->in_room);
		if (!rm) {
		  char_to_room(point->character, 0);
		  rm = real_roomp(point->character->in_room);
		}

		if(IS_SET(ch->specials.prompt, PROMPT_R)) {
		  sprintf(promptbuf,"<Rm: %d ", rm->number);
		  write_to_descriptor(point->descriptor, promptbuf);
		}
                if(IS_SET(ch->specials.prompt, PROMPT_S)) {
		  sprinttype(rm->sector_type,sector_types,promptbuf);
		  write_to_descriptor(point->descriptor, promptbuf);
		  write_to_descriptor(point->descriptor, " ");
		}
                if(IS_SET(ch->specials.prompt, PROMPT_F)) {
		  sprintbit((unsigned long)rm->room_flags,room_bits,promptbuf);
		  write_to_descriptor(point->descriptor, promptbuf);
		}
		sprintf(promptbuf, "> ");
		write_to_descriptor(point->descriptor, promptbuf);
	      } else {
		if(IS_SET(ch->specials.prompt, PROMPT_H)) {
		  sprintf(promptbuf,"H:%d ",
			  point->character->points.hit);
		  write_to_descriptor(point->descriptor, promptbuf);
		}
		if(IS_SET(ch->specials.prompt, PROMPT_M)) {
		  sprintf(promptbuf,"M:%d ",
			  point->character->points.mana);
		  write_to_descriptor(point->descriptor, promptbuf);
		}
		if(IS_SET(ch->specials.prompt, PROMPT_V)) {
		  sprintf(promptbuf,"V:%d ",
			  point->character->points.move);
		  write_to_descriptor(point->descriptor, promptbuf);
		}
		sprintf(promptbuf, "> ");
		write_to_descriptor(point->descriptor, promptbuf);
	      }
	    } else {
	      sprintf(promptbuf, "> ");
	      write_to_descriptor(point->descriptor, promptbuf);
	    }
	  }
	point->prompt_mode = 0;
      }
    
    
    /* handle heartbeat stuff */
    /* Note: pulse now changes every 1/4 sec  */
    
    pulse++;
    
    if (!(pulse % PULSE_ZONE))  {
      zone_update();
      if (lawful)
	gr(s);
    }
    
    
    if (!(pulse % PULSE_RIVER)) {
      RiverPulseStuff(pulse);
    }
    
    if (!(pulse % PULSE_TELEPORT)) {
      TeleportPulseStuff(pulse);
    }
    
    if (!(pulse % PULSE_VIOLENCE))
      perform_violence( pulse );
    
    
    if (!(pulse % (SECS_PER_MUD_HOUR*4))){
      weather_and_time(1);
      affect_update(pulse);  /* things have been sped up by combining */
      if ( time_info.hours == 1 )
	update_time();
    }
    
    if (pulse >= 2400) {
      pulse = 0;
      if (lawful)
	night_watchman();
      check_reboot();
    }
    
    tics++;        /* tics since last checkpoint signal */
  }
}






/* ******************************************************************
*  general utility stuff (for local use)									 *
****************************************************************** */




int get_from_q(struct txt_q *queue, char *dest)
{
	struct txt_block *tmp;

 	/* Q empty? */
	if (!queue->head)
		return(0);

	if (!dest) {
	  log_sev("Sending message to null destination.", 5);
	  return(0);
	}

	tmp = queue->head;
	if (dest && queue->head->text) {
	    strncpy(dest, queue->head->text, MAX_STRING_LENGTH);
            dest[MAX_STRING_LENGTH - 1] = '\0';
	}
	queue->head = queue->head->next;
        if (!queue->head)
          queue->tail = 0;

	free(tmp->text);
	free(tmp);

	return(1);
}




void write_to_q(char *txt, struct txt_q *queue)
{
  struct txt_block *new;
  int strl;

  if (!queue) {
    logE("Output message to non-existant queue");
    return;
  }

  CREATE(new, struct txt_block, 1);
  strl = strlen(txt);
  if (strl < 0 || strl > 15000) {
    logE("strlen returned bogus length in write_to_q");
    free(new);
    return;
  }
#if 0 /* Changed for test */
  CREATE(new->text, char, strl+1);
  strcpy(new->text, txt);
#else
  
  new->text = (char *)strdup(txt);
#endif

  new->next = NULL;

  /* Q empty? */
  if (!queue->head)  {
    queue->head = queue->tail = new;
  } else	{
    queue->tail->next = new;
    queue->tail = new;
  }
}
		






struct timeval timediff(struct timeval *a, struct timeval *b)
{
	struct timeval rslt, tmp;

	tmp = *a;

	if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0)
	{
		rslt.tv_usec += 1000000;
		--(tmp.tv_sec);
	}
	if ((rslt.tv_sec = tmp.tv_sec - b->tv_sec) < 0)
	{
		rslt.tv_usec = 0;
		rslt.tv_sec =0;
	}
	return(rslt);
}






/* Empty the queues before closing connection */
void flush_queues(struct descriptor_data *d)
{
	char dummy[MAX_STRING_LENGTH];

	while (get_from_q(&d->output, dummy));
	while (get_from_q(&d->input, dummy));
}






/* ******************************************************************
*  socket handling							 *
****************************************************************** */




int init_socket(int port)
{
	int s;
	char *opt;
	char hostname[MAX_HOSTNAME+1];
	struct sockaddr_in sa;
	struct hostent *hp;
	struct linger ld;

	bzero(&sa, sizeof(struct sockaddr_in));
	gethostname(hostname, MAX_HOSTNAME);
	hp = gethostbyname(hostname);
	if (hp == NULL)	{
		perror("gethostbyname");
		assert(0);
	}
	sa.sin_family = hp->h_addrtype;
	sa.sin_port	= htons(port);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) 	{
		perror("Init-socket");
		assert(0);
 	}
	if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR,
		(char *) &opt, sizeof (opt)) < 0) 	{
		perror ("setsockopt REUSEADDR");
		exit (1);
	}

	ld.l_onoff = 1;
	ld.l_linger = 100;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ld, sizeof(ld)) < 0)	{
		perror("setsockopt LINGER");
		assert(0);
	}

	if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) < 0)	{
	    perror("bind");
	    exit(0);
	}

	listen(s, 5);
	return(s);
}





int new_connection(int s)
{
  struct sockaddr_in isa;
#ifdef sun
   struct sockaddr peer;
#endif
  int i;
  int t;
  char buf[100];
  
  i = sizeof(isa);
#if 0
  getsockname(s, &isa, &i);
#endif
  
  if ((t = accept(s, (struct sockaddr *)&isa, &i)) < 0){
    perror("Accept");
    return(-1);
  }
  nonblock(t);
#if 0  
#ifdef sun

  
    
  i = sizeof(peer);
  if (!getpeername(t, &peer, &i))	{
    if (i > 0) {
      *(peer.sa_data + 49) = '\0';
      sprintf(buf, "New connection from addr %s.", peer.sa_data);
      log(buf);
    }
  }

#endif
#endif
  return(t);
}



/* print an internet host address prettily */
static void printhost(addr, buf)
     struct in_addr	*addr;
     char	*buf;
{
  struct hostent	*h;
  char	*s;

  h = gethostbyaddr(addr, sizeof(struct in_addr),AF_INET);
  s = (h==NULL) ? NULL : h->h_name;

  if (s) {
    strcpy(buf, s);
  } else {
    strcpy(buf, (const char *)inet_ntoa(*addr));
  }
}


/* print an internet host address prettily */
static void printhostaddr(addr, buf)
     struct in_addr	*addr;
     char	*buf;
{
  struct hostent	*h;
  char	*s;

  h = gethostbyaddr(addr, sizeof(*addr),AF_INET);
  s = (h==NULL) ? "1.1.1.1" : h->h_name;

  strcpy(buf, s);

}

int new_descriptor(int s)
{

  int desc;
  struct descriptor_data *newd;
  int size;
  struct sockaddr_in sock;
  char buf[200];
  
  if ((desc = new_connection(s)) < 0)
    return (-1);
  
  
  if ((desc + 1) >= 64) {

    struct descriptor_data *d;

    write_to_descriptor(desc, "Sorry.. The game is full. Try again later\n\r");
    close(desc);
    
    for (d = descriptor_list; d; d = d->next) {
      if (!d->character)
	close_socket(d);
    }
    return(0);
  }
  else
    if (desc > maxdesc)
      maxdesc = desc;
  
  CREATE(newd, struct descriptor_data, 1);

  /* find info */
  size = sizeof(sock);
  if (getpeername(desc, (struct sockaddr *) &sock, &size) < 0) {
    perror("getpeername");
    newd->port = 0;
    *newd->host = '\0';
  }
  else {
    newd->port = sock.sin_port;
    strncpy(newd->host, inet_ntoa(sock.sin_addr), 49);
    *(newd->host + 49) = '\0';
    sprintf(buf, "New connection from addr %s: %d: %d",
            newd->host, desc, maxdesc);
    log_sev(buf,3);
  }

  /* init desc data */
  newd->descriptor = desc;
  newd->connected  = CON_NME;
  newd->close = 0;
  newd->wait = 1;
  newd->prompt_mode = 0;
  *newd->buf = '\0';
  newd->str = 0;
  newd->showstr_head = 0;
  newd->showstr_point = 0;
  *newd->last_input= '\0';
  newd->output.head = NULL;
  newd->input.head = NULL;
  newd->next = descriptor_list;
  newd->character = 0;
  newd->original = 0;
  newd->snoop.snooping = 0;
  newd->snoop.snoop_by = 0;
  
  /* prepend to list */
  
  descriptor_list = newd;
  
  SEND_TO_Q(login, newd);
  SEND_TO_Q("What is thy name? ", newd);

  return(0);
}




int process_output(struct descriptor_data *t)
{
  char i[MAX_STRING_LENGTH + MAX_STRING_LENGTH];
  
  if (!t->prompt_mode && !t->connected)
    if (write_to_descriptor(t->descriptor, "\n\r") < 0)
      return(-1);
  
  
  /* Cycle thru output queue */
  while (get_from_q(&t->output, i))	{  
    if ((t->snoop.snoop_by) && (t->snoop.snoop_by->desc)) {
      write_to_q("% ",&t->snoop.snoop_by->desc->output);
      write_to_q(i,&t->snoop.snoop_by->desc->output);
    }
    if (write_to_descriptor(t->descriptor, i))
      return(-1);
  }
  
  if (!t->connected && !(t->character && !IS_NPC(t->character) && 
	 IS_SET(t->character->specials.act, PLR_COMPACT)))
    if (write_to_descriptor(t->descriptor, "\n\r") < 0)
      return(-1);
  
  return(1);
}


int write_to_descriptor(int desc, char *txt)
{
  int sofar, thisround, total;
  
  total = strlen(txt);
  sofar = 0;
  
  do {
      thisround = write(desc, txt + sofar, total - sofar);
      if (thisround < 0)	{
	  if (errno == EWOULDBLOCK)
	    break;
	  perror("Write to socket");
	  close_socket_fd(desc);
	  return(-1);
	}
      sofar += thisround;
  } 
  while (sofar < total);
  
  return(0);
}

int write_to_descriptor_echo_on(struct descriptor_data *d)
{
  if (d->descriptor == 0) {
    /* disable for websocket connections
    SEND_TO_Q(echo_on, d);
     */
    return 0;
  }
  return write(d->descriptor, echo_on, 6);
}

int write_to_descriptor_echo_off(struct descriptor_data *d)
{
  if (d->descriptor == 0) {
    /* disable for websocket connections
    SEND_TO_Q(echo_off, d);
     */
    return 0;
  }
  return write(d->descriptor, echo_off, 4);
}

int process_input(struct descriptor_data *t)
{
  int sofar, thisround, begin, squelch, i, k, flag;
  char tmp[MAX_INPUT_LENGTH+2], buffer[MAX_INPUT_LENGTH + 60];
  
  sofar = 0;
  flag = 0;
  begin = strlen(t->buf);
  
  /* Read in some stuff */
  do  {
    if ((thisround = read(t->descriptor, t->buf + begin + sofar, 
			  MAX_STRING_LENGTH - (begin + sofar) - 1)) > 0) {
      sofar += thisround;
    } else {
      if (thisround < 0) {
	if (errno != EWOULDBLOCK) {
	  perror("Read1 - ERROR");
	  return(-1);
	} else {
	  break;
	}
      } else {
	logE("EOF encountered on socket read.");
	return(-1);
      }
    }
  } while (!ISNEWL(*(t->buf + begin + sofar - 1)));	
  
  *(t->buf + begin + sofar) = 0;
  
  /* if no newline is contained in input, return without proc'ing */
  for (i = begin; !ISNEWL(*(t->buf + i)); i++)
    if (!*(t->buf + i))
      return(0);
  
  /* input contains 1 or more newlines; process the stuff */
  for (i = 0, k = 0; *(t->buf + i);)	{
    if (!ISNEWL(*(t->buf + i)) && !(flag=(k>=(MAX_INPUT_LENGTH - 2))))
      if (*(t->buf + i) == '\b') {	 /* backspace */
	if (k) { /* more than one char ? */
	  if (*(tmp + --k) == '$')
	    k--;				
	  i++;
	} else {
	  i++;  /* no or just one char.. Skip backsp */
	}
      } else {
	if (isascii(*(t->buf + i)) && isprint(*(t->buf + i))) {
	  /* 
	    trans char, double for '$' (printf)	
	    */
	  if ((*(tmp + k) = *(t->buf + i)) == '$')
	    *(tmp + ++k) = '$';
	  k++;
	  i++;
	} else {
	  i++;
	}
      } else 	{
	*(tmp + k) = 0;
	if(*tmp == '!')
	  strcpy(tmp,t->last_input);
	else
	  strcpy(t->last_input,tmp);
	
	write_to_q(tmp, &t->input);
	
	if ((t->snoop.snoop_by) && (t->snoop.snoop_by->desc)){
	  write_to_q("% ",&t->snoop.snoop_by->desc->output);
	  write_to_q(tmp,&t->snoop.snoop_by->desc->output);
	  write_to_q("\n\r",&t->snoop.snoop_by->desc->output);
	}
	
	if (flag) {
	  sprintf(buffer, 
		  "Line too long. Truncated to:\n\r%s\n\r", tmp);
	  if (write_to_descriptor(t->descriptor, buffer) < 0)
	    return(-1);
	  
	  /* skip the rest of the line */
	  for (; !ISNEWL(*(t->buf + i)); i++);
	}
	
	/* find end of entry */
	for (; ISNEWL(*(t->buf + i)); i++);
	
	/* squelch the entry from the buffer */
	for (squelch = 0;; squelch++)
	  if ((*(t->buf + squelch) = 
	       *(t->buf + i + squelch)) == '\0')
	    break;
	k = 0;
	i = 0;
      }
  }
  return(1);
}




void close_sockets(int s)
{
  logE("Closing all sockets.");
  
  while (descriptor_list)
    close_socket(descriptor_list);
  
  close(s);
}





void close_socket(struct descriptor_data *d)
{
  struct descriptor_data *tmp;
  char buf[100];
  struct txt_block *txt, *txt2;

  void do_save(struct char_data *ch, char *argument, int cmd);
  
  if (!d) return;
  if (d->descriptor == 0) {
    d->close = 1;
    return;
  }
  
  close(d->descriptor);
  flush_queues(d);
  if (d->descriptor == maxdesc)
    --maxdesc;
  
  /* Forget snooping */
  if (d->snoop.snooping)
    d->snoop.snooping->desc->snoop.snoop_by = 0;
  
  if (d->snoop.snoop_by)		{
    send_to_char("Your victim is no longer among us.\n\r",d->snoop.snoop_by);
    d->snoop.snoop_by->desc->snoop.snooping = 0;
  }
  
  if (d->character)
    if (d->connected == CON_PLYNG) 	{
       do_save(d->character, "", 0);
      act("$n has lost $s link.", TRUE, d->character, 0, 0, TO_ROOM);
      sprintf(buf, "Closing link to: %s.", GET_NAME(d->character));
      logE(buf);
      if (IS_NPC(d->character)) { /* poly, or switched god */
	if (d->character->desc)
	  d->character->orig = d->character->desc->original;
      }
      d->character->desc = 0;
      d->character->invis_level = LOW_IMMORTAL;

      if (!IS_AFFECTED(d->character, AFF_CHARM)) {
	if (d->character->master) {
	  stop_follower(d->character);
	}
      }

    } else {
      if (GET_NAME(d->character)) {
	sprintf(buf, "Losing player: %s.", GET_NAME(d->character));
	logE(buf);
      }
      free_char(d->character);
    }
  else
    logE("Losing descriptor without char.");
  
  
  if (next_to_process == d)    	/* to avoid crashing the process loop */
    next_to_process = next_to_process->next;   
  
  if (d == descriptor_list) /* this is the head of the list */
    descriptor_list = descriptor_list->next;
  else  /* This is somewhere inside the list */    {
    /* Locate the previous element */
    for (tmp = descriptor_list; tmp && (tmp->next != d); 
	 tmp = tmp->next);
    
    if (tmp)
      tmp->next = d->next;
    else {
      /* die a slow death you motherfucking piece of shit machine */
      /* :-) */
      
    }
  }
#if 0
  if (d->showstr_head)   /* this piece of code causes core dumps on */
    free(d->showstr_head);   /* ardent titans */
#endif
  /*
    free the input and output queues.
   */

  txt = d->output.head;

  while(txt) {
    if (txt->text) {
      free(txt->text);
      txt2 = txt;
      txt = txt2->next;
      free(txt2);
    }
  }

  txt = d->input.head;

  while(txt) {
    if (txt->text) {
      free(txt->next);
      txt2 = txt;
      txt = txt->next;
      free(txt2);
    }
  }

  free(d);
}





void nonblock(int s)
{
  if (fcntl(s, F_SETFL, O_NONBLOCK) == -1)    {
    perror("Noblock");
    assert(0);
  }
}




#define COMA_SIGN \
"\n\r\
DikuMUD is currently inactive due to excessive load on the host machine.\n\r\
Please try again later.\n\r\n\
\n\r\
   Sadly,\n\r\
\n\r\
    the DikuMUD system operators\n\r\n\r"


/* sleep while the load is too high */
void coma(int s)
{
  fd_set input_set;
  static struct timeval timeout =
    {
      60, 
      0
      };
  int conn;
  sigset_t mask;
  
  int workhours(void);
  int load(void);
  
  logE("Entering comatose state.");
  
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGPIPE);
  sigaddset(&mask, SIGALRM);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGURG);
  sigaddset(&mask, SIGXCPU);
  sigaddset(&mask, SIGHUP);
  sigprocmask(SIG_BLOCK, &mask, 0);
  
  while (descriptor_list)
    close_socket(descriptor_list);
  
  FD_ZERO(&input_set);
  do {
    FD_SET(s, &input_set);
    if (select(64, &input_set, 0, 0, &timeout) < 0){
      perror("coma select");
      assert(0);
    }
    if (FD_ISSET(s, &input_set))	{
      if (load() < 6){
	logE("Leaving coma with visitor.");
	sigsetmask(0);
	return;
      }
      if ((conn = new_connection(s)) >= 0)     {
	write_to_descriptor(conn, COMA_SIGN);
	sleep(2);
	close(conn);
      }
    }			
    
    tics = 1;
    if (workhours())  {
      logE("Working hours collision during coma. Exit.");
      assert(0);
    }
  }
  while (load() >= 6);
  
  logE("Leaving coma.");
  sigprocmask(SIG_UNBLOCK, &mask, 0);
}



/* ****************************************************************
*	Public routines for system-to-player-communication	  *
**************************************************************** */



void send_to_char(char *messg, struct char_data *ch)
{
  if (ch)
     if (ch->desc && messg)
       	write_to_q(messg, &ch->desc->output);
}


void save_all()
{
  struct descriptor_data *i;
  
  for (i = descriptor_list; i; i = i->next)
    if (i->character)
      save_char(i->character,AUTO_RENT);
}

void send_to_all(char *messg)
{
  struct descriptor_data *i;
  
  if (messg)
    for (i = descriptor_list; i; i = i->next)
      if (!i->connected)
	write_to_q(messg, &i->output);
}


void send_to_outdoor(char *messg)
{
  struct descriptor_data *i;
  
  if (messg)
    for (i = descriptor_list; i; i = i->next)
      if (!i->connected)
	if (OUTSIDE(i->character) )
	  write_to_q(messg, &i->output);
}

void send_to_desert(char *messg)
{
  struct descriptor_data *i;
  struct room_data *rp;
  extern struct zone_data *zone_table;

  if (messg) {
    for (i = descriptor_list; i; i = i->next) {
      if (!i->connected) {
	if (OUTSIDE(i->character)) {
	  if ((rp = real_roomp(i->character->in_room))!=NULL) {
	    if (IS_SET(zone_table[rp->zone].reset_mode, ZONE_DESERT) ||
		rp->sector_type == SECT_DESERT) {
	      write_to_q(messg, &i->output);
	    }
	  }
	}
      }
    }
  }
}

void send_to_out_other(char *messg)
{
  struct descriptor_data *i;
  struct room_data *rp;
  extern struct zone_data *zone_table;

  if (messg) {
    for (i = descriptor_list; i; i = i->next) {
      if (!i->connected) {
	if (OUTSIDE(i->character)) {
	  if ((rp = real_roomp(i->character->in_room))!=NULL) {
	    if (!IS_SET(zone_table[rp->zone].reset_mode, ZONE_DESERT) &&
	        !IS_SET(zone_table[rp->zone].reset_mode, ZONE_ARCTIC) &&
		rp->sector_type != SECT_DESERT) {
	      write_to_q(messg, &i->output);
	    }
	  }
	}
      }
    }
  }
}


void send_to_arctic(char *messg)
{
  struct descriptor_data *i;
  struct room_data *rp;
  extern struct zone_data *zone_table;

  if (messg) {
    for (i = descriptor_list; i; i = i->next) {
      if (!i->connected) {
	if (OUTSIDE(i->character)) {
	  if ((rp = real_roomp(i->character->in_room))!=NULL) {
	    if (IS_SET(zone_table[rp->zone].reset_mode, ZONE_ARCTIC)) {
	      write_to_q(messg, &i->output);
	    }
	  }
	}
      }
    }
  }
}


void send_to_except(char *messg, struct char_data *ch)
{
  struct descriptor_data *i;
  
  if (messg)
    for (i = descriptor_list; i; i = i->next)
      if (ch->desc != i && !i->connected)
	write_to_q(messg, &i->output);
}


void send_to_zone(char *messg, struct char_data *ch)
{
  struct descriptor_data *i;
  
  if (messg)
    for (i = descriptor_list; i; i = i->next)
      if (ch->desc != i && !i->connected)
	if (real_roomp(i->character->in_room)->zone == 
	    real_roomp(ch->in_room)->zone)
	  write_to_q(messg, &i->output);
}



void send_to_room(char *messg, int room)
{
  struct char_data *i;
  
  if (messg)
    for (i = real_roomp(room)->people; i; i = i->next_in_room)
      if (i->desc)
	write_to_q(messg, &i->desc->output);
}




void send_to_room_except(char *messg, int room, struct char_data *ch)
{
  struct char_data *i;
  
  if (messg)
    for (i = real_roomp(room)->people; i; i = i->next_in_room)
      if (i != ch && i->desc)
	write_to_q(messg, &i->desc->output);
}

void send_to_room_except_two
  (char *messg, int room, struct char_data *ch1, struct char_data *ch2)
{
  struct char_data *i;
  
  if (messg)
    for (i = real_roomp(room)->people; i; i = i->next_in_room)
      if (i != ch1 && i != ch2 && i->desc)
	write_to_q(messg, &i->desc->output);
}



/* higher-level communication */


void act(char *str, int hide_invisible, struct char_data *ch,
	 struct obj_data *obj, void *vict_obj, int type)
{
  register char *strp, *point, *i;
  struct char_data *to;
  char buf[MAX_STRING_LENGTH];
  
  if (!str)
    return;
  if (!*str)
    return;
  
  if (ch->in_room <= -1)
    return;  /* can't do it. in room -1 */
  
  if (type == TO_VICT)
    to = (struct char_data *) vict_obj;
  else if (type == TO_CHAR)
    to = ch;
  else
    to = real_roomp(ch->in_room)->people;
  
  for (; to; to = to->next_in_room)	{
    if (to->desc && ((to != ch) || (type == TO_CHAR)) &&  
	(CAN_SEE(to, ch) || !hide_invisible) && AWAKE(to) &&
	!((type == TO_NOTVICT)&&(to==(struct char_data *) vict_obj))){
      for (strp = str, point = buf;;)
	if (*strp == '$') {
	  switch (*(++strp)) {
	  case 'n': i = PERS(ch, to); 
	    break;
	  case 'N': i = PERS((struct char_data *) vict_obj, to); 
	    break;
	  case 'm': i = HMHR(ch); 
	    break;
	  case 'M': i = HMHR((struct char_data *) vict_obj); 
	    break;
	  case 's': i = HSHR(ch); 
	    break;
	  case 'S': i = HSHR((struct char_data *) vict_obj); 
	    break;
	  case 'e': i = HSSH(ch); 
	    break;
	  case 'E': i = HSSH((struct char_data *) vict_obj); 
	    break;
	  case 'o': i = OBJN(obj, to); 
	    break;
	  case 'O': i = OBJN((struct obj_data *) vict_obj, to); 
	    break;
	  case 'p': i = OBJS(obj, to); 
	    break;
	  case 'P': i = OBJS((struct obj_data *) vict_obj, to); 
	    break;
	  case 'a': i = SANA(obj); 
	    break;
	  case 'A': i = SANA((struct obj_data *) vict_obj); 
	    break;
	  case 'T': i = (char *) vict_obj; 
	    break;
	  case 'F': i = fname((char *) vict_obj); 
	    break;
	  case '$': i = "$"; 
	    break;
	  default:
	    logE("Illegal $-code to act():");
	    logE(str);
	    break;
	  }
	  
	  while (*point = *(i++))
	    ++point;
	  
	  ++strp;
	  
	}	else if (!(*(point++) = *(strp++)))
	  break;
      
      *(--point) = '\n';
      *(++point) = '\r';
      *(++point) = '\0';
      
      write_to_q(CAP(buf), &to->desc->output);
    }
    if ((type == TO_VICT) || (type == TO_CHAR))
      return;
  }
}

int raw_force_all( char *to_force)
{
  struct descriptor_data *i;
  char buf[400];

  for (i = descriptor_list; i; i = i->next)
    if (!i->connected) {
      sprintf(buf, "The game has forced you to '%s'.\n\r", to_force);
      send_to_char(buf, i->character);
      command_interpreter(i->character, to_force);
    }
}


#if 0
#endif

void UpdateScreen(struct char_data *ch, int update)
{
 char buf[255];
 int size;
 
 size = ch->size;

 if(IS_SET(update, INFO_MANA)) {
    sprintf(buf, VT_CURSAVE);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 2, 7);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "          ");
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 2, 7);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "%d(%d)", GET_MANA(ch), GET_MAX_MANA(ch));
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURREST);
    write_to_descriptor(ch->desc->descriptor, buf);
  }
 
 if(IS_SET(update, INFO_MOVE)) {
    sprintf(buf, VT_CURSAVE);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 3, 58);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "          ");
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 3, 58);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "%d(%d)", GET_MOVE(ch), GET_MAX_MOVE(ch));
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURREST);
    write_to_descriptor(ch->desc->descriptor, buf);
  }
 
 if(IS_SET(update, INFO_HP)) {
    sprintf(buf, VT_CURSAVE);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 3, 13);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "          ");
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 3, 13);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "%d(%d)", GET_HIT(ch), GET_MAX_HIT(ch));
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURREST);
    write_to_descriptor(ch->desc->descriptor, buf);
  }
 
 if(IS_SET(update, INFO_GOLD)) {
    sprintf(buf, VT_CURSAVE);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 2, 47);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "                ");
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 2, 47);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "%d", GET_GOLD(ch));
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURREST);
    write_to_descriptor(ch->desc->descriptor, buf);
  }
 
 if(IS_SET(update, INFO_EXP)) {
    sprintf(buf, VT_CURSAVE);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 1, 20);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "                ");
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURSPOS, size - 1, 20);
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, "%d", GET_EXP(ch));
    write_to_descriptor(ch->desc->descriptor, buf);
    sprintf(buf, VT_CURREST);
    write_to_descriptor(ch->desc->descriptor, buf);
  }
}
 
 
void InitScreen(struct char_data *ch)
{
 char buf[255];
 int size;

 size = ch->size; 
 sprintf(buf, VT_HOMECLR);
 send_to_char(buf, ch);
 sprintf(buf, VT_MARGSET, 0, size - 5);
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 4, 1);
 send_to_char(buf, ch);
 sprintf(buf, "-===========================================================================-");
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 3, 1);
 send_to_char(buf, ch);
 sprintf(buf, "Hit Points: ");
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 3, 40);
 send_to_char(buf, ch);
 sprintf(buf, "Movement Points: ");
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 2, 1);
 send_to_char(buf, ch);
 sprintf(buf, "Mana: ");
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 2, 40);
 send_to_char(buf, ch);
 sprintf(buf, "Gold: ");
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 1, 1);
 send_to_char(buf, ch);
 sprintf(buf, "Experience Points: ");
 send_to_char(buf, ch);
 
 ch->last.mana = GET_MANA(ch);
 ch->last.mmana = GET_MAX_MANA(ch);
 ch->last.hit = GET_HIT(ch);
 ch->last.mhit = GET_MAX_HIT(ch);
 ch->last.move = GET_MOVE(ch);
 ch->last.mmove = GET_MAX_MOVE(ch);
 ch->last.exp = GET_EXP(ch);
 ch->last.gold = GET_GOLD(ch);
 
 /* Update all of the info parts */
 sprintf(buf, VT_CURSPOS, size - 3, 13);
 send_to_char(buf, ch);
 sprintf(buf, "%d(%d)", GET_HIT(ch), GET_MAX_HIT(ch));
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 3, 58);
 send_to_char(buf, ch);
 sprintf(buf, "%d(%d)", GET_MOVE(ch), GET_MAX_MOVE(ch));
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 2, 7);
 send_to_char(buf, ch);
 sprintf(buf, "%d(%d)", GET_MANA(ch), GET_MAX_MANA(ch));
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 2, 47);
 send_to_char(buf, ch);
 sprintf(buf, "%d", GET_GOLD(ch));
 send_to_char(buf, ch);
 sprintf(buf, VT_CURSPOS, size - 1, 20);
 send_to_char(buf, ch);
 sprintf(buf, "%d", GET_EXP(ch));
 send_to_char(buf, ch);

 sprintf(buf, VT_CURSPOS, 0, 0);
 send_to_char(buf, ch);

}
