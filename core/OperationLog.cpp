#include "OperationLog.h"

OperationLogger::OperationLogger(QObject* parent)
    : QObject(parent) {}

void OperationLogger::log(const QString& operatorId, bool isAdmin,
                           const QString& operation, const QString& details)
{
    LogEntry entry;
    entry.timestamp  = QDateTime::currentDateTime();
    entry.operatorId = operatorId;
    entry.isAdmin    = isAdmin;
    entry.operation  = operation;
    entry.details    = details;

    entries.push_back(entry);

    // Keep bounded
    while (static_cast<int>(entries.size()) > MAX_ENTRIES)
        entries.erase(entries.begin());

    emit entryAdded(entry);
}

std::vector<LogEntry> OperationLogger::getFiltered(bool adminOnly) const
{
    if (!adminOnly) return entries;

    std::vector<LogEntry> result;
    for (const auto& e : entries) {
        if (e.isAdmin)
            result.push_back(e);
    }
    return result;
}

void OperationLogger::clear()
{
    entries.clear();
}
