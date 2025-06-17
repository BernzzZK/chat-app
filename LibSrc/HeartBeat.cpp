#include "HeartBeat.h"
#include "Response.h"
#include "Common.h"
#include <muduo/base/Logging.h>

HeartBeat::HeartBeat(const std::string &info)
{
    auto res = common::splitString(info);
    reqHead_.set(static_cast<type::reqType>(std::stoi(res[0])), res[1]);
    msg_ = res[2];
}

Response HeartBeat::handler() const
{
    return Response(type::heartbeat, true, "recieved");
}
