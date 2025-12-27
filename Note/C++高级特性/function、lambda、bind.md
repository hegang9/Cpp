## function
- 概念：一个抽象了函数参数以及函数返回值的类模板
- 特点：function对象可以拷贝
- 用途及其语法：
  - 保存普通函数和类静态成员函数
  - 保存仿函数（函数对象）：
    ```c++
    class Func{
    public:
        void operator()(int count){
            cout << count << endl;
        }
        void operator()(int a,int b){
            cout<< a+b << endl;
        }
    }
    // 使用仿函数对象对function对象进行初始化，会自动进行类型匹配到相应的operator()重载版本
    function<void(int)> f1 = Hello();
    f1(0);
    function<void(int,int)> f2=Hello();
    f2(0,1);
    ```

  - 保存类成员函数
    ```c++
    class C{
    public:
        void func(int count);
    }

    // 参数列表第一个为指向类对象的this指针，使用类成员函数的指针来初始化
    function<void(C*,int)> f3 = &C::func;
    C c;
    f3(c,0);
    ```

  - 保存lambda表达式
    ```c++
    function<int(int)> f4 = [](int a)->int{return a;};
    ```
  - 保存bind返回的函数对象
    - bind：一个函数适配器，主要用来绑定函数以及函数参数的方式生成函数对象（闭包），还提供占位符实现灵活绑定
    ```c++
    // 绑定普通函数
    void F(int i,int j);
    auto f5 = bind(&F,9,placeholders::_2); // 占位符后的_n表示这个传入的参数应该赋值给参数列表中的第几个参数，但是传入的参数的优先级比绑定的参数值的优先级低，也就是说，若此处为placeholders::_1，运行后第一个参数还是9而不是传入的0。
    f5(0);

    // 绑定类成员函数
    auto f6 = bind(&C::func,&c,0);
    f6();
    ```