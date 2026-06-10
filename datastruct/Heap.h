#pragma once
#include "Common.h"
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

/**
 * 最大堆（优先队列）模板类
 * 应用场景：热门图书排行榜 Top K
 * 支持：插入、删除堆顶、更新键值、获取TopN
 *
 * T: 元素类型（如 Book*）
 * KeyFunc: 提取键值的函数对象（如借书次数），返回 int
 */
template <typename T, typename KeyFunc>
class MaxHeap {
public:
    explicit MaxHeap(KeyFunc keyFunc) : getKey(keyFunc) {}

    // 从已有数据构建堆（建堆算法 O(n)）
    void buildFrom(std::vector<T*>& items) {
        data.clear();
        indexMap.clear();
        data.reserve(items.size());
        for (T* item : items) {
            if (item) data.push_back(item);
        }
        int n = static_cast<int>(data.size());
        for (int i = (n / 2) - 1; i >= 0; --i) {
            heapifyDown(i);
        }
        rebuildIndex();
    }

    Status insert(T* item) {
        if (!item) return Status::Invalid;
        data.push_back(item);
        int idx = static_cast<int>(data.size()) - 1;
        indexMap[getId(item)] = idx;
        heapifyUp(idx);
        return Status::OK;
    }

    // 弹出最大值
    Status popTop(T*& out) {
        if (isEmpty()) return Status::Empty;
        out = data[0];
        indexMap.erase(getId(out));
        data[0] = data.back();
        data.pop_back();
        if (!isEmpty()) {
            indexMap[getId(data[0])] = 0;
            heapifyDown(0);
        }
        return Status::OK;
    }

    T* peekTop() const {
        return isEmpty() ? nullptr : data[0];
    }

    // 更新某个元素的键值（如借阅次数变了），然后调整堆
    Status updateKey(T* item) {
        if (!item) return Status::Invalid;
        std::string id = getId(item);
        auto it = indexMap.find(id);
        if (it == indexMap.end()) {
            // 不在堆中，直接插入
            return insert(item);
        }
        int idx = it->second;
        int oldKey = getKey(data[idx]);
        data[idx] = item;
        int newKey = getKey(item);
        if (newKey > oldKey) heapifyUp(idx);
        else if (newKey < oldKey) heapifyDown(idx);
        return Status::OK;
    }

    // 获取前N个元素（不删除，用于排行榜展示）
    std::vector<T*> getTopN(int n) const {
        std::vector<T*> copy = data;
        int k = std::min(n, static_cast<int>(copy.size()));
        std::nth_element(copy.begin(), copy.begin() + k, copy.end(),
            [this](T* a, T* b) { return getKey(a) > getKey(b); });
        copy.resize(k);
        std::sort(copy.begin(), copy.end(),
            [this](T* a, T* b) { return getKey(a) > getKey(b); });
        return copy;
    }

    // 堆化：从某个节点开始向上调整
    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (getKey(data[idx]) <= getKey(data[parent])) break;
            swapNodes(idx, parent);
            idx = parent;
        }
    }

    // 堆化：从某个节点开始向下调整
    void heapifyDown(int idx) {
        int n = static_cast<int>(data.size());
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int largest = idx;
            if (left < n && getKey(data[left]) > getKey(data[largest]))
                largest = left;
            if (right < n && getKey(data[right]) > getKey(data[largest]))
                largest = right;
            if (largest == idx) break;
            swapNodes(idx, largest);
            idx = largest;
        }
    }

    int size() const { return static_cast<int>(data.size()); }
    bool isEmpty() const { return data.empty(); }
    const std::vector<T*>& getData() const { return data; }

private:
    std::vector<T*> data;
    KeyFunc getKey;
    std::map<std::string, int> indexMap;   // item唯一ID -> 堆中索引，用于快速定位

    // 默认用元素内存地址转字符串作为ID（如果T没有唯一字段）
    // 但更好的方式是让调用者通过 KeyFunc 或其他方式提供ID
    // 这里我们做一个简化：要求 T 有一个 getIdentifier() 方法，或者由调用者保证
    // 实际上由于 template 的限制，我们用一个辅助函数，对 Book* 用 ISBN，对其他用指针地址

    std::string getId(T* item) const {
        // 这里使用模板特化策略比较困难，改为通过指针地址
        // 但更好的做法是在调用时保证 item 指针稳定，并且我们直接用指针值作为key
        // 因为同一本书的 Book* 地址是不变的
        return std::to_string(reinterpret_cast<uintptr_t>(item));
    }

    void swapNodes(int i, int j) {
        std::swap(data[i], data[j]);
        indexMap[getId(data[i])] = i;
        indexMap[getId(data[j])] = j;
    }

    void rebuildIndex() {
        indexMap.clear();
        for (int i = 0; i < static_cast<int>(data.size()); ++i) {
            indexMap[getId(data[i])] = i;
        }
    }
};
