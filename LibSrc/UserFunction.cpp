//
// Created by xwz on 25-5-25.
//

#include "UserFunction.h"
#include "LoginReq.h"
#include <iostream>
#include <limits>

#include "LogoutReq.h"
#include "RegisterReq.h"

std::string UserFunction::login(std::string *acc) {
    std::string username, password;
    std::cout << "用户名: ";
    std::getline(std::cin, username);
    std::cout << "密码: ";
    std::getline(std::cin, password);
    std::cout << "是否保持登录状态? (y/n): ";
    std::string keepLogin;
    std::getline(std::cin, keepLogin);
    bool keepAlive = (keepLogin == "y" || keepLogin == "Y");
    std::string info;
    if (acc != nullptr) {
       *acc = username;
    }
    info.append("#" + username);
    info.append("#" + password);
    info.append("#" + std::to_string(keepAlive));
    LoginReq loginReq(info, 1);
    std::string req = loginReq.toString();
    return req;
}

std::string UserFunction::registerUser() {
    std::string username, password;
    // 输入用户名
    std::cout << "用户名: ";
    std::getline(std::cin, username);
    while (username.size() > 16 || username.size() < 4) {
        std::cout << "用户名长度必须在4到16之间，请重新输入: ";
        std::getline(std::cin, username);
    }
    // 输入密码
    std::cout << "密码: ";
    std::getline(std::cin, password);
    while (password.size() > 16 || password.size() < 4) {
        std::cout << "密码长度必须在4到16之间，请重新输入: ";
        std::getline(std::cin, password);
    }

    // 确保密码没有换行符
    if (!password.empty() && password.back() == '\n') {
        password.pop_back();
    }
    RegisterReq registerReq(username, password);
    std::string req = registerReq.toString();
    return req;
}

std::string UserFunction::sendMsg() {
    return "";
}

std::string UserFunction::addFriend() {
    return "";
}

std::string UserFunction::logout(std::string &acc) {
    const std::string username = acc;
    LogoutReq logoutReq(username, 1);
    std::string req = logoutReq.toString();
    acc = "";
    return req;
}
