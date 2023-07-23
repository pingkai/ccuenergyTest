//
// Created by 韩冰 on 2023-07-23.
//
#include <TCPClient.h>

#include <cstring>
#include <iostream>
#include <thread>
using namespace std;
using namespace AccuEnergyTest;

static void printHelp(const char *name)
{
    cerr << "Error parameters number\n";
    cout << "Usage:\n";
    cout << name << " serverip "
         << "serverport" << endl;
}

static int clientSend(TCPClient &client, const uint8_t *buffer, size_t size)
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
static int clientReceive(TCPClient &client, uint8_t *buffer, size_t size)
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
static string getTime()
{
    time_t rawtime;
    time(&rawtime);
    return ctime(&rawtime);
}
int main(int argc, const char *argv[])
{
    if (argc < 3) {
        printHelp(argv[0]);
        return -1;
    }
    string serverIp = argv[1];
    int serverPort = atoi(argv[2]);

    if (serverPort < 0 || serverPort > 65535) {
        cerr << "Error port number\n";
        return -1;
    }
    TCPClient client;
    int ret = client.connectServer(serverIp, serverPort);
    if (ret < 0) {
        cerr << "Can't connect to server\n";
        return ret;
    }
    uint8_t buffer[256];
    buffer[0] = 0;
    while (true) {
        snprintf(reinterpret_cast<char *>(buffer), 255, "client time is: %s", getTime().c_str());
        ret = clientSend(client, buffer, strlen(reinterpret_cast<const char *>(buffer)) + 1);
        if (ret <= 0) {
            cerr << "send error " << strerror(ret) << endl;
            break;
        }
        buffer[0] = 0;

        ret = clientReceive(client, buffer, sizeof(buffer));
        if (ret < 0) {
            cerr << "receive error " << strerror(-ret) << endl;
            break;
        }
        if (ret == 0) {
            cerr << "Server Closed" << endl;
            break;
        }
        buffer[ret] = 0;
        cout << buffer;
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    return ret;
}
