#include "connect.h"

void connection::set_user_info_to_handle() {
   std::map<std::string, std::string> content = req->get_map_contents();
   hi->u_info->username = content["username"];
   hi->u_info->password = content["password"];
}
