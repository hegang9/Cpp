#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <stdexcept>

// 手撕线程池
class ThreadPool
{
private:
    std::vector<std::thread> workers;        // 工作线程数组
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queueMutex;                   // 保护任务队列的互斥锁
    std::condition_variable condition;       // 条件变量，用于通知工作线程有新任务
    bool stop;                               // 是否停止线程池
public:
    // 构造函数，创建指定数量的工作线程
    explicit ThreadPool(size_t threadCount) : stop(false)
    {
        for (size_t i = 0; i < threadCount; ++i)
        {
            workers.emplace_back(&ThreadPool::Worker, this);
        }
    }

    // 析构函数，停止线程池并等待所有线程完成
    ~ThreadPool()
    {
        // 设置停止标志，并通知所有工作线程
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
        {
            worker.join();
        }
    }

    // 完美转发任务到线程池，F是一个可调用对象，Args是它的参数列表
    template <typename F, typename... Args>
    void Post(F &&f, Args &&...args)
    {
        // 将任务封装成一个可调用对象，并添加到任务队列中
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop)
                throw std::runtime_error("ThreadPool has been stopped");
            tasks.emplace(task);
        }
        condition.notify_one(); // 通知一个工作线程有新任务
    }

private:
    // 工作线程的函数，循环等待并执行任务
    void Worker()
    {
        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this]
                               { return stop || !tasks.empty(); });
                if (stop && tasks.empty())
                    return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }
};