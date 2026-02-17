#include <iostream>
#include <vector>
using namespace std;

class Solution
{
public:
	vector<int> spiralOrder(vector<vector<int>> &matrix)
	{
		int m = matrix.size();
		int n = matrix[0].size();
		vector<vector<int>> already(m, vector<int>(n, 0));
		vector<vector<int>> direction{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

		vector<int> ans;

		int row = 0, column = 0;
		int num = 0;	// 用于记录遍历的元素个数，用于结束遍历循环
		int dindex = 0; // 用于遍历direction数组

		while (num < m * n)
		{
			ans.push_back(matrix[row][column]);
			already[row][column] = 1;
			int next_row = row + direction[dindex][0];
			int next_column = column + direction[dindex][1];
			if (next_row == m || next_column == n || already[next_row][next_column] == 1)
			{
				// 如果到达边界，那么就要改变方向
				dindex = (dindex + 1) % 4;
				next_row = row + direction[dindex][0];
				next_column = column + direction[dindex][1];
			}
			row = next_row;
			column = next_column;
			++num;
		}

		return ans;
	}
};

int main()
{
	Solution s;
	vector<vector<int>> matrix{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
	vector<int> ans = s.spiralOrder(matrix);
	for (int i : ans)
	{
		cout << i << " ";
	}
	cout << endl;
	return 0;
}