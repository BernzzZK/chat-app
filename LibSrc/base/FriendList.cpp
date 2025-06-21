//
// Created by xwz on 25-6-10.
//

#include "FriendList.h"
#include <sstream>
#include <muduo/base/Logging.h>
#include "Common.h"

FriendList::FriendList(std::string friendAcc, std::string friendUserName, std::string createTime, bool isOnline)
    : userAcc_(friendAcc)
    , userName_(friendUserName)
    , createTime_(createTime)
    , isOnline_(isOnline)
    ,reqType_(type::friendList)
{
}

FriendList::FriendList(const std::string &info) {
    auto elements = common::splitString(info);
    if (elements.size()!= 4) {
        LOG_ERROR << "Invalid format";
    }
    userAcc_ = elements[0];
    userName_ = elements[1];
    createTime_ = elements[2];
    isOnline_ = std::stoi(elements[3]);
    reqType_ = type::friendList;
}

std::string FriendList::toString() const {
    std::stringstream ss;
    ss << "#" << userAcc_ << "#" << userName_ << "#" << createTime_ << "#" << isOnline_ << "#" << reqType_;
    return ss.str();
}
