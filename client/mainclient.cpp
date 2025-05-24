#include "Client.h"
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <thread>
#include <iostream>

using namespace muduo::net;

// 用户输入处理线程
void inputThread(ChatClient &client)
{
    std::string msg;
    while (std::getline(std::cin, msg))
    {
        if (msg == "/quit")
        {
            client.disconnect();
            break;
        }
        client.send(msg);
    }
}

int main()
{
    LOG_INFO << "ChatClient starting...";

    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", 8888);
    ChatClient client(&loop, serverAddr);

    client.connect();

    // 启动输入线程
    std::thread thr(inputThread, std::ref(client));
    thr.detach();

    loop.loop();
}