#pragma once
#include <cstddef>
#include <cstring>

class MyString
{
private:
    char *data;
    size_t length;

public:
    MyString();
    MyString(const char *str);
    MyString(const MyString &other);
    ~MyString();

    size_t size() const;
    const char *c_str() const;
    MyString operator+(const MyString &other) const;
    MyString &operator=(const MyString &other);
};