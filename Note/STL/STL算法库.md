## STL算法库
STL算法库提供了大量的算法函数，可以对容器中的元素进行各种操作，如排序、查找、变换等。这些算法函数通常以迭代器为参数，可以与STL容器无缝配合使用。
### 常用算法函数
1. **排序算法**：
   - `std::sort`: 对范围内的元素进行排序。
   - `std::stable_sort`: 保持相等元素的相对位置进行排序。
   - `std::partial_sort`: 对范围内的前N个元素进行排序。
   - `std::nth_element`: 将第N个元素放到正确的位置，前面的元素都不大于它，后面的元素都不小于它。
   - `std::sort_heap`: 对堆进行排序。
   - `std::stable_partition`: 根据条件将元素分区，保持相等元素的相对位置。
   - `std::partition`: 根据条件将元素分区。
   - `std::is_partitioned`: 检查范围内的元素是否已经根据条件分区。
2. **查找算法**：
   - `std::find`: 在范围内查找第一个等于给定值的元素。
   - `std::find_if`: 在范围内查找第一个满足条件的元素。
   - `std::find_if_not`: 在范围内查找第一个不满足条件的元素。
   - `std::binary_search`: 在已排序的范围内检查是否存在某个值。
   - `std::lower_bound`: 在已排序的范围内找到第一个不小于给定值的位置。
   - `std::upper_bound`: 在已排序的范围内找到第一个大于给定值的位置。
   - `std::equal_range`: 在已排序的范围内找到等于给定值的元素范围。
    - `std::search`: 在范围内查找子范围。
    - `std::search_n`: 在范围内查找连续N个满足条件的元素。
3. **变换算法**：
   - `std::transform`: 对范围内的元素进行变换，结果存储在另一个范围内。
   - `std::replace`: 将范围内的某个值替换为另一个值。
   - `std::replace_if`: 将范围内满足条件的元素替换为另一个值。
   - `std::replace_copy`: 将范围内的某个值替换为另一个值，并将结果存储在另一个范围内。
   - `std::replace_copy_if`: 将范围内满足条件的元素替换为另一个值，并将结果存储在另一个范围内。
   - `std::fill`: 将范围内的元素填充为某个值。
   - `std::fill_n`: 将范围内的前N个元素填充为某个值。
   - `std::generate`: 使用生成器函数填充范围内的元素。
   - `std::generate_n`: 使用生成器函数填充范围内的前N个元素。
4. **其他算法**：
   - `std::for_each`: 对范围内的每个元素执行一个函数。
   - `std::count`: 统计范围内等于某个值的元素数量。
   - `std::count_if`: 统计范围内满足条件的元素数量。
   - `std::accumulate`: 对范围内的元素进行累加。
   - `std::inner_product`: 计算两个范围内元素的内积。
   - `std::adjacent_difference`: 计算范围内相邻元素之间的差异。
   - `std::partial_sum`: 计算范围内元素的部分和。
   - `std::iota`: 将范围内的元素填充为递增的值。