#pragma once
#include <mysql/mysql.h>
#include <muduo/base/Timestamp.h>
#include <memory>
#include <string>

class MysqlConn
{
public:
    // 构造函数
    MysqlConn(const std::string &host, const std::string &user, const std::string &pwd,
              const std::string &db, unsigned int port = 3306, const std::string &charSet = "utf8");

    // 析构函数
    ~MysqlConn();

    // 执行查询
    MYSQL_RES *Query(const std::string &query);

    // 自动提交设置
    bool AutoCommit(int mode);

    // 提交事务
    bool Commit();

    // 回滚事务
    void Rollback();

    // 检查连接是否存活
    bool isAlive() const;

    // 获取原始MYSQL指针
    // MYSQL *getRaw() const;

private:
    // 关闭连接
    void closeConn();

    MYSQL *conn_; // MySQL连接
    std::string host_;
    std::string user_;
    std::string pwd_;
    std::string db_;
    unsigned int port_;
    std::string char_set_;
};
