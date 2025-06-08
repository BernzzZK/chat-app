#include "Server.h"

#include "AddFriendReq.h"
#include "LoginReq.h"
#include "RegisterReq.h"
#include "Common.h"
#include "DBConnGuard.h"
#include "LogoutReq.h"
#include "Response.h"
#include "MysqlConnPool.h"
#include "RedisConnPool.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "SendMsgReq.h"
using namespace std::placeholders;

Server::Server(net::EventLoop *loop, const net::InetAddress listenaddr, std::string fileName = "")
    : server_(loop, listenaddr, "chat server")
    , fileName_(fileName)
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
        mysqlConfig_.max_wait_ms
        );
    RedisConnPool::instance().init(
        redisConfig_.host,
        redisConfig_.port,
        redisConfig_.min_conn,
        redisConfig_.max_conn,
        redisConfig_.max_wait_ms
        );
}

void Server::onMessage(const muduo::net::TcpConnectionPtr &conn, net::Buffer *buff, Timestamp time) {
    std::string msg(buff->retrieveAllAsString());
    try{
        auto type = static_cast<reqType>(std::stoi(common::parsing(msg)));
        if (type == registered)
        {
            RegisterReq registerReq(msg);
            const Response resp = registerReq.handler();
            conn->send(resp.toString());
        }
        else if (type == login)
        {
            LoginReq loginReq(msg);
            Response resp = loginReq.handler();
            std::lock_guard lock(connMutex_);
            if (loginUser_.find(loginReq.getAccount()) == loginUser_.end()) {
                if (resp.isSuccess()) {
                    loginUser_[loginReq.getAccount()] = conn;
                    RedisConnGuard redisGuard;
                    if (redisGuard.isValid()) {
                        (*redisGuard)->execute("sadd loginUser " + loginReq.getAccount());
                    }
                }
            } else {
                Response repeatLogin(false, "account already login");
                conn->send(repeatLogin.toString());
            }
            conn->send(resp.toString());
        }
        else if (type == logout)
        {
            LogoutReq logoutReq(msg);
            Response resp = logoutReq.handler();
            std::lock_guard lock(connMutex_);
            auto it = loginUser_.find(logoutReq.getAccount());
            if (it != loginUser_.end()) {
                loginUser_.erase(it);
                conn->send(resp.toString());
                RedisConnGuard redisGuard;
                if (redisGuard.isValid()) {
                    (*redisGuard)->execute("srem loginUser " + logoutReq.getAccount());
                }
            } else {
                Response repeatLogout(false, "account not login");
                conn->send(repeatLogout.toString());
            }

        }
        else if (type == sendMsg)
        {
            LOG_INFO << "msg: " << msg;
            SendMsgReq sendMsgReq(msg);
            Response resp = sendMsgReq.handler();
            conn->send(resp.toString());
        }
        else if (type == addFriend)
        {
            AddFriendReq addFriendReq(msg);
            Response resp = addFriendReq.handler();
            if (resp.isSuccess()) {

            }
            conn->send(resp.toString());
        }
    } catch(const std::invalid_argument& e) {
       LOG_ERROR << "Invalid argument: " << e.what(); 
    }
}

void Server::onConnection(const muduo::net::TcpConnectionPtr &conn) {
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
            for (auto it : loginUser_) {
                if (it.second == conn) {
                    const auto acc = it.first;
                    loginUser_.erase(acc);
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

Server::~Server() {
    MysqlConnPool::instance().stop();
    RedisConnPool::instance().stop();
}

bool Server::LoadConfig(const std::string &filename) {
    try {
        YAML::Node config = YAML::LoadFile(filename);

        // 解析Redis配置
        if (config["redis"]) {
            YAML::Node redisNode = config["redis"];
            if (redisNode["host"]) redisConfig_.host = redisNode["host"].as<std::string>();
            if (redisNode["port"]) redisConfig_.port = redisNode["port"].as<unsigned int>();
            if (redisNode["min_conn"]) redisConfig_.min_conn = redisNode["min_conn"].as<int>();
            if (redisNode["max_conn"]) redisConfig_.max_conn = redisNode["max_conn"].as<int>();
            if (redisNode["max_wait_ms"]) redisConfig_.max_wait_ms = redisNode["max_wait_ms"].as<int>();
        }

        // 解析MySQL配置
        if (config["mysql"]) {
            YAML::Node mysqlNode = config["mysql"];
            if (mysqlNode["host"]) mysqlConfig_.host = mysqlNode["host"].as<std::string>();
            if (mysqlNode["user"]) mysqlConfig_.user = mysqlNode["user"].as<std::string>();
            if (mysqlNode["pwd"]) mysqlConfig_.pwd = mysqlNode["pwd"].as<std::string>();
            if (mysqlNode["db"]) mysqlConfig_.db = mysqlNode["db"].as<std::string>();
            if (mysqlNode["port"]) mysqlConfig_.port = mysqlNode["port"].as<unsigned int>();
            if (mysqlNode["char_set"]) mysqlConfig_.char_set = mysqlNode["char_set"].as<std::string>();
            if (mysqlNode["min_conn"]) mysqlConfig_.min_conn = mysqlNode["min_conn"].as<int>();
            if (mysqlNode["max_conn"]) mysqlConfig_.max_conn = mysqlNode["max_conn"].as<int>();
            if (mysqlNode["max_wait_ms"]) mysqlConfig_.max_wait_ms = mysqlNode["max_wait_ms"].as<int>();
        }
        return true;
    } catch (const YAML::Exception& e) {
        LOG_ERROR << "Config Error: " << e.what();
        return false;
    }
}


