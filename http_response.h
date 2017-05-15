#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

class http_response {
public:
   http_response() {}
   ~http_response() {}

   bool write_head();
   bool write_content();
};

#endif //
