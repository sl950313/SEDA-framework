class sedaObject {
private:
   unsigned type:4;
   unsigned encoding:4;
   int refcount;

public:
   sedaObject();
   virtual ~sedaObject() = 0;
   void decrRefCount();
   void incrRefCount();
   void resetRefcount();
   void *ptr;
};

