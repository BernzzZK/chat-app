//
// Created by xwz on 25-5-25.
//

#ifndef USERFUNCTION_H
#define USERFUNCTION_H
#include <string>

namespace UserFunction {
    std::string login(std::string *acc);
    std::string registerUser();
    std::string sendMsg();
    std::string addFriend();
    std::string logout(std::string &acc);



};
#endif //USERFUNCTION_H
