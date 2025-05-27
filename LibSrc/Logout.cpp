//
// Created by xwz on 25-5-26.
//

#include "Logout.h"

Logout::Logout(const std::string &acc)
    : _acc(acc)
{
}

std::string Logout::logout() const {
    std::string res;
    if (_acc.empty()) {
        res = "logout failed: invalid account";
    } else {
        res = "";
    }
    return res;
}
