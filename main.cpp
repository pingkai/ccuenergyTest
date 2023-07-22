#include "EchoServer.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include <cstring>
#include <iostream>
using namespace std;
using namespace AccuEnergyTest;

void clientSend(TCPClient &client, const uint8_t *buffer, size_t size)
{
    int p = 0;
    while (size > 0) {
        int len = client.sendMessage(buffer + p, size);
        if (len > 0) {
            p += len;
            size -= len;
        } else {
            printf("send error\n");
            break;
        }
    }
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
int main()
{
    uint8_t buffer[256];
    TCPServer server(8080, nullptr);
    EchoServer echoServer(server);
    server.init();
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

    return 0;
}
