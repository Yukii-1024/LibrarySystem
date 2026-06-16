#include "LogPanel.h"
#include "core/LibrarySystem.h"
#include "core/OperationLog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>

LogPanel::LogPanel(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib)
{
    logger = library->getLogger();
    setupUI();

    connect(logger, &OperationLogger::entryAdded,
            this, &LogPanel::onNewEntry);
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogPanel::onFilterChanged);
}

void LogPanel::setupUI()
{
    auto* main = new QVBoxLayout(this);
    main->setContentsMargins(4, 4, 4, 4);
    main->setSpacing(4);

    // Title bar
    auto* topBar = new QHBoxLayout();
    titleLabel = new QLabel(QString::fromUtf8("操作日志"), this);
    titleLabel->setStyleSheet(
        "font-weight: bold; font-size: 12px; color: #37474F;"
        "background: #E3F2FD; border-radius: 3px; padding: 2px 6px;");
    topBar->addWidget(titleLabel);
    topBar->addStretch();

    filterCombo = new QComboBox(this);
    filterCombo->addItem(QString::fromUtf8("全部操作"));
    filterCombo->addItem(QString::fromUtf8("仅管理员操作"));
    filterCombo->setMaximumWidth(140);
    topBar->addWidget(filterCombo);

    main->addLayout(topBar);

    // Log table
    logTable = new QTableWidget(0, 4, this);
    logTable->setHorizontalHeaderLabels({
        QString::fromUtf8("时间"),
        QString::fromUtf8("操作人"),
        QString::fromUtf8("操作类型"),
        QString::fromUtf8("详情")
    });
    logTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    logTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    logTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    logTable->horizontalHeader()->setStretchLastSection(true);
    logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    logTable->setAlternatingRowColors(true);

    main->addWidget(logTable, 1);

    refreshAll();
}

void LogPanel::onFilterChanged(int index)
{
    adminOnly = (index == 1);
    refreshTable();
}

void LogPanel::onNewEntry(const LogEntry& entry)
{
    Q_UNUSED(entry);
    if (adminOnly && !entry.isAdmin) return;
    refreshTable();
}

void LogPanel::refreshAll()
{
    refreshTable();
    emit logRefreshed();
}

void LogPanel::refreshTable()
{
    auto entries = logger->getFiltered(adminOnly);
    int n = static_cast<int>(entries.size());
    logTable->setRowCount(n);

    for (int i = 0; i < n; ++i) {
        const auto& e = entries[i];
        logTable->setItem(i, 0, new QTableWidgetItem(
            e.timestamp.toString("MM-dd hh:mm:ss")));
        logTable->setItem(i, 1, new QTableWidgetItem(e.operatorId));
        logTable->setItem(i, 2, new QTableWidgetItem(e.operation));
        logTable->setItem(i, 3, new QTableWidgetItem(e.details));
    }

    // Auto-scroll to bottom
    if (n > 0)
        logTable->scrollToBottom();
}
