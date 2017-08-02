#include "stage.h"
#include <vector>
#include <string>
using namespace std;

void *print(void *arg) {
   //printf("main [print] : %s", (char *)arg);
   LogUtil::debug("main [print] : %s", (char *)arg);
   return NULL;
}

int main(int argc, char **argv) { 
   LogUtil::init();
   LogUtil::debug("main.cpp : [main] config success");
   Config config(CONFIG_FILE);
   stage s1;
   vector<string> resources;
   resources.push_back("tcp://localhost:5666");
   s1.setResources(resources);
   s1.init(config);
   LogUtil::debug("main : [main] stage s1 init success");
   Function fun;
   fun.setFunction(print);
   s1.getHandler()->setHandler(fun);
   LogUtil::debug("main : all initial over");
   s1.run();
   //s1.loop();
   while(1) {}
   return 0;
}
