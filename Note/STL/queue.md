# std::queue 详解

`std::queue` 是 C++ 标准模板库（STL）中的一种**容器适配器（Container Adapter）**，它提供了一种先进先出（FIFO，First-In-First-Out）的数据结构。元素从队尾（back）入队，从队头（front）出队。

## 1. 底层实现原理

### 1.1 容器适配器机制
`std::queue` 本身并不是一个全新实现的独立容器，而是基于现有的基础容器封装而成的**适配器**。它通过屏蔽底层容器的某些操作，只暴露特定的 FIFO 接口（如入队、出队、查看队首和队尾）来实现队列的语义。

### 1.2 默认底层容器：`std::deque`
在 STL 的各种实现（如 GCC、MSVC）中，`std::queue` 默认使用的底层容器是 **`std::deque`（双端队列）**。
- **为什么选择 `deque`？** 
  `deque` 允许在头部和尾部都进行高效的插入和删除操作（时间复杂度均为 $O(1)$），而且内存是分段连续的，扩容时代价较小，不会像 `vector` 那样需要频繁搬移大量数据，非常适合用于必须在两端进行操作的队列结构。

### 1.3 自定义底层容器
虽然默认是 `deque`，但你也可以在定义时显式指定使用其他容器，比如 `std::list`。
**只要该容器支持以下四种操作即可被用作 queue 的底层容器**：
- `front()`：访问首元素
- `back()`：访问尾元素
- `push_back()`：在尾部添加元素
- `pop_front()`：在头部移除元素

*(注意：`std::vector` 不能作为 `queue` 的底层容器，因为它不提供高效的 `pop_front()` 方法。)*

```cpp
// 默认使用 deque
std::queue<int> q1; 

// 显式指定使用 list 作为底层实现
std::queue<int, std::list<int>> q2; 
```

## 2. 常用方法 (API)

要使用 `queue`，需要包含头文件 `<queue>`：
```cpp
#include <queue>
```

| 方法名 | 原型与说明 | 时间复杂度 |
| :--- | :--- | :--- |
| `push(x)` | 将元素 `x` 压入队尾。 | $O(1)$ |
| `emplace(args...)` | 在队尾原地构造一个元素，相比 `push` 可避免不必要的拷贝/移动（C++11）。 | $O(1)$ |
| `pop()` | 移除队头元素。（注意：**该方法只移除元素，不返回被移除的元素值**） | $O(1)$ |
| `front()` | 返回队头元素的引用（不会移除该元素）。如果队列为空，行为未定义。 | $O(1)$ |
| `back()` | 返回队尾元素的引用。如果队列为空，行为未定义。 | $O(1)$ |
| `empty()` | 返回布尔值，检查队列是否为空（`true` 表示空）。 | $O(1)$ |
| `size()` | 返回队列中当前元素的个数。 | $O(1)$ |
| `swap(other)` | 交换当前队列和 `other` 队列的内容。 | $O(1)$ |

## 3. 基本使用示例
```cpp
#include <iostream>
#include <queue>

int main() {
    std::queue<int> myQueue;

    // 入队
    myQueue.push(10);
    myQueue.push(20);
    myQueue.push(30);

    std::cout << "队列大小: " << myQueue.size() << std::endl; // 输出 3

    // 访问队头和队尾
    std::cout << "队头元素: " << myQueue.front() << std::endl; // 输出 10
    std::cout << "队尾元素: " << myQueue.back() << std::endl;  // 输出 30

    // 遍历并清空队列
    while (!myQueue.empty()) {
        std::cout << "出队: " << myQueue.front() << std::endl;
        myQueue.pop(); // 弹出队头
    }

    return 0;
}
```