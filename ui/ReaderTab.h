#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>

class LibrarySystem;

class ReaderTab : public QWidget {
    Q_OBJECT
public:
    explicit ReaderTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshTable();

private slots:
    void onAdd();
    void onDelete();
    void onSearch();

private:
    void setupUI();
    LibrarySystem* library;
    QLineEdit* idEdit, *pwdEdit, *nameEdit, *deptEdit, *searchEdit;
    QTableWidget* table;
    QLabel* statusLabel;
};
