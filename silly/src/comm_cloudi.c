/*-*-Mode:C;coding:utf-8;tab-width:2;c-basic-offset:2;indent-tabs-mode:()-*-
 * ex: set ft=cpp fenc=utf-8 sts=2 ts=2 sw=2 et:
 */
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <cloudi.h>
#include "structs.h"
#include "utils.h"
#include "vt100c.h"
#include "comm.h"

/* only the function declarations actually used are listed
 * instead of using the protos.h header file
 */

/* act.other.c */
void do_save(struct char_data *ch, char *argument, int cmd);
/* comm.c */
#define OPT_USEC 250000       /* time delay corresponding to 4 passes/sec */
extern struct descriptor_data *descriptor_list;
extern struct descriptor_data *next_to_process;
extern int mudshutdown;
extern int reboot;
extern int pulse;
extern int tics;
struct timeval timediff(struct timeval *a, struct timeval *b);
int get_from_q(struct txt_q *queue, char *dest);
void act(char *str, int hide_invisible, struct char_data *ch,
         struct obj_data *obj, void *vict_obj, int type);
void flush_queues(struct descriptor_data *d);
void write_to_q(char *txt, struct txt_q *queue);
/* constants.c */
extern char *sector_types[];
extern const char *room_bits[];
/* create.c */
void RoomEdit(struct char_data *ch, char *arg);
/* db.c */
extern char login[];
extern struct time_info_data time_info;
void boot_db();
void zone_update();
void update_time();
void free_char(struct char_data *ch);
struct room_data *real_roomp(int virtual);
/* fight.c */
void perform_violence(int pulse);
/* handler.c */
void char_from_room(struct char_data *ch);
void char_to_room(struct char_data *ch, int room);
/* interpreter.c */
void command_interpreter(struct char_data *ch, char *argument);
void nanny(struct descriptor_data *d, char *arg);
/* limits.c */
int mana_limit(struct char_data *ch);
int hit_limit(struct char_data *ch);
int move_limit(struct char_data *ch);
/* modify.c */
void string_add(struct descriptor_data *d, char *str);
void show_string(struct descriptor_data *d, char *input);
/* multiclass.c */
int GetMaxLevel(struct char_data *ch);
/* signals.c */
void signal_setup();
void signals_block();
void signals_unblock();
/* spell_parser.c */
void affect_update(int pulse);
void stop_follower(struct char_data *ch);
/* utility.c */
void logE(char *str);
void log_sev(char *str, int sev);
void sprintbit(unsigned long vektor, char *names[], char *result);
void sprinttype(int type, char *names[], char *result);
void RiverPulseStuff(int pulse);
void TeleportPulseStuff(int pulse);
/* weather.c */
void weather_and_time(int mode);

/* internal */
static char * response_with_cloudi = 0;
static size_t response_size_with_cloudi = 0;
static void game_loop(cloudi_instance_t * const api);
static struct descriptor_data * new_descriptor(char const * const
                                                   source_address,
                                               int const source_port);
static void close_socket(struct descriptor_data *d);
static int process_input(struct descriptor_data *t,
                         void const * const request,
                         uint32_t const request_size);
static void process_output(struct descriptor_data * t,
                           int newlines,
                           char ** response,
                           size_t * response_size_allocated,
                           size_t * response_size);
static void process_commands(struct descriptor_data *point);
static void send_to_char(char *messg, struct char_data *ch);
static void UpdateScreen(struct char_data *ch, int update);

static void source_info_from_cloudi(char const ** info,
                                    char const ** source_address,
                                    int * source_port)
{
  char const * forwarded_address = 0;
  char const * forwarded_port = 0;
  *source_address = 0;
  *source_port = 0;
  int found = 0;
  size_t i;
  for (i = 0; info[i] && found < 2; i += 2)
  {
    /* cloudi_service_tcp or cloudi_service_http_cowboy
     * request_info key names
     */
    if (strncmp(info[i], "source", 6) == 0 &&
        (info[i][6] == '-' || info[i][6] == '_'))
    {
      if (strcmp(&(info[i][7]), "address") == 0)
      {
        *source_address = info[i + 1];
      }
      else if (strcmp(&(info[i][7]), "port") == 0)
      {
        *source_port = atoi(info[i + 1]);
      }
    }
    /* cloudi_service_http_cowboy proxied request_info key names */
    if (strcmp(info[i], "x-forwarded-for") == 0) {
      forwarded_address = info[i + 1];
      ++found;
    }
    else if (strcmp(info[i], "x-forwarded-port") == 0) {
      forwarded_port = info[i + 1];
      ++found;
    }
  }
  if (forwarded_address) {
    *source_address = forwarded_address;
  }
  if (forwarded_port) {
    *source_port = atoi(forwarded_port);
  }
}

