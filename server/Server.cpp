#include "Server.h"
#include "LoginReq.h"
#include "RegisterReq.h"
#include "Common.h"
#include "Response.h"
#include "MysqlConnPool.h"
using namespace std::placeholders;

Server::Server(net::EventLoop *loop, const net::InetAddress listenaddr) 
    : server_(loop, listenaddr, "chat server")
{
    server_.setConnectionCallback(bind(&Server::onConnection, this, _1));
    server_.setMessageCallback(bind(&Server::onMessage, this, _1, _2, _3));
    server_.setThreadNum(4);
    MysqlConnPool::instance().init("43.142.101.247", "root", "20040508", "chat");
}

void Server::onMessage(const muduo::net::TcpConnectionPtr &conn, net::Buffer *buff, Timestamp time) {
    std::string msg(buff->retrieveAllAsString());
    try{
        reqType type = (reqType)std::stoi(common::parsing(msg));
        if (type == registered)
        {
            conn->send("registered unrealized");
        }
        else if (type == login)
        {
            LoginReq loginReq(msg);
            const Response resp = loginReq.handler();
            LOG_INFO << "response: " << resp.toString();
            conn->send(resp.toString());
        }
        else if (type == logout)
        {
            conn->send("logout unrealized");
        }
        else if (type == sendMsg)
        {
            conn->send("sendMsg unrealized");
        }
    } catch(const std::invalid_argument& e) {
       LOG_ERROR << "Invalid argument: " << e.what(); 
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
