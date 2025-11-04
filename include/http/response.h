
#ifndef PROXY_RESPONSE_H_
#define PROXY_RESPONSE_H_

#include <string>


#include "http/status_codes.h"
#include "http/request.h"


namespace proxy_http {

void Respond(int client_fd, Request& request);

void ErrorHandler(int client_fd, Request& request);


}  // namespace proxy_http

#endif  // PROXY_RESPONSE_H_
