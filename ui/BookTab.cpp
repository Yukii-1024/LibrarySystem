#include "BookTab.h"
#include "core/LibrarySystem.h"
#include "model/Book.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

BookTab::BookTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib)
{
    setupUI();
    refreshTable();
}

void BookTab::setupUI()
{
    auto* main = new QVBoxLayout(this);

    // --- Top form ---
    formBox = new QGroupBox(QString::fromUtf8("图书信息管理"), this);
    auto* formHL = new QHBoxLayout(formBox);

    auto* form = new QFormLayout();
    isbnEdit = new QLineEdit(); isbnEdit->setPlaceholderText("978-7-xxx-xxxxx-x");
    form->addRow("ISBN:", isbnEdit);
    callNumEdit = new QLineEdit(); callNumEdit->setPlaceholderText("TP311.1/C01");
    form->addRow(QString::fromUtf8("索书号:"), callNumEdit);
    titleEdit = new QLineEdit();
    form->addRow(QString::fromUtf8("书名:"), titleEdit);
    authorEdit = new QLineEdit();
    form->addRow(QString::fromUtf8("作者:"), authorEdit);
    pubEdit = new QLineEdit();
    form->addRow(QString::fromUtf8("出版社:"), pubEdit);
    stockSpin = new QSpinBox(); stockSpin->setRange(1, 100); stockSpin->setValue(3);
    form->addRow(QString::fromUtf8("库存:"), stockSpin);
    formHL->addLayout(form);

    auto* btnLay = new QVBoxLayout();
    addBtn = new QPushButton(QString::fromUtf8("添加图书"));
    editBtn = new QPushButton(QString::fromUtf8("修改选中"));
    editBtn->setEnabled(false);
    auto* delBtn = new QPushButton(QString::fromUtf8("删除选中"));
    auto* clrBtn = new QPushButton(QString::fromUtf8("清空表单"));
    btnLay->addWidget(addBtn); btnLay->addWidget(editBtn);
    btnLay->addWidget(delBtn);
    btnLay->addWidget(clrBtn); btnLay->addStretch();
    formHL->addLayout(btnLay);

    // --- Search ---
    auto* srcBox = new QGroupBox(QString::fromUtf8("搜索"), this);
    auto* srcLay = new QHBoxLayout(srcBox);
    srcLay->addWidget(new QLabel("ISBN:"));
    searchEdit = new QLineEdit();
    auto* srcBtn = new QPushButton(QString::fromUtf8("查找"));
    srcLay->addWidget(searchEdit); srcLay->addWidget(srcBtn);
    srcLay->addWidget(new QLabel(QString::fromUtf8("  索书号范围:")));
    rangeLowEdit = new QLineEdit(); rangeLowEdit->setPlaceholderText("低");
    rangeHighEdit = new QLineEdit(); rangeHighEdit->setPlaceholderText("高");
    auto* rangeBtn = new QPushButton(QString::fromUtf8("范围查询"));
    srcLay->addWidget(rangeLowEdit); srcLay->addWidget(new QLabel("-"));
    srcLay->addWidget(rangeHighEdit); srcLay->addWidget(rangeBtn);

    main->addWidget(formBox);
    main->addWidget(srcBox);

    // --- Table ---
    table = new QTableWidget(0, 8, this);
    table->setHorizontalHeaderLabels({
        "ISBN", QString::fromUtf8("索书号"), QString::fromUtf8("书名"),
        QString::fromUtf8("作者"), QString::fromUtf8("出版社"),
        QString::fromUtf8("总库存"), QString::fromUtf8("可借"), QString::fromUtf8("借阅次数")
    });
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    main->addWidget(table);

    statusLabel = new QLabel(this);
    main->addWidget(statusLabel);

    // Connections
    connect(addBtn, &QPushButton::clicked, this, &BookTab::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &BookTab::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &BookTab::onDelete);
    connect(clrBtn, &QPushButton::clicked, this, [this]() {
        isbnEdit->clear(); isbnEdit->setReadOnly(false);
        callNumEdit->clear(); titleEdit->clear();
        authorEdit->clear(); pubEdit->clear(); stockSpin->setValue(3);
        editingISBN.clear();
        addBtn->setText(QString::fromUtf8("添加图书"));
    });
    connect(srcBtn, &QPushButton::clicked, this, &BookTab::onSearch);
    connect(rangeBtn, &QPushButton::clicked, this, &BookTab::onRangeQuery);
    connect(searchEdit, &QLineEdit::returnPressed, this, &BookTab::onSearch);
    connect(table, &QTableWidget::itemSelectionChanged, this, [this]() {
        editBtn->setEnabled(table->currentRow() >= 0);
    });
}

