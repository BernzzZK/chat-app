#include "RedisConn.h"
#include <muduo/base/Logging.h>
using namespace muduo;

RedisConn::RedisConn(string ip = "127.0.0.1", unsigned int port = 6379)
    : ip_(ip)
    , port_(port)
{
    conn_ = redisConnect(ip_.c_str(), port_);
    if(conn_->err != 0)
        LOG_ERROR << "Redis connect error!";
}

RedisConn::~RedisConn() {
    redisFree(this->conn_);
}

redisReply *RedisConn::query(string cmd) {
    void *tmp = redisCommand(conn_, cmd.c_str());
    rpy_ = (redisReply *)tmp;
    if (rpy_->type == 6) {
        freeReplyObject(this->rpy_);
        return nullptr;
    }
    return rpy_;
}