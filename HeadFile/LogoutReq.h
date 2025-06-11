//
// Created by xwz on 25-5-26.
//

#ifndef LOGOUTREQ_H
#define LOGOUTREQ_H
#include "Request.h"

class Response;

class LogoutReq {
private:
    Request _reqHead;
    std::string _acc;
public:
    LogoutReq(const std::string& acc);
    LogoutReq(std::string, int type);

    void toLogoutReq(std::string &info);
    std::string toString();
    Response handler() const;
    std::string getAccount() const { return _acc;};
};

#endif //LOGOUTREQ_H
