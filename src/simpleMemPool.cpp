//
// Created by 韩冰 on 2023-07-22.
//

#include "SimpleMemPool.h"
#include "utils.h"
#include <cassert>
#include <cerrno>
using namespace AccuEnergyTest;
SimpleMemPool &SimpleMemPool::getInstance()
{
    static SimpleMemPool pool;
    return pool;
}
int SimpleMemPool::setSliceSize(int size)
{
    if (mSliceSize == 0 && size > 0) {
        mSliceSize = size;
        return 0;
    }
    return -EINVAL;
}
int SimpleMemPool::setPoolCount(int size)
{
    if (size <= 0) {
        return -EINVAL;
    }
    mPoolSize = size;
    return 0;
}
uint8_t *SimpleMemPool::getSlice()
{
    if (!mSliceSize || !mPoolSize) {
        return nullptr;
    }
    if (mQueue.empty()) {
        return new uint8_t[mSliceSize];
    }
    uint8_t *p = mQueue.front();
    mQueue.pop();
    return p;
}
void SimpleMemPool::returnSlice(uint8_t *slice)
{
    if (mQueue.size() < mPoolSize) {
        mQueue.push(slice);
        return;
    }
    delete[] slice;
}
std::unique_ptr<SimpleMemPool::PoolMemory> SimpleMemPool::getMemory()
{
    return make_unique<PoolMemory>();
}
SimpleMemPool::~SimpleMemPool()
{
    while (!mQueue.empty()) {
        delete[] mQueue.front();
        mQueue.pop();
    }
}
SimpleMemPool::PoolMemory::PoolMemory()
{
    mSlice = SimpleMemPool::getInstance().getSlice();
    assert(mSlice);
}
SimpleMemPool::PoolMemory::~PoolMemory()
{
    assert(mSlice);
    SimpleMemPool::getInstance().returnSlice(mSlice);
}
uint8_t *SimpleMemPool::PoolMemory::get()
{
    return mSlice;
}
