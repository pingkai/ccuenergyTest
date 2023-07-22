//
// Created by 韩冰 on 2023-07-21.
//

#include "TCPServer.h"
#include "utils.h"
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
using namespace std;
class TCPServer::Impl {
public:
    const static int invalidFd = -1;
    vector<pollfd> clients{};
    inline pollfd *fds()
    {
        return &clients[0];
    }
    inline int64_t nFDs() const
    {
        return (int64_t) clients.size();
    }
    void addFd(int fd)
    {
        assert(std::find_if(clients.begin(), clients.end(), [fd](const pollfd &fs) { return fs.fd == fd; }) == clients.end());
        auto item = std::find_if(clients.begin(), clients.end(), [fd](const pollfd &fs) { return fs.fd == Impl::invalidFd; });

        if (item != clients.end()) {
            printf("replace a invalid one\n");
            item->fd = fd;
            item->events = POLLIN;
            item->revents = 0;
        } else {
            clients.emplace_back(pollfd{fd, POLLIN, 0});
        }
    }
};

// TODO: where can we check the port number?
TCPServer::TCPServer(int port, Listener *listener) : IServer(port, listener)
{
    mImpl = make_unique<Impl>(Impl());
}
int TCPServer::init()
{
    if (mFd > 0) {
        // TODO: define the error code;
        return 0;
    }
    mFd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(mFd, SOL_SOCKET, SO_NOSIGPIPE, (char *) &on, sizeof(on));
    int rc = setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
    rc = ioctl(mFd, FIONBIO, (char *) &on);
    struct sockaddr_in serv_addr {};
    if (mFd < 0) {
        return errno;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(mPort);

    int ret = ::bind(mFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        return errno;
    }
    return 0;
}
TCPServer::~TCPServer()
{
    if (mFd > 0) {
        close(mFd);
    }
    for (auto fd : mImpl->clients) {
        if (fd.fd > 0) {
            close(fd.fd);
        }
    }
}
int TCPServer::start()
{
    // TODO: check if started
    int ret = listen(mFd, 1);
    if (ret < 0) {
        return errno;
    }
    mImpl->addFd(mFd);
    return 0;
}
int TCPServer::readClient(int id, uint8_t *buffer, int32_t size) const
{
    return (int) read(id, buffer, size);
}

// TODO: async send, add to a task queue, poll

int TCPServer::sendToClient(int id, const uint8_t *buffer, int32_t size) const
{
    return (int) send(id, buffer, size, MSG_NOSIGNAL);
}
int TCPServer::acceptClient()
{
    // TODO: reuse sockaddr_in
    struct sockaddr_in cli_addr {};
    int clilen = sizeof(cli_addr);
    int newsockfd = ::accept(mFd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
    return newsockfd;
}
void TCPServer::acceptClients()
{
    int new_client;
    do {
        new_client = acceptClient();
        if (new_client < 0) {
            if (errno != EWOULDBLOCK) {
                perror("accept() failed");
                //end_server = TRUE;
            }
            break;
        }
        printf("New incoming connection - %d\n", new_client);

        mImpl->addFd(new_client);
    } while (true);
}
int TCPServer::pollIn()
{
    int timeout = 10;
    int rc = poll(mImpl->fds(), mImpl->nFDs(), timeout);

    if (rc < 0) {
        perror("poll() failed");
        return rc;
    }
    if (rc == 0) {
        //printf("  poll() timed out.\n");
        return rc;
    }

    int current_size = mImpl->nFDs();
    for (int i = 0; i < current_size; i++) {
        if (mImpl->fds()[i].revents == 0) {
            continue;
        }

        if (mImpl->fds()[i].revents != POLLIN) {
            printf("Error! revents = %d\n", mImpl->fds()[i].revents);
            if (mImpl->fds()[i].revents & POLLHUP) {
                printf("socket closed\n");
                close(mImpl->fds()[i].fd);
                mImpl->fds()[i].fd = Impl::invalidFd;
            }
            break;
        }
        if (mImpl->fds()[i].fd == mFd) {
            printf("Listening socket is readable\n");
            acceptClients();
            continue;
        }

        if (mListener) {
            mListener->onClient(*this, mImpl->fds()[i].fd);
        }
    }
    return 0;
}
