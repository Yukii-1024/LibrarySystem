#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include "core/LibrarySystem.h"

class VisualPanel;
class LoginDialog;

/**
 * 主窗口
 * 左侧：功能Tab页（图书、读者、借阅、座位、推荐）
 * 右侧：数据结构可视化面板
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginSuccess();
    void showAbout();

private:
    void setupUI();
    void setupMenuBar();
    void setupConnections();

    QSplitter* centralSplitter = nullptr;
    QTabWidget* funcTabs = nullptr;
    VisualPanel* visualPanel = nullptr;
    LibrarySystem* library = nullptr;

    bool isAdmin = false;
    QString currentReaderId;
};
