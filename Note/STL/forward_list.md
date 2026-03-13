## forward_list
`std::forward_list` 是 C++11 引入的一个单向链表容器，提供了一个轻量级的链表实现。与 `std::list` 不同，`std::forward_list` 只支持单向迭代器，因此它的功能相对有限，但在某些场景下可以提供更好的性能和更低的内存开销。
### 1. 基本特性
- **单向链表**：`std::forward_list` 只支持单向迭代器，意味着只能从头部向尾部遍历，无法进行双向迭代。
- **内存效率**：由于只需要一个指针来链接节点，`std::forward_list` 的内存开销比 `std::list` 更小，适合需要大量小对象的场景。
- **不支持大小查询**：`std::forward_list` 不提供 `size()` 成员函数，因为计算大小需要遍历整个链表，时间复杂度为 O(n)。
### 2. 常用成员函数
- **`push_front(const T& value)`**：在链表头部插入一个元素。
- **`pop_front()`**：删除链表头部的元素。
- **`insert_after(iterator pos, const T& value)`**：在指定位置之后插入一个元素。
- **`erase_after(iterator pos)`**：删除指定位置之后的元素。
- **`before_begin()`**：返回一个特殊的迭代器，指向链表头部之前的位置，用于在链表头部插入元素。
- **`begin()`**：返回指向链表第一个元素的迭代
- **`end()`**：返回指向链表最后一个元素之后位置的迭代器。