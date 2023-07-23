//
// Created by 韩冰 on 2023-07-22.
//

#include "src/EchoServer.h"
#include "src/TCPClient.h"
#include "src/TCPServer.h"
#include "src/utils.h"
#include "gtest/gtest.h"
#include <cstring>
#include <iostream>
#include <memory>
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

TEST(Client, 1)
{
    const int port = 8082;
    TCPClient client;
    auto thread = std::thread([&] { int ret = client.connectServer("127.0.0.2", port); });
    this_thread::sleep_for(std::chrono::milliseconds(2000));
    client.cancel();
    thread.join();

    TCPClient client2;
    auto thread2 = std::thread([&] { int ret = client2.connectServer("127.0.0.2", port, 1); });
    this_thread::sleep_for(std::chrono::milliseconds(2000));
    client2.cancel();
    thread2.join();

    TCPClient client3;
    client3.connectServer("127.0.0.2", 65536);
}

TEST(main, 1)
{
    uint8_t buffer[256];
    TCPServer server(8080, nullptr);
    EchoServer echoServer(server);
    if (server.init() < 0) {
        return;
    }
    echoServer.start();
    string s = "Hello from client";
    TCPClient client, client1;
    int ret = client.connectServer("127.0.0.1", 8080);
    ret = client1.connectServer("127.0.0.1", 8080);

    int i = 1000;
    while (i--) {
        snprintf(reinterpret_cast<char *>(buffer), 255, "Hello from client %d", i);
        clientSend(client, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        clientSend(client1, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        buffer[0] = 0;
        ret = clientReceive(client, buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        // TODO: while receive
        buffer[0] = 0;
        ret = clientReceive(client1, buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        //    this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return;
}