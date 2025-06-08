//
// Created by xwz on 25-6-7.
//

#ifndef SENDMSGREQ_H
#define SENDMSGREQ_H

#include "Request.h"
#include "Response.h"

class SendMsgReq {
public:
    SendMsgReq(const std::string &msg, const std::string &sender_acc, const std::string &receiver_acc);
    SendMsgReq(const std::string &info);

    std::string toString();
    void toSendMsgReq(const std::string &info);
    Response handler() const;

private:
    Request reqHead_;
    std::string msg_;
    std::string sender_acc_;
    std::string receiver_acc_;
};



#endif //SENDMSGREQ_H
