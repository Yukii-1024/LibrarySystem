#include "MessageQueue.h"
#include "LibrarySystem.h"
#include "model/Reader.h"
#include "model/Book.h"
#include "model/Seat.h"
#include <QDateTime>

MessageQueueProcessor::MessageQueueProcessor(LibrarySystem* lib, QObject* parent)
    : QObject(parent), library(lib) {}

void MessageQueueProcessor::enqueue(OperationMessage* msg)
{
    if (!msg) return;
    msg->id = nextId++;
    msg->processed = false;
    messageQueue.enqueue(msg);
    emit queueChanged();
}

void MessageQueueProcessor::processAll()
{
    while (messageQueue.size() > 0) {
        processOne();
    }
}

bool MessageQueueProcessor::processOne()
{
    OperationMessage* msg = nullptr;
    if (messageQueue.dequeue(msg) != Status::OK || !msg)
        return false;

    executeMessage(msg);
    history.push_back(msg);
    // Keep history bounded
    if (history.size() > 200) {
        delete history.front();
        history.erase(history.begin());
    }
    emit queueChanged();
    return true;
}

void MessageQueueProcessor::clear()
{
    while (messageQueue.size() > 0) {
        OperationMessage* msg = nullptr;
        if (messageQueue.dequeue(msg) == Status::OK && msg)
            delete msg;
    }
    emit queueChanged();
}

int MessageQueueProcessor::pendingCount() const
{
    return messageQueue.size();
}

void MessageQueueProcessor::recordOperation(
    MsgType type, const QString& readerId, bool succeeded,
    const QString& resultText, const QString& bookISBN,
    int seatRow, int seatCol)
{
    auto* msg = new OperationMessage();
    msg->type = type;
    msg->readerId = readerId;
    msg->bookISBN = bookISBN;
    msg->seatRow = seatRow;
    msg->seatCol = seatCol;
    msg->succeeded = succeeded;
    msg->resultText = resultText;
    msg->processed = true;  // pre-resolved, skip execution in processAll
    enqueue(msg);
    processAll();  // immediately record to history
}

void MessageQueueProcessor::executeMessage(OperationMessage* msg)
{
    if (!msg || !library) return;

    // If already resolved (recorded from LibrarySystem), skip execution
    if (msg->processed) {
        history.push_back(msg);
        if (history.size() > 200) {
            delete history.front();
            history.erase(history.begin());
        }
        emit messageProcessed(msg->id, msg->succeeded, msg->resultText);
        return;
    }

    switch (msg->type) {
        case MsgType::BorrowBook: {
            Reader* r = library->findReader(msg->readerId);
            Book* b = library->findByISBN(msg->bookISBN);
            if (!r) {
                msg->succeeded = false;
                msg->resultText = QString::fromUtf8("借书失败：读者 %1 不存在").arg(msg->readerId);
            } else if (!b) {
                msg->succeeded = false;
                msg->resultText = QString::fromUtf8("借书失败：图书 %1 不存在").arg(msg->bookISBN);
            } else if (!r->canBorrow()) {
                msg->succeeded = false;
                msg->resultText = QString::fromUtf8("借书失败：读者 %1 已达最大借阅数").arg(msg->readerId);
            } else if (b->availableStock <= 0) {
                msg->succeeded = false;
                msg->resultText = QString::fromUtf8("借书失败：《%1》库存不足 (剩余%2)")
                    .arg(QString::fromStdString(b->title)).arg(b->availableStock);
            } else {
                Status s = library->borrowBook(msg->readerId, msg->bookISBN);
                msg->succeeded = (s == Status::OK);
                msg->resultText = msg->succeeded
                    ? QString::fromUtf8("✓ 借书成功：%1 借阅《%2》")
                        .arg(msg->readerId, QString::fromStdString(b->title))
                    : QString::fromUtf8("借书失败：系统错误");
            }
            break;
        }
        case MsgType::ReturnBook: {
            Status s = library->returnBook(msg->readerId, msg->bookISBN);
            msg->succeeded = (s == Status::OK);
            msg->resultText = msg->succeeded
                ? QString::fromUtf8("✓ 还书成功：%1 归还 %2").arg(msg->readerId, msg->bookISBN)
                : QString::fromUtf8("还书失败：未找到借阅记录");
            break;
        }
        case MsgType::ReserveSeat: {
            Reader* r = library->findReader(msg->readerId);
            if (!r) {
                msg->succeeded = false;
                msg->resultText = QString::fromUtf8("预约失败：读者 %1 不存在").arg(msg->readerId);
            } else {
                Seat* s = library->getSeat(msg->seatRow, msg->seatCol);
                if (s && !s->isFree()) {
                    msg->succeeded = false;
                    msg->resultText = QString::fromUtf8("预约失败：座位(%1,%2)已被 %3 占用")
                        .arg(msg->seatRow).arg(msg->seatCol)
                        .arg(QString::fromStdString(s->readerId));
                } else {
                    Status st = library->reserveSeat(msg->seatRow, msg->seatCol,
                        msg->readerId, msg->startTime, msg->endTime);
                    msg->succeeded = (st == Status::OK);
                    msg->resultText = msg->succeeded
                        ? QString::fromUtf8("✓ 预约成功：%1 预约座位(%2,%3)")
                            .arg(msg->readerId).arg(msg->seatRow).arg(msg->seatCol)
                        : QString::fromUtf8("预约失败：系统错误");
                }
            }
            break;
        }
        case MsgType::ReleaseSeat: {
            Seat* s = library->getSeat(msg->seatRow, msg->seatCol);
            if (!s || s->isFree()) {
                msg->succeeded = false;
                msg->resultText = QString::fromUtf8("释放失败：座位(%1,%2)未被占用")
                    .arg(msg->seatRow).arg(msg->seatCol);
            } else {
                Status st = library->releaseSeat(msg->seatRow, msg->seatCol);
                msg->succeeded = (st == Status::OK);
                msg->resultText = msg->succeeded
                    ? QString::fromUtf8("✓ 释放成功：座位(%1,%2)已释放")
                        .arg(msg->seatRow).arg(msg->seatCol)
                    : QString::fromUtf8("释放失败");
            }
            break;
        }
        case MsgType::AddBook: {
            // book data is passed via extra
            msg->succeeded = true;
            msg->resultText = QString::fromUtf8("✓ 添加图书：%1").arg(msg->extra);
            break;
        }
        case MsgType::AddReader: {
            msg->succeeded = true;
            msg->resultText = QString::fromUtf8("✓ 添加读者：%1").arg(msg->readerId);
            break;
        }
    }

    msg->processed = true;
    emit messageProcessed(msg->id, msg->succeeded, msg->resultText);
}
