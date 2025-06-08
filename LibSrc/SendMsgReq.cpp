//
// Created by xwz on 25-6-7.
//

#include "SendMsgReq.h"
#include <iostream>
#include <muduo/base/Logging.h>
#include "Common.h"
#include "DBConnGuard.h"

SendMsgReq::SendMsgReq(const std::string &msg, const std::string &sender_acc, const std::string &receiver_acc)
        : msg_(msg)
        , sender_acc_(sender_acc)
        , receiver_acc_(receiver_acc)
        , reqHead_(sendMsg, muduo::Timestamp::now().toFormattedString())
{
}

SendMsgReq::SendMsgReq(const std::string &info) {
    auto res = common::splitString(info);
    if (res.size() != 3) {
        LOG_ERROR << "Invalid request format";
    }
    reqHead_.set(static_cast<reqType>(std::stoi(res[0])), res[1]);
    toSendMsgReq(res[2]);
}

std::string SendMsgReq::toString() {
    std::string ret;
    ret.append("#" + msg_);
    ret.append("#" + sender_acc_);
    ret.append("#" + receiver_acc_);
    return reqHead_.toString() + "@" + ret;
}

void SendMsgReq::toSendMsgReq(const std::string &info) {
    auto res = common::splitString(info);
    msg_ = common::unescapeIllegalCharacters(res[0]);
    sender_acc_ = res[1];
    receiver_acc_ = res[2];
    common::removeNewline(msg_);
    common::removeNewline(sender_acc_);
    common::removeNewline(receiver_acc_);
}

Response SendMsgReq::handler() const {
    // 1. 检查发送方和接收方是否存在，同时获取其ID
    std::string sender_id, receiver_id;
    std::string getSenderIdQuery = "SELECT ID FROM User WHERE account = '" + sender_acc_ + "'";
    std::string from_id_str = common::getSingleValue(getSenderIdQuery);
    if (from_id_str.empty()) {
        LOG_ERROR << "Sender account not found";
        return Response(false, "Sender account not found");
    } else {
        sender_id = from_id_str;
    }

    std::string getReceiverIdQuery = "SELECT ID FROM User WHERE account = '" + receiver_acc_ + "'";
    std::string to_id_str = common::getSingleValue(getReceiverIdQuery);
    if (to_id_str.empty()) {
        LOG_ERROR << "Receiver account not found";
        return Response(false, "Receiver account not found");
    } else {
        receiver_id = to_id_str;
    }

    MysqlConnGuard mysql_db;
    if (!mysql_db.isValid()) {
        LOG_ERROR << "connect mysql failed";
        return Response(false, "connect mysql failed");
    }
    // 2. 插入消息记录到数据库
    std::string insertMsgQuery = "INSERT INTO Offline_Message (sender_id, receiver_id, content, create_time) VALUES (" +
                                 sender_id + ", " + receiver_id + ", '" + msg_ + "', CURRENT_TIMESTAMP)";
    if ((*mysql_db)->Query(insertMsgQuery) != nullptr) {
        return Response(false, "insert message failed");
    }
    return Response(true, "send message succeeded");
}
