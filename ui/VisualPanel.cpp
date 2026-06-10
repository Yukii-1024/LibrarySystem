#include "VisualPanel.h"
#include "core/LibrarySystem.h"
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QLabel>
#include <QTextEdit>

VisualPanel::VisualPanel(LibrarySystem* lib, QWidget* parent)
    : QWidget(parent), library(lib)
{
    setupUI();
}

void VisualPanel::setupUI()
{
    auto* layout = new QVBoxLayout(this);

    // 顶部操作提示
    operationLabel = new QLabel(QString::fromUtf8("当前操作：无"), this);
    operationLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2196F3;");
    layout->addWidget(operationLabel);

    // 描述文本
    descriptionText = new QTextEdit(this);
    descriptionText->setReadOnly(true);
    descriptionText->setMaximumHeight(80);
    descriptionText->setPlaceholderText(QString::fromUtf8("操作描述将显示在这里..."));
    layout->addWidget(descriptionText);

    // Tab 切换各数据结构
    dsTabs = new QTabWidget(this);

    bstScene = new QGraphicsScene(this);
    hashScene = new QGraphicsScene(this);
    heapScene = new QGraphicsScene(this);
    graphScene = new QGraphicsScene(this);
    stackScene = new QGraphicsScene(this);
    queueScene = new QGraphicsScene(this);
    matrixScene = new QGraphicsScene(this);

    dsTabs->addTab(createGraphView(), QString::fromUtf8("\U0001F332 BST/AVL树"));
    dsTabs->addTab(createGraphView(), QString::fromUtf8("#️⃣ 哈希表"));
    dsTabs->addTab(createGraphView(), QString::fromUtf8("\U0001F4CA 堆"));
    dsTabs->addTab(createGraphView(), QString::fromUtf8("\U0001F578️ 图"));
    dsTabs->addTab(createGraphView(), QString::fromUtf8("\U0001F4E5 栈"));
    dsTabs->addTab(createGraphView(), QString::fromUtf8("\U0001F4E4 队列"));
    dsTabs->addTab(createGraphView(), QString::fromUtf8("\U0001FA91 座位矩阵"));

    layout->addWidget(dsTabs);
    setLayout(layout);

    // 初始占位文字
    for (QGraphicsScene* scene : {bstScene, hashScene, heapScene, graphScene, stackScene, queueScene, matrixScene}) {
        scene->addText(QString::fromUtf8("暂无数据\n请执行相关操作后查看可视化"));
    }
}

QGraphicsView* VisualPanel::createGraphView()
{
    auto* view = new QGraphicsView(this);
    view->setRenderHints(QPainter::Antialiasing);
    view->setScene(matrixScene);  // 默认占位，后续根据Tab切换
    return view;
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

void VisualPanel::refreshBST()
{
    if (!library) return;
    bstScene->clear();
    bstScene->addText(QString::fromUtf8("BST 可视化待实现"));
}

void VisualPanel::refreshHashTable()
{
    if (!library) return;
    hashScene->clear();
    hashScene->addText(QString::fromUtf8("哈希表可视化待实现"));
}

void VisualPanel::refreshHeap()
{
    if (!library) return;
    heapScene->clear();
    heapScene->addText(QString::fromUtf8("堆可视化待实现"));
}

void VisualPanel::refreshGraph()
{
    if (!library) return;
    graphScene->clear();
    graphScene->addText(QString::fromUtf8("图可视化待实现"));
}

void VisualPanel::refreshStack()
{
    if (!library) return;
    stackScene->clear();
    stackScene->addText(QString::fromUtf8("栈可视化待实现"));
}

void VisualPanel::refreshQueue()
{
    if (!library) return;
    queueScene->clear();
    queueScene->addText(QString::fromUtf8("队列可视化待实现"));
}

void VisualPanel::refreshMatrix()
{
    if (!library) return;
    matrixScene->clear();
    matrixScene->addText(QString::fromUtf8("座位矩阵可视化待实现"));
}
