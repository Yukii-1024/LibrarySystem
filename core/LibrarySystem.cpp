#include "LibrarySystem.h"
#include <QDateTime>

LibrarySystem::LibrarySystem(QObject* parent)
    : QObject(parent)
    , bookBST(new BST<Book, std::function<std::string(Book*)>>(
          [](Book* b) { return b ? b->callNumber : ""; }))
    , readerHash(new HashTable<Reader, std::function<std::string(Reader*)>>(
          64, [](Reader* r) { return r ? r->id : ""; }))
    , bookTitleHash(new HashTable<Book, std::function<std::string(Book*)>>(
          64, [](Book* b) { return b ? b->title : ""; }))
    , hotHeap(new MaxHeap<Book, std::function<int(Book*)>>(
          [](Book* b) { return b ? b->borrowCount : 0; }))
{
}

LibrarySystem::~LibrarySystem()
{
    // 清理链表中的指针
    bookList.traverse([](Book* b) { delete b; });
    readerList.traverse([](Reader* r) { delete r; });
    borrowRecords.traverse([](BorrowRecord* br) { delete br; });
    delete bookBST;
    delete readerHash;
    delete bookTitleHash;
    delete hotHeap;
}

// ---- 图书管理 ----
Status LibrarySystem::addBook(Book* book)
{
    if (!book) return Status::Invalid;
    bookList.append(book);
    bookBST->insert(book);
    bookTitleHash->insert(book);
    hotHeap->insert(book);
    recommendationGraph.addVertex(book->isbn);

    emitOp("添加图书", {"链表", "二叉排序树", "哈希表", "图", "堆"},
           QString("将《%1》加入图书链表，索书号%2插入BST，书名加入哈希表，顶点加入推荐图，堆初始化借阅次数为0")
               .arg(QString::fromStdString(book->title))
               .arg(QString::fromStdString(book->callNumber)));
    return Status::OK;
}

Status LibrarySystem::removeBook(const QString& isbn)
{
    std::string sIsbn = isbn.toStdString();
    Book* target = findByISBN(isbn);
    if (!target) return Status::NotFound;

    bookList.removeIf([&](Book* b) { return b->isbn == sIsbn; });
    bookBST->remove(target->callNumber);
    bookTitleHash->remove(target->title);
    recommendationGraph.removeVertex(sIsbn);
    // 注意：hotHeap不支持直接删除，这里简化处理

    delete target;
    emitOp("删除图书", {"链表", "二叉排序树", "哈希表", "图"},
           "从各数据结构中移除图书信息");
    return Status::OK;
}

Book* LibrarySystem::findByISBN(const QString& isbn) const
{
    std::string s = isbn.toStdString();
    Book* result = nullptr;
    bookList.traverse([&](Book* b) {
        if (b->isbn == s) result = b;
    });
    return result;
}

Book* LibrarySystem::findByCallNumber(const QString& callNum) const
{
    return bookBST->find(callNum.toStdString());
}

std::vector<Book*> LibrarySystem::rangeQuery(const QString& low, const QString& high) const
{
    std::vector<Book*> result;
    bookBST->rangeQuery(low.toStdString(), high.toStdString(), result);
    return result;
}

std::vector<Book*> LibrarySystem::getAllBooks() const
{
    std::vector<Book*> result;
    bookList.traverse([&](Book* b) { result.push_back(b); });
    return result;
}

// ---- 读者管理 ----
Status LibrarySystem::addReader(Reader* reader)
{
    if (!reader) return Status::Invalid;
    readerList.append(reader);
    readerHash->insert(reader);

    emitOp("添加读者", {"链表", "哈希表"},
           QString("读者 %1 加入读者链表，学号映射到哈希表")
               .arg(QString::fromStdString(reader->name)));
    return Status::OK;
}

Status LibrarySystem::removeReader(const QString& id)
{
    std::string sid = id.toStdString();
    Reader* r = findReader(id);
    if (!r) return Status::NotFound;
    if (r->currentBorrow > 0) return Status::Invalid; // 有书未还不能注销

    readerList.removeIf([&](Reader* reader) { return reader->id == sid; });
    readerHash->remove(sid);
    delete r;

    emitOp("注销读者", {"链表", "哈希表"}, "从各结构中移除读者");
    return Status::OK;
}

Reader* LibrarySystem::findReader(const QString& id) const
{
    return readerHash->find(id.toStdString());
}

bool LibrarySystem::verifyLogin(const QString& id, const QString& pwd, bool& isAdmin) const
{
    Reader* r = findReader(id);
    if (!r || !r->active) return false;
    if (!r->hasPassword(pwd.toStdString())) return false;
    isAdmin = r->isAdmin;
    return true;
}

// ---- 借阅与归还 ----
Status LibrarySystem::borrowBook(const QString& readerId, const QString& isbn)
{
    Reader* r = findReader(readerId);
    Book* b = findByISBN(isbn);
    if (!r || !b) return Status::NotFound;
    if (!r->canBorrow()) return Status::Full;
    if (b->availableStock <= 0) return Status::Full;

    // 执行借阅
    b->availableStock--;
    b->borrowCount++;
    r->currentBorrow++;

    auto* record = new BorrowRecord(nextRecordId++, readerId.toStdString(), isbn.toStdString());
    borrowRecords.append(record);

    // 更新堆
    hotHeap->updateKey(b);

    // 更新推荐图（读者借了两本以上的书，增加它们之间的边）
    std::vector<std::string> readerBooks;
    borrowRecords.traverse([&](BorrowRecord* br) {
        if (!br->returned && br->readerId == readerId.toStdString() && br->bookISBN != isbn.toStdString()) {
            readerBooks.push_back(br->bookISBN);
        }
    });
    for (const auto& otherIsbn : readerBooks) {
        recommendationGraph.addEdge(b->isbn, otherIsbn, 1);
    }

    // 记录操作历史（用于Undo）
    OperationRecord op;
    op.type = OperationType::BorrowBook;
    op.readerId = readerId;
    op.bookISBN = isbn;
    op.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    undoStack.push(op);

    emitOp("借书", {"哈希表", "二叉排序树", "链表", "堆", "图", "栈"},
           QString("哈希表验证读者，BST/索书号定位图书，链表生成借阅记录，堆更新排行，图更新关联，栈记录操作"));
    return Status::OK;
}

