#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <memory>

class RedisConn {
public:
    using Ptr = std::unique_ptr<RedisConn>;

    RedisConn(const std::string& ip = "127.0.0.1", unsigned int port = 6379);
    ~RedisConn();

    // 禁用拷贝
    RedisConn(const RedisConn&) = delete;
    RedisConn& operator=(const RedisConn&) = delete;

    // 执行命令并返回结果（自动管理回复内存）
    redisReply* execute(const std::string& cmd);

    // 检查连接是否有效
    bool ping();

    // 获取原始连接上下文
    redisContext* context() { return conn_; }

private:
    redisContext* conn_ = nullptr;
    std::string ip_;
    unsigned int port_;
};