//
// Created by 韩冰 on 2023-07-21.
//

#ifndef ACCUENERGYTEST_UTILS_H
#define ACCUENERGYTEST_UTILS_H
#include <memory>

// TODO: c++14
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif//ACCUENERGYTEST_UTILS_H
