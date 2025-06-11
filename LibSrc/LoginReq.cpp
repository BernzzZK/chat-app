#include "LoginReq.h"
#include "Login.h"
#include "Common.h"
#include <iostream>
#include <muduo/base/Logging.h>
#include "Response.h"

LoginReq::LoginReq(const std::string &account, const std::string &password)
    : _acc(account), _pwd(password)
{
    _reqHead.set(type::login, muduo::Timestamp::now().toFormattedString());
}

LoginReq::LoginReq(const std::string &req)
    : _acc(""), _pwd("")
{
    auto res = common::splitString(req);
    if (res.size() != 3)
    {
        LOG_ERROR << "Invalid request format";
        return;
    }
    _reqHead.set(static_cast<type::reqType>(std::stoi(res[0])), res[1]);
    toLoginReq(res[2]);
}

Response LoginReq::handler() const
{
    Login login(_acc, _pwd);
    std::string res = login.validateLogin();
    Response resp;
    if (res == "")
    {
        resp.set(type::login, true, "login success");
    }
    else
    {
        resp.set(type::login, false, res);
    }
    return resp;
}

std::string LoginReq::toString()
{
    std::string ret;
    ret.append("#" + _acc);
    ret.append("#" + _pwd);
    return _reqHead.toString() + "@" + ret; // to split req in two part -- head & info
}

void LoginReq::toLoginReq(std::string info)
{
    std::vector<std::string> res = common::splitString(info);
    this->_acc = res[0];
    this->_pwd = res[1];
}