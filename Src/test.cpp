#include <iostream>
#include <string>
using namespace std;

//在类中定义常量的方法：
class Student
{
private:
	enum { Month = 12 };
	static const int month = 12;

	//作用域内枚举：(注意：与传统枚举量不同，作用域内枚举量不能隐式转换为整型)
	enum class egg{small,large};
	enum class Tshirt{small,large};

public:
	int age;
	Student operator+(const Student& a) const;	//重载+操作符，重载函数可以不设置为成员函数

	//通过让函数成为类的友元，可以赋予该函数与类的成员函数相同的访问权限
	friend ostream& getAge(ostream& os,const Student& a);
};

Student Student::operator+(const Student& a) const
{
	Student sum;
	sum.age = this->age + a.age;
}

//友元函数的定义不要加frend修饰
ostream& getAge(ostream& os, const Student& a)
{
	os << a.age << endl;
	return os;
}

//重载运算符的限制：1、操作符的操作数至少有一个为用户自定义类型 2、从操作符必须遵循原来的句法规则和优先级
/*
不能重载的操作符：
sizeof：sizeof运算符。
.：成员运算符。
. *：成员指针运算符。
::：作用域解析运算符。
?:：条件运算符。
typeid：一个RTTI运算符。
const_cast：强制类型转换运算符。
dynamic_cast：强制类型转换运算符。
reinterpret_cast：强制类型转换运算符。
static_cast：强制类型转换运算符。
*/


int main()
{
	//两种调用+运算符的方法：
	Student s;
	Student t;
	Student sum = s + t;
	Student sum1 = s.operator+(t);
	return 0;
}