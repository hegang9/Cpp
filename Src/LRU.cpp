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
        node *pre;
        node *next;
        node() : key(-1), val(-1), pre(nullptr), next(nullptr) {};
        node(int _key, int _val) : key(_key), val(_val), pre(nullptr), next(nullptr) {};
    };
    unordered_map<int, node *> index2node;
    int size;
    int capacity;
    node *head;
    node *tail;

public:
    LRUCache(int _capacity) : capacity(_capacity), size(0)
    {
        // 使用伪头部和伪尾部节点
        head = new node();
        tail = new node();
        head->next = tail;
        tail->pre = head;
        index2node.reserve(_capacity + 1);
    }

    int get(int key)
    {
        if (index2node.count(key))
        {
            node *target = index2node[key];
            moveToHead(target);
            return target->val;
        }
        return -1;
    }

    void put(int key, int value)
    {
        if (!index2node.count(key))
        {
            node *temp = new node(key, value);
            if (size == capacity)
            {
                // 缓存此刻是满的，需要将链表最后一个有效节点替换出去
                node *loser = tail->pre;
                int loser_key = loser->key;
                loser->pre->next = tail;
                tail->pre = loser->pre;
                index2node.erase(loser_key);
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
        // 如果已经存在,将其移动到链表头部并修改其值
        node *target = index2node[key];
        moveToHead(target);
        target->val = value;
        return;
    }

    void moveToHead(node *target)
    {
        target->pre->next = target->next;
        target->next->pre = target->pre;
        target->next = head->next;
        head->next->pre = target;
        target->pre = head;
        head->next = target;
        return;
    }
};