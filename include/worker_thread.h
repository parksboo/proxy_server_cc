
#ifndef PROXY_WORKER_THREAD_H_
#define PROXY_WORKER_THREAD_H_

// related code
// worker_thread.cc

// system header
#include <unistd.h>

// C++ standard library

#include "server.h"


namespace proxy_server {

class Worker {
 public:
  explicit Worker(int fd, Server &server) : client_fd_(fd), server_(server) {}

  // Non-copyable, movable
  Worker(const Worker&) = delete;
  Worker& operator=(const Worker&) = delete;
  Worker(Worker&& other) noexcept : client_fd_(other.client_fd_), server_(other.server_) { other.client_fd_ = -1; }
  Worker& operator=(Worker&& other) noexcept {
    if (this != &other) {
      close_now(); 
      client_fd_ = other.client_fd_;
      other.client_fd_ = -1;
    }
    return *this;
  }

  ~Worker() { close_now(); }

  void operator()() { Run(); }

 private:
  int client_fd_{-1};
  Server &server_;

  void close_now() {
    if (client_fd_ >= 0) { ::close(client_fd_); client_fd_ = -1; }
  }

  void Run();
};

}  // namespace proxy_server

#endif  // PROXY_WORKER_THREAD_H_
