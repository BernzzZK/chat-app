//
// Created by xwz on 25-6-8.
//

#include "Message.h"

#include <sstream>
#include <muduo/base/Logging.h>

#include "Common.h"

Message::Message(std::string id, std::string content, std::string time, std::string friendUserName, std::string friendAcc)
    : id_(id)
    , content_(content)
    , time_(time)
    , friendUserName_(friendUserName)
    , friendAcc_(friendAcc)
    , reqType_(type::sendMsg)
{
    content_ = common::escapeIllegalCharacters(content_);
}

Message::Message(const std::string &info) {
    auto elements = common::splitString(info);
    if (elements.size() != 5) {
        LOG_ERROR << "Invalid format";
    }
    id_ = elements[0];
    content_ = elements[1];
    time_ = elements[2];
    friendUserName_ = elements[3];
    friendAcc_ = elements[4];
    reqType_ = type::sendMsg;
}

std::string Message::toString() const {
    std::stringstream ss;
    ss << "#" <<id_ << "#" << content_ << "#" << time_ << "#" << friendUserName_ << "#" << friendAcc_ << "#" << reqType_;
    return ss.str();
}


