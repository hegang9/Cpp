#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
using namespace std;

class RandomizedSet
{
public:
	RandomizedSet() : num(0)
	{
	}

	bool insert(int val)
	{
		// 若元素已经存在，返回false
		if (val2index.find(val) != val2index.end())
		{
			return false;
		}

		set.emplace_back(val);
		val2index[val] = num;
		num += 1;
		return true;
	}

	bool remove(int val)
	{
		// 若元素不存在，返回false
		if (val2index.find(val) == val2index.end())
		{
			return false;
		}

		int index = val2index[val]; // 找到下标

		val2index[set[num - 1]] = index; // 更新原末尾元素的下标
		val2index.erase(val);			 // 删除对应元素哈希表项

		set[index] = set[num - 1];

		set.pop_back();

		num -= 1; // 将集合元素数-1

		return true;
	}

	int getRandom() const
	{
		srand(time(0));
		int random_number = rand() % num;

		return set[random_number];
	}

private:
	// 设计数据结构：使用数组vector来存储集合元素，这样插入直接push_back，满足了O(1)插入；要实现O(1)删除，就需要使用哈希表unordered_map记录每个元素在数组中的下标，删除时先从哈希表中找到元素所在下标，再将其与末尾元素替换后使用pop_back删除，避免直接删除带来的元素移动开销，实现O(1)删除；对于“每个元素应该有 相同的概率 被返回”这个要求
	std::vector<int> set;
	unordered_map<int, int> val2index;
	int num; // 记录集合元素个数
};

int main()
{
	RandomizedSet randomizedSet;
	cout << randomizedSet.remove(0) << endl;   // 向集合中插入元素1。返回true表示插入成功。
	cout << randomizedSet.remove(0) << endl;   // 从集合中移除元素2。返回false表示元素不存在。
	cout << randomizedSet.insert(0) << endl;   // 向集合中插入元素2。返回true表示插入成功。
	cout << randomizedSet.getRandom() << endl; // 从集合中随机返回一个元素。由于集合包含1和2，所以有50%的概率返回1，50%的概率返回2。
	cout << randomizedSet.remove(0) << endl;   // 从集合中移除元素1。返回true表示移除成功。
	cout << randomizedSet.insert(0) << endl;   // 向集合中插入元素2。返回false表示元素已经存在。

	return 0;
}