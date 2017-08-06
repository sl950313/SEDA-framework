#ifndef _CONFIG_H
#define _CONFIG_H 
#include "marcos.h"
#include <vector>
#include <string>

using namespace std;

class stage_config {
private:
   string res;
   string des;

public:
   stage_config() {
      stage_config("", "");
   }
   
   stage_config(string res, string des) {
      this->res = res;
      this->des = des;
   }
   string getResource() {
      return res;
   }
   string getDestination() { 
      return des;
   }
};

class Config {
public:
   Config(const char *config_file);
   vector<stage_config > &getConfigs() {
      return config;
   }

private:
   vector<stage_config > config;
};

#endif /* _CONFIG_H */
