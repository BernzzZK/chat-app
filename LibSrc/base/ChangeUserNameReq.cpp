//
// Created by xwz on 25-6-13.
//

#include "ChangeUserNameReq.h"
#include <sstream>

#include "ChangeUserName.h"
#include "Common.h"
#include "Response.h"

ChangeUserNameReq::ChangeUserNameReq(const std::string& acc, const std::string& newName)
        : _acc(acc)
        , _newName(newName)
{
}

ChangeUserNameReq::ChangeUserNameReq(const std::string& str) {
        auto res = common::splitString(str);
        if (res.size() != 3) {
                throw std::runtime_error("ChangeUserNameReq format error");
        }
        _reqHead.set(static_cast<type::reqType>(std::stoi(res[0])), res[1]);
        toChangeUserNameReq(res[2]);
}

std::string ChangeUserNameReq::toString() {
        std::stringstream ss;
        ss << _reqHead.toString() << "@#" << _acc << "#" << _newName;
        return ss.str();
}

void ChangeUserNameReq::toChangeUserNameReq(const std::string& str) {
        auto res = common::splitString(str);
        if (res.size()!= 2) {
                throw std::runtime_error("ChangeUserNameReq format error");
        }
        _acc = res[0];
        _newName = res[1];
}

Response ChangeUserNameReq::handle() {
        ChangeUserName changeUserName(_acc, _newName);
        std::string res = changeUserName.changeUserName();
        if (res == "修改成功") {
                return Response(type::changeUserName, true, "修改成功");
        }
        return Response(type::changeUserName, false, res);
}

