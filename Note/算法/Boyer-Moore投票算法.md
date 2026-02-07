## "出现次数超过n/k"问题
该种问题有多种解法，它们分别在不同的场景中有使用，场景类别包含以下几种：
1. 内存敏感场景
2. 通用离线场景
3. k值极小的特殊场景（多数投票）

### Boyer-Moore投票算法
- 适用场景：适用于**内存敏感场景**和**k值极小的特殊场景（多数投票,通常k≤5）**。
  - 流数据处理中的频繁项挖掘
  - 大规模日志分析中的热门关键词提取
  - 社交网络中的流行话题发现
  - 系统监控中的异常检测
  - 大规模分布式：需要减少数据传输
  - 实时系统：需要O(1)内存

- 算法思想：利用相互抵消的原理：将多数元素与其他元素一一抵消，由于多数元素的数量超过其他元素总和，最后剩下的必然是多数元素。
- 算法思路：
  - 初始化 k-1 个候选元素 candidate 和 k-1 个计数器 count。
  - 遍历数组中的每个元素 num：
    - 如果 count == 0，将 candidate设为当前元素 num
    - 如果 num == candidate，count++
    - 否则 count--
- 遍历结束后，candidate就是可能的多数元素
- 如果需要验证（题目未保证一定存在多数元素），可以再次遍历数组统计 candidate的出现次数
- Boyer-Moore优势：O(k)空间，O(nk)时间，单次遍历
- 示例代码：
```c++
vector<int> majorityKElement(vector<int>& nums, int k) {
    int n = nums.size();
    vector<int> result;
    
    if (n == 0 || k <= 1) return result;
    
    // 最多有 k-1 个候选
    vector<int> candidates(k-1, 0);
    vector<int> counts(k-1, 0);
    
    // 阶段1：寻找候选
    for (int num : nums) {
        bool found = false;
        
        // 如果 num 已经是候选
        for (int i = 0; i < k-1; i++) {
            if (counts[i] > 0 && num == candidates[i]) {
                counts[i]++;
                found = true;
                break;
            }
        }
        
        if (found) continue;
        
        // 如果有空槽
        for (int i = 0; i < k-1; i++) {
            if (counts[i] == 0) {
                candidates[i] = num;
                counts[i] = 1;
                found = true;
                break;
            }
        }
        
        if (found) continue;
        
        // 否则，所有候选计数器减1
        for (int i = 0; i < k-1; i++) {
            counts[i]--;
        }
    }
    
    // 阶段2：验证候选
    vector<int> finalCounts(k-1, 0);
    
    for (int num : nums) {
        for (int i = 0; i < k-1; i++) {
            if (counts[i] > 0 && num == candidates[i]) {
                finalCounts[i]++;
                break;
            }
        }
    }
    
    for (int i = 0; i < k-1; i++) {
        if (finalCounts[i] > n / k) {
            result.push_back(candidates[i]);
        }
    }
    
    return result;
}
```

### 哈希表解法
- 适用场景：内存充足，需要简单可靠解
- 示例代码：
```c++
vector<int> majorityKHash(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    vector<int> result;
    
    for (int num : nums) freq[num]++;
    
    for (auto& [num, count] : freq) {
        if (count > nums.size() / k) {
            result.push_back(num);
        }
    }
    
    return result;
}
```