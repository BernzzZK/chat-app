#pragma once
#include <muduo/base/Timestamp.h>

class Response {
public:
    Response(bool success, std::string failReson);
    Response(std::string);

    std::string toString();
    void toResponse(std::string);

private:
    bool _success;
    std::string _time;
    std::string _failReson;
};