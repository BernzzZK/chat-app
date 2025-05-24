#include "LoginReq.h"
#include "Login.h"
#include "Common.h"

LoginReq::LoginReq(std::string req)
    : _acc("")
    , _pwd("")
    , _keepLogin(false)
{
    std::vector<std::string> res = common::splitString(req);
    _head.type = (reqType)std::stoi(res[0]);
    _head.time = res[1];
    toLoginReq(res[2]);
}

Response& LoginReq::handler() const {
    Login login(_acc, _pwd, _keepLogin);
    std::string res = login.validateLogin();
    if (res == "") {
        Response resp(true, res);
    } else {
        Response resp(false, res);
    }
}

std::string LoginReq::toString() {
    std::string ret;
    ret.append("#" + _acc);
    ret.append("#" + _pwd);
    ret.append("#" + std::to_string(_keepLogin));
    return _head.toString() + "@" + ret; // to split req in two part -- head & info
}
 
void LoginReq::toLoginReq(std::string info) {
    std::vector<std::string> res = common::splitString(info);
    this->_acc = res[0];
    this->_pwd = res[1];
    this->_keepLogin = std::stoi(res[2]);
}