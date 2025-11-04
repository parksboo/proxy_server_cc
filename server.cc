
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>



#include "server.h"
#include "worker_thread.h"

namespace proxy_server {

Server::Server(int port, int ttl) : port_(port), ttl(ttl), server_fd_(-1) {
  // Constructor implementation
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ == -1) {
    std::cerr << "Failed to create socket." << std::endl;
    return;
  }
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_);

  if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Failed to bind socket." << std::endl;
    return;
  }

  if (listen(server_fd_, SOMAXCONN) == -1) {
    std::cerr << "Failed to listen on socket." << std::endl;
    return;
  }
  CallBlockList();
  AcceptConnections();
}

void Server::CallBlockList() {
    std::ifstream file("./blocklist");
    std::vector<std::string> BlockList;
    std::string line;
    while (std::getline(file, line)) {
        BlockList.push_back(line);
    }
    std::cout << "Blocklist loaded with " << BlockList.size() << " entries." << std::endl;
    block_list = BlockList;
}


void Server::AcceptConnections() {
  // AcceptConnections implementation
  std::cout << "Proxy server listening" << std::endl;
    while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd_, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) continue;  // Accept failed, try again
    std::thread(Worker{client_fd, *this}).detach();
  }
}

}  // namespace proxy_server
