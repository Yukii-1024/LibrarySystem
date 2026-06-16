#include "SeedData.h"
#include "model/Book.h"
#include "model/Reader.h"
#include "model/Seat.h"

void SeedData::populate(LibrarySystem* lib)
{
    if (!lib) return;
    if (!lib->getAllBooks().empty()) return; // already populated

    // --- Books (30) ---
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

    auto* b11 = new Book("978-7-111-44250-4", "TP316/O01",
        "深入理解计算机系统", "Bryant", "机械工业出版社", 4);
    auto* b12 = new Book("978-7-302-54132-7", "TP311.13/D01",
        "数据库系统概念", "Silberschatz", "机械工业出版社", 3);
    auto* b13 = new Book("978-7-111-58823-6", "TP18/R01",
        "人工智能：一种现代方法", "Russell", "清华大学出版社", 3);
    auto* b14 = new Book("978-7-115-48549-8", "TP312/J02",
        "Java核心技术 卷I", "Horstmann", "机械工业出版社", 5);
    auto* b15 = new Book("978-7-302-47364-0", "TP312/P01",
        "流畅的Python", "Ramalho", "人民邮电出版社", 4);
    auto* b16 = new Book("978-7-111-59563-0", "TP311.5/S01",
        "软件工程：实践者的研究方法", "Pressman", "机械工业出版社", 2);
    auto* b17 = new Book("978-7-302-51116-0", "TP393.08/W01",
        "网络安全基础", "Stallings", "电子工业出版社", 3);
    auto* b18 = new Book("978-7-111-56125-7", "TP311.1/L01",
        "程序员修炼之道", "Hunt", "人民邮电出版社", 3);
    auto* b19 = new Book("978-7-121-35903-3", "TP312/J03",
        "深入理解Java虚拟机", "周志明", "机械工业出版社", 5);
    auto* b20 = new Book("978-7-302-56432-8", "TP311.1/D03",
        "算法图解", "Bhargava", "人民邮电出版社", 4);
    auto* b21 = new Book("978-7-111-62259-5", "TP316/L01",
        "UNIX环境高级编程", "Stevens", "机械工业出版社", 2);
    auto* b22 = new Book("978-7-111-53516-4", "TP312/G01",
        "Go程序设计语言", "Donovan", "机械工业出版社", 3);
    auto* b23 = new Book("978-7-115-42280-2", "TP393.09/W01",
        "HTTP权威指南", "Gourley", "人民邮电出版社", 2);
    auto* b24 = new Book("978-7-302-48497-4", "TP183/D01",
        "深度学习", "Goodfellow", "人民邮电出版社", 3);
    auto* b25 = new Book("978-7-111-60162-3", "TP314/C01",
        "编译原理", "Aho", "机械工业出版社", 2);
    auto* b26 = new Book("978-7-121-34694-5", "TP312/R01",
        "R语言实战", "Kabacoff", "人民邮电出版社", 3);
    auto* b27 = new Book("978-7-111-54803-6", "TP311.5/M01",
        "人月神话", "Brooks", "清华大学出版社", 3);
    auto* b28 = new Book("978-7-302-37551-5", "TP312/S01",
        "Scala编程", "Odersky", "电子工业出版社", 2);
    auto* b29 = new Book("978-7-111-67281-6", "TP316/D01",
        "分布式系统：概念与设计", "Coulouris", "机械工业出版社", 2);
    auto* b30 = new Book("978-7-121-39042-5", "TP311.1/K01",
        "代码整洁之道", "Martin", "人民邮电出版社", 4);

    lib->addBook(b1); lib->addBook(b2); lib->addBook(b3);
    lib->addBook(b4); lib->addBook(b5); lib->addBook(b6);
    lib->addBook(b7); lib->addBook(b8); lib->addBook(b9);
    lib->addBook(b10); lib->addBook(b11); lib->addBook(b12);
    lib->addBook(b13); lib->addBook(b14); lib->addBook(b15);
    lib->addBook(b16); lib->addBook(b17); lib->addBook(b18);
    lib->addBook(b19); lib->addBook(b20); lib->addBook(b21);
    lib->addBook(b22); lib->addBook(b23); lib->addBook(b24);
    lib->addBook(b25); lib->addBook(b26); lib->addBook(b27);
    lib->addBook(b28); lib->addBook(b29); lib->addBook(b30);

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
