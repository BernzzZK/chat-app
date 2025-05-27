//
// Created by xwz on 25-5-25.
//

#ifndef USERFUNCTION_H
#define USERFUNCTION_H
#include <string>

namespace UserFunction {
    std::string login(std::string *acc);
    std::string registerUser();
    std::string sendmsg();
    std::string addfriend(std::string &acc);
    std::string logout(std::string &acc);
};
#endif //USERFUNCTION_H
