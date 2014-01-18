void spell_feeblemind(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  int t,i;

  if (!saves_spell(victim, SAVING_SPELL)) {

/* eld - I took the liberty of adding this little dandy..  In my opinion,  */
/*       this spell should not be accumulative.                            */

    if(affected_by_spell(victim, SPELL_FEEBLEMIND)) {
       send_to_char("They are already dumb enough as it is!\n\r", ch);
       return;
    } 

    send_to_char("You feel really really dumb\n\r", victim);

    af.type      = SPELL_FEEBLEMIND;
    af.duration  = 24;
    af.modifier  = -5;
    af.location  = APPLY_INT;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    af.type      = SPELL_FEEBLEMIND;
    af.duration  = 24;
    af.modifier  = 70;
    af.location  = APPLY_SPELLFAIL;
    af.bitvector = 0;
    affect_to_char(victim, &af);

      /*
      last, but certainly not least
      */

    if (!victim->skills)
      return;

    t = number(1,100);

    while (1) {
      for (i=0;i<MAX_SKILLS;i++) {
	if (victim->skills[i].learned)
	  t--;
	if (t==0) {
	  victim->skills[i].learned = 0;
	  victim->skills[i].flags = 0;
	  break;
	}

/* eld - what happens if you get outside the for loop?  Yer screwed...  */
/*       this fixes it by giving the function something to do (return)  */

      }
      return;
    }
  }
}

