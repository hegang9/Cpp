#include "MemoryPool.hpp"

// 静态成员定义
MemoryPool HashBucket::memoryPool[MEMORY_POOL_NUM];

MemoryPool::MemoryPool(size_t blockSize)
    : blockSize_(blockSize),
      slotSize_(0),
      firstBlock_(nullptr),
      curSlot_(nullptr),
      freeList_(0), // 初始化为0（nullptr + tag=0）
      lastSlot_(nullptr),
      allocatingBlock_(false)
{
}

MemoryPool::~MemoryPool()
{
    // 释放所有内存块
    Slot *cur = firstBlock_.load(std::memory_order_relaxed);
    while (cur)
    {
        Slot *next = cur->next;
        ::free(reinterpret_cast<void *>(cur));
        cur = next;
    }
}

void MemoryPool::init(size_t slotSize) { slotSize_ = slotSize; }

void *MemoryPool::allocate()
{
    // 优先使用空闲链表中的内存槽（无锁栈 pop）
    uintptr_t oldHead = freeList_.load(std::memory_order_acquire);
    Slot *oldPtr = unpackPointer(oldHead);

    while (oldPtr)
    {
        Slot *nextSlot = oldPtr->next;
        uintptr_t oldTag = getTag(oldHead);
        uintptr_t newHead = packPointer(nextSlot, oldTag + 1);

        // CAS：若 freeList_ 仍是 oldHead，则更新为 newHead（带版本号防ABA）
        if (freeList_.compare_exchange_weak(oldHead, newHead,
                                            std::memory_order_acq_rel,
                                            std::memory_order_acquire))
        {
            return reinterpret_cast<void *>(oldPtr);
        }
        // CAS 失败，oldHead 已被自动更新为最新值，继续重试
        oldPtr = unpackPointer(oldHead);
    }

    // 空闲链表为空，从当前内存块中分配未使用过的内存槽
    while (true)
    {
        Slot *cur = curSlot_.load(std::memory_order_acquire);
        Slot *last = lastSlot_.load(std::memory_order_acquire);

        if (cur && cur <= last)
        {
            // 尝试通过 CAS 推进 curSlot_ 来原子地"占用"一个槽
            Slot *next = reinterpret_cast<Slot *>(reinterpret_cast<char *>(cur) + slotSize_);
            if (curSlot_.compare_exchange_weak(cur, next,
                                               std::memory_order_acq_rel,
                                               std::memory_order_acquire))
            {
                return reinterpret_cast<void *>(cur);
            }
            // CAS 失败，说明其他线程抢先推进了，重试即可
        }
        else
        {
            // 当前内存块已用完，需要分配新的内存块
            // 使用自旋标志保证同一时刻只有一个线程分配新块
            bool expected = false;
            if (allocatingBlock_.compare_exchange_strong(expected, true,
                                                         std::memory_order_acq_rel))
            {
                // 双重检查：其他线程可能刚分配完
                Slot *curAfter = curSlot_.load(std::memory_order_acquire);
                Slot *lastAfter = lastSlot_.load(std::memory_order_acquire);
                if (!curAfter || curAfter > lastAfter)
                {
                    allocateNewBlock();
                }
                allocatingBlock_.store(false, std::memory_order_release);
            }
            // 无论是自己分配的还是其他线程分配的，回到循环顶部重试
        }
    }
}

void MemoryPool::deallocate(void *p)
{
    if (!p)
        return;
    // 无锁栈 push（带版本号防ABA）
    Slot *slot = reinterpret_cast<Slot *>(p);
    uintptr_t oldHead = freeList_.load(std::memory_order_acquire);
    uintptr_t newHead;
    do
    {
        slot->next = unpackPointer(oldHead);
        uintptr_t oldTag = getTag(oldHead);
        newHead = packPointer(slot, oldTag + 1);
    } while (!freeList_.compare_exchange_weak(oldHead, newHead,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
}

void MemoryPool::allocateNewBlock()
{
    void *newBlock = ::malloc(blockSize_);
    if (!newBlock)
        return;

    // 无锁头插法将新内存块插入内存块链表
    Slot *newBlockSlot = reinterpret_cast<Slot *>(newBlock);
    Slot *oldFirst = firstBlock_.load(std::memory_order_relaxed);
    do
    {
        newBlockSlot->next = oldFirst;
    } while (!firstBlock_.compare_exchange_weak(oldFirst, newBlockSlot,
                                                std::memory_order_release,
                                                std::memory_order_relaxed));

    // 计算对齐后的起始槽位置
    char *endofhead = reinterpret_cast<char *>(newBlock) + sizeof(Slot *);
    size_t paddingsize = padPointer(endofhead, slotSize_);
    Slot *newCurSlot = reinterpret_cast<Slot *>(endofhead + paddingsize);

    Slot *newLastSlot = reinterpret_cast<Slot *>(reinterpret_cast<size_t>(newBlock) +
                                                 blockSize_ - slotSize_ + 1);

    // 先更新 lastSlot_ 再更新 curSlot_，保证其他线程看到 curSlot_ 更新时 lastSlot_ 已经就绪
    lastSlot_.store(newLastSlot, std::memory_order_release);
    curSlot_.store(newCurSlot, std::memory_order_release);
}

size_t MemoryPool::padPointer(char *p, size_t slotSize)
{
    return (slotSize - reinterpret_cast<size_t>(p)) % slotSize;
}

void HashBucket::initMemoryPool()
{
    for (int i = 1; i < MEMORY_POOL_NUM; ++i)
    {
        getMemoryPool(i).init((i + 1) * SLOT_BASE_SIZE);
    }
}

MemoryPool &HashBucket::getMemoryPool(int index) { return memoryPool[index]; }

void *HashBucket::useMemory(size_t size)
{
    if (size <= 0)
        return nullptr;

    // 若大于512字节，直接使用系统分配
    if (size > MAX_SLOT_SIZE)
    {
        return ::malloc(size);
    }

    // 否则从内存池分配
    // 计算该size应该使用的内存池索引
    int index = (size + 7) / SLOT_BASE_SIZE - 1;
    return getMemoryPool(index).allocate();
}

void HashBucket::freeMemory(void *p, size_t size)
{
    if (!p)
        return;

    // 大于512字节，释放给系统
    if (size > MAX_SLOT_SIZE)
    {
        ::free(p);
        return;
    }

    int index = (size + 7) / SLOT_BASE_SIZE - 1;
    getMemoryPool(index).deallocate(p);
    return;
}
