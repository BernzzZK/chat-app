#include "Server.h"

int main() {
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(8888);
    Server server(&loop, listenAddr);
    server.start();
    loop.loop();
}