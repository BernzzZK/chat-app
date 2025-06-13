//
// Created by xwz on 25-6-13.
//

#include "ChangeUserName.h"

#include <muduo/base/Logging.h>

#include "DBConnGuard.h"

ChangeUserName::ChangeUserName(const std::string& acc, const std::string& newName)
        : _acc(acc)
        , _newName(newName)
{
}

std::string ChangeUserName::changeUserName() const {
        MysqlConnGuard mysql;
        // 检查账号是否存在
        std::string sql = "select ID from User where acc='" + _acc + "'";
        MYSQL_RES *res;
        if (mysql.isValid()) {
                res = (*mysql)->Query(sql);
        } else {
                return "数据库错误，请稍后再试";
        }
        if (res) {
                MYSQL_ROW row = mysql_fetch_row(res);
                if (row == nullptr)
                        return "账号不存在";
        }

        sql = "update User set username='" + _newName + "' where acc='" + _acc + "'";
        if (mysql.isValid()) {
                res = (*mysql)->Query(sql);
        }
        if (res) {
                return "修改成功";
        }
        return "修改失败";
}

