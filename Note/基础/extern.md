# extern 关键字

## 基本用途

`extern` 用于"声明"外部变量，相当于将变量从其他文件中借来使用，实现共享全局资源。

## 重要区分

### 误点辨析

1. **只是声明，不分配内存空间**
2. **函数声明默认带有 `extern`**

## 扩展用法 1：extern "C"

### 问题背景

C++ 的行为与 C 的行为存在差异，导致链接时符号不匹配。

#### C++ 的行为

为了支持函数重载（同名不同参），C++ 编译器会将函数名"修饰"成独一无二的符号。

例如：`void func(int)` 可能会被编译成符号 `_Z4funci`

#### C 的行为

C 语言不支持重载，函数名就是符号名。

例如：`void func(int)` 编译后的符号就是 `_func`

### 解决方案

使用 `extern "C"` 避免名称修饰。

### 使用场景

1. **C++ 调用 C 语言库时**
   - 需要使用 `extern "C"` 来避免名称修饰
   - 确保链接时符号匹配

2. **C 语言调用 C++ 编写的函数时**
   - 也需要使用 `extern "C"` 来确保链接正确
   - 否则 C 程序将找不到函数名

### 语法

```cpp
extern "C" {
    void c_function(int);
}
```

### 完整示例

```cpp
// C++ 文件调用 C 函数
extern "C" {
    #include "c_header.h"
}

// 或者
extern "C" void c_func();
```

## 扩展用法 2：模板的显式实例化声明

### 问题背景

通常，模板代码写在头文件中。如果在 10 个 `.cpp` 文件中都使用了该类型模板的同一个实例，编译器会在每个 `.cpp` 文件中都生成一份该实例的代码。

虽然链接器最后会删除重复的副本，但这浪费了大量的编译时间。

### 解决方案

使用**显式实例化**和**显式实例化声明**来避免重复实例化。

### 语法

#### 在一个 .cpp 文件中进行显式实例化

```cpp
template void myFunc<int>(int);
```

这会强制编译器根据模板生成一个函数定义。

#### 在其他 .cpp 文件中声明（不生成定义）

```cpp
extern template void myFunc<int>(int);
```

这告诉编译器"这个实例化在别处已生成，不要重复生成"。

### 区别

| 操作 | 语法 | 作用 |
|-----|------|------|
| 显式实例化 | `template void func<int>(int);` | **生成**函数定义 |
| 显式实例化声明 | `extern template void func<int>(int);` | **声明**但不生成定义 |

### 示例

```cpp
// MyTemplate.cpp - 在此处显式实例化
template class MyClass<int>;
template void myFunc<int>(int);

// Other.cpp - 在其他文件中声明
extern template class MyClass<int>;
extern template void myFunc<int>(int);
```

## 总结

| 用途 | 语法 | 说明 |
|-----|------|------|
| 声明外部变量 | `extern int x;` | 从其他文件借用变量 |
| 声明外部函数 | `extern void func();` | 默认包含 |
| 声明 C 函数 | `extern "C" void cfunc();` | 避免名称修饰 |
| 显式实例化 | `template void func<int>();` | 生成模板实例 |
| 显式声明 | `extern template void func<int>();` | 声明外部模板实例 |
