#include "RecommendTab.h"
#include "core/LibrarySystem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

RecommendTab::RecommendTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib) { setupUI(); }

void RecommendTab::setupUI()
{
    auto* main = new QVBoxLayout(this);

    auto* box = new QGroupBox(QString::fromUtf8("图书推荐 (基于共同借阅图)"), this);
    auto* hl = new QHBoxLayout(box);
    hl->addWidget(new QLabel(QString::fromUtf8("输入ISBN:")));
    isbnEdit = new QLineEdit(); isbnEdit->setPlaceholderText("978-7-xxx-xxxxx-x");
    hl->addWidget(isbnEdit);
    hl->addWidget(new QLabel(QString::fromUtf8("推荐数量:")));
    topSpin = new QSpinBox(); topSpin->setRange(1, 20); topSpin->setValue(5);
    hl->addWidget(topSpin);
    auto* btn = new QPushButton(QString::fromUtf8("获取推荐"));
    hl->addWidget(btn);
    hl->addStretch();
    main->addWidget(box);

    table = new QTableWidget(0, 3, this);
    table->setHorizontalHeaderLabels({ QString::fromUtf8("排名"), "ISBN", QString::fromUtf8("关联权重") });
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    main->addWidget(table);

    statusLabel = new QLabel(
        QString::fromUtf8("提示: 当同一读者借阅多本书时，这些书之间会建立关联边，权重为共同借阅次数"), this);
    statusLabel->setWordWrap(true);
    main->addWidget(statusLabel);

    connect(btn, &QPushButton::clicked, this, &RecommendTab::onRecommend);
    connect(isbnEdit, &QLineEdit::returnPressed, this, &RecommendTab::onRecommend);
}

void RecommendTab::onRecommend()
{
    QString isbn = isbnEdit->text().trimmed();
    if (isbn.isEmpty()) { QMessageBox::warning(this, QString::fromUtf8("输入"), QString::fromUtf8("请输入ISBN")); return; }
    auto recs = library->recommendBooks(isbn, topSpin->value());
    table->setRowCount(0);
    for (size_t i = 0; i < recs.size(); ++i) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        table->setItem(row, 1, new QTableWidgetItem(recs[i].first));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(recs[i].second)));
    }
    statusLabel->setText(recs.empty()
        ? QString::fromUtf8("无推荐结果 — 该书未被与其他图书共同借阅过")
        : QString::fromUtf8("找到 %1 条推荐").arg(recs.size()));
    emit graphRefreshed();
}
