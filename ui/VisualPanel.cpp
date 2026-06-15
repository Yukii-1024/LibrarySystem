#include "VisualPanel.h"
#include "core/LibrarySystem.h"
#include "model/Book.h"
#include "model/Reader.h"
#include "model/Seat.h"
#include <QVBoxLayout>
#include <QSplitter>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QTextEdit>
#include <QScrollArea>

// ── helpers ────────────────────────────────────────────────

static QString dsKey(const QString& ds) {
    if (ds.contains("BST") || ds.contains(QString::fromUtf8("树")))   return "bst";
    if (ds.contains(QString::fromUtf8("哈希")))                      return "hash";
    if (ds.contains(QString::fromUtf8("堆")))                        return "heap";
    if (ds.contains(QString::fromUtf8("图")))                        return "graph";
    if (ds.contains(QString::fromUtf8("栈")))                        return "stack";
    if (ds.contains(QString::fromUtf8("队列")))                      return "queue";
    if (ds.contains(QString::fromUtf8("矩阵")))                      return "matrix";
    return QString();
}

static QString dsTitle(const QString& key) {
    static const QMap<QString, QString> titles = {
        {"bst",    QString::fromUtf8("BST / AVL树  —  索书号索引")},
        {"hash",   QString::fromUtf8("哈希表  —  读者学号索引")},
        {"heap",   QString::fromUtf8("最大堆  —  热门图书排行")},
        {"graph",  QString::fromUtf8("图 (邻接表)  —  图书推荐网络")},
        {"stack",  QString::fromUtf8("栈  —  操作撤销历史")},
        {"queue",  QString::fromUtf8("队列  —  预约排队")},
        {"matrix", QString::fromUtf8("稀疏矩阵  —  座位分布")}
    };
    return titles.value(key, key);
}


// ── VisualPanel ────────────────────────────────────────────

VisualPanel::VisualPanel(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib)
{
    setupUI();
}

void VisualPanel::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    // ── Top: compact operation info ──
    auto* topFrame = new QWidget(this);
    topFrame->setStyleSheet("background: #E8F0FE; border-radius: 6px; padding: 1px;");
    topFrame->setFixedHeight(52);
    auto* topLay = new QVBoxLayout(topFrame);
    topLay->setContentsMargins(8, 4, 8, 4);
    topLay->setSpacing(1);

    operationLabel = new QLabel(QString::fromUtf8("当前操作：无"), topFrame);
    operationLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #1565C0; background: transparent;");
    topLay->addWidget(operationLabel);

    descriptionText = new QTextEdit(topFrame);
    descriptionText->setReadOnly(true);
    descriptionText->setFrameShape(QFrame::NoFrame);
    descriptionText->setStyleSheet("font-size: 11px; color: #455A64; background: transparent;");
    descriptionText->setPlaceholderText(QString::fromUtf8("执行操作后此处显示描述"));
    descriptionText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    descriptionText->setFixedHeight(28);
    topLay->addWidget(descriptionText);

    layout->addWidget(topFrame);

    // ── Bottom: dynamic splitter ──
    dsSplitter = new QSplitter(Qt::Vertical, this);
    dsSplitter->setChildrenCollapsible(false);
    dsSplitter->setHandleWidth(3);
    dsSplitter->setStyleSheet(
        "QSplitter::handle { background: #BBDEFB; }"
    );
    layout->addWidget(dsSplitter, 1);

    // ── Create all scenes & views once ──
    bstScene    = new QGraphicsScene(this);
    hashScene   = new QGraphicsScene(this);
    heapScene   = new QGraphicsScene(this);
    graphScene  = new QGraphicsScene(this);
    stackScene  = new QGraphicsScene(this);
    queueScene  = new QGraphicsScene(this);
    matrixScene = new QGraphicsScene(this);

    bstView    = createViewForScene(bstScene);
    hashView   = createViewForScene(hashScene);
    heapView   = createViewForScene(heapScene);
    graphView  = createViewForScene(graphScene);
    stackView  = createViewForScene(stackScene);
    queueView  = createViewForScene(queueScene);
    matrixView = createViewForScene(matrixScene);

    // Pre-build scrollable panes (hidden until needed)
    dsPanes["bst"]    = createDSPane(dsTitle("bst"),    bstView);
    dsPanes["hash"]   = createDSPane(dsTitle("hash"),   hashView);
    dsPanes["heap"]   = createDSPane(dsTitle("heap"),   heapView);
    dsPanes["graph"]  = createDSPane(dsTitle("graph"),  graphView);
    dsPanes["stack"]  = createDSPane(dsTitle("stack"),  stackView);
    dsPanes["queue"]  = createDSPane(dsTitle("queue"),  queueView);
    dsPanes["matrix"] = createDSPane(dsTitle("matrix"), matrixView);
    for (auto* pane : dsPanes)
        pane->hide();

    // Welcome
    welcomePane = new QWidget(this);
    auto* wl = new QVBoxLayout(welcomePane);
    auto* welcomeLabel = new QLabel(
        QString::fromUtf8("操作左侧功能标签页后\n此处将显示涉及的数据结构状态"), welcomePane);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("color: #9E9E9E; font-size: 15px;");
    wl->addWidget(welcomeLabel);
    dsSplitter->addWidget(welcomePane);

    refreshAll();
}

