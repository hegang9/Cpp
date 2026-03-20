## thread
创建线程的基本方式——实例化thread对象，并提供一个可调用对象作为线程的入口

#### 函数原型
1. 默认构造函数 thread() noexcept;
2. 移动构造函数 thread(thread&& other) noexcept;
3. 拷贝删除 thread(const thread&) = delete; **thread对象只可移动不可拷贝**
4. 初始化构造函数
    ```c++
    template< class Function, class... Args >
    explicit thread(Function&& f, Args&&... args);
    ```
    参数说明：
    - `Function&& f`：一个可调用对象，具体见C++高级特性
    - `Args&&... args`：这是传递给可调用对象 f的参数包。参数会以值传递的方式被拷贝到新线程的存储空间中。若需要传递引用以在新线程中修改主线程的变量，必须使用std::ref()进行引用。std::cref()对应于常量引用，适用于线程函数只需要读取而不需要修改原始变量的场景。
    - 参数使用转发引用（万能引用），可实现完美转发

#### thread对象生命周期管理
一旦启动了线程，就要明确是否要等待新线程结束后与之汇合，还是任由它独立运行与之分离。如果等到新线程thread对象销毁之际还没有决定，新线程thread对象在析构时会调用terminate()终止整个程序。
- 等待并汇合：my_thread.join()
- 独立并分离：my_thread.detach()，分离后的新线程的归属权和控制权交给C++运行时库，运行库会保证线程退出会正确回收资源。
- 异常等待：为了防止新线程因抛出异常导致程序终结，那么应该在捕获异常后对子线程进行join（即在catch块中调用my_thread.join()），以避免意外的生存期问题。或者还可以设计一个类用来管理新线程，在该类的析构函数中调用join，即RAII思想。
- 在调用join或者detach前，都需要保证有与线程关联的thread对象，可通过joinable()函数来检查

#### 参数传递注意事项
线程函数参数默认以**浅拷贝**按值传递，因此需要注意处理指针传递。副本是临时变量，以**右值**形式传递给新线程上的函数或可调用对象。</br>
如果使用移动语义传递参数，会将对象的归属权移交给新线程，这个对象先进入线程内部存储空间，在传递给线程函数。

#### 传递类成员函数作为线程函数
与bind()类似：
```c++
class X{
public:
    void do_lengthy_work();
};
X my_x;
std::thread t (&X::do_lengthy_work, &my_x);
```

#### std::thread::hardware_ concurrency()获取真正并发的线程数量
在多核系统上，该值可能就是 CPU 的核芯数量。若信息无法获取，该函数则可能返回 0。

#### 识别线程
线程 ID 所属型别是std::thread::id,可调用get_id()成员函数获取。若当前thread对象没有关联线程，对其调用get_id()会返回一个ID，它按默认构造生成，表示线程不存在。</br>
获取std::thread::id还有一种方式，std::this_thread::get_id()自由函数，获取自身线程ID。

#### 线程同步的方式：
1. 互斥锁
2. 信号量
3. 条件变量 
4. 屏障
5. 读写锁：允许并发读，互斥写
6. 自旋锁：在获取锁之前会不断循环检查锁是否可用，避免上下文切换带来的开销，但是自旋锁所在的线程会一直占用CPU资源
7. 原子操作：在单个CPU指令中完成数据的读取和修改，避免竞争条件

## 线程操作 (std::this_thread)
C++标准库在 `<thread>` 头文件中提供了 `std::this_thread` 命名空间，包含了几个用于控制当前线程行为的自由函数： 

1. **`std::this_thread::yield()`**
   - **作用**：主动让出当前线程的CPU时间片，允许操作系统调度其他处于就绪状态的线程。
   - **使用场景**：常用于无锁编程或自旋等待（Spin Wait）中。如果线程当前等待的条件未满足而需要继续轮询，调用 `yield()` 可以避免 `while` 死循环白白耗尽 100% 的 CPU 资源，从而提升系统整体并发效率。

2. **`std::this_thread::sleep_for(const std::chrono::duration& rel_time)`**
   - **作用**：使当前执行的线程阻塞（休眠）一段指定的相对时间。
   - **使用场景**：定时任务、或者是简单的轮询间隔。需要配合 `std::chrono` 时间库进行使用。例如休眠 100 毫秒：`std::this_thread::sleep_for(std::chrono::milliseconds(100));`。

3. **`std::this_thread::sleep_until(const std::chrono::time_point& abs_time)`**
   - **作用**：使当前线程阻塞，直到达到指定的绝对时间点。
   - **使用场景**：当需要精准控制线程在未来某个具体时刻自动唤醒时使用。例如：`std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));`。

4. **`std::this_thread::get_id()`**
   - **作用**：获取当前运行环境所在线程的唯一ID（类型为 `std::thread::id`）。
   - **使用场景**：常用于记录多线程日志、调试程序的运行流向，或者在无锁容器中基于线程 ID 作为某种散列、映射的键使用。
