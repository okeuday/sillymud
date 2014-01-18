/* Prototypes */
#include "structs.h"
#include "area.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "hash.h"
#include "heap.h"
#include "interpreter.h"
#include "limits.h"
#include "poly.h"
#include "race.h"
#include "script.h"
#include "spells.h"
#include "trap.h"
#include "utils.h"
#include "vt100c.h"
#include "wizlist.h"
#include "parser.h"

char *strdup(char *source);

/* From Heap.c */
 
void SmartStrCpy(char *s1, const char *s2);
void StringHeap(char *string, struct StrHeap *Heap);
struct StrHeap *InitHeap();
void DisplayStringHeap(struct StrHeap *Heap, struct char_data *ch, int type, int d);
 
 
/* From Opinion.c */
 
int FreeHates( struct char_data *ch);
int FreeFears( struct char_data *ch);
int RemHated( struct char_data *ch, struct char_data *pud);
int AddHated( struct char_data *ch, struct char_data *pud);
int AddHatred( struct char_data *ch, int parm_type, int parm);
int RemHatred( struct char_data *ch, unsigned short bitv);
int Hates( struct char_data *ch, struct char_data *v);
int Fears( struct char_data *ch, struct char_data *v);
int RemFeared( struct char_data *ch, struct char_data *pud);
int AddFeared( struct char_data *ch, struct char_data *pud);
int AddFears( struct char_data *ch, int parm_type, int parm);
struct char_data *FindAHatee( struct char_data *ch);
struct char_data *FindAFearee( struct char_data *ch);
void ZeroHatred(struct char_data *ch, struct char_data *v);
void ZeroFeared(struct char_data *ch, struct char_data *v);
void DeleteFears(struct char_data *ch);
void KillTheOrcs(struct char_data *ch);
 
/* From Sound.c */
 
int RecGetObjRoom(struct obj_data *obj);
void MakeNoise(int room, char *local_snd, char *distant_snd);
int MakeSound(int pulse);
 
 
/* From Trap.c */
 
void do_settrap( struct char_data *ch, char *arg, int cmd);
int CheckForMoveTrap(struct char_data *ch, int dir);
int CheckForInsideTrap(struct char_data *ch, struct obj_data *i);
int CheckForAnyTrap(struct char_data *ch, struct obj_data *i);
int CheckForGetTrap(struct char_data *ch, struct obj_data *i);
int TriggerTrap( struct char_data *ch, struct obj_data *i);
void FindTrapDamage( struct char_data *v, struct obj_data *i);
void TrapDamage(struct char_data *v, int damtype, int amnt, struct obj_data *t);
void TrapDam(struct char_data *v, int damtype, int amnt, struct obj_data *t);
void TrapTeleport(struct char_data *v);
void TrapSleep(struct char_data *v);
void InformMess( struct char_data *v);
 
 
/* From act.comm.c and act.move.c*/

void UpdateScreen(struct char_data *ch, int update);
void InitScreen(struct char_data *ch); 
void do_say(struct char_data *ch, char *argument, int cmd);
void do_shout(struct char_data *ch, char *argument, int cmd);
void do_commune(struct char_data *ch, char *argument, int cmd);
void do_tell(struct char_data *ch, char *argument, int cmd);
void do_whisper(struct char_data *ch, char *argument, int cmd);
void do_ask(struct char_data *ch, char *argument, int cmd);
void do_write(struct char_data *ch, char *argument, int cmd);
char *RandomWord();
void do_sign(struct char_data *ch, char *argument, int cmd);
void do_move(struct char_data *ch, char *argument, int cmd);
void do_enter(struct char_data *ch, char *argument, int cmd);
void do_rest(struct char_data *ch, char *argument, int cmd);
void do_stand(struct char_data *ch, char *argument, int cmd);
void do_sit(struct char_data *ch, char *argument, int cmd);
void do_lock(struct char_data *ch, char *argument, int cmd);
void do_unlock(struct char_data *ch, char *argument, int cmd);
void do_pick(struct char_data *ch, char *argument, int cmd);
void do_sleep(struct char_data *ch, char *argument, int cmd);
void do_wake(struct char_data *ch, char *argument, int cmd);
void do_trans(struct char_data *ch, char *argument, int cmd);
void do_follow(struct char_data *ch, char *argument, int cmd);
void do_open(struct char_data *ch, char *argument, int cmd);
void do_close(struct char_data *ch, char *argument, int cmd);
void do_enter(struct char_data *ch, char *argument, int cmd);
void do_leave(struct char_data *ch, char *argument, int cmd);
void bird_garble(char *buf, char *buf2, struct char_data *ch);
void rat_garble(char *buf, char *buf2, struct char_data *ch);
void half_orc_garble(char *buf, char *buf2, struct char_data *ch);
void ogre_garble(char *buf, char *buf2, struct char_data *ch);
void do_fly(struct char_data *ch, char *argument, int cmd);
void do_walk(struct char_data *ch, char *argument, int cmd);
 
/* From act.info.c */

void do_resize(struct char_data *ch, char *arg, int cmd);
void ScreenOff(struct char_data *ch);
void do_display(struct char_data *ch, char *arg, int cmd);
int singular( struct obj_data *o);
void argument_split_2(char *argument, char *first_arg, char *second_arg);
struct obj_data *get_object_in_equip_vis(struct char_data *ch,
       		 char *arg, struct obj_data *equipment[], int *j);
char *find_ex_description(char *word, struct extra_descr_data *list);
void show_obj_to_char(struct obj_data *object, struct char_data *ch, int mode);
void show_mult_obj_to_char(struct obj_data *o, struct char_data *ch, int m, int n);
void list_obj_in_room(struct obj_data *list, struct char_data *ch);
void list_obj_in_heap(struct obj_data *list, struct char_data *ch);
void list_obj_to_char(struct obj_data *l, struct char_data *ch, int m, bool show);
void show_char_to_char(struct char_data *i, struct char_data *ch, int mode);
void show_mult_char_to_char(struct char_data *i, struct char_data *ch, int m, int n);
void list_char_in_room(struct char_data *list, struct char_data *ch);
void list_char_to_char(struct char_data *list, struct char_data *ch, int mode);
void do_look(struct char_data *ch, char *argument, int cmd);
void do_read(struct char_data *ch, char *argument, int cmd);
void do_examine(struct char_data *ch, char *argument, int cmd);
void do_exits(struct char_data *ch, char *argument, int cmd);
void do_score(struct char_data *ch, char *argument, int cmd);
void do_time(struct char_data *ch, char *argument, int cmd);
void do_weather(struct char_data *ch, char *argument, int cmd);
void do_help(struct char_data *ch, char *argument, int cmd);
void do_wizhelp(struct char_data *ch, char *argument, int cmd);
void do_who(struct char_data *ch, char *argument, int cmd);
void do_users(struct char_data *ch, char *argument, int cmd);
void do_inventory(struct char_data *ch, char *argument, int cmd);
void do_equipment(struct char_data *ch, char *argument, int cmd);
void do_credits(struct char_data *ch, char *argument, int cmd);
void do_news(struct char_data *ch, char *argument, int cmd);
void do_info(struct char_data *ch, char *argument, int cmd);
void do_wizlist(struct char_data *ch, char *argument, int cmd);
int which_number_mobile(struct char_data *ch, struct char_data *mob);
char *numbered_person(struct char_data *ch, struct char_data *person);
void do_where_person(struct char_data *ch, struct char_data *p,
			    struct string_block *sb);
void do_where_object(struct char_data *ch, struct obj_data *obj,
			    int recurse, struct string_block *sb);
