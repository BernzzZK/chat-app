#pragma once
#include "Request.h"

class LoginReq {
private:

    std::string _acc;
    std::string _pwd;
    bool _keepLogin;
    Request _reqHead;

public:
    Response handler() const;
    std::string toString();
    void toLoginReq(std::string);

    LoginReq(std::string);
    ~LoginReq() {};
};