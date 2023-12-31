//
// Created by pingkai on 2023-07-23.
//

#include <EchoServer.h>
#include <TCPServer.h>
#include <cstring>
#include <iostream>
using namespace std;
using namespace AccuEnergyTest;

static const int serverPort = 8081;

int main()
{
    TCPServer server(serverPort);
    int ret = server.init();
    if (ret < 0) {
        cerr << "Server start error:" << strerror(-ret) << endl;
        return ret;
    }
    EchoServer echoServer(server);
    echoServer.start();

    bool quite = false;
    while (!quite) {
        char c;
        cin >> c;
        switch (c) {
            case 'q':
                quite = true;
                break;
            default:
                break;
        }
    }
}