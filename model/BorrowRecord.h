#pragma once
#include <string>
#include <ctime>

/**
 * 借阅记录实体
 * 对应模块：借阅与归还（线性表/链表存储）
 */
class BorrowRecord {
public:
    int recordId = 0;               // 记录编号
    std::string readerId;           // 读者ID
    std::string bookISBN;           // 图书ISBN
    std::string borrowTime;         // 借出时间
    std::string returnTime;         // 归还时间（空表示未还）
    bool returned = false;          // 是否已归还

    BorrowRecord() = default;
    BorrowRecord(int id, std::string rId, std::string bIsbn)
        : recordId(id), readerId(std::move(rId)), bookISBN(std::move(bIsbn)),
          returned(false) {
        borrowTime = getCurrentTimeString();
    }

    void markReturned() {
        returned = true;
        returnTime = getCurrentTimeString();
    }

private:
    static std::string getCurrentTimeString() {
        time_t now = time(nullptr);
        tm t;
        localtime_s(&t, &now);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
        return std::string(buf);
    }
};
