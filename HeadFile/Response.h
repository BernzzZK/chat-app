#pragma once
#include <muduo/base/Timestamp.h>

class Response {
public:
    Response();
    Response(bool success, std::string failReson);
    Response(std::string);

    std::string toString();
    void toResponse(std::string);
    void set(bool success, std::string failReson);

private:
    bool _success;
    std::string _time;
    std::string _failReson;
};