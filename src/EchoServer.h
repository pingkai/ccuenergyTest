//
// Created by pingkai on 2023-07-21.
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
    private:
        class EchoTask {
        public:
            EchoTask(std::unique_ptr<SimpleMemPool::PoolMemory> &&buffer, int size);
            uint8_t *getBuffer();
            void skip(int p);
            int getBufferSize() const;

        private:
            int mBufferSize;
            std::unique_ptr<SimpleMemPool::PoolMemory> mBuffer;
            int index{};
        };

    public:
        explicit EchoServer(IServer &server);

        /**
         *  Start to receive clients, typically you can only one server in a process. But if you want to
         *  start more than one server for testing for example, you can use startForTest api under a debug
         *  version build
         * @return
         */
        int start();
#ifndef NDEBUG
        int startForTest();
#endif
        ~EchoServer() override;

    private:
        int onClientRead(const IServer &server, int64_t id) override;
        int onClientWrite(const IServer &server, int64_t id) override;
        int onClientError(const IServer &server, int64_t id) override;

    private:
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
