#include "Server.h"
#include "Common.h"
#include "DBConnGuard.h"
#include "Response.h"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "FriendApplication.h"
#include "HeartBeat.h"
#include "FriendList.h"
#include "Message.h"
#include "Req.h"

using namespace std::placeholders;

Server::Server(net::EventLoop *loop, const net::InetAddress &listenaddr, const std::string &fileName = "")
    : server_(loop, listenaddr, "chat server"), fileName_(fileName)
{
    server_.setConnectionCallback(bind(&Server::onConnection, this, _1));
    server_.setMessageCallback(bind(&Server::onMessage, this, _1, _2, _3));
    server_.setThreadNum(4);
    LoadConfig(fileName_);
    MysqlConnPool::instance().init(
        mysqlConfig_.host,
        mysqlConfig_.user,
        mysqlConfig_.pwd,
        mysqlConfig_.db,
        mysqlConfig_.port,
        mysqlConfig_.char_set,
        mysqlConfig_.min_conn,
        mysqlConfig_.max_conn,
        mysqlConfig_.max_wait_ms);
    RedisConnPool::instance().init(
        redisConfig_.host,
        redisConfig_.port,
        redisConfig_.min_conn,
        redisConfig_.max_conn,
        redisConfig_.max_wait_ms);
}

void Server::onMessage(const net::TcpConnectionPtr &conn, net::Buffer *buff, Timestamp time)
{
    std::string msg(buff->retrieveAllAsString());
    LOG_INFO << "msg: " << msg;
    if (msg == "ping")
    {
        conn->send("ping");
        return;
    } else if (msg[0] == '#') {
        auto type = static_cast<type::reqType>(std::stoi(*(common::splitString(msg).end() - 1)));
        if (type == type::addFriend) {
            FriendApplication friendApplication(msg);
        }
        return;
    }
    try
    {
        auto type = static_cast<type::reqType>(std::stoi(common::parsing(msg)));
        if (type == type::registered)
        {
            RegisterReq registerReq(msg);
            const Response resp = registerReq.handler();
            conn->send(resp.toString() + "\r\n");
        }
        else if (type == type::login)
        {
            LoginReq loginReq(msg);
            Response resp = loginReq.handler();
            if(resp.isSuccess()) {
                std::lock_guard lock(connMutex_);
                const std::string user = loginReq.getAccount();
                if (loginUser_.find(user) == loginUser_.end())
                {
                    if (resp.isSuccess()) {
                        loginUser_[user] = conn;
                        {
                            RedisConnGuard redisGuard;
                            if (redisGuard.isValid()) {
                                (*redisGuard)->execute("sadd loginUser " + user);
                            }   
                        }
                        conn->send(resp.toString() + "\r\n");
                        // @TODO: 登录时触发，检查该用户是否有未读信息或好友请求
                        hasUnreadMsg(user, conn);
                        hasUnprocessAddFriend(user, conn);
                        friendList(user, conn);
                    }
                } else {
                    Response repeatLogin(type::login, false, "account already login");
                    conn->send(repeatLogin.toString() + "\r\n");
                }
            } else {
                conn->send(resp.toString() + "\r\n");
            }
        }
        else if (type == type::logout)
        {
            LogoutReq logoutReq(msg);
            Response resp = logoutReq.handler();
            std::lock_guard lock(connMutex_);
            auto it = loginUser_.find(logoutReq.getAccount());
            if (it != loginUser_.end())
            {
                loginUser_.erase(it);
                conn->send(resp.toString());
                RedisConnGuard redisGuard;
                if (redisGuard.isValid())
                {
                    (*redisGuard)->execute("srem loginUser " + logoutReq.getAccount());
                }
            }
            else
            {
                Response repeatLogout(type::logout, false, "account not login");
                conn->send(repeatLogout.toString() + "\r\n");
            }
        }
        else if (type == type::sendMsg)
        {
            LOG_INFO << "msg: " << msg;
            SendMsgReq sendMsgReq(msg);
            Response resp = sendMsgReq.handler();
            // @TODO: 发送给对方(重新设计一个类，封装发送信息的逻辑)
            if (resp.isSuccess())
            {
                const auto &to = sendMsgReq.getReceiver();
                std::lock_guard lock(connMutex_);
                auto it = loginUser_.find(to);
                if (it != loginUser_.end())
                {
                    hasUnreadMsg(to, it->second);
                }
            }
            conn->send(resp.toString() + "\r\n");
        }
        else if (type == type::addFriend)
        {
            AddFriendReq addFriendReq(msg);
            Response resp = addFriendReq.handler();
            // @TODO: 发送给对方(重新设计一个类，封装发送好友请求的逻辑)
            if (resp.isSuccess())
            {
                const auto &to = addFriendReq.getAccount();
                std::lock_guard lock(connMutex_);
                auto it = loginUser_.find(to);
                if (it != loginUser_.end())
                {
                    hasUnprocessAddFriend(to, it->second);
                }
            }
            conn->send(resp.toString() + "\r\n");
        }
        else if (type == type::heartbeat) {
            HeartBeat hb(msg);
            auto resp = hb.handler();
            conn->send(resp.toString() + "\r\n");
        }
    }
    catch (const std::invalid_argument &e)
    {
        LOG_ERROR << "Invalid argument: " << e.what();
    }
}

