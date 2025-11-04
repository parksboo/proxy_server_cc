
#ifndef PROXY_REQUEST_H_
#define PROXY_REQUEST_H_

#include <string>
#include <vector>
#include <map>

#include "http/status_codes.h"
#include "server.h"

namespace proxy_http {

class Request {
 public:
    std::string method;
    std::string path;
    std::string version;
	std::string body;
	std::string content_type;
    std::map<std::string, std::string> headers;
	
	Request();
	
	std::string GetFileExtension();
	Request& SetError(StatusCode code);
	StatusCode GetStatus() const { return error_message_; }

 private:
	StatusCode error_message_;

	
};

Request Get(int client_fd, proxy_server::Server &server);
Request Parse(const std::string& raw, const std::vector<std::string>& block_list);
int Caching(Request& request, proxy_server::Server& server, const std::string& request_str);


}  // namespace proxy_http

#endif  // PROXY_REQUEST_H_
