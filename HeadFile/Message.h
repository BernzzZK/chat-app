//
// Created by xwz on 25-6-8.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include "Request.h"

class Message
{
public:
    Message(std::string id, std::string content, std::string time, bool isRead, std::string friendUserName, std::string friendAcc);
    Message(const std::string &info);
    Message(const Message &message) = delete;
    Message &operator=(const Message &message) = delete;

    std::string toString() const;
    void setRead() { isRead_ = true; }

private:
    type::reqType reqType_;
    std::string id_;
    std::string content_;
    std::string time_;
    bool isRead_;
    std::string friendUserName_;
    std::string friendAcc_;
};

#endif // MESSAGE_H
