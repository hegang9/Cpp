# deque

## 结构与迭代器

deque 是一个双端开口的线性空间，其内部为分段连续的空间（每段空间大小相等，称为分块数组）组成，随时可以增加一段新的空间并链接，其内部使用一个 map 数组（称为中控映射）存储每段空间的首地址，从而实现整体连续、局部分割的效果。当 deque 容器需要在头部或尾部增加存储空间时，它会申请一段新的连续空间，同时在 map 数组的开头或结尾添加指向该空间的指针，由此该空间就串接到了 deque 容器的头部或尾部。这些设计使得 deque 容器的迭代器十分复杂：

```c
template<class T,...>
struct __deque_iterator{
    ...
    T* cur;
    T* first;
    T* last;
    map_pointer node;//map_pointer 等价于 T**
}
```

迭代器内部包含四个指针：
- cur：指向当前正在遍历的元素
- first：指向当前连续空间的首地址
- last：指向当前连续空间的末尾地址
- node：一个二级指针，用于指向 map 数组中存储的指向当前连续空间的指针

## 时间性能
1、随机访问：O(1)，但常数因子比 vector 大，因为需要计算索引和偏移量，即随机访问稍慢于 vector
2、头尾插入删除操作：O(1)
3、中间插入删除操作：O(n)

## 注意
头尾插入操作若导致 map 数组空间不足，会重新找一个更大的连续空间存放 map 数组，并将原 map 数组中的指针复制到新 map 数组中，这会导致所有迭代器失效，但不会导致指针和引用失效，因为分块数组中的元素位置并没有变化；而中间操作会导致操作点之后的迭代器、指针、引用全部失效。

## deque 常用成员函数
- `iterator begin() noexcept;` 返回指向容器第一个元素的迭代器
- `iterator end() noexcept;` 返回指向容器最后一个元素之后位置的迭代器
- `bool empty() const noexcept;` 检查容器是否为空，若为空返回 true，否则返回 false
- `size_type size() const noexcept;` 返回容器中当前元素的数量
- `size_type max_size() const noexcept;` 返回容器由于系统或库的限制所能容纳的最大元素数量
- `void resize(size_type count);` 改变容器的大小为 count，若扩大则默认构造新元素，若缩小则截断
- `void shrink_to_fit();` 请求移除未使用的容量，释放多余内存（非强制）
- `reference operator[](size_type pos);` 返回位置 pos 处元素的引用，不进行边界检查
- `reference at(size_type pos);` 返回位置 pos 处元素的引用，进行边界检查，越界抛出 `std::out_of_range` 异常
- `reference front();` 返回容器第一个元素的引用
- `reference back();` 返回容器最后一个元素的引用
- `void push_back(const T& value);` 将元素 value 的副本添加到容器末尾
- `void pop_back();` 移除容器末尾的元素
- `void push_front(const T& value);` 将元素 value 的副本添加到容器头部
- `void pop_front();` 移除容器头部的元素
- `template<class... Args> reference emplace_back(Args&&... args);` 在容器末尾原地构造元素，参数 args 被转发给构造函数
- `template<class... Args> reference emplace_front(Args&&... args);` 在容器头部原地构造元素，参数 args 被转发给构造函数
- `iterator insert(const_iterator pos, const T& value);` 在 pos 指定的位置前插入元素 value，返回指向新插入元素的迭代器
- `template<class... Args> iterator emplace(const_iterator pos, Args&&... args);` 在 pos 指定的位置前原地构造元素，返回指向新元素的迭代器
- `iterator erase(const_iterator pos);` 移除 pos 指定位置的元素，返回指向被移除元素之后元素的迭代器
- `void clear() noexcept;` 移除容器中的所有元素，size() 变为 0
- `void swap(deque& other) noexcept;` 交换两个 deque 的内容，效率高（常数时间）
- `void assign(size_type count, const T& value);` 将容器内容替换为 count 个 value 的副本
- `template<class InputIt> void assign(InputIt first, InputIt last);` 将容器内容替换为范围 [first, last) 中的元素

## 常与 deque 组合作使用的函数（需包含 <algorithm> 或 <numeric>）
- `void sort(RandomIt first, RandomIt last);` 对范围 [first, last) 内的元素进行升序排序
- `void sort(RandomIt first, RandomIt last, Compare comp);` 使用自定义比较函数 comp 对范围 [first, last) 内的元素进行排序
- `void reverse(BidirectionalIt first, BidirectionalIt last);` 反转范围 [first, last) 内元素的顺序
- `InputIt find(InputIt first, InputIt last, const T& value);` 在范围 [first, last) 中查找等于 value 的第一个元素
- `InputIt find_if(InputIt first, InputIt last, UnaryPredicate p);` 在范围 [first, last) 中查找满足谓词 p 的第一个元素
- `typename iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last, const T& value);` 统计范围 [first, last) 中等于 value 的元素个数
- `typename iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last, UnaryPredicate p);` 统计范围 [first, last) 中满足谓词 p 的元素个数
- `UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f);` 对范围 [first, last) 中的每个元素应用函数 f
- `OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op);` 对范围 [first1, last1) 中的每个元素应用 unary_op，并将结果存入从 d_first 开始的范围
- `ForwardIt remove(ForwardIt first, ForwardIt last, const T& value);` 移除范围 [first, last) 中所有等于 value 的元素（逻辑移除，需配合 erase 使用）
- `ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate p);` 移除范围 [first, last) 中所有满足谓词 p 的元素（逻辑移除，需配合 erase 使用）
- `ForwardIt unique(ForwardIt first, ForwardIt last);` 移除范围 [first, last) 中连续重复的元素（逻辑移除，需配合 erase 使用）
- `T accumulate(InputIt first, InputIt last, T init);` 计算范围 [first, last) 中所有元素的和（需包含 <numeric> 头文件），初始值为 init
