#include "VisualPanel.h"
#include "core/LibrarySystem.h"
#include "model/Book.h"
#include "model/Reader.h"
#include "model/Seat.h"
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QLabel>
#include <QTextEdit>
#include <QString>

VisualPanel::VisualPanel(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib)
{
    setupUI();
}

void VisualPanel::setupUI()
{
    auto* layout = new QVBoxLayout(this);

    operationLabel = new QLabel(QString::fromUtf8("当前操作：无"), this);
    operationLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2196F3;");
    layout->addWidget(operationLabel);

    descriptionText = new QTextEdit(this);
    descriptionText->setReadOnly(true);
    descriptionText->setMaximumHeight(80);
    descriptionText->setPlaceholderText(QString::fromUtf8("操作描述将显示在这里..."));
    layout->addWidget(descriptionText);

    dsTabs = new QTabWidget(this);

    // Create scenes and views
    bstScene = new QGraphicsScene(this);
    hashScene = new QGraphicsScene(this);
    heapScene = new QGraphicsScene(this);
    graphScene = new QGraphicsScene(this);
    stackScene = new QGraphicsScene(this);
    queueScene = new QGraphicsScene(this);
    matrixScene = new QGraphicsScene(this);

    bstView = createViewForScene(bstScene);
    hashView = createViewForScene(hashScene);
    heapView = createViewForScene(heapScene);
    graphView = createViewForScene(graphScene);
    stackView = createViewForScene(stackScene);
    queueView = createViewForScene(queueScene);
    matrixView = createViewForScene(matrixScene);

    dsTabs->addTab(bstView, QString::fromUtf8("\U0001F332 BST/AVL树"));
    dsTabs->addTab(hashView, QString::fromUtf8("#️⃣ 哈希表"));
    dsTabs->addTab(heapView, QString::fromUtf8("\U0001F4CA 堆"));
    dsTabs->addTab(graphView, QString::fromUtf8("\U0001F578 图"));
    dsTabs->addTab(stackView, QString::fromUtf8("\U0001F4E5 栈"));
    dsTabs->addTab(queueView, QString::fromUtf8("\U0001F4E4 队列"));
    dsTabs->addTab(matrixView, QString::fromUtf8("\U0001FA91 座位矩阵"));

    layout->addWidget(dsTabs);
    setLayout(layout);

    // Show initial state
    refreshAll();
}

QGraphicsView* VisualPanel::createViewForScene(QGraphicsScene* scene)
{
    auto* view = new QGraphicsView(this);
    view->setRenderHints(QPainter::Antialiasing);
    view->setScene(scene);
    view->setMinimumHeight(200);
    return view;
}

void VisualPanel::onOperation(const QString& opName,
                               const QStringList& dsUsed,
                               const QString& description)
{
    showOperation(opName, dsUsed, description);
    // Auto-refresh affected data structure tabs
    for (const auto& ds : dsUsed) {
        if (ds.contains(QString::fromUtf8("BST")) || ds.contains(QString::fromUtf8("树")))
            { refreshBST(); continue; }
        if (ds.contains(QString::fromUtf8("哈希"))) { refreshHashTable(); }
        if (ds.contains(QString::fromUtf8("堆"))) { refreshHeap(); }
        if (ds.contains(QString::fromUtf8("图"))) { refreshGraph(); }
        if (ds.contains(QString::fromUtf8("栈"))) { refreshStack(); }
        if (ds.contains(QString::fromUtf8("队列"))) { refreshQueue(); }
        if (ds.contains(QString::fromUtf8("矩阵"))) { refreshMatrix(); }
    }
}

void VisualPanel::showOperation(const QString& opName,
                                 const QStringList& dsUsed,
                                 const QString& description)
{
    operationLabel->setText(QString::fromUtf8("当前操作：%1").arg(opName));
    descriptionText->setText(
        QString::fromUtf8("涉及数据结构：%1\n\n%2")
            .arg(dsUsed.join(", "), description));
}

// --- Actual visualization logic ---

