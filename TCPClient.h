//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_TCPCLIENT_H
#define ACCUENERGYTEST_TCPCLIENT_H

#include "IClient.h"
#include <cinttypes>
#include <cstddef>

class TCPClient : public IClient {
public:
    explicit TCPClient(int port);

    TCPClient(const TCPClient &rhs) = default;
    TCPClient &operator=(const TCPClient &rhs) = delete;
    TCPClient &operator=(TCPClient &&rhs) = delete;

    int connectServer();
    // TODO: define a class for message
    int sendMessage(const uint8_t *buffer, size_t size) const;

    int receiveMessage(uint8_t *buffer, size_t size) const;

    ~TCPClient() override;


private:
    int mPort;
    int sockfd{};
};


#endif//ACCUENERGYTEST_TCPCLIENT_H