#ifndef _HANDLE_INFO_H
#define _HANDLE_INFO_H 
#include <string>

struct user_info {
   std::string username;
   std::string password;
};

class handle_info {
public:
   handle_info() : u_info(new user_info()) {}
   ~handle_info();

   user_info *u_info;
};

#endif //
