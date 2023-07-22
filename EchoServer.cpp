//
// Created by 韩冰 on 2023-07-21.
//

#include "EchoServer.h"
#include "utils.h"
#include <iostream>
#include <thread>
static const int SND_BUF_SIZE = (16 * 1024);
using namespace std;
using namespace AccuEnergyTest;

EchoServer::EchoServer(IServer &server) : mServer(server)
{
    mServer.setListener(this);
}
int EchoServer::echoBack(int64_t id)
{
    int rc;

    if (mTaskMap.find(id) != mTaskMap.end()) {// send out all data first
        mServer.enablePoll(id, false, true);
        return 0;
    }
    unique_ptr<uint8_t> buffer = static_cast<unique_ptr<uint8_t>>((uint8_t *) malloc(SND_BUF_SIZE));

    rc = mServer.readClient(id, buffer.get(), SND_BUF_SIZE);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            perror("recv() failed");
            //      close_conn = TRUE;
            return 0;
        }
    } else if (rc == 0) {
        printf("  Connection closed\n");
        //       close_conn = TRUE;
        return 0;
    }
    mTaskMap[id] = make_unique<EchoTask>(EchoTask(std::move(buffer), rc));
    mServer.enablePoll(id, false, true);
    return 0;
}
int EchoServer::loopOnce()
{
    int timeout = 10;
    int ret = mServer.pollIn();
    return ret;
}
void EchoServer::serverLoop()
{
    while (!mStop) {
        if (loopOnce() < 0) {
            break;
        }
    }
}

int EchoServer::init()
{
    mServer.start();
    mThread = make_unique<thread>(thread([&] { this->serverLoop(); }));

    if (!mThread) {
        printf("thread init error\n");
    }
    return 0;
}

int EchoServer::start()
{
    static std::once_flag flag;
    std::call_once(flag, [this] { this->init(); });
    return 0;
}
int EchoServer::onClientRead(const IServer &server, int64_t id)
{
    echoBack(id);
    return 0;
}
int EchoServer::onClientWrite(const IServer &server, int64_t id)
{
    auto item = mTaskMap.find(id);
    if (item == mTaskMap.end()) {
        mServer.enablePoll(id, true, false);
        return 0;
    }

    int rc = mServer.sendToClient(id, item->second->mBuffer.get(), item->second->mBufferSize);
    if (rc <= 0) {
        // TODO: close the client
        printf("write error %d\n", rc);
        mTaskMap.erase(id);
        mServer.enablePoll(id, false, false);
        return 0;
    }
    item->second->mBufferSize -= rc;
    if (item->second->mBufferSize == 0) {
        //     printf("write out a task\n");
        mTaskMap.erase(id);
        mServer.enablePoll(id, true, false);
    }
    return 0;
}
EchoServer::~EchoServer()
{
    mStop = true;
    if (mThread && mThread->joinable()) {
        mThread->join();
    }
}
