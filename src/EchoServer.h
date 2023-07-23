//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ECHOSERVER_H
#define ACCUENERGYTEST_ECHOSERVER_H

#include "IServer.h"
#include "SimpleMemPool.h"
#include <atomic>
#include <memory>
#include <thread>
#include <unordered_map>
namespace AccuEnergyTest {
    class EchoServer : private IServer::Listener {

    public:
        class EchoTask {
        public:
            EchoTask(std::unique_ptr<SimpleMemPool::PoolMemory> &&buffer, int size) : mBufferSize(size), mBuffer(std::move(buffer))
            {}

        public:
            uint8_t *getBuffer()
            {
                return mBuffer->get() + index;
            }

            void skip(int p)
            {
                index += p;
                mBufferSize -= p;
            }
            int getBufferSize()
            {
                return mBufferSize;
            }

        private:
            int mBufferSize;
            std::unique_ptr<SimpleMemPool::PoolMemory> mBuffer;
            int index{};
        };
        explicit EchoServer(IServer &server);

        int start();
#ifdef NDEBUG
#else
        int startForTest();
#endif

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
        std::atomic<bool> mStop{};
        std::unordered_map<int64_t, std::unique_ptr<EchoTask>> mTaskMap{};
    };
}// namespace AccuEnergyTest


#endif//ACCUENERGYTEST_ECHOSERVER_H
