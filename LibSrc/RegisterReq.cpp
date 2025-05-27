#include "RegisterReq.h"
#include <muduo/base/Logging.h>
#include "Common.h"
#include "Register.h"

RegisterReq::RegisterReq(std::string acc, std::string pwd)
    : _reqHead(registered, muduo::Timestamp::now().toFormattedString())
    , _acc(acc)
    , _pwd(pwd)
{
}

RegisterReq::RegisterReq(std::string req) {
    auto res = common::splitString(req);
    if (res.size() != 3) {
        LOG_ERROR << "Invalid request format";
        return;
    }
    _reqHead.set(static_cast<reqType>(std::stoi(res[0])), res[1]);
    toRegisterReq(res[2]);
}

RegisterReq::RegisterReq(std::string req, int type)
    : _acc("")
    , _pwd("")
{
    _reqHead.set(login, muduo::Timestamp::now().toFormattedString());
    toRegisterReq(req);
}

void RegisterReq::toRegisterReq(std::string &info) {
    auto res = common::splitString(info);
    this->_acc = res[0];
    this->_pwd = res[1];
}

std::string RegisterReq::toString() {
    std::string ret;
    ret.append("#" + _acc);
    ret.append("#" + _pwd);
    return _reqHead.toString() + "@" + ret;
}

Response RegisterReq::handler() const {
    Register reg(_acc, _pwd);
    const std::string res = reg.validateRegister();
    Response resp;
    if (res.empty()) {
        resp.set(true, "register success");
    } else {
        resp.set(false, res);
    }
    return resp;
}