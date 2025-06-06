#include "Server.h"

int main(int argc, char **argv) {
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(8888);
    std::string fileName;
    if (argc > 1) {
        fileName = argv[1];
    } else {
        fileName = "";
    }
    Server server(&loop, listenAddr, fileName);
    server.start();
    loop.loop();
}