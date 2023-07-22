//
// Created by 韩冰 on 2023-07-21.
//

#include "TCPClient.h"
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;
using namespace AccuEnergyTest;
#define POLLING_TIME 100

// TODO: set timeout
int TCPClient::connectServer(const std::string &hostIp, int port)
{
    if (port < 0 || port >= 65535) {
        return -EINVAL;
    }

    struct sockaddr_in serv_addr {};
    fd_set myset;
    struct timeval tv {};
    socklen_t lon;
    int valopt;
    char on = 1;
    // TODO: IPV4/V6
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(hostIp.c_str());

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
#ifdef SO_NOSIGPIPE
    setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (char *) &on, sizeof(on));
#endif
    int res = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (res < 0) {
        if (errno == EINPROGRESS) {
            printf("EINPROGRESS in connect() - selecting\n");
            int time = 0;
            do {
                if (mCanceled) {
                    return -ECANCELED;
                }
                tv.tv_sec = 1;
                tv.tv_usec = 0;
                FD_ZERO(&myset);
                FD_SET(sockfd, &myset);
                res = select(sockfd + 1, nullptr, &myset, nullptr, &tv);
                if (res < 0 && errno != EINTR) {
                    fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
                    return -errno;
                } else if (res > 0) {
                    // Socket selected for write
                    lon = sizeof(int);
                    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *) (&valopt), &lon) < 0) {
                        fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
                        return -errno;
                    }
                    // Check the value returned...
                    if (valopt) {
                        fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt));
                        return -errno;
                    }
                    break;
                } else {
                    fprintf(stderr, "Timeout in select() - Cancelling!\n");
                    //   return -errno;
                }
            } while (time++ < 15);

            if (time >= 15) {
                return -errno;
            }
        } else {
            fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
            return -errno;
        }
    }
    return 0;
}
TCPClient::~TCPClient()
{
    if (sockfd > 0) {
        close(sockfd);
    }
}
static int waite_timeout(int id, bool write)
{
    int ev = write ? POLLOUT : POLLIN;
    struct pollfd p = {.fd = id, .events = static_cast<short>(ev), .revents = 0};
    int ret;
    ret = poll(&p, 1, POLLING_TIME);
    return ret < 0 ? -errno : p.revents & (ev | POLLERR | POLLHUP) ? 0 : -EAGAIN;
}
int TCPClient::sendMessage(const uint8_t *buffer, size_t size) const
{
    int ret;
    while (true) {
        if (mCanceled) {
            return -ECANCELED;
        }
        ret = waite_timeout(sockfd, true);
        if (ret != -EAGAIN) {
            break;
        }
    }
    if (ret) {
        return ret;
    }
    ret = ::send(sockfd, buffer, size, MSG_NOSIGNAL);
    if (ret < 0){
        return -errno;
    }
    return ret;
}
int TCPClient::receiveMessage(uint8_t *buffer, size_t size) const
{
    int ret;
    while (true) {
        if (mCanceled) {
            return -ECANCELED;
        }
        ret = waite_timeout(sockfd, false);
        if (ret != -EAGAIN) {
            break;
        }
    }
    if (ret) {
        return ret;
    }
    ret = ::read(sockfd, buffer, size);
    if (ret < 0) {
        return -errno;
    }
    return ret;
}
