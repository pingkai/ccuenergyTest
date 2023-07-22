//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ECHOSERVER_H
#define ACCUENERGYTEST_ECHOSERVER_H

#include "IServer.h"
#include <memory>
#include <stdatomic.h>
#include <thread>
#include <unordered_map>
using namespace AccuEnergyTest;
class EchoServer : private IServer::Listener {

public:
    class EchoTask {
    public:
        EchoTask(std::unique_ptr<uint8_t> &&buffer, int size) : mBufferSize(size), mBuffer(std::move(buffer))
        {}

    public:
        int mBufferSize;
        std::unique_ptr<uint8_t> mBuffer;
    };
    explicit EchoServer(IServer &server);

    int start();

    ~EchoServer() override;

private:
    int onClientRead(const IServer &server, int64_t id) override;
    int onClientWrite(const IServer &server, int64_t id) override;
    int init();
    void serverLoop();
    int loopOnce();
    int echoBack(int64_t id);

private:
    IServer &mServer;
    std::unique_ptr<std::thread> mThread{};
    std::atomic_bool mStop{};
    std::unordered_map<int64_t, std::unique_ptr<EchoTask>> mTaskMap{};
};


#endif//ACCUENERGYTEST_ECHOSERVER_H
