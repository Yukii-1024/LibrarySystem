#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include "model/Book.h"
#include "model/Reader.h"
#include "model/BorrowRecord.h"
#include "model/Seat.h"
#include "datastruct/LinkedList.h"
#include "datastruct/Stack.h"
#include "datastruct/Queue.h"
#include "datastruct/BST.h"
#include "datastruct/HashTable.h"
#include "datastruct/Graph.h"
#include "datastruct/Heap.h"
#include "datastruct/SparseMatrix.h"

class MessageQueueProcessor;
class OperationLogger;

// 操作记录结构（用于Undo栈）
struct OperationRecord {
    OperationType type = OperationType::None;
    QString readerId;
    QString bookISBN;
    QString extraData;
    QString timestamp;
};

/**
 * 图书馆系统核心类
 * 整合所有数据结构，提供业务接口
 * 同时通过 Qt 信号将操作传递给可视化面板
 */
class LibrarySystem : public QObject {
    Q_OBJECT

public:
    explicit LibrarySystem(QObject* parent = nullptr);
    ~LibrarySystem();

    // ---- 图书管理（BST + LinkedList + HashTable）----
    Status addBook(Book* book);
    Status removeBook(const QString& isbn);
    Status updateBook(const QString& isbn, const QString& newTitle,
                      const QString& newAuthor, const QString& newPublisher,
                      int newTotalStock);
    Book* findByISBN(const QString& isbn) const;
    Book* findByCallNumber(const QString& callNum) const;
    std::vector<Book*> rangeQuery(const QString& low, const QString& high) const;
    std::vector<Book*> getAllBooks() const;

    // ---- 读者管理（HashTable）----
    Status addReader(Reader* reader);
    Status removeReader(const QString& id);
    Status updateReader(const QString& id, const QString& newName,
                        const QString& newDept, const QString& newPwd = QString());
    Reader* findReader(const QString& id) const;
    bool verifyLogin(const QString& id, const QString& pwd, bool& isAdmin) const;

    // ---- 借阅与归还（Stack + Queue + LinkedList）----
    Status borrowBook(const QString& readerId, const QString& isbn);
    Status returnBook(const QString& readerId, const QString& isbn);
    Status undoLastOperation();
    bool canBorrow(const QString& readerId, const QString& isbn) const;
    Status enqueueReservation(const QString& readerId, const QString& isbn);

    // ---- 推荐系统（Graph）----
    std::vector<std::pair<QString, int>> recommendBooks(const QString& isbn, int topN = 5) const;

    // ---- 热门排行（Heap）----
    std::vector<Book*> getHotBooks(int topN = 10) const;

    // ---- 座位管理（SparseMatrix）----
    Status reserveSeat(int row, int col, const QString& readerId,
                       const QString& start, const QString& end);
    Status releaseSeat(int row, int col);
    Seat* getSeat(int row, int col) const;
    std::vector<std::vector<Seat>> getSeatGrid(int rows, int cols) const;

    // ---- 获取数据结构引用（供可视化层使用）----
    LinkedList<Book*>& getBookList() { return bookList; }
    LinkedList<Reader*>& getReaderList() { return readerList; }
    LinkedList<BorrowRecord*>& getBorrowRecords() { return borrowRecords; }
    Stack<OperationRecord>& getUndoStack() { return undoStack; }
    BST<Book, std::function<std::string(Book*)>>& getBookBST() { return *bookBST; }
    HashTable<Reader, std::function<std::string(Reader*)>>& getReaderHash() { return *readerHash; }
    HashTable<Book, std::function<std::string(Book*)>>& getBookTitleHash() { return *bookTitleHash; }
    HashTable<Book, std::function<std::string(Book*)>>& getBookISBNHash() { return *bookISBNHash; }
    Graph& getRecommendationGraph() { return recommendationGraph; }
    MaxHeap<Book, std::function<int(Book*)>>& getHotHeap() { return *hotHeap; }
    SparseMatrix<Seat>& getSeatMatrix() { return seatMatrix; }
    const std::map<std::string, Queue<QString>>& getReservationQueues() const { return reservationQueues; }
    std::map<std::string, Queue<QString>>& getReservationQueuesRef() { return reservationQueues; }
    void setNextRecordId(int id) { nextRecordId = id; }
    int getNextRecordId() const { return nextRecordId; }

    /// Recreate index structures (BST, HashTable, Heap) after clearing data.
    void resetStructures();

    /// 消息队列处理器（所有写操作经此序列化，保证并发安全）
    MessageQueueProcessor* getMessageQueue() { return messageQueue; }

    /// 操作日志记录器
    OperationLogger* getLogger() { return operationLogger; }

    /// 设置当前操作用户（供日志记录）
    void setCurrentOperator(const QString& id, bool admin) {
        currentOpId = id; currentOpAdmin = admin; }

signals:
    void operationPerformed(const QString& opName,
                            const QStringList& dsUsed,
                            const QString& description);

private:
    // 底层数据存储
    LinkedList<Book*> bookList;                 // 所有图书基础列表（链表）
    LinkedList<Reader*> readerList;             // 所有读者基础列表（链表）
    LinkedList<BorrowRecord*> borrowRecords;    // 借阅记录（链表）
    Stack<OperationRecord> undoStack;           // Undo操作历史（栈）

    // 索引结构
    BST<Book, std::function<std::string(Book*)>>* bookBST = nullptr;           // 索书号索引（BST）
    HashTable<Reader, std::function<std::string(Reader*)>>* readerHash = nullptr; // 读者索引（哈希表）
    HashTable<Book, std::function<std::string(Book*)>>* bookTitleHash = nullptr;  // 书名索引（哈希表）
    HashTable<Book, std::function<std::string(Book*)>>* bookISBNHash = nullptr;   // ISBN索引（哈希表）

    // 推荐与排行
    Graph recommendationGraph;                  // 图书推荐图（图）
    MaxHeap<Book, std::function<int(Book*)>>* hotHeap = nullptr;              // 热门排行堆（堆）

    // 座位
    SparseMatrix<Seat> seatMatrix;              // 座位分布（稀疏矩阵）

    // 预约队列：每本书对应一个队列（ISBN -> Queue）
    std::map<std::string, Queue<QString>> reservationQueues;

    // 消息队列处理器（多读者并发安全）
    MessageQueueProcessor* messageQueue = nullptr;

    // 操作日志记录器
    OperationLogger* operationLogger = nullptr;

    // 当前操作用户
    QString currentOpId;
    bool currentOpAdmin = false;

    int nextRecordId = 1;

    void emitOp(const QString& name, const QStringList& ds, const QString& desc);
};
