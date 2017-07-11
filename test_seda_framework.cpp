#include "seda.h"

int main() { 
   SEDA *seda = new SEDA();
   seda->init();
   stage *s1 = seda->create_stage("accept");
   stage *s2 = seda->create_stage("process");
   stage *s3 = seda->create_stage("log");
   stage *s4 = seda->create_stage("write_back");

   seda->add_msg_edge(s1, s2, new stage_message("accept"));
   seda->add_msg_edge(s1, s3, new stage_message("accept"));
   seda->add_msg_edge(s2, s3, new stage_message("process"));
   seda->add_msg_edge(s2, s4, new stage_message("process"));
   seda->add_msg_edge(s4, s3, new stage_message("write_back"));
   seda->run();
   delete seda;
   return 0;
}
