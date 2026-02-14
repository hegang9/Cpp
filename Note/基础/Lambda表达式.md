# Lambda 表达式

## Lambda 完整语法

```
[capture](parameters) mutable -> return_type { body }
```

### 语法要素说明

- `[capture]`：捕获列表，指定如何捕获外部变量
- `parameters`：参数列表（可选）
- `mutable`：允许修改按值捕获的变量（可选）
- `-> return_type`：返回类型（可选，编译器可推导）
- `{ body }`：函数体

## 捕获方式

### 基本捕获方式

| 捕获方式 | 说明 |
|--------|------|
| `[]` | 不捕获任何变量 |
| `[=]` | 按值捕获所有外部变量 |
| `[&]` | 按引用捕获所有外部变量 |
| `[this]` | 捕获当前对象的 this 指针 |
| `[a, &b]` | 按值捕获 a，按引用捕获 b |
| `[=, &a]` | 按值捕获所有变量，但按引用捕获 a |
| `[&, a]` | 按引用捕获所有变量，但按值捕获 a |

## 底层实现原理

Lambda 表达式在编译时被转换为一个**匿名的函数对象类（functor）**。所以 Lambda 表达式实际上是一个函数对象类的实例。

### 编译器转换示例

```cpp
// Lambda 表达式
auto lambda = [x, &y](int a) { return x + y + a; };

// 编译器大致生成如下类
class __lambda_unique_name {
private:
    int x;      // 按值捕获
    int& y;     // 按引用捕获
public:
    __lambda_unique_name(int _x, int& _y) : x(_x), y(_y) {}
    
    auto operator()(int a) const {
        return x + y + a;
    }
};
```

## 重要注意事项

### const 问题

默认情况下 `operator()` 是 `const` 的，函数内部不能修改按值捕获的变量。

如果需要修改，需要在捕获列表后加 `mutable` 关键字：

```cpp
int x = 10;
auto p = [x](int a) mutable -> int {
    x += 1;  // 允许修改 x
    return x + a;
};
```

**重要**：这里的 const 只针对按值捕获的变量，对于传入的变量（如参数 a）和按引用捕获的变量（如 y）不受影响。

### const 修改的效果

```cpp
int x = 10;
int y = 20;
auto p = [x, y](int a) mutable -> int {
    return x + y + a;
};
cout << p(30) << endl;  // 输出 60
x += 1;
cout << p(30) << endl;  // 输出 60（x、y 均按值捕获）
```

**说明**：x、y 均按值捕获，只是在 lambda 函数对应的类对象初始化时被拷贝了一次，之后无论如何修改 x、y 均不会影响 lambda 函数体内的值。

### this 指针

`this` 永远是**按引用捕获**的。

## GCC 编译器特性

若使用的是 GCC 编译器，可以直接使用 `lambda.x` 访问类成员，但一般不推荐使用。

## C++14 泛型 Lambda

### 新标准特性

在 C++14 标准之后，引入了泛型 Lambda，允许使用 `auto` 作为参数类型。

### 示例

```cpp
auto p = [](auto a, auto b) { return a + b; };
cout << p(1, 2) << endl;        // 整数相加
cout << p(1.5, 2.5) << endl;    // 浮点数相加
```

## 完整示例

```cpp
#include <iostream>
using namespace std;

int main()
{
    int x = 10;
    int y = 20;
    
    // 可修改的 Lambda 表达式
    auto p = [x, y](auto a) mutable -> int {
        x++;
        return x + y + a;
    };
    
    cout << p(30) << endl;  // 输出 61（x 在 lambda 内部被修改）
    cout << p(30) << endl;  // 输出 62（x 继续自增）
    
    return 0;
}
```
