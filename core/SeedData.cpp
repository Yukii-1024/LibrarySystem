#include "SeedData.h"
#include "model/Book.h"
#include "model/Reader.h"
#include "model/Seat.h"

void SeedData::populate(LibrarySystem* lib)
{
    if (!lib) return;
    if (!lib->getAllBooks().empty()) return; // already populated

    // --- Books (10) ---
    auto* b1 = new Book("978-7-302-23456-1", "TP311.1/C01",
        "数据结构（C语言版）", "严蔚敏", "清华大学出版社", 5);
    auto* b2 = new Book("978-7-111-40701-8", "TP311.1/C02",
        "算法导论", "Cormen", "机械工业出版社", 3);
    auto* b3 = new Book("978-7-302-32423-2", "TP312/C01",
        "C++ Primer", "Lippman", "电子工业出版社", 4);
    auto* b4 = new Book("978-7-111-52901-6", "TP312/J01",
        "Effective Java", "Bloch", "机械工业出版社", 3);
    auto* b5 = new Book("978-7-111-50815-3", "TP311.5/D01",
        "设计模式", "GoF", "机械工业出版社", 2);
    auto* b6 = new Book("978-7-121-28482-7", "TP393/N01",
        "计算机网络", "谢希仁", "电子工业出版社", 4);
    auto* b7 = new Book("978-7-111-39286-5", "TP311.1/D02",
        "编程珠玑", "Bentley", "人民邮电出版社", 2);
    auto* b8 = new Book("978-7-302-23729-6", "TP311.1/P01",
        "Python编程从入门到实践", "Matthes", "人民邮电出版社", 5);
    auto* b9 = new Book("978-7-111-61574-1", "TP311.5/R01",
        "重构", "Fowler", "机械工业出版社", 2);
    auto* b10 = new Book("978-7-111-40702-5", "TP312/C02",
        "C程序设计语言", "K&R", "机械工业出版社", 3);

    lib->addBook(b1); lib->addBook(b2); lib->addBook(b3);
    lib->addBook(b4); lib->addBook(b5); lib->addBook(b6);
    lib->addBook(b7); lib->addBook(b8); lib->addBook(b9);
    lib->addBook(b10);

    // --- Readers (5) ---
    lib->addReader(new Reader("admin", "admin123", "管理员", "图书馆", true));
    lib->addReader(new Reader("2024001", "123456", "张三", "计算机科学系", false));
    lib->addReader(new Reader("2024002", "123456", "李四", "软件工程系", false));
    lib->addReader(new Reader("2024003", "123456", "王五", "信息管理系", false));
    lib->addReader(new Reader("2024004", "123456", "赵六", "数学系", false));

    // --- Borrow some books (creates graph edges + heap activity) ---
    lib->borrowBook("2024001", "978-7-302-23456-1"); // 张三借《数据结构》
    lib->borrowBook("2024001", "978-7-302-32423-2"); // 张三借《C++ Primer》
    lib->borrowBook("2024002", "978-7-111-40701-8"); // 李四借《算法导论》
    lib->borrowBook("2024002", "978-7-111-52901-6"); // 李四借《Effective Java》
    lib->borrowBook("2024003", "978-7-302-23456-1"); // 王五借《数据结构》(第二本)

    // --- Seat reservations ---
    lib->reserveSeat(0, 0, "2024001", "2025-06-01 08:00", "2025-06-01 12:00");
    lib->reserveSeat(0, 1, "2024002", "2025-06-01 09:00", "2025-06-01 11:00");
    lib->reserveSeat(1, 2, "2024003", "2025-06-01 14:00", "2025-06-01 18:00");
}
