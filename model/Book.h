#pragma once
#include <string>

/**
 * 图书实体类
 * 对应模块：图书信息管理、热门图书排行
 */
class Book {
public:
    std::string isbn;           // ISBN（唯一标识）
    std::string callNumber;     // 索书号（如 TP311.1，用于BST/AVL树索引）
    std::string title;          // 书名
    std::string author;         // 作者
    std::string publisher;      // 出版社
    int totalStock = 0;         // 总库存
    int availableStock = 0;     // 当前可用库存
    int borrowCount = 0;        // 累计借阅次数（用于热门排行）

    Book() = default;
    Book(std::string isbn, std::string callNum, std::string title,
         std::string author, std::string publisher, int stock)
        : isbn(std::move(isbn)), callNumber(std::move(callNum)),
          title(std::move(title)), author(std::move(author)),
          publisher(std::move(publisher)), totalStock(stock),
          availableStock(stock), borrowCount(0) {}

    // 用于BST/AVL树比较：按索书号排序
    bool operator<(const Book& other) const { return callNumber < other.callNumber; }
    bool operator>(const Book& other) const { return callNumber > other.callNumber; }
    bool operator==(const Book& other) const { return callNumber == other.callNumber; }
    bool operator!=(const Book& other) const { return callNumber != other.callNumber; }

    // 哈希表比较：按ISBN精准匹配
    bool isbnEquals(const std::string& targetISBN) const { return isbn == targetISBN; }
    bool titleEquals(const std::string& targetTitle) const { return title == targetTitle; }
};
