#ifndef _IELEMENT_H
#define _IELEMENT_H 
#include <string.h>

class IElement {
public:
   IElement() {
      memset(element, 0, 256);
   }
   IElement(char *ele, int len) {
      memset(element, 0, 256);
      strncpy(element, ele, len);
      _len = len;
   }
   void *getElement() {
      return element;
   }

   int length() {
      return _len;
   }
private:
   char element[256];
   int _len;
};

#endif /* _IELEMENT_H */
