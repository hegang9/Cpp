#include <iostream>
#include <string>
#include <fstream> //文件读写头文件
using namespace std;


int main()
{
	//文件写
	ofstream outFile;
	outFile.open("C++Primer/文件读写操作.txt");
	if (outFile.is_open()) {
		outFile << "文件读写操作";
	}
	else {
		cout << "没有打开文件" << endl;
	}
	//文件读
	ifstream inFile;
	inFile.open("C++Primer/文件读写操作.txt");
	string in;
	inFile >> in;
	cout << in;

	return 0;
}