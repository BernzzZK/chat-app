//
// Created by xwz on 25-5-26.
//
#include "LogoutReq.h"
#include <muduo/base/Logging.h>
#include "Common.h"
#include "Logout.h"
#include <algorithm>
#include "Response.h"

LogoutReq::LogoutReq(std::string acc, int type)
    : _reqHead(type::logout, muduo::Timestamp::now().toFormattedString()), _acc(acc)
{
    auto removeNewlines = [](std::string &str)
    {
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    };
    removeNewlines(_acc);
}

LogoutReq::LogoutReq(const std::string &req)
{
    auto res = common::splitString(req);
    if (res.size() != 3)
    {
        LOG_ERROR << "Invalid request format";
        return;
    }
    _reqHead.set(static_cast<type::reqType>(std::stoi(res[0])), res[1]);
    toLogoutReq(res[2]);
}

void LogoutReq::toLogoutReq(std::string &info)
{
    auto res = common::splitString(info);
    this->_acc = res[0];
    auto removeNewlines = [](std::string &str)
    {
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    };
    removeNewlines(_acc);
}

std::string LogoutReq::toString()
{
    std::string ret;
    ret.append("#" + _acc);
    return _reqHead.toString() + "@" + ret;
}

Response LogoutReq::handler() const
{
    Logout logout(_acc);
    const std::string res = logout.logout();
    Response resp;
    if (res.empty())
    {
        resp.set(type::logout, true, "logout success");
    }
    else
    {
        resp.set(type::logout, false, res);
    }
    return resp;
}