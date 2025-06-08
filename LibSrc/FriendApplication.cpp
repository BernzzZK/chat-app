//
// Created by xwz on 25-6-8.
//

#include "FriendApplication.h"

#include <sstream>
#include <muduo/base/Logging.h>

#include "Common.h"

FriendApplication::FriendApplication(std::string id, std::string time, std::string status, std::string senderUserName, std::string senderAcc)
    : id_(id)
    , time_(time)
    , status_(status)
    , senderUserName_(senderUserName)
    , senderAcc_(senderAcc)
    , reqType_(addFriend)
{
}

FriendApplication::FriendApplication(const std::string &info) {
    auto elements = common::splitString(info);
    if (elements.size()!= 5) {
        LOG_ERROR << "Invalid format";
    }
    id_ = elements[0];
    time_ = elements[1];
    status_ = elements[2];
    senderUserName_ = elements[3];
    senderAcc_ = elements[4];
    reqType_ = addFriend;
}

std::string FriendApplication::toString() const {
    std::stringstream ss;
    ss << "#" <<id_ << "#" << time_ << "#" << status_ << "#" << senderUserName_ << "#" << senderAcc_ << "#" << reqType_;
    return ss.str();
}

