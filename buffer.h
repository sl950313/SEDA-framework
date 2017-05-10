#ifndef _BUFFER_H
#define _BUFFER_H

#include "marcos.h"

class buffer {
public:
   buffer();
   ~buffer();
   void push_back(char *buf, int len);
   unsigned int length();
   char *ptr();

private:
   bool expand(); 
   char *_ptr;
   int _size;
   int _len;
};

#endif //
