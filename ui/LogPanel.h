#pragma once
#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include "core/OperationLog.h"

class LibrarySystem;
class OperationLogger;

/**
 * 操作日志面板（从 视图 菜单开启/关闭）
 * 记录并展示所有重要操作，支持按 全部/管理员 筛选
 */
class LogPanel : public QWidget {
    Q_OBJECT

public:
    explicit LogPanel(LibrarySystem* lib, QWidget* parent = nullptr);

signals:
    void logRefreshed();

public slots:
    void refreshAll();

private slots:
    void onFilterChanged(int index);
    void onNewEntry(const LogEntry& entry);

private:
    void setupUI();
    void refreshTable();

    LibrarySystem* library;
    OperationLogger* logger;

    QLabel* titleLabel;
    QComboBox* filterCombo;
    QTableWidget* logTable;

    bool adminOnly = false;
};
