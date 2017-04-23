#ifndef _LOG_H
#define _LOG_H

#include <string>

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

private:
   int init();
   const char *log_output;
   int output_fd;
   int log_level;
};

#endif //
