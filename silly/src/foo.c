
/* Original fread_string was a memory leaking piece of garbage... -DM */
char *fread_string(FILE *f1)
{
 char buf[MAX_STRING_LENGTH];
 int i = 0, tmp;
 
 buf[0] = '\0';
 
 while(i < MAX_STRING_LENGTH - 2) {
   tmp = fgetc(f1);
   if(!tmp) {
     perror("Fread_string");
     assert(0);
   }
   
   if(tmp == '~') {
     break;
   }
 
   buf[i++] = (char)tmp;
   if (buf[i-1] == '\n')
     buf[i++] = '\r';
 }

 if(i == MAX_STRING_LENGTH - 3) { /* We filled the buffer */
    buf[i] = '\0';
    log("File too long (fread_string).");
    while(tmp = fgetc(f1))
        if(tmp == '~')
           break;
  }
 
 else
    buf[i] = '\0';
 
 fgetc(f1);
 
 return((char *)strdup(buf));
}
 
