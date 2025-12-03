#include "my_string.h"

MyString::MyString()
{
    // 默认构造函数，初始化为空字符串
    data = new char[1];
    data[0] = '\0';
    length = 0;
}

MyString::MyString(const char *str)
{
    // 构造函数，初始化为给定的C字符串
    // 先检查传入的指针的有效性
    if (!str)
    {
        data = new char[1];
        data[0] = '\0';
        length = 0;
    }
    else
    {
        length = std::strlen(str);
        data = new char[length + 1];
        std::strcpy(data, str);
    }
}

MyString::MyString(const MyString &other)
{
    // 拷贝构造函数
    length = other.length;
    data = new char[length + 1];
    std::strcpy(data, other.data);
}

MyString::~MyString()
{
    // 析构函数，释放动态分配的内存
    delete[] data;
}

size_t MyString::size() const
{
    return size_t();
}

const char *MyString::c_str() const
{
    // 返回C字符串表示
    return data;
}

MyString MyString::operator+(const MyString &other) const
{
    // 字符串连接操作符
    MyString result;      // 需要重新分配内存
    delete[] result.data; // 释放默认构造函数分配的内存

    result.length = length + other.length;
    result.data = new char[result.length + 1];

    std::strcpy(result.data, data);
    std::strcat(result.data, other.data);

    return result;
}

MyString &MyString::operator=(const MyString &other)
{
    // 赋值操作符
    if (this != &other) // 防止自我赋值
    {
        delete[] data; // 释放当前对象的内存

        length = other.length;
        data = new char[length + 1];
        std::strcpy(data, other.data);
    }
    return *this;
}
