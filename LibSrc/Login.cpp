#include "Login.h"
#include "MysqlConnGuard.h"

Login::Login(std::string acc, std::string pwd, bool keepLogin)
    : _acc(acc), _pwd(pwd), _keepLogin(keepLogin)
{
}

std::string Login::validateLogin()
{
    MysqlConnGuard mysql_db;
    std::string query = "select pwd from test where acc = '" + _acc + "'";
    MYSQL_RES *res;
    if (mysql_db.isValid()) {
        res = (*mysql_db)->Query(query);
    } else {
        return "Error in query";
    }
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row == nullptr)
            return "no such account";
        else if (row[0] != _pwd)
            return "error account or password";
        else
            return "";
    }
    return "error res";
}
