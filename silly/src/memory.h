 
 
struct string_entry {
	char *string;       /* Pointer to the actual data      */
	int   count;        /* How many strings are using it   */
	struct string_entry *next;
};
 
