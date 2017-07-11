#ifndef _SEDA_H
#define _SEDA_H 
#include <vector>
#include <map>
#include "stage.h"
#include "config.h"
#include "stage_message.h"

using namespace std;

class SEDA { 
public:
   Config config_sys(string config_filename);
   void init();
   stage *create_stage(string stage_name);
   bool add_msg_edge(stage *from, stage *to, string msg);
   bool run();

private:
   void stop();
   vector<stage *> stages_v;
   map<string, stage *> stages;
   map<string, stage_message *> messages;
   map<stage_message *, pair<stage *, stage *> > msg_relat;

   bool msgExist2Stages(stage *from, stage *to, stage_message *sm);
   void addSM(stage *from, stage *to, stage_message *sm);
};

#endif /* _SEDA_H */
