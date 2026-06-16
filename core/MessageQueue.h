#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include "datastruct/Queue.h"

class LibrarySystem;

/**
 * 操作消息类型
 */
enum class MsgType {
    BorrowBook,     // 借书
    ReturnBook,     // 还书
    ReserveSeat,    // 预约座位
    ReleaseSeat,    // 释放座位
    AddBook,        // 添加图书
    AddReader       // 添加读者
};

inline QString msgTypeName(MsgType t) {
    switch (t) {
        case MsgType::BorrowBook:  return QString::fromUtf8("借书");
        case MsgType::ReturnBook:  return QString::fromUtf8("还书");
        case MsgType::ReserveSeat: return QString::fromUtf8("预约座位");
        case MsgType::ReleaseSeat: return QString::fromUtf8("释放座位");
        case MsgType::AddBook:     return QString::fromUtf8("添加图书");
        case MsgType::AddReader:   return QString::fromUtf8("添加读者");
        default: return QString::fromUtf8("未知");
    }
}

/**
 * 操作消息 — 多读者操作的统一消息格式
 * 所有写操作经过消息队列序列化处理，保证并发安全
 */
struct OperationMessage {
    int id = 0;
    MsgType type = MsgType::BorrowBook;
    QString readerId;
    QString bookISBN;
    int seatRow = 0;
    int seatCol = 0;
    QString startTime;
    QString endTime;
    QString extra;
    QString resultText;
    bool succeeded = false;
    bool processed = false;
};

/**
 * 消息队列处理器
 * 内部使用 Queue<OperationMessage*> 链队列（FIFO）
 * 所有对图书馆系统的写操作先入队，再逐条处理，
 * 以此保证多读者并发操作时的数据一致性与操作顺序。
 */
class MessageQueueProcessor : public QObject {
    Q_OBJECT

public:
    explicit MessageQueueProcessor(LibrarySystem* lib, QObject* parent = nullptr);

    /// 入队一条操作消息
    void enqueue(OperationMessage* msg);

    /// 立即处理队列中所有消息
    void processAll();

    /// 逐条处理一条消息
    bool processOne();

    /// 清空队列
    void clear();

    /// 待处理数量
    int pendingCount() const;

    /// 获取底层队列引用（供可视化使用）
    Queue<OperationMessage*>& getQueue() { return messageQueue; }

    /// 快捷方法：记录一条已完成的操作（供 LibrarySystem 内部调用）
    void recordOperation(MsgType type, const QString& readerId,
                         bool succeeded, const QString& resultText,
                         const QString& bookISBN = QString(),
                         int seatRow = 0, int seatCol = 0);

    /// 获取处理历史（最近 N 条，供可视化面板使用）
    const std::vector<OperationMessage*>& getHistory() const { return history; }

signals:
    void messageProcessed(int id, bool success, const QString& result);
    void queueChanged();

private:
    void executeMessage(OperationMessage* msg);

    LibrarySystem* library;
    Queue<OperationMessage*> messageQueue;
    std::vector<OperationMessage*> history;  // 处理历史
    int nextId = 1;
};