void do_where(struct char_data *ch, char *argument, int cmd);
void do_levels(struct char_data *ch, char *argument, int cmd);
void do_consider(struct char_data *ch, char *argument, int cmd);
void do_spells(struct char_data *ch, char *argument, int cmd);
void do_world(struct char_data *ch, char *argument, int cmd);
void do_attribute(struct char_data *ch, char *argument, int cmd);
void do_value(struct char_data *ch, char *argument, int cmd);
char *AlignDesc(int a);
char *ArmorDesc(int a);
char *HitRollDesc(int a);
char *DamRollDesc(int a);
char *DescRatio(float f);
char *DescDamage(float dam);
char *DescAttacks(float a);
char *EgoDesc(int a);
void show_exits(struct char_data *ch);
 
 
/* From act.obj1.c */
 
void get(struct char_data *ch, struct obj_data *obj_object, 
	struct obj_data *sub_object);
void do_get(struct char_data *ch, char *argument, int cmd);
void do_drop(struct char_data *ch, char *argument, int cmd);
void do_put(struct char_data *ch, char *argument, int cmd);
int newstrlen(char *p);
void do_give(struct char_data *ch, char *argument, int cmd);
 
 
/* From act.obj2.c */
 
void weight_change_object(struct obj_data *obj, int weight);
void name_from_drinkcon(struct obj_data *obj);
void name_to_drinkcon(struct obj_data *obj,int type);
void do_drink(struct char_data *ch, char *argument, int cmd);
void do_eat(struct char_data *ch, char *argument, int cmd);
void do_pour(struct char_data *ch, char *argument, int cmd);
void do_sip(struct char_data *ch, char *argument, int cmd);
void do_taste(struct char_data *ch, char *argument, int cmd);
int perform_wear(struct char_data *ch, struct obj_data *obj_object, int keyword);
int IsRestricted(int Mask, int Class);
void wear(struct char_data *ch, struct obj_data *obj_object, int keyword);
void do_wear(struct char_data *ch, char *argument, int cmd);
void do_wield(struct char_data *ch, char *argument, int cmd);
void do_grab(struct char_data *ch, char *argument, int cmd);
void do_remove(struct char_data *ch, char *argument, int cmd);
 
 
/* From act.off.c */
 
void do_hit(struct char_data *ch, char *argument, int cmd);
void do_kill(struct char_data *ch, char *argument, int cmd);
void do_backstab(struct char_data *ch, char *argument, int cmd);
void do_order(struct char_data *ch, char *argument, int cmd);
void do_flee(struct char_data *ch, char *argument, int cmd);
void do_bash(struct char_data *ch, char *argument, int cmd);
void do_rescue(struct char_data *ch, char *argument, int cmd);
void do_assist(struct char_data *ch, char *argument, int cmd);
void do_kick(struct char_data *ch, char *argument, int cmd);
void do_wimp(struct char_data *ch, char *argument, int cmd);
void do_breath(struct char_data *ch, char *argument, int cmd);
void do_shoot(struct char_data *ch, char *argument, int cmd);
void do_springleap(struct char_data *ch, char *argument, int cmd);
void do_quivering_palm( struct char_data *ch, char *arg, int cmd);
void kick_messages(struct char_data *ch, struct char_data *victim, int damage);
 
 
/* From act.other.c */
 
void do_gain(struct char_data *ch, char *argument, int cmd);
void do_guard(struct char_data *ch, char *argument, int cmd);
void do_junk(struct char_data *ch, char *argument, int cmd);
void do_qui(struct char_data *ch, char *argument, int cmd);
void do_title(struct char_data *ch, char *argument, int cmd);
void do_quit(struct char_data *ch, char *argument, int cmd);
void do_save(struct char_data *ch, char *argument, int cmd);
void do_not_here(struct char_data *ch, char *argument, int cmd);
void do_sneak(struct char_data *ch, char *argument, int cmd);
void do_hide(struct char_data *ch, char *argument, int cmd);
void do_steal(struct char_data *ch, char *argument, int cmd);
void do_practice(struct char_data *ch, char *arg, int cmd);
void do_idea(struct char_data *ch, char *argument, int cmd);
void do_typo(struct char_data *ch, char *argument, int cmd);
void do_bug(struct char_data *ch, char *argument, int cmd);
void do_brief(struct char_data *ch, char *argument, int cmd);
void do_compact(struct char_data *ch, char *argument, int cmd);
void do_group(struct char_data *ch, char *argument, int cmd);
void do_quaff(struct char_data *ch, char *argument, int cmd);
void do_recite(struct char_data *ch, char *argument, int cmd);
void do_use(struct char_data *ch, char *argument, int cmd);
void do_plr_noshout(struct char_data *ch, char *argument, int cmd);
void do_alias(struct char_data *ch, char *arg, int cmd);
int Dismount(struct char_data *ch, struct char_data *h, int pos);
void do_mount(struct char_data *ch, char *arg, int cmd);
void do_donate(struct char_data *ch, char *argument, int cmd);
void do_auto(struct char_data *ch, char *argument, int cmd);  
void do_gname(struct char_data *ch, char *argument, int cmd);
void do_show_exits( struct char_data *ch, char *arg, int cmd);
void do_split( struct char_data *ch, char *arg, int cmd);
void do_report( struct char_data *ch, char *arg, int cmd);
void do_prompt(struct char_data *ch, char *argument, int cmd);


/* From act.social.c */
 
char *fread_action(FILE *fl);
void boot_social_messages();
int find_action(int cmd);
void do_action(struct char_data *ch, char *argument, int cmd);
void do_insult(struct char_data *ch, char *argument, int cmd);
void boot_pose_messages();
void do_pose(struct char_data *ch, char *argument, int cmd);
 
 
/* From act.wizard.c */
 
void do_auth(struct char_data *ch, char *argument, int cmd);
void do_imptest(struct char_data *ch, char *arg, int cmd);
void do_passwd(struct char_data *ch, char *argument, int cmdnum);
void do_setsev(struct char_data *ch, char *arg, int cmd);
void dsearch(char *string, char *tmp);
void do_bamfin(struct char_data *ch, char *arg, int cmd);
void do_bamfout(struct char_data *ch, char *arg, int cmd);
void do_instazone(struct char_data *ch, char *argument, int cmdnum);
void do_highfive(struct char_data *ch, char *argument, int cmd);
void do_addhost(struct char_data *ch, char *argument, int command);
void do_removehost(struct char_data *ch, char *argument, int command);
void do_listhosts(struct char_data *ch, char *argument, int command);
void do_silence(struct char_data *ch, char *argument, int cmd);
void do_wizlock(struct char_data *ch, char *argument, int cmd);
void do_rload(struct char_data *ch, char *argument, int cmd);
void do_rsave(struct char_data *ch, char *argument, int cmd);
void do_emote(struct char_data *ch, char *argument, int cmd);
void do_echo(struct char_data *ch, char *argument, int cmd);
void do_system(struct char_data *ch, char *argument, int cmd);
void do_trans(struct char_data *ch, char *argument, int cmd);
void do_at(struct char_data *ch, char *argument, int cmd);
void do_goto(struct char_data *ch, char *argument, int cmd);
void do_stat(struct char_data *ch, char *argument, int cmd);
void do_set(struct char_data *ch, char *argument, int cmd);
void do_shutdow(struct char_data *ch, char *argument, int cmd);
void do_shutdown(struct char_data *ch, char *argument, int cmd);
void do_snoop(struct char_data *ch, char *argument, int cmd);
void do_switch(struct char_data *ch, char *argument, int cmd);
void do_return(struct char_data *ch, char *argument, int cmd);
void do_force(struct char_data *ch, char *argument, int cmd);
void do_load(struct char_data *ch, char *argument, int cmd);
void purge_one_room(int rnum, struct room_data *rp, int *range);
void do_purge(struct char_data *ch, char *argument, int cmd);
void roll_abilities(struct char_data *ch);
void do_start(struct char_data *ch);
void do_advance(struct char_data *ch, char *argument, int cmd);
void do_reroll(struct char_data *ch, char *argument, int cmd);
void do_restore(struct char_data *ch, char *argument, int cmd);
void do_noshout(struct char_data *ch, char *argument, int cmd);
void do_nohassle(struct char_data *ch, char *argument, int cmd);
void do_stealth(struct char_data *ch, char *argument, int cmd);
void print_room(int rnum, struct room_data *rp, struct string_block *sb);
void print_death_room(int rnum, struct room_data *rp, struct string_block *sb);
void print_private_room(int r, struct room_data *rp, struct string_block *sb);
void do_show(struct char_data *ch, char *argument, int cmd);
void do_debug(struct char_data *ch, char *argument, int cmd);
void do_invis(struct char_data *ch, char *argument, int cmd);
void do_create( struct char_data *ch, char *argument, int cmd);
void CreateOneRoom( int loc_nr);
void do_set_log(struct char_data *ch, char *arg, int cmd);
void do_event(struct char_data *ch, char *arg, int cmd);
void do_beep(struct char_data *ch, char *argument, int cmd);
 
 
/* From board.c */
 
