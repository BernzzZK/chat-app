#include "RedisConnPool.h"
#include <muduo/base/Logging.h>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace muduo;

RedisConnPool &RedisConnPool::instance()
{
    static RedisConnPool pool;
    return pool;
}

bool RedisConnPool::init(
    const string &host,
    unsigned int port,
    int minConns,
    int maxConns,
    int maxWaitMs)
{
    if (minConns <= 0 || maxConns < minConns || maxWaitMs < 0)
    {
        LOG_ERROR << "Invalid connection pool configuration";
        return false;
    }

    config_.host = host;
    config_.port = port;
    config_.min_conn = minConns;
    config_.max_conn = maxConns;
    config_.max_wait_ms = maxWaitMs;

    running_ = true;

    for (int i = 0; i < config_.min_conn; ++i)
    {
        auto conn = createConnection();
        if (conn)
        {
            conn_queue_.push(conn);
            all_conns_.push_back(conn);
            ++current_conns_;
        }
    }

    LOG_INFO << "Redis connection pool initialized with " << current_conns_ << " connections.";

    cleaner_thread_ = thread(&RedisConnPool::cleanupConnections, this);
    cleaner_thread_.detach();

    return true;
}

shared_ptr<RedisConn> RedisConnPool::createConnection()
{
    auto conn = std::make_shared<RedisConn>(config_.host, config_.port);

    if (!conn || !conn->ping())
    {
        LOG_ERROR << "Failed to create Redis connection.";
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        all_conns_.push_back(conn);
        ++current_conns_;
    }

    LOG_INFO << "New Redis connection created.";

    return conn;
}

shared_ptr<RedisConn> RedisConnPool::getConnection()
{
    unique_lock<mutex> lock(mutex_);

    if (conn_queue_.empty() && current_conns_ < config_.max_conn)
    {
        auto newConn = createConnection();
        if (newConn)
        {
            conn_queue_.push(newConn);
            all_conns_.push_back(newConn);
            ++current_conns_;
        }
    }

    if (cond_.wait_for(lock, chrono::milliseconds(config_.max_wait_ms),
                       [this]
                       { return !conn_queue_.empty() || !running_; }) == false)
    {
        LOG_WARN << "Get Redis connection timeout.";
        return nullptr;
    }

    if (!running_)
    {
        LOG_WARN << "Redis connection pool is stopping.";
        return nullptr;
    }

    auto conn = conn_queue_.front();
    conn_queue_.pop();

    // 如果连接失效，重新创建一个
    if (!conn->ping())
    {
        LOG_WARN << "Redis connection is dead. Creating a new one.";
        conn = createConnection();
        if (conn)
        {
            conn_queue_.push(conn);
            all_conns_.push_back(conn);
            ++current_conns_;
            return getConnection();
        }
    }

    LOG_INFO << "Got Redis connection from pool. Available: " << conn_queue_.size();
    return conn;
}

void RedisConnPool::releaseConnection(shared_ptr<RedisConn> conn)
{
    if (!conn)
        return;

    unique_lock<mutex> lock(mutex_);
    conn_queue_.push(conn);
    cond_.notify_one();
    LOG_INFO << "Redis connection released back to pool. Available: " << conn_queue_.size();
}

void RedisConnPool::stop()
{
    running_ = false;
    cond_.notify_all();
    if (cleaner_thread_.joinable())
    {
        cleaner_thread_.join();
    }
}

void RedisConnPool::cleanupConnections()
{
    while (running_)
    {
        {
            lock_guard<mutex> lock(mutex_);
            int idleCount = conn_queue_.size();
            while (idleCount > config_.min_conn && current_conns_ > config_.min_conn)
            {
                auto conn = conn_queue_.front();
                conn_queue_.pop();
                --idleCount;
                LOG_INFO << "Closing idle Redis connection. Total connections: " << current_conns_--;
            }
        }
        this_thread::sleep_for(chrono::seconds(30));
    }
}
