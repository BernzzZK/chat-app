//
// Created by xwz on 25-6-8.
//

#ifndef FRIENDAPPLICATION_H
#define FRIENDAPPLICATION_H

#include "Request.h"

class FriendApplication
{
public:
    FriendApplication(std::string id, std::string time, std::string status, std::string senderUserName, std::string senderAcc);
    FriendApplication(const std::string &info);
    FriendApplication(const FriendApplication &message) = delete;
    FriendApplication &operator=(const FriendApplication &message) = delete;

    std::string toString() const;

private:
    type::reqType reqType_;
    std::string id_;
    std::string time_;
    std::string status_;
    std::string senderUserName_;
    std::string senderAcc_;
};

#endif // FRIENDAPPLICATION_H
