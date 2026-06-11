#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>

class LibrarySystem;

class BorrowTab : public QWidget {
    Q_OBJECT
public:
    explicit BorrowTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshTable();

private slots:
    void onBorrow();
    void onReturn();
    void onUndo();
    void onReserve();
    void checkStatus();

private:
    void setupUI();
    LibrarySystem* library;
    QLineEdit* readerEdit, *isbnEdit, *resIsbnEdit;
    QTableWidget* table;
    QLabel* statusLabel;
};
