## C++内存序详解

**定义：**
**内存序（Memory Order）**是在多线程并发编程中，一套用来约束**编译器（编译期指令重排）**和**CPU硬件（运行期乱序执行与缓存可见性）**行为的规则。它通过定义原子操作前后的内存访问读写指令的**执行顺序**和**数据可见性（同步关系）**，来保证多线程共享数据在特定时刻状态的正确性。

在多线程编程中，如果不使用任何同步机制，不同线程读写共享变量时的顺序可能会因为**编译器优化**和**CPU乱序执行（Out-of-Order Execution）**而被打乱。为了在性能和正确性之间取得平衡，C++11 引入了原子操作（`std::atomic`）以及一套完整的**内存模型（Memory Model）和内存序（Memory Order）**。

内存序用于告诉编译器和 CPU：在执行这个原子操作时，周围的普通内存读写操作应该遵循怎样的重排限制。

C++11 在 `<atomic>` 头文件中定义了 6 种内存序（`std::memory_order`），按照严格跨度分类通常构成 3 种模型：

### 1. 宽松内存模型（Relaxed Ordering）
- **枚举值**：`std::memory_order_relaxed`
- **特点**：**不提供任何跨线程的同步顺序保证**。它仅仅保证当前原子变量自身操作的**原子性**（单次读或写是一次性的不可分割的操作）。
- **适用场景**：只关心变量本身的值，而不关心其他变量的状态。比如典型的**计数器**（如 `shared_ptr` 的引用计数自增）。
- **示例**：
  ```cpp
  std::atomic<int> counter{0};
  // 多线程同时执行，哪怕乱序，最终的计数值也是绝对安全的
  counter.fetch_add(1, std::memory_order_relaxed);
  ```

### 2. 获取-释放内存模型（Acquire-Release Ordering）
这是最常用的用于线程间同步的内存序组合。它成对使用，通常是一个线程写（Release），另一个线程读（Acquire）。

- **`std::memory_order_release`（释放）**：
  - 通常用于**写操作**（Store）。
  - **屏障作用**：在这个写操作**之前**的所有内存读写操作，都不能被编译器或CPU重排到这个写操作**之后**。
- **`std::memory_order_acquire`（获取）**：
  - 通常用于**读操作**（Load）。
  - **屏障作用**：在这个读操作**之后**的所有内存读写操作，都不能被重排到这个读操作**之前**。
- **`std::memory_order_acq_rel`（获取-释放协同）**：
  - 用于**读-修改-写**（Read-Modify-Write）操作（如 `fetch_add`，`compare_exchange`）。同时具有 Acquire 和 Release 的双向屏障效果。

- **经典场景（消息传递 / 同步标志）**：
  ```cpp
  std::atomic<bool> ready{false};
  int data = 0;

  // 线程 1：生产数据
  void producer() {
      data = 42; // 普通写
      // release: 保证上面 data = 42 这行代码绝对不会被重排到 ready 赋值之后
      ready.store(true, std::memory_order_release); 
  }

  // 线程 2：消费数据
  void consumer() {
      // acquire: 保证只有在看到 ready 为 true 之后，后面的普通读才会发生
      while (!ready.load(std::memory_order_acquire)) {}
      assert(data == 42); // 绝对安全，必定能断言成功
  }
  ```

### 3. 顺序一致性内存模型（Sequential Consistency）
- **枚举值**：`std::memory_order_seq_cst`
- **特点**：**C++ `std::atomic` 操作缺省的默认内存序**。最严格的内存序，不仅具有 Acquire-Release 的所有同步保证，还额外保证**所有具有该内存序的原子操作在这个程序的所有线程看来，都有一个完全相同的全局顺序**。它解决了“多个观察者（线程）对多个独立原子事件的发生顺序，产生认知分歧”的问题。
- **代价**：在某些弱内存模型架构（如 ARM、PowerPC）上需要插入极重的全内存屏障（Full Memory Barrier）指令，性能损耗较大；在 x86 上代价相对较小（因为 x86 本身就是强内存模型），但也会阻止 store buffer 的优化等。
- **适用场景**：默认选项。如果对无锁编程没有极高的要求，默认使用它最安全。

### 4. 消费内存模型（Consume Ordering / 罕见）
- **枚举值**：`std::memory_order_consume`
- **特点**：是 `acquire` 的弱化版。`acquire` 会阻止其后所有的操作重排到它前面，而 `consume` 只阻止**与它有数据依赖关系**的操作重排（比如读取了指针，紧接着使用指针访问成员）。
- **现状**：由于在编译器端实现这种“数据依赖追踪”极度复杂和困难，C++ 17 明确提出暂不建议使用它。目前的绝大部分主流编译器（GCC, Clang）都会直接将 `consume` 视为底层的 `acquire`。日常开发中直接忽略即可。

---

### 总结：日常开发如何选用？

1. **默认直接用 `seq_cst`**：只要不显式带参数，如 `ready.store(true)` 就是 `seq_cst`。这是最高优先级的做法，正确性压倒一切。
2. **需要极致性能时的读写隔离**：如果是实现像“通知-等待”之类的跨线程同步控制（你写我读），使用一套 `acquire` - `release` 配对。
3. **完全无同步需求的独立原子更新**：如果操作**仅仅是一个无关紧要的全局计数统计**，用 `relaxed` 来追求极致的无锁高效率。
