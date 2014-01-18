/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "protos.h"

void checkpointing(int);
void shutdown_request(int);
void logsig(int);
void hupsig(int);

int raw_force_all( char *to_force);

void signal_setup()
{
	struct itimerval itime;
	struct timeval interval;

	signal(SIGUSR2, shutdown_request);

	/* just to be on the safe side: */

	signal(SIGHUP, hupsig);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, hupsig);
	signal(SIGALRM, logsig);
	signal(SIGTERM, hupsig);

	/* set up the deadlock-protection */

	interval.tv_sec = 900;    /* 15 minutes */
	interval.tv_usec = 0;
	itime.it_interval = interval;
	itime.it_value = interval;
	setitimer(ITIMER_VIRTUAL, &itime, 0);
	signal(SIGVTALRM, checkpointing);
}



void checkpointing(int signum)
{
  extern int tics;
	
  if (!tics)
    {
      logE("CHECKPOINT shutdown: tics not updated");
      abort();
    }
  else
    tics = 0;
}




void shutdown_request(int signum)
{
	extern int mudshutdown;

	logE("Received USR2 - shutdown request");
	mudshutdown = 1;
}


/* kick out players etc */
void hupsig(int signum)
{
  int i;
  extern int mudshutdown, reboot;

  logE("Received SIGHUP, SIGINT, or SIGTERM. Shutting down");

  raw_force_all("return");
  raw_force_all("save");
  for (i=0;i<30;i++) {
    SaveTheWorld();
  }
  mudshutdown = reboot = 1;
}

void logsig(int signum)
{
	logE("Signal received. Ignoring.");
}
