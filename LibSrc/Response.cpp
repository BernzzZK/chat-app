#include "Response.h"
#include <muduo/base/Logging.h>
#include "Common.h"

Response::Response()
    : _success(false)
    , _time(muduo::Timestamp::now().toFormattedString())
    , _failReason("")
{
}

Response::Response(const bool success, const std::string &failReason = "")
    : _success(success)
    , _time(muduo::Timestamp::now().toString())
    , _failReason(failReason)
{
}

Response::Response(std::string &resp)
    : _success(false)
    , _time(muduo::Timestamp::now().toFormattedString())
    , _failReason("")
{
    toResponse(resp);
}

std::string Response::toString() const {
    std::string ret;
    ret.append("@" + std::to_string(_success));
    ret.append("@" + _time);
    ret.append("@" + _failReason);
    return ret;
}

void Response::toResponse(std::string &resp) {
    if (resp.empty()) {
        LOG_ERROR << "Invalid response format";
        return;
    }
    const auto res = common::splitString(resp);
    _success = std::stoi(res[0]);
    _time = res[1];
    _failReason = res[2];
}

void Response::set(const bool success, const std::string &failReason) {
    _success = success;
    _failReason = failReason;
}

bool Response::isSuccess() const {
    return _success;
}

std::string Response::getReason() const {
    return _failReason;
}

void Response::makeEmpty() {
    this->_success = false;
    this->_time = "";
    this->_failReason = "";
}
