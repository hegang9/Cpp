## static所有用法

### 1. 修饰局部变量（函数体内）
- **作用**：使变量具有静态存储期，生命周期为整个程序运行期，但作用域仅限于函数体内。
- **特性**：只初始化一次，后续调用保留上次值。
```cpp
void foo() {
	static int count = 0;
	++count;
	std::cout << count << std::endl;
}
```

### 2. 修饰全局变量/函数（文件作用域）
- **作用**：限制变量/函数的链接属性为内部链接（internal linkage），只在当前源文件可见，防止外部访问和重名冲突。
```cpp
static int g_val = 42; // 仅当前文件可见
static void helper() { /* ... */ } // 仅当前文件可用
```

### 3. 修饰类的静态成员变量
- **作用**：属于整个类而非某个对象，所有对象共享同一份数据。
- **特性**：需在类外定义和初始化。
```cpp
class MyClass {
public:
	static int count;
};
int MyClass::count = 0;
```

### 4. 修饰类的静态成员函数
- **作用**：不依赖于具体对象，可通过类名直接调用。
- **特性**：**不能访问非静态成员变量和this指针**。
```cpp
class MyClass {
public:
	static void show() { std::cout << "static func" << std::endl; }
};
MyClass::show();
```

### 5. 修饰命名空间作用域变量（C++17起）
- **作用**：在匿名命名空间或命名空间内修饰变量，等价于内部链接。
```cpp
namespace {
	static int ns_val = 1; // 仅当前编译单元可见
}
```

### 6. static与constexpr/const的区别
- static 影响存储期和链接属性，const/constexpr 影响常量性和编译期计算。
- 可组合使用：static const int x = 10;

### 7. static的注意事项
- 静态局部变量多线程下需注意竞态条件。
- 静态成员变量需类外定义，否则链接错误。
- 静态函数不能被虚函数重写。

---
> static 是C/C++中最常用的存储类说明符之一，掌握其用法有助于理解对象生命周期、作用域和链接属性。
