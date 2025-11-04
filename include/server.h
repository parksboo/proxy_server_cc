#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace proxy_server {

class Server {
 public:
  explicit Server(int port, int ttl);
  std::vector<std::string> block_list;
  std::unordered_map<std::string, std::pair<std::string, time_t>> cache;
  int ttl;
  std::mutex cache_mutex;

 private:
  int port_;
  int server_fd_;
  void AcceptConnections();
  void CallBlockList();
};
}  // namespace proxy_server

#endif // SERVER_H