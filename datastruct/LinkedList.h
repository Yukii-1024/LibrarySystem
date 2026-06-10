#pragma once
#include "Common.h"
#include <functional>

/**
 * 双向链表模板类
 * 应用场景：图书基础列表、读者基础列表、借阅记录表
 * 支持：头插、尾插、按条件删除、遍历查询
 */
template <typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* prev = nullptr;
        Node* next = nullptr;
        Node(const T& d) : data(d) {}
    };

    LinkedList() = default;
    ~LinkedList() { clear(); }

    // 禁止拷贝（避免浅拷贝问题），允许移动
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;
    LinkedList(LinkedList&& other) noexcept { swap(other); }
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            swap(other);
        }
        return *this;
    }

    void swap(LinkedList& other) noexcept {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(length, other.length);
    }

    // 在尾部插入
    Status append(const T& value) {
        Node* node = new Node(value);
        if (!tail) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        ++length;
        return Status::OK;
    }

    // 在头部插入
    Status prepend(const T& value) {
        Node* node = new Node(value);
        if (!head) {
            head = tail = node;
        } else {
            node->next = head;
            head->prev = node;
            head = node;
        }
        ++length;
        return Status::OK;
    }

    // 删除第一个满足条件的节点（lambda返回true）
    Status removeIf(const std::function<bool(const T&)>& predicate) {
        Node* cur = head;
        while (cur) {
            if (predicate(cur->data)) {
                removeNode(cur);
                return Status::OK;
            }
            cur = cur->next;
        }
        return Status::NotFound;
    }

    // 查找第一个满足条件的节点，返回指针（nullptr表示未找到）
    T* findIf(const std::function<bool(const T&)>& predicate) const {
        Node* cur = head;
        while (cur) {
            if (predicate(cur->data)) return &(cur->data);
            cur = cur->next;
        }
        return nullptr;
    }

    // 遍历并执行操作（可用于可视化）
    void traverse(const std::function<void(T&)>& callback) {
        Node* cur = head;
        while (cur) {
            callback(cur->data);
            cur = cur->next;
        }
    }

    void traverse(const std::function<void(const T&)>& callback) const {
        Node* cur = head;
        while (cur) {
            callback(cur->data);
            cur = cur->next;
        }
    }

    // 获取节点指针（供可视化使用）
    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }

    int size() const { return length; }
    bool isEmpty() const { return length == 0; }

    void clear() {
        Node* cur = head;
        while (cur) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        length = 0;
    }

private:
    Node* head = nullptr;
    Node* tail = nullptr;
    int length = 0;

    void removeNode(Node* node) {
        if (!node) return;
        if (node->prev) node->prev->next = node->next;
        else head = node->next;
        if (node->next) node->next->prev = node->prev;
        else tail = node->prev;
        delete node;
        --length;
    }
};