void Server::onConnection(const net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        LOG_INFO << "New connection: " << conn->name();
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            connections_[conn->name()] = conn;
        }
        // 发送欢迎消息
        conn->send("Welcome to chat room!\r\n");
    }
    else
    {
        LOG_INFO << "Connection closed: " << conn->name();
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            for (auto it : loginUser_)
            {
                if (it.second == conn)
                {
                    const auto acc = it.first;
                    loginUser_.erase(acc);
                    RedisConnGuard redisGuard;
                    if (redisGuard.isValid())
                    {
                        (*redisGuard)->execute("srem loginUser " + acc);
                    }
                    break;
                }
            }
            connections_.erase(conn->name());
        }
    }
}

void Server::start()
{
    server_.start();
}

Server::~Server()
{
    MysqlConnPool::instance().stop();
    RedisConnPool::instance().stop();
}

bool Server::LoadConfig(const std::string &filename)
{
    try
    {
        YAML::Node config = YAML::LoadFile(filename);

        // 解析Redis配置
        if (config["redis"])
        {
            YAML::Node redisNode = config["redis"];
            if (redisNode["host"])
                redisConfig_.host = redisNode["host"].as<std::string>();
            if (redisNode["port"])
                redisConfig_.port = redisNode["port"].as<unsigned int>();
            if (redisNode["min_conn"])
                redisConfig_.min_conn = redisNode["min_conn"].as<int>();
            if (redisNode["max_conn"])
                redisConfig_.max_conn = redisNode["max_conn"].as<int>();
            if (redisNode["max_wait_ms"])
                redisConfig_.max_wait_ms = redisNode["max_wait_ms"].as<int>();
        }

        // 解析MySQL配置
        if (config["mysql"])
        {
            YAML::Node mysqlNode = config["mysql"];
            if (mysqlNode["host"])
                mysqlConfig_.host = mysqlNode["host"].as<std::string>();
            if (mysqlNode["user"])
                mysqlConfig_.user = mysqlNode["user"].as<std::string>();
            if (mysqlNode["pwd"])
                mysqlConfig_.pwd = mysqlNode["pwd"].as<std::string>();
            if (mysqlNode["db"])
                mysqlConfig_.db = mysqlNode["db"].as<std::string>();
            if (mysqlNode["port"])
                mysqlConfig_.port = mysqlNode["port"].as<unsigned int>();
            if (mysqlNode["char_set"])
                mysqlConfig_.char_set = mysqlNode["char_set"].as<std::string>();
            if (mysqlNode["min_conn"])
                mysqlConfig_.min_conn = mysqlNode["min_conn"].as<int>();
            if (mysqlNode["max_conn"])
                mysqlConfig_.max_conn = mysqlNode["max_conn"].as<int>();
            if (mysqlNode["max_wait_ms"])
                mysqlConfig_.max_wait_ms = mysqlNode["max_wait_ms"].as<int>();
        }
        return true;
    }
    catch (const YAML::Exception &e)
    {
        LOG_ERROR << "Config Error: " << e.what();
        return false;
    }
}

