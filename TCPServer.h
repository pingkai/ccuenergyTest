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
    explicit TCPServer(int port,Listener *listener = nullptr);
    int init() override;
    int start() override;

    int pollIn() override;

    int acceptClient() override;

    int readClient(int id, uint8_t *buffer, int32_t size) const override;
    int sendToClient(int id, const uint8_t *buffer, int32_t size) const override;

    ~TCPServer() override;

private:
    void acceptClients();
    int addNew(int fd);

private:
    int mFd{};
    std::unique_ptr<Impl> mImpl{};
};


#endif//ACCUENERGYTEST_TCPSERVER_H
