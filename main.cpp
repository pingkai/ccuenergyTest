#include "EchoServer.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include <iostream>
using namespace std;

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
        snprintf(reinterpret_cast<char *>(buffer),255,"Hello from client %d",i);
        client.sendMessage(buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        client1.sendMessage(buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        ret = client.receiveMessage(buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        ret = client1.receiveMessage(buffer, sizeof(buffer));
        buffer[ret] = 0;
        cout << buffer << endl;

        this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
