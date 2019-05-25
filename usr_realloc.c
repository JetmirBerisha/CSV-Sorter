#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "usr_realloc.h"

char* usr_realloc(char* ptr)
{
  int original_size = strlen(ptr);
  int new_size = strlen(ptr)*2;
  char* holder = (char*)malloc(new_size*sizeof(char));
  if(holder == NULL)
  {
    printf("Error File:%s, Line:%s\n",__FILE__, __LINE__);
    return NULL;
  }
  holder = memcpy(holder, ptr, original_size);
  *ptr = (char) -99;
  free(ptr);
  while(*ptr == (char) -99){}; //waits until free() finishes before retur$
  return holder;
}
