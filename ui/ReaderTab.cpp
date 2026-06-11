#include "ReaderTab.h"
#include "core/LibrarySystem.h"
#include "model/Reader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

ReaderTab::ReaderTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib) { setupUI(); refreshTable(); }

void ReaderTab::setupUI()
{
    auto* main = new QVBoxLayout(this);

    auto* formBox = new QGroupBox(QString::fromUtf8("读者信息管理"), this);
    auto* formHL = new QHBoxLayout(formBox);
    auto* form = new QFormLayout();
    idEdit = new QLineEdit(); idEdit->setPlaceholderText(QString::fromUtf8("学号/工号"));
    form->addRow(QString::fromUtf8("学号/工号:"), idEdit);
    pwdEdit = new QLineEdit(); pwdEdit->setEchoMode(QLineEdit::Password); pwdEdit->setPlaceholderText(QString::fromUtf8("密码"));
    form->addRow(QString::fromUtf8("密码:"), pwdEdit);
    nameEdit = new QLineEdit();
    form->addRow(QString::fromUtf8("姓名:"), nameEdit);
    deptEdit = new QLineEdit();
    form->addRow(QString::fromUtf8("院系:"), deptEdit);
    formHL->addLayout(form);

    auto* btnLay = new QVBoxLayout();
    addBtn = new QPushButton(QString::fromUtf8("添加读者"));
    editBtn = new QPushButton(QString::fromUtf8("修改选中"));
    editBtn->setEnabled(false);
    auto* delBtn = new QPushButton(QString::fromUtf8("删除读者"));
    auto* clrBtn = new QPushButton(QString::fromUtf8("清空表单"));
    btnLay->addWidget(addBtn); btnLay->addWidget(editBtn);
    btnLay->addWidget(delBtn);
    btnLay->addWidget(clrBtn); btnLay->addStretch();
    formHL->addLayout(btnLay);
    main->addWidget(formBox);

    auto* srcHL = new QHBoxLayout();
    srcHL->addWidget(new QLabel(QString::fromUtf8("搜索学号:")));
    searchEdit = new QLineEdit();
    auto* srcBtn = new QPushButton(QString::fromUtf8("查找"));
    srcHL->addWidget(searchEdit); srcHL->addWidget(srcBtn);
    srcHL->addStretch();
    main->addLayout(srcHL);

    table = new QTableWidget(0, 6, this);
    table->setHorizontalHeaderLabels({
        QString::fromUtf8("学号"), QString::fromUtf8("姓名"), QString::fromUtf8("院系"),
        QString::fromUtf8("已借/上限"), QString::fromUtf8("管理员"), QString::fromUtf8("状态")
    });
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    main->addWidget(table);

    statusLabel = new QLabel(this);
    main->addWidget(statusLabel);

    connect(addBtn, &QPushButton::clicked, this, &ReaderTab::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &ReaderTab::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &ReaderTab::onDelete);
    connect(clrBtn, &QPushButton::clicked, this, [this]() {
        idEdit->clear(); pwdEdit->clear(); nameEdit->clear(); deptEdit->clear();
        editingId.clear();
        addBtn->setText(QString::fromUtf8("添加读者"));
    });
    connect(table, &QTableWidget::itemSelectionChanged, this, [this]() {
        editBtn->setEnabled(table->currentRow() >= 0);
    });
    connect(srcBtn, &QPushButton::clicked, this, &ReaderTab::onSearch);
    connect(searchEdit, &QLineEdit::returnPressed, this, &ReaderTab::onSearch);
}

void ReaderTab::onAdd()
{
    QString id = idEdit->text().trimmed();
    QString pwd = pwdEdit->text();
    QString name = nameEdit->text().trimmed();
    QString dept = deptEdit->text().trimmed();

    // 编辑模式下调用更新
    if (!editingId.isEmpty()) {
        library->updateReader(editingId, name, dept, pwd);
        editingId.clear();
        addBtn->setText(QString::fromUtf8("添加读者"));
        refreshTable();
        statusLabel->setText(QString::fromUtf8("已修改读者: %1").arg(name));
        return;
    }

    if (id.isEmpty() || pwd.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("学号、密码和姓名为必填"));
        return;
    }
    if (library->findReader(id)) {
        QMessageBox::warning(this, QString::fromUtf8("重复"), QString::fromUtf8("该学号已存在"));
        return;
    }
    library->addReader(new Reader(id.toStdString(), pwd.toStdString(),
                                  name.toStdString(), dept.toStdString(), false));
    refreshTable();
    statusLabel->setText(QString::fromUtf8("已添加读者: %1").arg(name));
}

void ReaderTab::onDelete()
{
    int row = table->currentRow();
    if (row < 0) { QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请选中要删除的读者")); return; }
    QString id = table->item(row, 0)->text();
    auto st = library->removeReader(id);
    if (st == Status::Invalid)
        QMessageBox::warning(this, QString::fromUtf8("无法删除"), QString::fromUtf8("该读者尚有未还图书"));
    else
        statusLabel->setText(QString::fromUtf8("已删除: %1").arg(id));
    refreshTable();
}

void ReaderTab::onEdit()
{
    int row = table->currentRow();
    if (row < 0) return;

    editingId = table->item(row, 0)->text();
    idEdit->setText(editingId);
    idEdit->setReadOnly(true);
    nameEdit->setText(table->item(row, 1)->text());
    // 院系列在索引2
    QStringList parts = table->item(row, 3)->text().split("/");
    deptEdit->setText(table->item(row, 2)->text());
    pwdEdit->clear();
    addBtn->setText(QString::fromUtf8("保存修改"));
    statusLabel->setText(QString::fromUtf8("正在编辑读者: %1").arg(editingId));
}

void ReaderTab::onSearch()
{
    QString id = searchEdit->text().trimmed();
    if (id.isEmpty()) { refreshTable(); return; }
    Reader* r = library->findReader(id);
    table->setRowCount(0);
    if (r) {
        table->insertRow(0);
        table->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(r->id)));
        table->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(r->name)));
        table->setItem(0, 2, new QTableWidgetItem(QString::fromStdString(r->department)));
        table->setItem(0, 3, new QTableWidgetItem(QString("%1/%2").arg(r->currentBorrow).arg(r->maxBorrow)));
        table->setItem(0, 4, new QTableWidgetItem(r->isAdmin ? QString::fromUtf8("是") : ""));
        table->setItem(0, 5, new QTableWidgetItem(r->active ? QString::fromUtf8("正常") : QString::fromUtf8("已停用")));
    }
    statusLabel->setText(r ? QString::fromUtf8("找到") : QString::fromUtf8("未找到"));
}

void ReaderTab::refreshTable()
{
    table->setRowCount(0);
    library->getReaderList().traverse([&](Reader*& r) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r->id)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r->name)));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(r->department)));
        table->setItem(row, 3, new QTableWidgetItem(QString("%1/%2").arg(r->currentBorrow).arg(r->maxBorrow)));
        table->setItem(row, 4, new QTableWidgetItem(r->isAdmin ? QString::fromUtf8("是") : ""));
        table->setItem(row, 5, new QTableWidgetItem(r->active ? QString::fromUtf8("正常") : QString::fromUtf8("已停用")));
    });
    statusLabel->setText(QString::fromUtf8("共 %1 位读者").arg(table->rowCount()));
}
