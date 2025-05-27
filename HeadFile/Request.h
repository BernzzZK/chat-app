#pragma once
#include <map>
#include <string>
#include <muduo/base/Timestamp.h>
#include <Response.h>
#include "nocopyable.h"

enum reqType
{
    nullTyp,
    registered,
    login,
    logout,
    sendMsg,
    addFriend
};

struct reqHead {

    reqType type;
    std::string time;

    std::string toString();
    void toReqHead(std::string);
    reqHead()
    : type(nullTyp) 
    , time(muduo::Timestamp::now().toString())
    {};
    reqHead(std::string str) {
        toReqHead(str);
    }
};

class Request : public chat::nocopyable {
public:
    Request() { }

    Request(std::string str)
    : _head(str)
    { }

    Request(const reqType type, std::string time);

    std::string toString();

    void set(const reqType type, const std::string time);

private:
    reqHead _head;
};