Status LibrarySystem::returnBook(const QString& readerId, const QString& isbn)
{
    Reader* r = findReader(readerId);
    Book* b = findByISBN(isbn);
    if (!r || !b) return Status::NotFound;

    b->availableStock++;
    r->currentBorrow--;

    // 标记记录为已归还
    std::string sReader = readerId.toStdString();
    std::string sIsbn = isbn.toStdString();
    borrowRecords.traverse([&](BorrowRecord* br) {
        if (!br->returned && br->readerId == sReader && br->bookISBN == sIsbn) {
            br->markReturned();
        }
    });

    hotHeap->updateKey(b);

    OperationRecord op;
    op.type = OperationType::ReturnBook;
    op.readerId = readerId;
    op.bookISBN = isbn;
    op.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    undoStack.push(op);

    emitOp("还书", {"哈希表", "二叉排序树", "链表", "堆", "栈"},
           "归还图书，更新库存、读者借阅数、堆排行，栈记录操作");
    return Status::OK;
}

Status LibrarySystem::undoLastOperation()
{
    OperationRecord op;
    if (undoStack.pop(op) != Status::OK) return Status::Empty;

    if (op.type == OperationType::BorrowBook) {
        // 撤销借书 = 还书（但不记录栈）
        Reader* r = findReader(op.readerId);
        Book* b = findByISBN(op.bookISBN);
        if (r && b) {
            b->availableStock++;
            r->currentBorrow--;
            b->borrowCount--;
            hotHeap->updateKey(b);
        }
        // 标记记录为已归还
        std::string sReader = op.readerId.toStdString();
        std::string sIsbn = op.bookISBN.toStdString();
        borrowRecords.traverse([&](BorrowRecord* br) {
            if (!br->returned && br->readerId == sReader && br->bookISBN == sIsbn) {
                br->markReturned();
            }
        });
    } else if (op.type == OperationType::ReturnBook) {
        // 撤销还书 = 重新借出（但不记录栈）
        Reader* r = findReader(op.readerId);
        Book* b = findByISBN(op.bookISBN);
        if (r && b) {
            b->availableStock--;
            r->currentBorrow++;
            b->borrowCount++;
            hotHeap->updateKey(b);
        }
    }

    emitOp("撤销操作", {"栈"}, "从栈中弹出上一次操作并执行反向逻辑");
    return Status::OK;
}

bool LibrarySystem::canBorrow(const QString& readerId, const QString& isbn) const
{
    Reader* r = findReader(readerId);
    Book* b = findByISBN(isbn);
    return r && b && r->canBorrow() && b->availableStock > 0;
}

Status LibrarySystem::enqueueReservation(const QString& readerId, const QString& isbn)
{
    std::string sIsbn = isbn.toStdString();
    Book* b = findByISBN(isbn);
    if (!b) return Status::NotFound;
    if (b->availableStock > 0) return Status::Invalid; // 有库存不需要排队

    reservationQueues[sIsbn].enqueue(readerId);

    emitOp("预约排队", {"队列"}, "该书库存为0，读者加入预约队列");
    return Status::OK;
}

// ---- 推荐系统 ----
std::vector<std::pair<QString, int>> LibrarySystem::recommendBooks(const QString& isbn, int topN) const
{
    auto result = recommendationGraph.recommend(isbn.toStdString(), topN);
    std::vector<std::pair<QString, int>> converted;
    for (auto& pair : result) {
        converted.emplace_back(QString::fromStdString(pair.first), pair.second);
    }
    return converted;
}

// ---- 热门排行 ----
std::vector<Book*> LibrarySystem::getHotBooks(int topN) const
{
    return hotHeap->getTopN(topN);
}

// ---- 座位管理 ----
Status LibrarySystem::reserveSeat(int row, int col, const QString& readerId,
                                   const QString& start, const QString& end)
{
    Seat* s = seatMatrix.getPtr(row, col);
    if (s && !s->isFree()) return Status::Full;

    Seat newSeat(row, col);
    newSeat.occupy(readerId.toStdString(), start.toStdString(), end.toStdString());
    seatMatrix.set(row, col, newSeat);

    emitOp("预约座位", {"稀疏矩阵"}, QString("座位(%1,%2)状态更新为已预约").arg(row).arg(col));
    return Status::OK;
}

Status LibrarySystem::releaseSeat(int row, int col)
{
    Seat* s = seatMatrix.getPtr(row, col);
    if (!s) return Status::NotFound;
    s->release();
    // 更新稀疏矩阵
    seatMatrix.set(row, col, *s);

    emitOp("释放座位", {"稀疏矩阵"}, QString("座位(%1,%2)状态更新为空闲").arg(row).arg(col));
    return Status::OK;
}

Seat* LibrarySystem::getSeat(int row, int col) const
{
    return const_cast<SparseMatrix<Seat>&>(seatMatrix).getPtr(row, col);
}

std::vector<std::vector<Seat>> LibrarySystem::getSeatGrid(int rows, int cols) const
{
    return seatMatrix.toDenseMatrix(rows, cols, Seat());
}

void LibrarySystem::emitOp(const QString& name, const QStringList& ds, const QString& desc)
{
    emit operationPerformed(name, ds, desc);
}
