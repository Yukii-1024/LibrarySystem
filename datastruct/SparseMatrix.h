#pragma once
#include "Common.h"
#include <map>
#include <vector>
#include <functional>

/**
 * 稀疏矩阵模板类（基于map的坐标映射）
 * 应用场景：图书馆座位管理
 * 当座位总数大但预约稀疏时，比二维数组更省空间
 * 同时提供紧凑数组模式用于可视化
 *
 * RowKey/ColKey: 坐标类型（通常 int）
 * T: 存储的数据类型（如 Seat* 或 SeatStatus）
 */
template <typename T, typename RowKey = int, typename ColKey = int>
class SparseMatrix {
public:
    // 用 pair 的 operator< 作为 map 的键
    using Coord = std::pair<RowKey, ColKey>;

    SparseMatrix() = default;

    // 设置/更新元素
    Status set(RowKey row, ColKey col, const T& value) {
        data[{row, col}] = value;
        return Status::OK;
    }

    // 获取元素（如果不存在返回默认值）
    T get(RowKey row, ColKey col, const T& defaultVal = T()) const {
        auto it = data.find({row, col});
        return (it != data.end()) ? it->second : defaultVal;
    }

    // 获取元素指针（nullptr表示不存在）
    T* getPtr(RowKey row, ColKey col) {
        auto it = data.find({row, col});
        return (it != data.end()) ? &(it->second) : nullptr;
    }

    const T* getPtr(RowKey row, ColKey col) const {
        auto it = data.find({row, col});
        return (it != data.end()) ? &(it->second) : nullptr;
    }

    // 删除元素
    Status remove(RowKey row, ColKey col) {
        auto it = data.find({row, col});
        if (it == data.end()) return Status::NotFound;
        data.erase(it);
        return Status::OK;
    }

    // 是否存在
    bool contains(RowKey row, ColKey col) const {
        return data.count({row, col}) > 0;
    }

    // 获取所有非零元素的坐标（用于遍历和可视化）
    std::vector<Coord> getNonZeroCoords() const {
        std::vector<Coord> result;
        result.reserve(data.size());
        for (const auto& pair : data) {
            result.push_back(pair.first);
        }
        return result;
    }

    // 遍历所有非零元素
    void traverseNonZero(const std::function<void(RowKey, ColKey, T&)>& callback) {
        for (auto& pair : data) {
            callback(pair.first.first, pair.first.second, pair.second);
        }
    }

    void traverseNonZero(const std::function<void(RowKey, ColKey, const T&)>& callback) const {
        for (const auto& pair : data) {
            callback(pair.first.first, pair.first.second, pair.second);
        }
    }

    // 转换为二维数组形式（用于可视化渲染，给定行列范围）
    std::vector<std::vector<T>> toDenseMatrix(RowKey maxRow, ColKey maxCol, const T& defaultVal = T()) const {
        std::vector<std::vector<T>> mat(maxRow, std::vector<T>(maxCol, defaultVal));
        for (const auto& pair : data) {
            RowKey r = pair.first.first;
            ColKey c = pair.first.second;
            if (r >= 0 && r < maxRow && c >= 0 && c < maxCol) {
                mat[r][c] = pair.second;
            }
        }
        return mat;
    }

    int nonZeroCount() const { return static_cast<int>(data.size()); }
    bool isEmpty() const { return data.empty(); }
    void clear() { data.clear(); }

private:
    std::map<Coord, T> data;
};
