//
// Created by 韩冰 on 2023-07-22.
//

#ifndef ACCUENERGYTEST_SIMPLEMEMPOOL_H
#define ACCUENERGYTEST_SIMPLEMEMPOOL_H
#include <memory>
#include <queue>

namespace AccuEnergyTest {
    class PoolMemory {
    public:
        PoolMemory();
        uint8_t *get();
        ~PoolMemory();

    private:
        uint8_t *mSlice;
    };

    class SimpleMemPool {
        friend class PoolMemory;

    public:
        static SimpleMemPool &getInstance();
        std::unique_ptr<PoolMemory> getMemory();
        int setSliceSize(int size);
        int setPoolCount(int size);
        SimpleMemPool(const SimpleMemPool &rhs) = delete;
        SimpleMemPool &operator=(const SimpleMemPool &rhs) = delete;
        SimpleMemPool &operator=(SimpleMemPool &&rhs) = delete;

    private:
        explicit SimpleMemPool() = default;
        uint8_t *getSlice();
        void returnSlice(uint8_t *slice);

        ~SimpleMemPool();

    private:
        int mSliceSize{};
        int mPoolSize{};
        std::queue<uint8_t *> mQueue{};
    };
}// namespace AccuEnergyTest


#endif//ACCUENERGYTEST_SIMPLEMEMPOOL_H
