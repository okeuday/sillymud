void die(struct char_data *ch)
{
  struct char_data *pers;
  int i,temp;
  char buf[80];
  int fraction;

  /* need at least 1/fraction worth of exp for the minimum needed for */
  /* the pc's current level, or else you lose a level.  If all three  */
  /* classes are lacking in exp, you lose one level in each class. */

  fraction = 16;
  if (IS_NPC(ch) && (IS_SET(ch->specials.act, ACT_POLYSELF))) {
    /*
     *   take char from storage, to room
     */
    if (ch->desc) {  /* hmmm */
       pers = ch->desc->original;
       char_from_room(pers);
       char_to_room(pers, ch->in_room);
       SwitchStuff(ch, pers);
       extract_char(ch);
       ch = pers;
     } else {
       /* we don't know who the original is.  Gets away with it, i guess*/
     }
  }

#if LEVEL_LOSS

  for(i=0;i<MAX_CLASS;i++) {
    if (GET_LEVEL(ch,i) > 1) {
      if (GET_EXP(ch) < (titles[i][GET_LEVEL(ch, class)].exp/fraction)) {
	tmp = GET_MAX_HIT(ch)/GetMaxLevel(ch);
	GET_MAX_HIT(ch) -= tmp;
	GET_LEVEL(ch, i) -= 1;
	ch->specials.spells_to_learn -= MAX(1, MAX(2, wis_app[GET_RWIS(ch)].bonus)/HowManyClasses(ch));
      }
    }
  }
#endif

#if NEWEXP  
  if (GetMaxLevel(ch) > 15)
    gain_exp(ch, -GET_EXP(ch)/2);
  else if (GetMaxLevel(ch) > 10)
    gain_exp(ch, -GET_EXP(ch)/3);
  else if (GetMaxLevel(ch) > 5)
    gain_exp(ch, -GET_EXP(ch)/4);
  else 
    gain_exp(ch, -GET_EXP(ch)/5);
#else	     
  gain_exp(ch, -GET_EXP(ch)/2);
#endif


	
#if LEVEL_LOSS

  /* warn people if their next death will result in a level loss */
  for(i=0;i<MAX_CLASS;i++) {
    if (GET_LEVEL(ch,i) > 1) {
      if (GET_EXP(ch) < (titles[i][GET_LEVEL(ch, class)].exp/fraction)) {
	send_to_char("WARNING WARNING WARNING WARNING WARNING WARNING\n\r",
		     ch);
	send_to_char("Your next death will result in the loss of a level,\n\r",
		     ch);
	sprintf(buf,"unless you get at least %d more exp points.\n\r",
		(titles[i][GET_LEVEL(ch, class)].exp/fraction) - GET_EXP(ch));
	send_to_char(buf,ch);
      }
    }
  }
#endif

  /*
   **      Set the talk[2] to be TRUE, i.e. DEAD
   */
  ch->player.talks[2] = 1;  /* char is dead */

  DeleteHatreds(ch);
  DeleteFears(ch);
  raw_kill(ch);
}
