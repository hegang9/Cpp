#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
	vector<int> numbers{1, 2, 3, 4, 5};
	sort(numbers.rbegin(), numbers.rend());
	for (const auto &num : numbers)
	{
		cout << num << " ";
	}
	cout << endl;
	return 0;
}