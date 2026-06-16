#pragma once
#include <string>

/**
 * 座位状态枚举
 */
enum class SeatStatus {
    Free = 0,       // 空闲
    Occupied = 1   // 已预约
};

/**
 * 座位实体
 * 对应模块：座位预约管理（稀疏矩阵/二维数组）
 */
class Seat {
public:
    int row = 0;                // 行号
    int col = 0;                // 列号
    SeatStatus status = SeatStatus::Free;
    std::string readerId;       // 预约人ID
    std::string startTime;      // 预约开始时间
    std::string endTime;        // 预约结束时间

    Seat() = default;
    Seat(int r, int c) : row(r), col(c), status(SeatStatus::Free) {}

    bool isFree() const { return status == SeatStatus::Free; }

    void occupy(const std::string& rId, const std::string& start, const std::string& end) {
        status = SeatStatus::Occupied;
        readerId = rId;
        startTime = start;
        endTime = end;
    }

    void release() {
        status = SeatStatus::Free;
        readerId.clear();
        startTime.clear();
        endTime.clear();
    }
};
