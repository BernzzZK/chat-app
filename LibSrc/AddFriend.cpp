//
// Created by xwz on 25-5-27.
//

#include "AddFriend.h"
#include <muduo/base/Logging.h>
#include "MysqlConnGuard.h"

bool sendFriendRequest(MysqlConnGuard& mysql_db, const std::string& _curr_acc, const std::string& _friendName) {
    // 首先查询发送者的ID
    std::string get_sender_id_query = "SELECT ID FROM User WHERE account = '" + _curr_acc + "'";
    MYSQL_RES *sender_res = (*mysql_db)->Query(get_sender_id_query);
    if (!sender_res) {
        LOG_ERROR << "Failed to query sender ID.";
        return false;
    }

    MYSQL_ROW sender_row = mysql_fetch_row(sender_res);
    if (sender_row == nullptr || sender_row[0] == nullptr) {
        LOG_ERROR << "Sender account not found: " << _curr_acc;
        mysql_free_result(sender_res);
        return false;
    }
    int from_id = atoi(sender_row[0]);
    mysql_free_result(sender_res);

    // 然后查询接收者的ID
    std::string get_receiver_id_query = "SELECT ID FROM User WHERE account = '" + _friendName + "'";
    MYSQL_RES *receiver_res = (*mysql_db)->Query(get_receiver_id_query);
    if (!receiver_res) {
        LOG_ERROR << "Failed to query receiver ID.";
        return false;
    }

    MYSQL_ROW receiver_row = mysql_fetch_row(receiver_res);
    if (receiver_row == nullptr || receiver_row[0] == nullptr) {
        LOG_ERROR << "Receiver account not found: " << _friendName;
        mysql_free_result(receiver_res);
        return false;
    }
    int to_id = atoi(receiver_row[0]);
    mysql_free_result(receiver_res);

    // 构造并执行插入好友请求的SQL语句
    std::string insert_friend_request_query = "INSERT INTO Friend_Req (from_id, to_id, status, create_time) VALUES ("
                                              + std::to_string(from_id) + ", "
                                              + std::to_string(to_id) + ", 'pending', CURRENT_TIMESTAMP)";

    if ((*mysql_db)->Query(insert_friend_request_query.c_str()) == nullptr) {
        LOG_ERROR << "Failed to insert friend request.";
        return false;
    }
    return true;
}

AddFriend::AddFriend(const std::string &friendName, const std::string &curr_acc)
        : _friendName(friendName)
        , _curr_acc(curr_acc)
{
}

std::string AddFriend::addFriend() {
    MysqlConnGuard mysql_db;
    if (!mysql_db.isValid()) {
        return "Error in query";
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

    // 插入好友请求
    if (sendFriendRequest(mysql_db, _curr_acc, _friendName)) {
        return "";
    } else {
        return "Error to send request";
    }
}
