#include <iostream>
#include <string>
#include <fstream> //文件读写头文件
using namespace std;


int main()
{
	//文件写
	ofstream outFile;
	outFile.open("Note/文件读写操作.txt");
	if (outFile.is_open()) {
		outFile << "文件读写操作" << endl;
	}
	else {
		cout << "没有打开文件" << endl;
	}
	outFile.close();

	//文件读
	ifstream inFile;
	inFile.open("Note/文件读写操作.txt");
	string in;
	inFile >> in;
	cout << in;

	inFile.close();

	return 0;
}