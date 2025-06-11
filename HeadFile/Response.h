#pragma once
#include <muduo/base/Timestamp.h>
#include "Request.h"

class Response
{
public:
    Response();
    ~Response() { makeEmpty(); };
    Response(type::reqType type, bool success, const std::string &failReason);

    Response(std::string &resp);

    std::string toString() const;
    void toResponse(std::string &);
    void set(const type::reqType type, bool success, const std::string &failReason);
    bool isSuccess() const;
    std::string getReason() const;
    void makeEmpty();

private:
    type::reqType _type;
    bool _success;
    std::string _time;
    std::string _failReason;
};