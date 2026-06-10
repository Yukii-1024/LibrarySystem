#include "MainWindow.h"
#include "VisualPanel.h"
#include "LoginDialog.h"
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QStatusBar>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , library(new LibrarySystem(this))
{
    setWindowTitle(QString::fromUtf8("高校图书馆智能管理系统 - 请登录"));
    resize(1400, 900);

    setupUI();
    setupMenuBar();
    setupConnections();

    LoginDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        isAdmin = dlg.isAdminLogin();
        currentReaderId = dlg.getReaderId();
        onLoginSuccess();
    } else {
        QApplication::quit();
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    centralSplitter = new QSplitter(Qt::Horizontal, this);

    funcTabs = new QTabWidget(centralSplitter);
    funcTabs->addTab(new QWidget(), QString::fromUtf8("\U0001F4DA 图书管理"));
    funcTabs->addTab(new QWidget(), QString::fromUtf8("\U0001F464 读者管理"));
    funcTabs->addTab(new QWidget(), QString::fromUtf8("\U0001F4D6 借阅/归还"));
    funcTabs->addTab(new QWidget(), QString::fromUtf8("\U0001F4BA 座位预约"));
    funcTabs->addTab(new QWidget(), QString::fromUtf8("\U0001F525 热门排行"));
    funcTabs->addTab(new QWidget(), QString::fromUtf8("\U0001F517 图书推荐"));

    visualPanel = new VisualPanel(library, centralSplitter);

    centralSplitter->addWidget(funcTabs);
    centralSplitter->addWidget(visualPanel);
    centralSplitter->setSizes({900, 500});
    centralSplitter->setStretchFactor(0, 3);
    centralSplitter->setStretchFactor(1, 2);

    setCentralWidget(centralSplitter);
    statusBar()->showMessage(QString::fromUtf8("就绪 | 左侧操作，右侧查看数据结构实时状态"));
}

void MainWindow::setupMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu(QString::fromUtf8("文件"));
    QAction* saveAct = fileMenu->addAction(QString::fromUtf8("\U0001F4BE 保存数据"));
    QAction* loadAct = fileMenu->addAction(QString::fromUtf8("\U0001F4C2 加载数据"));
    fileMenu->addSeparator();
    QAction* exitAct = fileMenu->addAction(QString::fromUtf8("❌ 退出"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QMenu* viewMenu = menuBar()->addMenu(QString::fromUtf8("视图"));
    QAction* toggleVisualAct = viewMenu->addAction(QString::fromUtf8("\U0001F50D 切换可视化面板"));
    toggleVisualAct->setCheckable(true);
    toggleVisualAct->setChecked(true);
    connect(toggleVisualAct, &QAction::toggled, visualPanel, &QWidget::setVisible);

    QMenu* helpMenu = menuBar()->addMenu(QString::fromUtf8("帮助"));
    QAction* aboutAct = helpMenu->addAction(QString::fromUtf8("关于"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupConnections()
{
    // 后续将 library 的操作信号连接到 visualPanel
    // connect(library, &LibrarySystem::operationPerformed,
    //         visualPanel, &VisualPanel::onOperation);
}

void MainWindow::onLoginSuccess()
{
    QString role = isAdmin
        ? QString::fromUtf8("管理员")
        : QString::fromUtf8("读者");
    setWindowTitle(QString::fromUtf8("高校图书馆智能管理系统 [%1] 当前用户: %2")
                       .arg(role, currentReaderId));
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, QString::fromUtf8("关于"),
        QString::fromUtf8(
            "<h2>高校图书馆智能管理系统</h2>"
            "<p>数据结构课程大作业</p>"
            "<p>涵盖数据结构：链表、栈、队列、二叉排序树、哈希表、图、堆、稀疏矩阵</p>"
            "<p>技术栈：C++17 / Qt6 / CMake</p>"));
}
