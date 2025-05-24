#pragma once
#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>
#include <muduo/base/Timestamp.h>
#include <mysql/mysql.h>
#include <thread>

class MysqlConn;

class MysqlConnPool
{
public:
    static MysqlConnPool &instance();

    bool init(
        const std::string &host,
        const std::string &user,
        const std::string &password,
        const std::string &database,
        unsigned int port = 3306,
        const std::string &charSet = "utf8",
        int minConns = 5,
        int maxConns = 20,
        int maxWaitMs = 1000);

    std::shared_ptr<MysqlConn> getConnection();
    void releaseConnection(std::shared_ptr<MysqlConn> conn);
    void stop();

private:
    MysqlConnPool() = default;
    ~MysqlConnPool() {
        stop();
    };

    struct PoolConfig
    {
        std::string host;
        std::string user;
        std::string pwd;
        std::string db;
        unsigned int port = 3306;
        std::string char_set;
        int min_conn = 5;
        int max_conn = 20;
        int max_wait_ms = 1000;
    } config_;

    std::shared_ptr<MysqlConn> createConnection();
    void cleanupConnections();

    std::queue<std::shared_ptr<MysqlConn>> conn_queue_;
    std::vector<std::shared_ptr<MysqlConn>> all_conns_;
    std::mutex mutex_;
    std::condition_variable cond_;
    int current_conns_ = 0;
    bool running_ = false;

    // 后台清理线程
    std::thread cleaner_thread_;
};