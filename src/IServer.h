//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ISERVER_H
#define ACCUENERGYTEST_ISERVER_H

#include "utils.h"
#include <cstdint>
namespace AccuEnergyTest {
    class IServer {
    public:
        class Listener {
        public:
            virtual ~Listener() = default;

            /**
             * When server got a new client, this callback will be called.
             * @param server the reference of the server
             * @param id Client's id in the server
             * @return not used by now, maybe if you returned a negative value, the server will stop in the future
             */

            virtual int onClient(const IServer &server, int64_t id)
            {
                return 0;
            }

            /**
             *  When server accepted a client, the server will poll the client for readability automatically.
             *  If the server received any data from the client, this callback will be called, then you can read
             *  the data from the client by calling the readClient api of the server.
             * @param server The reference of the server.
             * @param id Client's id in the server.
             * @return Not used by now, maybe if you returned a negative value, the server will stop in the future.
             */
            virtual int onClientRead(const IServer &server, int64_t id) = 0;

            /**
             * If you want to send some data to a client, you should let the sever poll for its writability by
             * using enablePoll api of the server, then if the client is writeable, this callback will be called, then you can write
             * the data to the client by calling the sendToClient api of the server.
             *
             * And if you sent out all the data, you should disable the sever for polling its writability.
             *
             * @param server The reference of the server.
             * @param id Client's id in the server.
             * @return Not used by now, maybe if you returned a negative value, the server will stop in the future.
             */
            virtual int onClientWrite(const IServer &server, int64_t id) = 0;

            /**
             * When the client is not available(e.g., closed by client) any more when server polling it, the client will be closed,
             * and then, this callback will be called, you should clear any data relevant to the client synchronously in this
             * callback, because the client id will be typically reused by other new client.
             *
             * @param server The reference of the server.
             * @param id Client's id in the server.
             * @return Not used by now, maybe if you returned a negative value, the server will stop in the future.
             */
            virtual int onClientError(const IServer &server, int64_t id) = 0;
        };
        explicit IServer(int port, Listener *listener = nullptr) : mPort(port), mListener(listener)
        {}

        void setListener(Listener *listener)
        {
            mListener = listener;
        }

        WARN_UNUSED virtual int init() = 0;
        virtual int start() = 0;

        virtual int poll(int timeout) = 0;

        virtual int readClient(int64_t id, uint8_t *buffer, int32_t size) const = 0;

        virtual int enablePoll(int64_t id, bool In, bool out) = 0;

        virtual int sendToClient(int64_t id, const uint8_t *buffer, int32_t size) const = 0;

        virtual ~IServer() = default;

    protected:
        int mPort{};
        Listener *mListener;
    };
}// namespace AccuEnergyTest

#endif//ACCUENERGYTEST_ISERVER_H
