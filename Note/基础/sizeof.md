# sizeof 运算符

## 基本概念

- `sizeof` 是在**编译期计算**的运算符
- **注意**：是运算符不是函数
- 不会产生运行时代码

## 基本用法

### 获取基本类型的大小

```cpp
sizeof(int)         // 获取 int 类型的大小
sizeof(x)           // 获取基本类型变量 x 的类型大小
```

### 获取数组的大小

```cpp
int arr[10];
sizeof(arr)         // 固定长度数组占用的总字节数
```

### 获取指针的大小

```cpp
sizeof(int*)        // 获取指针类型的大小
                    // 与操作系统有关，与指向的类型无关
                    // 32 位系统：4 字节
                    // 64 位系统：8 字节
```

## 进阶用法

### 获取函数返回值类型的大小

```cpp
int func();
sizeof(func())      // 获取函数返回值类型 int 的大小
```

**注意**：函数实际不会被调用，编译器只是从返回类型推导大小。

### 获取引用类型的大小

```cpp
int a = 10;
int& ref = a;
sizeof(ref)         // 获取引用所指向的类型大小，等同于 sizeof(int)
```

### 空类的大小

```cpp
class EmptyClass {};
sizeof(EmptyClass)  // 空类的大小为 1 字节
                    // 确保每个实例有唯一地址
```

### 结构体大小

```cpp
struct MyStruct {
    int a;      // 4 字节
    char b;     // 1 字节
    // 对齐填充：3 字节
};
sizeof(MyStruct)    // 可能是 8 字节（包含对齐填充）
                    // 可能大于各成员大小之和
```

### 类的大小

```cpp
class MyClass {
    int a;          // 4 字节
    virtual void func();  // 虚函数表指针：8 字节
};
sizeof(MyClass)     // 包含对齐填充和虚函数表指针
                    // 可能大于各成员大小之和
```

## 扩展用法 1：模板元编程

### 编译期断言

```cpp
template <typename T>
struct TypeSize {
    static constexpr size_t value = sizeof(T);
};

// 编译期检查
static_assert(sizeof(int) == 4, "int must be 4 bytes");
```

### 应用场景

- 编译期断言
- 内存池分配
- 类型推导

## 扩展用法 2：条件编译

```cpp
#if sizeof(void*) == 8
    // 64 位系统代码
    typedef long long PtrInt;
#else
    // 32 位系统代码
    typedef int PtrInt;
#endif
```

## 局限性和陷阱

### 1. 无法获取动态数组的大小

```cpp
int* arr = new int[10];
sizeof(arr);        // 8 字节（指针大小），而非 40 字节
                    // 只能获取指针大小，无法获取数组实际大小
```

### 2. 无法获取不完整类型的大小

```cpp
class Incomplete;   // 前向声明
sizeof(Incomplete); // 编译错误！
```

### 3. 数组退化为指针

```cpp
void func(int arr[]) {
    sizeof(arr);    // 8 字节（指针大小）
                    // 数组在函数参数中退化为指针
}

int arr[10];
sizeof(arr);        // 40 字节（在函数外，是真实数组）
```

## 实用技巧

### 获取数组元素个数

```cpp
int arr[10];
int count = sizeof(arr) / sizeof(arr[0]);  // 10
```

### 检查系统架构

```cpp
#if sizeof(void*) == 8
    // 64 位系统
#elif sizeof(void*) == 4
    // 32 位系统
#endif
```

### 编译期验证成员偏移

```cpp
struct Data {
    int x;
    char y;
};
static_assert(sizeof(Data) >= 5, "Data structure too small");
```
