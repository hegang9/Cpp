## priority_queue
`std::priority_queue`是STL中的一个适配器容器，用于实现优先级队列。优先级队列是一种特殊的队列，其中每个元素都有一个优先级，元素按照优先级顺序进行访问。默认情况下，`std::priority_queue`使用最大堆（max-heap）来实现，即优先级最高的元素会被最先访问。

### priority_queue的定义
```cpp
template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class priority_queue;
```

### priority_queue的用法
`std::priority_queue` 包含三个模板参数：`T`（存储的元素类型），`Container`（底层容器，默认为 `std::vector<T>`），`Compare`（比较器，决定优先级规则，默认为 `std::less<T>`，即最大堆）。

要完整使用所有模板参数，可以改变优先队列的顺序（例如实现最小堆）或使用自定义类型的比较法则。

```cpp
#include <iostream>
#include <queue>
#include <vector>

// 3. 自定义比较器：让绝对值大的数优先级小
struct CustomCompare {
    bool operator()(const int& lhs, const int& rhs) const {
        return abs(lhs) > abs(rhs); // 注意：优先队列的Compare和sort逻辑相反，返回true表示优先级更低
    }
};

int main() {
    // 1. 默认用法：最大堆（底层默认使用 std::vector，比较器默认使用 std::less）
    std::priority_queue<int> max_pq;
    max_pq.push(3); max_pq.push(1); max_pq.push(2);
    std::cout << "Max Heap: ";
    while (!max_pq.empty()) { std::cout << max_pq.top() << " "; max_pq.pop(); } // 3 2 1
    std::cout << "\n";

    // 2. 最小堆：必须显式指定三个模板参数
    // 参数1：数据类型 int
    // 参数2：底层容器 std::vector<int>
    // 参数3：比较器 std::greater<int> (使得较小的元素优先级最高)
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
    min_pq.push(3); min_pq.push(1); min_pq.push(2);
    std::cout << "Min Heap: ";
    while (!min_pq.empty()) { std::cout << min_pq.top() << " "; min_pq.pop(); } // 1 2 3
    std::cout << "\n";

    // 3. 自定义比较器
    std::priority_queue<int, std::vector<int>, CustomCompare> custom_pq;
    custom_pq.push(3); custom_pq.push(-5); custom_pq.push(1);
    std::cout << "Custom Heap: ";
    while (!custom_pq.empty()) { std::cout << custom_pq.top() << " "; custom_pq.pop(); } // 1 3 -5
    std::cout << "\n";

    return 0;
}
```

### priority_queue的成员函数
- `empty()`: 检查优先级队列是否为空。
- `size()`: 返回优先级队列中的元素数量。
- `top()`: 返回优先级最高的元素。
- `push()`: 向优先级队列中添加元素。
- `pop()`: 从优先级队列中移除最高优先级的元素。
- `swap()`: 交换两个优先级队列的内容。
- `emplace()`: 在优先级队列中直接构造一个元素。
- `emplace_hint()`: 在优先级队列中直接构造一个元素，并提供一个提示位置。
- `get_allocator()`: 返回优先级队列使用的分配器。

### 应用场景
优先级队列在许多算法和应用中非常有用，例如：
- Dijkstra算法：用于计算图中最短路径。
- 堆排序：利用堆的性质进行排序。
- 任务调度：根据任务的优先级进行调度。

### 性能
- 插入元素（push）和删除堆顶元素（pop）的时间复杂度为 **O(log n)**。
- 访问堆顶元素（top）的时间复杂度为 **O(1)**。

### 深入理解 priority_queue 的自定义比较
在 `std::priority_queue` 中，比较器（Comparator）的逻辑与通常的排序算法（如 `std::sort`）相比可能有些反直觉：
- **默认是最大堆**：使用 `std::less<T>`（对应 `<` 运算符）。
- **内部逻辑**：当比较器 `Compare(a, b)` 返回 `true` 时，在优先队列内部代表的意思是 **`a` 的优先级低于 `b`**（`a` 会被放到 `b` 的下面）。因此，返回 `true` 代表元素发生下沉。

我们有四种常见的方式来自定义优先队列的比较逻辑：

#### 1. 重载数据类型内部的 `<` 运算符
如果优先队列中存储的是自定义的结构体或类，最简单且自然的方法是直接重载结构体内部的 `operator<`。由于队列默认使用的是最大堆形式，如果我们要构建关于某个字段的最大堆，正常写 `<` 即可。

```cpp
struct Point {
    int x, y;
    // 重载 < 运算符
    bool operator<(const Point& other) const {
        // 我们想让 x 坐标大的在前面。如果当前元素的 x 比较小，返回 true，代表优先级低
        return this->x < other.x;
    }
};

// 默认使用 std::less，它内部会自动调用我们写好的重载 <
std::priority_queue<Point> pq; 
```

#### 2. 自定义仿函数（Functor）
使用一个重载了 `operator()` 的结构体或类。当无法或不方便修改原始类代码时（或者处理如 `pair` 或基础数据类型时），推荐使用这种方式。
```cpp
#include <queue>
#include <vector>

struct Cmp {
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
        // 目标：构造按 second 字段升序的【最小堆】
        // 记住口诀：返回 true 表示 a 的优先级比 b 低，a 会下沉！
        // 因此，如果我们要最小的元素在最上面，当 a.second > b.second 时，由于 a 更大，a 应该下沉（排在后面），所以此时返回 true。
        return a.second > b.second; 
    }
};

int main() {
    // 必须填满三个模板参数：数据类型，底层容器，和比较器类型
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, Cmp> pq;
    return 0;
}
```

#### 3. 使用 Lambda 表达式 (C++11 及以上)
对于一些局部的、只用一次的自定义比较法则，使用 Lambda 可以让代码更紧凑。由于 `std::priority_queue` 的模板参数需要的是“类型”，我们需要借助于 `decltype` 推导 Lambda 表达式的类型，通常**还要把 Lambda 实例作为参数传入给优先队列的构造函数**。

```cpp
#include <queue>
#include <vector>

int main() {
    // 自定义 lambda 法则
    auto cmp = [](int left, int right) {
        // 以偶数优先为例：或者说你想制定任何奇怪的规则。
        // 返回true代表left优先级更低。
        // 这里举个简单的最小堆例子：
        return left > right; 
    };
    
    // 模板参数填入 decltype(cmp)，并将 cmp 传入构造函数
    std::priority_queue<int, std::vector<int>, decltype(cmp)> pq(cmp);
    
    pq.push(3);
    pq.push(1);
    
    return 0;
}
```

#### 4. 使用函数指针 (传统做法)
与 Lambda 表达式相似，这种做法通常存在于较老的C++代码中，显得略微繁琐。

```cpp
bool myCmp(int a, int b) {
    return a > b; // 最小堆
}

int main() {
    // 传入函数指针类型，以及向构造函数传入该函数名
    std::priority_queue<int, std::vector<int>, bool(*)(int, int)> pq(myCmp);
    // 或者使用 decltype： std::priority_queue<int, std::vector<int>, decltype(&myCmp)> pq(myCmp);
    return 0;
}
```