#pragma once
#include "Common.h"
#include <functional>
#include <string>
#include <vector>

/**
 * 哈希表模板类（链地址法解决冲突）
 * 应用场景：读者登录验证（学号->读者）、按书名快速查找图书
 * Key: std::string（学号/书名），Value: T* 指针
 */
template <typename T, typename KeyFunc>
class HashTable {
public:
    struct HashNode {
        std::string key;
        T* value;
        HashNode* next = nullptr;
        HashNode(std::string k, T* v) : key(std::move(k)), value(v) {}
    };

    explicit HashTable(int initCapacity = 64, KeyFunc keyFunc = KeyFunc())
        : capacity(initCapacity), getKey(keyFunc) {
        if (capacity < 4) capacity = 4;
        table = new HashNode*[capacity]();
    }

    ~HashTable() {
        for (int i = 0; i < capacity; ++i) {
            HashNode* cur = table[i];
            while (cur) {
                HashNode* next = cur->next;
                delete cur;
                cur = next;
            }
        }
        delete[] table;
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // 插入/更新
    Status insert(T* value) {
        std::string key = getKey(value);
        int idx = hash(key);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == key) {
                cur->value = value; // 更新
                return Status::OK;
            }
            cur = cur->next;
        }
        // 头插法
        HashNode* node = new HashNode(key, value);
        node->next = table[idx];
        table[idx] = node;
        ++size_;
        if (size_ >= capacity * 0.75) resize();
        return Status::OK;
    }

    // 删除
    Status remove(const std::string& key) {
        int idx = hash(key);
        HashNode* cur = table[idx];
        HashNode* prev = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (prev) prev->next = cur->next;
                else table[idx] = cur->next;
                delete cur;
                --size_;
                return Status::OK;
            }
            prev = cur;
            cur = cur->next;
        }
        return Status::NotFound;
    }

    // 查找
    T* find(const std::string& key) const {
        int idx = hash(key);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == key) return cur->value;
            cur = cur->next;
        }
        return nullptr;
    }

    bool contains(const std::string& key) const {
        return find(key) != nullptr;
    }

    // 遍历所有元素（用于可视化、持久化）
    void traverse(const std::function<void(const std::string&, T*)>& callback) const {
        for (int i = 0; i < capacity; ++i) {
            HashNode* cur = table[i];
            while (cur) {
                callback(cur->key, cur->value);
                cur = cur->next;
            }
        }
    }

    // 获取某一下标桶的链表头（用于可视化）
    HashNode* getBucket(int index) const {
        if (index < 0 || index >= capacity) return nullptr;
        return table[index];
    }

    int getCapacity() const { return capacity; }
    int size() const { return size_; }
    bool isEmpty() const { return size_ == 0; }

private:
    HashNode** table = nullptr;
    int capacity = 0;
    int size_ = 0;
    KeyFunc getKey;

    // 简单字符串哈希（djb2）
    unsigned long hash(const std::string& key) const {
        unsigned long h = 5381;
        for (char c : key) {
            h = ((h << 5) + h) + static_cast<unsigned char>(c); // h * 33 + c
        }
        return h % capacity;
    }

    void resize() {
        int oldCap = capacity;
        capacity *= 2;
        HashNode** oldTable = table;
        table = new HashNode*[capacity]();
        size_ = 0;

        for (int i = 0; i < oldCap; ++i) {
            HashNode* cur = oldTable[i];
            while (cur) {
                // 重新插入
                int idx = hash(cur->key);
                HashNode* node = new HashNode(cur->key, cur->value);
                node->next = table[idx];
                table[idx] = node;
                ++size_;
                HashNode* toDel = cur;
                cur = cur->next;
                delete toDel;
            }
        }
        delete[] oldTable;
    }
};
