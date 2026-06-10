#pragma once
#include <string>
#include <vector>

/**
 * 读者实体类
 * 对应模块：读者管理（哈希表）
 */
class Reader {
public:
    std::string id;             // 学号/工号（哈希表Key）
    std::string password;       // 密码（明文存储用于课程演示，实际应哈希）
    std::string name;           // 姓名
    std::string department;     // 院系/部门
    int maxBorrow = 10;         // 最大可借数量
    int currentBorrow = 0;      // 当前已借数量
    bool isAdmin = false;       // 是否为管理员
    bool active = true;         // 账户是否有效

    Reader() = default;
    Reader(std::string id, std::string pwd, std::string name,
           std::string dept, bool admin = false)
        : id(std::move(id)), password(std::move(pwd)),
          name(std::move(name)), department(std::move(dept)),
          isAdmin(admin), active(true), currentBorrow(0) {}

    bool canBorrow() const { return currentBorrow < maxBorrow; }
    bool hasPassword(const std::string& pwd) const { return password == pwd; }
};
