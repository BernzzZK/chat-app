//
// Created by xwz on 25-5-27.
//

#include "AddFriendReq.h"
#include <muduo/base/Logging.h>
#include "Common.h"
#include "AddFriend.h"

AddFriendReq::AddFriendReq(const std::string &friendName, const std::string &curr_acc)
        : _reqHead(addFriend, muduo::Timestamp::now().toFormattedString())
        , _friendName(friendName)
        , _curr_acc(curr_acc)
{
}

AddFriendReq::AddFriendReq(const std::string &req) {
        auto res = common::splitString(req);
        if (res.size() != 3) {
                LOG_ERROR << "Invalid request format";
                return;
        }
        _reqHead.set(static_cast<reqType>(std::stoi(res[0])), res[1]);
        toAddFriendReq(res[2]);
}

void AddFriendReq::toAddFriendReq(std::string &info) {
        auto res = common::splitString(info);
        this->_friendName = res[0];
        this->_curr_acc = res[1];
}

std::string AddFriendReq::toString() {
        std::string ret;
        ret.append("#" + _friendName);
        ret.append("#" + _curr_acc);
        return _reqHead.toString() + "@" + ret;
}

Response AddFriendReq::handler() const {
        AddFriend addfriend(_friendName, _curr_acc);
        const std::string res = addfriend.addFriend();
        Response resp;
        if (res.empty()) {
                resp.set(true, "send friend application success");
        }
        else {
                resp.set(false, res);
        }
        return resp;
}