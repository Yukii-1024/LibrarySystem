#pragma once

/**
 * 数据结构通用定义与状态码
 */

// 操作状态码
enum class Status {
    OK = 0,             // 成功
    Error = 1,          // 通用错误
    NotFound = 2,       // 未找到
    Duplicate = 3,      // 重复
    Overflow = 4,       // 溢出
    Underflow = 5,      // 下溢
    Invalid = 6,        // 无效参数
    Unauthorized = 7,   // 未授权
    Full = 8,           // 已满
    Empty = 9           // 为空
};

// 操作类型（用于Undo栈和可视化）
enum class OperationType {
    None = 0,
    AddBook,
    RemoveBook,
    UpdateBook,
    AddReader,
    RemoveReader,
    UpdateReader,
    BorrowBook,
    ReturnBook,
    ReserveSeat,
    ReleaseSeat,
    EnqueueReservation,
    DequeueReservation
};

// 数据结构类型（用于可视化面板标识）
enum class DSType {
    LinkedList = 0,
    Stack,
    Queue,
    BST,
    AVLTree,
    HashTable,
    Graph,
    Heap,
    SparseMatrix
};

// 辅助函数：获取数据结构名称（用于UI显示）
inline const char* dsTypeName(DSType type) {
    switch (type) {
        case DSType::LinkedList: return "链表";
        case DSType::Stack: return "栈";
        case DSType::Queue: return "队列";
        case DSType::BST: return "二叉排序树";
        case DSType::AVLTree: return "AVL树";
        case DSType::HashTable: return "哈希表";
        case DSType::Graph: return "图";
        case DSType::Heap: return "堆";
        case DSType::SparseMatrix: return "稀疏矩阵";
        default: return "未知";
    }
}
