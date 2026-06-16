#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>

class LibrarySystem;
class MessageQueueProcessor;

/**
 * 消息队列视图面板（从 视图 菜单开启/关闭）
 * 显示消息队列的实时状态：待处理队列 + 处理历史日志
 * 所有写操作自动经过消息队列序列化，保证多读者并发安全
 */
class MessageQueueTab : public QWidget {
    Q_OBJECT

public:
    explicit MessageQueueTab(LibrarySystem* lib, QWidget* parent = nullptr);

signals:
    void queueRefreshed();

public slots:
    void refreshAll();

private slots:
    void onQueueChanged();
    void onMessageProcessed(int id, bool success, const QString& result);

private:
    void setupUI();
    void refreshQueueTable();
    void refreshHistoryLog();

    LibrarySystem* library;
    MessageQueueProcessor* processor;

    QLabel* titleLabel;
    QLabel* pendingLabel;
    QTableWidget* queueTable;
    QTextEdit* historyLog;
};
