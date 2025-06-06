#pragma once
#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include "RedisConn.h"

class RedisConnPool
{
public:
    static RedisConnPool &instance();

    bool init(
        const std::string &host = "127.0.0.1",
        unsigned int port = 6379,
        int minConns = 5,
        int maxConns = 20,
        int maxWaitMs = 1000);

    std::shared_ptr<RedisConn> getConnection();
    void releaseConnection(std::shared_ptr<RedisConn> conn);
    void stop();

private:
    RedisConnPool() = default;
    ~RedisConnPool() {
        stop();
    };

    struct PoolConfig
    {
        std::string host;
        unsigned int port = 6379;
        int min_conn = 5;
        int max_conn = 20;
        int max_wait_ms = 1000;
    } config_;

    std::shared_ptr<RedisConn> createConnection();
    void cleanupConnections();

    std::queue<std::shared_ptr<RedisConn>> conn_queue_;
    std::vector<std::shared_ptr<RedisConn>> all_conns_;
    std::mutex mutex_;
    std::condition_variable cond_;
    int current_conns_ = 0;
    bool running_ = false;

    // 后台清理线程
    std::thread cleaner_thread_;
};
