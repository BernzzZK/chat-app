#include "Server.h"

#include "AddFriendReq.h"
#include "LoginReq.h"
#include "RegisterReq.h"
#include "Common.h"
#include "LogoutReq.h"
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
        reqType type = static_cast<reqType>(std::stoi(common::parsing(msg)));
        if (type == registered)
        {
            RegisterReq registerReq(msg);
            const Response resp = registerReq.handler();
            LOG_INFO << "response: " << resp.toString();
            conn->send(resp.toString());
        }
        else if (type == login)
        {
            LoginReq loginReq(msg);
            LOG_INFO << "login: " << loginReq.toString();
            Response resp = loginReq.handler();
            LOG_INFO << "response: " << resp.toString();
            std::lock_guard<std::mutex> lock(connMutex_);
            if (loginUser_.find(loginReq.getAccount()) == loginUser_.end()) {
                loginUser_[loginReq.getAccount()] = conn;
            } else {
                Response repeatLogin(false, "account already login");
                conn->send(repeatLogin.toString());
            }
            conn->send(resp.toString());
        }
        else if (type == logout)
        {
            LOG_INFO << "msg: " << msg;
            LogoutReq logoutReq(msg);
            Response resp = logoutReq.handler();
            LOG_INFO << "response: " << resp.toString();
            LOG_INFO << "logoutReq: " << logoutReq.toString();
            std::lock_guard<std::mutex> lock(connMutex_);
            auto it = loginUser_.find(logoutReq.getAccount());
            if (it != loginUser_.end()) {
                loginUser_.erase(it);
            }
            conn->send(resp.toString());
        }
        else if (type == sendMsg)
        {
            conn->send("sendMsg unrealized");
        }
        else if (type == addFriend)
        {
            LOG_INFO << "addFriend: " << msg;
            AddFriendReq addFriendReq(msg);
            Response resp = addFriendReq.handler();
            LOG_INFO << "response: " << resp.toString();
            conn->send(resp.toString());
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
            for (auto it : loginUser_) {
                if (it.second == conn) {
                    const auto acc = it.first;
                    loginUser_.erase(acc);
                    break;
                }
            }
            connections_.erase(conn->name());
        }
    }
}

void Server::start()
{
    server_.start();
}
