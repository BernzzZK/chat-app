//
// Created by xwz on 25-6-13.
//

#ifndef CHANGEUSERNAME_H
#define CHANGEUSERNAME_H

#include <string>

class ChangeUserName {
private:
    std::string _acc;
    std::string _newName;
public:
    ChangeUserName(const std::string& acc, const std::string& newName);
    std::string changeUserName() const ;
};

#endif //CHANGEUSERNAME_H
