#include "Response.h"
#include "Common.h"

Response::Response(bool success, std::string failReaon = "")
    : _time(muduo::Timestamp::now().toString())
    , _success(success)
    , _failReson(failReaon)
{
}

Response::Response(std::string resp) {
    toResponse(resp);
}

std::string Response::toString() {
    std::string ret;
    ret.append("@" + std::to_string(_success));
    ret.append("@" + _time);
    ret.append("@" + _failReson);
    return ret;
}

void Response::toResponse(std::string resp) {
    auto res = common::splitString(resp);
    _success = std::stoi(res[0]);
    _time = res[1];
    _failReson = res[2];
}