QGraphicsView* VisualPanel::createViewForScene(QGraphicsScene* scene)
{
    auto* view = new QGraphicsView(this);
    view->setRenderHints(QPainter::Antialiasing);
    view->setScene(scene);
    view->setMinimumHeight(80);
    view->setStyleSheet("border: none; background: #FAFAFA;");
    view->setFrameShape(QFrame::NoFrame);
    return view;
}

QWidget* VisualPanel::createDSPane(const QString& title, QGraphicsView* view)
{
    auto* pane = new QWidget(this);
    auto* lay = new QVBoxLayout(pane);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Compact title bar
    auto* bar = new QLabel(title, pane);
    bar->setStyleSheet(
        "font-weight: bold; font-size: 12px; color: #37474F;"
        "background: #E3F2FD; border-radius: 3px; padding: 2px 6px;");
    bar->setFixedHeight(20);
    lay->addWidget(bar);

    // Scrollable view area
    auto* scroll = new QScrollArea(pane);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(view);
    lay->addWidget(scroll, 1);

    return pane;
}

// ── show / hide panes ──────────────────────────────────────

void VisualPanel::showDSPanes(const QStringList& dsUsed)
{
    while (dsSplitter->count() > 0)
        dsSplitter->widget(0)->setParent(nullptr);

    if (dsUsed.isEmpty()) {
        dsSplitter->addWidget(welcomePane);
        return;
    }

    QStringList keys;
    for (const auto& ds : dsUsed) {
        QString k = dsKey(ds);
        if (!k.isEmpty() && !keys.contains(k))
            keys.append(k);
    }

    if (keys.isEmpty()) {
        dsSplitter->addWidget(welcomePane);
        return;
    }

    for (const auto& k : keys) {
        if (dsPanes.contains(k)) {
            auto* pane = dsPanes[k];
            pane->show();
            dsSplitter->addWidget(pane);

            if      (k == "bst")    refreshBST();
            else if (k == "hash")   refreshHashTable();
            else if (k == "heap")   refreshHeap();
            else if (k == "graph")  refreshGraph();
            else if (k == "stack")  refreshStack();
            else if (k == "queue")  refreshQueue();
            else if (k == "matrix") refreshMatrix();
        }
    }

    // Even split
    QList<int> sizes;
    int h = dsSplitter->height();
    if (h < 10) h = 400;
    int each = h / keys.size();
    for (int i = 0; i < keys.size(); ++i)
        sizes.append(each);
    dsSplitter->setSizes(sizes);
}

// ── slot ───────────────────────────────────────────────────

void VisualPanel::onOperation(const QString& opName,
                               const QStringList& dsUsed,
                               const QString& description)
{
    showOperation(opName, dsUsed, description);
    showDSPanes(dsUsed);
}

