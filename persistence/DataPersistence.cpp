#include "DataPersistence.h"
#include "core/LibrarySystem.h"
#include "model/Book.h"
#include "model/Reader.h"
#include "model/BorrowRecord.h"
#include "model/Seat.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDebug>

QString DataPersistence::dbPath_;

// ── init / create tables ──────────────────────────────────

bool DataPersistence::initDatabase(const QString& dbPath)
{
    dbPath_ = dbPath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qWarning() << "Cannot open database:" << db.lastError().text();
        return false;
    }
    createTables();

    // Bootstrap: ensure there is at least one admin account
    QSqlQuery q;
    q.exec("SELECT COUNT(*) FROM readers");
    if (q.next() && q.value(0).toInt() == 0) {
        q.exec("INSERT INTO readers VALUES ('admin','admin123','Administrator','',10,0,1,1)");
        qDebug() << "Bootstrap admin account created";
    }

    return true;
}

void DataPersistence::createTables()
{
    QSqlQuery q;
    q.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        " isbn TEXT PRIMARY KEY,"
        " call_number TEXT NOT NULL,"
        " title TEXT NOT NULL,"
        " author TEXT DEFAULT '',"
        " publisher TEXT DEFAULT '',"
        " total_stock INTEGER DEFAULT 1,"
        " available_stock INTEGER DEFAULT 1,"
        " borrow_count INTEGER DEFAULT 0"
        ")"
    );
    q.exec(
        "CREATE TABLE IF NOT EXISTS readers ("
        " id TEXT PRIMARY KEY,"
        " password TEXT NOT NULL,"
        " name TEXT NOT NULL,"
        " department TEXT DEFAULT '',"
        " max_borrow INTEGER DEFAULT 10,"
        " current_borrow INTEGER DEFAULT 0,"
        " is_admin INTEGER DEFAULT 0,"
        " active INTEGER DEFAULT 1"
        ")"
    );
    q.exec(
        "CREATE TABLE IF NOT EXISTS borrow_records ("
        " record_id INTEGER PRIMARY KEY,"
        " reader_id TEXT NOT NULL,"
        " book_isbn TEXT NOT NULL,"
        " borrow_time TEXT DEFAULT '',"
        " return_time TEXT DEFAULT '',"
        " returned INTEGER DEFAULT 0"
        ")"
    );
    q.exec(
        "CREATE TABLE IF NOT EXISTS seats ("
        " row_idx INTEGER,"
        " col_idx INTEGER,"
        " status INTEGER DEFAULT 0,"
        " reader_id TEXT DEFAULT '',"
        " start_time TEXT DEFAULT '',"
        " end_time TEXT DEFAULT '',"
        " PRIMARY KEY (row_idx, col_idx)"
        ")"
    );
    q.exec(
        "CREATE TABLE IF NOT EXISTS reservations ("
        " isbn TEXT NOT NULL,"
        " reader_id TEXT NOT NULL,"
        " position INTEGER NOT NULL,"
        " PRIMARY KEY (isbn, reader_id)"
        ")"
    );
}

// ── save ──────────────────────────────────────────────────

bool DataPersistence::saveAll(LibrarySystem* sys)
{
    if (!sys) return false;
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) return false;

    db.transaction();

    // Clear existing rows
    QSqlQuery q;
    q.exec("DELETE FROM books");
    q.exec("DELETE FROM readers");
    q.exec("DELETE FROM borrow_records");
    q.exec("DELETE FROM seats");
    q.exec("DELETE FROM reservations");

    // Books
    q.prepare("INSERT INTO books VALUES (?,?,?,?,?,?,?,?)");
    sys->getBookList().traverse([&](Book*& b) {
        if (!b) return;
        q.addBindValue(QString::fromStdString(b->isbn));
        q.addBindValue(QString::fromStdString(b->callNumber));
        q.addBindValue(QString::fromStdString(b->title));
        q.addBindValue(QString::fromStdString(b->author));
        q.addBindValue(QString::fromStdString(b->publisher));
        q.addBindValue(b->totalStock);
        q.addBindValue(b->availableStock);
        q.addBindValue(b->borrowCount);
        q.exec();
    });

    // Readers
    q.prepare("INSERT INTO readers VALUES (?,?,?,?,?,?,?,?)");
    sys->getReaderList().traverse([&](Reader*& r) {
        if (!r) return;
        q.addBindValue(QString::fromStdString(r->id));
        q.addBindValue(QString::fromStdString(r->password));
        q.addBindValue(QString::fromStdString(r->name));
        q.addBindValue(QString::fromStdString(r->department));
        q.addBindValue(r->maxBorrow);
        q.addBindValue(r->currentBorrow);
        q.addBindValue(r->isAdmin ? 1 : 0);
        q.addBindValue(r->active ? 1 : 0);
        q.exec();
    });

    // Borrow records
    q.prepare("INSERT INTO borrow_records VALUES (?,?,?,?,?,?)");
    sys->getBorrowRecords().traverse([&](BorrowRecord*& br) {
        if (!br) return;
        q.addBindValue(static_cast<int>(br->recordId));
        q.addBindValue(QString::fromStdString(br->readerId));
        q.addBindValue(QString::fromStdString(br->bookISBN));
        q.addBindValue(QString::fromStdString(br->borrowTime));
        q.addBindValue(QString::fromStdString(br->returnTime));
        q.addBindValue(br->returned ? 1 : 0);
        q.exec();
    });

    // Seats
    q.prepare("INSERT INTO seats VALUES (?,?,?,?,?,?)");
    sys->getSeatMatrix().traverseNonZero([&](int r, int c, const Seat& s) {
        q.addBindValue(r);
        q.addBindValue(c);
        q.addBindValue(static_cast<int>(s.status));
        q.addBindValue(QString::fromStdString(s.readerId));
        q.addBindValue(QString::fromStdString(s.startTime));
        q.addBindValue(QString::fromStdString(s.endTime));
        q.exec();
    });

    // Reservations
    q.prepare("INSERT INTO reservations VALUES (?,?,?)");
    const auto& queues = sys->getReservationQueues();
    for (const auto& [isbn, qu] : queues) {
        int pos = 0;
        qu.traverse([&](const QString& readerId) {
            q.addBindValue(QString::fromStdString(isbn));
            q.addBindValue(readerId);
            q.addBindValue(pos++);
            q.exec();
        });
    }

    db.commit();
    qDebug() << "Data saved to" << dbPath_;
    return true;
}

