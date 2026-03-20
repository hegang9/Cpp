// 两线程交替打印
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>

std::atomic<int> turn{0};

// id:线程唯一id，n:线程数量，m:每个线程需要打印的次数
void print(int id, int n, int m)
{
    for (int i = 0; i < m; ++i)
    {
        while (turn.load() != id)
        {
            std::this_thread::yield(); // 让出CPU时间片，避免忙等待
        }
        printf("Thread %d: %d\n", id, i);
        turn.store((id + 1) % n);
    }
}

int main()
{
    int n = 10;
    int m = 5;
    std::vector<std::thread> threads;
    for (int i = 0; i < n; ++i)
    {
        threads.emplace_back(print, i, n, m);
    }
    for (auto &t : threads)
    {
        t.join();
    }
    return 0;
}