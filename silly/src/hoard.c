/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement

  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

#include <stdio.h>

#include "protos.h"

#define RATIO1 1.2  /* Ratio for items that have only 1 slot */
#define RATIO2 2.2  /* Ratio for items that have 2 slots */

/* Anti-Hoarding Stuff */

int No_Hoard(struct char_data *ch, struct char_data *re,
	   struct obj_data *obj, struct obj_cost *cost) {

  char buf[MAX_INPUT_LENGTH];
  int i, misfits, flag, placed;
  long newflags;
  struct equip_fill eq;
  struct obj_data *obj2, *objbak;
  
  misfits = 0;
  i = 0;
  flag = 0;
  
  /* Initializing just to be safe */
  
  for(i=0; i<2; i++) {
    eq.head[i]=0;
    eq.body[i]=0;
    eq.arms[i]=0;
    eq.hands[i]=0;
    eq.legs[i]=0;
    eq.feet[i]=0;
    eq.waist[i]=0;
    eq.wield[i]=0;
    eq.about[i]=0;
    eq.shield[i]=0;
  }
  for(i=0; i<3; i++) {
    eq.finger[i]=0;
    eq.wrist[i]=0;
    eq.neck[i]=0;
  }
  
  obj = ch->carrying;
  
  while(obj) {
    newflags = obj->obj_flags.wear_flags;
    
    if(newflags & 1) { /* This is to strip the take out of here */
      newflags--;
    }
    if((newflags & 2) && (newflags == 2)) {
      if(!(eq.finger[0])) {
	eq.finger[0] = obj->item_number;
	eq.fingerego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.finger[1])) {
	eq.finger[1] = obj->item_number;
	eq.fingerego += GET_OBJ_EGO(obj);
      }
      else if(!(eq.finger[2])) {
	eq.finger[2] = obj->item_number;
	eq.fingerego += GET_OBJ_EGO(obj);
	if(eq.fingerego > RATIO2*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);  /* This bastard is a hoarding Mother F@#$er! */
    }
    else if((newflags & 4) && (newflags == 4)) {
      if(!(eq.neck[0])) {
	eq.neck[0] = obj->item_number;
	eq.neckego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.neck[1])) {
	eq.neck[1] = obj->item_number;
	eq.neckego += GET_OBJ_EGO(obj);
      }
      else if(!(eq.neck[2])) {
	eq.neck[2] = obj->item_number;
	eq.neckego += GET_OBJ_EGO(obj);
	if(eq.neckego > RATIO2*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 8) && (newflags == 8)) {
      if(!(eq.body[0])) {
	eq.body[0] = obj->item_number;
	eq.bodyego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.body[1])) {
	eq.body[1] = obj->item_number;
	eq.bodyego += GET_OBJ_EGO(obj);
	if(eq.bodyego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 16) && (newflags == 16)) {
      if(!(eq.head[0])) {
	eq.head[0] = obj->item_number;
	eq.headego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.head[1])) {
	eq.head[1] = obj->item_number;
	eq.headego += GET_OBJ_EGO(obj);
	if(eq.headego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 32) && (newflags == 32)) {
      if(!(eq.legs[0])) {
	eq.legs[0] = obj->item_number;
	eq.legsego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.legs[1])) {
	eq.legs[1] = obj->item_number;
	eq.legsego += GET_OBJ_EGO(obj);
	if(eq.legsego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 64) && (newflags == 64)) {
      if(!(eq.feet[0])) {
	eq.feet[0] = obj->item_number;
	eq.feetego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.feet[1])) {
	eq.feet[1] = obj->item_number;
	eq.feetego += GET_OBJ_EGO(obj);
	if(eq.feetego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 128) && (newflags == 128)) {
      if(!(eq.hands[0])) {
	eq.hands[0] = obj->item_number;
	eq.handsego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.hands[1])) {
	eq.hands[1] = obj->item_number;
	eq.handsego += GET_OBJ_EGO(obj);
	if(eq.handsego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 256) && (newflags == 256)) {
      if(!(eq.arms[0])) {
	eq.arms[0] = obj->item_number;
	eq.armsego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.arms[1])) {
	eq.arms[1] = obj->item_number;
	eq.armsego += GET_OBJ_EGO(obj);
	if(eq.armsego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 512) && (newflags == 512)) {
      if(!(eq.shield[0])) {
	eq.shield[0] = obj->item_number;
	eq.shieldego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.shield[1])) {
	eq.shield[1] = obj->item_number;
	eq.shieldego += GET_OBJ_EGO(obj);
	if(eq.shieldego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 1024) && (newflags == 1024)) {
      if(!(eq.about[0])) {
	eq.about[0] = obj->item_number;
	eq.aboutego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.about[1])) {
	eq.about[1] = obj->item_number;
	eq.aboutego += GET_OBJ_EGO(obj);
	if(eq.aboutego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 2048) && (newflags == 2048)) {
      if(!(eq.waist[0])) {
	eq.waist[0] = obj->item_number;
	eq.waistego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.waist[1])) {
	eq.waist[1] = obj->item_number;
	eq.waistego += GET_OBJ_EGO(obj);
	if(eq.waistego > RATIO1*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 4096) && (newflags == 4096)) {
      if(!(eq.wrist[0])) {
	eq.wrist[0] = obj->item_number;
	eq.wristego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.wrist[1])) {
	eq.wrist[1] = obj->item_number;
	eq.wristego += GET_OBJ_EGO(obj);
      }
      else if(!(eq.wrist[2])) {
	eq.wrist[2] = obj->item_number;
	eq.wristego += GET_OBJ_EGO(obj);
	if(eq.wristego > RATIO2*GET_EGO(ch))
	  return(1);
      }
      else
	return(1);
    }
    else if((newflags & 8192) && (newflags == 8192)) {
      if(!(eq.wield[0])) {
	eq.wield[0] = obj->item_number;
	eq.wieldego = GET_OBJ_EGO(obj);
      }
      else if(!(eq.wield[1])) {
	eq.wield[1] = obj->item_number;
	eq.wieldego += GET_OBJ_EGO(obj);
	   if(eq.wieldego > RATIO1*GET_EGO(ch))
	     return(1);
      }
      else
	return(1);
    }
    else if(newflags == 0);       /* This will clear up TAKE */
    else if(newflags == 16384);   /* This will clear up TAKE, HOLD's */
    else {
      eq.misfit[misfits] = obj->obj_flags.wear_flags;
      eq.misfitego[misfits++] = GET_OBJ_EGO(obj);
    }
    
    if((obj->obj_flags.type_flag == 15) && (flag == 0)) {
      objbak = obj;
      obj = obj->contains;
      flag = 1;
    }
    else if((obj->obj_flags.type_flag == 15) && (flag == 1)) {
      obj2 = obj;
      obj = obj->contains;
      flag = 2;
    }
    else if((obj->obj_flags.type_flag == 15) && (flag == 2)) {
      return(1);  /* Anyone burying shit in bags within bags is a hoarder by my book! */
    }
    else if(!(obj->next_content)) {
      if(flag == 2) {
	obj = obj2->next_content;
	obj2 = 0;
	flag = 1;
      }
      else if(flag == 1) {
	obj = objbak->next_content;
	objbak = 0;
	flag = 0;
      }
      else if(flag == 0) {
	obj = obj->next_content;  /* This won't pass the while.. We're Done */
      }
    }
    else {
      obj = obj->next_content;
    }
  }
  
  for(i=0; i<MAX_WEAR; i++) {
    obj = ch->equipment[i];
    while(obj) {
      newflags = obj->obj_flags.wear_flags;
      
      if(newflags & 1) { /* This is to strip the take out of here */
	newflags--;
      }
      if((newflags & 2) && (newflags == 2)) {
	if(!(eq.finger[0])) {
	  eq.finger[0] = obj->item_number;
	  eq.fingerego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.finger[1])) {
	  eq.finger[1] = obj->item_number;
	  eq.fingerego += GET_OBJ_EGO(obj);
	}
	else if(!(eq.finger[2])) {
	  eq.finger[2] = obj->item_number;
	  eq.fingerego += GET_OBJ_EGO(obj);
	  if(eq.fingerego > RATIO2*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);  /* This bastard is a hoarding Mother F@#$er! */
      }
      else if((newflags & 4) && (newflags == 4)) {
	if(!(eq.neck[0])) {
	  eq.neck[0] = obj->item_number;
	  eq.neckego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.neck[1])) {
	  eq.neck[1] = obj->item_number;
	  eq.neckego += GET_OBJ_EGO(obj);
	}
	else if(!(eq.neck[2])) {
	  eq.neck[2] = obj->item_number;
	  eq.neckego += GET_OBJ_EGO(obj);
	  if(eq.neckego > RATIO2*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 8) && (newflags == 8)) {
	if(!(eq.body[0])) {
	  eq.body[0] = obj->item_number;
	  eq.bodyego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.body[1])) {
	  eq.body[1] = obj->item_number;
	  eq.bodyego += GET_OBJ_EGO(obj);
	  if(eq.bodyego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 16) && (newflags == 16)) {
	if(!(eq.head[0])) {
	  eq.head[0] = obj->item_number;
	  eq.headego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.head[1])) {
	  eq.head[1] = obj->item_number;
	  eq.headego += GET_OBJ_EGO(obj);
	  if(eq.headego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 32) && (newflags == 32)) {
	if(!(eq.legs[0])) {
	  eq.legs[0] = obj->item_number;
	  eq.legsego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.legs[1])) {
	  eq.legs[1] = obj->item_number;
	  eq.legsego += GET_OBJ_EGO(obj);
	  if(eq.legsego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 64) && (newflags == 64)) {
	if(!(eq.feet[0])) {
	  eq.feet[0] = obj->item_number;
	  eq.feetego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.feet[1])) {
	  eq.feet[1] = obj->item_number;
	  eq.feetego += GET_OBJ_EGO(obj);
	  if(eq.feetego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 128) && (newflags == 128)) {
	if(!(eq.hands[0])) {
	  eq.hands[0] = obj->item_number;
	  eq.handsego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.hands[1])) {
	  eq.hands[1] = obj->item_number;
	  eq.handsego += GET_OBJ_EGO(obj);
	  if(eq.handsego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 256) && (newflags == 256)) {
	if(!(eq.arms[0])) {
	  eq.arms[0] = obj->item_number;
	  eq.armsego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.arms[1])) {
	  eq.arms[1] = obj->item_number;
	  eq.armsego += GET_OBJ_EGO(obj);
	  if(eq.armsego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 512) && (newflags == 512)) {
	if(!(eq.shield[0])) {
	  eq.shield[0] = obj->item_number;
	  eq.shieldego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.shield[1])) {
	  eq.shield[1] = obj->item_number;
	  eq.shieldego += GET_OBJ_EGO(obj);
	  if(eq.shieldego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 1024) && (newflags == 1024)) {
	if(!(eq.about[0])) {
	  eq.about[0] = obj->item_number;
	  eq.aboutego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.about[1])) {
	  eq.about[1] = obj->item_number;
	  eq.aboutego += GET_OBJ_EGO(obj);
	  if(eq.aboutego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 2048) && (newflags == 2048)) {
	if(!(eq.waist[0])) {
	  eq.waist[0] = obj->item_number;
	  eq.waistego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.waist[1])) {
	  eq.waist[1] = obj->item_number;
	  eq.waistego += GET_OBJ_EGO(obj);
	  if(eq.waistego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 4096) && (newflags == 4096)) {
	if(!(eq.wrist[0])) {
	  eq.wrist[0] = obj->item_number;
	  eq.wristego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.wrist[1])) {
	  eq.wrist[1] = obj->item_number;
	  eq.wristego += GET_OBJ_EGO(obj);
	}
	else if(!(eq.wrist[2])) {
	  eq.wrist[2] = obj->item_number;
	  eq.wristego += GET_OBJ_EGO(obj);
	  if(eq.wristego > RATIO2*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if((newflags & 8192) && (newflags == 8192)) {
	if(!(eq.wield[0])) {
	  eq.wield[0] = obj->item_number;
	  eq.wieldego = GET_OBJ_EGO(obj);
	}
	else if(!(eq.wield[1])) {
	  eq.wield[1] = obj->item_number;
	  eq.wieldego += GET_OBJ_EGO(obj);
	  if(eq.wieldego > RATIO1*GET_EGO(ch))
	    return(1);
	}
	else
	  return(1);
      }
      else if(newflags == 0);
      else if(newflags == 16384);
      else {
	eq.misfit[misfits] = obj->obj_flags.wear_flags;
	eq.misfitego[misfits++] = GET_OBJ_EGO(obj);
      }
      
      if((obj->obj_flags.type_flag == 15) && (flag == 0)) {
	objbak = obj;
	obj = obj->contains;
	flag = 1;
      }
      else if((obj->obj_flags.type_flag == 15) && (flag == 1)) {
	obj2 = obj;
	obj = obj->contains;
	flag = 2;
      }
      else if((obj->obj_flags.type_flag == 15) && (flag == 2)) {
	return(1);  /* Anyone burying shit in bags within bags is a hoarder by my book! */
      }
      else if(!(obj->next_content)) {
	if(flag == 2) {
	  obj = obj2->next_content;
	  obj2 = 0;
	  flag = 1;
	}
	else if(flag == 1) {
	  obj = objbak->next_content;
	  objbak = 0;
	  flag = 0;
	}
	else if(flag == 0) {
	  obj = obj->next_content; /* This won't pass the while.. We're Done */
	}
      }
      else {
	obj = obj->next_content;
      }
    }
  }
  placed = 0;
  
  for(i=0; i<misfits; i++) {   /* Now to see if the multi-wears fit anywhere */
    
    newflags = eq.misfit[i];
    
    if((newflags & 2) && !(placed)) {
      if(!(eq.finger[0])) {
	eq.finger[0] = eq.misfit[i];
	eq.fingerego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.finger[1])) {
	eq.finger[1] = eq.misfit[i];
	eq.fingerego += eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.finger[2])) {
	eq.finger[2] = eq.misfit[i];
	eq.fingerego += eq.misfitego[i];
	placed = 1;
	if(eq.fingerego > RATIO2*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 4) && !(placed)) {
      if(!(eq.neck[0])) {
	eq.neck[0] = eq.misfit[i];
	eq.neckego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.neck[1])) {
	eq.neck[1] = eq.misfit[i];
	eq.neckego += eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.neck[2])) {
	eq.neck[2] = eq.misfit[i];
	eq.neckego += eq.misfitego[i];
	placed = 1;
	if(eq.neckego > RATIO2*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 8) && !(placed)) {
      if(!(eq.body[0])) {
	eq.body[0] = eq.misfit[i];
	eq.bodyego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.body[1])) {
	eq.body[1] = eq.misfit[i];
	eq.bodyego += eq.misfitego[i];
	placed = 1;
	if(eq.bodyego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 16) && !(placed)) {
      if(!(eq.head[0])) {
	eq.head[0] = eq.misfit[i];
	eq.headego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.head[1])) {
	eq.head[1] = eq.misfit[i];
	eq.headego += eq.misfitego[i];
	placed = 1;
	if(eq.headego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 32) && !(placed)) {
      if(!(eq.legs[0])) {
	eq.legs[0] = eq.misfit[i];
	eq.legsego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.legs[1])) {
	eq.legs[1] = eq.misfit[i];
	eq.legsego += eq.misfitego[i];
	placed = 1;
	if(eq.legsego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 64) && !(placed)) {
      if(!(eq.feet[0])) {
	eq.feet[0] = eq.misfit[i];
	eq.feetego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.feet[1])) {
	eq.feet[1] = eq.misfit[i];
	eq.feetego += eq.misfitego[i];
	placed = 1;
	if(eq.feetego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 128) && !(placed)) {
      if(!(eq.hands[0])) {
	eq.hands[0] = eq.misfit[i];
	eq.handsego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.hands[1])) {
	eq.hands[1] = eq.misfit[i];
	eq.handsego += eq.misfitego[i];
	placed = 1;
	if(eq.handsego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 256) && !(placed)) {
      if(!(eq.arms[0])) {
	eq.arms[0] = eq.misfit[i];
	eq.armsego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.arms[1])) {
	eq.arms[1] = eq.misfit[i];
	eq.armsego += eq.misfitego[i];
	placed = 1;
	if(eq.armsego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 512) && !(placed)) {
      if(!(eq.shield[0])) {
	eq.shield[0] = eq.misfit[i];
	eq.shieldego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.shield[1])) {
	eq.shield[1] = eq.misfit[i];
	eq.shieldego += eq.misfitego[i];
	placed = 1;
	if(eq.shieldego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 1024) && !(placed)) {
      if(!(eq.about[0])) {
	eq.about[0] = eq.misfit[i];
	eq.aboutego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.about[1])) {
	eq.about[1] = eq.misfit[i];
	eq.aboutego += eq.misfitego[i];
	placed = 1;
	if(eq.aboutego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 2048) && !(placed)) {
      if(!(eq.waist[0])) {
	eq.waist[0] = eq.misfit[i];
	eq.waistego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.waist[1])) {
	eq.waist[1] = eq.misfit[i];
	eq.waistego += eq.misfitego[i];
	placed = 1;
	if(eq.waistego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 4096) && !(placed)) {
      if(!(eq.wrist[0])) {
	eq.wrist[0] = eq.misfit[i];
	eq.wristego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.wrist[1])) {
	eq.wrist[1] = eq.misfit[i];
	eq.wristego += eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.wrist[2])) {
	eq.wrist[2] = eq.misfit[i];
	eq.wristego += eq.misfitego[i];
	placed = 1;
	if(eq.wristego > RATIO2*GET_EGO(ch))
	  return(1);
      }
    }
    if((newflags & 8192) && !(placed)) {
      if(!(eq.wield[0])) {
	eq.wield[0] = eq.misfit[i];
	eq.wieldego = eq.misfitego[i];
	placed = 1;
      }
      else if(!(eq.wield[1])) {
	eq.wield[1] = eq.misfit[i];
	eq.wieldego += eq.misfitego[i];
	placed = 1;
	if(eq.wieldego > RATIO1*GET_EGO(ch))
	  return(1);
      }
    }
    if(!(placed)) {
      return(1);
    }
  }
  
  return(0);  /* Passed the checks */
}


