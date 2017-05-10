#include "buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

buffer::buffer() {
   _ptr = (char *)calloc(1, BUFSIZE);
   _size = BUFSIZE;
   _len = 0;
}

buffer::~buffer() {
   free(_ptr);
   _ptr = NULL;
}

unsigned int buffer::length() {
   return _len;
}

char *buffer::ptr() {
   return _ptr;
}

void buffer::push_back(char *buf, int len) {
   if (len + _len > 2 * _size / 3) {
      if (!expand()) {
         // log error.
         printf("expand error\n");
         return ;
      }
   }
   memcpy(_ptr + _len, buf, len);
}

bool buffer::expand() {
   char *new_ptr = (char *)calloc(1, 2 * _size);
   _size *= 2;
   memcpy(new_ptr, _ptr, _len);
   free(_ptr);
   _ptr = new_ptr;
   return true;
}
