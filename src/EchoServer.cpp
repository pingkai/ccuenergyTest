//
// Created by 韩冰 on 2023-07-21.
//

#include "EchoServer.h"
#include "SimpleMemPool.h"
#include "utils.h"
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
static const int SND_BUF_SIZE = (16 * 1024);
using namespace std;
using namespace AccuEnergyTest;

EchoServer::EchoTask::EchoTask(std::unique_ptr<SimpleMemPool::PoolMemory> &&buffer, int size)
    : mBufferSize(size), mBuffer(std::move(buffer))
{}

uint8_t *EchoServer::EchoTask::getBuffer()
{
    return mBuffer->get() + index;
}

void EchoServer::EchoTask::skip(int p)
{
    index += p;
    mBufferSize -= p;
}

int EchoServer::EchoTask::getBufferSize() const
{
    return mBufferSize;
}

EchoServer::EchoServer(IServer &server) : mServer(server)
{
    SimpleMemPool::getInstance().setPoolCount(100);
    SimpleMemPool::getInstance().setSliceSize(SND_BUF_SIZE);
    mServer.setListener(this);
}
int EchoServer::echoBack(int64_t id)
{
    if (mTaskMap.find(id) != mTaskMap.end()) {// send out all data first
        mServer.enablePoll(id, false, true);
        return 0;
    }

    unique_ptr<SimpleMemPool::PoolMemory> buffer = SimpleMemPool::getInstance().getMemory();

    int rc = mServer.readClient(id, buffer->get(), SND_BUF_SIZE);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            perror("recv() failed");
            return -errno;
        }
    } else if (rc == 0) {
        printf("  Connection closed\n");
        return -errno;
    }
    mTaskMap[id] = make_unique<EchoTask>(std::move(buffer), rc);
    mServer.enablePoll(id, false, true);
    return 0;
}
int EchoServer::loopOnce()
{
    int ret = mServer.poll(1000);
    if (ret < 0) {
        printf("server poll error: %s\n", strerror(-ret));
    }
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
    mThread = make_unique<thread>(([&] { this->serverLoop(); }));
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

    int rc = mServer.sendToClient(id, item->second->getBuffer(), item->second->getBufferSize());
    if (rc <= 0) {
        // TODO: close the client
        printf("write error %d\n", rc);
        mTaskMap.erase(id);
        mServer.enablePoll(id, false, false);
        return 0;
    }
    item->second->skip(rc);
    if (item->second->getBufferSize() == 0) {
        //     printf("write out a task\n");
        mTaskMap.erase(id);
        mServer.enablePoll(id, true, false);
    }
    return 0;
}

int EchoServer::onClientError(const IServer &server, int64_t id)
{
    auto item = mTaskMap.find(id);
    if (item != mTaskMap.end()) {
        mTaskMap.erase(id);
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
#ifdef NDEBUG
#else
int EchoServer::startForTest()
{
    this->init();
    return 0;
}
#endif
