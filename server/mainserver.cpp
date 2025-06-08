#include "Server.h"

int main(int argc, char **argv) {
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(8888);
    std::string fileName;
    if (argc > 1) {
        fileName = argv[1];
        LOG_INFO << "config file = " << fileName;
    } else {
        fileName = "";
        LOG_INFO << "no config file, if you want to use config file, use: ./ChatServer config.yaml";
    }
    Server server(&loop, listenAddr, fileName);
    server.start();
    loop.loop();
}