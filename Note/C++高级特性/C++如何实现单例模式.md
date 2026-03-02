## C++ 单例模式的实现与总结

单例模式（Singleton Pattern）是一种常见的设计模式，其核心目的是**确保一个类只有一个实例，并提供一个全局访问点**。

常见的实现流派分为**懒汉式**（延迟加载，用时再创建）和**饿汉式**（随程序启动就创建）。
由于 C++ 中对对象的生命周期和内存管理有着更底层的操作权限，对于不同流派，我们还可以分为**返回引用**和**返回指针**两种形式。

下面我们逐一分析其实现方式与线程安全特性。

---

### 一、 懒汉式 (Lazy Initialization)
懒汉式的特点是“延迟加载”，即直到 `getInstance()` 第一次被调用时才会进行实例化。这种方式节省了程序初始启动的开销。

#### 1. 懒汉式：返回引用 (Meyers' Singleton)
**【线程安全：是】（C++11 之后强保证）**
这是 C++ 中最推荐、最简洁的单例实现方式。它利用了 C++ 的**局部静态变量特性**。
自 C++11 标准开始，编译器强制保证**局部静态变量的初始化是线程安全的**（如果多个线程试图同时初始化同一个局部静态变量，只有一个线程能进行初始化，其他线程将被阻塞等待）。不存在内存泄漏问题，且优雅地避开了静态初始化顺序灾难。

```cpp
class SingletonLazyRef {
public:
    // 全局访问点，返回引用
    static SingletonLazyRef& getInstance() {
        static SingletonLazyRef instance;  // 局部静态变量，C++11起天然线程安全
        return instance;
    }

private:
    SingletonLazyRef() = default;
    ~SingletonLazyRef() = default;

    // 禁用拷贝和赋值
    SingletonLazyRef(const SingletonLazyRef&) = delete;
    SingletonLazyRef(SingletonLazyRef&&) = delete;
    SingletonLazyRef& operator=(const SingletonLazyRef&) = delete;
};
```

#### 2. 懒汉式：返回指针 (传统实现)
**【线程安全：否】**
这是许多基于 GC 的语言转过来的开发者常写的形式。它默认是**非线程安全**的。因为多个线程同时判断 `if (instance == nullptr)` 时，可能会引发竞争条件，导致实例对象被多次 `new` 出来，造成内存泄漏和不可预期行为。

```cpp
class SingletonLazyPtr {
public:
    static SingletonLazyPtr* getInstance() {
        if (instance == nullptr) {      // 【非线程安全】竞争点
            instance = new SingletonLazyPtr();
        }
        return instance;
    }

private:
    SingletonLazyPtr() = default;
    ~SingletonLazyPtr() = default;
    SingletonLazyPtr(const SingletonLazyPtr&) = delete;
    SingletonLazyPtr& operator=(const SingletonLazyPtr&) = delete;

    static SingletonLazyPtr* instance;
};

// 静态指针成员在类外初始化
SingletonLazyPtr* SingletonLazyPtr::instance = nullptr;
```

#### 3. 懒汉式：返回指针 (双重检查锁定 DCLP)
**【线程安全：是】**
为了解决上面“普通返回指针”的线程安全问题，我们需要加锁。但是直接在函数顶层加锁会导致即使对象已创建也会每次锁住，性能低下。
**双重检查锁定 (Double-Checked Locking Pattern)** 机制由此而生。在 C++11 后，必须结合 `std::atomic` 和**内存屏障**，才能杜绝指令重排导致的野指针问题，保证极致的性能和绝对的线程安全。
*(注：由于其极度复杂，所以除非有极其特殊的基础架构需求，这年头都推荐回第一种 Meyers Singleton)*。

```cpp
#include <mutex>
#include <atomic>

class SingletonDCLP {
public:
    static SingletonDCLP* getInstance() {
        // 第一重检查：使用 acquire 内存屏障，确保在读到有效指针前，对对象的初始化完全可见
        SingletonDCLP* tmp = instance.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);  // 加锁
            
            // 第二重检查：拿到锁后再次确认，防止阻塞中的线程多开实例
            tmp = instance.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new SingletonDCLP();
                // 使用 release 内存屏障，确保对象的写入操作先于指针赋值生效，防止指令重排
                instance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

private:
    SingletonDCLP() = default;
    ~SingletonDCLP() = default;
    SingletonDCLP(const SingletonDCLP&) = delete;
    SingletonDCLP& operator=(const SingletonDCLP&) = delete;

    static std::atomic<SingletonDCLP*> instance;
    static std::mutex mtx;
};

// 静态成员类外初始化
std::atomic<SingletonDCLP*> SingletonDCLP::instance{nullptr};
std::mutex SingletonDCLP::mtx;
```

---

### 二、 饿汉式 (Eager Initialization)
饿汉式的特点是“着急”，无论你用不用，甚至在进入 `main()` 函数之前，全局实例就已经建立好了。它的唯一优点是不存在运行时加锁带来的开销。

**隐患警告**：所有的饿汉式和依赖全局对象的单例，都会面临 **静态初始化顺序灾难 (Static Initialization Order Fiasco)**。由于跨文件的全局对象初始化顺序是未定义的，会导致一个先初始化的全局对象在自己的构造行为中，访问了另一个尚未初始化的单例，从而导致直接崩溃。

#### 1. 饿汉式：返回引用
**【线程安全：是】**
因为局部全局变量初始化是在单主线程进入 `main` 之前完成的，不涉及并发覆盖或竞争读写。

```cpp
class SingletonEagerRef {
public:
    static SingletonEagerRef& getInstance() {
        return instance;
    }

private:
    SingletonEagerRef() = default;
    ~SingletonEagerRef() = default;
    SingletonEagerRef(const SingletonEagerRef&) = delete;
    SingletonEagerRef& operator=(const SingletonEagerRef&) = delete;

    static SingletonEagerRef instance;
};

// 在进入主函数前触发构造
SingletonEagerRef SingletonEagerRef::instance;
```

#### 2. 饿汉式：返回指针
**【线程安全：是】**
同样在进入 `main()` 之前就在堆上构建完成，没有初始化的线程安全问题。
它的额外弊端在于，如果不借助 `destroy` 函数、智能指针或 `atexit()` 技术手段，申请在堆上的内存在程序退出时可能无法正常调用析构函数（导致内存泄漏，虽然操作系统最终会收回，但可能导致你资源没有妥善 clean-up）。

```cpp
class SingletonEagerPtr {
public:
    static SingletonEagerPtr* getInstance() {
        return instance;
    }

private:
    SingletonEagerPtr() = default;
    ~SingletonEagerPtr() = default;
    SingletonEagerPtr(const SingletonEagerPtr&) = delete;
    SingletonEagerPtr& operator=(const SingletonEagerPtr&) = delete;

    static SingletonEagerPtr* instance;
};

// 程序启动前在堆分配
SingletonEagerPtr* SingletonEagerPtr::instance = new SingletonEagerPtr();
```

---

### 总结
在现代 C++ 开发中（C++11 及以上），如果被要求写一个单例：
- 直接、无脑地写 **懒汉式（即 Meyers 单例，内部静态变量引用返回）**。它是**绝对的线程安全**、代码量最少、零内存泄漏，并且彻底免疫了静态初始化顺序灾难。