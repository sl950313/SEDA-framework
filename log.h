#ifndef _LOG_H
#define _LOG_H

#include <string>
#include <stdlib.h>
#include <stdarg.h>

enum {
   ERROR = 1,
   INFO = 2,
   DEBUG = 3
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

private:
   int init();
   void make_message(const char *fmt, ...);
   const char *log_output;
   int output_fd;
   int log_level;
};

#endif //
