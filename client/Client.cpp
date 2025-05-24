#include "Client.h"
#include <muduo/base/Logging.h>

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
        // 可添加自动重连逻辑
    }
}

// 接收消息回调
void ChatClient::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp time)
{
    std::string msg(buf->retrieveAllAsString());
    printf("\n[Server] %s", msg.c_str());
    fflush(stdout); // 确保及时输出
}