#include "utiltools.h"

std::list<char *> split_string(char *src, char *tail, char token) {
   std::list<char *> _l;
   char *t_src = src;
   while (t_src != tail) {
      if (*t_src == token) {
         
      }
   }
   return _l;
}

unsigned int utiltool::hash(const char *_str, int len) {
   unsigned int hash = 0;

   char *str = (char *)_str;
   while (len--) {
      // equivalent to: hash = 65599*hash + (*str++);
      hash = (*str++) + (hash << 6) + (hash << 16) - hash;
   }

   return (hash & 0x7FFFFFFF);
}
