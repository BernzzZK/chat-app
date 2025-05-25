#include "Client.h"

#include <iostream>
#include <muduo/base/Logging.h>

#include "LoginReq.h"
#include "Request.h"
#include "Response.h"

using namespace muduo;
using namespace muduo::net;

ChatClient::ChatClient(EventLoop *loop, const InetAddress &serverAddr)
    : client_(loop, serverAddr, "ChatClient"),
      loop_(loop)
{
    client_.setConnectionCallback(
        std::bind(&ChatClient::onConnection, this, _1));
    client_.setMessageCallback(
        std::bind(&ChatClient::onMessage, this, _1, _2, _3));
}

void ChatClient::connect()
{
    client_.connect();
}

void ChatClient::disconnect()
{
    if (connected_)
    {
        client_.disconnect();
    }
}

void ChatClient::send(const std::string &message)
{
    MutexLockGuard lock(mutex_);
    if (connected_)
    {
        conn_->send(message + "\r\n");
    }
}

// 连接状态回调
void ChatClient::onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << "Connection state: " << conn->connected();

    MutexLockGuard lock(mutex_);
    if (conn->connected())
    {
        connected_ = true;
        conn_ = conn;
        LOG_INFO << "Connected to server";
    }
    else
    {
        connected_ = false;
        conn_.reset();
        LOG_INFO << "Disconnected from server";
    }
}

// 接收消息回调
void ChatClient::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp time)
{
    std::string msg(buf->retrieveAllAsString());
    if (msg.empty()) {
         LOG_INFO << "Empty message";
    } else {
        if (msg[0] != '@') {
            LOG_INFO << "[server]: " << msg;
        }
        else {
            Response response(msg);
            LOG_INFO << "request " << (response.isSuccess() ? ": " : "fail: ") << response.getReason();
        }
    }
}

void ChatClient::start() {
    while (true) {
        sleep(1);
        std::cout << "\n请选择操作:\n"
                  << "1. 登录\n"
                  << "2. 发送消息\n"
                  << "3. 退出\n"
                  << "输入选项: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // 清除输入缓冲区

        switch (choice) {
            case 1: {
                std::string username, password;
                std::cout << "用户名: ";
                std::getline(std::cin, username);
                std::cout << "密码: ";
                std::getline(std::cin, password);
                std::cout << "是否保持登录状态? (y/n): ";
                std::string keepLogin;
                std::getline(std::cin, keepLogin);
                bool keepAlive = (keepLogin == "y" || keepLogin == "Y");
                std::string info;
                info.append("#" + username);
                info.append("#" + password);
                info.append("#" + std::to_string(keepAlive));
                LoginReq loginReq(info, 1);
                std::string req = loginReq.toString();
                send(req);
                break;
            }
            case 2: {
                if (!connected_) {
                    std::cout << "请先登录!\n";
                    break;
                }
                std::string message;
                std::cout << "输入消息: ";
                std::getline(std::cin, message);
                send(message);
                break;
            }
            case 3:
                disconnect();
                return;
            default:
                std::cout << "无效选项!\n";
        }
    }
}