#include <unordered_map>
#include <chrono>
using namespace std;

// 实现思路：哈希表加双向链表
// 双向链表按照被使用的顺序存储键值对，靠近链表头部的键值对是最近使用的，而靠近尾部的键值对是最久未被使用的
// 哈希表用于快速访问链表中的节点，将访问时间复杂度从O(n)降低到O(1)

class LRUCache
{
private:
    struct node
    {
        int key;
        int val;
        std::chrono::steady_clock::time_point expire_time; // 节点的过期时间
        node *pre;
        node *next;
        
        node() : key(-1), val(-1), pre(nullptr), next(nullptr) {};
        node(int _key, int _val) : key(_key), val(_val), pre(nullptr), next(nullptr) {};
    };
    unordered_map<int, node *> index2node;
    int size;
    int capacity;
    long long ttl_ms; // 全局超时时间（毫秒）
    node *head;
    node *tail;

public:
    LRUCache(int _capacity, long long _ttl_ms = 5000) : capacity(_capacity), size(0), ttl_ms(_ttl_ms)
    {
        // 使用伪头部和伪尾部节点
        head = new node();
        tail = new node();
        head->next = tail;
        tail->pre = head;
        index2node.reserve(_capacity + 1);
    }

    // 提取一个删除节点的辅助方法
    void removeNode(node *target)
    {
        target->pre->next = target->next;
        target->next->pre = target->pre;
    }

    

    int get(int key)
    {
        if (index2node.count(key))
        {
            node *target = index2node[key];
            // 检查节点是否已超时
            auto now = std::chrono::steady_clock::now();
            if (now > target->expire_time)
            {
                // 超时则懒删除
                removeNode(target);
                index2node.erase(key);
                delete target;
                --size;
                return -1;
            }
            // 未超时，更新为最近使用（不续期）
            moveToHead(target);
            return target->val;
        }
        return -1;
    }

    void put(int key, int value)
    {
        auto now = std::chrono::steady_clock::now();
        auto new_expire = now + std::chrono::milliseconds(ttl_ms);

        if (!index2node.count(key))
        {
            node *temp = new node(key, value);
            temp->expire_time = new_expire; // 设置失效时间

            if (size == capacity)
            {
                // 可以优先淘汰已超时的节点，但为了实现简单，此处依然实行LRU替换末尾
                node *loser = tail->pre;
                removeNode(loser);
                index2node.erase(loser->key);
                delete loser;
                --size;
            }
            // 再加入新节点
            temp->next = head->next;
            temp->pre = head;
            head->next->pre = temp;
            head->next = temp;
            index2node.insert({key, temp});
            ++size;
            return;
        }
        // 如果已经存在, 将其移动到链表头部并修改值与超时时间
        node *target = index2node[key];
        target->val = value;
        target->expire_time = new_expire; // 刷新超时时间
        moveToHead(target);
        return;
    }

    void moveToHead(node *target)
    {
        removeNode(target);
        target->next = head->next;
        head->next->pre = target;
        target->pre = head;
        head->next = target;
        return;
    }
};