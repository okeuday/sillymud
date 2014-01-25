/*-*-Mode:C;coding:utf-8;tab-width:2;c-basic-offset:2;indent-tabs-mode:()-*-
 * ex: set ft=cpp fenc=utf-8 sts=2 ts=2 sw=2 et:

  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "protos.h"

static sigset_t signal_mask;

void checkpointing(int);
void shutdown_request(int);
void logsig(int);
void hupsig(int);

int raw_force_all( char *to_force);

void signal_setup()
{
  sigemptyset(&signal_mask);
  sigaddset(&signal_mask, SIGUSR1);
  sigaddset(&signal_mask, SIGUSR2);
  sigaddset(&signal_mask, SIGINT);
  sigaddset(&signal_mask, SIGPIPE);
  sigaddset(&signal_mask, SIGALRM);
  sigaddset(&signal_mask, SIGTERM);
  sigaddset(&signal_mask, SIGURG);
  sigaddset(&signal_mask, SIGXCPU);
  sigaddset(&signal_mask, SIGHUP);

  struct sigaction shutdown_signal;
  memset(&shutdown_signal, 0, sizeof(struct sigaction));
  sigemptyset(&shutdown_signal.sa_mask);
  sigaddset(&shutdown_signal.sa_mask, SIGHUP);
  sigaddset(&shutdown_signal.sa_mask, SIGINT);
  sigaddset(&shutdown_signal.sa_mask, SIGTERM);
  sigaddset(&shutdown_signal.sa_mask, SIGALRM);
  sigaddset(&shutdown_signal.sa_mask, SIGVTALRM);
  shutdown_signal.sa_flags = 0;
  shutdown_signal.sa_handler = shutdown_request;
  if (sigaction(SIGUSR2, &shutdown_signal, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  /* just to be on the safe side: */

  struct sigaction ignore_signal;
  memset(&ignore_signal, 0, sizeof(struct sigaction));
  sigemptyset(&ignore_signal.sa_mask);
  ignore_signal.sa_flags = 0;
  ignore_signal.sa_handler = SIG_IGN;
  if (sigaction(SIGPIPE, &ignore_signal, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  int const hupsig_array[] = {SIGHUP, SIGINT, SIGTERM};
  int i;
  for (i = 0; i < sizeof(hupsig_array) / sizeof(int); ++i) {
    struct sigaction hupsig_signal;
    memset(&hupsig_signal, 0, sizeof(struct sigaction));
    sigemptyset(&hupsig_signal.sa_mask);
    sigaddset(&hupsig_signal.sa_mask, SIGUSR2);
    if (hupsig_array[i] == SIGHUP) {
      sigaddset(&hupsig_signal.sa_mask, SIGINT);
      sigaddset(&hupsig_signal.sa_mask, SIGTERM);
    }
    else if (hupsig_array[i] == SIGINT) {
      sigaddset(&hupsig_signal.sa_mask, SIGHUP);
      sigaddset(&hupsig_signal.sa_mask, SIGTERM);
    }
    else if (hupsig_array[i] == SIGTERM) {
      sigaddset(&hupsig_signal.sa_mask, SIGHUP);
      sigaddset(&hupsig_signal.sa_mask, SIGINT);
    }
    sigaddset(&hupsig_signal.sa_mask, SIGALRM);
    sigaddset(&hupsig_signal.sa_mask, SIGVTALRM);
    hupsig_signal.sa_flags = 0;
    hupsig_signal.sa_handler = hupsig;
    if (sigaction(hupsig_array[i], &hupsig_signal, NULL) == -1) {
      perror("sigaction");
      exit(1);
    }
  }

  struct sigaction log_signal;
  memset(&log_signal, 0, sizeof(struct sigaction));
  sigemptyset(&log_signal.sa_mask);
  sigaddset(&log_signal.sa_mask, SIGUSR2);
  sigaddset(&log_signal.sa_mask, SIGHUP);
  sigaddset(&log_signal.sa_mask, SIGINT);
  sigaddset(&log_signal.sa_mask, SIGTERM);
  sigaddset(&log_signal.sa_mask, SIGVTALRM);
  log_signal.sa_flags = 0;
  log_signal.sa_handler = logsig;
  if (sigaction(SIGALRM, &log_signal, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  /* this is not deadlock-protection but rather
   * protection from really slow filesystems
   */
  /* set up the deadlock-protection */
  struct sigaction checkpointing_signal;
  memset(&checkpointing_signal, 0, sizeof(struct sigaction));
  sigemptyset(&checkpointing_signal.sa_mask);
  sigaddset(&checkpointing_signal.sa_mask, SIGUSR2);
  sigaddset(&checkpointing_signal.sa_mask, SIGHUP);
  sigaddset(&checkpointing_signal.sa_mask, SIGINT);
  sigaddset(&checkpointing_signal.sa_mask, SIGTERM);
  sigaddset(&checkpointing_signal.sa_mask, SIGALRM);
  checkpointing_signal.sa_flags = 0;
  checkpointing_signal.sa_handler = checkpointing;
  if (sigaction(SIGVTALRM, &checkpointing_signal, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  struct itimerval itime;
  struct timeval interval;
  interval.tv_sec = 900;    /* 15 minutes */
  interval.tv_usec = 0;
  itime.it_interval = interval;
  itime.it_value = interval;
  setitimer(ITIMER_VIRTUAL, &itime, 0);
}

void signals_block()
{
  sigprocmask(SIG_BLOCK, &signal_mask, 0);
}

void signals_unblock()
{
  sigprocmask(SIG_UNBLOCK, &signal_mask, 0);
}

void checkpointing(int signum)
{
  extern int tics;
	
  if (!tics) {
    logE("CHECKPOINT shutdown: tics not updated");
    abort();
  }
  else {
    tics = 0;
  }
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
