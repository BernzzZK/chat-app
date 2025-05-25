#include "Request.h"
#include "Login.h"
#include "Common.h"

std::string reqHead::toString()
{
    std::string req;
    req.append("@" + std::to_string(this->type));
    req.append("@" + this->time);
    return req;
}

void reqHead::toReqHead(std::string str)
{
    std::vector<std::string> res = common::splitString(str);
    if (!res.empty()) {
        this->type = (reqType)std::stoi(res[0]);
        this->time = res[1];
    }
}

std::string Request::toString() {
    return _head.toString();
}

Request::Request(const reqType type, std::string time) {
    set(type, time);
}

void Request::set(const reqType type, std::string time) {
    _head.type = type;
    _head.time = time;
}