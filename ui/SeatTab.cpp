#include "SeatTab.h"
#include "core/LibrarySystem.h"
#include "model/Seat.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QBrush>

SeatTab::SeatTab(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib) { setupUI(); refreshGrid(); }

void SeatTab::setCurrentUser(const QString& id, bool admin)
{
    currentUserId = id;
    isAdmin = admin;
    userLabel->setText(QString::fromUtf8("当前用户: %1%2")
        .arg(id, admin ? QString::fromUtf8(" (管理员·可预约多个座位)") : QString::fromUtf8(" (同一时段仅一个座位)")));
}

void SeatTab::setupUI()
{
    auto* main = new QVBoxLayout(this);

    // User info bar
    userLabel = new QLabel(this);
    userLabel->setStyleSheet("font-size: 12px; color: #1565C0; font-weight: bold; padding: 2px 6px;");
    main->addWidget(userLabel);

    // Form
    auto* formBox = new QGroupBox(QString::fromUtf8("座位预约"), this);
    auto* formHL = new QHBoxLayout(formBox);
    auto* form = new QFormLayout();
    rowSpin = new QSpinBox(); rowSpin->setRange(0, 9);
    colSpin = new QSpinBox(); colSpin->setRange(0, 9);
    form->addRow(QString::fromUtf8("行 (0-9):"), rowSpin);
    form->addRow(QString::fromUtf8("列 (0-9):"), colSpin);
    startEdit = new QDateTimeEdit(QDateTime::currentDateTime()); startEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    endEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(7200)); endEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    form->addRow(QString::fromUtf8("开始:"), startEdit);
    form->addRow(QString::fromUtf8("结束:"), endEdit);
    formHL->addLayout(form);

    auto* btnLay = new QVBoxLayout();
    auto* resBtn = new QPushButton(QString::fromUtf8("预约座位"));
    auto* relBtn = new QPushButton(QString::fromUtf8("释放座位"));
    auto* refBtn = new QPushButton(QString::fromUtf8("刷新"));
    btnLay->addWidget(resBtn); btnLay->addWidget(relBtn); btnLay->addWidget(refBtn); btnLay->addStretch();
    formHL->addLayout(btnLay);
    main->addWidget(formBox);

    statusLabel = new QLabel(this);
    main->addWidget(statusLabel);

    // 10x10 Grid
    grid = new QTableWidget(10, 10, this);
    grid->horizontalHeader()->setVisible(false);
    grid->verticalHeader()->setVisible(false);
    grid->setSelectionMode(QAbstractItemView::SingleSelection);
    grid->setMinimumSize(500, 400);
    for (int r = 0; r < 10; ++r) {
        grid->setRowHeight(r, 40);
        grid->setColumnWidth(r, 50);
    }
    main->addWidget(grid);

    connect(resBtn, &QPushButton::clicked, this, &SeatTab::onReserve);
    connect(relBtn, &QPushButton::clicked, this, &SeatTab::onRelease);
    connect(refBtn, &QPushButton::clicked, this, &SeatTab::refreshGrid);
    connect(grid, &QTableWidget::cellClicked, this, &SeatTab::onCellClicked);
}

bool SeatTab::isReaderSeatConflict(const QString& readerId) const
{
    auto seatGrid = library->getSeatGrid(10, 10);
    for (int r = 0; r < 10; ++r)
        for (int c = 0; c < 10; ++c)
            if (seatGrid[r][c].status == SeatStatus::Occupied
                && QString::fromStdString(seatGrid[r][c].readerId) == readerId)
                return true;
    return false;
}

void SeatTab::onReserve()
{
    if (currentUserId.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("未登录，无法预约"));
        return;
    }

    int r = rowSpin->value(), c = colSpin->value();

    // Non-admin: check one-seat-per-reader rule
    if (!isAdmin && isReaderSeatConflict(currentUserId)) {
        QMessageBox::warning(this, QString::fromUtf8("预约限制"),
            QString::fromUtf8("你已预约了一个座位，同一时段只能预约一个位置。\n请先释放现有座位后再预约新座位。"));
        return;
    }

    Seat* s = library->getSeat(r, c);
    if (s && !s->isFree()) {
        QMessageBox::warning(this, QString::fromUtf8("座位已被占用"), QString::fromUtf8("请选择其他座位"));
        return;
    }

    library->reserveSeat(r, c, currentUserId,
        startEdit->dateTime().toString("yyyy-MM-dd hh:mm"),
        endEdit->dateTime().toString("yyyy-MM-dd hh:mm"));
    statusLabel->setText(QString::fromUtf8("座位(%1,%2)预约成功").arg(r).arg(c));
    refreshGrid();
}

void SeatTab::onRelease()
{
    if (currentUserId.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("未登录"));
        return;
    }

    if (!isAdmin) {
        // Reader: auto-find and release their only seat
        auto seatGrid = library->getSeatGrid(10, 10);
        for (int r = 0; r < 10; ++r) {
            for (int c = 0; c < 10; ++c) {
                const Seat& s = seatGrid[r][c];
                if (s.status == SeatStatus::Occupied
                    && QString::fromStdString(s.readerId) == currentUserId) {
                    library->releaseSeat(r, c);
                    statusLabel->setText(QString::fromUtf8("座位(%1,%2)已释放").arg(r).arg(c));
                    refreshGrid();
                    return;
                }
            }
        }
        QMessageBox::information(this, QString::fromUtf8("提示"),
            QString::fromUtf8("你没有预约任何座位"));
        return;
    }

    // Admin: release selected seat
    int r = rowSpin->value(), c = colSpin->value();
    Seat* s = library->getSeat(r, c);
    if (!s || s->isFree()) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("该座位未被占用"));
        return;
    }

    library->releaseSeat(r, c);
    statusLabel->setText(QString::fromUtf8("座位(%1,%2)已释放").arg(r).arg(c));
    refreshGrid();
}

void SeatTab::onCellClicked(int row, int col)
{
    rowSpin->setValue(row);
    colSpin->setValue(col);
}

void SeatTab::refreshGrid()
{
    auto seatGrid = library->getSeatGrid(10, 10);
    bool hasOwn = false;
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            const Seat& s = seatGrid[r][c];
            auto* item = new QTableWidgetItem(QString("(%1,%2)").arg(r).arg(c));
            if (s.status == SeatStatus::Occupied) {
                bool isOwn = (QString::fromStdString(s.readerId) == currentUserId);
                if (isOwn) {
                    item->setBackground(QBrush(QColor(255, 200, 100))); // orange = own
                    hasOwn = true;
                } else {
                    item->setBackground(QBrush(QColor(255, 150, 150))); // red = occupied by others
                }
                item->setToolTip(QString::fromUtf8("占用: %1\n%2 - %3")
                    .arg(QString::fromStdString(s.readerId),
                         QString::fromStdString(s.startTime),
                         QString::fromStdString(s.endTime)));
            } else {
                item->setBackground(QBrush(QColor(150, 255, 150))); // green = free
            }
            item->setTextAlignment(Qt::AlignCenter);
            grid->setItem(r, c, item);
        }
    }
    QString tip = QString::fromUtf8("座位分布图 (绿=空闲, 红=他人占用, 橙=你的座位)");
    if (!isAdmin && hasOwn)
        tip += QString::fromUtf8("  [你已预约座位]");
    statusLabel->setText(tip);
    emit gridRefreshed();
}
