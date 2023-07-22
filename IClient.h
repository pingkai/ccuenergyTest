//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_ICLIENT_H
#define ACCUENERGYTEST_ICLIENT_H

#include <atomic>

class IClient {
public:
    virtual ~IClient() = default;
    void cancel(){
        mCanceled = true;
    }
protected:
    std::atomic_bool mCanceled{};
};

#endif//ACCUENERGYTEST_ICLIENT_H
