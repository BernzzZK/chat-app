//
// Created by xwz on 25-5-25.
//

#include "UserFunction.h"
#include "LoginReq.h"
#include <iostream>

std::string UserFunction::login() {
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
    info.append("#" + username);
    info.append("#" + password);
    info.append("#" + std::to_string(keepAlive));
    LoginReq loginReq(info, 1);
    std::string req = loginReq.toString();
    return req;
}
