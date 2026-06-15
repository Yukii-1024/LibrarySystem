#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include "core/LibrarySystem.h"

class VisualPanel;
class LoginDialog;
class BookTab;
class ReaderTab;
class BorrowTab;
class SeatTab;
class HotRankTab;
class RecommendTab;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onLoginSuccess();
    void showAbout();
    void onSaveChanges();
    void onDiscardChanges();
    void onTabChanged(int index);
    void markDirty();

private:
    void setupUI();
    void setupMenuBar();
    void setupConnections();
    void loadFromDatabase();
    void refreshAllTabs();

    QSplitter* centralSplitter = nullptr;
    QTabWidget* funcTabs = nullptr;
    VisualPanel* visualPanel = nullptr;
    LibrarySystem* library = nullptr;

    BookTab* bookTab = nullptr;
    ReaderTab* readerTab = nullptr;
    BorrowTab* borrowTab = nullptr;
    SeatTab* seatTab = nullptr;
    HotRankTab* hotRankTab = nullptr;
    RecommendTab* recommendTab = nullptr;

    bool isAdmin = false;
    QString currentReaderId;
    bool dirty = false;
};
