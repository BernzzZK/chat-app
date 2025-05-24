#pragma once

#include <hiredis/hiredis.h>
#include <string>

using namespace std;

class RedisConn {
private:
    redisContext *conn_;
    redisReply *rpy_;
    string ip_;
    unsigned int port_;

public:
    RedisConn(string ip, unsigned int port);
    ~RedisConn();

    redisReply *query(string cmd);
};