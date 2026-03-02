## 操作系统中的PV操作与信号量

### 核心概念

**信号量（semaphore）**是操作系统提供的一种同步原语，用来控制并发访问共享资源或实现线程/进程间的同步。它本质上维护一个计数器和一个等待队列。

- **计数型信号量**：计数器可为任意非负整数，适合资源池数量控制。
- **二值信号量**：计数器只取 0/1，类似互斥锁的语义。

### PV 操作含义

PV 操作来自 Dijkstra 的记号：

- **P 操作（proberen / wait）**：申请资源或等待条件。
  - 语义：计数器减 1；若结果 < 0，则阻塞当前线程/进程。
- **V 操作（verhogen / signal）**：释放资源或发出信号。
  - 语义：计数器加 1；若结果 <= 0，则唤醒一个等待者。

### C++ 线程同步中的信号量机制

在 C++ 中常见的信号量用法有两类：

- **C++20 标准库**：`std::counting_semaphore` / `std::binary_semaphore`。
- **POSIX**：`sem_t` + `sem_wait` / `sem_post`（常见于 Linux）。

它们与 PV 操作的对应关系：

- `wait()` / `acquire()` ≈ **P 操作**
- `post()` / `release()` ≈ **V 操作**

### 示例：用信号量实现生产者/消费者（C++20）

```cpp
#include <semaphore>
#include <thread>
#include <vector>
#include <iostream>

std::counting_semaphore<10> emptySlots(10);
std::counting_semaphore<10> fullSlots(0);

int main() {
	std::vector<int> buffer;

	auto producer = [&]() {
		for (int i = 0; i < 5; ++i) {
			emptySlots.acquire();          // P
			buffer.push_back(i);
			fullSlots.release();           // V
		}
	};

	auto consumer = [&]() {
		for (int i = 0; i < 5; ++i) {
			fullSlots.acquire();           // P
			int value = buffer.back();
			buffer.pop_back();
			emptySlots.release();          // V
			std::cout << value << std::endl;
		}
	};

	std::thread t1(producer);
	std::thread t2(consumer);
	t1.join();
	t2.join();
	return 0;
}
```

### 示例：PV 操作伪代码（OS 视角）

```text
semaphore S = N

P(S):
	S = S - 1
	if S < 0:
		block current

V(S):
	S = S + 1
	if S <= 0:
		wakeup one
```

### 小结

- **PV 操作是信号量的经典抽象**，C++ 的 `acquire/release` 与 `wait/post` 直接对应它。
- **信号量适合计数型资源控制与线程同步**，可用于生产者/消费者、限制并发度等场景。
