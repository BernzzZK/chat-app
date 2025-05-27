//
// Created by xwz on 25-5-26.
//
#include "LogoutReq.h"
#include <muduo/base/Logging.h>
#include "Common.h"
#include "Logout.h"

LogoutReq::LogoutReq(std::string acc, int type)
    : _reqHead(logout, muduo::Timestamp::now().toFormattedString())
    , _acc(acc)
{
}

LogoutReq::LogoutReq(const std::string& req) {
    auto res = common::splitString(req);
    if (res.size() != 3) {
        LOG_ERROR << "Invalid request format";
        return;
    }
    _reqHead.set(static_cast<reqType>(std::stoi(res[0])), res[1]);
    toLogoutReq(res[2]);
}

void LogoutReq::toLogoutReq(std::string &info) {
    auto res = common::splitString(info);
    this->_acc = res[0];
}

std::string LogoutReq::toString() {
    std::string ret;
    ret.append("#" + _acc);
    return _reqHead.toString() + "@" + ret;
}

Response LogoutReq::handler() const {
    Logout logout(_acc);
    const std::string res = logout.logout();
    Response resp;
    if (res.empty()) {
        resp.set(true, "logout success");
    } else {
        resp.set(false, res);
    }
    return resp;
}