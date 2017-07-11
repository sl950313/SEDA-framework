#ifndef _IELEMENT_H
#define _IELEMENT_H 

class IElement {
public:
   IElement(void *ele) : element(ele) {}
   void *getElement();
private:
   void *element;
};

#endif /* _IELEMENT_H */