void VisualPanel::showOperation(const QString& opName,
                                 const QStringList& dsUsed,
                                 const QString& description)
{
    operationLabel->setText(QString::fromUtf8("当前操作：%1").arg(opName));
    descriptionText->setPlainText(
        QString::fromUtf8("涉及：%1  —  %2").arg(dsUsed.join(", "), description));
}

// ── refresh implementations ─────────────────────────────────

void VisualPanel::refreshBST()
{
    bstScene->clear();
    if (!library) return;
    auto& bst = library->getBookBST();
    int y = 4;
    auto add = [&](const QString& t, const QString& s = "text") {
        auto* item = bstScene->addText(t);
        QFont f; f.setPointSize(9); item->setFont(f);
        item->setPos(8, y); y += 18;
    };
    add(QString::fromUtf8("节点总数: %1    树高度: %2    索书号范围: [全部图书]")
        .arg(bst.size()).arg(bst.getHeight(bst.getRoot())));
    add("──");
    bst.inOrder([&](Book* b) {
        if (b) add(QString::fromUtf8("  %1  |  《%2》")
            .arg(QString::fromStdString(b->callNumber),
                 QString::fromStdString(b->title)).left(70));
    });
}

void VisualPanel::refreshHashTable()
{
    hashScene->clear();
    if (!library) return;
    auto& hash = library->getReaderHash();
    int y = 4;
    auto add = [&](const QString& t) {
        auto* item = hashScene->addText(t);
        QFont f; f.setPointSize(9); item->setFont(f);
        item->setPos(8, y); y += 17;
    };
    add(QString::fromUtf8("容量: %1    元素: %2    负载: %3%   算法: djb2 · 链地址法")
        .arg(hash.getCapacity()).arg(hash.size())
        .arg(static_cast<int>(100.0 * hash.size() / hash.getCapacity())));
    add("──");
    hash.traverse([&](const std::string& key, Reader* r) {
        if (r) add(QString::fromUtf8("  %1 → %2")
            .arg(QString::fromStdString(key).left(12),
                 QString::fromStdString(r->name)));
    });
}

void VisualPanel::refreshHeap()
{
    heapScene->clear();
    if (!library) return;
    auto& heap = library->getHotHeap();
    int y = 4;
    auto add = [&](const QString& t) {
        auto* item = heapScene->addText(t);
        QFont f; f.setPointSize(9); item->setFont(f);
        item->setPos(8, y); y += 18;
    };
    Book* top = heap.peekTop();
    add(QString::fromUtf8("大小: %1    堆顶: %2 (借阅 %3 次)")
        .arg(heap.size())
        .arg(top ? QString::fromStdString(top->title) : "无")
        .arg(top ? QString::number(top->borrowCount) : "0"));
    add("──");
    auto& d = heap.getData();
    for (size_t i = 0; i < d.size(); ++i) {
        auto* b = d[i];
        if (b) add(QString::fromUtf8("  [%1] 《%2》  %3次")
            .arg(i).arg(QString::fromStdString(b->title).left(26)).arg(b->borrowCount));
    }
}

void VisualPanel::refreshGraph()
{
    graphScene->clear();
    if (!library) return;
    auto& g = library->getRecommendationGraph();
    int y = 4;
    auto add = [&](const QString& t) {
        auto* item = graphScene->addText(t);
        QFont f; f.setPointSize(9); item->setFont(f);
        item->setPos(8, y); y += 17;
    };
    auto verts = g.getAllVertices();
    int edges = 0;
    for (const auto& v : verts) edges += static_cast<int>(g.getNeighbors(v).size());
    add(QString::fromUtf8("顶点: %1    边: %2    推荐算法: 邻居权重排序")
        .arg(verts.size()).arg(edges / 2));
    add("──");
    for (const auto& v : verts) {
        auto nb = g.getNeighbors(v);
        if (nb.empty()) continue;
        QString line = QString::fromStdString(v).left(17) + " -> ";
        for (const auto& [to, w] : nb)
            line += QString("%1:%2 ").arg(QString::fromStdString(to).left(15)).arg(w);
        add(line.left(110));
    }
}

