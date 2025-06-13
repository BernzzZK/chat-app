//
// Created by xwz on 25-6-8.
//

#include "FriendApplication.h"

#include <sstream>
#include <muduo/base/Logging.h>

#include "Common.h"
#include "DBConnGuard.h"

FriendApplication::FriendApplication(std::string id, std::string time, std::string status, std::string senderUserName, std::string senderAcc)
    : id_(id)
    , time_(time)
    , status_(status)
    , senderUserName_(senderUserName)
    , senderAcc_(senderAcc)
    , reqType_(type::addFriend)
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
    reqType_ = type::addFriend;
}

std::string FriendApplication::toString() const {
    std::stringstream ss;
    ss << "#" <<id_ << "#" << time_ << "#" << status_ << "#" << senderUserName_ << "#" << senderAcc_ << "#" << reqType_;
    return ss.str();
}

Response FriendApplication::handle() {
    if (status_ == "accepted") {
        MysqlConnGuard guard;
        // 获取双方ID
        std::string senderId, receiverId;
        std::string sql = "SELECT id FROM User WHERE account = '" + senderAcc_ + "'";
        senderId = common::getSingleValue(sql);
        sql = "SELECT to_id FROM Friend_Req WHERE req_id = '" + id_ + "'";
        receiverId = common::getSingleValue(sql);
        // 插入好友关系
        sql = "INSERT INTO Friend (from_id, to_id) VALUES ('" + senderId + "', '" + receiverId + "')";
        if ((*guard)->Query(sql)) {
            LOG_ERROR << "Failed to insert friend relation";
        }
        sql = "INSERT INTO Friend (from_id, to_id) VALUES ('" + receiverId + "', '" + senderId + "')";
        if ((*guard)->Query(sql)) {
            LOG_ERROR << "Failed to insert friend relation";
        }
        return Response(this->reqType_, "success", "You have accepted the friend request from " + senderUserName_);
    }
    if (status_ == "rejected") {
        MysqlConnGuard guard;
        std::string sql = "UPDATE Friend_Req set status = 'rejected' WHERE req_id = '" + id_ + "'";
        if ((*guard)->Query(sql)) {
            LOG_ERROR << "Failed to update friend request status";
        }
        return Response(this->reqType_, "success", "You have rejected the friend request from " + senderUserName_);
    }
    return Response(this->reqType_, "success", "");
}

