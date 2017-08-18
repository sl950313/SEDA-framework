#ifndef _STAGE_H
#define _STAGE_H
#include <string> 
#include <map>
#include "stage_queue.h"
#include "stage_handler.h"
#include "worker_pool.h"
#include "stage_control.h"
#include "event_core.h"
#include "log.h"
#include "config.h"
#include "receiver.h"

using namespace std;

class stage { 
public:
   stage();
   stage(string stage_name);
   string get_stage_name();
   bool init(Config &config);
   bool run();
   bool setHandler(stage_handler *sh);
   void setResources(vector<string> &res) {
      this->resources = res;
   }
   void setDestination(string &des) {
      this->des = des;
   }
   //void loop();

   stage_handler *getHandler() {
      return sh;
   }

   void setId(int id) {
      this->stage_id = id;
   }
   void setStageName(string &name) {
      this->stage_name = name;
   }

   void static setPersistData(string key, void *data);
   static void *getPersistData(string key);
   static map<string, void *> persistData;

private:
   string stage_name;
   vector<string> resources;
   string des;
   int stage_id;

   receiver *rc;
   stage_queue *sq;
   stage_handler *sh;
   worker_pool *wp;
   stage_control *sc;
   event_core *ec;

};

#endif /* _STAGE_H */
