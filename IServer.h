//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ISERVER_H
#define ACCUENERGYTEST_ISERVER_H

#include <cstdint>
class IServer {
public:
    class Listener {
    public:
        virtual ~Listener() = default;
        // TODO: use a class of client instead of int
        virtual int onClient(const IServer &server, int fd) = 0;
    };
    explicit IServer(int port, Listener *listener = nullptr) : mPort(port), mListener(listener)
    {}

    void setListener(Listener *listener){
        mListener = listener;
    }

    virtual int init() = 0;
    virtual int start() = 0;
    virtual int acceptClient() = 0;

    virtual int pollIn() = 0;

    virtual int readClient(int id, uint8_t *buffer, int32_t size) const = 0;
    virtual int sendToClient(int id, const uint8_t *buffer, int32_t size) const = 0;

    virtual ~IServer() = default;

protected:
    int mPort{};
    Listener *mListener;
};

#endif//ACCUENERGYTEST_ISERVER_H