#ifndef _IELEMENT_H
#define _IELEMENT_H 
#include <string.h>

class IElement {
public:
   IElement(char *ele) {
      memset(element, 0, 256);
      strncpy(element, ele, strlen(ele));
   }
   void *getElement() {
      return element;
   }
private:
   char element[256];
};

#endif /* _IELEMENT_H */
