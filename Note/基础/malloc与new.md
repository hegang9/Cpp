# malloc 与 new

## malloc 函数介绍

### 基本概念

`malloc` 函数是 C/C++ 标准库中的一个内存分配函数，用于动态分配指定大小的内存块。

### 返回值

- 返回指向已分配空间的 `void` 指针
- 如果可用内存不足，则返回 `NULL`
- 若要返回指向类型而非 `void` 的指针，需要在返回值上使用类型转换

### 特殊情况

- 如果 size 为 0，则 malloc 在堆中分配零长度的项并向该项返回有效的指针
- malloc 函数分配至少为 size 个字节的内存块
- 由于对齐和维护信息所需的空间，该块可能大于 size 个字节

### 头文件

定义在头文件 `<stdlib.h>` 或 `<cstdlib>` 中。

```cpp
#include <cstdlib>
```

## malloc 与 new 的区别

| 特性 | malloc/free | new/delete |
|-----|-----------|-----------|
| 语言属性 | C 语言函数 | C++ 运算符 |
| 重载 | 不允许重载 | 允许重载 |
| 构造/析构 | 不调用 | 会调用 |
| 分配来源 | 堆 | 自由存储区 |
| 返回类型 | void* | 具体类型指针 |
| 失败返回 | NULL | 抛出异常 |

### 详细说明

1. **malloc/free 是 C 语言的内存分配函数**
   - 不允许重载
   - 不调用构造函数和析构函数

2. **new/delete 是 C++ 的内存分配运算符**
   - 允许重载
   - 会调用构造函数和析构函数

3. **分配来源不同**
   - new 从自由存储区分配内存
   - malloc 从堆中分配内存
   - 自由存储区通常以堆为基础，但它是一个更高级的概念，允许定制

## malloc/free 使用示例

```cpp
#include <iostream>
#include <cstdlib>
using namespace std;

void example_malloc_free()
{
    // 分配 10 个整数的内存
    int* arr = (int*)malloc(10 * sizeof(int));
    
    // 检查分配是否成功
    if (arr == nullptr) {
        cerr << "Memory allocation failed!" << endl;
        return;
    }
    
    // 初始化数组
    for (int i = 0; i < 10; ++i) {
        arr[i] = i * 10;
    }
    
    // 输出数组内容
    for (int i = 0; i < 10; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    // 释放内存
    free(arr);
}

int main()
{
    example_malloc_free();
    return 0;
}
```

## 选择建议

### 何时使用 new/delete

- 在 C++ 程序中，通常**优先使用 new/delete**
- 特别是当涉及对象（需要调用构造函数/析构函数）时

### 何时使用 malloc/free

- 需要与 C 代码互操作时
- 处理基本数据类型的大块内存时
- 需要与现有 C 库集成时

### 智能指针

在现代 C++ 中，更推荐使用**智能指针**（`std::unique_ptr`、`std::shared_ptr`）自动管理内存，避免手动调用 `delete`。
