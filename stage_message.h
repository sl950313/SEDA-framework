#ifndef _STAGE_MESSAGE_H
#define _STAGE_MESSAGE_H 
#include <string>

class stage_message {
public:
   stage_message(std::string msg_name);

private:
   std::string msg_name;
};

#endif /* _STAGE_MESSAGE_H */
