//
// Created by xwz on 25-5-27.
//

#ifndef ADDFRIENDREQ_H
#define ADDFRIENDREQ_H

#include "Request.h"

class Response;

class AddFriendReq {
private:
    Request _reqHead;
    std::string _curr_acc;
    std::string _friendName;

public:
    AddFriendReq(const std::string &friendName);
    AddFriendReq(const std::string &friendName, const std::string &curr_acc);

    void toAddFriendReq(std::string &info);
    std::string toString();
    Response handler() const;
    std::string getAccount() const { return _friendName; };
};

#endif //ADDFRIENDREQ_H
