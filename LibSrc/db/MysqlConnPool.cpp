// MysqlConnPool.cpp
#include "MysqlConnPool.h"
#include "MysqlConn.h"
#include <thread>
#include <chrono>
#include "algorithm"
#include <muduo/base/Logging.h>

using namespace std;
using namespace muduo;

MysqlConnPool &MysqlConnPool::instance()
{
    static MysqlConnPool pool;
    return pool;
}

bool MysqlConnPool::init(
    const string &host,
    const string &user,
    const string &password,
    const string &database,
    unsigned int port,
    const string &charSet,
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
    config_.user = user;
    config_.pwd = password;
    config_.db = database;
    config_.port = port;
    config_.char_set = charSet;
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

    LOG_INFO << "MySQL connection pool initialized with " << current_conns_ << " connections.";

    cleaner_thread_ = thread(&MysqlConnPool::cleanupConnections, this);
    cleaner_thread_.detach();

    return true;
}

shared_ptr<MysqlConn> MysqlConnPool::createConnection()
{
    auto conn = std::make_shared<MysqlConn>(config_.host, config_.user, config_.pwd,
                                            config_.db, config_.port, config_.char_set);

    if (!conn)
    {
        LOG_ERROR << "Failed to create MySQL connection.";
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        all_conns_.push_back(conn);
        ++current_conns_;
    }

    LOG_INFO << "New MySQL connection created.";

    return conn;
}

shared_ptr<MysqlConn> MysqlConnPool::getConnection()
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
        LOG_WARN << "Get connection timeout.";
        return nullptr;
    }

    if (!running_)
    {
        LOG_WARN << "Connection pool is stopping.";
        return nullptr;
    }

    auto conn = conn_queue_.front();
    conn_queue_.pop();

    // 如果连接失效，重新创建一个
    if (!conn->isAlive())
    {
        LOG_WARN << "Connection is dead. Creating a new one.";
        conn = createConnection();
        if (conn)
        {
            conn_queue_.push(conn);
            all_conns_.push_back(conn);
            ++current_conns_;
            return getConnection();
        }
    }

    LOG_INFO << "Got connection from pool. Available: " << conn_queue_.size();
    return conn;
}

void MysqlConnPool::releaseConnection(shared_ptr<MysqlConn> conn)
{
    if (!conn)
        return;

    unique_lock<mutex> lock(mutex_);
    conn_queue_.push(conn);
    cond_.notify_one();
    LOG_INFO << "Connection released back to pool. Available: " << conn_queue_.size();
}

void MysqlConnPool::stop()
{
    running_ = false;
    cond_.notify_all();
    if (cleaner_thread_.joinable())
    {
        cleaner_thread_.join();
    }
}

void MysqlConnPool::cleanupConnections()
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
                LOG_INFO << "Closing idle connection. Total connections: " << current_conns_--;
            }
        }
        this_thread::sleep_for(chrono::seconds(30));
    }
}