void VisualPanel::refreshBST()
{
    bstScene->clear();
    if (!library) return;
    auto& bst = library->getBookBST();
    int y = 10;

    auto addLine = [&](const QString& text) {
        auto* item = bstScene->addText(text);
        item->setPos(10, y);
        y += 20;
    };

    addLine(QString::fromUtf8("BST 节点总数: %1").arg(bst.size()));
    addLine(QString::fromUtf8("树高度: %1").arg(bst.getHeight(bst.getRoot())));
    addLine(QString::fromUtf8("--- 中序遍历 (按索书号排序) ---"));
    bst.inOrder([&](Book* b) {
        if (b) addLine(QString::fromUtf8("  %1 | 《%2》")
            .arg(QString::fromStdString(b->callNumber),
                 QString::fromStdString(b->title)).left(80));
    });
    addLine(QString::fromUtf8("--- 可升级为AVL树 (旋转代码已预留) ---"));
}

void VisualPanel::refreshHashTable()
{
    hashScene->clear();
    if (!library) return;
    auto& hash = library->getReaderHash();
    int y = 10;

    auto addLine = [&](const QString& text) {
        auto* item = hashScene->addText(text);
        item->setPos(10, y);
        y += 18;
    };

    addLine(QString::fromUtf8("哈希表容量: %1  元素数: %2  负载因子: %3%")
        .arg(hash.getCapacity()).arg(hash.size())
        .arg(static_cast<int>(100.0 * hash.size() / hash.getCapacity())));
    addLine(QString::fromUtf8("哈希算法: djb2 (h = h*33 + c)"));
    addLine(QString::fromUtf8("--- 桶分布 (索引: 链长度) ---"));
    QString bucketInfo;
    for (int i = 0; i < hash.getCapacity(); ++i) {
        auto* node = hash.getBucket(i);
        int len = 0;
        while (node) { ++len; node = node->next; }
        if (len > 0) {
            if (!bucketInfo.isEmpty()) bucketInfo += ", ";
            bucketInfo += QString("%1:%2").arg(i).arg(len);
        }
    }
    if (bucketInfo.isEmpty()) bucketInfo = QString::fromUtf8("(空)");
    addLine(bucketInfo);
    addLine(QString::fromUtf8("--- 所有读者 (学号 -> 姓名) ---"));
    hash.traverse([&](const std::string& key, Reader* r) {
        if (r) addLine(QString::fromUtf8("  %1 -> %2")
            .arg(QString::fromStdString(key),
                 QString::fromStdString(r->name)).left(60));
    });
}

void VisualPanel::refreshHeap()
{
    heapScene->clear();
    if (!library) return;
    auto& heap = library->getHotHeap();
    int y = 10;

    auto addLine = [&](const QString& text) {
        auto* item = heapScene->addText(text);
        item->setPos(10, y);
        y += 20;
    };

    addLine(QString::fromUtf8("堆大小: %1").arg(heap.size()));
    Book* top = heap.peekTop();
    addLine(QString::fromUtf8("堆顶 (最热门): %1 (借阅%2次)")
        .arg(top ? QString::fromStdString(top->title) : QString::fromUtf8("无"),
             top ? QString::number(top->borrowCount) : QString("0")));
    addLine(QString::fromUtf8("--- 堆数组 (按完全二叉树排列) ---"));
    auto& data = heap.getData();
    for (size_t i = 0; i < data.size(); ++i) {
        auto* b = data[i];
        if (b) addLine(QString::fromUtf8("  [%1] 《%2》(借阅%3)")
            .arg(i).arg(QString::fromStdString(b->title).left(30))
            .arg(b->borrowCount));
    }
    if (data.empty()) addLine(QString::fromUtf8("  (空)"));
}

void VisualPanel::refreshGraph()
{
    graphScene->clear();
    if (!library) return;
    auto& g = library->getRecommendationGraph();
    int y = 10;

    auto addLine = [&](const QString& text) {
        auto* item = graphScene->addText(text);
        item->setPos(10, y);
        y += 18;
    };

    auto verts = g.getAllVertices();
    int edgeCount = 0;
    for (const auto& v : verts) {
        auto neighbors = g.getNeighbors(v);
        edgeCount += static_cast<int>(neighbors.size());
    }
    addLine(QString::fromUtf8("顶点数 (图书ISBN): %1  边数: %2").arg(verts.size()).arg(edgeCount / 2));
    addLine(QString::fromUtf8("--- 邻接表 (ISBN -> 关联图书:权重) ---"));
    for (const auto& v : verts) {
        auto neighbors = g.getNeighbors(v);
        if (neighbors.empty()) continue;
        QString line = QString::fromStdString(v).left(20) + " -> ";
        for (const auto& [to, w] : neighbors)
            line += QString("%1:%2  ").arg(QString::fromStdString(to).left(18)).arg(w);
        addLine(line.left(120));
    }
    if (verts.empty()) addLine(QString::fromUtf8("  (暂无数据 — 借阅图书后将自动建立关联边)"));
}

