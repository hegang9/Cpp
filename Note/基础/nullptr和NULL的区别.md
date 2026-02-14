# nullptr 和 NULL 的区别

## nullptr

### 定义

`nullptr` 是**空指针类型**。

### 特点

- 可隐式转换为任意指针
- 不能转换为整数
- 类型安全性强

## NULL

### 定义

`NULL` 是**宏定义**。

### 特点

- 通常定义为 0 或 `(void*)0`
- 其本质还是整数常量
- 在隐式转换为指针类型时可能发生分歧

### 转换问题

```cpp
// 可能的问题
void func(int x);
void func(int* ptr);

func(NULL);  // 二义性：NULL 既可被视为 0（int），也可被视为空指针
```

## 对比表

| 特性 | nullptr | NULL |
|-----|---------|------|
| 类型 | 空指针类型 | 宏定义（整数常量） |
| 转换为指针 | ✓ 安全 | ✗ 可能歧义 |
| 转换为整数 | ✗ | ✓ |
| 类型安全 | 高 | 低 |

## 推荐做法

**推荐使用 `nullptr`**

- `nullptr` 不能转换为整数，类型安全性强
- 避免在函数重载中产生歧义
- 是 C++11 标准推荐的做法

### 示例

```cpp
void func(int x);
void func(int* ptr);

func(nullptr);  // 明确调用 func(int* ptr)，无歧义
```
