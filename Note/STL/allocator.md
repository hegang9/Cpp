## allocator
allocator 是 STL 中的一个重要组件，它负责管理内存的分配和释放。它提供了一套接口，使得容器能够高效地使用内存。

### allocator 的基本概念
allocator 是一个模板类，定义在 `<memory>` 头文件中。它提供了以下主要成员函数：
- `allocate(size_type n)`: 分配 n 个元素的内存，但不构造对象。
- `deallocate(pointer p, size_type n)`: 释放之前分配的内存。
- `construct(pointer p, Args&&... args)`: 在已分配的内存上构造对象。
- `destroy(pointer p)`: 销毁对象，但不释放内存。
- `max_size()`: 返回 allocator 能分配的最大元素数量。
- `rebind<U>::other`: 定义一个新的 allocator 类型，用于分配不同类型的对象。

