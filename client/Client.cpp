#include "Client.h"

#include <iostream>
#include <muduo/base/Logging.h>
#include "Request.h"
#include "UserFunction.h"

using namespace muduo;
using namespace muduo::net;

ChatClient::ChatClient(EventLoop *loop, const InetAddress &serverAddr)
    : client_(loop, serverAddr, "ChatClient")
    , loop_(loop)
    , acc_("")
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
        } else if (msg[0] == '@'){
            std::unique_lock<std::mutex> lck(resp_mutex_);
            resp_ = Response(msg);
            rec_resp_ = true;
            resp_cv_.notify_one();
        } else if (msg[0] == '#'){

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
        while (true) {
            if (std::cin >> choice) {
                break;
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "输入无效，请重新输入！" << std::endl;
                std::cout << "输入选项: ";
            }
        }

        if (isLogin_ ==false) {
            switch (choice) {
                case 1: {
                    send(UserFunction::login(&acc_));
                    std::unique_lock lck(resp_mutex_);
                    while (!rec_resp_) {
                        resp_cv_.wait_for(lck, std::chrono::seconds(3));
                    }
                    if (resp_.isSuccess()) {
                        isLogin_ = true;
                        LOG_INFO << "account " << acc_ << " Login success: " << resp_.getReason();
                    } else {
                        LOG_INFO << "Login failed: " << resp_.getReason();
                    }
                    rec_resp_ = false;
                    break;
                }
                case 2: {
                    send(UserFunction::registerUser());
                    std::unique_lock lck(resp_mutex_);
                    while (!rec_resp_) {
                        resp_cv_.wait_for(lck, std::chrono::seconds(3));
                    }
                    if (resp_.isSuccess()) {
                        LOG_INFO << "register success: " << resp_.getReason();
                    } else {
                        LOG_INFO << "register failed: " << resp_.getReason();
                    }
                    rec_resp_ = false;
                    lck.unlock();
                    break;
                }
                case 3:
                    disconnect();
                    sleep(1);
                    exit(0);
                default:
                    LOG_ERROR << "无效选项!";
            }
        } else {
            switch (choice) {
                case 1: {
                    std::unique_lock lck(acc_mutex_);
                    send(UserFunction::sendmsg(acc_));
                    lck.unlock();
                    std::unique_lock lckr(resp_mutex_);
                    while (!rec_resp_) {
                        resp_cv_.wait_for(lckr, std::chrono::seconds(3));
                    }
                    if (resp_.isSuccess()) {
                        LOG_INFO << "sendmsg success: " << resp_.getReason();
                    } else {
                        LOG_INFO << "sendmsg failed: " << resp_.getReason();
                    }
                    rec_resp_ = false;
                    break;
                }
                case 2: {
                    std::unique_lock lck(acc_mutex_);
                    send(UserFunction::addfriend(acc_));
                    lck.unlock();
                    std::unique_lock lckr(resp_mutex_);
                    while (!rec_resp_) {
                        resp_cv_.wait_for(lckr, std::chrono::seconds(3));
                    }
                    if (resp_.isSuccess()) {
                        LOG_INFO << "addfriend success: " << resp_.getReason();
                    } else {
                        LOG_INFO << "addfriend failed: " << resp_.getReason();
                    }
                    rec_resp_ = false;
                    break;
                }
                case 3: {
                    std::unique_lock lck(acc_mutex_);
                    send(UserFunction::logout(acc_));
                    lck.unlock();
                    std::unique_lock lckr(resp_mutex_);
                    while (!rec_resp_) {
                        resp_cv_.wait_for(lckr, std::chrono::seconds(3));
                    }
                    if (resp_.isSuccess()) {
                        isLogin_ = false;
                        LOG_INFO << "account " << acc_ << " Logout success: " << resp_.getReason();
                    } else {
                        LOG_INFO << "Login failed: " << resp_.getReason();
                    }
                    rec_resp_ = false;
                    break;
                }
                default:
                    LOG_ERROR << "无效选项!";
                    break;
            }
        }
    }
}