void BookTab::onAdd()
{
    QString isbn = isbnEdit->text().trimmed();
    QString callNum = callNumEdit->text().trimmed();
    QString title = titleEdit->text().trimmed();

    // 编辑模式下调用更新
    if (!editingISBN.isEmpty()) {
        library->updateBook(editingISBN, title,
            authorEdit->text().trimmed(), pubEdit->text().trimmed(), stockSpin->value());
        editingISBN.clear();
        addBtn->setText(QString::fromUtf8("添加图书"));
        refreshTable();
        statusLabel->setText(QString::fromUtf8("已修改: 《%1》").arg(title));
        return;
    }

    if (isbn.isEmpty() || callNum.isEmpty() || title.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("ISBN、索书号和书名为必填"));
        return;
    }
    auto* b = new Book(isbn.toStdString(), callNum.toStdString(), title.toStdString(),
                       authorEdit->text().trimmed().toStdString(),
                       pubEdit->text().trimmed().toStdString(), stockSpin->value());
    library->addBook(b);
    refreshTable();
    statusLabel->setText(QString::fromUtf8("已添加: 《%1》").arg(title));
}

void BookTab::onDelete()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("操作提示"), QString::fromUtf8("请先选中要删除的图书"));
        return;
    }
    QString isbn = table->item(row, 0)->text();
    library->removeBook(isbn);
    refreshTable();
    statusLabel->setText(QString::fromUtf8("已删除 ISBN: %1").arg(isbn));
}

void BookTab::onEdit()
{
    int row = table->currentRow();
    if (row < 0) return;

    editingISBN = table->item(row, 0)->text();
    callNumEdit->setText(table->item(row, 1)->text());
    titleEdit->setText(table->item(row, 2)->text());
    authorEdit->setText(table->item(row, 3)->text());
    pubEdit->setText(table->item(row, 4)->text());
    stockSpin->setValue(table->item(row, 5)->text().toInt());

    isbnEdit->setText(editingISBN);
    isbnEdit->setReadOnly(true);
    addBtn->setText(QString::fromUtf8("保存修改"));
    statusLabel->setText(QString::fromUtf8("正在编辑: %1").arg(editingISBN));
}

void BookTab::onSearch()
{
    QString isbn = searchEdit->text().trimmed();
    if (isbn.isEmpty()) { refreshTable(); return; }
    Book* b = library->findByISBN(isbn);
    table->setRowCount(0);
    if (b) {
        table->insertRow(0);
        table->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(b->isbn)));
        table->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(b->callNumber)));
        table->setItem(0, 2, new QTableWidgetItem(QString::fromStdString(b->title)));
        table->setItem(0, 3, new QTableWidgetItem(QString::fromStdString(b->author)));
        table->setItem(0, 4, new QTableWidgetItem(QString::fromStdString(b->publisher)));
        table->setItem(0, 5, new QTableWidgetItem(QString::number(b->totalStock)));
        table->setItem(0, 6, new QTableWidgetItem(QString::number(b->availableStock)));
        table->setItem(0, 7, new QTableWidgetItem(QString::number(b->borrowCount)));
    }
    statusLabel->setText(b ? QString::fromUtf8("找到 1 条记录") : QString::fromUtf8("未找到"));
}

void BookTab::onRangeQuery()
{
    QString low = rangeLowEdit->text().trimmed();
    QString high = rangeHighEdit->text().trimmed();
    if (low.isEmpty()) low = "A";
    if (high.isEmpty()) high = "ZZZZ";
    auto results = library->rangeQuery(low, high);
    table->setRowCount(0);
    for (size_t i = 0; i < results.size(); ++i) {
        auto* b = results[i];
        table->insertRow(static_cast<int>(i));
        table->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::fromStdString(b->isbn)));
        table->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::fromStdString(b->callNumber)));
        table->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::fromStdString(b->title)));
        table->setItem(static_cast<int>(i), 3, new QTableWidgetItem(QString::fromStdString(b->author)));
        table->setItem(static_cast<int>(i), 4, new QTableWidgetItem(QString::fromStdString(b->publisher)));
        table->setItem(static_cast<int>(i), 5, new QTableWidgetItem(QString::number(b->totalStock)));
        table->setItem(static_cast<int>(i), 6, new QTableWidgetItem(QString::number(b->availableStock)));
        table->setItem(static_cast<int>(i), 7, new QTableWidgetItem(QString::number(b->borrowCount)));
    }
    statusLabel->setText(QString::fromUtf8("范围 [%1, %2] 查到 %3 条").arg(low, high).arg(results.size()));
}

void BookTab::refreshTable()
{
    table->setRowCount(0);
    auto books = library->getAllBooks();
    for (size_t i = 0; i < books.size(); ++i) {
        auto* b = books[i];
        int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(b->isbn)));
        table->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(b->callNumber)));
        table->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(b->title)));
        table->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(b->author)));
        table->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(b->publisher)));
        table->setItem(r, 5, new QTableWidgetItem(QString::number(b->totalStock)));
        table->setItem(r, 6, new QTableWidgetItem(QString::number(b->availableStock)));
        table->setItem(r, 7, new QTableWidgetItem(QString::number(b->borrowCount)));
    }
    statusLabel->setText(QString::fromUtf8("共 %1 本图书").arg(books.size()));
}

void BookTab::setAdminMode(bool admin)
{
    formBox->setVisible(admin);
}
