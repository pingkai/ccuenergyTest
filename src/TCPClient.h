//
// Created by pingkai on 2023-07-21.
//

#ifndef ACCUENERGYTEST_TCPCLIENT_H
#define ACCUENERGYTEST_TCPCLIENT_H

#include "IClient.h"
#include "utils.h"
#include <cinttypes>
#include <cstddef>
#include <string>
namespace AccuEnergyTest {
    class TCPClient : public IClient {
    public:
        TCPClient() = default;
        TCPClient(const TCPClient &rhs) = delete;
        TCPClient &operator=(const TCPClient &rhs) = delete;
        TCPClient &operator=(TCPClient &&rhs) = delete;

        WARN_UNUSED int connectServer(const std::string &hostIp, int port, int timeOuts = 15);
        // TODO: define a class for message
        int sendMessage(const uint8_t *buffer, size_t size) const;

        int receiveMessage(uint8_t *buffer, size_t size) const;

        ~TCPClient() override;


    private:
        int sockfd{};
    };
}// namespace AccuEnergyTest


#endif//ACCUENERGYTEST_TCPCLIENT_H