// ── load ──────────────────────────────────────────────────

void DataPersistence::clearAll(LibrarySystem* sys)
{
    // Remove all data from in-memory structures
    sys->getBookList().traverse([](Book*& b) { delete b; });
    sys->getBookList().clear();

    sys->getReaderList().traverse([](Reader*& r) { delete r; });
    sys->getReaderList().clear();

    sys->getBorrowRecords().traverse([](BorrowRecord*& br) { delete br; });
    sys->getBorrowRecords().clear();

    sys->getUndoStack().clear();

    sys->getRecommendationGraph().clear();
    sys->getSeatMatrix().clear();
    sys->getReservationQueuesRef().clear();

    // Recreate structures
    sys->resetStructures();
    sys->setNextRecordId(1);
}

bool DataPersistence::loadAll(LibrarySystem* sys)
{
    if (!sys) return false;
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) return false;

    clearAll(sys);

    QSqlQuery q;

    // Books
    q.exec("SELECT * FROM books");
    while (q.next()) {
        auto* b = new Book(
            q.value("isbn").toString().toStdString(),
            q.value("call_number").toString().toStdString(),
            q.value("title").toString().toStdString(),
            q.value("author").toString().toStdString(),
            q.value("publisher").toString().toStdString(),
            q.value("total_stock").toInt()
        );
        b->availableStock = q.value("available_stock").toInt();
        b->borrowCount    = q.value("borrow_count").toInt();
        sys->addBook(b);
    }

    // Readers
    q.exec("SELECT * FROM readers");
    while (q.next()) {
        auto* r = new Reader(
            q.value("id").toString().toStdString(),
            q.value("password").toString().toStdString(),
            q.value("name").toString().toStdString(),
            q.value("department").toString().toStdString(),
            q.value("is_admin").toBool()
        );
        r->maxBorrow     = q.value("max_borrow").toInt();
        r->currentBorrow = q.value("current_borrow").toInt();
        r->active        = q.value("active").toBool();
        sys->addReader(r);
    }

    // Borrow records
    int maxId = 0;
    q.exec("SELECT * FROM borrow_records");
    while (q.next()) {
        auto* br = new BorrowRecord();
        int id = q.value("record_id").toInt();
        br->recordId   = static_cast<unsigned int>(id);
        br->readerId   = q.value("reader_id").toString().toStdString();
        br->bookISBN   = q.value("book_isbn").toString().toStdString();
        br->borrowTime = q.value("borrow_time").toString().toStdString();
        br->returnTime = q.value("return_time").toString().toStdString();
        br->returned   = q.value("returned").toBool();
        sys->getBorrowRecords().append(br);
        if (id > maxId) maxId = id;
    }
    sys->setNextRecordId(maxId + 1);

    // Seats
    q.exec("SELECT * FROM seats");
    while (q.next()) {
        Seat s;
        s.row       = q.value("row_idx").toInt();
        s.col       = q.value("col_idx").toInt();
        s.status    = static_cast<SeatStatus>(q.value("status").toInt());
        s.readerId  = q.value("reader_id").toString().toStdString();
        s.startTime = q.value("start_time").toString().toStdString();
        s.endTime   = q.value("end_time").toString().toStdString();
        sys->getSeatMatrix().set(s.row, s.col, s);
    }

    // Reservations
    q.exec("SELECT * FROM reservations ORDER BY isbn, position");
    while (q.next()) {
        std::string isbn = q.value("isbn").toString().toStdString();
        QString readerId = q.value("reader_id").toString();
        sys->getReservationQueuesRef()[isbn].enqueue(readerId);
    }

    qDebug() << "Data loaded from" << dbPath_;
    return true;
}

// ── helpers ────────────────────────────────────────────────

void DataPersistence::close()
{
    QSqlDatabase::database().close();
}

QString DataPersistence::currentPath()
{
    return dbPath_;
}
