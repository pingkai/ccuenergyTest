#include "EchoServer.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include <iostream>
using namespace std;

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

int main()
{
    uint8_t buffer[256];
    TCPServer server(8080, nullptr);
    EchoServer echoServer(server);
    server.init();
    echoServer.start();
    string s = "Hello from client";
    TCPClient client(8080);
    TCPClient client1(8080);
    int ret = client.connectServer();
    ret = client1.connectServer();

    int i = 10;
    while (i--) {
        snprintf(reinterpret_cast<char *>(buffer), 255, "Hello from client %d", i);
        clientSend(client, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        clientSend(client1, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        ret = client.receiveMessage(buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        // TODO: while receive
        ret = client1.receiveMessage(buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}