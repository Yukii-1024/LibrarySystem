#include "BorrowTab.h"
#include "core/LibrarySystem.h"
#include "model/Book.h"
#include "model/Reader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>

BorrowTab::BorrowTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib) { setupUI(); refreshTable(); }

void BorrowTab::setupUI()
{
    auto* main = new QVBoxLayout(this);

    // Borrow / Return bar
    auto* actBox = new QGroupBox(QString::fromUtf8("借阅操作"), this);
    auto* actHL = new QHBoxLayout(actBox);
    actHL->addWidget(new QLabel(QString::fromUtf8("读者ID:")));
    readerEdit = new QLineEdit(); readerEdit->setPlaceholderText("2024001");
    actHL->addWidget(readerEdit);
    actHL->addWidget(new QLabel(QString::fromUtf8("ISBN:")));
    isbnEdit = new QLineEdit(); isbnEdit->setPlaceholderText("978-7-xxx-xxxxx-x");
    actHL->addWidget(isbnEdit);
    auto* borrowBtn = new QPushButton(QString::fromUtf8("借书"));
    auto* returnBtn = new QPushButton(QString::fromUtf8("还书"));
    auto* undoBtn = new QPushButton(QString::fromUtf8("↩ 撤销"));
    actHL->addWidget(borrowBtn); actHL->addWidget(returnBtn); actHL->addWidget(undoBtn);
    main->addWidget(actBox);

    // Status
    statusLabel = new QLabel(QString::fromUtf8("输入读者ID和ISBN查看状态"), this);
    statusLabel->setStyleSheet("font-weight: bold; padding: 4px;");
    main->addWidget(statusLabel);

    // Reserve
    auto* resBox = new QGroupBox(QString::fromUtf8("预约排队 (使用上方读者ID)"), this);
    auto* resHL = new QHBoxLayout(resBox);
    resHL->addWidget(new QLabel(QString::fromUtf8("预约ISBN:")));
    resIsbnEdit = new QLineEdit();
    resIsbnEdit->setPlaceholderText("ISBN");
    resHL->addWidget(resIsbnEdit);
    auto* resBtn = new QPushButton(QString::fromUtf8("加入预约队列"));
    auto* showQBtn = new QPushButton(QString::fromUtf8("查看排队情况"));
    resHL->addWidget(resBtn); resHL->addWidget(showQBtn);
    resHL->addStretch();
    main->addWidget(resBox);

    // Table
    table = new QTableWidget(0, 6, this);
    table->setHorizontalHeaderLabels({
        QString::fromUtf8("记录ID"), QString::fromUtf8("读者ID"), "ISBN",
        QString::fromUtf8("借出时间"), QString::fromUtf8("归还时间"), QString::fromUtf8("状态")
    });
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    main->addWidget(table);

    connect(borrowBtn, &QPushButton::clicked, this, &BorrowTab::onBorrow);
    connect(returnBtn, &QPushButton::clicked, this, &BorrowTab::onReturn);
    connect(undoBtn, &QPushButton::clicked, this, &BorrowTab::onUndo);
    connect(resBtn, &QPushButton::clicked, this, &BorrowTab::onReserve);
    connect(showQBtn, &QPushButton::clicked, this, &BorrowTab::onShowQueues);
    connect(readerEdit, &QLineEdit::textChanged, this, &BorrowTab::checkStatus);
    connect(isbnEdit, &QLineEdit::textChanged, this, &BorrowTab::checkStatus);
}

void BorrowTab::onBorrow()
{
    QString rid = readerEdit->text().trimmed();
    QString isbn = isbnEdit->text().trimmed();
    if (rid.isEmpty() || isbn.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请填写读者ID和ISBN"));
        return;
    }
    Status st = library->borrowBook(rid, isbn);
    if (st == Status::NotFound) QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("读者或图书未找到"));
    else if (st == Status::Full) QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("库存不足或读者已达借阅上限"));
    else statusLabel->setText(QString::fromUtf8("借书成功！"));
    refreshTable();
}

