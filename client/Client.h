#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Mutex.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Response.h"


class ChatClient
{
public:
    ChatClient(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &serverAddr);

    void connect();
    void disconnect();
    void send(const std::string &message);

    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf,
                   muduo::Timestamp time);
    
    
    muduo::net::TcpClient client_;
    muduo::net::EventLoop *loop_;
    std::atomic<bool> connected_{false};
    muduo::MutexLock mutex_;
    muduo::net::TcpConnectionPtr conn_;

    muduo::MutexLock resp_mutex_;
    std::condition_variable resp_cv_;
    Response resp_;

    std::atomic<bool> isLogin_{false};
};