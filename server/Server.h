#pragma once 
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <mutex>
#include <unordered_map>
#include <jsoncpp/json/json.h>

using namespace muduo;

class Request;

class Server {
public:
    Server(net::EventLoop *loop, const net::InetAddress listenaddr);
    ~Server() {};

    void start();

private:
    net::TcpServer server_;
    std::mutex connMutex_;
    std::unordered_map<std::string, muduo::net::TcpConnectionPtr> connections_;

    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr &conn, net::Buffer *buff, Timestamp time);
    void broadcast(const std::string &message);
};