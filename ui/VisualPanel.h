#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

class LibrarySystem;

class VisualPanel : public QWidget {
    Q_OBJECT

public:
    explicit VisualPanel(LibrarySystem* lib, QWidget* parent = nullptr);

    void showOperation(const QString& opName,
                       const QStringList& dsUsed,
                       const QString& description);

    void refreshBST();
    void refreshHashTable();
    void refreshHeap();
    void refreshGraph();
    void refreshStack();
    void refreshQueue();
    void refreshMatrix();

private:
    void setupUI();
    QGraphicsView* createGraphView();

    LibrarySystem* library = nullptr;
    QLabel* operationLabel = nullptr;
    QTextEdit* descriptionText = nullptr;
    QTabWidget* dsTabs = nullptr;

    QGraphicsScene* bstScene = nullptr;
    QGraphicsScene* hashScene = nullptr;
    QGraphicsScene* heapScene = nullptr;
    QGraphicsScene* graphScene = nullptr;
    QGraphicsScene* stackScene = nullptr;
    QGraphicsScene* queueScene = nullptr;
    QGraphicsScene* matrixScene = nullptr;
};
