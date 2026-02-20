# unordered_map

## 概述

`unordered_map` 是 C++11 引入的关联容器，基于**哈希表**实现，存储键值对（key-value pair）。与 `map` 不同，`unordered_map` 不对元素进行排序，因此查找、插入和删除操作的平均时间复杂度为 **O(1)**。

### 头文件
```cpp
#include <unordered_map>
```

### 特点
- **无序性**：元素不按键排序
- **唯一键**：每个键只能出现一次
- **快速访问**：基于哈希表，平均 O(1) 时间复杂度
- **动态大小**：自动调整容量

---

## 构造函数

### 1. 默认构造
```cpp
std::unordered_map<int, std::string> umap;
```

### 2. 初始化列表构造
```cpp
std::unordered_map<int, std::string> umap = {
    {1, "one"},
    {2, "two"},
    {3, "three"}
};
```

### 3. 范围构造
```cpp
std::unordered_map<int, std::string> umap1 = {{1, "one"}, {2, "two"}};
std::unordered_map<int, std::string> umap2(umap1.begin(), umap1.end());
```

### 4. 拷贝构造
```cpp
std::unordered_map<int, std::string> umap1 = {{1, "one"}};
std::unordered_map<int, std::string> umap2(umap1);
```

---

## 常用方法

### 元素访问

#### `operator[]`
访问或插入元素，如果键不存在则创建。

```cpp
std::unordered_map<int, std::string> umap;
umap[1] = "one";           // 插入
std::string val = umap[1]; // 访问
// 注意：如果键不存在，会插入默认值
```

#### `at()`
访问指定键的值，如果键不存在则抛出 `std::out_of_range` 异常。

```cpp
std::unordered_map<int, std::string> umap = {{1, "one"}};
std::string val = umap.at(1);  // 返回 "one"
// umap.at(2);  // 抛出异常
```

---

### 容量相关

#### `empty()`
检查容器是否为空。

```cpp
std::unordered_map<int, std::string> umap;
if (umap.empty()) {
    std::cout << "Map is empty" << std::endl;
}
```

#### `size()`
返回容器中元素的数量。

```cpp
std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}};
std::cout << "Size: " << umap.size() << std::endl;  // 输出: Size: 2
```

#### `max_size()`
返回容器可容纳的最大元素数量。

```cpp
std::cout << "Max size: " << umap.max_size() << std::endl;
```

---

### 修改操作

#### `insert()`
插入元素，返回 `pair<iterator, bool>`，其中 `bool` 表示是否插入成功。

```cpp
std::unordered_map<int, std::string> umap;

// 插入单个元素
auto result = umap.insert({1, "one"});
if (result.second) {
    std::cout << "Inserted successfully" << std::endl;
}

// 插入多个元素
umap.insert({{2, "two"}, {3, "three"}});

// 使用 make_pair
umap.insert(std::make_pair(4, "four"));
```

#### `emplace()`
原位构造元素，比 `insert` 更高效（避免拷贝）。

```cpp
std::unordered_map<int, std::string> umap;
umap.emplace(1, "one");
umap.emplace(std::piecewise_construct, 
             std::forward_as_tuple(2), 
             std::forward_as_tuple("two"));
```

#### `emplace_hint()`
带位置提示的原位构造。

```cpp
auto it = umap.begin();
umap.emplace_hint(it, 3, "three");
```

#### `erase()`
删除元素。

```cpp
std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}, {3, "three"}};

// 通过键删除
umap.erase(1);

// 通过迭代器删除
auto it = umap.find(2);
if (it != umap.end()) {
    umap.erase(it);
}

// 删除范围
umap.erase(umap.begin(), umap.end());
```

#### `clear()`
清空所有元素。

```cpp
umap.clear();
std::cout << "Size after clear: " << umap.size() << std::endl;  // 输出: 0
```

#### `swap()`
交换两个 `unordered_map` 的内容。

```cpp
std::unordered_map<int, std::string> umap1 = {{1, "one"}};
std::unordered_map<int, std::string> umap2 = {{2, "two"}};
umap1.swap(umap2);
```

---

### 查找操作

#### `find()`
查找指定键，返回迭代器。如果未找到，返回 `end()`。

