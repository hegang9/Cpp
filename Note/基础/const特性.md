# const 特性详解

## 修饰内置基本类型变量

对于基本类型，`const` 写在类型名前后意义相同。

```cpp
int const MAX = 100;
const int MIN = 0;
```

## 指向常量的指针

### 定义

`const` 在 `*` 号之前。

```cpp
int x = 1;
const int* p1 = &x;     // 类型前 const
int const* p2 = &x;     // 类型后 const （与上等价）
```

### 特点

- 指针指向的对象可变
- 指向的对象的值不可变
- 可以改变指针指向的对象，但不能通过指针修改对象的值

## 常量指针

### 定义

`const` 在 `*` 号之后。

```cpp
int* const p3 = &x;
```

### 特点

- 指针指向的对象**不可变**（绑定死了）
- 指向的对象的值可变
- 指针被"绑定"到某个对象，不能指向其他对象，但可以通过指针修改对象的值

## 指向常量数据的常量指针

### 定义

```cpp
const int* const p4 = &x;
```

### 特点

- 指针和指向的对象的值**都不可变**
- 既不能改变指针指向的对象，也不能通过指针修改对象的值

## 常成员函数

### 定义

`const` 在函数声明末尾。

```cpp
class MyClass {
public:
    void normalFunc();                // 普通成员函数
    void constFunc() const;           // const 成员函数（位置在参数列表后）
};
```

### 特点

- 不能修改非 `mutable` 成员变量
- 可以调用其他 const 成员函数
- 可以调用静态成员函数

## 常量类成员变量

### 方式一：内联初始化（仅整型和枚举）

```cpp
class MyClass1 {
public:
    const int ID = 1;  // 常量成员
};
```

### 方式二：静态常量

```cpp
class MyClass1 {
public:
    static const int MAX1 = 100;  // 静态常量（可在类内初始化）
};
```

### 方式三：在构造函数初始化列表中初始化

```cpp
class MyClass1 {
private:
    const int id;
public:
    MyClass1(int i) : id(i) {}  // 在初始化列表中初始化常量成员
};
```

## mutable 成员

### 用途

允许在 `const` 成员函数中修改某些成员。

```cpp
class MyClass1 {
public:
    mutable int cache;  // 特殊：可在 const 成员函数中修改
    
    void someConstFunc() const {
        cache = 10;  // 允许修改 mutable 成员
    }
};
```

### 常见应用

- 缓存：在 const 函数中更新计算结果的缓存
- 计数器：记录 const 函数被调用的次数
- 日志：在 const 函数中记录调用日志

## const 总结表格

| 声明方式 | 指针可变 | 所指对象可变 | 示例 |
|---------|--------|----------|------|
| `const int* p` | ✓ | ✗ | 指向常量的指针 |
| `int* const p` | ✗ | ✓ | 常量指针 |
| `const int* const p` | ✗ | ✗ | 指向常量的常量指针 |
| `const` 成员函数 | - | ✗ (非mutable) | 不修改对象状态 |
