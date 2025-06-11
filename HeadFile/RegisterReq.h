#pragma once
#include "Request.h"

class Response;

class RegisterReq {
private:
    Request _reqHead;
    std::string _acc;
    std::string _pwd;
public:
    RegisterReq(std::string acc, std::string pwd);
    RegisterReq(std::string);
    RegisterReq(std::string, int type);

    void toRegisterReq(std::string &info);
    std::string toString();
    Response handler() const;
    std::string getAccount() const { return _acc;};
};