void VisualPanel::refreshStack()
{
    stackScene->clear();
    if (!library) return;
    auto& stack = library->getUndoStack();
    int y = 10;

    auto addLine = [&](const QString& text) {
        auto* item = stackScene->addText(text);
        item->setPos(10, y);
        y += 20;
    };

    addLine(QString::fromUtf8("Undo栈大小: %1").arg(stack.size()));
    addLine(QString::fromUtf8("--- 从栈底到栈顶 (最近操作在最下方) ---"));
    if (stack.isEmpty()) {
        addLine(QString::fromUtf8("  (空 — 执行借书/还书操作后将有记录)"));
    } else {
        stack.traverse([&](const OperationRecord& rec) {
            QString typeStr;
            switch (rec.type) {
                case OperationType::BorrowBook: typeStr = QString::fromUtf8("借书"); break;
                case OperationType::ReturnBook: typeStr = QString::fromUtf8("还书"); break;
                default: typeStr = QString::fromUtf8("其他"); break;
            }
            addLine(QString::fromUtf8("  [%1] %2 | 读者:%3 | ISBN:%4")
                .arg(rec.timestamp, typeStr, rec.readerId, rec.bookISBN.left(20)));
        });
    }
}

void VisualPanel::refreshQueue()
{
    queueScene->clear();
    if (!library) return;
    const auto& queues = library->getReservationQueues();
    int y = 10;

    auto addLine = [&](const QString& text) {
        auto* item = queueScene->addText(text);
        item->setPos(10, y);
        y += 20;
    };

    addLine(QString::fromUtf8("活跃预约队列数: %1").arg(queues.size()));
    addLine(QString::fromUtf8("--- 每本书的预约队列 ---"));
    if (queues.empty()) {
        addLine(QString::fromUtf8("  (空 — 当库存为0时读者可加入预约队列)"));
    } else {
        for (const auto& [isbn, q] : queues) {
            addLine(QString::fromUtf8("  ISBN: %1 (排队%2人)")
                .arg(QString::fromStdString(isbn)).arg(q.size()));
            q.traverse([&](const QString& readerId) {
                addLine(QString::fromUtf8("    <- %1").arg(readerId));
            });
        }
    }
}

void VisualPanel::refreshMatrix()
{
    matrixScene->clear();
    if (!library) return;
    auto seatGrid = library->getSeatGrid(10, 10);
    int cellSize = 55;

    // Title
    auto* title = matrixScene->addText(QString::fromUtf8("座位矩阵 (10x10)  绿色=空闲  红色=已占用  灰色=维护中"));
    title->setPos(10, 0);

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            const Seat& s = seatGrid[r][c];
            // Color
            QColor color(150, 255, 150); // green = free
            QString tip = QString::fromUtf8("(%1,%2) 空闲").arg(r).arg(c);
            if (s.status == SeatStatus::Occupied) {
                color = QColor(255, 150, 150);
                tip = QString::fromUtf8("(%1,%2) %3\n%4 - %5")
                    .arg(r).arg(c)
                    .arg(QString::fromStdString(s.readerId),
                         QString::fromStdString(s.startTime),
                         QString::fromStdString(s.endTime));
            } else if (s.status == SeatStatus::Maintenance) {
                color = QColor(200, 200, 200);
                tip = QString::fromUtf8("(%1,%2) 维护中").arg(r).arg(c);
            }
            auto* rect = matrixScene->addRect(c * cellSize + 5, r * cellSize + 25,
                                              cellSize - 2, cellSize - 2,
                                              QPen(Qt::black), QBrush(color));
            rect->setToolTip(tip);
            auto* label = matrixScene->addText(QString("(%1,%2)").arg(r).arg(c));
            label->setPos(c * cellSize + 8, r * cellSize + 38);
            label->setDefaultTextColor(Qt::black);
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

// All refresh methods are implemented directly above
