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
    std::string _created_time;
public:
    Register(std::string acc, std::string pwd);

    std::string validateRegister() const;
};



#endif //REGISTER_H
