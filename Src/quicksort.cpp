#include <vector>
using namespace std;

// 手撕快排
void quickSort(vector<int> &nums, int left, int right)
{
    if (left >= right)
        return; // 递归基：区间内元素小于等于1个则终止

    // 三数取中法：只需最多3次比较即可找到中位数，并将其放到 left 的位置
    int mid = left + (right - left) / 2;
    if (nums[left] > nums[mid])
        swap(nums[left], nums[mid]);
    if (nums[left] > nums[right])
        swap(nums[left], nums[right]);
    if (nums[mid] > nums[right])
        swap(nums[mid], nums[right]);

    // 此时必定满足: nums[left] <= nums[mid] <= nums[right]
    // nums[mid] 即为中位数。我们将它和 left 交换，这样就不需要修改后面“挖坑法”的逻辑了
    swap(nums[mid], nums[left]);

    int target = nums[left]; // 此时基准已经是三数的中位数了
    int i = left, j = right;
    while (i < j)
    {
        while (i < j && nums[j] >= target)
            j--; // 从右往左找第一个小于基准的数，从右往左填坑
        if (i < j)
            nums[i++] = nums[j]; // 将这个数放到左边

        while (i < j && nums[i] <= target)
            i++; // 从左往右找第一个大于基准的数，从左往右填坑
        if (i < j)
            nums[j--] = nums[i]; // 将这个数放到右边
    }
    nums[i] = target; // 将基准放到正确的位置

    // 对基准值左右两边的子区间分别进行快排，注意不要再包含基准值 i
    quickSort(nums, left, i - 1);
    quickSort(nums, i + 1, right);
}