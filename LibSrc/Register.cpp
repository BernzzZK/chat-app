//
// Created by xwz on 25-5-26.
//

#include "Register.h"
#include <muduo/base/Logging.h>
#include "MysqlConnGuard.h"
#include <algorithm>
#include "UniqueHashGenerator.h"

Register::Register(std::string acc, std::string pwd)
    : _acc(acc)
    , _pwd(pwd)
    , _created_time(muduo::Timestamp::now().toFormattedString())
{
}

std::string Register::validateRegister() const {
    MysqlConnGuard mysql_db;

    // 1. 移除换行符
    auto removeNewlines = [](std::string &str) {
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    };
    removeNewlines(_acc);
    removeNewlines(_pwd);

    // 2. 检查账号是否已存在
    const std::string query = "SELECT account FROM User WHERE account = '" + _acc + "'";
    MYSQL_RES *res = (*mysql_db)->Query(query);
    if (!res) {
        return "Error in query";
    }

    UniqueHashGenerator hashGenerator;
    const std::string id = hashGenerator(_acc);

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row != nullptr) {
        return "Account already exists";
    }

    const std::string insert =
        "INSERT INTO User(ID, username, account, password, role, created_time) "
        "VALUES( " + id +", '默认名称', '" + _acc + "', '" + _pwd + "', 'user', CURRENT_TIMESTAMP)";
    if ((*mysql_db)->Query(insert)) {
        return "Error in insert";
    }
    return ""; // 成功
}
