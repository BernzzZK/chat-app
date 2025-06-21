#include "RedisConn.h"
#include <muduo/base/Logging.h>

RedisConn::RedisConn(const std::string& ip, unsigned int port)
    : ip_(ip)
    , port_(port)
{
    conn_ = redisConnect(ip_.c_str(), port_);
    if (!conn_ || conn_->err) {
        LOG_ERROR << "Redis connect error: "
                 << (conn_ ? conn_->errstr : "connection failed");
        if (conn_) {
            redisFree(conn_);
            conn_ = nullptr;
        }
    }
}

RedisConn::~RedisConn() {
    if (conn_) {
        redisFree(conn_);
    }
}

redisReply* RedisConn::execute(const std::string& cmd) {
    if (!conn_) return nullptr;

    redisReply* reply = static_cast<redisReply*>(redisCommand(conn_, cmd.c_str()));
    if (!reply) {
        LOG_ERROR << "Redis command failed: " << cmd;
        return nullptr;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        LOG_ERROR << "Redis error: " << reply->str;
        freeReplyObject(reply);
        return nullptr;
    }

    return reply;
}

bool RedisConn::ping() {
    redisReply* reply = execute("PING");
    if (!reply) return false;

    bool ok = reply->type == REDIS_REPLY_STATUS &&
              std::string(reply->str) == "PONG";
    freeReplyObject(reply);
    return ok;
}