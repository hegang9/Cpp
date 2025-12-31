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