#pragma once
#include <muduo/base/Timestamp.h>

class Response {
public:
    Response();
    ~Response() {makeEmpty();};
    Response(bool success, const std::string &failReason);

    Response(std::string & resp);

    std::string toString() const;
    void toResponse(std::string &);
    void set(bool success, const std::string &failReason);
    bool isSuccess() const;
    std::string getReason() const;
    void makeEmpty();

private:
    bool _success;
    std::string _time;
    std::string _failReason;
};