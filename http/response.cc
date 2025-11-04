
// related header
#include "http/response.h"

// system header
#include <sys/types.h>

// C++ standard library
#include <string>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>

#include <iostream>

// project header
#include "http/status_codes.h"
#include "server.h"
#include "http/request.h"
#include "strutil.h"

namespace proxy_http {

void Respond(int client_fd, Request& request) {
  // serve file
  if (request.GetStatus() != StatusCode::OK) {
    return ErrorHandler(client_fd, request);
  }
  else {
    std::string cache_key = proxy_util::cryptic_hash(request.path);
    std::string filepath = "cache/" + cache_key;
    // Check cache
    if (access(filepath.c_str(), F_OK) == 0) {
      std::ifstream ifs(filepath,  std::ios::binary);
      constexpr size_t BUF_SIZE = 8192;
      char buffer[BUF_SIZE];
      while (ifs.good()) {
        ifs.read(buffer, BUF_SIZE);
        std::streamsize bytes_read = ifs.gcount();
        if (bytes_read > 0) {
          ssize_t bytes_sent = 0;
          while (bytes_sent < bytes_read) {
            ssize_t n = write(client_fd, buffer + bytes_sent, bytes_read - bytes_sent);
            if (n < 0) {
              ifs.close();
            }
            bytes_sent += n;
          }
        }
      }
    ifs.close();
    }
  }
}

void ErrorHandler(int client_fd, Request& request) {
  std::string status_text;
  switch (request.GetStatus()) {
    case StatusCode::BAD_REQUEST:
      status_text = "400 Bad Request";
      break;
    case StatusCode::FORBIDDEN:
      status_text = "403 Forbidden";
      break;
    case StatusCode::NOT_FOUND:
      status_text = "404 Not Found";
      break;
    case StatusCode::OK:
      status_text = "200 OK";
      break;
    
  }

  std::ostringstream resp;
  resp << request.version << " " << status_text << "\r\n"
       << "Content-Type: text/plain\r\n"
       << "Content-Length: " << std::to_string(status_text.size() + 1) << "\r\n"
       << "\r\n"
       << status_text << "\n";
  std::string response = resp.str();
    // std::cout << resp.str() << std::endl; // Debug: Print the full response
    ::write(client_fd, response.c_str(), response.size());
}

}  // namespace ns_http
