
#include <linux/string.h> // 或 string.h  
#include <stdlib.h>  
#include <stdio.h>  
  
int main(int argc, const char *argv[]) {  
  char* const delim = "/";  
  char str[] = "some/split/string";  
  
  char *token, *cur = str;  
  while ((token = strsep(&cur, delim))) {  
      printf("%s\n", token);  
    }  
  return 0;  
} 
