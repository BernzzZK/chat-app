#include "Client.h"

#include <iostream>
#include <muduo/base/Logging.h>
#include "Request.h"
#include "UserFunction.h"

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
            MutexLockGuard lock(mutex_);
            resp_ = Response(msg);
            LOG_INFO << "request " << (resp_.isSuccess() ? ": " : "fail: ") << resp_.getReason();
        }
    }
}

void ChatClient::start() {
    while (true) {
        sleep(2);
        if (isLogin_ == false) {
            std::cout << "\n请选择操作:\n"
                  << "1. 登录\n"
                  << "2. 注册\n"
                  << "3. 退出\n"
                  << "输入选项: ";
        } else {
            std::cout << "\n请选择操作:\n"
                  << "1. 发信息\n"
                  << "2. 添加好友\n"
                  << "3. 退出登录\n"
                  << "输入选项: ";
        }

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // 清除输入缓冲区
        if (isLogin_ ==false) {
            switch (choice) {
                case 1: {
                    send(UserFunction::login());
                    break;
                }
                case 2: {
                    send(UserFunction::registerUser());
                    break;
                }
                case 3:
                    disconnect();
                    return;
                default:
                    std::cout << "无效选项!\n";
            }
        } else {
            switch (choice) {
                case 1: {
                    send(UserFunction::sendMsg());
                    break;
                }
                case 2: {
                    send(UserFunction::addFriend());
                    break;
                }
                case 3: {
                    send(UserFunction::logout());
                    isLogin_ = false;
                    break;
                }
                default:
                    std::cout << "无效选项!\n";
                    break;
            }
        }

    }
}