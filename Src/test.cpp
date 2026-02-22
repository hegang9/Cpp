
#include <iostream>
#include <vector>
using namespace std;

class Solution
{
public:
	// 合并若干个区间的函数
	vector<int> func(vector<vector<int>> &intervals, int begin, int end)
	{
		// 合并intervals[begin,end]内的区间
		// 找左右边界
		int maxLeft = INT_MAX, maxRight = 0;
		for (int i = begin; i <= end; ++i)
		{
			maxLeft = min(maxLeft, intervals[i][0]);
			maxRight = max(maxRight, intervals[i][1]);
		}
		return {maxLeft, maxRight};
	}

	vector<vector<int>> insert(vector<vector<int>> &intervals, vector<int> &newInterval)
	{
		int left = newInterval[0];
		int right = newInterval[1];

		vector<vector<int>> ans;

		int begin = 0;
		while (begin < intervals.size() && intervals[begin][1] < left)
		{
			ans.emplace_back(intervals[begin]);
			++begin;
		}
		if (begin == intervals.size())
		{
			intervals.push_back({left, right});
			return intervals;
		}

		int end = begin;
		while (end < intervals.size() && intervals[end][0] <= right)
			++end;
		--end;

		// 此时[begin,end]就是intervals在插入newInterval之后需要合并的区间
		ans.emplace_back(func(intervals, begin, end));

		// 将剩余的区间加入数组
		for (int i = end + 1; i < intervals.size(); ++i)
		{
			ans.emplace_back(intervals[i]);
		}

		return ans;
	}
};

int main()
{
	Solution s;
	vector<vector<int>> intervals = {{1, 3}, {6, 9}};
	vector<int> newInterval = {2, 5};
	auto ans = s.insert(intervals, newInterval);
	for (auto &interval : ans)
	{
		cout << interval[0] << " " << interval[1] << endl;
	}
	return 0;
}