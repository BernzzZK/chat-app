#pragma once
#pragma once
#include <memory>
#include "MysqlConn.h"
#include "MysqlConnPool.h"
#include "RedisConn.h"
#include "RedisConnPool.h"

class MysqlConnGuard {
public:
    MysqlConnGuard()
        : conn_(MysqlConnPool::instance().getConnection()) {}

    ~MysqlConnGuard()
    {
        if (conn_)
        {
            MysqlConnPool::instance().releaseConnection(conn_);
        }
    }

    std::shared_ptr<MysqlConn> operator->() { return conn_; }
    std::shared_ptr<MysqlConn> &operator*() { return conn_; }

    bool isValid() const { return conn_ != nullptr; }

private:
    std::shared_ptr<MysqlConn> conn_;
};

class RedisConnGuard {
public:
    RedisConnGuard()
        : conn_(RedisConnPool::instance().getConnection()) {}

    ~RedisConnGuard()
    {
        if (conn_)
        {
            RedisConnPool::instance().releaseConnection(conn_);
        }
    }

    std::shared_ptr<RedisConn> operator->() { return conn_; }
    std::shared_ptr<RedisConn> &operator*() { return conn_; }

    bool isValid() const { return conn_ != nullptr; }

private:
    std::shared_ptr<RedisConn> conn_;
};

