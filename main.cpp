#include "stage.h"
#include <vector>
#include <string>
using namespace std;

void *print(void *arg) {
   //printf("main [print] : %s", (char *)arg);
   LogUtil::debug("main [print] : %s", (char *)arg);
   return arg;
}

int main(int argc, char **argv) { 
   LogUtil::init();
   LogUtil::debug("main.cpp : [main] config success");
   Config config(CONFIG_FILE);
   stage s1;
   vector<string> resources, des;
   resources.push_back("tcp://localhost:5666");
   s1.setResources(resources);
   des.push_back("tcp://*:5667");
   s1.setDestination(des);
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
