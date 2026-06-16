#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QAction>
#include "core/LibrarySystem.h"

class VisualPanel;
class LoginDialog;
class BookTab;
class ReaderTab;
class BorrowTab;
class SeatTab;
class HotRankTab;
class RecommendTab;
class MessageQueueTab;
class LogPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool isLoginSuccess() const { return loginSuccess; }

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onLoginSuccess();
    void showAbout();
    void onSaveChanges();
    void onDiscardChanges();
    void onTabChanged(int index);
    void markDirty();
    void onToggleVisualPanel(bool visible);
    void onToggleMQPanel(bool visible);
    void onToggleLogPanel(bool visible);

private:
    void setupUI();
    void setupMenuBar();
    void setupConnections();
    void loadFromDatabase();
    void refreshAllTabs();
    void updateRightPanelVisibility();

    QSplitter* centralSplitter = nullptr;
    QSplitter* rightSplitter = nullptr;
    QTabWidget* funcTabs = nullptr;
    VisualPanel* visualPanel = nullptr;
    MessageQueueTab* mqPanel = nullptr;
    LogPanel* logPanel = nullptr;
    LibrarySystem* library = nullptr;

    BookTab* bookTab = nullptr;
    ReaderTab* readerTab = nullptr;
    BorrowTab* borrowTab = nullptr;
    SeatTab* seatTab = nullptr;
    HotRankTab* hotRankTab = nullptr;
    RecommendTab* recommendTab = nullptr;

    QAction* toggleVisualAct = nullptr;
    QAction* toggleMQAct = nullptr;
    QAction* toggleLogAct = nullptr;

    bool isAdmin = false;
    QString currentReaderId;
    bool dirty = false;
    bool loginSuccess = false;
};
