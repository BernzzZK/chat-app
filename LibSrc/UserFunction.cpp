//
// Created by xwz on 25-5-25.
//

#include "UserFunction.h"
#include "LoginReq.h"
#include <iostream>
#include <muduo/base/Logging.h>
#include "LogoutReq.h"
#include "RegisterReq.h"
#include "AddFriendReq.h"
#include "Common.h"
#include "SendMsgReq.h"

std::string UserFunction::login(std::string *acc) {
    std::string username, password;
    std::cin.ignore();
    // 输入账号名
    std::cout << "账号名: ";
    std::getline(std::cin, username);
    while (true) {
        if (username.size() < 4 || username.size() > 16) {
            std::cout << "账号名长度必须在4到16之间，请重新输入: ";
            std::getline(std::cin, username);
        } else if (common::containsInvalidChars(username)) {
            std::cout << "账号名不能包含以下字符： ";
            for (const char c : common::illegalAlphabet) {
                std::cout << c << " ";
            }
            std::cout << "请重新输入: ";
            std::getline(std::cin, username);
        } else {
            break; // 输入合法
        }
    }
    // 输入密码
    std::cout << "密码: ";
    std::getline(std::cin, password);
    while (true) {
        if (password.size() < 4 || password.size() > 16) {
            std::cout << "密码长度必须在4到16之间，请重新输入: ";
            std::getline(std::cin, password);
        } else if (common::containsInvalidChars(password)) {
            std::cout << "密码不能包含: ";
            for (const char c : common::illegalAlphabet) {
                std::cout << c << " ";
            }
            std::cout << "请重新输入: ";
            std::getline(std::cin, password);
        } else {
            break; // 输入合法
        }
    }
    password = common::encryption(password);
    LOG_INFO << "password: " << password;
    std::cout << "是否保持登录状态? (y/n): ";
    std::string keepLogin;
    std::getline(std::cin, keepLogin);
    bool keepAlive = (keepLogin == "y" || keepLogin == "Y");
    if (acc != nullptr) {
       *acc = username;
    }
    LoginReq loginReq(username, password, keepAlive);
    std::string req = loginReq.toString();
    return req;
}

std::string UserFunction::registerUser() {
    std::string username, password;
    std::cin.ignore();
    // 输入账号名
    std::cout << "账号名: ";
    std::getline(std::cin, username);
    while (true) {
        if (username.size() < 4 || username.size() > 16) {
            std::cout << "账号名长度必须在4到16之间，请重新输入: ";
            std::getline(std::cin, username);
        } else if (common::containsInvalidChars(username)) {
            std::cout << "账号名不能包含以下字符： ";
            for (const char c : common::illegalAlphabet) {
                std::cout << c << " ";
            }
            std::cout << "请重新输入: ";
            std::getline(std::cin, username);
        } else {
            break; // 输入合法
        }
    }
    // 输入密码
    std::cout << "密码: ";
    std::getline(std::cin, password);
    while (true) {
        if (password.size() < 4 || password.size() > 16) {
            std::cout << "密码长度必须在4到16之间，请重新输入: ";
            std::getline(std::cin, password);
        } else if (common::containsInvalidChars(password)) {
            std::cout << "密码不能包含: ";
            for (const char c : common::illegalAlphabet) {
                std::cout << c << " ";
            }
            std::cout << "请重新输入: ";
            std::getline(std::cin, password);
        } else {
            break; // 输入合法
        }
    }
    password = common::encryption(password);
    LOG_INFO << "password: " << password;
    RegisterReq registerReq(username, password);
    std::string req = registerReq.toString();
    return req;
}

std::string UserFunction::sendmsg(std::string &acc) {
    const std::string curr_username = acc;
    std::string username;
    std::cin.ignore();
    // 输入账号名
    std::cout << "账号名: ";
    std::getline(std::cin, username);
    while (true) {
        if (username.size() < 4 || username.size() > 16) {
            std::cout << "账号名长度必须在4到16之间，请重新输入: ";
            std::getline(std::cin, username);
        } else if (common::containsInvalidChars(username)) {
            std::cout << "账号名不能包含以下字符： ";
            for (const char c : common::illegalAlphabet) {
                std::cout << c << " ";
            }
            std::cout << "请重新输入: ";
            std::getline(std::cin, username);
        } else {
            break; // 输入合法
        }
    }
    std::string msg;
    std::cout << "消息: ";
    std::getline(std::cin, msg);
    msg = common::escapeIllegalCharacters(msg);
    SendMsgReq sendMsgReq(msg, curr_username, username);
    return sendMsgReq.toString();
}

std::string UserFunction::addfriend(std::string &acc) {
    const std::string curr_username = acc;
    std::string username;
    std::cin.ignore();
    // 输入账号名
    std::cout << "账号名: ";
    std::getline(std::cin, username);
    while (true) {
        if (username.size() < 4 || username.size() > 16) {
            std::cout << "账号名长度必须在4到16之间，请重新输入: ";
            std::getline(std::cin, username);
        } else if (common::containsInvalidChars(username)) {
            std::cout << "账号名不能包含以下字符： ";
            for (const char c : common::illegalAlphabet) {
                std::cout << c << " ";
            }
            std::cout << "请重新输入: ";
            std::getline(std::cin, username);
        } else {
            break; // 输入合法
        }
    }
    AddFriendReq addFriendReq(username, curr_username);
    std::string req = addFriendReq.toString();
    return req;
}

std::string UserFunction::logout(std::string &acc) {
    const std::string username = acc;
    LogoutReq logoutReq(username, 1);
    std::string req = logoutReq.toString();
    acc = "";
    return req;
}