void VisualPanel::refreshStack()
{
    stackScene->clear();
    if (!library) return;
    auto& stack = library->getUndoStack();
    int y = 4;
    auto add = [&](const QString& t) {
        auto* item = stackScene->addText(t);
        QFont f; f.setPointSize(9); item->setFont(f);
        item->setPos(8, y); y += 18;
    };
    add(QString::fromUtf8("栈大小: %1  (栈顶 = 最近操作)").arg(stack.size()));
    add("──");
    if (stack.isEmpty()) {
        add(QString::fromUtf8("  (空 — 执行借书/还书后显示)"));
    } else {
        stack.traverse([&](const OperationRecord& rec) {
            QString ts = (rec.type == OperationType::BorrowBook) ? QString::fromUtf8("借书")
                       : (rec.type == OperationType::ReturnBook) ? QString::fromUtf8("还书")
                       : QString::fromUtf8("其他");
            add(QString::fromUtf8("  %1  %2  读者: %3  ISBN: %4")
                .arg(rec.timestamp, ts, rec.readerId, rec.bookISBN.left(16)));
        });
    }
}

void VisualPanel::refreshQueue()
{
    queueScene->clear();
    if (!library) return;
    const auto& queues = library->getReservationQueues();
    int y = 4;
    auto add = [&](const QString& t) {
        auto* item = queueScene->addText(t);
        QFont f; f.setPointSize(9); item->setFont(f);
        item->setPos(8, y); y += 18;
    };
    add(QString::fromUtf8("活跃预约队列: %1 本").arg(queues.size()));
    add("──");
    if (queues.empty()) {
        add(QString::fromUtf8("  (空 — 库存为零时读者可排队)"));
    } else {
        for (const auto& [isbn, q] : queues) {
            add(QString::fromUtf8("  ISBN: %1    排队: %2 人")
                .arg(QString::fromStdString(isbn)).arg(q.size()));
            q.traverse([&](const QString& rid) {
                add(QString::fromUtf8("      ← %1").arg(rid));
            });
        }
    }
}

void VisualPanel::refreshMatrix()
{
    matrixScene->clear();
    if (!library) return;
    auto seatGrid = library->getSeatGrid(10, 10);
    const int cs = 52;

    auto* title = matrixScene->addText(
        QString::fromUtf8("座位 10×10  (绿 = 空闲  红 = 已占用  灰 = 维护)"));
    title->setPos(8, 2);

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            const Seat& s = seatGrid[r][c];
            QColor color(144, 238, 144);
            QString tip = QString::fromUtf8("(%1,%2) 空闲").arg(r).arg(c);
            if (s.status == SeatStatus::Occupied) {
                color = QColor(255, 130, 130);
                tip = QString::fromUtf8("(%1,%2) %3\n%4 — %5")
                    .arg(r).arg(c)
                    .arg(QString::fromStdString(s.readerId),
                         QString::fromStdString(s.startTime),
                         QString::fromStdString(s.endTime));
            } else if (s.status == SeatStatus::Maintenance) {
                color = QColor(192, 192, 192);
                tip = QString::fromUtf8("(%1,%2) 维护中").arg(r).arg(c);
            }
            auto* rect = matrixScene->addRect(
                c * cs + 5, r * cs + 22, cs - 2, cs - 2,
                QPen(Qt::darkGray), QBrush(color));
            rect->setToolTip(tip);
            auto* lab = matrixScene->addText(QString("(%1,%2)").arg(r).arg(c));
            lab->setPos(c * cs + 9, r * cs + 36);
            lab->setDefaultTextColor(Qt::black);
        }
    }
}

void VisualPanel::refreshAll()
{
    refreshBST();
    refreshHashTable();
    refreshHeap();
    refreshGraph();
    refreshStack();
    refreshQueue();
    refreshMatrix();
}