struct descriptor_data * descriptor_data_from_cloudi(char const * const
                                                       source_address,
                                                     int const source_port)
{
  struct descriptor_data *point, *next_point;
  for (point = descriptor_list; point; point = next_point) {
    next_point = point->next;
    if (point->port == source_port &&
        strcmp(point->host, source_address) == 0) {
      return point;
    }
  }
  return 0;
}

static void new_descriptor_with_cloudi(int const command,
                                       char const * const name,
                                       char const * const pattern,
                                       void const * const request_info,
                                       uint32_t const request_info_size,
                                       void const * const request,
                                       uint32_t const request_size,
                                       uint32_t timeout,
                                       int8_t priority,
                                       char const * const trans_id,
                                       char const * const pid,
                                       uint32_t const pid_size,
                                       void * state,
                                       cloudi_instance_t * api)
{
  char const ** info = cloudi_info_key_value_parse(request_info,
                                                   request_info_size);
  char const * source_address;
  int source_port;
  source_info_from_cloudi(info, &source_address, &source_port);
  cloudi_info_key_value_parse_destroy(info);
  fprintf(stderr, "connection open: %s:%d\n", source_address, source_port);

  /* New connection */
  struct descriptor_data *point = new_descriptor(source_address, source_port);
  size_t response_size = 0;
  if (point->output.head) {
    process_output(point, 1,
                   &response_with_cloudi, &response_size_with_cloudi,
                   &response_size);
  }

  cloudi_return(api, command, name, pattern, "", 0,
                response_with_cloudi, response_size + 1,
                timeout, trans_id, pid, pid_size);
}

static void close_socket_with_cloudi(int const command,
                                     char const * const name,
                                     char const * const pattern,
                                     void const * const request_info,
                                     uint32_t const request_info_size,
                                     void const * const request,
                                     uint32_t const request_size,
                                     uint32_t timeout,
                                     int8_t priority,
                                     char const * const trans_id,
                                     char const * const pid,
                                     uint32_t const pid_size,
                                     void * state,
                                     cloudi_instance_t * api)
{
  char const ** info = cloudi_info_key_value_parse(request_info,
                                                   request_info_size);
  char const * source_address;
  int source_port;
  source_info_from_cloudi(info, &source_address, &source_port);
  cloudi_info_key_value_parse_destroy(info);
  fprintf(stderr, "connection close: %s:%d\n", source_address, source_port);

  struct descriptor_data *point =
    descriptor_data_from_cloudi(source_address, source_port);
  close_socket(point);

  cloudi_return(api, command, name, pattern, "", 0, "", 0,
                timeout, trans_id, pid, pid_size);
}

