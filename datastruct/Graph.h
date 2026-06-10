#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

/**
 * 无向带权图（邻接表实现）
 * 应用场景：图书推荐系统
 * 顶点：图书（用ISBN字符串标识）
 * 边：两本书被同一读者借阅过，权重为共同借阅次数
 */
class Graph {
public:
    struct Edge {
        std::string to;     // 邻接顶点ISBN
        int weight = 0;     // 共同借阅次数
        Edge* next = nullptr;

        Edge(const std::string& t, int w) : to(t), weight(w) {}
    };

    struct Vertex {
        std::string isbn;       // 图书ISBN（顶点标识）
        Edge* firstEdge = nullptr;
        bool exists = true;     // 顶点是否存在（软删除用）
    };

    Graph() = default;
    ~Graph() { clear(); }

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    // 添加顶点（图书ISBN）
    Status addVertex(const std::string& isbn) {
        if (vertices.count(isbn)) return Status::Duplicate;
        Vertex v;
        v.isbn = isbn;
        vertices[isbn] = v;
        return Status::OK;
    }

    // 删除顶点
    Status removeVertex(const std::string& isbn) {
        if (!vertices.count(isbn)) return Status::NotFound;
        // 先删除所有以该顶点为终点的边
        for (auto& pair : vertices) {
            if (pair.first != isbn) {
                removeEdge(pair.first, isbn);
            }
        }
        // 删除该顶点的边链表
        Edge* cur = vertices[isbn].firstEdge;
        while (cur) {
            Edge* next = cur->next;
            delete cur;
            cur = next;
        }
        vertices.erase(isbn);
        return Status::OK;
    }

    // 增加边的权重（无向图：双向都增加，若边不存在则创建，权重为1）
    Status addEdge(const std::string& u, const std::string& v, int delta = 1) {
        if (u == v) return Status::Invalid;
        ensureVertex(u);
        ensureVertex(v);
        addDirectedEdge(u, v, delta);
        addDirectedEdge(v, u, delta);
        return Status::OK;
    }

    // 获取顶点的所有邻接边（用于推荐算法和可视化）
    std::vector<std::pair<std::string, int>> getNeighbors(const std::string& isbn) const {
        std::vector<std::pair<std::string, int>> result;
        auto it = vertices.find(isbn);
        if (it == vertices.end()) return result;
        Edge* cur = it->second.firstEdge;
        while (cur) {
            result.emplace_back(cur->to, cur->weight);
            cur = cur->next;
        }
        return result;
    }

    // 推荐算法：返回与给定图书关联度最高的TopN本书
    std::vector<std::pair<std::string, int>> recommend(const std::string& isbn, int topN = 5) const {
        auto neighbors = getNeighbors(isbn);
        std::sort(neighbors.begin(), neighbors.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        if (neighbors.size() > static_cast<size_t>(topN))
            neighbors.resize(topN);
        return neighbors;
    }

    // 获取所有顶点（用于打印邻接表和可视化）
    std::vector<std::string> getAllVertices() const {
        std::vector<std::string> result;
        for (const auto& pair : vertices) result.push_back(pair.first);
        return result;
    }

    // 获取顶点是否存在
    bool hasVertex(const std::string& isbn) const {
        return vertices.count(isbn) > 0;
    }

    // 遍历邻接表（用于可视化）
    void traverseAdjList(const std::function<void(const std::string&, const std::vector<std::pair<std::string, int>>&)>& callback) const {
        for (const auto& pair : vertices) {
            callback(pair.first, getNeighbors(pair.first));
        }
    }

    void clear() {
        for (auto& pair : vertices) {
            Edge* cur = pair.second.firstEdge;
            while (cur) {
                Edge* next = cur->next;
                delete cur;
                cur = next;
            }
        }
        vertices.clear();
    }

private:
    std::map<std::string, Vertex> vertices;  // 用map保证ISBN有序，便于遍历

    void ensureVertex(const std::string& isbn) {
        if (!vertices.count(isbn)) {
            Vertex v;
            v.isbn = isbn;
            vertices[isbn] = v;
        }
    }

    void addDirectedEdge(const std::string& from, const std::string& to, int delta) {
        Vertex& v = vertices[from];
        Edge* cur = v.firstEdge;
        while (cur) {
            if (cur->to == to) {
                cur->weight += delta;
                return;
            }
            cur = cur->next;
        }
        // 新建边，头插法
        Edge* e = new Edge(to, delta);
        e->next = v.firstEdge;
        v.firstEdge = e;
    }

    void removeEdge(const std::string& from, const std::string& to) {
        Vertex& v = vertices[from];
        Edge* cur = v.firstEdge;
        Edge* prev = nullptr;
        while (cur) {
            if (cur->to == to) {
                if (prev) prev->next = cur->next;
                else v.firstEdge = cur->next;
                delete cur;
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }
};
