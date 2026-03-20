// C++单例模式实现

// 线程安全懒汉式
class Singleton
{
private:
    // 第一步先将构造和析构私有
    Singleton() {};
    ~Singleton() {};
    // 第二步禁止拷贝构造和赋值操作符
    Singleton(const Singleton &other) = delete;
    Singleton &operator=(const Singleton &other) = delete;

public:
    // 返回单例对象的引用
    static Singleton &creat()
    {
        static Singleton instance;
        return instance;
    }

    // 返回单例对象的指针
    static Singleton *getInstance()
    {
        return &creat();
    }
};

// 饿汉式
class Singleton
{
private:
    Singleton() {};
    ~Singleton() {};
    Singleton(const Singleton &other) = delete;
    Singleton &operator=(const Singleton &other) = delete;
    static Singleton instance; // 在类外定义并初始化
public:
    static Singleton &creat()
    {
        return instance;
    }

    static Singleton *getInstance()
    {
        return &instance;
    }
};

// 类外定义并初始化饿汉式单例对象
Singleton Singleton::instance;