static void game_loop_with_cloudi(int const command,
                                  char const * const name,
                                  char const * const pattern,
                                  void const * const request_info,
                                  uint32_t const request_info_size,
                                  void const * const request,
                                  uint32_t const request_size,
                                  uint32_t timeout,
                                  int8_t priority,
                                  char const * const trans_id,
                                  char const * const pid,
                                  uint32_t const pid_size,
                                  void * state,
                                  cloudi_instance_t * api)
{
  char const ** info = cloudi_info_key_value_parse(request_info,
                                                   request_info_size);
  char const * source_address;
  int source_port;
  source_info_from_cloudi(info, &source_address, &source_port);
  cloudi_info_key_value_parse_destroy(info);

  struct descriptor_data *point =
    descriptor_data_from_cloudi(source_address, source_port);
  char const * response_info = "";
  size_t response_info_size = 0;
  char const * response_info_close = "connection\0close\0";
  size_t response_info_size_close = 17;
  size_t response_size = 0;

  if (! point) {
    logE("unknown connection closed.");
    cloudi_return(api, command, name, pattern,
                  response_info_close, response_info_size_close, "", 0,
                  timeout, trans_id, pid, pid_size);
  }

  if (request_size == 0) {
    if (point->output.head) {
      process_output(point, 0,
                     &response_with_cloudi, &response_size_with_cloudi,
                     &response_size);
    }
    cloudi_return(api, command, name, pattern,
                  "", 0, response_with_cloudi, response_size + 1,
                  timeout, trans_id, pid, pid_size);
  }

  signals_block();
  process_input(point, request, request_size);

  /* process_commands; */
  process_commands(point);

  if (point->output.head) {
    process_output(point, 1,
                   &response_with_cloudi, &response_size_with_cloudi,
                   &response_size);
    point->prompt_mode = 1;
  }

  /* give the people some prompts  */
  if (point->prompt_mode) {
    if (point->str) {
      SEND_TO_Q("-> ", point);
    }
    else if (!point->connected) {
      if (point->showstr_point) {
        SEND_TO_Q("[Return to continue/Q to quit]", point);
      }
      else {
        struct char_data *ch = point->character;
        char promptbuf[80];
        if (point->character->term == VT100) {
          int update = 0;

          if (GET_MOVE(ch) != ch->last.move) {
            SET_BIT(update, INFO_MOVE);
            ch->last.move = GET_MOVE(ch);
          }
          if (GET_MAX_MOVE(ch) != ch->last.mmove) {
            SET_BIT(update, INFO_MOVE);
            ch->last.mmove = GET_MAX_MOVE(ch);
          }
          if (GET_HIT(ch) != ch->last.hit) {
            SET_BIT(update, INFO_HP);
            ch->last.hit = GET_HIT(ch);
          }
          if (GET_MAX_HIT(ch) != ch->last.mhit) {
            SET_BIT(update, INFO_HP);
            ch->last.mhit = GET_MAX_HIT(ch);
          }
          if (GET_MANA(ch) != ch->last.mana) {
            SET_BIT(update, INFO_MANA);
            ch->last.mana = GET_MANA(ch);
          }
          if (GET_MAX_MANA(ch) != ch->last.mmana) {
            SET_BIT(update, INFO_MANA);
            ch->last.mmana = GET_MAX_MANA(ch);
          }
          if (GET_GOLD(ch) != ch->last.gold) {
            SET_BIT(update, INFO_GOLD);
            ch->last.gold = GET_GOLD(ch);
          }
          if (GET_EXP(ch) != ch->last.exp) {
            SET_BIT(update, INFO_EXP);
            ch->last.exp = GET_EXP(ch);
          }
          if (update) {
            UpdateScreen(ch, update);
          }
        }
        else if (point->character->term == 0) {
          if (IS_IMMORTAL(point->character)) {
            struct room_data *rm = real_roomp(point->character->in_room);
            if (!rm) {
              char_to_room(point->character, 0);
              rm = real_roomp(point->character->in_room);
            }
            if (IS_SET(ch->specials.prompt, PROMPT_R)) {
              sprintf(promptbuf,"<Rm: %d ", rm->number);
              SEND_TO_Q(promptbuf, point);
            }
            if (IS_SET(ch->specials.prompt, PROMPT_S)) {
              sprinttype(rm->sector_type, sector_types, promptbuf);
              SEND_TO_Q(promptbuf, point);
              SEND_TO_Q(" ", point);
            }
            if (IS_SET(ch->specials.prompt, PROMPT_F)) {
              sprintbit((unsigned long)rm->room_flags,(char **)room_bits,promptbuf);
              SEND_TO_Q(promptbuf, point);
            }
            sprintf(promptbuf, "> ");
            SEND_TO_Q(promptbuf, point);
          } else {
            if (IS_SET(ch->specials.prompt, PROMPT_H)) {
              sprintf(promptbuf,"H:%d ", point->character->points.hit);
              SEND_TO_Q(promptbuf, point);
            }
            if (IS_SET(ch->specials.prompt, PROMPT_M)) {
              sprintf(promptbuf,"M:%d ", point->character->points.mana);
              SEND_TO_Q(promptbuf, point);
            }
            if (IS_SET(ch->specials.prompt, PROMPT_V)) {
              sprintf(promptbuf,"V:%d ", point->character->points.move);
              SEND_TO_Q(promptbuf, point);
            }
            sprintf(promptbuf, "> ");
            SEND_TO_Q(promptbuf, point);
          }
        } else {
          sprintf(promptbuf, "> ");
          SEND_TO_Q(promptbuf, point);
        }
      }
    }
    point->prompt_mode = 0;
  }
  if (point->output.head) {
    process_output(point, 0,
                   &response_with_cloudi, &response_size_with_cloudi,
                   &response_size);
  }

  if (point->close) {
    response_info = response_info_close;
    response_info_size = response_info_size_close;
    close_socket(point);
  }
  signals_unblock();
  cloudi_return(api, command, name, pattern,
                response_info, response_info_size,
                response_with_cloudi, response_size + 1,
                timeout, trans_id, pid, pid_size);
}

