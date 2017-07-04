#include <stdio.h>

enum hello {
   HELLO1,
   HELLO2
};

class ha {
public:
   hello h1;
};

int main() {
   hello h1;
   h1 = HELLO1;
   printf("h1 : %d\n", h1);

   ha a1;
   a1.h1 = HELLO1;
   printf("h1 : %d\n", a1.h1);
}
