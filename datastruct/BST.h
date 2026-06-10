#pragma once
#include "Common.h"
#include <functional>
#include <vector>
#include <string>

/**
 * 二叉排序树 (Binary Search Tree) 模板类
 * 应用场景：按索书号快速检索图书、范围查询
 * 注意：节点存储的是指向实际数据的指针（或引用），树本身不拥有数据生命周期
 *
 * 后续可升级为 AVLTree，只需在插入/删除时加入平衡因子调整
 */
template <typename T, typename KeyFunc>
class BST {
public:
    struct Node {
        T* data = nullptr;          // 指向实际数据对象的指针
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;             // 预留：用于AVL升级

        Node(T* d) : data(d) {}
    };

    BST(KeyFunc keyExtractor) : getKey(keyExtractor) {}
    ~BST() { destroy(root); }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    // 插入：以 key 为索引插入 data 指针
    Status insert(T* data) {
        root = insertNode(root, data);
        ++nodeCount;
        return Status::OK;
    }

    // 删除：按 key 删除对应节点
    Status remove(const std::string& key) {
        if (!findNode(root, key)) return Status::NotFound;
        root = removeNode(root, key);
        --nodeCount;
        return Status::OK;
    }

    // 查找：返回数据指针
    T* find(const std::string& key) const {
        Node* n = findNode(root, key);
        return n ? n->data : nullptr;
    }

    // 范围查询：[low, high] 之间的所有数据
    void rangeQuery(const std::string& low, const std::string& high, std::vector<T*>& out) const {
        rangeQueryNode(root, low, high, out);
    }

    // 中序遍历（按键有序，用于可视化）
    void inOrder(const std::function<void(T*)>& callback) const {
        inOrderNode(root, callback);
    }

    // 获取根节点（供可视化遍历整棵树）
    Node* getRoot() const { return root; }
    int size() const { return nodeCount; }
    bool isEmpty() const { return root == nullptr; }

    // ---------- 以下为AVL升级预留 ----------
    // 获取节点平衡因子（AVL使用）
    int getBalance(Node* n) const {
        if (!n) return 0;
        return getHeight(n->left) - getHeight(n->right);
    }

    int getHeight(Node* n) const {
        return n ? n->height : 0;
    }

private:
    Node* root = nullptr;
    int nodeCount = 0;
    KeyFunc getKey;     // 函数对象：从 T* 提取出 std::string 类型的 key

    Node* findNode(Node* n, const std::string& key) const {
        if (!n) return nullptr;
        if (key == getKey(n->data)) return n;
        if (key < getKey(n->data)) return findNode(n->left, key);
        return findNode(n->right, key);
    }

    Node* insertNode(Node* n, T* data) {
        if (!n) return new Node(data);
        std::string key = getKey(data);
        std::string nodeKey = getKey(n->data);
        if (key < nodeKey)
            n->left = insertNode(n->left, data);
        else if (key > nodeKey)
            n->right = insertNode(n->right, data);
        else {
            // 相同key：更新数据指针（或按需求处理）
            n->data = data;
            return n;
        }
        // 预留AVL：更新高度并平衡
        n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
        return n; // 升级AVL时这里改为 return balance(n);
    }

    Node* removeNode(Node* n, const std::string& key) {
        if (!n) return nullptr;
        std::string nodeKey = getKey(n->data);
        if (key < nodeKey)
            n->left = removeNode(n->left, key);
        else if (key > nodeKey)
            n->right = removeNode(n->right, key);
        else {
            if (!n->left) {
                Node* temp = n->right;
                delete n;
                return temp;
            } else if (!n->right) {
                Node* temp = n->left;
                delete n;
                return temp;
            }
            Node* temp = findMin(n->right);
            n->data = temp->data;
            n->right = removeNode(n->right, getKey(temp->data));
        }
        n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
        return n; // 升级AVL时改为 return balance(n);
    }

    Node* findMin(Node* n) const {
        while (n && n->left) n = n->left;
        return n;
    }

    void inOrderNode(Node* n, const std::function<void(T*)>& callback) const {
        if (!n) return;
        inOrderNode(n->left, callback);
        callback(n->data);
        inOrderNode(n->right, callback);
    }

    void rangeQueryNode(Node* n, const std::string& low, const std::string& high, std::vector<T*>& out) const {
        if (!n) return;
        std::string key = getKey(n->data);
        if (low < key) rangeQueryNode(n->left, low, high, out);
        if (low <= key && key <= high) out.push_back(n->data);
        if (key < high) rangeQueryNode(n->right, low, high, out);
    }

    void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    // ---------- AVL旋转操作（预留） ----------
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
        return y;
    }

    Node* balance(Node* n) {
        int bal = getBalance(n);
        if (bal > 1 && getBalance(n->left) >= 0)
            return rotateRight(n);
        if (bal > 1 && getBalance(n->left) < 0) {
            n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (bal < -1 && getBalance(n->right) <= 0)
            return rotateLeft(n);
        if (bal < -1 && getBalance(n->right) > 0) {
            n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }
};
