// related header
#include "http/request.h"

// system header
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

// C++ standard library
#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <fstream>
#include <mutex>
// #include <iostream>

// project header
#include "server.h"
#include "strutil.h"
#include "http/status_codes.h"

namespace proxy_http {

static const std::set<std::string> supported_methods = {"GET"};
static const std::set<std::string> supported_versions = {"HTTP/1.1", "HTTP/1.0"};

Request::Request() : method(""), path(""), version("HTTP/1.1"), headers(), error_message_(StatusCode::OK) {}

Request Get(int client_fd, proxy_server::Server &server) {
  std::string request_str;
  Request request;
  char buf[4096];

  while (request_str.find("\r\n\r\n") == std::string::npos) {
    ssize_t bytes_read = recv(client_fd, buf, sizeof(buf), 0);
    if (bytes_read <= 0) return Request().SetError(StatusCode::BAD_REQUEST);
    request_str.append(buf, bytes_read);
    //  // Debug: Print received chunk
    //   std::cout << "--- Received chunk (" << bytes_read << " bytes) ---" << std::endl;
    //   std::cout.write(buf, bytes_read);
    //   std::cout << "--------------------------" << std::endl;
  }
  request = Parse(request_str, server.block_list);
  if (request.GetStatus() != StatusCode::OK) {
    return request;
  }
  else {
    // get the reponse from web server for request_str
    std::string cache_key = proxy_util::cryptic_hash(request.path);
    bool cache_hit = false;
    // Check cache
    {
      std::lock_guard<std::mutex> lock(server.cache_mutex);
      cache_hit = server.cache.find(cache_key) != server.cache.end();
    }
    if (cache_hit) {
        auto& [cached_path, timestamp] = server.cache[cache_key];
        time_t current_time = time(nullptr);
        if (difftime(current_time, timestamp) > server.ttl) {
            // Cache expired
            server.cache[cache_key].second = current_time;
            Caching(request, server, request_str);
            return request;
        }
    }
    else {
      Caching(request, server, request_str);
    return request;
    }
  }
}

Request Parse(const std::string& raw_request, const std::vector<std::string>& block_list) {
  Request request;
  std::istringstream iss(raw_request);
  std::string line;

  // Parse request line
  std::getline(iss, line);
  std::istringstream line_iss(line);
  line_iss >> request.method >> request.path >> request.version;
  // Parse headers
  while (std::getline(iss, line) && !proxy_util::Trim(line).empty()) {
    std::string key, value;
    std::size_t colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
        key = line.substr(0, colon_pos);
        value = line.substr(colon_pos + 1);
        request.headers[proxy_util::Trim(key)] = proxy_util::Trim(value);
    }
  }

  // 400 Bad Request
  if (line_iss.fail() || request.method!= "GET" || request.path.empty() || request.version.empty()) {
    return request.SetError(StatusCode::BAD_REQUEST);
  }
  // 403 Forbidden
  std::string host = request.headers["Host"];
  if (std::find(block_list.begin(), block_list.end(), host) != block_list.end()) {
    return request.SetError(StatusCode::FORBIDDEN);
  }

  return request;
}

Request& Request::SetError(StatusCode code) {
  error_message_ = code;
  return *this;
}

int Caching(Request& request, proxy_server::Server& server, const std::string& request_str) {
  std::string cache_key = proxy_util::cryptic_hash(request.path);
  struct addrinfo hints{}, *res;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(request.headers["Host"].c_str(), "80", &hints, &res) != 0) {
      request.SetError(StatusCode::NOT_FOUND);
      return -1;
  }
  int web_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (web_fd == -1) { 
      freeaddrinfo(res);
      request.SetError(StatusCode::NOT_FOUND);
      return -1;
  }
  if (connect(web_fd, res->ai_addr, res->ai_addrlen) == -1) {
      freeaddrinfo(res);
      request.SetError(StatusCode::NOT_FOUND);
      return -1;
  }
  ::write(web_fd, request_str.c_str(), request_str.size());

  //read response
  std::string response;
  char buf[8192];
  ssize_t n;
  while ((n = read(web_fd, buf, sizeof(buf))) > 0) {
      response.append(buf, n);
  }
  close(web_fd);
  freeaddrinfo(res);
  
  std::string cache_path = "cache/" + cache_key;
  std::ofstream cache_file(cache_path, std::ios::binary);
  cache_file << response;
  cache_file.close();
  {
      std::lock_guard<std::mutex> lock(server.cache_mutex);
      server.cache[cache_key] = {cache_path, time(nullptr)};
  }
  return 0;
}

}  // namespace proxy_http
