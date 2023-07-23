//
// Created by 韩冰 on 2023-07-22.
//

#include "gtest/gtest.h"
#include <EchoServer.h>
#include <TCPClient.h>
#include <TCPServer.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <utils.h>
using namespace std;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
using namespace std;
using namespace AccuEnergyTest;

int clientSend(TCPClient &client, const uint8_t *buffer, size_t size)
{
    int p = 0;
    while (size > 0) {
        int len = client.sendMessage(buffer + p, size);
        if (len > 0) {
            p += len;
            size -= len;
        } else {
            printf("send error %s\n", strerror(-len));
            return len;
        }
    }
    return p;
}
int clientReceive(TCPClient &client, uint8_t *buffer, size_t size)
{
    int p = 0;
    while (size > 0) {
        int len = client.receiveMessage(buffer + p, size);
        if (len <= 0) {
            if (len == -EAGAIN) {
                continue;
            }
            break;
        }
        p += len;
        size -= len;
        if (buffer[len - 1] == 0) {// read a string for test
            break;
        }
    }
    return p;
}
TEST(Server, NoClient)
{
    const int port = 8080;
    TCPServer server(port, nullptr);
    EchoServer echoServer(server);
    if (server.init() < 0) {
        return;
    }
    echoServer.startForTest();
}

TEST(ReleaseServer, 1)
{
    const int port = 8080;
    uint8_t buffer[256];
    unique_ptr<TCPServer> server = make_unique<TCPServer>(port, nullptr);
    unique_ptr<EchoServer> echoServer = make_unique<EchoServer>(*server.get());
    if (server->init() < 0) {
        return;
    }
    echoServer->startForTest();

    TCPClient client;
    int ret = client.connectServer("127.0.0.1", port);

    int i = 10;
    while (i--) {
        snprintf(reinterpret_cast<char *>(buffer), 255, "Hello from client %d", i);
        ret = clientSend(client, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        if (ret < 0){
            if (ret == -EPIPE){
                printf("closed by server\n");
                break;
            }
        }
        buffer[0] = 0;
        ret = clientReceive(client, buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        if (i == 5){
            echoServer = nullptr;
            server = nullptr;
        }
    }
}
TEST(ReleaseServer, 2)
{
    const int port = 8082;
    uint8_t buffer[256];
    unique_ptr<TCPServer> server = make_unique<TCPServer>(port, nullptr);
    unique_ptr<EchoServer> echoServer = make_unique<EchoServer>(*server.get());
    int ret = server->init();
    if (ret < 0) {
        return;
    }
    echoServer->startForTest();

    TCPClient client;
    ret = client.connectServer("127.0.0.1", port);

    int i = 10;
    while (i--) {
        snprintf(reinterpret_cast<char *>(buffer), 255, "Hello from client %d", i);
        ret = clientSend(client, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        if (i == 5){
            echoServer = nullptr;
            server = nullptr;
            this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        buffer[0] = 0;
        ret = clientReceive(client, buffer, sizeof(buffer));
        if (i <= 5) {
            ASSERT_EQ(ret, 0);
            if (ret == 0){
                printf("closed by server\n");
                break;
            }
        }
        buffer[ret] = 0;
        cout << buffer << endl;
    }
}
