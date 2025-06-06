//
// Created by xwz on 25-5-27.
//

#include "AddFriend.h"
#include <muduo/base/Logging.h>
#include "DBConnGuard.h"
#include "Common.h"

AddFriend::AddFriend(const std::string &friendName, const std::string &curr_acc)
        : _friendName(friendName)
        , _curr_acc(curr_acc)
{
    common::removeNewline(_friendName);
    common::removeNewline(_curr_acc);
}

std::string AddFriend::addFriend() {
    MysqlConnGuard mysql_db;
   if (!mysql_db.isValid()) {
        return "Error in db";
    }

    // 工具 lambda：执行查询并返回行数
    auto getCountFromQuery = [&](const std::string& query) -> int {
        MYSQL_RES* res = (*mysql_db)->Query(query);
        if (!res) return -1;
        MYSQL_ROW row = mysql_fetch_row(res);
        int count = row ? atoi(row[0]) : 0;
        mysql_free_result(res);
        return count;
   };

    // 工具 lambda：执行查询并返回单个字段值（字符串）
    auto getSingleValue = [&](const std::string& query) -> std::string {
        MYSQL_RES* res = (*mysql_db)->Query(query);
        if (!res) return "";
        MYSQL_ROW row = mysql_fetch_row(res);
        std::string value = row ? row[0] : "";
        mysql_free_result(res);
        return value;
    };

    // 1. 获取发送者和接收者的ID
    std::string getSenderIdQuery = "SELECT ID FROM User WHERE account = '" + _curr_acc + "'";
    std::string from_id_str = getSingleValue(getSenderIdQuery);
    if (from_id_str.empty()) {
        return "Sender account not found";
    }
    int from_id = atoi(from_id_str.c_str());

    std::string getReceiverIdQuery = "SELECT ID FROM User WHERE account = '" + _friendName + "'";
    std::string to_id_str = getSingleValue(getReceiverIdQuery);
    if (to_id_str.empty()) {
        return "Receiver account not found";
    }
    int to_id = atoi(to_id_str.c_str());

    // 2. 检查好友是否存在（直接判断 to_id 是否有效即可）
    // 如果上面能取到 to_id，说明用户存在，所以这步可以省略或简化为 true
    if (to_id <= 0) {
        return "no such user";
    }

    // 3. 检查是否已经是好友
    std::string checkFriendQuery = "SELECT COUNT(*) FROM Friend WHERE (user_id = " + std::to_string(from_id) +
                                   " AND friend_id = " + std::to_string(to_id) + ") OR "
                                   "(user_id = " + std::to_string(to_id) +
                                   " AND friend_id = " + std::to_string(from_id) + ")";
    int isAlreadyFriend = getCountFromQuery(checkFriendQuery);
    if (isAlreadyFriend < 0) return "Error in query";
    if (isAlreadyFriend > 0) return "already friend";

    // 4. 检查是否已经发送过请求
    std::string checkRequestQuery = "SELECT COUNT(*) FROM Friend_Req WHERE from_id = " +
                                    std::to_string(from_id) + " AND to_id = " + std::to_string(to_id);
    int hasSentRequest = getCountFromQuery(checkRequestQuery);
    if (hasSentRequest < 0) return "Error in query";
    if (hasSentRequest > 0) return "already send request";

    // 5. 插入好友请求
    std::string insert_friend_request_query = "INSERT INTO Friend_Req "
                                              "(from_id, to_id, status, create_time) VALUES ("
                                              + std::to_string(from_id) + ", "
                                              + std::to_string(to_id) + ", "
                                              "'pending', CURRENT_TIMESTAMP)";
    if ((*mysql_db)->Query(insert_friend_request_query) != nullptr) {
        return "fail to send request";
    }

    return "";
}
