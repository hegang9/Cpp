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