void board_write_msg(struct char_data *ch, char *arg, int bnum);
int board_display_msg(struct char_data *ch, char *arg, int bnum);
int board_remove_msg(struct char_data *ch, char *arg, int bnum);
void board_save_board();
void board_load_board();
int board_show_board(struct char_data *ch, char *arg, int bnum);
int fwrite_string(char *buf, FILE *fl);
int board(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type);
char *fix_returns(char *text_string);
int board_check_locks (int bnum, struct char_data *ch);
 
 
/* From comm.c */
 
int __main ();
int close_socket_fd( int desc);
int main (int argc, char **argv);
int run_the_game(int port);
int game_loop(int s);
int get_from_q(struct txt_q *queue, char *dest);
void write_to_q(char *txt, struct txt_q *queue);
struct timeval timediff(struct timeval *a, struct timeval *b);
void flush_queues(struct descriptor_data *d);
int init_socket(int port);
int new_connection(int s);

int new_descriptor(int s);
int process_output(struct descriptor_data *t);
int write_to_descriptor(int desc, char *txt);
int process_input(struct descriptor_data *t);
void close_sockets(int s);
void close_socket(struct descriptor_data *d);
void nonblock(int s);
void coma(int s);
void send_to_char(char *messg, struct char_data *ch);
void save_all();
void send_to_all(char *messg);
void send_to_outdoor(char *messg);
void send_to_desert(char *messg);
void send_to_out_other(char *messg);
void send_to_arctic(char *messg);
void send_to_except(char *messg, struct char_data *ch);
void send_to_zone(char *messg, struct char_data *ch);
void send_to_room(char *messg, int room);
void send_to_room_except(char *messg, int room, struct char_data *ch);
void send_to_room_except_two
  (char *messg, int room, struct char_data *ch1, struct char_data *ch2);
void act(char *str, int hide_invisible, struct char_data *ch,
	 struct obj_data *obj, void *vict_obj, int type);
int raw_force_all( char *to_force);
 
 
/* From constants.c */
 
 
/* From db.c */

void SaveTheWorld(); 
void boot_db();
void reset_time();
void update_time();
void build_player_index();
struct index_data *generate_indices(FILE *fl, int *top);
void cleanout_room(struct room_data *rp);
void completely_cleanout_room(struct room_data *rp);
void load_one_room(FILE *fl, struct room_data *rp);
void boot_world();
void allocate_room(int room_number);
void setup_dir(FILE *fl, int room, int dir);
void renum_zone_table();
void boot_zones();
struct char_data *read_mobile(int nr, int type);
struct obj_data *read_object(int nr, int type);
void zone_update();
void reset_zone(int zone);
int is_empty(int zone_nr);
int load_char(char *name, struct char_file_u *char_element);
void store_to_char(struct char_file_u *st, struct char_data *ch);
void char_to_store(struct char_data *ch, struct char_file_u *st);
int create_entry(char *name);
void save_char(struct char_data *ch, sh_int load_room);
int compare(struct player_index_element *arg1, struct player_index_element 
	*arg2);
char *fread_string(FILE *fl);
void free_char(struct char_data *ch);
void free_obj(struct obj_data *obj);
int file_to_string(char *name, char *buf);
void ClearDeadBit(struct char_data *ch);
void clean_playerfile();
void reset_char(struct char_data *ch);
void clear_char(struct char_data *ch);
void clear_object(struct obj_data *obj);
void init_char(struct char_data *ch);
struct room_data *real_roomp(int virtual);
int real_mobile(int virtual);
int real_object(int virtual);
int ObjRoomCount(int nr, struct room_data *rp);
int str_len(char *buf);
int load();
void gr();
int workhours();
void reboot_text(struct char_data *ch, char *arg, int cmd);
void InitScripts();
void ReloadRooms();
void FreeZone(int zone_nr); 
void BootFigurines();
/* char *strmake(char *string); KEEP THIS OUT UNTIL FIXED */
/* void strfree(char *string);  KEEP THIS OUT UNTIL FIXED */
 
/* From fight.c */
 
void appear(struct char_data *ch);
int LevelMod(struct char_data *ch, struct char_data *v, int exp);
int RatioExp( struct char_data *ch, struct char_data *victim, int total);
void load_messages();
void update_pos( struct char_data *victim );
int check_peaceful(struct char_data *ch, char *msg);
void set_fighting(struct char_data *ch, struct char_data *vict);
void stop_fighting(struct char_data *ch);
void make_corpse(struct char_data *ch);
void change_alignment(struct char_data *ch, struct char_data *victim);
void death_cry(struct char_data *ch);
void raw_kill(struct char_data *ch);
void die(struct char_data *ch);
void group_gain(struct char_data *ch, struct char_data *victim);
char *replace_string(char *str, char *weapon, char *weapon_s);
void dam_message(int dam, struct char_data *ch, struct char_data *victim,
                 int w_type);
int DamCheckDeny(struct char_data *ch, struct char_data *victim, int type);
int DamDetailsOk( struct char_data *ch, struct char_data *v, int dam, int type);
int SetCharFighting(struct char_data *ch, struct char_data *v);
int SetVictFighting(struct char_data *ch, struct char_data *v);
int DamageTrivia(struct char_data *ch, struct char_data *v, int dam, int type);
int DoDamage(struct char_data *ch, struct char_data *v, int dam, int type);
int DamageMessages( struct char_data *ch, struct char_data *v, int dam,
		    int attacktype);
int DamageEpilog(struct char_data *ch, struct char_data *victim);
int MissileDamage(struct char_data *ch, struct char_data *victim,
	          int dam, int attacktype);
int damage(struct char_data *ch, struct char_data *victim,
	          int dam, int attacktype);
int GetWeaponType(struct char_data *ch, struct obj_data **wielded);
int Getw_type(struct obj_data *wielded);
int HitCheckDeny(struct char_data *ch, struct char_data *victim, int type);
int CalcThaco(struct char_data *ch);
int HitOrMiss(struct char_data *ch, struct char_data *victim, int calc_thaco);
int MissVictim(struct char_data *ch, struct char_data *v, int type, int w_type,
	       int (*dam_func)());
int GetWeaponDam(struct char_data *ch, struct char_data *v, 
		 struct obj_data *wielded);
int GetBackstabMult(struct char_data *ch, struct char_data *v);
int HitVictim(struct char_data *ch, struct char_data *v, int dam, 
		   int type, int w_type, int (*dam_func)());
void root_hit(struct char_data *ch, struct char_data *victim, int type, 
	      int (*dam_func)());
