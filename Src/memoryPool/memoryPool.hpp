#pragma once

#include <cstddef>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <atomic>
#include <cstdlib>

constexpr size_t MEMORY_POOL_NUM = 64;
constexpr size_t SLOT_BASE_SIZE = 8;
constexpr size_t MAX_SLOT_SIZE = 512;

struct Slot
{
    Slot *next;
};

class MemoryPool
{
public:
    // BlockSize默认设计为4096字节是因为操作系统页大小通常是 4KB，与页对齐可以提高内存访问效率，避免跨页访问导致的性能损失
    MemoryPool(size_t blockSize = 4096);
    ~MemoryPool();

    void init(size_t slotSize); // 延迟初始化
    void *allocate();           // 分配一个内存槽，返回槽指针
    void deallocate(void *p);   // 回收内存槽到空闲槽链表
private:
    void allocateNewBlock();                     // 向OS申请一个新内存块
    size_t padPointer(char *p, size_t slotSize); // 计算内存对齐

    // 指针打包/解包辅助函数（利用指针低3位存储版本号）
    static constexpr uintptr_t PTR_MASK = ~static_cast<uintptr_t>(0x7);
    static constexpr uintptr_t TAG_MASK = 0x7;

    Slot *unpackPointer(uintptr_t packed) const
    {
        return reinterpret_cast<Slot *>(packed & PTR_MASK);
    }

    uintptr_t getTag(uintptr_t packed) const
    {
        return packed & TAG_MASK;
    }

    uintptr_t packPointer(Slot *ptr, uintptr_t tag) const
    {
        return reinterpret_cast<uintptr_t>(ptr) | (tag & TAG_MASK);
    }

    size_t blockSize_;                  // 内存块大小
    size_t slotSize_;                   // 槽大小
    std::atomic<Slot *> firstBlock_;    // 指向内存池管理的首个内存块（无锁头插）
    std::atomic<Slot *> curSlot_;       // 指向当前内存块中下一个未被使用过的槽
    std::atomic<uintptr_t> freeList_;   // 无锁空闲槽栈头指针（打包指针+版本号解决ABA）
    std::atomic<Slot *> lastSlot_;      // 当前内存块中最后能够存放元素的内存槽指针
    std::atomic<bool> allocatingBlock_; // 自旋标志，保证同一时刻只有一个线程在分配新内存块
};

class HashBucket
{
public:
    static void initMemoryPool();
    // 单例模式
    static MemoryPool &getMemoryPool(int index); // 获取内存池接口
    static void *useMemory(size_t size);
    static void freeMemory(void *p, size_t size);

    template <typename T, typename... Args>
    friend T *newElement(Args &&...args); // 提供给用户在内存池分配的内存中创建对象的外部接口

    template <typename T>
    friend void deleteElement(T *p); // 析构内存池分配的内存中的对象

private:
    static MemoryPool memoryPool[MEMORY_POOL_NUM]; // 设置为static，借助 C++11 的线程安全静态初始化保证只初始化一次
};

template <typename T, typename... Args>
inline T *newElement(Args &&...args)
{
    T *res = reinterpret_cast<T *>(HashBucket::useMemory(sizeof(T)));
    if (res)
    {
        // placement new与完美转发
        new (res) T(std::forward<Args>(args)...);
    }
    return res;
}

template <typename T>
inline void deleteElement(T *p)
{
    if (p)
    {
        p->~T();
        HashBucket::freeMemory(reinterpret_cast<void *>(p), sizeof(T));
    }
}