```cpp
std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}};
auto it = umap.find(1);
if (it != umap.end()) {
    std::cout << "Found: " << it->second << std::endl;
} else {
    std::cout << "Not found" << std::endl;
}
```

#### `count()`
返回指定键的元素数量（对于 `unordered_map` 只能是 0 或 1）。

```cpp
if (umap.count(1)) {
    std::cout << "Key 1 exists" << std::endl;
}
```

#### `contains()` (C++20)
检查是否包含指定键。

```cpp
if (umap.contains(1)) {
    std::cout << "Key 1 exists" << std::endl;
}
```

#### `equal_range()`
返回键等于给定值的元素范围。

```cpp
auto range = umap.equal_range(1);
for (auto it = range.first; it != range.second; ++it) {
    std::cout << it->first << ": " << it->second << std::endl;
}
```

---

### 迭代器

#### `begin()` / `end()`
返回指向容器首元素/尾后位置的迭代器。

```cpp
std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}};
for (auto it = umap.begin(); it != umap.end(); ++it) {
    std::cout << it->first << ": " << it->second << std::endl;
}
```

#### `cbegin()` / `cend()`
返回常量迭代器。

```cpp
for (auto it = umap.cbegin(); it != umap.cend(); ++it) {
    std::cout << it->first << ": " << it->second << std::endl;
}
```

#### 范围 for 循环
```cpp
for (const auto& pair : umap) {
    std::cout << pair.first << ": " << pair.second << std::endl;
}

// C++17 结构化绑定
for (const auto& [key, value] : umap) {
    std::cout << key << ": " << value << std::endl;
}
```

---

### 哈希策略

#### `bucket_count()`
返回桶的数量。

```cpp
std::cout << "Bucket count: " << umap.bucket_count() << std::endl;
```

#### `max_bucket_count()`
返回最大桶数量。

```cpp
std::cout << "Max bucket count: " << umap.max_bucket_count() << std::endl;
```

#### `bucket_size(n)`
返回第 n 个桶中的元素数量。

```cpp
for (size_t i = 0; i < umap.bucket_count(); ++i) {
    std::cout << "Bucket " << i << " size: " << umap.bucket_size(i) << std::endl;
}
```

#### `bucket(key)`
返回键所在的桶索引。

```cpp
std::cout << "Key 1 is in bucket: " << umap.bucket(1) << std::endl;
```

#### `load_factor()`
返回当前负载因子（元素数量 / 桶数量）。

```cpp
std::cout << "Load factor: " << umap.load_factor() << std::endl;
```

#### `max_load_factor()`
获取或设置最大负载因子。

```cpp
std::cout << "Max load factor: " << umap.max_load_factor() << std::endl;
umap.max_load_factor(0.8);  // 设置最大负载因子
```

#### `rehash(n)`
将桶数量设置为至少 n。

```cpp
umap.rehash(20);  // 至少 20 个桶
```

#### `reserve(n)`
预留空间以容纳至少 n 个元素。

```cpp
umap.reserve(100);  // 预留至少可容纳 100 个元素的空间
```

---

## 自定义哈希函数

对于自定义类型，需要提供哈希函数和相等比较函数。

### 方法 1：特化 `std::hash`

**适用场景**：你的类型在多个容器/库中都会作为键使用，希望统一默认哈希；或希望用户写 `unordered_map<Person, T>` 时不必显式传哈希类型。

**关键要点**：
- 只能对“用户自定义类型”做特化，不能为标准库类型（如 `std::string`）改写或重载。
- 放在 `namespace std` 内，并保持与 `operator==` 一致：相等对象必须产生相同哈希值。
- 建议定义为 `const`、`noexcept`，并满足 `size_t operator()(const T&)` 形式。
- 若类型在头文件中定义，特化也应放在同一头文件，避免 ODR 问题。

**优点**：
- 使用更简洁，容器类型不需要额外模板参数。
- 一次定义，多处复用，API 更统一。

**缺点**：
- 侵入性更强：在 `std` 命名空间内扩展，维护需谨慎。
- 不同项目若需要不同哈希策略，难以共存。

