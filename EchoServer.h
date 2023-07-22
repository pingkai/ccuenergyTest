//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ECHOSERVER_H
#define ACCUENERGYTEST_ECHOSERVER_H

#include "IServer.h"
#include <memory>
#include <stdatomic.h>
#include <thread>

class EchoServer : private IServer::Listener {

public:
    explicit EchoServer(IServer &server);

    int start();

    ~EchoServer() override;

private:
    int onClient(const IServer &server, int fd) override;
    int init();
    void serverLoop();
    int loopOnce();
    int echoBack(int fd);

private:
    IServer &mServer;
    std::unique_ptr<std::thread> mThread{};
    std::atomic_bool mStop{};
};


#endif//ACCUENERGYTEST_ECHOSERVER_H
