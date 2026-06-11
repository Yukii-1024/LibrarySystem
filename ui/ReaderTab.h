#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class LibrarySystem;

class ReaderTab : public QWidget {
    Q_OBJECT
public:
    explicit ReaderTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshTable();

private slots:
    void onAdd();
    void onDelete();
    void onEdit();
    void onSearch();

private:
    void setupUI();
    LibrarySystem* library;
    QLineEdit* idEdit, *pwdEdit, *nameEdit, *deptEdit, *searchEdit;
    QTableWidget* table;
    QLabel* statusLabel;
    QPushButton* addBtn;
    QPushButton* editBtn;
    QString editingId;
};
