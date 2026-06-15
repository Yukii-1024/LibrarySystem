#pragma once
#include <QWidget>
#include <QSplitter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMap>
#include <QScrollArea>

class LibrarySystem;

class VisualPanel : public QWidget {
    Q_OBJECT

public:
    explicit VisualPanel(LibrarySystem* lib, QWidget* parent = nullptr);

    void showOperation(const QString& opName,
                       const QStringList& dsUsed,
                       const QString& description);
    void refreshAll();

    void refreshBST();
    void refreshHashTable();
    void refreshHeap();
    void refreshGraph();
    void refreshStack();
    void refreshQueue();
    void refreshMatrix();

public slots:
    void onOperation(const QString& opName, const QStringList& dsUsed, const QString& description);

private:
    void setupUI();
    QGraphicsView* createViewForScene(QGraphicsScene* scene);
    QWidget* createDSPane(const QString& title, QGraphicsView* view);
    void showDSPanes(const QStringList& dsUsed);

    LibrarySystem* library = nullptr;
    QLabel* operationLabel = nullptr;
    QTextEdit* descriptionText = nullptr;
    QSplitter* dsSplitter = nullptr;
    QWidget* welcomePane = nullptr;
    QMap<QString, QWidget*> dsPanes;   // key -> pane widget

    // Scenes and views (created once)
    QGraphicsScene* bstScene = nullptr;
    QGraphicsScene* hashScene = nullptr;
    QGraphicsScene* heapScene = nullptr;
    QGraphicsScene* graphScene = nullptr;
    QGraphicsScene* stackScene = nullptr;
    QGraphicsScene* queueScene = nullptr;
    QGraphicsScene* matrixScene = nullptr;

    QGraphicsView* bstView = nullptr;
    QGraphicsView* hashView = nullptr;
    QGraphicsView* heapView = nullptr;
    QGraphicsView* graphView = nullptr;
    QGraphicsView* stackView = nullptr;
    QGraphicsView* queueView = nullptr;
    QGraphicsView* matrixView = nullptr;
};
