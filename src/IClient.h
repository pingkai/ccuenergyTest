//
// Created by pingkai on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ICLIENT_H
#define ACCUENERGYTEST_ICLIENT_H

#include <atomic>
namespace AccuEnergyTest {
    class IClient {
    public:
        virtual ~IClient() = default;
        void cancel()
        {
            mCanceled = true;
        }

    protected:
        std::atomic_bool mCanceled{};
    };
}// namespace AccuEnergyTest
#endif//ACCUENERGYTEST_ICLIENT_H
