#include <stdio.h>
#include <iostream>
#include <functional>

using namespace std;

void fun(int a, int b, int c)
{
	cout << a << b << c << endl;
}

int main()
{
	auto f1 = bind(fun, placeholders::_1, placeholders::_2, placeholders::_3);
	auto f2 = bind(fun, 1, placeholders::_2, placeholders::_1);
	f1(1, 2, 3);
	f2(1, 2, 3);
}