void MissileHit(struct char_data *ch, struct char_data *victim, int type);
void hit(struct char_data *ch, struct char_data *victim, int type);
void perform_violence(int pulse);
struct char_data *FindVictim( struct char_data *ch);
struct char_data *FindAnyVictim( struct char_data *ch);
int BreakLifeSaverObj( struct char_data *ch);
int BrittleCheck(struct char_data *ch, int dam);
int PreProcDam(struct char_data *ch, int type, int dam);
int DamageOneItem( struct char_data *ch, int dam_type, struct obj_data *obj);
void MakeScrap( struct char_data *ch, struct obj_data *obj);
void DamageAllStuff( struct char_data *ch, int dam_type);
int DamageItem(struct char_data *ch, struct obj_data *o, int num);
int ItemSave( struct obj_data *i, int dam_type);
int DamagedByAttack( struct obj_data *i, int dam_type);
int WeaponCheck(struct char_data *ch, struct char_data *v, int type, int dam);
int DamageStuff(struct char_data *v, int type, int dam);
int GetItemDamageType( int type);
int SkipImmortals(struct char_data *v, int amnt);
int WeaponSpell( struct char_data *c, struct char_data *v, int type);
struct char_data *FindAnAttacker(struct char_data *ch);
void shoot( struct char_data *ch, struct char_data *victim);
struct char_data *FindMetaVictim( struct char_data *ch);
void NailThisSucker( struct char_data *ch);
int GetFormType(struct char_data *ch);
int MonkDodge( struct char_data *ch, struct char_data *v, int *dam);
int GetMonkw_type(struct char_data *ch); 
int AreaDamage(struct char_data *ch, int dam, int attacktype,
               char *same_room_hitmsg, char *same_room_missmsg,
               char *global_msg, bool save_negates, bool heat_blind);

/* From handler.c */
 
char *fname(char *namelist);
int split_string(char *str, char *sep, char **argv);
int isname(const char *str, const char *namelist);
void init_string_block(struct string_block *sb);
void append_to_string_block(struct string_block *sb, char *str);
void page_string_block(struct string_block *sb, struct char_data *ch);
void destroy_string_block(struct string_block *sb);
void affect_modify(struct char_data *ch,byte loc, long mod, long bitv,bool add);
void affect_total(struct char_data *ch);
void affect_to_char( struct char_data *ch, struct affected_type *af );
void affect_remove( struct char_data *ch, struct affected_type *af );
void affect_from_char( struct char_data *ch, short skill);
bool affected_by_spell( struct char_data *ch, short skill );
void affect_join( struct char_data *ch, struct affected_type *af,
		 bool avg_dur, bool avg_mod );
void char_from_room(struct char_data *ch);
void char_to_room(struct char_data *ch, int room);
void obj_to_char(struct obj_data *object, struct char_data *ch);
void obj_from_char(struct obj_data *object);
int apply_ac(struct char_data *ch, int eq_pos);
void equip_char(struct char_data *ch, struct obj_data *obj, int pos);
int GiveMinStrToWield(struct obj_data *obj, struct char_data *ch);
struct obj_data *unequip_char(struct char_data *ch, int pos);
int get_number(char **name);
struct obj_data *get_obj_in_list(char *name, struct obj_data *list);
struct obj_data *get_obj_in_list_num(int num, struct obj_data *list);
struct obj_data *get_obj(char *name);
struct obj_data *get_obj_num(int nr);
struct char_data *get_char_room(char *name, int room);
struct char_data *get_char(char *name);
struct char_data *get_char_num(int nr);
void obj_to_room(struct obj_data *object, int room);
void obj_to_room2(struct obj_data *object, int room);
void obj_from_room(struct obj_data *object);
void obj_to_obj(struct obj_data *obj, struct obj_data *obj_to);
void obj_from_obj(struct obj_data *obj);
void object_list_new_owner(struct obj_data *list, struct char_data *ch);
void extract_obj(struct obj_data *obj);
void update_object( struct obj_data *obj, int use);
void update_char_objects( struct char_data *ch );
void extract_char(struct char_data *ch);
void extract_char_smarter(struct char_data *ch, int save_room);
struct char_data *get_char_room_vis(struct char_data *ch, char *name);
struct char_data *get_char_vis_world(struct char_data *ch, char *name,
				     int *count);
struct char_data *get_char_vis(struct char_data *ch, char *name);
struct obj_data *get_obj_in_list_vis(struct char_data *ch, char *name, 
				     struct obj_data *list);
struct obj_data *get_obj_vis_world(struct char_data *ch, char *name,
				   int *count);
struct obj_data *get_obj_vis(struct char_data *ch, char *name);
struct obj_data *get_obj_vis_accessible(struct char_data *ch, char *name);
struct obj_data *create_money( int amount );
int generic_find(char *arg, int bitvector, struct char_data *ch,
		 struct char_data **tar_ch, struct obj_data **tar_obj);
void AddAffects( struct char_data *ch, struct obj_data *o);
 
 
/* From hash.c */
 
void init_hash_table(struct hash_header	*ht, int rec_size, int table_size);
void init_world(struct room_data *room_db[]);
void destroy_hash_table(struct hash_header *ht, void (*gman)());
void _hash_enter(struct hash_header *ht, int key, void *data);
struct room_data *room_find( struct room_data *room_db[], int key);
void *hash_find(struct hash_header *ht, int key);
int room_enter(struct room_data *rb[], int key, struct room_data *rm);
int hash_enter(struct hash_header *ht, int key, void *data);
struct room_data *room_find_or_create(struct room_data *rb[], int key);
void *hash_find_or_create(struct hash_header *ht, int key);
int room_remove(struct room_data *rb[], int key);
void *hash_remove(struct hash_header *ht, int key);
void room_iterate(struct room_data *rb[], void (*func)(), void *cdata);
void hash_iterate(struct hash_header *ht, void (*func)(), void *cdata);
 
 
/* From interpreter.c */
 
void commando(int number, int min_pos, void (*pointer)(), int min_level);
int search_block(char *arg, char **list, bool exact);
int old_search_block(char *argument,int begin,int length,char **list,int mode);
void command_interpreter(struct char_data *ch, char *argument);
void argument_interpreter(char *argument,char *first_arg,char *second_arg );
int is_number(char *str);
char *one_argument(char *argument, char *first_arg );
void only_argument(char *argument, char *dest);
int fill_word(char *argument);
int is_abbrev(char *arg1, char *arg2);
void half_chop(char *string, char *arg1, char *arg2);
int special(struct char_data *ch, int cmd, char *arg);
void assign_command_pointers ();
int find_name(char *name);
int _parse_name(char *arg, char *name);
void nanny(struct descriptor_data *d, char *arg);
void DisplayRaces(struct descriptor_data *d);
int get_racial_alignment(struct descriptor_data *d);
int CheckValidClass(struct descriptor_data *d, int class);
void DisplayRaceClasses(struct descriptor_data *d);

#if 0
int command_search( char *arg, int len, struct command_info *cmd_info, 
  char **list);
#endif 
 
/* From limits.c */
 
char *ClassTitles(struct char_data *ch);
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6);
int mana_limit(struct char_data *ch);
int hit_limit(struct char_data *ch);
int move_limit(struct char_data *ch);
int mana_gain(struct char_data *ch);
int hit_gain(struct char_data *ch);
int move_gain(struct char_data *ch);
void advance_level(struct char_data *ch, int class);
void drop_level(struct char_data *ch, int class);
void set_title(struct char_data *ch);
void gain_exp(struct char_data *ch, int gain);
void gain_exp_regardless(struct char_data *ch, int gain, int class);
void gain_condition(struct char_data *ch,int condition,int value);
void check_idling(struct char_data *ch);
int ObjFromCorpse( struct obj_data *c);
int ClassSpecificStuff( struct char_data *ch);
 
 
/* From magic.c */
 
