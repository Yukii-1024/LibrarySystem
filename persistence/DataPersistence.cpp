#include "DataPersistence.h"
#include "model/Book.h"
#include "model/Reader.h"
#include "model/BorrowRecord.h"
#include "model/Seat.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

bool DataPersistence::saveAll(LibrarySystem* sys, const QString& filePath)
{
    if (!sys) return false;

    QJsonObject root;
    root["version"] = 1;
    int maxRecordId = 0;

    // --- Books ---
    QJsonArray booksArr;
    sys->getBookList().traverse([&](Book*& b) {
        if (!b) return;
        QJsonObject obj;
        obj["isbn"]        = QString::fromStdString(b->isbn);
        obj["callNumber"]  = QString::fromStdString(b->callNumber);
        obj["title"]       = QString::fromStdString(b->title);
        obj["author"]      = QString::fromStdString(b->author);
        obj["publisher"]   = QString::fromStdString(b->publisher);
        obj["totalStock"]  = b->totalStock;
        obj["availableStock"] = b->availableStock;
        obj["borrowCount"] = b->borrowCount;
        booksArr.append(obj);
    });
    root["books"] = booksArr;

    // --- Readers ---
    QJsonArray readersArr;
    sys->getReaderList().traverse([&](Reader*& r) {
        if (!r) return;
        QJsonObject obj;
        obj["id"]           = QString::fromStdString(r->id);
        obj["password"]     = QString::fromStdString(r->password);
        obj["name"]         = QString::fromStdString(r->name);
        obj["department"]   = QString::fromStdString(r->department);
        obj["maxBorrow"]    = r->maxBorrow;
        obj["currentBorrow"] = r->currentBorrow;
        obj["isAdmin"]      = r->isAdmin;
        obj["active"]       = r->active;
        readersArr.append(obj);
    });
    root["readers"] = readersArr;

    // --- Borrow Records ---
    QJsonArray recordsArr;
    sys->getBorrowRecords().traverse([&](BorrowRecord*& br) {
        if (!br) return;
        QJsonObject obj;
        obj["recordId"]   = static_cast<int>(br->recordId);
        obj["readerId"]   = QString::fromStdString(br->readerId);
        obj["bookISBN"]   = QString::fromStdString(br->bookISBN);
        obj["borrowTime"] = QString::fromStdString(br->borrowTime);
        obj["returnTime"] = QString::fromStdString(br->returnTime);
        obj["returned"]   = br->returned;
        recordsArr.append(obj);
        if (static_cast<int>(br->recordId) > maxRecordId)
            maxRecordId = static_cast<int>(br->recordId);
    });
    root["borrowRecords"] = recordsArr;
    root["nextRecordId"] = maxRecordId + 1;

    // --- Seats ---
    QJsonArray seatsArr;
    sys->getSeatMatrix().traverseNonZero([&](int row, int col, const Seat& s) {
        QJsonObject obj;
        obj["row"]       = row;
        obj["col"]       = col;
        obj["status"]    = static_cast<int>(s.status);
        obj["readerId"]  = QString::fromStdString(s.readerId);
        obj["startTime"] = QString::fromStdString(s.startTime);
        obj["endTime"]   = QString::fromStdString(s.endTime);
        seatsArr.append(obj);
    });
    root["seats"] = seatsArr;

    // --- Reservation Queues ---
    QJsonArray resArr;
    const auto& queues = sys->getReservationQueues();
    for (const auto& pair : queues) {
        QJsonObject obj;
        obj["isbn"] = QString::fromStdString(pair.first);
        QJsonArray qArr;
        pair.second.traverse([&](const QString& readerId) {
            qArr.append(readerId);
        });
        obj["queue"] = qArr;
        resArr.append(obj);
    }
    root["reservations"] = resArr;

    // Write
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool DataPersistence::loadAll(LibrarySystem* sys, const QString& filePath)
{
    if (!sys) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject root = doc.object();

    // Books
    for (const auto& val : root["books"].toArray()) {
        QJsonObject o = val.toObject();
        auto* b = new Book(
            o["isbn"].toString().toStdString(),
            o["callNumber"].toString().toStdString(),
            o["title"].toString().toStdString(),
            o["author"].toString().toStdString(),
            o["publisher"].toString().toStdString(),
            o["totalStock"].toInt()
        );
        b->availableStock = o["availableStock"].toInt();
        b->borrowCount    = o["borrowCount"].toInt();
        sys->addBook(b);
    }

    // Readers
    for (const auto& val : root["readers"].toArray()) {
        QJsonObject o = val.toObject();
        auto* r = new Reader(
            o["id"].toString().toStdString(),
            o["password"].toString().toStdString(),
            o["name"].toString().toStdString(),
            o["department"].toString().toStdString(),
            o["isAdmin"].toBool()
        );
        r->maxBorrow     = o["maxBorrow"].toInt(10);
        r->currentBorrow = o["currentBorrow"].toInt();
        r->active        = o["active"].toBool(true);
        sys->addReader(r);
    }

    // Borrow Records
    for (const auto& val : root["borrowRecords"].toArray()) {
        QJsonObject o = val.toObject();
        auto* br = new BorrowRecord();
        br->recordId  = static_cast<unsigned int>(o["recordId"].toInt());
        br->readerId  = o["readerId"].toString().toStdString();
        br->bookISBN  = o["bookISBN"].toString().toStdString();
        br->borrowTime = o["borrowTime"].toString().toStdString();
        br->returnTime = o["returnTime"].toString().toStdString();
        br->returned  = o["returned"].toBool();
        sys->getBorrowRecords().append(br);
    }

    // Seats
    for (const auto& val : root["seats"].toArray()) {
        QJsonObject o = val.toObject();
        Seat s;
        s.row       = o["row"].toInt();
        s.col       = o["col"].toInt();
        s.status    = static_cast<SeatStatus>(o["status"].toInt());
        s.readerId  = o["readerId"].toString().toStdString();
        s.startTime = o["startTime"].toString().toStdString();
        s.endTime   = o["endTime"].toString().toStdString();
        sys->getSeatMatrix().set(s.row, s.col, s);
    }

    // Reservation Queues
    for (const auto& val : root["reservations"].toArray()) {
        QJsonObject o = val.toObject();
        std::string isbn = o["isbn"].toString().toStdString();
        for (const auto& qv : o["queue"].toArray())
            sys->getReservationQueuesRef()[isbn].enqueue(qv.toString());
    }

    sys->setNextRecordId(root["nextRecordId"].toInt(1));
    return true;
}