void BorrowTab::onReturn()
{
    QString rid = readerEdit->text().trimmed();
    QString isbn = isbnEdit->text().trimmed();
    if (rid.isEmpty() || isbn.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请填写读者ID和ISBN"));
        return;
    }
    library->returnBook(rid, isbn);
    statusLabel->setText(QString::fromUtf8("还书成功！"));
    refreshTable();
}

void BorrowTab::onUndo()
{
    Status st = library->undoLastOperation();
    if (st == Status::Empty) QMessageBox::information(this, QString::fromUtf8("撤销"), QString::fromUtf8("没有可撤销的操作"));
    else statusLabel->setText(QString::fromUtf8("已撤销上次操作"));
    refreshTable();
}

void BorrowTab::onReserve()
{
    QString rid = readerEdit->text().trimmed();
    QString isbn = resIsbnEdit->text().trimmed();
    if (rid.isEmpty() || isbn.isEmpty()) { QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("请填写读者ID和ISBN")); return; }
    Status st = library->enqueueReservation(rid, isbn);
    if (st == Status::NotFound) QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("图书未找到"));
    else if (st == Status::Invalid) QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("该书尚有库存，无需排队"));
    else statusLabel->setText(QString::fromUtf8("已加入预约队列"));
    refreshTable();
}

void BorrowTab::onShowQueues()
{
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("预约排队情况"));
    dlg.resize(500, 350);

    auto* layout = new QVBoxLayout(&dlg);
    auto* label = new QLabel(QString::fromUtf8("以下为所有图书的预约队列："), &dlg);
    layout->addWidget(label);

    auto* qTable = new QTableWidget(0, 3, &dlg);
    qTable->setHorizontalHeaderLabels({
        "ISBN", QString::fromUtf8("书名"), QString::fromUtf8("排队读者数")
    });
    qTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qTable->horizontalHeader()->setStretchLastSection(true);
    qTable->setAlternatingRowColors(true);
    layout->addWidget(qTable);

    const auto& queues = library->getReservationQueues();
    for (const auto& pair : queues) {
        const std::string& isbn = pair.first;
        const Queue<QString>& q = pair.second;
        int row = qTable->rowCount();
        qTable->insertRow(row);
        qTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(isbn)));

        Book* b = library->findByISBN(QString::fromStdString(isbn));
        qTable->setItem(row, 1, new QTableWidgetItem(
            b ? QString::fromStdString(b->title) : QString::fromUtf8("未知")));
        qTable->setItem(row, 2, new QTableWidgetItem(QString::number(q.size())));
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    buttons->button(QDialogButtonBox::Close)->setText(QString::fromUtf8("关闭"));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.exec();
}

void BorrowTab::checkStatus()
{
    QString rid = readerEdit->text().trimmed();
    QString isbn = isbnEdit->text().trimmed();
    if (rid.isEmpty() || isbn.isEmpty()) {
        statusLabel->setText(QString::fromUtf8("输入读者ID和ISBN查看状态"));
        return;
    }
    Reader* r = library->findReader(rid);
    Book* b = library->findByISBN(isbn);
    if (!r) statusLabel->setText(QString::fromUtf8("读者不存在"));
    else if (!b) statusLabel->setText(QString::fromUtf8("图书不存在"));
    else if (!library->canBorrow(rid, isbn))
        statusLabel->setText(QString::fromUtf8("⚠ 不可借：库存不足或已达上限"));
    else
        statusLabel->setText(QString::fromUtf8("✓ 可以借阅"));
}

void BorrowTab::refreshTable()
{
    table->setRowCount(0);
    library->getBorrowRecords().traverse([&](BorrowRecord*& br) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(br->recordId)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(br->readerId)));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(br->bookISBN)));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(br->borrowTime)));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(br->returnTime)));
        table->setItem(row, 5, new QTableWidgetItem(br->returned ? QString::fromUtf8("已还") : QString::fromUtf8("借出中")));
    });
}