void Server::hasUnreadMsg(const std::string &user, const net::TcpConnectionPtr &conn)
{
    // 检查未读信息并发送
    MysqlConnGuard mysqlGuard;
    if (!mysqlGuard.isValid())
    {
        LOG_ERROR << "mysql connection error";
        return;
    }
    std::string sql = "SELECT m.ID, m.content, m.create_time, m.is_read, "
                      "sender.username AS sender_username, sender.account AS sender_account "
                      "FROM Offline_Message m "
                      "INNER JOIN User receiver ON m.receiver_id = receiver.ID "
                      "INNER JOIN User sender ON m.sender_id = sender.ID "
                      "WHERE receiver.account = '" +
                      user + "' AND m.is_read = 0;";
    auto result = (*mysqlGuard)->Query(sql);
    if (result != nullptr)
    {
        while (auto row = mysql_fetch_row(result))
        {
            std::string msgId = row[0];
            std::string content = row[1];
            std::string createTime = row[2];
            std::string isRead = row[3];
            std::string senderUsername = row[4];
            std::string senderAccount = row[5];
            Message msg(msgId, content, createTime, senderUsername, senderAccount);
            conn->send(msg.toString() + "\r\n");
            sql = "UPDATE Offline_Message"
                  "SET is_read = 1 "
                  "WHERE ID = " + msgId + ";";
            (*mysqlGuard)->Query(sql);
        }
    }
    
}

void Server::hasUnprocessAddFriend(const std::string &user, const net::TcpConnectionPtr &conn)
{
    // 检查未处理的好友请求并发送
    MysqlConnGuard mysqlGuard;
    std::string sql = "SELECT f.req_id, u.account AS from_account, u.username AS from_username, "
                      "f.create_time, f.`status` "
                      "FROM Friend_Req f "
                      "INNER JOIN User u ON f.from_id = u.ID "
                      "WHERE f.to_id = (SELECT ID FROM User WHERE account = '"+ user +"') "
                      "AND f.status IN ('pending', 'rejected');";
    auto result = (*mysqlGuard)->Query(sql);
    if (result != nullptr)
    {
        while (auto row = mysql_fetch_row(result))
        {
            std::string id = row[0];
            std::string fromAccount = row[1];
            std::string fromUsername = row[2];
            std::string createTime = row[3];
            std::string status = row[4];
            FriendApplication addFriendReq(id, createTime, status, fromUsername, fromAccount);
            conn->send(addFriendReq.toString() + "\r\n");
        }
    }
}

void Server::friendList(const std::string &user, const net::TcpConnectionPtr &conn)
{
    MysqlConnGuard mysqlGuard;
    std::string sqlQuery =
        "SELECT u.account, u.username, f.created_time "
        "FROM Friend f "
        "INNER JOIN User u ON (f.friend_id = u.ID) "
        "WHERE f.user_id = (SELECT ID FROM User WHERE account = '" +
        user + "') "
               "UNION "
               "SELECT u.account, u.username, f.created_time "
               "FROM Friend f "
               "INNER JOIN User u ON (f.user_id = u.ID) "
               "WHERE f.friend_id = (SELECT ID FROM User WHERE account = '" +
        user + "');";
    auto result = (*mysqlGuard)->Query(sqlQuery);
    if (result != nullptr)
    {
        while (auto row = mysql_fetch_row(result))
        {
            std::string friendAccount = row[0];
            std::string friendUsername = row[1];
            std::string createTime = row[2];
            bool isOnline;
            if (loginUser_.find(friendAccount) != loginUser_.end()) {
                isOnline = true;
            } else {
                isOnline = false;
            }
            FriendList friendList(friendAccount, friendUsername, createTime, isOnline);
            conn->send(friendList.toString() + "\r\n");
        }
    }
}

// void Server::isAlive()
// {
//     std::thread aliveThread([this]() {
//         while (true) {
//             sleep(25);
//             std::lock_guard<std::mutex> lock(connMutex_);
//             // 移除不在reply_的连接
//             for (auto it = connections_.begin(); it != connections_.end();) {
//                 if (reply_.find(it->second) == reply_.end()) {
//                     it = connections_.erase(it);
//                 } else {
//                     ++it;
//                 }
//             }
//             for (auto it : connections_) {
//                 if (it.second->connected()) {
//                     it.second->send("ping");
//                 }
//             }
//         } });
//     aliveThread.detach();
// }
