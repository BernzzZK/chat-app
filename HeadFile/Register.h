//
// Created by xwz on 25-5-26.
//

#ifndef REGISTER_H
#define REGISTER_H
#include <string>

class Register {
private:
    std::string &_acc;
    std::string &_pwd;

public:
    Register(std::string acc, std::string pwd);

    std::string validateRegister() const;
};



#endif //REGISTER_H
