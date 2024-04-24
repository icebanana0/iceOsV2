#ifndef STRING
#define STRING

int strlen(char *str);
void reverse(char *s);
int strcmp(char *str1,char *str2);
void strcpy(char* buf_to,char* buf_from);
void memcpy(char *source, char *dest, int nbytes);
char *strchr(const char *s, int c);
#endif  
