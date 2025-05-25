#include "LoginReq.h"
#include "Login.h"
#include "Common.h"
#include <iostream>
#include <muduo/base/Logging.h>

LoginReq::LoginReq(std::string req, int type)
    : _acc("")
    , _pwd("")
    , _keepLogin(false)
{
    _reqHead.set(login, muduo::Timestamp::now().toFormattedString());
    toLoginReq(req);
}

LoginReq::LoginReq(std::string req)
    : _acc("")
    , _pwd("")
    , _keepLogin(false)
{
    auto res = common::splitString(req);
    if (res.size() != 3) {
        LOG_ERROR << "Invalid request format";
        return;
    }
    _reqHead.set(static_cast<reqType>(std::stoi(res[0])), res[1]);
    toLoginReq(res[2]);
}

Response LoginReq::handler() const {
    Login login(_acc, _pwd, _keepLogin);
    std::string res = login.validateLogin();
    Response resp;
    if (res == "") {
       resp.set(true, "login success");
    } else {
       resp.set(false, res);
    }
    return resp;
}

std::string LoginReq::toString() {
    std::string ret;
    ret.append("#" + _acc);
    ret.append("#" + _pwd);
    ret.append("#" + std::to_string(_keepLogin));
    return _reqHead.toString() + "@" + ret; // to split req in two part -- head & info
}
 
void LoginReq::toLoginReq(std::string info) {
    std::vector<std::string> res = common::splitString(info);
    this->_acc = res[0];
    this->_pwd = res[1];
    this->_keepLogin = std::stoi(res[2]);
}