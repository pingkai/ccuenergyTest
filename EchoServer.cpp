//
// Created by 韩冰 on 2023-07-21.
//

#include "EchoServer.h"
#include "utils.h"
#include <iostream>
#include <thread>
using namespace std;
EchoServer::EchoServer(IServer &server) : mServer(server)
{
    mServer.setListener(this);
}
int EchoServer::echoBack(int fd)
{
    int rc;
    int len;
    uint8_t buffer[256];
    do {
        rc = mServer.readClient(fd, buffer, sizeof(buffer));
        if (rc < 0) {
            if (errno != EWOULDBLOCK) {
                perror("recv() failed");
                //      close_conn = TRUE;
            }
            break;
        }
        if (rc == 0) {
            printf("  Connection closed\n");
            //       close_conn = TRUE;
            break;
        }
        len = rc;
      //  printf("%d bytes received\n", len);
        buffer[len] = 0;
 //       cout << buffer << endl;
        rc = mServer.sendToClient(fd, buffer, len);
        if (rc < 0) {
            perror("send() failed");
            //    close_conn = TRUE;
            break;
        }

    } while (true);
    return 0;
}
int EchoServer::loopOnce()
{
    int timeout = 10;
    int ret = mServer.pollIn();
    return ret;
}
void EchoServer::serverLoop()
{
    while (!mStop) {
        if (loopOnce() < 0){
            break;
        }
    }
}

int EchoServer::init()
{
    mServer.start();
    mThread = make_unique<thread>(thread([&]{
        this->serverLoop();
    }));

    if (!mThread){
        printf("thread init error\n");
    }
    return 0;
}

int EchoServer::start()
{
    static std::once_flag flag;
    std::call_once(flag,[this]{
        this->init();
    });
    return 0;
}
int EchoServer::onClient(const IServer &server, int fd)
{
    echoBack(fd);
    return 0;
}
EchoServer::~EchoServer()
{
    mStop = true;
    if (mThread && mThread->joinable()){
        mThread->join();
    }
}
