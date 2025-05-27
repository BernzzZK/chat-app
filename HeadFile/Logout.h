//
// Created by xwz on 25-5-26.
//

#ifndef LOGOUT_H
#define LOGOUT_H
#include <string>


class Logout {
private:
    std::string _acc;
public:
    Logout(const std::string &acc);
    std::string logout() const;
};



#endif //LOGOUT_H
