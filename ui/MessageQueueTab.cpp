#include "MessageQueueTab.h"
#include "core/LibrarySystem.h"
#include "core/MessageQueue.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSplitter>

MessageQueueTab::MessageQueueTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib)
{
    processor = library->getMessageQueue();
    setupUI();

    connect(processor, &MessageQueueProcessor::queueChanged,
            this, &MessageQueueTab::onQueueChanged);
    connect(processor, &MessageQueueProcessor::messageProcessed,
            this, &MessageQueueTab::onMessageProcessed);
}

void MessageQueueTab::setupUI()
{
    auto* main = new QVBoxLayout(this);
    main->setContentsMargins(4, 4, 4, 4);
    main->setSpacing(4);

    // Title
    titleLabel = new QLabel(QString::fromUtf8("消息队列 (FIFO)  —  多读者并发安全"), this);
    titleLabel->setStyleSheet(
        "font-weight: bold; font-size: 12px; color: #37474F;"
        "background: #E3F2FD; border-radius: 3px; padding: 2px 6px;");
    main->addWidget(titleLabel);

    // Pending count
    pendingLabel = new QLabel(this);
    pendingLabel->setStyleSheet("font-size: 11px; color: #616161; padding: 1px 4px;");
    main->addWidget(pendingLabel);

    // Splitter: queue table (top) + history log (bottom)
    auto* splitter = new QSplitter(Qt::Vertical, this);

    // Pending queue table
    queueTable = new QTableWidget(0, 4, splitter);
    queueTable->setHorizontalHeaderLabels({
        QString::fromUtf8("#"),
        QString::fromUtf8("类型"),
        QString::fromUtf8("读者/学号"),
        QString::fromUtf8("目标")
    });
    queueTable->horizontalHeader()->setStretchLastSection(true);
    queueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    queueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    queueTable->setMaximumHeight(160);

    // History log
    historyLog = new QTextEdit(splitter);
    historyLog->setReadOnly(true);
    historyLog->setStyleSheet("font-family: Consolas, monospace; font-size: 11px;"
                              "background: #FAFAFA;");
    historyLog->setPlaceholderText(QString::fromUtf8(
        "操作历史日志...\n"
        "执行借书/还书/预约座位等操作后，此处将显示消息队列处理记录"));

    splitter->addWidget(queueTable);
    splitter->addWidget(historyLog);
    main->addWidget(splitter, 1);

    refreshAll();
}

void MessageQueueTab::refreshAll()
{
    refreshQueueTable();
    refreshHistoryLog();
    emit queueRefreshed();
}

void MessageQueueTab::onQueueChanged()
{
    refreshQueueTable();
    emit queueRefreshed();
}

void MessageQueueTab::onMessageProcessed(int /*id*/, bool success, const QString& result)
{
    Q_UNUSED(success);
    // Append single message to log
    QString color = success ? "green" : "red";
    historyLog->append(QString::fromUtf8("<font color=%1>%2</font>").arg(color, result));
}

void MessageQueueTab::refreshQueueTable()
{
    auto& q = processor->getQueue();
    int n = q.size();
    queueTable->setRowCount(n);
    pendingLabel->setText(
        QString::fromUtf8("待处理: %1 条  |  历史: %2 条")
            .arg(n).arg(processor->getHistory().size()));

    int row = 0;
    q.traverse([&](OperationMessage* msg) {
        if (!msg) return;
        queueTable->setItem(row, 0, new QTableWidgetItem(QString::number(msg->id)));
        queueTable->setItem(row, 1, new QTableWidgetItem(msgTypeName(msg->type)));
        queueTable->setItem(row, 2, new QTableWidgetItem(msg->readerId));
        QString target;
        if (msg->type == MsgType::BorrowBook || msg->type == MsgType::ReturnBook
            || msg->type == MsgType::AddBook)
            target = msg->bookISBN;
        else
            target = QString::fromUtf8("座位(%1,%2)").arg(msg->seatRow).arg(msg->seatCol);
        queueTable->setItem(row, 3, new QTableWidgetItem(target));
        ++row;
    });
}

void MessageQueueTab::refreshHistoryLog()
{
    historyLog->clear();
    const auto& hist = processor->getHistory();
    if (hist.empty()) {
        historyLog->append(QString::fromUtf8(
            "<font color=gray>暂无操作记录。执行借书/还书/座位预约等操作后自动显示。</font>"));
        return;
    }
    for (auto* msg : hist) {
        if (!msg) continue;
        QString color = msg->succeeded ? "green" : "red";
        historyLog->append(QString::fromUtf8("<font color=%1>[#%2] %3</font>")
            .arg(color).arg(msg->id).arg(msg->resultText));
    }
}
