# heap 与 priority_queue

## heap
heap：建立在完全二叉树上，分为两种，大根堆、小根堆，其在 STL 中做 priority_queue 的助手。

## priority_queue
priority_queue：优先队列，优先队列是一种特殊的队列，它允许我们快速访问队列中具有最高（或最低）优先级的元素。默认是一个大根堆，这意味着队列的顶部元素总是具有最大的值。是一个容器适配器，底层默认使用 vector，也可以使用 deque。

### 时间性能
1、插入元素 push 和删除堆顶 pop 操作：O(log n)
2、访问堆顶：O(1)

### 注意
priority_queue 不支持除了访问堆顶之外的访问。

### 改用小根堆语法
`priority_queue<int, vector<int>, greater<int>> min_heap;`

### priority_queue 常用成员函数
- `const_reference top() const;` 返回优先队列中具有最高优先级的元素的引用（堆顶元素）
- `bool empty() const;` 检查容器是否为空，若为空返回 true，否则返回 false
- `size_type size() const;` 返回容器中当前元素的数量
- `void push(const T& value);` 将元素 value 插入优先队列，并重新排序以保持堆特性
- `void push(T&& value);` 将元素 value 移动插入优先队列，并重新排序以保持堆特性
- `template<class... Args> void emplace(Args&&... args);` 原地构造元素并插入优先队列，并重新排序
- `void pop();` 移除优先队列中具有最高优先级的元素（堆顶元素）
- `void swap(priority_queue& other) noexcept;` 交换两个优先队列的内容

## STL heap 算法函数（需包含 <algorithm>，作用于随机访问迭代器容器如 vector）
- `void make_heap(RandomIt first, RandomIt last);` 将范围 [first, last) 中的元素重新排列，使其成为一个堆（默认大根堆）
- `void make_heap(RandomIt first, RandomIt last, Compare comp);` 使用比较函数 comp 将范围 [first, last) 中的元素重新排列成为一个堆
- `void push_heap(RandomIt first, RandomIt last);` 将范围 [first, last-1) 视为堆，将位置 last-1 处的元素插入堆中
- `void push_heap(RandomIt first, RandomIt last, Compare comp);` 使用比较函数 comp，将位置 last-1 处的元素插入堆中
- `void pop_heap(RandomIt first, RandomIt last);` 将堆顶元素（first）移动到 last-1 位置，并将剩余范围 [first, last-1) 重新调整为堆
- `void pop_heap(RandomIt first, RandomIt last, Compare comp);` 使用比较函数 comp，将堆顶元素移动到 last-1，并调整剩余范围为堆
- `void sort_heap(RandomIt first, RandomIt last);` 将堆 [first, last) 转换为升序序列（排序后不再是堆）
- `void sort_heap(RandomIt first, RandomIt last, Compare comp);` 使用比较函数 comp，将堆转换为排序序列
- `bool is_heap(RandomIt first, RandomIt last);` 检查范围 [first, last) 是否是一个堆
- `bool is_heap(RandomIt first, RandomIt last, Compare comp);` 使用比较函数 comp，检查范围是否是一个堆
- `RandomIt is_heap_until(RandomIt first, RandomIt last);` 返回范围 [first, last) 中第一个破坏堆性质的元素位置
