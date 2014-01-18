/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/

struct hash_link {
  int	key;
  struct hash_link *next;
  void	*data;
};

struct hash_header {
  int	rec_size;
  int	table_size;
  int	*keylist, klistsize, klistlen; /* this is really lame,
					  AMAZINGLY lame */
  struct hash_link	**buckets;
};


#define WORLD_SIZE 30000
