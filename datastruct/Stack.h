#pragma once
#include "Common.h"
#include <vector>
#include <functional>

/**
 * 栈模板类（顺序栈实现，支持动态扩容）
 * 应用场景：借书/还书操作的撤销（Undo）
 * 存储操作类型和相关参数，便于回滚
 */
template <typename T>
class Stack {
public:
    Stack(int initCapacity = 64) : capacity(initCapacity) {
        if (capacity < 1) capacity = 1;
        data = new T[capacity];
    }

    ~Stack() { delete[] data; }

    // 禁止拷贝，允许移动
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    Stack(Stack&& other) noexcept {
        data = other.data;
        topIdx = other.topIdx;
        capacity = other.capacity;
        other.data = nullptr;
        other.topIdx = -1;
        other.capacity = 0;
    }

    Status push(const T& value) {
        if (topIdx + 1 >= capacity) {
            expand();
        }
        data[++topIdx] = value;
        return Status::OK;
    }

    Status pop(T& out) {
        if (isEmpty()) return Status::Empty;
        out = data[topIdx--];
        return Status::OK;
    }

    T* peek() const {
        if (isEmpty()) return nullptr;
        return &data[topIdx];
    }

    bool isEmpty() const { return topIdx < 0; }
    int size() const { return topIdx + 1; }

    void clear() { topIdx = -1; }

    // 遍历（从栈底到栈顶，用于可视化）
    void traverse(const std::function<void(const T&)>& callback) const {
        for (int i = 0; i <= topIdx; ++i) {
            callback(data[i]);
        }
    }

private:
    T* data = nullptr;
    int topIdx = -1;
    int capacity = 0;

    void expand() {
        int newCap = capacity * 2;
        T* newData = new T[newCap];
        for (int i = 0; i <= topIdx; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCap;
    }
};
