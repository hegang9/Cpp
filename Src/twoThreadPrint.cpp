// 两线程交替打印
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool turn = true; // true表示线程1的轮次，false表示线程2的轮次

void print1()
{
    for (int i = 0; i < 10; ++i)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []
                { return turn; }); // 等待线程1的轮次
        std::cout << "Thread 1: " << i << std::endl;
        turn = false;    // 切换到线程2的轮次
        cv.notify_all(); // 通知线程2
    }
}

void print2()
{
    for (int i = 0; i < 10; ++i)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []
                { return !turn; }); // 等待线程2的轮次
        std::cout << "Thread 2: " << i << std::endl;
        turn = true;     // 切换到线程1的轮次
        cv.notify_all(); // 通知线程1
    }
}

int main()
{
    std::thread t1(print1);
    std::thread t2(print2);

    t1.join();
    t2.join();

    return 0;
}