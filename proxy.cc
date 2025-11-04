// YOUR CODE HERE
#include <iostream>
#include "server.h"
#include "strutil.h"


int main(int argc, char* argv[]) {
    const int port = (argc > 1) ? std::stoi(argv[1]) : 8000;
    const int ttl = (argc > 2) ? std::stoi(argv[2]) : 60;
    std::cout << "Starting proxy on port " << port << " with TTL " << ttl << std::endl;
    proxy_server::Server server(port, ttl);
    return 0;
}