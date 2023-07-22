//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_TCPSERVER_H
#define ACCUENERGYTEST_TCPSERVER_H

#include "IServer.h"
#include <memory>
#include <thread>

class TCPServer : public IServer {
    class Impl;

public:
    explicit TCPServer(int port, Listener *listener = nullptr);
    TCPServer(const TCPServer &rhs) = delete;
    TCPServer &operator=(const TCPServer &rhs) = delete;
    TCPServer &operator=(TCPServer &&rhs) = delete;
    int init() override;
    int start() override;

    int pollIn() override;

    int acceptClient() override;

    int readClient(int64_t id, uint8_t *buffer, int32_t size) const override;
    int sendToClient(int64_t id, const uint8_t *buffer, int32_t size) const override;

    int enablePoll(int64_t id, bool In, bool out) override;

    ~TCPServer() override;

private:
    void acceptClients();

private:
    int mFd{};
    std::unique_ptr<Impl> mImpl{};
};


#endif//ACCUENERGYTEST_TCPSERVER_H
