#pragma once
#include <QObject>
#include <QString>
#include <QDateTime>
#include <vector>

/**
 * 操作日志条目
 */
struct LogEntry {
    QDateTime timestamp;
    QString operatorId;     // 操作人学号/工号
    bool isAdmin = false;   // 是否管理员操作
    QString operation;      // 操作类型名称
    QString details;        // 操作详情
};

/**
 * 操作日志管理器（用户层面审计日志）
 * 记录所有重要操作，支持按角色筛选
 */
class OperationLogger : public QObject {
    Q_OBJECT

public:
    explicit OperationLogger(QObject* parent = nullptr);

    /// 记录一条操作日志
    void log(const QString& operatorId, bool isAdmin,
             const QString& operation, const QString& details);

    /// 获取全部日志
    const std::vector<LogEntry>& getAll() const { return entries; }

    /// 筛选：管理员操作 or 全部
    std::vector<LogEntry> getFiltered(bool adminOnly) const;

    /// 清空日志
    void clear();

signals:
    void entryAdded(const LogEntry& entry);

private:
    std::vector<LogEntry> entries;
    static constexpr int MAX_ENTRIES = 500;
};
