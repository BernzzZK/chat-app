//
// Created by xwz on 25-5-27.
//

#include "AddFriend.h"
#include <muduo/base/Logging.h>
#include "MysqlConnGuard.h"
#include <algorithm>

// bool sendFriendRequest(const std::string& _curr_acc, const std::string& _friendName) {
//     MysqlConnGuard mysql_db;
//
// }

AddFriend::AddFriend(const std::string &friendName, const std::string &curr_acc)
        : _friendName(friendName)
        , _curr_acc(curr_acc)
{

    auto removeNewlines = [](std::string &str) {
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    };
    removeNewlines(_friendName);
    removeNewlines(_curr_acc);
}

std::string AddFriend::addFriend() {
    MysqlConnGuard mysql_db;
    bool flag = false;
    if (!mysql_db.isValid()) {
        return "Error in db";
    }
    // 检查好友是否存在
    std::string query = "select account from User where account = '" + _friendName + "'";
    MYSQL_RES *res = (*mysql_db)->Query(query);
    if (res == nullptr) {
        return "Error in query";
    }
    if (mysql_num_rows(res) == 0) {
        return "no such user";
    }
    mysql_free_result(res);

    // 检查是否已经是好友
    query = "SELECT COUNT(*) FROM Friend f "
            "JOIN User u1 ON f.user_id = u1.ID "
            "JOIN User u2 ON f.friend_id = u2.ID "
            "WHERE (u1.account = '" + _curr_acc + "' AND u2.account = '" + _friendName + "') OR "
            "      (u1.account = '" + _friendName + "' AND u2.account = '" + _curr_acc + "')";

    MYSQL_RES* friend_res = (*mysql_db)->Query(query);
    if (friend_res) {
        MYSQL_ROW row = mysql_fetch_row(friend_res);
        int count = atoi(row[0]);
        if (count > 0) {
            return "already friend";
        }
        mysql_free_result(friend_res);
    } else {
        return "Error in query";
    }

    // 检查是否已经发送过好友请求
    query = "SELECT COUNT(*) FROM Friend_Req fr "
            "JOIN User u1 ON fr.from_id = u1.ID "
            "JOIN User u2 ON fr.to_id = u2.ID "
            "WHERE u1.account = '" + _curr_acc + "' AND u2.account = '" + _friendName + "'";
    MYSQL_RES* request_res = (*mysql_db)->Query(query);
    if (request_res) {
        MYSQL_ROW row = mysql_fetch_row(request_res);
        int count = atoi(row[0]);
        if (count > 0) {
            return "already send request";
        }
        mysql_free_result(request_res);
    } else {
        return "Error in query";
    }

    // flag = true;
    // if (flag) {
    //     // 插入好友请求
    //     if (sendFriendRequest(_curr_acc, _friendName)) {
    //         return "";
    //     } else {
    //         return "Error to send request";
    //     }
    // }
    // 首先查询发送者的ID
    std::string get_sender_id_query = "SELECT ID FROM User WHERE account = '" + _curr_acc + "'";
    MYSQL_RES *sender_res = (*mysql_db)->Query(get_sender_id_query);
    if (!sender_res) {
        LOG_ERROR << "Failed to query sender ID.";
        return "Failed to query sender ID.";
    }

    MYSQL_ROW sender_row = mysql_fetch_row(sender_res);
    if (sender_row == nullptr) {
        LOG_ERROR << "Sender account not found: " << _curr_acc;
        mysql_free_result(sender_res);
        return "Sender account not found";
    }
    int from_id = atoi(sender_row[0]);
    mysql_free_result(sender_res);

    // 然后查询接收者的ID
    std::string get_receiver_id_query = "SELECT ID FROM User WHERE account = '" + _friendName + "'";
    MYSQL_RES *receiver_res = (*mysql_db)->Query(get_receiver_id_query);
    if (!receiver_res) {
        return "Failed to query receiver ID.";
    }

    MYSQL_ROW receiver_row = mysql_fetch_row(receiver_res);
    if (receiver_row == nullptr) {
        LOG_ERROR << "Receiver account not found: " << _friendName;
        mysql_free_result(receiver_res);
        return "Receiver account not found";
    }
    int to_id = atoi(receiver_row[0]);
    mysql_free_result(receiver_res);

    // 构造并执行插入好友请求的SQL语句
    std::string insert_friend_request_query = "insert into Friend_Req(from_id, to_id, status, create_time) values('"
                                              + std::to_string(from_id) + "', '"
                                              + std::to_string(to_id) + "', 'pending', CURRENT_TIMESTAMP)";
    if ((*mysql_db)->Query(insert_friend_request_query) != nullptr) {
        return "fail to send request";
    }
    return "";
}
