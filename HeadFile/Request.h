#pragma once
#include <string>
#include <muduo/base/Timestamp.h>
#include "nocopyable.h"

namespace type{
   enum reqType {
        nullTyp,
        registered,
        login,
        logout,
        sendMsg,
        addFriend,
        friendList,
        changeUserName,
        heartbeat
    };
}

struct reqHead {

    type::reqType type;
    std::string time;

    std::string toString();
    void toReqHead(std::string);
    reqHead()
    : type(type::nullTyp) 
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

    Request(const type::reqType type, std::string time);

    std::string toString();

    void set(const type::reqType type, const std::string time);

private:
    reqHead _head;
};