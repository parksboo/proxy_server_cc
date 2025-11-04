
// related header
#include "worker_thread.h"

// system header
#include <sys/socket.h>
#include <poll.h>

// C++ standard library
#include <thread>


// project header
#include "http/request.h"
#include "http/response.h"

namespace proxy_server {

void Worker::Run() {

  proxy_http::Request request = proxy_http::Get(client_fd_, server_);
  proxy_http::Respond(client_fd_, request);
  //prefetching : links, webserver fd
  if (request.cache_update) {
    proxy_http::PrefetchLinks(request, server_);
  }

  close(client_fd_);

}

}  // namespace proxy_server

  