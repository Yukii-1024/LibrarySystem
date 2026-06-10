#pragma once
#include "Common.h"
#include <functional>

/**
 * 链队列模板类
 * 应用场景：热门图书预约排队（每本书对应一个预约队列）
 * 遵循先到先服务（FIFO）原则
 */
template <typename T>
class Queue {
public:
    struct Node {
        T data;
        Node* next = nullptr;
        Node(const T& d) : data(d) {}
    };

    Queue() = default;
    ~Queue() { clear(); }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Status enqueue(const T& value) {
        Node* node = new Node(value);
        if (rear) {
            rear->next = node;
        } else {
            front = node;
        }
        rear = node;
        ++length;
        return Status::OK;
    }

    Status dequeue(T& out) {
        if (!front) return Status::Empty;
        Node* temp = front;
        out = temp->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        --length;
        return Status::OK;
    }

    T* peek() const {
        if (!front) return nullptr;
        return &(front->data);
    }

    bool isEmpty() const { return front == nullptr; }
    int size() const { return length; }

    void clear() {
        while (front) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
        length = 0;
    }

    // 遍历（从队首到队尾，用于可视化）
    void traverse(const std::function<void(const T&)>& callback) const {
        Node* cur = front;
        while (cur) {
            callback(cur->data);
            cur = cur->next;
        }
    }

    // 获取节点指针（供可视化使用）
    Node* getFront() const { return front; }
    Node* getRear() const { return rear; }

private:
    Node* front = nullptr;
    Node* rear = nullptr;
    int length = 0;
};
