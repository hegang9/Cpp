#include <iostream>
#include <string>
using namespace std;
char *GetMemory2(void)
{
	char p[] = "hello world";
	return p;
}

void Test2(void)
{
	char *str = NULL;
	str = GetMemory2();
	printf(str);
}

int main()
{
	Test2();
	return 0;
}