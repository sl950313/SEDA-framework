#ifndef _LOG_H
#define _LOG_H

#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include "job_queue.h"
#include "marcos.h"

enum {
   ERROR = 1,
   INFO = 2,
   DEBUG = 3
};

struct async_log_content  {
   char log_content[LOG_LEN];
};

class _log {
public:
   _log();
   _log(std::string _log_output, int _log_level);
   void error(std::string _error);
   void debug(std::string _debug);
   void info(std::string _info);
   void _error(const char *fmt, ...);
   void _debug(const char *fmt, ...);
   void _info(const char *fmt, ...);
   bool set_async(bool async);

private:
   int init();
   void make_message(const char *fmt, ...);
   const char *log_output;
   int output_fd;
   int log_level;
   void write_log(std::string log);

   /*
    * for async logging
    */
   pthread_mutex_t async_lock;
   int is_async; 
   pthread_t log_pid;
   task_queue *tq;
   static void *log_from_queue(void *);

};

#endif //
