#include "stage.h"
#include <vector>
#include <string>
using namespace std;

void *print(void *arg) {
   printf("main [print] : %s", (char *)arg);
   return NULL;
}

int main(int argc, char **argv) { 
   Config config(CONFIG_FILE);
   stage s1;
   s1.init(config);
   vector<string> resources;
   resources.push_back("tcp://localhost:5666");
   s1.setResources(resources);
   Function fun;
   fun.setFunction(print);
   s1.getHandler()->setHandler(fun);
   s1.run();
   return 0;
}
