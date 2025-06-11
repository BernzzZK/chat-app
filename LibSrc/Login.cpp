#include "Login.h"
#include "DBConnGuard.h"
#include "Common.h"

Login::Login(std::string acc, std::string pwd)
    : _acc(acc), _pwd(pwd)
{
}

std::string Login::validateLogin()
{
    MysqlConnGuard mysql_db;
    common::removeNewline(_acc);
    common::removeNewline(_pwd);
    std::string query = "select password from User where account = '" + _acc + "'";
    MYSQL_RES *res;
    if (mysql_db.isValid()) {
        res = (*mysql_db)->Query(query);
    } else {
        return "数据库查询错误，请稍后再试";
    }
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row == nullptr)
            return "账号不存在";
        else if (row[0] != _pwd)
            return "账号或密码错误";
        else
            return "";
    }
    return "数据库结果错误，请稍后再试";
}
