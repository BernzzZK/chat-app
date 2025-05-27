#include "MysqlConn.h"
#include <muduo/base/Logging.h>

MysqlConn::MysqlConn(const std::string &host, const std::string &user, const std::string &pwd,
                     const std::string &db, unsigned int port, const std::string &charSet)
    : conn_(mysql_init(nullptr)), host_(host), user_(user), pwd_(pwd), db_(db), port_(port), char_set_(charSet)
{
    if (conn_ == nullptr)
    {
        LOG_ERROR << "MySQL init error!";
        return;
    }
    if (!mysql_real_connect(conn_, host_.c_str(), user_.c_str(), pwd_.c_str(), db_.c_str(), port_, nullptr, 0))
    {
        LOG_ERROR << "MySQL connect error!";
        mysql_close(conn_);
        conn_ = nullptr;
        return;
    }
    mysql_set_character_set(conn_, char_set_.c_str());
}

MysqlConn::~MysqlConn()
{
    closeConn();
}

MYSQL_RES *MysqlConn::Query(const std::string &query)
{
    if (mysql_query(conn_, query.c_str())) {
        LOG_ERROR << "MySQL query error!";
        return nullptr;
    }
    MYSQL_RES *res = mysql_store_result(conn_);
    return res;
}

bool MysqlConn::AutoCommit(int mode)
{
    if (mysql_autocommit(conn_, mode))
    {
        LOG_ERROR << "MySQL autocommit error!";
        return false;
    }
    if (mode == 0)
        LOG_INFO << "MySQL autocommit close!";
    else
        LOG_INFO << "MySQL autocommit open!";
    return true;
}

bool MysqlConn::Commit()
{
    if (mysql_commit(conn_))
    {
        LOG_ERROR << "MySQL commit fail! Begin to rollback ...";
        Rollback();
        return false;
    }
    else
    {
        LOG_INFO << "MySQL commit success!";
        return true;
    }
}

void MysqlConn::Rollback()
{
    if (mysql_rollback(conn_))
        LOG_ERROR << "MySQL rollback fail!";
    else
        LOG_INFO << "MySQL rollback success!";
}

bool MysqlConn::isAlive() const
{
    return conn_ != nullptr && mysql_ping(conn_) == 0;
}

// MYSQL *MysqlConn::getRaw() const
// {
//     return conn_;
// }

void MysqlConn::closeConn()
{
    if (conn_)
    {
        mysql_close(conn_);
        conn_ = nullptr;
    }
}