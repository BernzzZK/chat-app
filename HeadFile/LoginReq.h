#pragma once
#include "Request.h"

class Response;

class LoginReq {
private:
    std::string _acc;
    std::string _pwd;
    Request _reqHead;

public:
    Response handler() const;
    std::string toString();
    void toLoginReq(std::string);

    std::string getAccount() const { return _acc;}

    LoginReq(const std::string &);
    LoginReq(std::string, int type);

    LoginReq(const std::string & account, const std::string & password);

    ~LoginReq() {};
};