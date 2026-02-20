## 非线程安全的 STL 容器
- STL 容器本身**不是线程安全**的（除少数实现细节外，标准不保证线程安全）
- 不论是顺序容器、关联容器还是无序容器，**同时写**都不安全

## 基本结论（标准层面）
- **读读并发**：多个线程只读同一个容器是安全的（不发生结构修改）
- **读写并发**：一个线程读、一个线程写同一容器是未定义行为
- **写写并发**：多个线程对同一容器写是未定义行为
- **独立对象**：不同容器对象在不同线程操作是安全的

## 需要特别注意的操作
- 会改变容器结构的操作：`insert`、`erase`、`push_back`、`pop_back`、`clear`、`resize`、`swap`
- 可能触发重新分配/重排：`vector`/`string` 的扩容、`unordered_*` 的 `rehash`/`reserve`
- 迭代器/引用/指针在并发修改时会失效或悬垂

## 常见误区
- “只要不同时写就安全”：**错误**，读写并发仍是未定义行为
- “函数是 const 就安全”：**错误**，const 仅表示接口不改结构，不代表线程安全
- “unordered 容器只要不 rehash 就安全”：**错误**，读写并发依然不安全

## 典型安全用法
- **只读共享**：所有线程仅执行只读操作（遍历、查找）
- **线程内独占**：一个容器只在一个线程中修改
- **外部同步**：对容器加互斥锁（`std::mutex`/`std::shared_mutex`）

## 读多写少场景建议
- 使用 `std::shared_mutex`：读锁共享、写锁独占
- 写操作尽量集中在单线程或批量更新

## 示例：共享互斥锁保护容器
```c
#include <shared_mutex>
#include <vector>

std::vector<int> v;
std::shared_mutex mtx;

void reader() {
	std::shared_lock<std::shared_mutex> lk(mtx);
	for (int x : v) {
		// 只读访问
	}
}

void writer(int value) {
	std::unique_lock<std::shared_mutex> lk(mtx);
	v.push_back(value);
}
```

## 容器级“线程安全”替代方案
- 并发场景可用更高层封装：
  - 读多写少：`std::shared_mutex` + 容器
  - 高并发队列：使用无锁/并发队列实现（第三方库）

## 结论
- STL 容器本身不提供线程安全保证
- 并发访问必须通过**外部同步**或**线程隔离**保证正确性
