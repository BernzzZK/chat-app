//
// Created by xwz on 25-6-10.
//

#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include "Request.h"
#include <string>

class FriendList {
public:
    FriendList(std::string userName, std::string userAcc, std::string createTime, bool isOnline);
    FriendList(const std::string &info);
    FriendList(const FriendList &friendList) = delete;
    FriendList &operator=(const FriendList &friendList) = delete;
    std::string toString() const;
private:
    reqType reqType_;
    std::string userName_;
    std::string userAcc_;
    std::string createTime_;
    bool isOnline_;
};

#endif //FRIENDLIST_H
