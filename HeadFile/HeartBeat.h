#pragma once

#include "Request.h"

class Response;

class HeartBeat
{
private:
    std::string msg_;
    Request reqHead_;

public:
    Response handler() const;
    // std::string toString();
    // void toHeartBeat(std::string);

    HeartBeat(const std::string &);

    ~HeartBeat() {};
};