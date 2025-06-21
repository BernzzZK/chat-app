#pragma once 
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

using namespace muduo;
class Request;
class Server {
public:
    Server(net::EventLoop *loop, const net::InetAddress &listenaddr, const std::string &fileName);
    ~Server();

    void start();

private:
    bool LoadConfig(const std::string& filename);

    net::TcpServer server_;
    std::mutex connMutex_;
    std::unordered_map<std::string, net::TcpConnectionPtr> connections_;
    std::unordered_map<std::string, net::TcpConnectionPtr> loginUser_;
    std::unordered_set<net::TcpConnectionPtr> reply_;
    std::string fileName_;

    void onConnection(const net::TcpConnectionPtr &conn);
    void onMessage(const net::TcpConnectionPtr &conn, net::Buffer *buff, Timestamp time);

    void hasUnreadMsg(const std::string& user, const net::TcpConnectionPtr &conn);
    void hasUnprocessAddFriend(const std::string& user, const net::TcpConnectionPtr &conn);
    void friendList(const std::string& user, const net::TcpConnectionPtr &conn);

    // 检查连接是否存活且同步至redis
    // void isAlive();

    struct RedisPoolConfig
    {
        std::string host = "127.0.0.1";
        unsigned int port = 6379;
        int min_conn = 5;
        int max_conn = 20;
        int max_wait_ms = 1000;
    } redisConfig_;

    struct MysqlPoolConfig
    {
        std::string host = "127.0.0.1";
        std::string user = "root";
        std::string pwd = "123456";
        std::string db = "test";
        unsigned int port = 3306;
        std::string char_set;
        int min_conn = 5;
        int max_conn = 20;
        int max_wait_ms = 1000;
    } mysqlConfig_;
};