/* based on comm.c */
void run_the_game_with_cloudi()
{
  int thread_count;
  int result = cloudi_initialize_thread_count(&thread_count);
  assert(result == cloudi_success);
  assert(thread_count == 1);
  int const thread_index = 0;
  cloudi_instance_t api;
  result = cloudi_initialize(&api, thread_index, 0);
  assert(result == cloudi_success);
  /* Respond to whatever might be happening */
  result = cloudi_subscribe(&api, "connect", &new_descriptor_with_cloudi);
  assert(result == cloudi_success);
  result = cloudi_subscribe(&api, "disconnect", &close_socket_with_cloudi);
  assert(result == cloudi_success);
  /* cloudi_service_tcp
   * (incoming Telnet MUD commands)
   */
  result = cloudi_subscribe(&api, "game_loop", &game_loop_with_cloudi);
  assert(result == cloudi_success);
  /* cloudi_service_http_cowboy uses the HTTP method as a service name
   * suffix on its service requests, by default
   * (so incoming WebSocket MUD commands)
   */
  result = cloudi_subscribe(&api, "game_loop/get", &game_loop_with_cloudi);
  assert(result == cloudi_success);

  descriptor_list = 0;

  logE("Signal trapping.");
  signal_setup();

  boot_db();

  logE("Entering CloudI game loop.");

  game_loop(&api);

  if (reboot) {
    logE("Rebooting.");
  }

  logE("Normal termination of game.");

  cloudi_destroy(&api);
  free(response_with_cloudi);
}

/* based on comm.c */
static void game_loop(cloudi_instance_t * const api)
{
  struct descriptor_data *point, *next_point;
  struct timeval opt_time, last_time, now, timespent, timeout;
  int result;
  int poll_timeout; /* milliseconds */

  opt_time.tv_usec = OPT_USEC;  /* Init time values */
  opt_time.tv_sec = 0;
  gettimeofday(&last_time, (struct timezone *) 0);

  while (! mudshutdown) {
    /* Check what's happening out there */

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
    poll_timeout = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;
    if (timeout.tv_usec % 1000 >= 500)
      ++poll_timeout;

    result = cloudi_poll(api, poll_timeout);
    if (result != cloudi_timeout) {
      fprintf(stderr, "cloudi_poll error %d\n", result);
      mudshutdown = 1;
      break;
    }

    for (point = descriptor_list; point; point = next_to_process) {
      next_to_process = point->next;
      if (point->wait > 0) {
        --(point->wait);
      }
      process_commands(point);
      if (point->close) {
        close_socket(point);
      }
    }

    /* handle heartbeat stuff */
    /* Note: pulse now changes every 1/4 sec (OPT_USEC) */
    pulse++;
    signals_block();
    if (!(pulse % PULSE_ZONE))  {
      zone_update();
    }
    if (!(pulse % PULSE_RIVER)) {
      RiverPulseStuff(pulse);
    }
    if (!(pulse % PULSE_TELEPORT)) {
      TeleportPulseStuff(pulse);
    }
    if (!(pulse % PULSE_VIOLENCE))
      perform_violence(pulse);
    if (!(pulse % (SECS_PER_MUD_HOUR*4))) {
      /* every mud hour, execute this */
      weather_and_time(1);
      affect_update(pulse); /* things have been sped up by combining */
      if (time_info.hours == 1) {
        update_time();
      }
    }
    signals_unblock();
    if (pulse >= 2400) {
      pulse = 0;
      /* check_reboot(); not using reboot file */
    }
    /* currently, nothing is doing checkpointing when tics == 0 */
    tics++; /* tics since last checkpoint signal */
  }
}

