/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

struct StrHeapList {
   char *string;   /* the matching string */
   int  total;     /* total # of occurences */
};

struct StrHeap {
   int uniq;   /* number of uniq items in list */
   struct StrHeapList *str;   /* the list of strings and totals */
};