void heat_blind(struct char_data *ch);
void spell_magic_missile(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_chill_touch(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_burning_hands(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_shocking_grasp(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_lightning_bolt(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_colour_spray(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_energy_drain(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_fireball(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_earthquake(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_dispel_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_call_lightning(byte level, struct char_data *ch,
			  struct char_data *victim, struct obj_data *obj);
void spell_harm(byte level, struct char_data *ch,
		struct char_data *victim, struct obj_data *obj);
void spell_armor(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj);
void spell_astral_walk(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_teleport(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_bless(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj);
void spell_blindness(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_clone(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_control_weather(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_create_food(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_create_water(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_cure_blind(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_cure_critic(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_cure_light(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_curse(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_detect_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_detect_invisibility(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_detect_magic(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_detect_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_enchant_weapon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_enchant_armor(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_heal(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_invisibility(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_locate_object(byte level, struct char_data *ch,
			 struct char_data *victim, struct obj_data *obj);
void spell_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_protection_from_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_remove_curse(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_remove_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_fireshield(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_sanctuary(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_sleep(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_strength(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_ventriloquate(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_word_of_recall(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_summon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void RawSummon( struct char_data *v, struct char_data *c);
void spell_charm_person(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_charm_monster(byte level, struct char_data *ch,
			 struct char_data *victim, struct obj_data *obj);
void spell_sense_life(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_identify(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_fire_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_frost_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_acid_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_gas_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_lightning_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
 
 
/* Fom magic2.c */
 
void spell_resurrection(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj);
void spell_cause_light(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_cause_critical(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_cause_serious(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_cure_serious(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_mana(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_second_wind(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_flamestrike(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_dispel_good(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_turn(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_remove_paralysis(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_holy_word(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
  void holy_banish(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj);
  void holy_destroy(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj);
  void holy_bigdif(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj);
  void holy_meddif(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj);
  void holy_smldif(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj);
void spell_succor(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_detect_charm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_true_seeing(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_track(byte level, struct char_data *ch,
   struct char_data *targ, struct obj_data *obj);
void spell_poly_self(byte level, struct char_data *ch,
   struct char_data *mob, struct obj_data *obj);
void spell_minor_create(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_stone_skin(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_infravision(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_shield(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_weakness(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_invis_group(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_acid_blast(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_cone_of_cold(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_ice_storm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_poison_cloud(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_major_create(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_sending(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_meteor_swarm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_Create_Monster(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_light(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_fly(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_fly_group(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_refresh(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_water_breath(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_cont_light(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_animate_dead(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *corpse);
void spell_know_alignment(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_dispel_magic(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_paralyze(byte level, struct char_data *ch,
		    struct char_data *victim, struct obj_data *obj);
void spell_fear(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_calm(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_web(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_heroes_feast(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_conjure_elemental(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_faerie_fire (byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_faerie_fog (byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_cacaodemon(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_improved_identify(byte level, struct char_data *ch,
   struct char_data *victim, struct obj_data *obj);
void spell_geyser(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_green_slime(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
 
 
/* From magic3.c */
 
void spell_tree_travel(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj);
void spell_transport_via_plant(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj);
void spell_speak_with_plants(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj);
void spell_changestaff(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj);
void spell_pword_kill(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_pword_blind(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_chain_lightn(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_scare(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_haste(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_slow(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj);
void spell_familiar(byte level, struct char_data *ch,
		       struct char_data **victim, struct obj_data *obj);
void spell_aid(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj);
void spell_golem(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj);
void spell_feeblemind(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj);
void spell_shillelagh(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_goodberry(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_flame_blade(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_animal_growth(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_insect_growth(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_creeping_death(byte level, struct char_data *ch,
  struct char_data *victim, int dir);
void spell_commune(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_animal_summon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_elemental_summoning(byte level, struct char_data *ch,
  struct char_data *victim, int spell);
void spell_reincarnate(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_charm_veggie(byte level, struct char_data *ch,
			 struct char_data *victim, struct obj_data *obj);
void spell_veggie_growth(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_tree(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_animate_rock(byte level, struct char_data *ch,
     struct char_data *victim, struct obj_data *obj);
void spell_travelling(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_animal_friendship(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_invis_to_animals(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_slow_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_snare(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_entangle(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_barkskin(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_gust_of_wind(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_silence(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_warp_weapon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_heat_stuff(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_dust_devil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_sunray(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_know_monster(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_find_traps(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_firestorm(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_teleport_wo_error(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_portal(byte level, struct char_data *ch,
  struct char_data *tmp_ch, struct obj_data *obj);
void spell_mount(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_dragon_ride(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_thorn_spray(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_hold(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_electricity(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_cold(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_drain(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_acid(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_fire(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_energy(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_pierce(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_slash(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
void spell_resist_blunt(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj);
 
/* From magicutils.c */
 
int SwitchStuff( struct char_data *giver, struct char_data *taker);
int FailCharm(struct char_data *victim, struct char_data *ch);
int FailSnare(struct char_data *victim, struct char_data *ch);
int FailSleep(struct char_data *victim, struct char_data *ch);
int FailPara(struct char_data *victim, struct char_data *ch);
int FailCalm(struct char_data *victim, struct char_data *ch);
int ItemMagicFailure(struct char_data *ch, int skill_number);
 
/* From mobact.c */
 
void mobile_guardian(struct char_data *ch);
void mobile_wander(struct char_data *ch);
void MobHunt(struct char_data *ch);
void MobScavenge(struct char_data *ch);
void check_mobile_activity(int pulse);
void mobile_activity(struct char_data *ch);
int SameRace( struct char_data *ch1, struct char_data *ch2);
int AssistFriend( struct char_data *ch);
int FindABetterWeapon(struct char_data *mob);
int GetDamage(struct obj_data *w, struct char_data *ch);
int GetDamBonus(struct obj_data *w);
int GetHandDamage(struct char_data *ch);
int MobFriend( struct char_data *ch, struct char_data *f);
void PulseMobiles(int type);
void DoScript(struct char_data *ch);
int CommandSearch(char *arg);
void CommandAssign(char *arg, void (*p));
void CommandSetup();
void noop(char *arg, struct char_data *ch);
void end2(char *arg, struct char_data *ch);
void sgoto(char *arg, struct char_data *ch);
void do_act(char *arg, struct char_data *ch);
void do_jmp(char *arg, struct char_data *ch);
void do_jsr(char *arg, struct char_data *ch);
void do_rts(char *arg, struct char_data *ch);
void MobHit(struct char_data *, struct char_data *, int);
 
 
/* From modify.c */
 
void string_add(struct descriptor_data *d, char *str);
void quad_arg(char *arg, int *type, char *name, int *field, char *string);
void do_string(struct char_data *ch, char *arg, int cmd);
void bisect_arg(char *arg, int *field, char *string);
void do_edit(struct char_data *ch, char *arg, int cmd);
void do_setskill(struct char_data *ch, char *arg, int cmd);
char *one_word(char *argument, char *first_arg );
struct help_index_element *build_help_index(FILE *fl, int *num);
void page_string(struct descriptor_data *d, char *str, int keep_internal);
void show_string(struct descriptor_data *d, char *input);
void night_watchman();
void check_reboot();
 
 
/* From multiclass.c */
 
int GetClassLevel(struct char_data *ch, int class);
int CountBits(int class);
int OnlyClass( struct char_data *ch, int class);
int HasClass(struct char_data *ch, int class);
int HowManyClasses(struct char_data *ch);
int BestFightingClass(struct char_data *ch);
int BestThiefClass(struct char_data *ch);
int BestMagicClass(struct char_data *ch);
int GetSecMaxLev(struct char_data *ch);
int GetALevel(struct char_data *ch, int which);
int GetThirdMaxLev(struct char_data *ch);
int GetMaxLevel(struct char_data *ch);
int GetTotLevel(struct char_data *ch);
void StartLevels(struct char_data *ch);
int BestClass(struct char_data *ch);
 
 
/* From reception.c */
 
int add_obj_cost(struct char_data *ch, struct char_data *re,
                  struct obj_data *obj, struct obj_cost *cost, int hoarder);
bool recep_offer(struct char_data *ch,	struct char_data *receptionist,
		 struct obj_cost *cost);
void update_file(struct char_data *ch, struct obj_file_u *st);
void obj_store_to_char(struct char_data *ch, struct obj_file_u *st);
void load_char_objs(struct char_data *ch);
void put_obj_in_store(struct obj_data *obj, struct obj_file_u *st);
int contained_weight(struct obj_data *container);
void obj_to_store(struct obj_data *obj, struct obj_file_u *st,
                  struct char_data * ch, int delete);
void save_obj(struct char_data *ch, struct obj_cost *cost, int delete);
void update_obj_file();
void CountLimitedItems(struct obj_file_u *st);
void PrintLimitedItems();
int receptionist(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
void zero_rent( struct char_data *ch);
void ZeroRent( char *n);
int ReadObjs( FILE *fl, struct obj_file_u *st);
int WriteObjs( FILE *fl, struct obj_file_u *st);
void load_char_extra(struct char_data *ch);
void write_char_extra( struct char_data *ch);
void obj_store_to_room(int room, struct obj_file_u *st);
void load_room_objs(int room);
void save_room(int room);
 
 
/* From security.c */
 
int SecCheck(char *arg, char *site);
 
 
/* From shop.c */
 
int is_ok(struct char_data *keeper, struct char_data *ch, int shop_nr);
int trade_with(struct obj_data *item, int shop_nr);
int shop_producing(struct obj_data *item, int shop_nr);
void shopping_buy( char *arg, struct char_data *ch,
	 struct char_data *keeper, int shop_nr);
void shopping_sell( char *arg, struct char_data *ch,
		   struct char_data *keeper,int shop_nr);
void shopping_value( char *arg, struct char_data *ch, 
		    struct char_data *keeper, int shop_nr);
void shopping_list( char *arg, struct char_data *ch,
		   struct char_data *keeper, int shop_nr);
void shopping_kill( char *arg, struct char_data *ch,
		   struct char_data *keeper, int shop_nr);
int shop_keeper(struct char_data *ch, int cmd, char *arg, char *mob, int type);
void boot_the_shops();
void assign_the_shopkeepers();
int DoIHateYou(struct char_data *v); 
 
/* From signals.c */
 
void signal_setup();
int checkpointing();
int shutdown_request();
int hupsig();
int logsig();
 
 
/* From skills.c */
 
void do_inset(struct char_data *ch, char *argument, int cmd);
void do_train(struct char_data *ch, char *argument, int cmd);
void do_disarm(struct char_data *ch, char *argument, int cmd);
void do_track(struct char_data *ch, char *argument, int cmd);
int track( struct char_data *ch, struct char_data *vict);
int dir_track( struct char_data *ch, struct char_data *vict);
void donothing();
int find_path(int in_room, int (*predicate)(), void *c_data, 
	      int depth, int in_zone);
int choose_exit_global(int in_room, int tgt_room, int depth);
int choose_exit_in_zone(int in_room, int tgt_room, int depth);
int go_direction(struct char_data *ch, int dir);
void slam_into_wall( struct char_data *ch, struct room_direction_data *exitp);
void do_doorbash( struct char_data *ch, char *arg, int cmd);
void do_swim( struct char_data *ch, char *arg, int cmd);
int SpyCheck(struct char_data *ch);
void do_spy( struct char_data *ch, char *arg, int cmd);
int remove_trap( struct char_data *ch, struct obj_data *trap);
void do_feign_death( struct char_data *ch, char *arg, int cmd);
void do_first_aid( struct char_data *ch, char *arg, int cmd);
void do_disguise(struct char_data *ch, char *argument, int cmd);
void do_climb( struct char_data *ch, char *arg, int cmd);
void slip_in_climb(struct char_data *ch, int dir, int room);
void do_makepotion(struct char_data *ch, char *argument, int cmd);
void add_skill(int nr, int taught_by, int class_use, int percent);
void do_berserk( struct char_data *ch, char *arg, int cmd);
void do_palm( struct char_data *ch, char *arg, int cmd);
void do_peek( struct char_data *ch, char *arg, int cmd);
 
/* From spec_assign.c */
 
void assign_mobiles();
void assign_objects();
void assign_rooms();
 
 
/* From spec_procs.c */
 
int is_target_room_p(int room, void *tgt_room);
int named_object_on_ground(int room, void *c_data);
char *how_good(int percent);
int GainLevel(struct char_data *ch, int class);
struct char_data *FindMobInRoomWithFunction(int room, int (*func)());
int MageGuildMaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type) ;
int ClericGuildMaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type) ;
int ThiefGuildMaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type) ;
int WarriorGuildMaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type) ;
int Guildmaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type, int class);
int dump(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int mayor(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int andy_wilcox(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
struct char_data *find_mobile_here_with_spec_proc(int (*fcn)(), int rnumber);
int eric_johnson(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
void exec_social(struct char_data *npc, char *cmd, int next_line,
                 int *cur_line, void **thing);
void npc_steal(struct char_data *ch,struct char_data *victim);
int snake(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int MidgaardCityguard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int PaladinGuildGuard( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int GameGuard( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int GreyParamedic(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int types);
int AmberParamedic(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int blink( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int MidgaardCitizen(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int ghoul(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int CarrionCrawler(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int WizardGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int vampire(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int wraith(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int shadow(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int geyser(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int green_slime(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int DracoLich(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Drow(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Leader(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int thief(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int guild_guard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Inquisitor(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int puff(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int regenerator( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int replicant( struct char_data *ch, int cmd, char *arg, struct char_data *mob1, int type);
int Tytan(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int AbbarachDragon(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int fido(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int janitor(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int tormentor(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int RustMonster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int temple_labrynth_liar(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int temple_labrynth_sentry(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Whirlwind (struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int NudgeNudge(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int AGGRESSIVE(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int citizen(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Ringwraith( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int WarrenGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int zm_tired(struct char_data *zmaster);
int zm_stunned_followers(struct char_data *zmaster);
int zm_init_combat(struct char_data *zmaster, struct char_data *target);
int zm_kill_fidos(struct char_data *zmaster);
int zm_kill_aggressor(struct char_data *zmaster);
int zombie_master(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int pet_shops(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int Fountain(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int bank (struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int pray_for_items(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int chalice(struct char_data *ch, int cmd, char *arg);
int kings_hall(struct char_data *ch, int cmd, char *arg);
int Donation(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int House(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int sisyphus(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int jabberwocky(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int flame(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int banana(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int paramedics(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int jugglernaut(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int delivery_elf(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int delivery_beast(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Keftab(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int StormGiant(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Manticore(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Kraken(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int fighter(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int NewThalosMayor(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int SultanGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int NewThalosCitizen(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int NewThalosGuildGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int magic_user2(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int MordGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int MordGuildGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int CaravanGuildGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int StatTeller(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
void ThrowChar(struct char_data *ch, struct char_data *v, int dir);
int ThrowerMob(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Tyrannosaurus_swallower(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int soap(struct char_data *ch, int cmd, char *arg, struct obj_data *tobj, int type);
int nodrop(struct char_data *ch, int cmd, char *arg, struct obj_data *tobj, int type);
int lattimore(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int coldcaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int trapper(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int trogcook(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int shaman(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int golgar(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int troguard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int ghostsoldier(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Valik( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type );
int guardian(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int web_slinger(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
 
 
/* From spec_procs2.c */
 
int ghost(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int druid_protector(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Magic_Fountain(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int DruidAttackSpells(struct char_data *ch, struct char_data *vict, int level);
int Summoner(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int monk(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
void invert(char *arg1, char *arg2);
int jive_box(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type);
int magic_user(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int cleric(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int ninja_master(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int ettin(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int RepairGuy( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Samah( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int BitterBlade(struct char_data *ch, int cmd, char *arg,struct obj_data *tobj, int type);
int MakeQuest(struct char_data *ch, struct char_data *gm, int Class, char *arg, int cmd) ;
int AbyssGateKeeper( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int creeping_death( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
void  Submit(struct char_data *ch, struct char_data *t);
void  SayHello(struct char_data *ch, struct char_data *t);
void GreetPeople(struct char_data *ch);
int GenericCityguardHateUndead(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int GenericCityguard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int PrydainGuard(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
struct breath_victim *choose_victims(struct char_data *ch,
				     struct char_data *first_victim);
void free_victims(struct breath_victim *head);
int breath_weapon(struct char_data *ch, struct char_data *target,
		  int mana_cost, void (*func)());
int use_breath_weapon(struct char_data *ch, struct char_data *target,
		      int cost, void (*func)());
int BreathWeapon(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int sailor(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int loremaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int hunter(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int monk_master(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int DruidGuildMaster(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type) ;
int Teacher(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type, int teacher, char *say_str);
int Devil(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int Demon(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
void DruidHeal(struct char_data *ch, int level);
int DruidTree(struct char_data *ch);
int DruidMob(struct char_data *ch);
int DruidChallenger(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int MonkChallenger(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int druid_challenge_prep_room(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int druid_challenge_room(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int monk_challenge_room(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int monk_challenge_prep_room(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int portal(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type);
int scraps(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type);
int attack_rats(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int DragonHunterLeader(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int HuntingMercenary(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int SlotMachine(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type);
int astral_portal(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int DwarvenMiners(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int real_rabbit(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int real_fox(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int antioch_grenade(struct char_data *ch, int cmd, char *arg, struct obj_data *obj, int type);
 
 
/* From spec_procs3.c */
 
struct room_data *forward_square(struct room_data *room);
struct room_data *back_square(struct room_data *room);
struct room_data *left_square(struct room_data *room);
struct room_data *right_square(struct room_data *room);
struct room_data *forward_left_square(struct room_data *room);
struct room_data *forward_right_square(struct room_data *room);
struct room_data *back_right_square(struct room_data *room);
struct room_data *back_left_square(struct room_data *room);
struct char_data *square_contains_enemy(struct room_data *square);
int square_contains_friend(struct room_data *square);
int square_empty(struct room_data *square);
int chess_game(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int AcidBlob(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int baby_bear(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int timnus(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int winger(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
 int DeathRoom(int dt_room);
int YouthPotion( struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int warpstone(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int turbo_lift(struct char_data *ch, int cmd, char *arg, struct room_data *rp,
int type);
int entering_turbo_lift(struct char_data *ch, int cmd, char *arg, struct room_data *rp, int type);
int OldHag(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
/* From spell_parser.c */
 
void spello(int nr, byte beat, byte pos, byte mlev, byte clev, byte dlev, ubyte mana,sh_int tar, void *func, sh_int sf);
int SPELL_LEVEL(struct char_data *ch, int sn);
void affect_update( int pulse );
void clone_char(struct char_data *ch);
void clone_obj(struct obj_data *obj);
bool circle_follow(struct char_data *ch, struct char_data *victim);
void stop_follower(struct char_data *ch);
void die_follower(struct char_data *ch);
void add_follower(struct char_data *ch, struct char_data *leader);
int say_spell( struct char_data *ch, int si );
bool saves_spell(struct char_data *ch, sh_int save_type);
bool ImpSaveSpell(struct char_data *ch, sh_int save_type, int mod);
char *skip_spaces(char *string);
void do_cast(struct char_data *ch, char *argument, int cmd);
void assign_spell_pointers();
void SpellWearOffSoon(int s, struct char_data *ch);
void SpellWearOff(int s, struct char_data *ch);
void check_decharm( struct char_data *ch);
int check_falling( struct char_data *ch);
void check_drowning( struct char_data *ch);
void check_falling_obj( struct obj_data *obj, int room);
int check_nature( struct char_data *i);
int death_knight(struct char_data *ch, int cmd, char *arg, struct char_data *mob, int type);
int IsIntrinsic(struct char_data *ch, int spl);
int CastIntrinsic(struct char_data *ch, int spl);
 
/* From spells1.c */
 
void cast_burning_hands( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_call_lightning(byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_chill_touch( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_shocking_grasp( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_colour_spray( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_earthquake( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_energy_drain( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_fireball( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_harm( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_lightning_bolt( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_acid_blast( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_cone_of_cold( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_ice_storm( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_meteor_swarm( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_flamestrike( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_magic_missile( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_cause_light( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_cause_serious( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_cause_critic(byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_geyser( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_green_slime( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
 
 
/* From spells2.c */
 
void cast_resurrection( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_major_track( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_minor_track( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mana( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_armor( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_stone_skin( byte level, struct char_data *ch, char *arg, int type,
		     struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_astral_walk( byte level, struct char_data *ch, char *arg, int type,
		   struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_teleport( byte level, struct char_data *ch, char *arg, int type,
		   struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_bless( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_infravision( byte level, struct char_data *ch, char *arg, int type,
		      struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_true_seeing( byte level, struct char_data *ch, char *arg, int type,
		     struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_blindness( byte level, struct char_data *ch, char *arg, int type,
		    struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_light( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_cont_light( byte level, struct char_data *ch, char *arg, int type,
		     struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_web( byte level, struct char_data *ch, char *arg, int type,
	       struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_clone( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_control_weather(byte level,struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_create_food( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_create_water( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_water_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_flying( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_fly_group( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_heroes_feast( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_cure_blind( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_cure_critic( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_cure_light( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_cure_serious( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_refresh( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_second_wind( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_shield( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_curse( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_detect_evil( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_detect_invisibility( byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_detect_magic( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_detect_poison( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_dispel_evil( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_dispel_good( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_faerie_fire( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_enchant_weapon( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_enchant_armor( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_heal( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_invisibility( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_locate_object( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_poison( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_protection_from_evil( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_remove_curse( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_remove_poison( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_remove_paralysis( byte level, struct char_data *ch, char *arg, int type,  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_sanctuary( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_fireshield( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_sleep( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_strength( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_ventriloquate( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_word_of_recall( byte level, struct char_data *ch, char *arg,int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_summon( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_charm_person( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_charm_monster( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_sense_life( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_identify( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_dragon_breath(byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *potion );
void cast_fire_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_frost_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_acid_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_gas_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_lightning_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_knock( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_know_alignment(byte level, struct char_data *ch, char *arg, 
	  int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_weakness( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_dispel_magic( byte level, struct char_data *ch, char *arg, 
	    int type,struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animate_dead( byte level, struct char_data *ch, char *arg, int type,
	struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_succor( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_paralyze( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_fear( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_turn( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_faerie_fog( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj );
void cast_poly_self( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_minor_creation(byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_conjure_elemental( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_cacaodemon( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum1( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum2( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum3( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum4( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum5( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum6( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mon_sum7( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_tree_travel( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_speak_with_plants( byte level, struct char_data *ch, char *arg, 
		int type,  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_transport_via_plant( byte level, struct char_data *ch, char *arg, 
		int type,  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_haste( byte level, struct char_data *ch, char *arg, 
		int type,  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_slow( byte level, struct char_data *ch, char *arg, 
	       int type,  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_reincarnate( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_changestaff( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_pword_kill( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_pword_blind( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_chain_lightn( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_scare( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_familiar( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_aid( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_holyword( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_unholyword( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_golem( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_command( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_change_form( byte level, struct char_data *ch, char *arg, 
int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_shillelagh( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_goodberry( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_flame_blade( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animal_growth( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_insect_growth( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_creeping_death( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_commune( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_feeblemind( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animal_summon_1( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animal_summon_2( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animal_summon_3( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_fire_servant( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_earth_servant( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_water_servant( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_wind_servant( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_veggie_growth( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_charm_veggie( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_tree( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animate_rock( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_travelling( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_animal_friendship( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_invis_to_animals( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_slow_poison( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_entangle( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_snare( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_gust_of_wind( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_barkskin( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_warp_weapon( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_calm( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_heat_stuff( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_sunray( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_find_traps( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_firestorm( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_dust_devil( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_know_monster( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_silence( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_dragon_ride( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_mount( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_sending( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_portal( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_teleport_wo_error( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *tar_ch, struct obj_data *tar_obj );
void cast_thorn_spray( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_hold( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_electricity( byte level, struct char_data *ch, char *arg,
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_cold( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_drain( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_poison( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_acid( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_fire( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_energy( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_pierce( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_slash( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );
void cast_resist_blunt( byte level, struct char_data *ch, char *arg, 
     int type, struct char_data *victim, struct obj_data *tar_obj );

 
/* From utility.c */

int char_array_size(char *thingie[]);
int EgoBladeSave(struct char_data *ch);
int MIN(int a, int b);
int MAX(int a, int b);
int GetItemClassRestrictions(struct obj_data *obj);
int CAN_SEE(struct char_data *s, struct char_data *o);
int exit_ok(struct room_direction_data	*exit, struct room_data **rpp);
int MobVnum( struct char_data *c);
int ObjVnum( struct obj_data *o);
void Zwrite (FILE *fp, char cmd, int tf, int arg1, int arg2, int arg3, 
	     char *desc);
void RecZwriteObj(FILE *fp, struct obj_data *o);
FILE *MakeZoneFile( struct char_data *c);
int WeaponImmune(struct char_data *ch);
unsigned IsImmune(struct char_data *ch, int bit);
unsigned IsResist(struct char_data *ch, int bit);
unsigned IsSusc(struct char_data *ch, int bit);
int number(int from, int to);
int dice(int number, int size);
int scan_number(char *text, int *rval);
int str_cmp(char *arg1, char *arg2);
int strn_cmp(char *arg1, char *arg2, int n);
void log_sev(char *str,int sev);
void slog(char *str);
void sprintbit(unsigned long vektor, char *names[], char *result);
void sprinttype(int type, char *names[], char *result);
struct time_info_data real_time_passed(time_t t2, time_t t1);
struct time_info_data mud_time_passed(time_t t2, time_t t1);
void mud_time_passed2(time_t t2, time_t t1, struct time_info_data *t);
void age2(struct char_data *ch, struct time_info_data *g);
struct time_info_data age(struct char_data *ch);
char getall(char *name, char *newname);
int getabunch(char *name, char  *newname);
int DetermineExp( struct char_data *mob, int exp_flags);
void down_river( int pulse );
void RoomSave(struct char_data *ch, int start, int end);
void RoomLoad( struct char_data *ch, int start, int end);
void fake_setup_dir(FILE *fl, int room, int dir);
int IsHumanoid( struct char_data *ch);
int IsRideable( struct char_data *ch);
int IsAnimal( struct char_data *ch);
int IsVeggie( struct char_data *ch);
int IsUndead( struct char_data *ch);
int IsLycanthrope( struct char_data *ch);
int IsDiabolic( struct char_data *ch);
int IsReptile( struct char_data *ch);
int HasHands( struct char_data *ch);
int IsPerson( struct char_data *ch);
int IsGiantish( struct char_data *ch);
int IsSmall( struct char_data *ch);
int IsGiant ( struct char_data *ch);
int IsExtraPlanar( struct char_data *ch);
int IsOther( struct char_data *ch);
int IsGodly( struct char_data *ch);
void SetHunting( struct char_data *ch, struct char_data *tch);
void CallForGuard
  ( struct char_data *ch, struct char_data *vict, int lev, int area);
void StandUp (struct char_data *ch);
void MakeNiftyAttack( struct char_data *ch);
void FighterMove( struct char_data *ch);
void MonkMove( struct char_data *ch);
void DevelopHatred( struct char_data *ch, struct char_data *v);
int HasObject( struct char_data *ch, int ob_num);
int room_of_object(struct obj_data *obj);
struct char_data *char_holding(struct obj_data *obj);
int RecCompObjNum( struct obj_data *o, int obj_num);
void RestoreChar(struct char_data *ch);
void RemAllAffects( struct char_data *ch);
int CheckForBlockedMove
  (struct char_data *ch, int cmd, char *arg, int room, int dir, int class);
void TeleportPulseStuff(int pulse);
void RiverPulseStuff(int pulse);
int apply_soundproof(struct char_data *ch);
int check_soundproof(struct char_data *ch);
int MobCountInRoom( struct char_data *list);
void *Mymalloc( long size);
int SpaceForSkills(struct char_data *ch);
int CountLims(struct obj_data *obj);
char *lower(char *s);
int getFreeAffSlot( struct obj_data *obj);
void SetRacialStuff( struct char_data *mob);
int check_nomagic(struct char_data *ch, char *msg_ch, char *msg_rm);
int NumCharmedFollowersInRoom(struct char_data *ch);
struct char_data *FindMobDiffZoneSameRace(struct char_data *ch);
int NoSummon(struct char_data *ch);
int GetNewRace(struct char_file_u *s);
int GetApprox(int num, int perc);
int MountEgoCheck(struct char_data *ch, struct char_data *horse);
int RideCheck( struct char_data *ch, int mod);
void FallOffMount(struct char_data *ch, struct char_data *h);
int EqWBits(struct char_data *ch, int bits);
int InvWBits(struct char_data *ch, int bits);
int HasWBits(struct char_data *ch, int bits);
int LearnFromMistake(struct char_data *ch, int sknum, int silent, int max);
int GetSumRaceMaxLevInRoom( struct char_data *ch);
int too_many_followers(struct char_data *ch);
int follow_time(struct char_data *ch);
int ItemAlignClash(struct char_data *ch, struct obj_data *obj);
int ItemEgoClash(struct char_data *ch, struct obj_data *obj, int bon);
void IncrementZoneNr(int nr); 
int IsDarkOutside(struct room_data *rp);
int GET_OBJ_EGO(struct obj_data *obj);
int GET_EGO(struct char_data *ch);
int IsInSameZone(struct char_data *ch, struct char_data *v);
int GetPlane(struct char_data *ch);
int IsOnSamePlane(struct char_data *ch, struct char_data *v);
int IsOnPmp(int room_nr);
void ImprovePreferedStat(struct char_data *ch, int num, byte onelifer);
int IsGoblinoid(struct char_data *ch);
int IsArticle(char *c);
void ChangeStrength(struct char_data *ch, int delta);
char *MovementType(struct char_data *ch, bool enter);

/* From weather.c */
 
void weather_and_time(int mode);
void another_hour(int mode);
void ChangeSeason(int month);
void weather_change();
void ChangeWeather( int change);
void GetMonth( int month);
void switch_light(byte why);
void RemoveSunBlind(struct char_data *ch);
void SunBlind(struct char_data *ch);
void SunProblemCheck (struct char_data *ch);
void GlobalSunProblemCheck(bool light);

/* From create.c */

void ChangeRoomFlags(struct room_data *rp, struct char_data *ch, char *arg, int type);
void ChangeRoomDesc(struct room_data *rp, struct char_data *ch, char *arg, int type);
void UpdateRoomMenu(struct char_data *ch);
void do_redit(struct char_data *ch, char *arg, int cmd);
void RoomEdit(struct char_data *ch, char *arg);
void ChangeRoomName(struct room_data *rp, struct char_data *ch, char *arg, int type);
void ChangeRoomType(struct room_data *rp, struct char_data *ch, char *arg, int type);
void ChangeExitDir(struct room_data *rp, struct char_data *ch, char *arg, int type);
void ChangeExitNumber(struct room_data *rp, struct char_data *ch, char *arg, int type);
void ChangeKeyNumber(struct room_data *rp, struct char_data *ch, char *arg, int type);
void AddExitToRoom(struct room_data *rp, struct char_data *ch, char *arg, int type);
void DeleteExit(struct room_data *rp, struct char_data *ch, char *arg, int type);


/* From parser.c */

void GenerateHash();
void AddNodeTail(NODE *n, int length, int radix);
NODE *SearchForNodeByName(NODE *head, char *name, int length);
void InitRadix();
NODE *FindValidCommand(char *name); 
 
/* from intrinsics.c */
void do_changeform(struct char_data *ch, char *argument, int cmd);
void MindflayerAttack(struct char_data *ch, struct char_data *v);
void do_bite(struct char_data *ch, char *argument, int cmd);
void VampireBite(struct char_data *ch, struct char_data *v);
int veggie_gain_mods(struct char_data *ch, int gain, bool msgs);
int vamp_gain_mods(struct char_data *ch, int gain, bool msgs);

/* From hoard.c */
int No_Hoard(struct char_data *ch, struct char_data *re,
                  struct obj_data *obj, struct obj_cost *cost);

/*  */
/* All Done! (yay!) */