/* based on comm.c */
static struct descriptor_data * new_descriptor(char const * const
                                                   source_address,
                                               int const source_port)
{
  struct descriptor_data *newd;
  char buf[200];

  CREATE(newd, struct descriptor_data, 1);

  strcpy(newd->host, source_address);
  newd->port = source_port;
  sprintf(buf, "New connection from addr %s:%d", newd->host, newd->port);
  log_sev(buf, 3);

  /* init desc data */
  newd->descriptor = 0;
  newd->connected  = CON_NME;
  newd->close = 0;
  newd->wait = 0;
  newd->prompt_mode = 0;
  *newd->buf = '\0';
  newd->str = 0;
  newd->showstr_head = 0;
  newd->showstr_point = 0;
  *newd->last_input= '\0';
  newd->output.head = 0;
  newd->input.head = 0;
  newd->next = descriptor_list;
  newd->character = 0;
  newd->original = 0;
  newd->snoop.snooping = 0;
  newd->snoop.snoop_by = 0;

  SEND_TO_Q(login, newd);
  SEND_TO_Q("What is thy name? ", newd);

  /* prepend to list */
  descriptor_list = newd;

  return newd;
}

/* based on comm.c */
static void close_socket(struct descriptor_data *d)
{
  struct descriptor_data *tmp;
  char buf[100];
  struct txt_block *txt, *txt2;

  if (! d)
    return;

  flush_queues(d);

  /* Forget snooping */
  if (d->snoop.snooping)
    d->snoop.snooping->desc->snoop.snoop_by = 0;

  if (d->snoop.snoop_by) {
    send_to_char("Your victim is no longer among us.\n\r",d->snoop.snoop_by);
    d->snoop.snoop_by->desc->snoop.snooping = 0;
  }

  if (d->character)
    if (d->connected == CON_PLYNG) {
      do_save(d->character, "", 0);
      act("$n has lost $s link.", TRUE, d->character, 0, 0, TO_ROOM);
      /* a reconnect only occurs with an entry in the character_list
       * but currently, characters are not being found in that list
       * (see interpreter.c:1278, case CON_PWDNRM in nanny())
       * link-death is not occurring during normal idling due to the
       * WebSocket client sending "\r\n" on a timer
       * (see limits.c:803, check_idling() for normal link-death handling)
       */
      /* at least make sure the character is removed from the room
       * when a user disconnects without quitting
       */
      if (d->character->in_room != NOWHERE) {
        char_from_room(d->character);
      }
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

  if (next_to_process == d) /* to avoid crashing the process loop */
    next_to_process = next_to_process->next;

  if (d == descriptor_list) /* this is the head of the list */
    descriptor_list = descriptor_list->next;
  else { /* This is somewhere inside the list */
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

  while (txt) {
    if (txt->text) {
      free(txt->text);
      txt2 = txt;
      txt = txt2->next;
      free(txt2);
    }
  }

  txt = d->input.head;

  while (txt) {
    if (txt->text) {
      free(txt->next);
      txt2 = txt;
      txt = txt->next;
      free(txt2);
    }
  }

  free(d);
}

/* based on comm.c */
static int process_input(struct descriptor_data *t,
                         void const * const request,
                         uint32_t const request_size)
{
  int sofar, begin, squelch, i, k, flag;
  char tmp[MAX_INPUT_LENGTH+2], buffer[MAX_INPUT_LENGTH + 60];

  sofar = 0;
  flag = 0;
  begin = strlen(t->buf);

  /* Read in some stuff */
  if (request_size < MAX_STRING_LENGTH - begin - 1)
    sofar = request_size;
  else
    sofar = MAX_STRING_LENGTH - begin - 1;
  memcpy(t->buf + begin, request, sofar);
  *(t->buf + begin + sofar) = 0;

  /* if no newline is contained in input, return without proc'ing */
  for (i = begin; !ISNEWL(*(t->buf + i)); i++)
    if (!*(t->buf + i))
      return(0);

  /* input contains 1 or more newlines; process the stuff */
  for (i = 0, k = 0; *(t->buf + i);)    {
    if (!ISNEWL(*(t->buf + i)) && !(flag=(k>=(MAX_INPUT_LENGTH - 2))))
      if (*(t->buf + i) == '\b') {       /* backspace */
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
      } else    {
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
          SEND_TO_Q(buffer, t);

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

/* based on comm.c */
static void process_output(struct descriptor_data * t,
                           int newlines,
                           char ** response,
                           size_t * response_size_allocated,
                           size_t * response_size)
{
  if (newlines) {
    if (!t->prompt_mode && !t->connected)
      SEND_TO_Q("\n\r", t);
  }

  /* Cycle thru output queue */
  size_t i = *response_size;
  if (*response_size_allocated == 0) {
    *response_size_allocated = MAX_STRING_LENGTH * 2;
    *response = (char *) malloc(*response_size_allocated);
  }
  while (get_from_q(&t->output, &((*response)[i])))     {
    if ((t->snoop.snoop_by) && (t->snoop.snoop_by->desc)) {
      write_to_q("% ",&t->snoop.snoop_by->desc->output);
      write_to_q(&((*response)[i]),&t->snoop.snoop_by->desc->output);
    }
    i += strlen(&((*response)[i]));
    if ((*response_size_allocated - i) < MAX_STRING_LENGTH) {
      *response_size_allocated += MAX_STRING_LENGTH;
      *response = (char *) realloc(*response, *response_size_allocated);
    }
  }
  *response_size = i;

  if (newlines) {
    if (!t->connected && !(t->character && !IS_NPC(t->character) &&
        IS_SET(t->character->specials.act, PLR_COMPACT)))
      SEND_TO_Q("\n\r", t);
  }
}

/* based on comm.c game_loop */
static void process_commands(struct descriptor_data *point)
{
  static char comm[MAX_STRING_LENGTH];
  if ((point->wait <= 0) && get_from_q(&point->input, comm)) {
    if (point->character && point->connected == CON_PLYNG &&
        point->character->specials.was_in_room != NOWHERE) {
      point->character->specials.was_in_room = NOWHERE;
      act("$n has returned.", TRUE, point->character, 0, 0, TO_ROOM);
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
    else if (point->connected == CON_EDITING)
      RoomEdit(point->character, comm);
    else
      nanny(point, comm);
  }
}

/* based on comm.c */
static void send_to_char(char *messg, struct char_data *ch)
{
  if (ch)
     if (ch->desc && messg)
        write_to_q(messg, &ch->desc->output);
}

/* based on comm.c */
static void UpdateScreen(struct char_data *ch, int update)
{
  char buf[255];
  int size = ch->size;
  if (IS_SET(update, INFO_MANA)) {
    sprintf(buf, VT_CURSAVE);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 2, 7);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "          ");
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 2, 7);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "%d(%d)", GET_MANA(ch), GET_MAX_MANA(ch));
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURREST);
    SEND_TO_Q(buf, ch->desc);
  }
  if (IS_SET(update, INFO_MOVE)) {
    sprintf(buf, VT_CURSAVE);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 3, 58);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "          ");
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 3, 58);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "%d(%d)", GET_MOVE(ch), GET_MAX_MOVE(ch));
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURREST);
    SEND_TO_Q(buf, ch->desc);
  }
  if (IS_SET(update, INFO_HP)) {
    sprintf(buf, VT_CURSAVE);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 3, 13);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "          ");
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 3, 13);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "%d(%d)", GET_HIT(ch), GET_MAX_HIT(ch));
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURREST);
    SEND_TO_Q(buf, ch->desc);
  }
  if (IS_SET(update, INFO_GOLD)) {
    sprintf(buf, VT_CURSAVE);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 2, 47);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "                ");
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 2, 47);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "%d", GET_GOLD(ch));
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURREST);
    SEND_TO_Q(buf, ch->desc);
  }
  if (IS_SET(update, INFO_EXP)) {
    sprintf(buf, VT_CURSAVE);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 1, 20);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "                ");
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURSPOS, size - 1, 20);
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, "%d", GET_EXP(ch));
    SEND_TO_Q(buf, ch->desc);
    sprintf(buf, VT_CURREST);
    SEND_TO_Q(buf, ch->desc);
  }
}