```cpp
struct Person {
    std::string name;
    int age;
    
    bool operator==(const Person& other) const {
        return name == other.name && age == other.age;
    }
};

// 特化 std::hash
namespace std {
    template <>
    struct hash<Person> {
        size_t operator()(const Person& p) const {
            return hash<string>()(p.name) ^ (hash<int>()(p.age) << 1);
        }
    };
}

std::unordered_map<Person, std::string> personMap;
```

### 方法 2：自定义哈希函数对象

**适用场景**：你只想在特定容器或特定场景下使用某个哈希策略；或希望为同一类型提供多个可切换的哈希实现。

**关键要点**：
- 与 `PersonEqual` 配套使用，二者的逻辑必须一致（相等即同哈希）。
- 可以带状态（如盐值、哈希种子），用于抗碰撞或不同策略实验。
- 适合做“局部策略”，不会影响全局默认行为。

**优点**：
- 灵活可控，可为同一类型提供多个哈希版本。
- 不修改 `std` 命名空间，侵入性小。

**缺点**：
- 使用时模板参数更冗长，可读性略差。
- 多处使用需重复写模板参数，维护成本高一些。

```cpp
struct PersonHash {
    size_t operator()(const Person& p) const {
        return std::hash<std::string>()(p.name) ^ (std::hash<int>()(p.age) << 1);
    }
};

struct PersonEqual {
    bool operator()(const Person& lhs, const Person& rhs) const {
        return lhs.name == rhs.name && lhs.age == rhs.age;
    }
};

std::unordered_map<Person, std::string, PersonHash, PersonEqual> personMap;
```

---

## 性能对比：unordered_map vs map

| 特性 | unordered_map | map |
|------|---------------|-----|
| 底层实现 | 哈希表 | 红黑树 |
| 元素顺序 | 无序 | 有序（按键排序） |
| 查找时间 | 平均 O(1)，最坏 O(n) | O(log n) |
| 插入时间 | 平均 O(1)，最坏 O(n) | O(log n) |
| 删除时间 | 平均 O(1)，最坏 O(n) | O(log n) |
| 内存占用 | 较大（需要额外桶空间） | 较小 |
| 适用场景 | 快速查找、不需要排序 | 需要有序遍历 |

---

## 使用示例

### 词频统计

```cpp
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>

int main() {
    std::string text = "hello world hello cpp world";
    std::unordered_map<std::string, int> wordCount;
    
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        wordCount[word]++;
    }
    
    for (const auto& [word, count] : wordCount) {
        std::cout << word << ": " << count << std::endl;
    }
    
    return 0;
}
```

### 两数之和问题

```cpp
#include <vector>
#include <unordered_map>

std::vector<int> twoSum(std::vector<int>& nums, int target) {
    std::unordered_map<int, int> numMap;
    
    for (int i = 0; i < nums.size(); ++i) {
        int complement = target - nums[i];
        if (numMap.find(complement) != numMap.end()) {
            return {numMap[complement], i};
        }
        numMap[nums[i]] = i;
    }
    
    return {};
}
```

### 缓存实现（简单版）

```cpp
#include <unordered_map>
#include <string>

class Cache {
private:
    std::unordered_map<std::string, std::string> cache;
    
public:
    void put(const std::string& key, const std::string& value) {
        cache[key] = value;
    }
    
    std::string get(const std::string& key) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
        return "";  // 未找到
    }
    
    bool contains(const std::string& key) {
        return cache.find(key) != cache.end();
    }
};
```

---

## 注意事项

1. **哈希冲突**：当多个键映射到同一个桶时，性能会下降到 O(n)
2. **迭代器失效**：
   - `insert` 可能导致 rehash，使所有迭代器失效
   - `erase` 只使被删除元素的迭代器失效
3. **键的不可变性**：插入后不应修改键的值，否则哈希值会不一致
4. **无序性**：遍历顺序不确定，每次可能不同
5. **自定义类型**：需要提供哈希函数和相等比较函数

---

## 总结

`unordered_map` 是 C++ 中非常高效的关联容器，适用于需要快速查找而不关心元素顺序的场景。合理使用可以大幅提升程序性能。

**优点**：
- 快速查找、插入、删除（平均 O(1)）
- 适合大数据量场景

**缺点**：
- 无序，不支持范围查询
- 内存占用较大
- 最坏情况性能退化
