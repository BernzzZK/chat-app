//
// Created by xwz on 25-5-26.
//

#include "Register.h"
#include <muduo/base/Logging.h>
#include "DBConnGuard.h"
#include "UniqueHashGenerator.h"
#include "Common.h"

Register::Register(std::string acc, std::string pwd)
    : _acc(acc)
    , _pwd(pwd)
    , _created_time(muduo::Timestamp::now().toFormattedString())
{
}

std::string Register::validateRegister() const {
    MysqlConnGuard mysql_db;
    common::removeNewline(_acc);
    common::removeNewline(_pwd);

    // 2. 检查账号是否已存在
    const std::string query = "SELECT account FROM User WHERE account = '" + _acc + "'";
    MYSQL_RES *res = (*mysql_db)->Query(query);
    if (!res) {
        return "数据库查询错误，请稍后再试";
    }

    UniqueHashGenerator hashGenerator;
    const std::string id = hashGenerator(_acc);

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row != nullptr) {
        return "账号已存在";
    }

    const std::string insert =
        "INSERT INTO User(ID, username, account, password, role, created_time) "
        "VALUES( " + id +", '默认名称', '" + _acc + "', '" + _pwd + "', 'user', CURRENT_TIMESTAMP)";
    if ((*mysql_db)->Query(insert)) {
        return "数据库错误，请稍后再试";
    }
    return ""; // 成功
}
