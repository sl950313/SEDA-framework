#include "stage.h"
#include "config.h"

stage::stage() {
   stage("");
}

stage::stage(string stage_name) {
   this->stage_name = stage_name;
}

string stage::get_stage_name() {
   return stage_name;
}

bool stage::init() {
   Config config(CONFIG_FILE);
   /*
    * 每个stage中的四个模块。
    */
   sq = new stage_queue(); 
   wp = new thread_worker_pool();
   sh = new stage_handler(sq, wp);
   sc = new stage_control(sq, wp);

   wp->start();
   sh->run();
   sc->run();

   ec = new event_core(config);
   return true;
}

bool stage::run() { 
   /*
    * 这里从event_core中启动监听。并将EPOLL_IN事件返回的事件结构中获取IElement元素, 传递给stage_queue。
    */
   int ret = ec->init();
   if (!ret) {
      LogUtil::debug("stage::run [ec->init] error");
      return false;
   }
   ec->run();
   return true;
}
