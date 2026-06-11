#include "HotRankTab.h"
#include "core/LibrarySystem.h"
#include "model/Book.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>

HotRankTab::HotRankTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib) { setupUI(); refreshRanking(); }

void HotRankTab::setupUI()
{
    auto* main = new QVBoxLayout(this);

    auto* topBox = new QGroupBox(QString::fromUtf8("热门图书排行榜"), this);
    auto* topHL = new QHBoxLayout(topBox);
    topHL->addWidget(new QLabel("TOP"));
    topSpin = new QSpinBox(); topSpin->setRange(1, 50); topSpin->setValue(10);
    topHL->addWidget(topSpin);
    auto* refBtn = new QPushButton(QString::fromUtf8("刷新排行"));
    topHL->addWidget(refBtn);
    topHL->addStretch();
    main->addWidget(topBox);

    table = new QTableWidget(0, 5, this);
    table->setHorizontalHeaderLabels({
        QString::fromUtf8("排名"), "ISBN", QString::fromUtf8("书名"),
        QString::fromUtf8("作者"), QString::fromUtf8("借阅次数")
    });
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    main->addWidget(table);

    statusLabel = new QLabel(QString::fromUtf8("底层数据结构: 最大堆 (MaxHeap) — 按借阅次数自动调整"), this);
    main->addWidget(statusLabel);

    connect(refBtn, &QPushButton::clicked, this, &HotRankTab::refreshRanking);
}

void HotRankTab::refreshRanking()
{
    int n = topSpin->value();
    auto hot = library->getHotBooks(n);
    table->setRowCount(0);
    for (size_t i = 0; i < hot.size(); ++i) {
        auto* b = hot[i];
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b->isbn)));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b->title)));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b->author)));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(b->borrowCount)));
    }
    statusLabel->setText(QString::fromUtf8("底层数据结构: 最大堆 (MaxHeap) — 前 %1 名已显示").arg(hot.size()));
}
