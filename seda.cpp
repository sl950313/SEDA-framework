#include "seda.h"
#include "log.h"

stage *SEDA::create_stage(string stage_name) { 
   stage *sg = new stage(stage_name);
   map<string, stage *>::iterator it = stages.find(stage_name);
   if (it != stages.end()) { 
      LogUtil::debug("The stage : %s is already existed", stage_name.c_str());
      return NULL;
   }
   stages[stage_name] = sg;
   stages_v.push_back(sg);
   return sg;
}

bool SEDA::add_msg_edge(stage *from, stage *to, string msg) { 
   stage_message *sm = new stage_message(msg);
   if (msgExist2Stages(from, to, sm)) {
      LogUtil::debug("The msg : %s is already exsited from the stage : %s to stage : %s", msg.c_str(), from->get_stage_name().c_str(), to->get_stage_name().c_str());
      return false;
   }
   addSM(from, to, sm);
   return true;
}

bool SEDA::msgExist2Stages(stage *from, stage *to, stage_message *sm) {
   map<stage_message *, pair<stage *, stage *> >::iterator it = msg_relat.find(sm);
   if (it != msg_relat.end()) {
      if ((it->second.first == from) && it->second.second == to) {
         return false; 
      }
   }
   return true;
}

void SEDA::addSM(stage *from, stage *to, stage_message *sm) {
   msg_relat.insert(pair<stage_message *, pair<stage *, stage *> >(sm, pair<stage *, stage *>(from, to)));
}

bool SEDA::run() {
   for (size_t i = 0; i < stages_v.size(); ++i) {
      if (!stages_v[i]->run()) { 
         LogUtil::debug("stage : %s run fail", stages_v[i]->get_stage_name().c_str());
         return false;
      }
   }
   return true;
}
