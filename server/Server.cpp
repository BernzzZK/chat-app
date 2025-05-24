#include "Server.h"
#include "LoginReq.h"
#include "Common.h"
#include "Response.h"
using namespace std::placeholders;

Server::Server(net::EventLoop *loop, const net::InetAddress listenaddr) 
    : server_(loop, listenaddr, "chat server")
{
    server_.setConnectionCallback(bind(&Server::onConnection, this, _1));
    server_.setMessageCallback(bind(&Server::onMessage, this, _1, _2, _3));
}

void Server::onMessage(const muduo::net::TcpConnectionPtr &conn, net::Buffer *buff, Timestamp time) {
    std::string msg(buff->retrieveAllAsString());
    reqType type = common::parsing(msg);
    if(type == registered) {

    }
    else if(type == login) {
        LoginReq loginReq(msg);
        Response resp = loginReq.handler();
        conn->send(resp.toString());
    }
    else if(type == logout) {

    }
    else if(type == sendMsg) {

    }
}

// 广播消息给所有用户
// void Server::broadcast(const std::string &message)
// {
//     std::lock_guard<std::mutex> lock(connMutex_);
//     for (auto &pair : connections_)
//     {
//         pair.second->send(message);
//     }
// }

void Server::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (conn->connected())
    {
        LOG_INFO << "New connection: " << conn->name();
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            connections_[conn->name()] = conn;
        }
        // 发送欢迎消息
        conn->send("Welcome to chat room!\r\n");
    }
    else
    {
        LOG_INFO << "Connection closed: " << conn->name();
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            connections_.erase(conn->name());
        }
    }
}

void Server::start()
{
    server_.start();
}
