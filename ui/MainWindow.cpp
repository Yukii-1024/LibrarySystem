#include "MainWindow.h"
#include "VisualPanel.h"
#include "LoginDialog.h"
#include "BookTab.h"
#include "ReaderTab.h"
#include "BorrowTab.h"
#include "SeatTab.h"
#include "HotRankTab.h"
#include "RecommendTab.h"
#include "MessageQueueTab.h"
#include "LogPanel.h"
#include "persistence/DataPersistence.h"
#include "core/SeedData.h"
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QStatusBar>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , library(new LibrarySystem(this))
{
    QString dbPath = QCoreApplication::applicationDirPath() + "/library.db";
    if (!DataPersistence::initDatabase(dbPath))
        qWarning() << "Failed to init database";
    else
        qDebug() << "Database ready:" << dbPath;
    loadFromDatabase();

    setWindowTitle(QString::fromUtf8("高校图书馆智能管理系统 - 请登录"));
    resize(1400, 900);

    setupUI();
    setupMenuBar();
    setupConnections();

    LoginDialog dlg(library, this);
    if (dlg.exec() == QDialog::Accepted) {
        isAdmin = dlg.isAdminLogin();
        currentReaderId = dlg.getReaderId();
        loginSuccess = true;
        library->setCurrentOperator(currentReaderId, isAdmin);
        onLoginSuccess();
    } else {
        close();  // 关闭窗口，程序将在 main 中正常退出
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (dirty) {
        QMessageBox::StandardButton btn = QMessageBox::question(this,
            QString::fromUtf8("未保存的修改"),
            QString::fromUtf8("你有未保存的修改，是否在退出前保存？"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Save) {
            DataPersistence::saveAll(library);
            event->accept();
        } else if (btn == QMessageBox::Discard) {
            event->accept();
        } else {
            event->ignore();
            return;
        }
    }
    DataPersistence::close();
    event->accept();
}

void MainWindow::setupUI()
{
    centralSplitter = new QSplitter(Qt::Horizontal, this);

    // ── Left: function tabs ──
    funcTabs = new QTabWidget(centralSplitter);

    bookTab = new BookTab(library);
    readerTab = new ReaderTab(library);
    borrowTab = new BorrowTab(library);
    seatTab = new SeatTab(library);
    hotRankTab = new HotRankTab(library);
    recommendTab = new RecommendTab(library);

    funcTabs->addTab(bookTab,      QString::fromUtf8("\U0001F4DA 图书管理"));
    funcTabs->addTab(readerTab,    QString::fromUtf8("\U0001F464 读者管理"));
    funcTabs->addTab(borrowTab,    QString::fromUtf8("\U0001F4D6 借阅/归还"));
    funcTabs->addTab(recommendTab, QString::fromUtf8("\U0001F517 图书推荐"));
    funcTabs->addTab(hotRankTab,   QString::fromUtf8("\U0001F525 热门排行"));
    funcTabs->addTab(seatTab,      QString::fromUtf8("\U0001F4BA 座位预约"));

    centralSplitter->addWidget(funcTabs);

    // ── Right: optional view panels (hidden by default) ──
    rightSplitter = new QSplitter(Qt::Vertical, centralSplitter);

    visualPanel = new VisualPanel(library, rightSplitter);
    mqPanel = new MessageQueueTab(library, rightSplitter);
    logPanel = new LogPanel(library, rightSplitter);

    rightSplitter->addWidget(visualPanel);
    rightSplitter->addWidget(mqPanel);
    rightSplitter->addWidget(logPanel);

    centralSplitter->addWidget(rightSplitter);
    // Start with all panels hidden — only funcTabs visible
    visualPanel->hide();
    mqPanel->hide();
    logPanel->hide();
    centralSplitter->setSizes({1, 0});
    centralSplitter->setStretchFactor(0, 1);
    centralSplitter->setStretchFactor(1, 0);

    setCentralWidget(centralSplitter);
    statusBar()->showMessage(QString::fromUtf8(
        "就绪 | 视图 → 开启数据结构可视化 / 消息队列 / 操作日志面板"));
}

void MainWindow::setupMenuBar()
{
    // ── 文件 ──
    QMenu* fileMenu = menuBar()->addMenu(QString::fromUtf8("文件"));
    QAction* saveAct = fileMenu->addAction(QString::fromUtf8("\U0001F4BE 保存修改"));
    QAction* discardAct = fileMenu->addAction(QString::fromUtf8("\U0001F504 放弃修改"));
    fileMenu->addSeparator();
    QAction* exitAct = fileMenu->addAction(QString::fromUtf8("❌ 退出"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveChanges);
    connect(discardAct, &QAction::triggered, this, &MainWindow::onDiscardChanges);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // ── 视图 ──
    QMenu* viewMenu = menuBar()->addMenu(QString::fromUtf8("视图"));

    toggleVisualAct = viewMenu->addAction(QString::fromUtf8("\U0001F50D 数据结构可视化"));
    toggleVisualAct->setCheckable(true);
    toggleVisualAct->setChecked(false);
    connect(toggleVisualAct, &QAction::toggled,
            this, &MainWindow::onToggleVisualPanel);

    toggleMQAct = viewMenu->addAction(QString::fromUtf8("\U0001F4E8 消息队列"));
    toggleMQAct->setCheckable(true);
    toggleMQAct->setChecked(false);
    connect(toggleMQAct, &QAction::toggled,
            this, &MainWindow::onToggleMQPanel);

    toggleLogAct = viewMenu->addAction(QString::fromUtf8("\U0001F4DD 操作日志"));
    toggleLogAct->setCheckable(true);
    toggleLogAct->setChecked(false);
    connect(toggleLogAct, &QAction::toggled,
            this, &MainWindow::onToggleLogPanel);

    // ── 帮助 ──
    QMenu* helpMenu = menuBar()->addMenu(QString::fromUtf8("帮助"));
    QAction* aboutAct = helpMenu->addAction(QString::fromUtf8("关于"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupConnections()
{
    connect(library, &LibrarySystem::operationPerformed,
            visualPanel, &VisualPanel::onOperation);
    connect(library, &LibrarySystem::operationPerformed,
            this, &MainWindow::markDirty);
    connect(funcTabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // Read-only tabs → visual panel
    connect(hotRankTab, &HotRankTab::heapRefreshed, this, [this]() {
        visualPanel->onOperation(QString::fromUtf8("刷新热门排行"),
            QStringList() << QString::fromUtf8("堆"),
            QString::fromUtf8("基于最大堆获取借阅次数最多的图书"));
    });
    connect(recommendTab, &RecommendTab::graphRefreshed, this, [this]() {
        visualPanel->onOperation(QString::fromUtf8("图书推荐查询"),
            QStringList() << QString::fromUtf8("图"),
            QString::fromUtf8("基于图的邻接表获取共同借阅推荐"));
    });
    connect(seatTab, &SeatTab::gridRefreshed, this, [this]() {
        visualPanel->onOperation(QString::fromUtf8("刷新座位"),
            QStringList() << QString::fromUtf8("矩阵"),
            QString::fromUtf8("基于稀疏矩阵展示座位分布"));
    });

    // Message queue panel
    connect(mqPanel, &MessageQueueTab::queueRefreshed, this, [this]() {
        visualPanel->onOperation(QString::fromUtf8("消息队列处理"),
            QStringList() << QString::fromUtf8("队列"),
            QString::fromUtf8("FIFO消息队列逐条处理操作请求，保证多读者并发安全"));
    });
}

void MainWindow::onToggleVisualPanel(bool visible)
{
    visualPanel->setVisible(visible);
    updateRightPanelVisibility();
    if (visible) visualPanel->refreshAll();
}

void MainWindow::onToggleMQPanel(bool visible)
{
    mqPanel->setVisible(visible);
    updateRightPanelVisibility();
    if (visible) mqPanel->refreshAll();
}

void MainWindow::onToggleLogPanel(bool visible)
{
    logPanel->setVisible(visible);
    updateRightPanelVisibility();
    if (visible) logPanel->refreshAll();
}

void MainWindow::updateRightPanelVisibility()
{
    bool visVis = visualPanel->isVisible();
    bool mqVis = mqPanel->isVisible();
    bool logVis = logPanel->isVisible();

    if (visVis || mqVis || logVis) {
        int w = centralSplitter->width();
        if (w < 200) w = 1400;
        centralSplitter->setSizes({w * 2 / 3, w / 3});
        centralSplitter->setStretchFactor(0, 2);
        centralSplitter->setStretchFactor(1, 1);

        // Split right side equally among visible panels
        int visibleCount = (visVis ? 1 : 0) + (mqVis ? 1 : 0) + (logVis ? 1 : 0);
        int h = rightSplitter->height();
        if (h < 10) h = 400;
        int each = h / visibleCount;
        QList<int> sizes;
        if (visVis) sizes.append(each);
        if (mqVis)  sizes.append(each);
        if (logVis) sizes.append(each);
        if (!sizes.isEmpty()) rightSplitter->setSizes(sizes);
    } else {
        centralSplitter->setSizes({1, 0});
        centralSplitter->setStretchFactor(0, 1);
        centralSplitter->setStretchFactor(1, 0);
    }
}

void MainWindow::markDirty()
{
    dirty = true;
    statusBar()->showMessage(QString::fromUtf8("⚡ 有未保存的修改"), 0);
}

void MainWindow::loadFromDatabase()
{
    if (DataPersistence::loadAll(library))
        qDebug() << "Data loaded from database";

    // Seed initial data when database is empty
    SeedData::populate(library);
}

void MainWindow::onLoginSuccess()
{
    QString role = isAdmin
        ? QString::fromUtf8("管理员")
        : QString::fromUtf8("读者");
    setWindowTitle(QString::fromUtf8("高校图书馆智能管理系统 [%1] 当前用户: %2")
                       .arg(role, currentReaderId));

    // Apply permissions
    bookTab->setAdminMode(isAdmin);
    seatTab->setCurrentUser(currentReaderId, isAdmin);
    funcTabs->setTabVisible(1, isAdmin);  // 读者管理 tab: only for admin

    refreshAllTabs();
}

void MainWindow::onSaveChanges()
{
    if (DataPersistence::saveAll(library)) {
        dirty = false;
        statusBar()->showMessage(QString::fromUtf8("✓ 已保存到数据库"), 5000);
    } else {
        QMessageBox::warning(this, QString::fromUtf8("保存失败"),
                             QString::fromUtf8("无法写入数据库"));
    }
}

void MainWindow::onDiscardChanges()
{
    if (!dirty) return;

    QMessageBox::StandardButton btn = QMessageBox::question(this,
        QString::fromUtf8("放弃修改"),
        QString::fromUtf8("确定要放弃本次所有修改、重新加载数据库中的数据吗？"),
        QMessageBox::Yes | QMessageBox::No);

    if (btn != QMessageBox::Yes) return;

    loadFromDatabase();
    dirty = false;
    refreshAllTabs();
    visualPanel->refreshAll();
    mqPanel->refreshAll();
    logPanel->refreshAll();
    statusBar()->showMessage(QString::fromUtf8("已从数据库重新加载"), 5000);
}

void MainWindow::onTabChanged(int index)
{
    switch (index) {
        case 0: bookTab->refreshTable(); break;
        case 1: readerTab->refreshTable(); break;
        case 2: borrowTab->refreshTable(); break;
        case 3: recommendTab->graphRefreshed(); break;
        case 4: hotRankTab->refreshRanking(); break;
        case 5: seatTab->refreshGrid(); break;
        default: break;
    }
}

void MainWindow::refreshAllTabs()
{
    bookTab->refreshTable();
    readerTab->refreshTable();
    borrowTab->refreshTable();
    seatTab->refreshGrid();
    hotRankTab->refreshRanking();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, QString::fromUtf8("关于"),
        QString::fromUtf8(
            "<h2>高校图书馆智能管理系统</h2>"
            "<p>数据结构课程大作业</p>"
            "<p>涵盖数据结构：链表、栈、队列、二叉排序树、哈希表、图、堆、稀疏矩阵</p>"
            "<p>技术栈：C++17 / Qt6 / SQLite / CMake</p>"));
}
