//
// Created by pingkai on 2023-07-21.
//

#ifndef ACCUENERGYTEST_UTILS_H
#define ACCUENERGYTEST_UTILS_H
#include <memory>

// TODO: c++14
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif//ACCUENERGYTEST_UTILS_H
